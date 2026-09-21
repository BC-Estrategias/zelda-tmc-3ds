#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="$(tr -d '\r\n' < "${ROOT}/platform/3ds/version.txt")"

# Map the human version to a monotonic CIA/TMD title version.
# Stable x.y releases use minor=y*16, micro=0. Pre-releases x.y-E<n>
# consume the 255 title-version steps between stable x.(y-1) and x.y.
# Example: 1.2-E5 -> CIA 1.16.5; stable 1.2 -> CIA 1.32.0.
CIA_MAJOR=0
CIA_MINOR=0
CIA_MICRO=0
if [[ "${VERSION}" =~ ^([0-9]+)\.([0-9]+)-E([0-9]+)$ ]]; then
  HUMAN_MAJOR="${BASH_REMATCH[1]}"
  HUMAN_MINOR="${BASH_REMATCH[2]}"
  EXPERIMENT="${BASH_REMATCH[3]}"
  if (( HUMAN_MINOR < 1 || EXPERIMENT < 1 || EXPERIMENT > 255 )); then
    echo "Unsupported prerelease version for CIA title version: ${VERSION}" >&2
    exit 1
  fi
  CIA_MAJOR="${HUMAN_MAJOR}"
  CIA_MINOR=$(( (HUMAN_MINOR - 1) * 16 + EXPERIMENT / 16 ))
  CIA_MICRO=$(( EXPERIMENT % 16 ))
elif [[ "${VERSION}" =~ ^([0-9]+)\.([0-9]+)$ ]]; then
  HUMAN_MAJOR="${BASH_REMATCH[1]}"
  HUMAN_MINOR="${BASH_REMATCH[2]}"
  CIA_MAJOR="${HUMAN_MAJOR}"
  CIA_MINOR=$(( HUMAN_MINOR * 16 ))
  CIA_MICRO=0
else
  echo "Unsupported 3DS version format for CIA title version: ${VERSION}" >&2
  exit 1
fi
if (( CIA_MAJOR > 63 || CIA_MINOR > 63 || CIA_MICRO > 15 )); then
  echo "CIA title version out of makerom range: ${CIA_MAJOR}.${CIA_MINOR}.${CIA_MICRO}" >&2
  exit 1
fi
DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"
BUILD="${ROOT}/build-3ds/game"
TOOLS_ROOT="${TMC3DS_TOOLS_ROOT:-${ROOT}/../Tools/bin}"
MAKEROM="${MAKEROM:-${TOOLS_ROOT}/makerom}"
BANNERTOOL="${BANNERTOOL:-${TOOLS_ROOT}/bannertool}"

if [[ ! -x "${MAKEROM}" ]] && command -v makerom >/dev/null 2>&1; then
  MAKEROM="$(command -v makerom)"
fi
if [[ ! -x "${BANNERTOOL}" ]] && command -v bannertool >/dev/null 2>&1; then
  BANNERTOOL="$(command -v bannertool)"
fi
if [[ ! -x "${MAKEROM}" && -x "${DEVKITPRO}/tools/bin/makerom" ]]; then
  MAKEROM="${DEVKITPRO}/tools/bin/makerom"
fi
if [[ ! -x "${BANNERTOOL}" && -x "${DEVKITPRO}/tools/bin/bannertool" ]]; then
  BANNERTOOL="${DEVKITPRO}/tools/bin/bannertool"
fi

export DEVKITPRO

if [[ -z "${UPDATE_DEPS_ROOT:-}" ]]; then
  # This fork links RetroAchievements against its in-tree mbedTLS 3.x.
  # Always build curl against that exact ABI instead of using a potentially
  # incompatible devkitPro portlibs curl built against mbedTLS 2.x.
  UPDATE_DEPS_ROOT="${ROOT}/build-3ds/update-deps/prefix"
  if [[ ! -f "${UPDATE_DEPS_ROOT}/lib/libcurl.a" || ! -f "${UPDATE_DEPS_ROOT}/lib/libjansson.a" ]]; then
    python3 "${ROOT}/platform/3ds/tools/build_update_deps.py" "${ROOT}/build-3ds/update-deps"
  fi
fi

# Keep the small 3DS ABI correction for the pinned rcheevos source reproducible
# without requiring a fork of the upstream dependency. Apply the one-line
# signature correction directly so CI does not depend on patch-file parsing.
RCHEEVOS_SOURCE="${ROOT}/third_party/rcheevos/src/rcheevos/rc_validate.c"
python3 - "${RCHEEVOS_SOURCE}" <<'PY'
from pathlib import Path
import sys

path = Path(sys.argv[1])
text = path.read_text()
old = "static const rc_operand_t* rc_validate_get_comparison(const rc_condition_t* condition, int* comparison, unsigned* value)"
new = "static const rc_operand_t* rc_validate_get_comparison(const rc_condition_t* condition, int* comparison, uint32_t* value)"

if new not in text:
    if old not in text:
        raise SystemExit("rcheevos ABI signature not found")
    path.write_text(text.replace(old, new, 1))
PY

cmake -S "${ROOT}/platform/3ds" -B "${BUILD}" \
  -DCMAKE_TOOLCHAIN_FILE="${DEVKITPRO}/cmake/3DS.cmake" \
  -DCMAKE_BUILD_TYPE=Release \
  -DUPDATE_DEPS_ROOT="${UPDATE_DEPS_ROOT:-${DEVKITPRO}/portlibs/3ds}"
cmake --build "${BUILD}" --parallel "${TMC3DS_JOBS:-4}"

if [[ ! -x "${MAKEROM}" || ! -x "${BANNERTOOL}" ]]; then
  printf '3DSX ready; makerom/bannertool are unavailable for CIA packaging.\n'
  exit 0
fi

"${BANNERTOOL}" makesmdh \
  -s "The Minish Cap 3DS v${VERSION}" \
  -l "The Minish Cap 3DS v${VERSION}" \
  -p "Bruno Cruz" \
  -i "${ROOT}/platform/3ds/assets/icon-48.png" \
  -f visible,allow3d,extendedbanner,nosavebackups \
  -o "${BUILD}/tmc-3ds.icn"

"${BANNERTOOL}" makebanner \
  -ci "${ROOT}/platform/3ds/assets/banner.cgfx" \
  -a "${ROOT}/platform/3ds/assets/banner.wav" \
  -o "${BUILD}/tmc-3ds.bnr"

(
cd "${ROOT}"
"${MAKEROM}" -f cia -o "${BUILD}/tmc-3ds-v${VERSION}.cia" \
  -DAPP_ROMFS="${BUILD#"${ROOT}/"}/romfs" \
  -rsf "${ROOT}/platform/3ds/cia/tmc3ds.rsf" -target t -exefslogo \
  -major "${CIA_MAJOR}" -minor "${CIA_MINOR}" -micro "${CIA_MICRO}" \
  -elf "${BUILD}/tmc-3ds.elf" -icon "${BUILD}/tmc-3ds.icn" \
  -banner "${BUILD}/tmc-3ds.bnr"
)

printf 'Ready:\n  %s\n  %s\n' \
  "${BUILD}/tmc-3ds-v${VERSION}.3dsx" "${BUILD}/tmc-3ds-v${VERSION}.cia"
