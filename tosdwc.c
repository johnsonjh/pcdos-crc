/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - tosdwc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 4ccd13c8-7fdd-11f1-9e93-80ee73e9b8e7
 */

/******************************************************************************/

/* Expands wildcards and modifies argc and argv for Atari ST (TOS) builds. */

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

#ifdef __VBCC__
# include <tos.h>
#endif

/******************************************************************************/

#ifdef __MINT__
# include <mint/osbind.h>
#endif

/******************************************************************************/

#ifndef FA_READONLY
# define FA_READONLY 0x01
#endif

/******************************************************************************/

#ifndef FA_HIDDEN
# define FA_HIDDEN 0x02
#endif

/******************************************************************************/

#ifndef FA_SYSTEM
# define FA_SYSTEM 0x04
#endif

/******************************************************************************/

#ifndef FA_VOLUME
# define FA_VOLUME 0x08
#endif

/******************************************************************************/

#ifndef FA_SUBDIR
# define FA_SUBDIR 0x10
#endif

/******************************************************************************/

#ifndef FA_ARCHIVE
# define FA_ARCHIVE 0x20
#endif

/******************************************************************************/

struct tos_dta
{
  char d_reserved [21];       /*cppcheck-suppress unusedStructMember*/
  unsigned char d_attrib;     /*cppcheck-suppress unusedStructMember*/
  unsigned char d_time [2];   /*cppcheck-suppress unusedStructMember*/
  unsigned char d_date [2];   /*cppcheck-suppress unusedStructMember*/
  unsigned char d_length [4]; /*cppcheck-suppress unusedStructMember*/
  char d_fname [14];
};

/******************************************************************************/

#define MAX_ARGS 128
#define MAX_PATH 128

/******************************************************************************/

#define ATTR_MASK (FA_READONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCHIVE)

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
      if ('*' == *s || '?' == * s)
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
  struct tos_dta dta;
  char prefix [MAX_PATH];
  const char * mask;
  char broad [MAX_PATH];
  int rc;

  split_prefix (pattern, prefix, & mask);

  if ('\0' != prefix[0])
    {
      unsigned long plen = (unsigned long)strlen (prefix);

      if ((MAX_PATH - 4) <= plen)
        {
          (void)add_arg (pattern);

          return;
        }

      (void)memcpy (broad, prefix, plen);
      (void)memcpy (broad + plen, "*.*", 4);
    }
  else
    {
      (void)memcpy (broad, "*.*", 4);
    }

  Fsetdta ((void *)& dta);

  rc = Fsfirst ((char *)broad, ATTR_MASK);

  if (0 == rc)
    do
      {
        if (match_mask (mask, dta.d_fname))
          {
            char full [MAX_PATH];
            unsigned long plen = (unsigned long)strlen (prefix);
            unsigned long nlen = (unsigned long)strlen (dta.d_fname);
            unsigned long total = plen + nlen;

            if (MAX_PATH <= total)
              {
                if (MAX_PATH <= plen)
                  {
                    rc = Fsnext ();

                    continue;
                  }

                nlen = MAX_PATH - 1 - plen;
                total = plen + nlen;
              }

            (void)memcpy (full, prefix, plen);
            (void)memcpy (full + plen, dta.d_fname, nlen);
            full [total] = '\0';

            if (0 != add_arg (full))
              break;
          }

        rc = Fsnext ();
      }
    while (0 == rc);
  else
    (void)add_arg (pattern);
}

/******************************************************************************/

void
#ifdef ANSI_COMPILER
tos_expand_wildcards ( /*cppcheck-suppress unusedFunction*/
  int * argc,
  char * * * argv)
#else
tos_expand_wildcards (argc, argv) /*cppcheck-suppress unusedFunction*/
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
