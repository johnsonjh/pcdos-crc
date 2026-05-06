#!/bin/sh
# .lint.sh
# Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
# SPDX-License-Identifier: MIT-0
# scspell-id: 9637d8e2-40d4-11f1-ab65-80ee73e9b8e7

################################################################################

# The following tools are optional; they will be used if available:
# Bear: https://github.com/rizsotto/bear
# Ch: https://www.softintegration.com/
# Clang: https://clang.llvm.org/
# Codespell: https://github.com/codespell-project/codespell
# Cppcheck: https://cppcheck.sourceforge.io/
# Cppi: https://www.gnu.org/software/cppi/
# Diff: https://www.gnu.org/software/diffutils/
# Editorconfig-Checker: https://editorconfig-checker.github.io/
# GCC: https://gcc.gnu.org/
# Markdown-toc: https://github.com/jonschlinkert/markdown-toc
# NetBSD Lint: https://man.netbsd.org/lint.1
# Oracle Lint: https://www.oracle.com/application-development/developerstudio/
# PVS-Studio: https://pvs-studio.com/
# REUSE: https://codeberg.org/fsfe/reuse-tool
# Scan-Build: https://clang-analyzer.llvm.org/
# ShellCheck: https://www.shellcheck.net/
# Shfmt: https://github.com/mvdan/sh

################################################################################

# It is also recommended to (manually) run the following periodically:
# DUMA: https://github.com/johnsonjh/duma
# Funcheck: https://github.com/froz42/funcheck
# Hunspell: https://hunspell.github.io/
# Semgrep: https://semgrep.dev/
# Smatch: https://repo.or.cz/w/smatch.git
# Valgrind: https://valgrind.org/

################################################################################

if [ -n "${ZSH_VERSION-}" ]; then
  emulate sh
  setopt sh_word_split
fi

################################################################################

# shellcheck disable=SC2065
test -f "./${0##*/}" > /dev/null 2>&1 || {
  printf '%s\n' "ERROR: Could not locate script in current directory."
  exit 1
}

################################################################################

test -d "/opt/freeware/bin" && {
  export PATH="/opt/freeware/bin:${PATH:-}"
}

################################################################################

test -d "/usr/pkg/gnu/bin" && {
  export PATH="${PATH:-}:/usr/pkg/gnu/bin"
}

################################################################################

# shellcheck disable=SC2065
test -f "./.common.sh" > /dev/null 2>&1 || {
  printf '%s\n' "ERROR: Could not locate .common.sh in current directory."
  exit 1
}

################################################################################

set -eu

################################################################################

export CPE1704TKS=1

# shellcheck disable=SC1091
. ./.common.sh

################################################################################

CC="$(command -v cc 2> /dev/null || command -v "${GCC_CMD:-gcc}" 2> /dev/null \
  || command -v "${CLANG_CMD:-clang}" 2> /dev/null || printf '%s\n' cc)"

export CC

################################################################################

export FIND_COMMAND_FATAL=1

find_command "${CC:?}" awk grep make paste rm sed sleep uname

################################################################################

export FIND_COMMAND_FATAL=0

# shellcheck disable=SC2310
if out=$(
  find_command \
    bear ch "${CLANG_CMD:-clang}" codespell cppcheck cppi diff \
    editorconfig-checker flawfinder "${GCC_CMD:-gcc}" git markdown-toc \
    plog-converter pvs-studio-analyzer reuse scan-build shellcheck shfmt 2>&1
); then
  status=0
else
  status="$?"
fi

width="$(detect_width)"

# shellcheck disable=SC2310
printf '%s\n' "${out:-}" \
  | wrap "${width:?}"

unset NEED_PAUSE

if [ "${status:?}" -ne 0 ]; then
  NEED_PAUSE=1
fi

################################################################################

os="$(uname -s 2> /dev/null)"

unset CHECK_OLINT

case "${os:?}" in
Linux)
  CHECK_OLINT=1
  ;;
Solaris)
  CHECK_OLINT=1
  ;;
*) : ;;
esac

unset OLINT

if [ "${CHECK_OLINT:-0}" -eq 1 ]; then
  if command -v "/opt/solarisstudio12.6/bin/lint" \
    > /dev/null 2>&1; then
    OLINT="/opt/solarisstudio12.6/bin/lint"
  elif command -v "/opt/oracle/developerstudio12.6/bin/lint" \
    > /dev/null 2>&1; then
    OLINT="/opt/oracle/developerstudio12.6/bin/lint"
  fi

  if [ -z "${OLINT+x}" ]; then
    printf '%s\n' "WARNING: Oracle Developer Studio Lint was not found!" \
      | wrap "${width:?}"
    NEED_PAUSE=1
  fi
fi

################################################################################

case ${OVERRIDE_PAUSE:-} in
'' | *[!0-9]*)
  unset OVERRIDE_PAUSE
  ;;
*) : ;;
esac

test "${NEED_PAUSE:-0}" -ne 1 || {
  printf '%s\n' \
    "Some checks will be skipped! [pausing ${OVERRIDE_PAUSE:-10}s]" \
    | wrap "${width:?}"
  sleep "${OVERRIDE_PAUSE:-10}"
}

################################################################################

set -x

################################################################################

:
:
: Cleanup
: :::::::
rm -f a.out crc log.pvs compile_commands.json selftest
rm -rf ./pvsreport
make clean

################################################################################

:
:
: Tag generation
: ::::::::::::::
make tags

################################################################################

:
:
: codespell
: :::::::::
command -v codespell > /dev/null 2>&1 && {
  command -v git > /dev/null 2>&1 && {
    CODESPELL_EXCLUDE=$({
      git ls-files --ignored --exclude-standard --others \
        | sed 's/["\\]/\\&/g' \
        | paste -sd',' -
    } | sed 's/^/"/; s/$/"/')
    codespell --skip "${CODESPELL_EXCLUDE:?}" .
  }
}

################################################################################

:
:
: reuse
: :::::
command -v reuse > /dev/null 2>&1 && {
  reuse lint -q || reuse lint
}

################################################################################

:
:
: diff
: ::::
command -v diff > /dev/null 2>&1 && {
  diff LICENSES/MIT-0.txt LICENSE
}

################################################################################

:
:
: ShellCheck
: ::::::::::
command -v shellcheck > /dev/null 2>&1 && {
  shellcheck -o any,all .common.sh .lint.sh .test.sh
}

################################################################################

:
:
: shfmt
: :::::
command -v shfmt > /dev/null 2>&1 && {
  shfmt -bn -sr -fn -i 2 -s -d .common.sh .lint.sh .test.sh
}

################################################################################

:
:
: Flawfinder
: ::::::::::
command -v flawfinder > /dev/null 2>&1 && {
  flawfinder --quiet --omittime --error-level=3 --context --minlevel=3 crc.c
}

################################################################################

:
:
: Banned operations
: :::::::::::::::::
grep -n -E '( / | /= | % | %= )' crc.c && {
  : ERROR: Banned operations found
  exit 1
}

################################################################################

:
:
: Oracle Lint - ANSI and non-ANSI
: :::::::::::::::::::::::::::::::
command -v "${OLINT:-}" > /dev/null 2>&1 && {

  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      "${OLINT:?}" -fd -std=c89 "${variant:-}" \
        -err=warn -XCC=no -errchk=structarg,parentheses,locfmtchk crc.c
      "${OLINT:?}" -Xa -DNOANSI "${variant:-}" \
        -err=warn -XCC=no -errchk=structarg,parentheses,locfmtchk crc.c
    else
      "${OLINT:?}" -fd -std=c89 \
        -err=warn -XCC=no -errchk=structarg,parentheses,locfmtchk crc.c
      "${OLINT:?}" -Xa -DNOANSI \
        -err=warn -XCC=no -errchk=structarg,parentheses,locfmtchk crc.c
    fi
  done
}

################################################################################

:
:
: Clang - ANSI
: ::::::::::::
command -v "${CLANG_CMD:-clang}" > /dev/null 2>&1 && {
  CLANG_ANSI_CFLAGS="-O3 -Weverything -Wno-unsafe-buffer-usage \
   -Wno-missing-noreturn -Werror"
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      # shellcheck disable=SC2086
      "${CLANG_CMD:-clang}" "${variant:-}" ${CLANG_ANSI_CFLAGS:?} crc.c
    else
      # shellcheck disable=SC2086
      "${CLANG_CMD:-clang}" ${CLANG_ANSI_CFLAGS:?} crc.c
    fi
    rm -f a.out
  done
}

################################################################################

:
:
: Clang and PVS-Studio Analyzers - ANSI
: :::::::::::::::::::::::::::::::::::::
command -v bear > /dev/null 2>&1 && {
  command -v scan-build > /dev/null 2>&1 && {
    command -v "${CLANG_CMD:-clang}" > /dev/null 2>&1 && {
      for variant in "" "-DNOFREAD"; do
        if [ -n "${variant:-}" ]; then
          env CC="${CLANG_CMD:-clang}" \
            CFLAGS="${variant:-} ${CLANG_ANSI_CFLAGS:?}" \
            bear -- scan-build --use-cc="${CLANG_CMD:-clang}" --status-bugs \
            make crc
        else
          env CC="${CLANG_CMD:-clang}" \
            CFLAGS="${CLANG_ANSI_CFLAGS:?}" \
            bear -- scan-build --use-cc="${CLANG_CMD:-clang}" --status-bugs \
            make crc
        fi
        rm -f crc

        command -v pvs-studio-analyzer > /dev/null 2>&1 && {
          command -v plog-converter > /dev/null 2>&1 && {
            test -f compile_commands.json && {
              pvs-studio-analyzer analyze --intermodular \
                -j "$(nproc || printf '%s\n' '1' || :)" -o log.pvs
              plog-converter -a "GA:1,2,3" -t markdown log.pvs -w \
                || plog-converter -a "GA:1,2,3" -t fullhtml \
                  log.pvs -w -o pvsreport
            }
          }
        }
      done
    }
  }
}

################################################################################

:
:
: GCC Analyzer - ANSI
: :::::::::::::::::::
command -v "${GCC_CMD:-gcc}" > /dev/null 2>&1 && {
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      "${GCC_CMD:-gcc}" -O3 -fanalyzer -Wall -Wextra -Wpedantic -Werror \
        -std=c89 "${variant:-}" -o crc crc.c
    else
      "${GCC_CMD:-gcc}" -O3 -fanalyzer -Wall -Wextra -Wpedantic -Werror \
        -std=c89 -o crc crc.c
    fi
    rm -f crc
  done
}

################################################################################

:
:
: Clang - non-ANSI
: ::::::::::::::::
command -v "${CLANG_CMD:-clang}" > /dev/null 2>&1 && {
  CLANG_NOANSI_CFLAGS="-O3 -Weverything -Wno-unsafe-buffer-usage \
   -Wno-missing-noreturn -Wno-deprecated-non-prototype \
   -Wno-strict-prototypes -Werror"
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      # shellcheck disable=SC2086
      "${CLANG_CMD:-clang}" -DNOANSI \
        "${variant:-}" ${CLANG_NOANSI_CFLAGS:?} crc.c
    else
      # shellcheck disable=SC2086
      "${CLANG_CMD:-clang}" -DNOANSI \
        ${CLANG_NOANSI_CFLAGS:?} crc.c
    fi
    rm -f a.out
  done
}

################################################################################

:
:
: Clang and PVS-Studio Analyzers - non-ANSI
: :::::::::::::::::::::::::::::::::::::::::
command -v bear > /dev/null 2>&1 && {
  command -v scan-build > /dev/null 2>&1 && {
    command -v "${CLANG_CMD:-clang}" > /dev/null 2>&1 && {
      for variant in "" "-DNOFREAD"; do
        if [ -n "${variant:-}" ]; then
          env CC="${CLANG_CMD:-clang}" \
            CFLAGS="-DNOANSI ${variant:-} ${CLANG_NOANSI_CFLAGS:?}" \
            bear -- scan-build --use-cc="${CLANG_CMD:-clang}" --status-bugs \
            make crc
        else
          env CC="${CLANG_CMD:-clang}" \
            CFLAGS="-DNOANSI ${CLANG_NOANSI_CFLAGS:?}" \
            bear -- scan-build --use-cc="${CLANG_CMD:-clang}" --status-bugs \
            make crc
        fi
        rm -f crc

        command -v pvs-studio-analyzer > /dev/null 2>&1 && {
          command -v plog-converter > /dev/null 2>&1 && {
            test -f compile_commands.json && {
              pvs-studio-analyzer analyze --intermodular \
                -j "$(nproc || printf '%s\n' '1' || :)" -o log.pvs
              plog-converter -a "GA:1,2,3" -t markdown log.pvs -w \
                || plog-converter -a "GA:1,2,3" -t fullhtml \
                  log.pvs -w -o pvsreport
            }
          }
        }
      done
    }
  }
}

################################################################################

:
:
: GCC Analyzer - non-ANSI
: :::::::::::::::::::::::
command -v "${GCC_CMD:-gcc}" > /dev/null 2>&1 && {
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      "${GCC_CMD:-gcc}" -O3 -fanalyzer -Wall -Wextra -Wpedantic -Werror \
        -DNOANSI "${variant:-}" -Wno-old-style-definition -o crc crc.c
    else
      "${GCC_CMD:-gcc}" -O3 -fanalyzer -Wall -Wextra -Wpedantic -Werror \
        -DNOANSI -Wno-old-style-definition -o crc crc.c
    fi
    rm -f crc
  done
}

################################################################################

:
:
: Cppcheck
: ::::::::
command -v cppcheck > /dev/null 2>&1 && {
  cppcheck --check-level=exhaustive 2>&1 \
    | grep -q 'unrecognized command line option' \
    || {
      CHECK_LEVEL="--check-level=exhaustive"
    }
  # shellcheck disable=2086
  cppcheck --enable=warning,style,performance,portability,unusedFunction \
    --force ${CHECK_LEVEL:-} --std=c89 --platform=unix64 \
    -D__CPPCHECK__ -D__LINT__ --inline-suppr --inconclusive crc.c
}

################################################################################

:
:
: markdown-toc
: ::::::::::::
command -v markdown-toc > /dev/null 2>&1 && {
  markdown-toc -i README.md
}

################################################################################

:
:
: editorconfig-checker
: ::::::::::::::::::::
command -v editorconfig-checker > /dev/null 2>&1 && {
  editorconfig-checker
}

################################################################################

:
:
: Ch
: ::
command -v ch > /dev/null 2>&1 && {
  ch -w -n ./crc.c
}

################################################################################

:
:
: Cppi
: ::::
command -v cppi > /dev/null 2>&1 && {
  cppi -ac crc.c
}

################################################################################

:
:
: SELFTEST
: ::::::::
command -v "${CC:-cc}" > /dev/null 2>&1 && {
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      # shellcheck disable=SC2086
      "${CC:-cc}" ${CFLAGS:--O3} ${LDFLAGS:-} -DSELFTEST "${variant:-}" \
        -o selftest crc.c
    else
      # shellcheck disable=SC2086
      "${CC:-cc}" ${CFLAGS:--O3} ${LDFLAGS:-} -DSELFTEST \
        -o selftest crc.c
    fi
    ./selftest --auto crc.c selftest
    rm -f selftest
  done
}

################################################################################

:
:
: NetBSD Lint
: :::::::::::
case "$(uname -s 2> /dev/null || :)" in
NetBSD)
  if command -p -v lint > /dev/null 2>&1; then
    lint -a -aa -b -c -e -g -h -P -r -T -u -w -z crc.c
  fi
  ;;
*) : ;;
esac

################################################################################

:
:
: Finish
: ::::::
rm -f a.out crc log.pvs compile_commands.json selftest

################################################################################

:
:
: You can run "./.test.sh" or "make test" now.
:

################################################################################

# Local Variables:
# mode: shell
# indent-tabs-mode: nil
# sh-basic-offset: 2
# tab-width: 2
# fill-column: 80
# eval: (add-hook 'before-save-hook 'untabify nil t)
# eval: (setq-local display-fill-column-indicator-column 80)
# eval: (display-fill-column-indicator-mode 1)
# End:

################################################################################
# vim: set ft=sh ts=2 sw=2 tw=0 ai expandtab cc=80 :
################################################################################
