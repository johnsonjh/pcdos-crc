/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - ia16dwc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 5a1630f6-7edd-11f1-8995-80ee73e9b8e7
 */

/******************************************************************************/

/* Expands wildcards and modifies argc and argv for IA16-GCC for MS-DOS. */

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

#include <stddef.h>
#include <string.h>

/******************************************************************************/

#define MAX_ARGS 128
#define MAX_PATH 128

/******************************************************************************/

#define ATTR_MASK 0x21

/******************************************************************************/

struct dta
{
  unsigned char reserved [21];
  unsigned char attr;
  unsigned short time;
  unsigned short date;
  unsigned long size;
  char name [13];
} __attribute__((packed));

/******************************************************************************/

static struct dta dtabuf;

/******************************************************************************/

static char argbuf [MAX_ARGS] [MAX_PATH];
static char * newargv [MAX_ARGS];
static int newargc;

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
set_dta (
  struct const dta * buf)
#else
set_dta (buf)
  struct const dta * buf;
#endif
{
  unsigned short off = (unsigned short)buf;

  __asm__ __volatile__ (
    "mov $0x1A, %%ah\n"
    "mov %0, %%dx\n"
    "int $0x21\n"
    :
    : "r"(off)
    : "ax", "dx"
  );
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
find_first (
  const char * pattern,
  unsigned short attr)
#else
find_first (pattern, attr)
  const char * pattern;
  unsigned short attr;
#endif
{
  unsigned short ax;
  unsigned short off = (unsigned short)pattern;

  __asm__ __volatile__ (
    "mov $0x4E, %%ah\n"
    "mov %2, %%cx\n"
    "mov %1, %%dx\n"
    "int $0x21\n"
    "mov %%ax, %0\n"
    : "=r"(ax)
    : "r"(off), "r"(attr)
    : "ax", "cx", "dx"
  );

  return (int)ax;
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
  unsigned short ax;

  __asm__ __volatile__ (
    "mov $0x4F, %%ah\n"
    "int $0x21\n"
    "mov %%ax, %0\n"
    : "=r"(ax)
    :
    : "ax"
  );

  return (int)ax;
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
      long plen = 0L;
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
  char prefix [MAX_PATH];
  const char * mask;
  int rc;

  split_prefix (pattern, prefix, & mask);

  set_dta (& dtabuf);

  rc = find_first (pattern, (unsigned short)ATTR_MASK);

  if (0 == rc)
    do
      {
        char full [MAX_PATH];
        unsigned long plen = (unsigned long)strlen (prefix);
        unsigned long nlen = (unsigned long)strlen (dtabuf.name);
        unsigned long total = plen + nlen;

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
ia16_expand_wildcards ( /*cppcheck-suppress unusedFunction*/
  int * argc,
  char * * * argv)
#else
ia16_expand_wildcards (argc, argv) /*cppcheck-suppress unusedFunction*/
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
