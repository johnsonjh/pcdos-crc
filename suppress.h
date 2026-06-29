/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - suppress.h
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 5d7b7d72-73e4-11f1-bc9c-80ee73e9b8e7
 * //-V::1107
 */

/******************************************************************************/

/*
 * Some compilers, such as older Aztec C releases, do not allow
 * unknown #pragma directives to appear *anywhere* in a source file;
 * they are parsed (and cause errors) even if guarded by an #if/#ifdef,
 * so the actual guards should live in the main `crc.c` file.  This
 * file is optional and currently used only by the MS-DOS MSC 8.00c
 * compiler, to quiet down informational output when `/W4` is used.
 */

/******************************************************************************/

#ifndef CRC_SUPPRESS_H
# define CRC_SUPPRESS_H
# pragma warning(disable: 4135)
# pragma warning(disable: 4702)
# pragma warning(disable: 4703)
# pragma warning(disable: 4711)
#endif

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
