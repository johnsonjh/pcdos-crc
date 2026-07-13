/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - aztecdwc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 41561dd2-3fff-11f1-8e90-80ee73e9b8e7
 */

/******************************************************************************/

/* Expands wildcards and modifies argc and argv for Aztec C86 5.2a for MS-DOS */

/******************************************************************************/

#include <dos.h>
#include <string.h>

/******************************************************************************/

#define MAX_ARGS 128
#define MAX_PATH 128

/******************************************************************************/

#define ATTR_MASK (0x01 | 0x20)

/******************************************************************************/

static char argbuf [MAX_ARGS] [MAX_PATH];
static char * newargv [MAX_ARGS];
static int newargc;

/******************************************************************************/

struct dta
{
  char reserved [21]; /*cppcheck-suppress unusedStructMember*/
  unsigned char attr; /*cppcheck-suppress unusedStructMember*/
  unsigned int time;  /*cppcheck-suppress unusedStructMember*/
  unsigned int date;  /*cppcheck-suppress unusedStructMember*/
  unsigned long size; /*cppcheck-suppress unusedStructMember*/
  char name [13];
};

/******************************************************************************/

static struct dta dtabuf;

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
set_dta (
  void)
#else
set_dta ()
#endif
{
  (void)bdosx (0x1A, (void far *)& dtabuf, 0);
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
find_first (
 const char * pattern,
 int attr)
#else
find_first (pattern, attr)
 const char * pattern;
 int attr;
#endif
{
  return bdosx (0x4E, (void far *)pattern, attr);
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
find_next (
 void)
#else
find_next ()
#endif
{
  return bdos (0x4F, 0, 0);
}

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
  const char * p = pattern;

  while (* p)
    {
      if ('\\' == * p || '/' == * p || ':' == * p)
        last_sep = p;

      p++;
    }

  if (NULL != last_sep)
    {
      size_t plen = 0;
      const char *q = pattern;

      while (q <= last_sep)
        {
          plen++;
          q++;
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
  char prefix [MAX_PATH];
  const char * mask;
  int rc;

  split_prefix (pattern, prefix, & mask);

  set_dta ();

  rc = find_first (pattern, ATTR_MASK);

  if (0 == rc)
    do
      {
        char full [MAX_PATH];
        size_t plen = strlen (prefix);
        size_t nlen = strlen (dtabuf.name);
        size_t total = plen + nlen;

        if (MAX_PATH <= total)
          {
            if (MAX_PATH <= plen)
              {
                rc = find_next ();

                continue;
              }

            nlen = MAX_PATH - 1 - plen;
            total = plen + nlen;
          }

        (void)memcpy (full, prefix, plen);
        (void)memcpy (full + plen, dtabuf.name, nlen);
        full [total] = '\0';

        if (0 != add_arg (full))
          break;

        rc = find_next ();
      }
    while (0 == rc);
  else
    (void)add_arg (pattern);
}

/******************************************************************************/

void
#ifdef ANSI_COMPILER
aztec_expand_wildcards ( /*cppcheck-suppress unusedFunction*/
  int * argc,
  char * * * argv)
#else
aztec_expand_wildcards (argc, argv) /*cppcheck-suppress unusedFunction*/
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

  for (; i < * argc; i++)
    {
      const char * p = (* argv)[i];
      int has_wc = 0;

      while (* p)
        {
          if ('*' == * p || '?' == * p)
            {
              has_wc = 1;

              break;
            }

          p++;
        }

      if (has_wc)
        expand_pattern ((* argv)[i]);
      else
        (void)add_arg ((* argv)[i]);
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
