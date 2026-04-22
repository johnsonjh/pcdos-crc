#!/bin/sh
set -ex

rm -f a.out crc log.pvs compile_commands.json
rm -rf ./pvsreport

printf '%s\n' ""
OLINT="/opt/oracle/developerstudio12.6/bin/lint"
test -x "${OLINT:?}" && {
  "${OLINT:?}" -V
  "${OLINT:?}" -std=c89 -errshort=tags -errtags=yes crc.c
}

CLANG_CFLAGS="-Weverything -Wno-unsafe-buffer-usage \
    -Wno-unused-macros -Wno-reserved-macro-identifier \
    -Wno-date-time -Wno-deprecated-non-prototype \
    -Wno-strict-prototypes -Wno-old-style-definition \
    -Wno-missing-noreturn -Werror"

printf '%s\n' ""
command -v clang && {
  # shellcheck disable=SC2086
  "$(command -v clang)" ${CLANG_CFLAGS:?} crc.c
}
rm -f a.out

printf '%s\n' ""
command -v bear && {
  command -v scan-build && {
   command -v clang && {
     env CC="$(command -v clang || :)" CFLAGS="${CLANG_CFLAGS:?}" \
        bear -- scan-build \
	  --use-cc="$(command -v clang || :)" \
	  --status-bugs make crc
      rm -f crc
    }
  }
}

printf '%s\n' ""
command -v pvs-studio-analyzer && {
  command -v plog-converter && {
    test -f compile_commands.json && {
      pvs-studio-analyzer analyze --intermodular \
        -j "$(nproc || printf '%s\n' '1' || :)" -o log.pvs
      plog-converter -a "GA:1,2,3" -t fullhtml log.pvs -o pvsreport
    }
  }
}

printf '%s\n' ""
command -v cppcheck && {
  cppcheck --force --check-level=exhaustive crc.c
}

printf '%s\n' ""
rm -f a.out crc log.pvs compile_commands.json
