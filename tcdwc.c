/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - tcdwc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 33420fc6-7ed4-11f1-9ee9-80ee73e9b8e7
 */

/******************************************************************************/

/* Expands wildcards and modifies argc and argv for Turbo C 1-1.5 for MS-DOS */

/******************************************************************************/

#define NOANSI

/******************************************************************************/

#ifdef NOANSI
# ifdef ANSI_COMPILER
#  undef ANSI_COMPILER
# endif
#endif

/******************************************************************************/

#include <dir.h>
#include <dos.h>
#include <stddef.h>
#include <string.h>

/******************************************************************************/

#define MAX_ARGS 128
#define MAX_PATH 128

#define ATTR_MASK (FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH)

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
  size_t len;

  if (MAX_ARGS <= newargc)
    return -1;

  len = strlen (s);

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
      size_t plen = 0;
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
  struct ffblk info;
  char prefix [MAX_PATH];
  const char * mask;
  int rc;

  split_prefix (pattern, prefix, & mask);

  rc = findfirst (pattern, & info, ATTR_MASK);

  if (0 == rc)
    do
      {
        char full [MAX_PATH];
        size_t plen = strlen (prefix);
        size_t nlen = strlen (info.ff_name);
        size_t total = plen + nlen;

        if (MAX_PATH <= total)
          {
            if (MAX_PATH <= plen)
              {
                rc = findnext (& info);

                continue;
              }

            nlen = MAX_PATH - 1 - plen;
            total = plen + nlen;
          }

        (void)memcpy (full, prefix, plen);
        (void)memcpy (full + plen, info.ff_name, nlen);
        full [total] = '\0';

        if (0 != add_arg (full))
          break;

        rc = findnext (& info);
      }
    while (0 == rc);
  else
    (void)add_arg (pattern);
}

/******************************************************************************/

void
#ifdef ANSI_COMPILER
tc_expand_wildcards ( /*cppcheck-suppress unusedFunction*/
  int * argc,
  char * * * argv)
#else
tc_expand_wildcards (argc, argv) /*cppcheck-suppress unusedFunction*/
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
