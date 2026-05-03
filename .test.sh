#!/bin/sh
# Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
# SPDX-License-Identifier: MIT-0
# scspell-id: c3b38310-44f4-11f1-89aa-80ee73e9b8e7
# vim: set ft=sh ts=2 sw=2 tw=0 ai expandtab cc=80 :
# shellcheck disable=SC2129

if [ -n "${ZSH_VERSION-}" ]; then
  emulate sh
  setopt sh_word_split
fi

# shellcheck disable=SC2065
test -f "./${0##*/}" > /dev/null 2>&1 || {
  printf '%s\n' "ERROR: Cannot locate script in current directory."
  exit 1
}

test -d "/usr/pkg/gnu/bin" && {
  export PATH="${PATH:-}:/usr/pkg/gnu/bin"
}

set -eu

PROG="./crc_test"
TEST_FILE="./.test_data.bin"
ZERO_FILE="./.test_zero.bin"
SEVEN_FILE="./.test_seven.txt"
OUT_FILE="./.test_results.log"
REF_FILE="./.ref_results.log"
SRC_FILE="crc.c"

CC="$(command -v cc 2> /dev/null || command -v gcc 2> /dev/null \
  || command -v clang 2> /dev/null || printf '%s\n' cc)"

"${CC:?}" -O3 "${SRC_FILE:?}" -o crc_test

export CC

rm -f "nonexistent" > /dev/null 2>&1 || :

printf '\252\125\377' > "${TEST_FILE:?}"
printf '\0\0\0' > "${ZERO_FILE:?}"
printf 'Hello' > "${SEVEN_FILE:?}"

run_test()
{
  label="${1:?}"
  shift
  printf '%s\n' "TEST: ${label:?}" >> "${OUT_FILE:?}"
  printf '%s\n\n' "ARGS: $*" | sed 's/[[:space:]]*$//' >> "${OUT_FILE}"
  set +e
  env "${PROG:?}" "$@" >> "${OUT_FILE:?}" 2>&1
  printf '\n%s\n' "EXIT: $?" >> "${OUT_FILE:?}"
  set -e
  printf '\n%s' "----------------------------------------" >> "${OUT_FILE:?}"
  printf '%s\n\n' "--------------------------------------" >> "${OUT_FILE:?}"
}

# /* REUSE-IgnoreStart */
printf '%s' "" > "${OUT_FILE:?}"
printf '%s\n' "# Test suite log" >> "${OUT_FILE:?}"

printf '%s\n' \
  "# Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>" \
  >> "${OUT_FILE:?}"
printf '%s\n' "# SPDX-License-Identifier: MIT-0" >> "${OUT_FILE:?}"
printf '\n%s' "----------------------------------------" >> "${OUT_FILE:?}"
printf '%s\n\n' "--------------------------------------" >> "${OUT_FILE:?}"
# /* REUSE-IgnoreEnd */

printf '%s' "Running test suite... "

run_test "No options"

run_test "Help (--help)" \
  "${TEST_FILE:?}" --help

run_test "Help (-h)" \
  "${TEST_FILE:?}" -h

run_test "Invalid bits (0)" \
  "${TEST_FILE:?}" --bits=0

run_test "Invalid bits (-1)" \
  "${TEST_FILE:?}" --bits=-1

run_test "Invalid bits (a)" \
  "${TEST_FILE:?}" --bits=a

run_test "Invalid bits (65)" \
  "${TEST_FILE:?}" --bits=65

run_test "Invalid limit (0)" \
  "${TEST_FILE:?}" --limit=0

run_test "Invalid limit (-1)" \
  "${TEST_FILE:?}" --limit=-1

run_test "Invalid limit (a)" \
  "${TEST_FILE:?}" --limit=a

run_test "Nonexistent" \
  "${TEST_FILE:?}" nonexistent

run_test "8-bit: Standard" \
  "${TEST_FILE:?}"

run_test "8-bit: Limit exact (16 bits)" \
  "${TEST_FILE:?}" --limit=16

run_test "8-bit: Limit mid-octet (12 bits), no pad" \
  "${TEST_FILE:?}" --limit=12

run_test "8-bit: Limit mid-octet (12 bits), with pad" \
  "${TEST_FILE:?}" --limit=12 --pad

run_test "8-bit: Limit > file (32 bits), no pad" \
  "${TEST_FILE:?}" --limit=32

run_test "8-bit: Limit > file (32 bits), with pad (Synthesis)" \
  "${TEST_FILE:?}" --limit=32 --pad

run_test "8-bit: Limit > file (36 bits), with pad (Synthesis + Truncation)" \
  "${TEST_FILE:?}" --limit=36 --pad

run_test "Fallback: bits=7" \
  "${TEST_FILE:?}" --bits=7

run_test "Fallback: bits=7, pad" \
  "${TEST_FILE:?}" --bits=7 --pad

run_test "Fallback: bits=7, limit=7 (1 char)" \
  "${TEST_FILE:?}" --bits=7 --limit=7

run_test "Fallback: bits=7, limit=10, no pad" \
  "${TEST_FILE:?}" --bits=7 --limit=10

run_test "Fallback: bits=7, limit=10, pad" \
  "${TEST_FILE:?}" --bits=7 --limit=10 --pad

run_test "Fallback: bits=7, limit=40 (exceeds), no pad" \
  "${TEST_FILE:?}" --bits=7 --limit=40

run_test "Fallback: bits=7, limit=40 (exceeds), pad" \
  "${TEST_FILE:?}" --bits=7 --limit=40 --pad

run_test "Fallback: bits=9" \
  "${TEST_FILE:?}" --bits=9

run_test "Fallback: bits=9, pad" \
  "${TEST_FILE:?}" --bits=9 --pad

run_test "Fallback: bits=13, limit=20, pad" \
  "${TEST_FILE:?}" --bits=13 --limit=20 --pad

run_test "Edge: Empty file" \
  /dev/null

run_test "Edge: Empty file, limit=16, pad" \
  /dev/null --limit=16 --pad

run_test "Edge: Multiple files" \
  "${TEST_FILE:?}" "${TEST_FILE:?}"

run_test "Edge: Multiple files + nonexistent" \
  "${TEST_FILE:?}" "${TEST_FILE:?}" "nonexistent"

run_test "Edge: Nonexistent + multiple files" \
  nonexistent "${TEST_FILE:?}" "${TEST_FILE:?}"

run_test "Verbose: Standard" \
  "${TEST_FILE:?}" -v

run_test "Verbose: Alias (--verbose)" \
  "${TEST_FILE:?}" --verbose

run_test "Verbose: With bits (7)" \
  "${TEST_FILE:?}" --bits=7 -v

run_test "Verbose: With limit (12) and pad" \
  "${TEST_FILE:?}" --limit=12 --pad -v

run_test "Verbose: Multiple files" \
  "${TEST_FILE:?}" "${TEST_FILE:?}" -v

run_test "Auto: 8-bit (high bit set)" \
  "${TEST_FILE:?}" --bits=auto

run_test "Auto: 7-bit (Hello)" \
  "${SEVEN_FILE:?}" --bits=auto

run_test "Auto: 7-bit, limit=14 (2 chars)" \
  "${SEVEN_FILE:?}" --bits=auto --limit=14

run_test "Auto: 7-bit, limit=10 (mid-char), no pad" \
  "${SEVEN_FILE:?}" --bits=auto --limit=10

run_test "Auto: 7-bit, limit=10 (mid-char), pad" \
  "${SEVEN_FILE:?}" --bits=auto --limit=10 --pad

run_test "Auto: Zero file" \
  "${ZERO_FILE:?}" --bits=auto

run_test "Auto: Empty file" \
  /dev/null --bits=auto

run_test "Auto: Mixed files (7-bit and 8-bit)" \
  "${SEVEN_FILE:?}" "${TEST_FILE:?}" --bits=auto

run_test "Auto: Alias (--auto)" \
  "${SEVEN_FILE:?}" --auto

run_test "Auto: Override (Manual then Auto)" \
  "${SEVEN_FILE:?}" --bits=8 --bits=auto

run_test "Auto: Override (Auto then Manual)" \
  "${SEVEN_FILE:?}" --bits=auto --bits=8

run_test "Pad: Explicit (Hello - 35 bits, needs pad)" \
  "${SEVEN_FILE:?}" --bits=auto --pad -v

run_test "Pad: Auto (Hello - 35 bits, needs pad)" \
  "${SEVEN_FILE:?}" --bits=auto --pad=auto

run_test "Pad: Auto (3 bytes - 24 bits, no pad needed)" \
  "${ZERO_FILE:?}" --bits=8 --pad=auto -v

run_test "Pad: Auto (limit exact, no pad needed)" \
  "${ZERO_FILE:?}" --bits=8 --limit=16 --pad=auto -v

run_test "Pad: Auto (limit mid-char, needs pad)" \
  "${ZERO_FILE:?}" --bits=8 --limit=12 --pad=auto -v

run_test "Pad: Auto (Synthesis, needs pad)" \
  "${ZERO_FILE:?}" --bits=8 --limit=32 --pad=auto -v

run_test "Pad: Mixed (needs pad then doesn't)" \
  "${SEVEN_FILE:?}" "${ZERO_FILE:?}" --bits=auto --pad=auto

run_test "Pad: Override (Manual then Auto)" \
  "${SEVEN_FILE:?}" --bits=auto --pad --pad=auto

run_test "Pad: Override (Auto then Manual)" \
  "${SEVEN_FILE:?}" --bits=auto --pad=auto --pad

run_test "Auto: Bits + Synthesis (Auto-Verbosity)" \
  "${SEVEN_FILE:?}" --bits=auto --limit=40 --pad=auto

run_test "Auto: Pad only (Auto-Verbosity)" \
  "${SEVEN_FILE:?}" --pad=auto

run_test "Auto: Exact match (No Auto-Verbosity)" \
  "${ZERO_FILE:?}" --bits=8 --pad=auto

rm -f ./nonexistent > /dev/null 2>&1 || :
rm -f "${PROG:?}" "${TEST_FILE:?}" "${ZERO_FILE:?}" "${SEVEN_FILE:?}"

printf '%s\n' "# EOF" >> "${OUT_FILE:?}"

printf '%s\n' "done, results saved to '${OUT_FILE:?}'"

# shellcheck disable=SC2015
diff "${REF_FILE:?}" "${OUT_FILE:?}" > /dev/null 2>&1 && {
  printf '%s\n' "SUCCESS!!! Test suite completed with no errors detected."
  exit 0
} || {
  printf '\n%s\n\n' "ERROR!!! '${OUT_FILE:?}' does not match '${REF_FILE:?}'!"
  exit 1
}
