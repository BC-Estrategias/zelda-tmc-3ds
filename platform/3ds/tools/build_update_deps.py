#!/usr/bin/env python3
"""Build 3DS updater dependencies against the project's own mbedTLS ABI."""
from pathlib import Path
import hashlib
import json
import os
import subprocess
import sys
import tempfile
import urllib.request

root = Path(sys.argv[1]).resolve()
source = root / 'src'
source.mkdir(parents=True, exist_ok=True)
prefix = root / 'prefix'
repo_root = Path(__file__).resolve().parents[3]
recipe = Path(__file__).resolve().parents[1] / 'update-dependencies'
devkit = Path(os.environ.get('DEVKITPRO', '/opt/devkitpro'))
jobs = os.environ.get('TMC3DS_JOBS', '4')
project_mbedtls = repo_root / 'third_party' / 'mbedtls'
project_mbedtls_config = repo_root / 'platform' / '3ds' / 'source'


def run(args, cwd=source, env=None):
    subprocess.run([str(x) for x in args], cwd=cwd, env=env, check=True)


if not (project_mbedtls / 'CMakeLists.txt').exists():
    raise SystemExit(
        'third_party/mbedtls is missing. Run: git submodule update --init third_party/mbedtls'
    )

records = {record['file']: record for record in json.loads((recipe / 'sources.json').read_text())}
for name in ('curl.tar.xz', 'jansson.tar.gz'):
    record = records[name]
    archive = source / name
    if not archive.exists():
        with urllib.request.urlopen(record['url'], timeout=60) as response:
            archive.write_bytes(response.read())
    if hashlib.sha256(archive.read_bytes()).hexdigest() != record['sha256']:
        raise SystemExit('Dependency checksum mismatch: ' + name)
    folder = {'curl.tar.xz': 'curl-8.4.0', 'jansson.tar.gz': 'jansson-2.14'}[name]
    if not (source / folder).exists():
        run(['tar', 'xf', archive])
        if folder.startswith('curl'):
            run(['patch', '-p1', '-i', recipe / 'curl.patch'], source / folder)

            # curl 8.4 configures mbedTLS RNG after mbedtls_ssl_setup().
            # That worked with older mbedTLS, but mbedTLS 3.6 validates more of
            # the config during ssl_setup and can reject it as incomplete.
            # Move ssl_setup after conf_rng, matching modern curl's backend.
            mbedtls_c = source / folder / 'lib' / 'vtls' / 'mbedtls.c'
            text = mbedtls_c.read_text()
            old_setup = '''  mbedtls_ssl_init(&backend->ssl);
  if(mbedtls_ssl_setup(&backend->ssl, &backend->config)) {
    failf(data, "mbedTLS: ssl_init failed");
    return CURLE_SSL_CONNECT_ERROR;
  }

'''
            if old_setup not in text:
                raise SystemExit('curl mbedTLS ssl_setup block not found')
            text = text.replace(old_setup, '  mbedtls_ssl_init(&backend->ssl);\\n\\n', 1)
            old_rng = '''  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &backend->ctr_drbg);
  mbedtls_ssl_set_bio(&backend->ssl, cf,
'''
            new_rng = '''  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &backend->ctr_drbg);

  ret = mbedtls_ssl_setup(&backend->ssl, &backend->config);
  if(ret) {
    mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed (-0x%04X) %s", -ret, errorbuf);
    return CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
'''
            if old_rng not in text:
                raise SystemExit('curl mbedTLS RNG block not found')
            text = text.replace(old_rng, new_rng, 1)
            mbedtls_c.write_text(text)

# Build and install the exact mbedTLS submodule used by RetroAchievements.
# libcurl is compiled against these headers/libs, but the final game links the
# in-tree CMake mbedTLS targets, avoiding a second TLS ABI in the executable.
mbed_build = root / 'mbedtls-project-build'
run([
    'cmake', '-S', project_mbedtls, '-B', mbed_build,
    '-DCMAKE_TOOLCHAIN_FILE=' + str(devkit / 'cmake/3DS.cmake'),
    '-DCMAKE_BUILD_TYPE=Release',
    '-DENABLE_PROGRAMS=OFF',
    '-DENABLE_TESTING=OFF',
    '-DMBEDTLS_FATAL_WARNINGS=OFF',
    '-DDISABLE_PACKAGE_CONFIG_AND_INSTALL=OFF',
    '-DMBEDTLS_USER_CONFIG_FILE=' + str(project_mbedtls_config / 'mbedtls_3ds_config.h'),
    '-DCMAKE_INSTALL_PREFIX=' + str(prefix),
])
run(['cmake', '--build', mbed_build, '--parallel', jobs])
run(['cmake', '--install', mbed_build])

# Jansson is the only additional library linked directly by the updater.
jansson_build = root / 'jansson-2.14-build'
run([
    'cmake', '-S', source / 'jansson-2.14', '-B', jansson_build,
    '-DCMAKE_TOOLCHAIN_FILE=' + str(devkit / 'cmake/3DS.cmake'),
    '-DCMAKE_BUILD_TYPE=Release',
    '-DCMAKE_POLICY_VERSION_MINIMUM=3.5',
    '-DCMAKE_INSTALL_PREFIX=' + str(prefix),
    '-DJANSSON_BUILD_DOCS=OFF',
    '-DJANSSON_BUILD_SHARED_LIBS=OFF',
    '-DJANSSON_WITHOUT_TESTS=ON',
])
run(['cmake', '--build', jansson_build, '--parallel', jobs])
run(['cmake', '--install', jansson_build])

# Curl's configure performs a link probe. The real game supplies these two
# mbedTLS ALT hooks from port_retroachievements_3ds.c; this tiny probe-only
# archive satisfies configure without being linked into the final game.
stub_c = root / 'mbedtls-3ds-probe.c'
stub_o = root / 'mbedtls-3ds-probe.o'
stub_a = prefix / 'lib' / 'libtmc_mbedtls_3ds_probe.a'
stub_c.write_text(r'''#include <3ds.h>
#include <mbedtls/entropy.h>
#include <mbedtls/platform_time.h>
mbedtls_ms_time_t mbedtls_ms_time(void) { return (mbedtls_ms_time_t)osGetTime(); }
int mbedtls_hardware_poll(void *p, unsigned char *out, size_t len, size_t *olen) {
    (void)p;
    sslcGenerateRandomData(out, len);
    if (olen) *olen = len;
    return 0;
}
''')
common_flags = [
    '-march=armv6k', '-mtune=mpcore', '-mfloat-abi=hard', '-mtp=soft',
    '-O2', '-mword-relocations', '-ffunction-sections', '-fdata-sections'
]
run([
    devkit / 'devkitARM/bin/arm-none-eabi-gcc', *common_flags,
    '-D_3DS', '-D__3DS__',
    '-I' + str(prefix / 'include'),
    '-I' + str(devkit / 'libctru/include'),
    '-c', stub_c, '-o', stub_o,
])
run([devkit / 'devkitARM/bin/arm-none-eabi-ar', 'rcs', stub_a, stub_o])

# Autoconf/Make need a space-free path for their library flags.
with tempfile.TemporaryDirectory(prefix='tmc-update-') as temp:
    alias = Path(temp) / 'deps'
    alias.symlink_to(root, target_is_directory=True)
    cp = str(alias / 'prefix')
    env = dict(os.environ)
    env.update(
        PATH=str(devkit / 'devkitARM/bin') + ':' + env['PATH'],
        CC='arm-none-eabi-gcc',
        AR='arm-none-eabi-ar',
        RANLIB='arm-none-eabi-ranlib',
        CFLAGS=' '.join(common_flags),
        CPPFLAGS='-D_3DS -D__3DS__ -I' + cp + '/include -I' + str(devkit / 'libctru/include'),
        LDFLAGS='-L' + cp + '/lib -L' + str(devkit / 'libctru/lib') + ' -specs=3dsx.specs',
        LIBS='-ltmc_mbedtls_3ds_probe -lctru',
    )
    curl = alias / 'src/curl-8.4.0'
    run([
        './configure', '--prefix=' + cp, '--host=arm-none-eabi',
        '--disable-shared', '--enable-static',
        '--disable-ipv6', '--disable-unix-sockets', '--disable-threaded-resolver',
        '--disable-manual', '--disable-pthreads', '--disable-socketpair',
        '--disable-ntlm-wb', '--with-mbedtls=' + cp,
        '--without-zlib', '--without-brotli', '--without-zstd',
        '--without-libpsl', '--without-libidn2', '--without-nghttp2',
        '--without-libssh2', '--disable-ldap', '--disable-ldaps',
        '--with-ca-bundle=romfs:/update-ca.pem'
    ], curl, env)
    run(['make', '-C', 'lib', '-j' + jobs], curl, env)
    run(['make', '-C', 'lib', 'install'], curl, env)
    run(['make', '-C', 'include', 'install'], curl, env)

print('Updater libraries ready (project mbedTLS ABI): ' + str(prefix))
