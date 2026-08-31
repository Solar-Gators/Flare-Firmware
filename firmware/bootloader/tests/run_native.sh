#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INC="-I${ROOT}/include"
CC="${CC:-gcc}"
CFLAGS="-std=c11 -Wall -Wextra -Werror -O1 ${INC}"
OUT="${1:-/tmp}"
mkdir -p "${OUT}"

${CC} ${CFLAGS} \
  "${ROOT}/tests/test_bl_slots.c" \
  "${ROOT}/tests/fake_flash.c" \
  "${ROOT}/src/core/bl_crc32.c" \
  "${ROOT}/src/core/bl_slots.c" \
  -o "${OUT}/test_bl_slots"
"${OUT}/test_bl_slots"

${CC} ${CFLAGS} \
  "${ROOT}/tests/test_bl_download.c" \
  "${ROOT}/tests/fake_flash.c" \
  "${ROOT}/src/core/bl_crc32.c" \
  "${ROOT}/src/core/bl_slots.c" \
  "${ROOT}/src/core/bl_protocol.c" \
  -o "${OUT}/test_bl_download"
"${OUT}/test_bl_download"
