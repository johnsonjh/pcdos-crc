/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - win32dwc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 4a2ba952-7ee5-11f1-b5f1-80ee73e9b8e7
 */

/******************************************************************************/

/* Expands wildcards and modifies argc and argv for Windows (Win32) builds. */

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

#define WIN32_LEAN_AND_MEAN

/******************************************************************************/

#include <windows.h>
#include <string.h>

/******************************************************************************/

#define MAX_ARGS 128
#define MAX_PATH_LEN MAX_PATH

/******************************************************************************/

#define ATTR_MASK (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_NORMAL)

/******************************************************************************/

static char argbuf [MAX_ARGS] [MAX_PATH_LEN];
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

  if (MAX_PATH_LEN <= len)
    len = MAX_PATH_LEN - 1;

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
  const char * p = pattern;

  while ('\0' != * p)
    {
      if ('\\' == * p || '/' == * p || ':' == * p)
        last_sep = p;
      p++;
    }

  if (NULL != last_sep)
    {
      size_t plen = 0;
      const char * q = pattern;

      while (q <= last_sep)
        {
          plen++;
          q++;
        }

      if (MAX_PATH_LEN <= plen)
        plen = MAX_PATH_LEN - 1;

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
  char prefix [MAX_PATH_LEN];
  const char * mask;
  WIN32_FIND_DATAA fd;
  HANDLE hFind;
  int any = 0;

  if (isalpha ((unsigned char)pattern [0]) &&
      ':' == pattern[1] && '\\' != pattern[2] && '/' != pattern[2])
    {
      static char fixedpat [MAX_PATH_LEN];
      size_t mlen = strlen(pattern + 2);

      if (4 + mlen >= MAX_PATH_LEN)
        mlen = MAX_PATH_LEN - 5;

      fixedpat [0] = pattern [0];
      fixedpat [1] = ':';
      fixedpat [2] = '.';
      fixedpat [3] = '\\';

      (void)memcpy (fixedpat + 4, pattern + 2, mlen);
      fixedpat [4 + mlen] = '\0';

      pattern = fixedpat;
    }

  split_prefix (pattern, prefix, &mask);

  hFind = FindFirstFileA (pattern, &fd);

  if (INVALID_HANDLE_VALUE == hFind)
    {
      (void)add_arg (pattern);

      return;
    }

  do
    {
      DWORD attr = fd.dwFileAttributes;

      if (0 != (attr & FILE_ATTRIBUTE_DIRECTORY))
        continue;

      if (0 == (attr & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE)))
        continue;

      {
        char full [MAX_PATH_LEN];
        size_t plen = strlen (prefix);
        size_t nlen = strlen (fd.cFileName);
        size_t total = plen + nlen;

        if (MAX_PATH_LEN <= total)
          {
            if (MAX_PATH_LEN <= plen)
              continue;

            nlen = MAX_PATH_LEN - 1 - plen;
            total = plen + nlen;
          }

        (void)memcpy (full, prefix, plen);
        (void)memcpy (full + plen, fd.cFileName, nlen);
        full [total] = '\0';

        if (0 != add_arg (full))
          break;

        any = 1;
      }
    }
  while (FindNextFileA (hFind, &fd));

  (void)FindClose (hFind);

  if (0 == any)
    (void)add_arg (pattern);
}

/******************************************************************************/

void
#ifdef ANSI_COMPILER
win32_expand_wildcards ( /*cppcheck-suppress unusedFunction*/
  int * argc,
  char * * * argv)
#else
win32_expand_wildcards (argc, argv) /*cppcheck-suppress unusedFunction*/
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

      if (0 != has_wc)
        expand_pattern((* argv)[i]);
      else
        (void)add_arg((* argv)[i]);

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
