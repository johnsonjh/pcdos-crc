/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - cpmdwc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 4eb55010-8131-11f1-a791-80ee73e9b8e7
 */

/******************************************************************************/

/*
 * Expands wildcards and modifies argc and argv for CP/M-80, CP/M-86, and
 * CP/M-68K builds, using direct BDOS Search First / Search Next calls.
 */

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

/*
 * BDOS parameter packing: Z88DK and Ack pass the FCB/DMA address as an int;
 * Aztec C on CP/M-86 (near 16-bit) and CP/M-68K (full pointer) accepts the
 * pointer directly.  HI-TECH C is varargs-friendly either way.
 */

#ifdef __Z88DK
# define BDOS_ARG(p) ((int)(p))
#else
# ifdef __ACK__
#  define BDOS_ARG(p) ((int)(p))
# else
#  define BDOS_ARG(p) (p)
# endif
#endif

/******************************************************************************/

/*
 * BDOS entry.  Z88DK provides it via <cpm.h>; other CP/M toolchains either
 * ship it in the C runtime (Aztec C86, HI-TECH) or link an external helper
 * (bdosack.s for Ack, bdos68k.a68 for Aztec C68K).
 */

#ifdef __Z88DK
# include <cpm.h>
#else
extern int bdos ();
#endif

/******************************************************************************/

#define MAX_ARGS 64
#define MAX_PATH 16

/******************************************************************************/

/* BDOS function numbers used here. */
#define BDOS_SET_DMA      26
#define BDOS_SEARCH_FIRST 17
#define BDOS_SEARCH_NEXT  18

/******************************************************************************/

static char argbuf [MAX_ARGS] [MAX_PATH];
static char * newargv [MAX_ARGS];
static int newargc;

/******************************************************************************/

static unsigned char dma_buf [128];
static unsigned char fcb [36];

/******************************************************************************/

/*
 * Local string helpers so this file does not depend on <string.h>, which is
 * missing or incomplete on several historical CP/M C compilers.
 */

static unsigned
#ifdef ANSI_COMPILER
cpm_strlen (
  /*cppcheck-suppress constParameterPointer*/
  char * s)
#else
cpm_strlen (s)
  /*cppcheck-suppress constParameterPointer*/
  char * s;
#endif
{
  unsigned n = 0;

  while ('\0' != * s)
    {
      n++;
      s++;
    }

  return n;
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
cpm_memcpy (
  char * dst,
  /*cppcheck-suppress constParameterPointer*/
  char * src,
  unsigned n)
#else
cpm_memcpy (dst, src, n)
  char * dst;
  /*cppcheck-suppress constParameterPointer*/
  char * src;
  unsigned n;
#endif
{
  while (0 != n)
    {
      * dst++ = * src++;
      n--;
    }
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
cpm_strcmp (
  /*cppcheck-suppress constParameterPointer*/
  char * a,
  /*cppcheck-suppress constParameterPointer*/
  char * b)
#else
cpm_strcmp (a, b)
  /*cppcheck-suppress constParameterPointer*/
  char * a;
  /*cppcheck-suppress constParameterPointer*/
  char * b;
#endif
{
  while ('\0' != * a && * a == * b)
    {
      a++;
      b++;
    }

  return (int)(unsigned char)* a - (int)(unsigned char)* b;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
add_arg (
  char * s)
#else
add_arg (s)
  char * s;
#endif
{
  unsigned len;
  int i;

  if (MAX_ARGS <= newargc)
    return -1;

  /* Skip duplicate names (multi-extent files share one FCB name). */
  i = 0;

  while (i < newargc)
    {
      if (0 == cpm_strcmp (argbuf [i], s))
        return 0;

      i++;
    }

  len = cpm_strlen (s);

  if (MAX_PATH <= len)
    len = MAX_PATH - 1;

  cpm_memcpy (argbuf [newargc], s, len);
  argbuf [newargc] [len] = '\0';
  newargv [newargc] = argbuf [newargc];
  newargc++;

  return 0;
}

/******************************************************************************/

/*
 * Build a CP/M FCB from a wildcard pattern.  Expands '*' into runs of '?' so
 * BDOS Search First / Search Next match correctly.  Optional "d:" drive
 * prefixes set FCB[0]; the caller keeps the drive letter for expanded names.
 */

static void
#ifdef ANSI_COMPILER
cpm_setfcb_wild (
  unsigned char * out_fcb,
  char * fn,
  char * drive_letter)
#else
cpm_setfcb_wild (out_fcb, fn, drive_letter)
  unsigned char * out_fcb;
  char * fn;
  char * drive_letter;
#endif
{
  char * p = fn;
  int i, c;

  for (i = 0; 36 > i; i++)
    out_fcb [i] = 0;

  for (i = 1; 11 >= i; i++)
    out_fcb [i] = ' ';

  * drive_letter = '\0';

  if ('\0' != p [0] && ':' == p [1])
    {
      c = (int)(unsigned char)p [0];

      if ('a' <= c && 'z' >= c)
        c -= 32;

      if ('A' <= c && 'P' >= c)
        {
          out_fcb [0] = (unsigned char)(c - 'A' + 1);
          * drive_letter = (char)c;
        }

      p += 2;
    }

  i = 1;

  while ('\0' != * p && '.' != * p && 8 >= i)
    {
      if ('*' == * p)
        {
          while (8 >= i)
            out_fcb [i++] = '?';

          break;
        }

      c = (int)(unsigned char)* p++;

      if ('a' <= c && 'z' >= c)
        c -= 32;

      out_fcb [i++] = (unsigned char)c;
    }

  while ('\0' != * p && '.' != * p)
    p++;

  if ('.' == * p)
    p++;

  i = 9;

  while ('\0' != * p && 11 >= i)
    {
      if ('*' == * p)
        {
          while (11 >= i)
            out_fcb [i++] = '?';

          break;
        }

      c = (int)(unsigned char)* p++;

      if ('a' <= c && 'z' >= c)
        c -= 32;

      out_fcb [i++] = (unsigned char)c;
    }
}

/******************************************************************************/

/*
 * Convert a 32-byte CP/M directory entry into a NUL-terminated filename
 * string (up to 12 characters: "FILENAME.EXT"), optionally prefixed with
 * a drive letter ("D:FILENAME.EXT").
 */

static void
#ifdef ANSI_COMPILER
cpm_dir_to_name (
  char * buf,
  /*cppcheck-suppress constParameterPointer*/
  unsigned char * entry,
  char drive_letter)
#else
cpm_dir_to_name (buf, entry, drive_letter)
  char * buf;
  /*cppcheck-suppress constParameterPointer*/
  unsigned char * entry;
  char drive_letter;
#endif
{
  int i, j, last, c, has_ext;

  j = 0;

  if ('\0' != drive_letter)
    {
      buf [j++] = drive_letter;
      buf [j++] = ':';
    }

  last = j;

  for (i = 1; 8 >= i; i++)
    {
      c = entry [i] & 0x7f;
      buf [j++] = (char)c;

      if (' ' != c)
        last = j;
    }

  j = last;
  has_ext = 0;

  for (i = 9; 11 >= i; i++)
    if (' ' != (entry [i] & 0x7f))
      has_ext = 1;

  if (0 != has_ext)
    {
      buf [j++] = '.';
      last = j;

      for (i = 9; 11 >= i; i++)
        {
          c = entry [i] & 0x7f;
          buf [j++] = (char)c;

          if (' ' != c)
            last = j;
        }

      j = last;
    }

  buf [j] = '\0';
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
expand_pattern (
  char * pattern)
#else
expand_pattern (pattern)
  char * pattern;
#endif
{
  char drive_letter;
  char name [MAX_PATH];
  int r;
  int found = 0;

  cpm_setfcb_wild (fcb, pattern, & drive_letter);

  (void)bdos (BDOS_SET_DMA, BDOS_ARG (dma_buf));

  r = bdos (BDOS_SEARCH_FIRST, BDOS_ARG (fcb)) & 0xff;

  while (0xff != r)
    {
      /*
       * Search First / Next return the directory-entry index (0..3) within
       * the 128-byte DMA buffer.  Ignore non-file entries (user number
       * 0xE5 = deleted; entries with the directory attribute bit set are
       * uncommon on classic CP/M but skip anything that is not a normal
       * file extent for the current user by trusting BDOS filtering).
       */
      if (3 >= r)
        {
          unsigned char * entry = & dma_buf [r * 32];

          /* Deleted directory slots should not appear, but be defensive. */
          if (0xe5 != entry [0])
            {
              cpm_dir_to_name (name, entry, drive_letter);

              if (0 != add_arg (name))
                return;

              found = 1;
            }
        }

      r = bdos (BDOS_SEARCH_NEXT, BDOS_ARG (fcb)) & 0xff;
    }

  /* No match: keep the original pattern so later open errors are visible. */
  if (0 == found)
    (void)add_arg (pattern);
}

/******************************************************************************/

void
#ifdef ANSI_COMPILER
cpm_expand_wildcards ( /*cppcheck-suppress unusedFunction*/
  int * argc,
  char * * * argv)
#else
cpm_expand_wildcards (argc, argv) /*cppcheck-suppress unusedFunction*/
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
      /*cppcheck-suppress constVariablePointer*/
      char * p = (* argv)[i];
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
