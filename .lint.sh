#!/bin/sh
# Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
# SPDX-License-Identifier: MIT-0
# scspell-id: 9637d8e2-40d4-11f1-ab65-80ee73e9b8e7
# vim: set ft=sh ts=2 sw=2 tw=0 ai expandtab cc=80 :
if [ -n "${ZSH_VERSION-}" ]; then
  emulate sh
  setopt sh_word_split
fi
set -eux
:
:
: Cleanup
: :::::::
rm -f a.out crc log.pvs compile_commands.json
rm -rf ./pvsreport
:
:
: codespell
: :::::::::
command -v codespell > /dev/null 2>&1 && {
  codespell .
}
:
:
: reuse
: :::::
command -v reuse > /dev/null 2>&1 && {
  reuse lint -q || reuse lint
}
:
:
: diff
: :::::
command -v diff > /dev/null 2>&1 && {
  diff LICENSES/MIT-0.txt LICENSE
}
:
:
: ShellCheck
: ::::::::::
command -v shellcheck > /dev/null 2>&1 && {
  shellcheck -o any,all .lint.sh
}
:
:
: shfmt
: :::::
command -v shfmt > /dev/null 2>&1 && {
  shfmt -bn -sr -fn -i 2 -s -d .lint.sh
}
:
:
: Oracle Lint - ANSI and non-ANSI
: :::::::::::::::::::::::::::::::
OLINT="/opt/oracle/developerstudio12.6/bin/lint"
test -x "${OLINT:?}" && {
  "${OLINT:?}" -fd -std=c89 crc.c
  "${OLINT:?}" -DNOANSI crc.c
}
:
:
: : Clang - ANSI
: : ::::::::::::
command -v clang > /dev/null 2>&1 && {
  CLANG_ANSI_CFLAGS="-Weverything -Wno-unsafe-buffer-usage \
   -Wno-missing-noreturn -Werror"
  # shellcheck disable=SC2086
  clang ${CLANG_ANSI_CFLAGS:?} crc.c
}
rm -f a.out
:
:
: Clang Analyzer - ANSI
: :::::::::::::::::::::
command -v bear > /dev/null 2>&1 && {
  command -v scan-build > /dev/null 2>&1 && {
    command -v clang > /dev/null 2>&1 && {
      env CC=clang CFLAGS="${CLANG_ANSI_CFLAGS:?}" \
        bear -- scan-build \
        --use-cc=clang \
        --status-bugs make crc
      rm -f crc
    }
  }
}
:
:
: PVS-Studio Analyzer - ANSI
: ::::::::::::::::::::::::::
command -v clang > /dev/null 2>&1 && {
  command -v bear > /dev/null 2>&1 && {
    command -v pvs-studio-analyzer > /dev/null 2>&1 && {
      command -v plog-converter > /dev/null 2>&1 && {
        test -f compile_commands.json && {
          rm -f log.pvs
          pvs-studio-analyzer analyze --intermodular \
            -j "$(nproc || printf '%s\n' '1' || :)" -o log.pvs
          plog-converter -a "GA:1,2,3" -t markdown log.pvs -w \
            || plog-converter -a "GA:1,2,3" -t fullhtml log.pvs -w -o pvsreport
        }
      }
    }
  }
}
:
:
: Clang - non-ANSI
: ::::::::::::::::
command -v clang > /dev/null 2>&1 && {
  CLANG_NOANSI_CFLAGS="-Weverything -Wno-unsafe-buffer-usage \
   -Wno-missing-noreturn -Wno-deprecated-non-prototype \
   -Wno-strict-prototypes -Werror"
  # shellcheck disable=SC2086
  clang -DNOANSI ${CLANG_NOANSI_CFLAGS:?} crc.c
}
rm -f a.out
:
:
: Clang Analyzer - non-ANSI
: :::::::::::::::::::::::::
command -v bear > /dev/null 2>&1 && {
  command -v scan-build > /dev/null 2>&1 && {
    command -v clang > /dev/null 2>&1 && {
      env CC=clang CFLAGS="-DNOANSI ${CLANG_NOANSI_CFLAGS:?}" \
        bear -- scan-build \
        --use-cc=clang \
        --status-bugs make crc
      rm -f crc
    }
  }
}
:
:
: PVS-Studio Analyzer - non-ANSI
: ::::::::::::::::::::::::::::::
command -v clang > /dev/null 2>&1 && {
  command -v bear > /dev/null 2>&1 && {
    command -v pvs-studio-analyzer > /dev/null 2>&1 && {
      command -v plog-converter > /dev/null 2>&1 && {
        test -f compile_commands.json && {
          rm -f log.pvs
          pvs-studio-analyzer analyze --intermodular \
            -j "$(nproc || printf '%s\n' '1' || :)" -o log.pvs
          plog-converter -a "GA:1,2,3" -t markdown log.pvs -w \
            || plog-converter -a "GA:1,2,3" -t fullhtml log.pvs -w -o pvsreport
        }
      }
    }
  }
}
:
:
: Cppcheck
: ::::::::
command -v cppcheck > /dev/null 2>&1 && {
  cppcheck --force --check-level=exhaustive crc.c
}
:
:
: Finish
: ::::::
rm -f a.out crc log.pvs compile_commands.json
