/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - amigadwc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: d1b36f20-7feb-11f1-9172-80ee73e9b8e7
 */

/******************************************************************************/

/* Expands wildcards and modifies argc and argv for AmigaOS (AmigaDOS). */

/******************************************************************************/

/*
 * If you are NOT using an ANSI C89 compiler (e.g., K&R, "C86"),
 * COMMENT OUT "#define ANSI_COMPILER" below (or define NOANSI).
 */

#define ANSI_COMPILER

/******************************************************************************/

#ifdef NOANSI
# ifdef ANSI_COMPILER
#  undef ANSI_COMPILER
# endif
#endif

/******************************************************************************/

#include <ctype.h>
#include <stddef.h>
#include <string.h>

/******************************************************************************/

typedef long LONG;
typedef char * STRPTR;
typedef LONG BPTR;

/******************************************************************************/

struct FileInfoBlock {
  LONG fib_DiskKey;      /*cppcheck-suppress unusedStructMember*/
  LONG fib_DirEntryType;
  char fib_FileName [108];
};

/******************************************************************************/

#ifndef ACCESS_READ
# define ACCESS_READ (-2L)
#endif

/******************************************************************************/

extern BPTR Lock (STRPTR name, LONG type);
extern LONG UnLock (BPTR lock);
extern LONG Examine (BPTR lock, struct FileInfoBlock * fib);
extern LONG ExNext (BPTR lock, struct FileInfoBlock * fib);

/******************************************************************************/

#define MAX_ARGS 128
#define MAX_PATH 256

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
      if ('/' == * scan || ':' == * scan)
        last_sep = scan;

      scan++;
    }

  if (NULL != last_sep)
    {
      long plen = 0;
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

static int
#ifdef ANSI_COMPILER
has_wildcards (
  const char * s)
#else
has_wildcards (s)
  const char * s;
#endif
{
  while ('\0' != * s)
    {
      if ('*' == * s || '?' == * s)
        return 1;

      s++;
    }

  return 0;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
match_mask (
  const char * mask,
  const char * name)
#else
match_mask (mask, name)
  const char * mask;
  const char * name;
#endif
{
  while ('\0' != * mask)
    {
      if ('*' == * mask)
        {
          mask++;

          if ('\0' == * mask)
            return 1;

          while ('\0' != * name)
            {
              if (match_mask (mask, name))
                return 1;

              name++;
            }

          return 0;
        }
      else if ('?' == * mask)
        {
          if ('\0' == * name)
            return 0;

          mask++;
          name++;
        }
      else
        {
          if (tolower ((unsigned char)* mask) !=
              tolower ((unsigned char)* name))
            return 0;

          mask++;
          name++;
        }
    }

  return ('\0' == * name);
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
  char prefix [MAX_PATH];
  const char * mask;
  BPTR lock;
  struct FileInfoBlock fib;
  LONG ok;

  split_prefix (pattern, prefix, & mask);

  lock = Lock ((STRPTR)(prefix [0] ? prefix : ""), ACCESS_READ);

  if (0 == lock)
    {
      (void)add_arg (pattern);

      return;
    }

  ok = Examine (lock, & fib);

  if (0 == ok)
    {
      UnLock (lock);
      (void)add_arg (pattern);

      return;
    }

  while (0 != ok)
    {
      if (fib.fib_DirEntryType < 0)
        {
          if (match_mask (mask, fib.fib_FileName))
            {
              char full [MAX_PATH];
              unsigned long plen = (unsigned long)strlen (prefix);
              unsigned long nlen = (unsigned long)strlen (fib.fib_FileName);
              unsigned long total = plen + nlen;

              if (MAX_PATH <= total)
                {
                  if (MAX_PATH <= plen)
                    {
                      ok = ExNext (lock, & fib);

                      continue;
                    }

                  nlen = MAX_PATH - 1 - plen;
                  total = plen + nlen;
                }

              (void)memcpy (full, prefix, plen);
              (void)memcpy (full + plen, fib.fib_FileName, nlen);
              full [total] = '\0';

              if (0 != add_arg (full))
                break;
            }
        }

      ok = ExNext (lock, & fib);
    }

  UnLock (lock);

  if (0 == newargc)
    (void)add_arg (pattern);
}

/******************************************************************************/

void
#ifdef ANSI_COMPILER
amiga_expand_wildcards ( /*cppcheck-suppress unusedFunction*/
  int * argc,
  char * * * argv)
#else
amiga_expand_wildcards (argc, argv) /*cppcheck-suppress unusedFunction*/
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
      const char * arg = (* argv)[i];

      if (has_wildcards (arg))
        expand_pattern (arg);
      else
        (void)add_arg (arg);

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
