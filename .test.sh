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
OUT_FILE="./.test_results.log"
REF_FILE="./.ref_results.log"

cc -O3 crc.c -o crc_test

rm -f "nonexistent" > /dev/null 2>&1 || :

printf '\252\125\377' > "${TEST_FILE:?}"

if command -v stdbuf > /dev/null 2>&1; then
  STDBUF=stdbuf
else
  STDBUF=
fi

STDBUF_FLAGS="-o L"
test -n "${STDBUF:-}" || {
  STDBUF_FLAGS=""
}

run_test()
{
  label="${1:?}"
  shift
  printf '%s\n' "TEST: ${label:?}" >> "${OUT_FILE:?}"
  printf '%s\n\n' "ARGS: $*" | sed 's/[[:space:]]*$//' >> "${OUT_FILE}"
  set +e
  if [ -n "${STDBUF:-}" ]; then
    # shellcheck disable=SC2086
    "${STDBUF}" ${STDBUF_FLAGS} "${PROG:?}" "$@" >> "${OUT_FILE:?}" 2>&1
  else
    env "${PROG:?}" "$@" >> "${OUT_FILE:?}" 2>&1
  fi
  printf '\n%s\n' "EXIT: $?" >> "${OUT_FILE:?}"
  set -e
  printf '\n%s' "----------------------------------------" >> "${OUT_FILE:?}"
  printf '%s\n\n' "--------------------------------------" >> "${OUT_FILE:?}"
}

# /* REUSE-IgnoreStart */
printf '%s' "" > "${OUT_FILE:?}"
printf '%s\n' "# Test suite log" >> "${OUT_FILE:?}"

test -n "${STDBUF:-}" || {
  printf '%s\n' "# WARNING: NO STDBUF! RESULTS MAY BE WRONG!" \
    >> "${OUT_FILE:?}"
}

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

rm -f ./nonexistent > /dev/null 2>&1 || :
rm -f "${PROG:?}" "${TEST_FILE:?}"

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
