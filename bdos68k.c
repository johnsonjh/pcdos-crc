/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - bdos68k.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 6a3c653e-73f9-11f1-ba58-80ee73e9b8e7
 * //-V::1107
 */

/******************************************************************************/

#asm
  MOVE.W 4(SP),D0        ; func
  MOVE.L 6(SP),D1        ; param (pointer/int)
  TRAP   #2              ; BDOS
#endasm

/******************************************************************************/

/*
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * tab-width: 2
 * c-basic-offset: 2
 * fill-column: 80
 * eval: (setq-local display-fill-column-indicator-column 80)
 * eval: (display-fill-column-indicator-mode 1)
 * End:
 */

/******************************************************************************/
/* vim: set ft=c ts=2 sw=2 tw=0 ai expandtab cc=80 : */
/******************************************************************************/
