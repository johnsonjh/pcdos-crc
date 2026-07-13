/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - dosdwc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 7b286b0c-7ed2-11f1-9426-80ee73e9b8e7
 */

/******************************************************************************/

/*
 * Expands wildcards and modifies argc and argv for
 * Digital Mars C/C++ or (Open) Watcom for MS-DOS.
 */

/******************************************************************************/

#include <ctype.h>
#include <dos.h>
#include <stddef.h>
#include <string.h>

/******************************************************************************/

#define MAX_ARGS 128
#define MAX_PATH 128

/******************************************************************************/

#define ATTR_MASK (_A_NORMAL | _A_ARCH)

/******************************************************************************/

static char argbuf [MAX_ARGS] [MAX_PATH];
static char * newargv [MAX_ARGS];
static int newargc;

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
add_arg (
  const char * s)
#else
add_arg (s)
  const char * s;
#endif
{
  unsigned long len;

  if (MAX_ARGS <= newargc)
    return -1;

  len = (unsigned long)strlen (s);

  if (MAX_PATH <= len)
    len = MAX_PATH - 1;

  (void)memcpy (argbuf [newargc], s, len);
  argbuf [newargc] [len] = '\0';
  newargv [newargc] = argbuf [newargc];
  newargc++;

  return 0;
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
split_prefix (
  const char * pattern,
  char * prefix,
  const char * * mask)
#else
split_prefix (pattern, prefix, mask)
  const char * pattern;
  char * prefix;
  const char * * mask;
#endif
{
  const char * last_sep = NULL;
  const char * scan = pattern;

  while ('\0' != * scan)
    {
      if ('\\' == * scan || '/' == * scan || ':' == * scan)
        last_sep = scan;

      scan++;
    }

  if (NULL != last_sep)
    {
      long plen = 0UL;
      const char * p = pattern;

      while (p <= last_sep)
        {
          plen++;
          p++;
        }

      if (MAX_PATH <= plen)
        plen = MAX_PATH - 1;

      (void)memcpy (prefix, pattern, plen);
      prefix [plen] = '\0';

      * mask = last_sep + 1;
    }
  else
    {
      prefix [0] = '\0';
      * mask = pattern;
    }
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
expand_pattern (
  const char * pattern)
#else
expand_pattern (pattern)
  const char * pattern;
#endif
{
  struct find_t info;
  char prefix [MAX_PATH];
  const char * mask;
  int rc;

  split_prefix (pattern, prefix, & mask);

  rc = _dos_findfirst (pattern, ATTR_MASK, & info);

  if (0 == rc)
    do
      {
        char full [MAX_PATH];
        unsigned long plen = (unsigned long)strlen (prefix);
        unsigned long nlen = (unsigned long)strlen (info.name);
        unsigned long total = plen + nlen;

        if (MAX_PATH <= total)
          {
            if (MAX_PATH <= plen)
              {
                rc = _dos_findnext (& info);

                continue;
              }

            nlen = MAX_PATH - 1 - plen;
            total = plen + nlen;
          }

        (void)memcpy (full, prefix, plen);
        (void)memcpy (full + plen, info.name, nlen);
        full [total] = '\0';

        if (0 != add_arg (full))
          break;

        rc = _dos_findnext (& info);
      }
    while (0 == rc);
  else
    (void)add_arg (pattern);
}

/******************************************************************************/

void
#ifdef ANSI_COMPILER
dos_expand_wildcards ( /*cppcheck-suppress unusedFunction*/
  int * argc,
  char * * * argv)
#else
dos_expand_wildcards (argc, argv) /*cppcheck-suppress unusedFunction*/
  int * argc;
  char * * * argv;
#endif
{
  int i;

  newargc = 0;

  if (0 < * argc)
    {
      (void)add_arg ((* argv)[0]);
      i = 1;
    }
  else
    i = 0;

  while (i < * argc)
    {
      const char * p = (* argv)[i];
      int has_wc = 0;

      while ('\0' != * p)
        {
          if ('*' == * p || '?' == * p)
            {
              has_wc = 1;

              break;
            }

          p++;
        }

      if (1 == has_wc)
        expand_pattern ((* argv)[i]);
      else
        (void)add_arg ((* argv)[i]);

      i++;
    }

  * argc = newargc;
  * argv = newargv;
}

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
