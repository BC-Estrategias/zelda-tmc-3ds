#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="$(tr -d '\r\n' < "${ROOT}/platform/3ds/version.txt")"
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
  -DCMAKE_BUILD_TYPE=Release
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
  -elf "${BUILD}/tmc-3ds.elf" -icon "${BUILD}/tmc-3ds.icn" \
  -banner "${BUILD}/tmc-3ds.bnr"
)

printf 'Ready:\n  %s\n  %s\n' \
  "${BUILD}/tmc-3ds-v${VERSION}.3dsx" "${BUILD}/tmc-3ds-v${VERSION}.cia"
