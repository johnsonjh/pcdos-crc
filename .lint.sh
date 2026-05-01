#!/bin/sh
# Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
# SPDX-License-Identifier: MIT-0
# scspell-id: 9637d8e2-40d4-11f1-ab65-80ee73e9b8e7
# vim: set ft=sh ts=2 sw=2 tw=0 ai expandtab cc=80 :

if [ -n "${ZSH_VERSION-}" ]; then
  emulate sh
  setopt sh_word_split
fi

test -d "/usr/pkg/gnu/bin" && {
  export PATH="${PATH:-}:/usr/pkg/gnu/bin"
}

set -eux
:
:
: Cleanup
: :::::::
rm -f a.out crc log.pvs compile_commands.json selftest
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
: ::::
command -v diff > /dev/null 2>&1 && {
  diff LICENSES/MIT-0.txt LICENSE
}
:
:
: ShellCheck
: ::::::::::
command -v shellcheck > /dev/null 2>&1 && {
  shellcheck -o any,all .lint.sh .test.sh
}
:
:
: shfmt
: :::::
command -v shfmt > /dev/null 2>&1 && {
  shfmt -bn -sr -fn -i 2 -s -d .lint.sh .test.sh
}
:
:
: Oracle Lint - ANSI and non-ANSI
: :::::::::::::::::::::::::::::::
command -v /opt/oracle/developerstudio12.6/bin/lint > /dev/null 2>&1 && {
  /opt/oracle/developerstudio12.6/bin/lint -fd -std=c89 crc.c
  /opt/oracle/developerstudio12.6/bin/lint -DNOANSI crc.c
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
  rm -f a.out
}
:
:
: Clang Analyzer - ANSI
: :::::::::::::::::::::
command -v bear > /dev/null 2>&1 && {
  command -v scan-build > /dev/null 2>&1 && {
    command -v clang > /dev/null 2>&1 && {
      env CC=clang CFLAGS="${CLANG_ANSI_CFLAGS:?}" \
        bear -- scan-build --use-cc=clang --status-bugs make crc
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
  rm -f a.out
}
:
:
: Clang Analyzer - non-ANSI
: :::::::::::::::::::::::::
command -v bear > /dev/null 2>&1 && {
  command -v scan-build > /dev/null 2>&1 && {
    command -v clang > /dev/null 2>&1 && {
      env CC=clang CFLAGS="-DNOANSI ${CLANG_NOANSI_CFLAGS:?}" \
        bear -- scan-build --use-cc=clang --status-bugs make crc
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
:
:
: markdown-toc
: ::::::::::::
command -v markdown-toc > /dev/null 2>&1 && {
  markdown-toc -i README.md
}
:
:
: editorconfig-checker
: ::::::::::::::::::::
command -v editorconfig-checker > /dev/null 2>&1 && {
  editorconfig-checker
}
:
:
: Ch
: ::
command -v ch > /dev/null 2>&1 && {
  ch -w -n crc.c
}
:
:
: Cppi
: ::::
command -v cppi > /dev/null 2>&1 && {
  cppi -ac crc.c
}
:
:
: SELFTEST
: ::::::::
command -v cc > /dev/null 2>&1 && {
  cc -O -DSELFTEST -o selftest crc.c
  ./selftest crc.c selftest
  rm -f selftest
}
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
:
:
: Test
: ::::
# shellcheck disable=SC2065
test -x ./.test.sh > /dev/null 2>&1 && {
  ./.test.sh
}
:
:
: Finish
: ::::::
rm -f a.out crc log.pvs compile_commands.json selftest
