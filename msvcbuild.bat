:: IBM PC-DOS CRC.EXE-compatible CRC calculator - msvcbuild.bat
:: Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
:: SPDX-License-Identifier: MIT-0
:: vim: set ft=dosbatch cc=80 :
:: scspell-id: 8b6e0fb6-752d-11f1-9c0c-80ee73e9b8e7

REM === Compile CRC ===
cl /Ob3 /GS- /Oi /O2 /W4 /wd4996 /Fecrc.exe crc.c win32dwc.c

:: Local Variables:
:: mode: bat-mode
:: fill-column: 80
:: eval: (setq-local display-fill-column-indicator-column 80)
:: eval: (display-fill-column-indicator-mode 1)
:: End:
