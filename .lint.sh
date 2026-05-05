#!/bin/sh
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
# Oracle Lint - https://www.oracle.com/application-development/developerstudio/
# PVS-Studio: https://pvs-studio.com/
# REUSE: https://codeberg.org/fsfe/reuse-tool
# Scan-Build: https://clang-analyzer.llvm.org/
# ShellCheck: https://www.shellcheck.net/
# Shfmt: https://github.com/mvdan/sh

################################################################################

if [ -n "${ZSH_VERSION-}" ]; then
  emulate sh
  setopt sh_word_split
fi

################################################################################

# shellcheck disable=SC2065
test -f "./${0##*/}" > /dev/null 2>&1 || {
  printf '%s\n' "ERROR: Cannot locate script in current directory."
  exit 1
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

# shellcheck disable=SC1091
. ./.common.sh

################################################################################

CC="$(command -v cc 2> /dev/null || command -v gcc 2> /dev/null \
  || command -v clang 2> /dev/null || printf '%s\n' cc)"

export CC

################################################################################

set -eu

################################################################################

export FIND_COMMAND_FATAL=1

find_command "${CC:?}" "awk" "grep" "make" "paste" "rm" "sed" "sleep" "uname"

################################################################################

export FIND_COMMAND_FATAL=0

# shellcheck disable=SC2310
if out=$(
  find_command \
    bear ch clang codespell cppcheck cppi diff editorconfig-checker \
    flawfinder gcc git markdown-toc plog-converter pvs-studio-analyzer \
    reuse scan-build shellcheck shfmt 2>&1
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

test "${NEED_PAUSE:-0}" -ne 1 || {
  printf '%s\n' "         Some checks will be skipped! [pausing 10s]" \
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
      /opt/oracle/developerstudio12.6/bin/lint -fd -std=c89 "${variant:-}" \
        -err=warn -XCC=no -errchk=structarg,parentheses,locfmtchk crc.c
      /opt/oracle/developerstudio12.6/bin/lint -Xa -DNOANSI "${variant:-}" \
        -err=warn -XCC=no -errchk=structarg,parentheses,locfmtchk crc.c
    else
      /opt/oracle/developerstudio12.6/bin/lint -fd -std=c89 \
        -err=warn -XCC=no -errchk=structarg,parentheses,locfmtchk crc.c
      /opt/oracle/developerstudio12.6/bin/lint -Xa -DNOANSI \
        -err=warn -XCC=no -errchk=structarg,parentheses,locfmtchk crc.c
    fi
  done
}

################################################################################

:
:
: Clang - ANSI
: ::::::::::::
command -v clang > /dev/null 2>&1 && {
  CLANG_ANSI_CFLAGS="-O3 -Weverything -Wno-unsafe-buffer-usage \
   -Wno-missing-noreturn -Werror"
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      # shellcheck disable=SC2086
      clang "${variant:-}" ${CLANG_ANSI_CFLAGS:?} crc.c
    else
      # shellcheck disable=SC2086
      clang ${CLANG_ANSI_CFLAGS:?} crc.c
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
    command -v clang > /dev/null 2>&1 && {
      for variant in "" "-DNOFREAD"; do
        if [ -n "${variant:-}" ]; then
          env CC=clang CFLAGS="${variant:-} ${CLANG_ANSI_CFLAGS:?}" \
            bear -- scan-build --use-cc=clang --status-bugs \
            make crc
        else
          env CC=clang CFLAGS="${CLANG_ANSI_CFLAGS:?}" \
            bear -- scan-build --use-cc=clang --status-bugs \
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
command -v gcc > /dev/null 2>&1 && {
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      gcc -O3 -fanalyzer -Wall -Wextra -Wpedantic -Werror \
        -std=c89 "${variant:-}" -o crc crc.c
    else
      gcc -O3 -fanalyzer -Wall -Wextra -Wpedantic -Werror \
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
command -v clang > /dev/null 2>&1 && {
  CLANG_NOANSI_CFLAGS="-O3 -Weverything -Wno-unsafe-buffer-usage \
   -Wno-missing-noreturn -Wno-deprecated-non-prototype \
   -Wno-strict-prototypes -Werror"
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      # shellcheck disable=SC2086
      clang -DNOANSI "${variant:-}" ${CLANG_NOANSI_CFLAGS:?} crc.c
    else
      # shellcheck disable=SC2086
      clang -DNOANSI ${CLANG_NOANSI_CFLAGS:?} crc.c
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
    command -v clang > /dev/null 2>&1 && {
      for variant in "" "-DNOFREAD"; do
        if [ -n "${variant:-}" ]; then
          env CC=clang CFLAGS="-DNOANSI ${variant:-} ${CLANG_NOANSI_CFLAGS:?}" \
            bear -- scan-build --use-cc=clang --status-bugs \
            make crc
        else
          env CC=clang CFLAGS="-DNOANSI ${CLANG_NOANSI_CFLAGS:?}" \
            bear -- scan-build --use-cc=clang --status-bugs \
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
command -v gcc > /dev/null 2>&1 && {
  for variant in "" "-DNOFREAD"; do
    if [ -n "${variant:-}" ]; then
      gcc -O3 -fanalyzer -Wall -Wextra -Wpedantic -Werror \
        -DNOANSI "${variant:-}" -Wno-old-style-definition -o crc crc.c
    else
      gcc -O3 -fanalyzer -Wall -Wextra -Wpedantic -Werror \
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
    -D__CPPCHECK__ --inline-suppr --inconclusive crc.c
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
  ch -w -n crc.c
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
      "${CC:-cc}" -O3 -DSELFTEST "${variant:-}" -o selftest crc.c
    else
      "${CC:-cc}" -O3 -DSELFTEST -o selftest crc.c
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
  {
    command -v lint > /dev/null 2>&1 && {
      lint -a -aa -b -c -e -g -h -P -r -u -z crc.c 2>&1 \
        | grep -Ev '(^lint: cannot find llib-lc\.ln$|^crc\.c:$)' || :
    }
  }
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
