/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator - crc.c
 * Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
 * SPDX-License-Identifier: MIT-0
 * scspell-id: 41561dd2-3fff-11f1-8e90-80ee73e9b8e7
 * //-V::1107
 */

/******************************************************************************/

/*
 * If you are NOT using an ANSI C89 compiler (e.g., K&R, "C86"),
 * COMMENT OUT "#define ANSI_COMPILER" below (or define NOANSI).
 */

#define ANSI_COMPILER

/******************************************************************************/

/*
 * If your environment does not provide the "errno.h" header,
 * COMMENT OUT "#define USE_ERRNO" below (or define NOERRNO).
 */

#define USE_ERRNO

/******************************************************************************/

/*
 * If your environment has a missing or broken "fread" function,
 * COMMENT OUT "#define USE_FREAD" below (or define NOFREAD).
 */

#define USE_FREAD

/******************************************************************************/

/* For more detailed information please review the README.md file. */

/******************************************************************************/

#ifdef MSDOS
# ifndef __MSDOS__
#  define __MSDOS__ MSDOS
# endif
#endif

/******************************************************************************/

#ifdef multics
typedef unsigned int crc_t;
#else
typedef unsigned long crc_t;
#endif

/******************************************************************************/

/*
 * MAX_CB_DIGITS is the number of maximum digits used for counting bits.
 *  ____________________
 *  | Value |   Size   |
 *  |-------|----------|
 *  |     8 | 11.9 MiB |
 *  |     9 |  119 MiB |
 *  |    10 | 1.16 GiB |
 *  |    11 | 11.6 GiB |
 *  |    12 |  117 GiB |
 *  |    13 | 1.14 TiB |
 *  |    14 | 11.4 TiB |
 *  |    15 |  114 TiB |
 *  --------------------
 */

/******************************************************************************/

#ifdef multics
# ifdef ANSI_COMPILER
#  undef ANSI_COMPILER
# endif
# ifdef USE_FREAD
#  undef USE_FREAD
# endif
# ifndef USE_PSYSERROR
#  define USE_PSYSERROR
# endif
# ifndef MAX_CB_DIGITS
#  define MAX_CB_DIGITS 9 /* ~119 MiB */
# endif
#endif

/******************************************************************************/

#ifdef __COMPILER_KCC__
# ifndef MAX_CB_DIGITS
#  define MAX_CB_DIGITS 9 /* ~119 MiB */
# endif
#endif

/******************************************************************************/

/*
 * NOTE: Borland Turbo C before 1.5 has a non-ANSI strerror, but every
 * version of Turbo C provides the `sys_errlist`/`sys_nerr` interface.
 */

#ifndef __BORLANDC__
# ifdef __TURBOC__
#  ifdef ANSI_COMPILER
#   undef ANSI_COMPILER
#  endif
#  ifndef USE_PSYSERROR
#   define USE_PSYSERROR
#  endif
# endif
#endif

/******************************************************************************/

#ifdef __BCC__
# ifdef ANSI_COMPILER
#  undef ANSI_COMPILER
# endif
# ifndef __MSDOS__
#  ifndef FORCE_STRERROR
#   define FORCE_STRERROR
#  endif
# endif
#endif

/******************************************************************************/

#ifdef __AZTEC_C_42T__
# ifndef NOSTRERROR
#  define NOSTRERROR
# endif
#endif

/******************************************************************************/

#ifdef NDEBUG
# ifdef DEBUG
#  undef DEBUG
# endif
#endif

/******************************************************************************/

#ifdef __FreeBSD__
# ifdef DEBUG
#  include <sys/param.h>
#  if !(__FreeBSD_version < 1000011)
const char * malloc_conf = "abort:true,confirm_conf:true,junk:true";
#  else
const char * malloc_conf = "JR";
#  endif
# endif
#endif

/******************************************************************************/

#ifdef __NetBSD__
# ifdef DEBUG
const char * malloc_conf = "abort:true,junk:true";
# endif
#endif

/******************************************************************************/

#ifdef __OpenBSD__
# ifdef DEBUG
const char * const malloc_options = "CFGJRU";
# else
const char * const malloc_options = "j";
# endif
#endif

/******************************************************************************/

#ifdef __ACK__
# ifdef CRC_CPM
#  ifndef MAX_CB_DIGITS
#   define MAX_CB_DIGITS 9 /* ~119 MiB (CP/M-Plus 3.0 allows 32 MiB files) */
#  endif
# endif
#endif

/******************************************************************************/

#ifdef HI_TECH_C
# ifdef CPM
#  ifndef HI_TECH_CPM
#   define HI_TECH_CPM
#  endif
#  ifdef ANSI_COMPILER
#   undef ANSI_COMPILER
#  endif
#  ifndef NOANSI
#   define NOANSI
#  endif
#  ifndef MAX_CB_DIGITS
#   define MAX_CB_DIGITS 9 /* ~119 MiB (CP/M-Plus 3.0 allows 32 MiB files) */
#  endif
# endif
#endif

/******************************************************************************/

#ifdef HI_TECH_C
# ifdef DOS
#  ifndef __PACIFIC__
#   define __PACIFIC__
#  endif
# endif
#endif

/******************************************************************************/

#ifdef __Z88DK
# ifdef __CPM__
#  include <unistd.h>
#  ifndef __SDCC
#   ifdef ANSI_COMPILER
#    undef ANSI_COMPILER
#   endif
#  else
#   ifndef NOSTRERROR
#    define NOSTRERROR
#   endif
#  endif
#  ifdef USE_PSYSERROR
#   undef USE_PSYSERROR
#  endif
#  ifndef BUFSIZ
#   define BUFSIZ 128
#  endif
#  ifndef NOFREAD
#   define NOFREAD
#  endif
#  ifndef MAX_CB_DIGITS
#   define MAX_CB_DIGITS 9 /* ~119 MiB (CP/M-Plus 3.0 allows 32 MiB files) */
#  endif
# endif
#endif

/******************************************************************************/

#ifndef MAX_CB_DIGITS
# define MAX_CB_DIGITS 12 /* ~117 GiB */
#endif

/******************************************************************************/

/*
 * CP/M Last Record Byte Count (LRBC) support.
 *
 * The --lrbc and --lrbc=isx options and the direct BDOS calls they rely on
 * are compiled only for CP/M targets: CP/M-80 via z88dk or HI-TECH C, and
 * CP/M-68K and CP/M-86 via Aztec C (the build passes -D__AZTEC_C_42T__ for
 * C86 4.2, -D__AZTEC_C_34T__ for C86 3.4, and -D__AZTEC_C_36T__ -D__CPM68K__
 * for C68K 3.6).  Define CRC_CPM on the command line to force it on elsewhere.
 */

/******************************************************************************/

#ifndef CRC_CPM
# ifdef __Z88DK
#  ifdef __CPM__
#   define CRC_CPM
#  endif
# endif
#endif

/******************************************************************************/

#ifndef CRC_CPM
# ifdef __AZTEC_C_42T__
#  define CRC_CPM
# endif
#endif

/******************************************************************************/

#ifndef CRC_CPM
# ifdef __AZTEC_C_34T__
#  define CRC_CPM
# endif
#endif

/******************************************************************************/

#ifndef CRC_CPM
# ifdef __AZTEC_C_36T__
#  ifdef __CPM68K__
#   define CRC_CPM
#  endif
# endif
#endif

/******************************************************************************/

#ifndef CRC_CPM
# ifdef HI_TECH_C
#  ifdef CPM
#   define CRC_CPM
#  endif
# endif
#endif

/******************************************************************************/

#ifndef CRC_NAME
# ifdef __MSDOS__
#  define CRC_NAME "CRC"
# else
#  ifdef CRC_CPM
#   define CRC_NAME "CRC"
#  else
#   define CRC_NAME "crc"
#  endif
# endif
#endif

/******************************************************************************/

#ifdef NOANSI
# ifdef ANSI_COMPILER
#  undef ANSI_COMPILER
# endif
#endif

/******************************************************************************/

#ifdef NOERRNO
# ifdef USE_ERRNO
#  undef USE_ERRNO
# endif
#endif

/******************************************************************************/

#ifdef NOFREAD
# ifdef USE_FREAD
#  undef USE_FREAD
# endif
#endif

/******************************************************************************/

#ifdef __has_include
# define HAS_INCLUDE(inc) __has_include(inc)
#else
# define HAS_INCLUDE(inc) 0
#endif

/******************************************************************************/

#ifndef __CPPCHECK__
# ifndef multics
#  if HAS_INCLUDE(<unistd.h>)
#   include <unistd.h>
#  endif
#  if HAS_INCLUDE(<sys/stat.h>)
#   ifndef HAVE_SYS_STAT
#    define HAVE_SYS_STAT
#   endif
#  endif
# endif
#endif

/******************************************************************************/

#ifdef _POSIX_VERSION
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __unix
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __UNIX__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __unix__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef unix
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __PACIFIC__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __HAIKU__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __WATCOMC__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __DJGPP__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __APPLE__
# ifdef __MACH__
#  ifndef HAVE_SYS_STAT
#   define HAVE_SYS_STAT
#  endif
# endif
#endif

/******************************************************************************/

#ifdef __NeXT__
# ifdef __MACH__
#  ifndef HAVE_SYS_STAT
#   define HAVE_SYS_STAT
#  endif
# endif
#endif

/******************************************************************************/

#ifdef _AIX
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __ORANGEC__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef C86PLUS
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef _MSC_VER
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __DMC__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __ZTC__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __SC__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __BORLANDC__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __IMPORTC__
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef _CH_
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef __ACK__
# ifndef NO_SYS_STAT
#  define NO_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifdef NO_SYS_STAT
# ifdef HAVE_SYS_STAT
#  undef HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#include <stdio.h>

/******************************************************************************/

#ifndef multics
# ifdef __BCC__
#  ifdef __MSDOS__
#   ifdef BCC_DOS
#    undef BCC_DOS
#   endif
#   define BCC_DOS __MSDOS__
#   undef __MSDOS__
#  endif
# endif
# ifndef NOSTDLIB
#  include <stdlib.h>
# endif
# ifdef BCC_DOS
#  ifndef __MSDOS__
#   define __MSDOS__ BCC_DOS
#  endif
# endif
#endif

/******************************************************************************/

#ifdef USE_ERRNO
# ifndef HI_TECH_CPM /* HI-TECH has errno but no errno.h */
#  include <errno.h>
# endif
#else
# ifdef errno
#  undef errno
# endif
# define errno 0
#endif

/******************************************************************************/

#ifdef ANSI_COMPILER
# ifndef NOSTRERROR
#  include <string.h>
# endif
#else
# ifdef FORCE_STRERROR
#  ifndef NO_DCL_STRERROR
extern char * strerror ();
#  endif
# endif
#endif

/******************************************************************************/

#ifdef HAVE_SYS_STAT
# ifdef __PACIFIC__
#  include <stat.h>
# else
#  include <sys/types.h>
#  include <sys/stat.h>
# endif
#endif

/******************************************************************************/

#ifdef multics
# include <types.h>
# include <stat.h>
# ifndef HAVE_SYS_STAT
#  define HAVE_SYS_STAT
# endif
#endif

/******************************************************************************/

#ifndef __BORLANDC__
# ifdef __TURBOC__
#  include <sys/stat.h>
#  ifndef HAVE_SYS_STAT
#   define HAVE_SYS_STAT
#  endif
# endif
#endif

/******************************************************************************/

#ifndef S_ISDIR
# ifndef S_IFMT
#  ifdef _S_IFMT
#   define S_IFMT _S_IFMT
#  endif
# endif
# ifndef S_IFDIR
#  ifdef _S_IFDIR
#   define S_IFDIR _S_IFDIR
#  endif
# endif
# ifdef S_IFMT
#  ifdef S_IFDIR
#   define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#  endif
# endif
#endif

/******************************************************************************/

#ifndef ANSI_COMPILER
# ifndef __CPPCHECK__
#  ifndef USE_CONST
#   ifdef const
#    undef const
#   endif
#   define const /* //-V1059 */
#  endif
# endif
#endif

/******************************************************************************/

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif

/******************************************************************************/

#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

/******************************************************************************/

#ifdef LINT
# ifndef __LINT__
#  define __LINT__ LINT
# endif
#endif

/******************************************************************************/

#ifdef _MSC_VER
# ifdef __MSDOS__
#  ifndef DOS_MSC_VER
#   define DOS_MSC_VER
#  endif
# endif
#endif

/******************************************************************************/

#ifdef DOS_MSC_VER
# if _MSC_VER > 600
#  include "suppress.h"
# endif
#endif

/******************************************************************************/

#ifdef __CPM68K__
# ifdef __AZTEC_C_36T__
#  ifndef CRC_EOL
#   ifndef CRC_EOL_CRLF
#    define CRC_EOL_CRLF
#   endif
#  endif
# endif
#endif

/******************************************************************************/

#ifndef CRC_EOL
# ifdef CRC_EOL_CRLF
#  define CRC_EOL "\r\n"
# else
#  define CRC_EOL "\n"
# endif
#endif

/******************************************************************************/

typedef struct {
  unsigned char d [MAX_CB_DIGITS];
} counter_t;

/******************************************************************************/

static int g_anyerr = 0;
static int g_fileerr = 0;
static int g_verbose = 0;
static int g_bits_auto = 0;
static int g_pad_auto = 0;
static long g_out_err = 0;
static unsigned char g_buf [BUFSIZ];

/******************************************************************************/

#ifdef CRC_CPM
static int g_cpm_lrbc = 0;
static int g_cpm_lrbc_explicit = 0;
static int g_lrbc_warned = 0;
#endif

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
out_err_check_int (
  int ret)
#else
out_err_check_int (ret)
  int ret;
#endif
{
  if (0 > ret)
    g_out_err++;

#ifdef __VBCC__
  if (0 != fflush (stdout))
    g_out_err++;

  if (0 != fflush (stderr))
    g_out_err++;
#endif
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
out_err_check_fputc (
  int ret)
#else
out_err_check_fputc (ret)
  int ret;
#endif
{
  if (EOF == ret)
    g_out_err++;

#ifdef __VBCC__
  if (0 != fflush (stdout))
    g_out_err++;

  if (0 != fflush (stderr))
    g_out_err++;
#endif
}

/******************************************************************************/

static const char hexdigits [] = "0123456789ABCDEF";
static const char alpuppers [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char alplowers [] = "abcdefghijklmnopqrstuvwxyz";

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
cb_zero (
  counter_t * const c)
#else
cb_zero (c)
  counter_t * const c;
#endif
{
#ifdef GCC_ANALYZER
  (void)__builtin_memset (c -> d, 0, sizeof (c -> d));
#else
  int i;

  for (i = 0; MAX_CB_DIGITS > i; i++)
    c -> d [i] = 0;
#endif
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
cb_is_zero (
  const counter_t * const c)
#else
cb_is_zero (c)
  const counter_t * const c;
#endif
{
  int i;

  for (i = 0; MAX_CB_DIGITS > i; i++)
    if (0 != c -> d [i])
      return 0;

  return 1;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
cb_is_one (
  const counter_t * const c)
#else
cb_is_one (c)
  const counter_t * const c;
#endif
{
  int i;

  if (1 != c -> d [0])
    return 0;

  for (i = 1; MAX_CB_DIGITS > i; i++)
    if (0 != c -> d [i])
      return 0;

  return 1;
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
cb_copy (
  counter_t * const dst,
  const counter_t * const src)
#else
cb_copy (dst, src)
  counter_t * const dst;
  const counter_t * const src;
#endif
{
#ifdef GCC_ANALYZER
  (void)__builtin_memcpy (dst -> d, src -> d, sizeof (dst -> d));
#else
  int i;

  for (i = 0; MAX_CB_DIGITS > i; i++)
    dst -> d [i] = src -> d [i];
#endif
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
cb_add (
  counter_t * const c,
  const unsigned int v)
#else
cb_add (c, v)
  counter_t * const c;
  const unsigned int v;
#endif
{
  int i = 0;
  unsigned int carry = v;

  while (0 != carry) {
    if (MAX_CB_DIGITS <= i)
      return 0;

    carry += (unsigned int)c -> d [i];

    {
      unsigned int q = 0;
      unsigned int r = carry;

      /* loop is division-free; fast only for small v */
      while (10 <= r) {
        r -= 10;
        q++;
      }

      c -> d [i] = (unsigned char)r;
      carry = q;
    }

    i++;
  }

  return 1;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
cb_sub (
  counter_t * const c,
  const unsigned int v)
#else
cb_sub (c, v)
  counter_t * const c;
  const unsigned int v;
#endif
{
  int i;
  unsigned int val = v;
  unsigned char d [MAX_CB_DIGITS];

  for (i = 0; MAX_CB_DIGITS > i; i++)
    d [i] = c -> d [i];

  i = 0;

  while (0 != val) {
    unsigned int digit_to_sub;

    {
      unsigned int q = 0;
      unsigned int r = val;

      /* loop is division-free; fast only for small v */
      while (10 <= r) {
        r -= 10;
        q++;
      }

      digit_to_sub = r;
      val = q;
    }

    if (MAX_CB_DIGITS <= i)
      return 0;

    if (d [i] >= digit_to_sub)
      d [i] = (unsigned char)(d [i] - (unsigned char)digit_to_sub);
    else {
      int j = i + 1;

      while (MAX_CB_DIGITS > j && 0 == d [j]) {
        d [j] = 9;
        j++;
      }

      if (MAX_CB_DIGITS <= j)
        return 0;

      d [j]--;
      d [i] = (unsigned char)(10 + d [i] - digit_to_sub);
    }

    i++;
  }

  for (i = 0; MAX_CB_DIGITS > i; i++)
    c -> d [i] = d [i];

  return 1;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
cb_cmp (
  const counter_t * const a,
  const counter_t * const b)
#else
cb_cmp (a, b)
  const counter_t * const a;
  const counter_t * const b;
#endif
{
  int i;

  for (i = MAX_CB_DIGITS - 1; 0 <= i; i--) {
    if (a -> d [i] > b -> d [i])
      return 1;

    if (a -> d [i] < b -> d [i])
      return -1;
  }

  return 0;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
cb_mul (
  counter_t * const c,
  const unsigned int v)
#else
cb_mul (c, v)
  counter_t * const c;
  const unsigned int v;
#endif
{
  int i;
  unsigned int carry = 0;

  for (i = 0; MAX_CB_DIGITS > i; i++) {
    const unsigned int val = (unsigned int)c -> d [i] * v + carry;
    unsigned int q = 0;
    unsigned int r = val;

    while (10 <= r) {
      r -= 10;
      q++;
    }

    c -> d [i] = (unsigned char)r;
    carry = q;
  }

  if (0 != carry)
    return 0;

  return 1;
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
cb_sub_counter (
  counter_t * const a,
  const counter_t * const b)
#else
cb_sub_counter (a, b)
  counter_t * const a;
  const counter_t * const b;
#endif
{
  int i;
  int borrow = 0;

  for (i = 0; MAX_CB_DIGITS > i; i++) {
    const int val_a = (int)a -> d [i];
    const int val_b = (int)b -> d [i];
    int diff = val_a - val_b - borrow;

    if (0 > diff) {
      diff += 10;
      borrow = 1;
    } else
      borrow = 0;

    a -> d [i] = (unsigned char)diff;
  }
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
cb_printf (
  FILE * const fp,
  const counter_t * const c)
#else
cb_printf (fp, c)
  FILE * const fp;
  const counter_t * const c;
#endif
{
  int i = MAX_CB_DIGITS - 1;

  while (0 < i && 0 == c -> d [i])
    i--;

  for (; 0 <= i; i--) {
    const int ch = hexdigits [c -> d [i]];

    if (NULL != fp)
      out_err_check_fputc (
        fputc (ch,
          fp
        )
      );
    else
      out_err_check_int (
        printf ("%c",
          ch
        )
      );
  }
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
cb_parse (
  counter_t * const c,
  const char * const s)
#else
cb_parse (c, s)
  counter_t * const c;
  const char * const s;
#endif
{
  int i;
  /*cppcheck-suppress constStatement*/
  const char * p;

  if ((const char *)0 == s || '\0' == * s)
    return 0;

  cb_zero (c);

  p = s;

  while ('\0' != * p) {
    int digit = -1;

    for (i = 0; 10 > i; i++)
      if (hexdigits [i] == * p) {
        digit = i;

        break;
      }

    if (0 > digit)
      return 0;

    {
      unsigned int carry = (unsigned int)digit;
      int j;

      for (j = 0; MAX_CB_DIGITS > j; j++) {
        const unsigned int val = (unsigned int)c -> d [j] * 10 + carry;
        unsigned int q = 0;
        unsigned int r = val;

        while (10 <= r) {
          r -= 10;
          q++;
        }

        c -> d [j] = (unsigned char)r;
        carry = q;
      }

      if (0 != carry)
        return 0;
    }

    p++;
  }

  return 1;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
xfold (
  const int c)
#else
xfold (c)
  const int c;
#endif
{
  int i;

  for (i = 0; 26 > i; i++)
    if ((int)(unsigned char)alplowers [i] == c)
      return (int)(unsigned char)alpuppers [i];

  return c;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
xstrncasecmp (
  const char * s1,
  const char * s2,
  int n)
#else
xstrncasecmp (s1, s2, n)
  const char * s1;
  const char * s2;
  int n;
#endif
{
  int c1, c2;
  int ret = 0;

  if (0 >= n)
    goto done;

  for (;;) {
    c1 = xfold ((unsigned char)* s1++);
    c2 = xfold ((unsigned char)* s2++);

    if (c1 != c2) {
      ret = ((c1 < c2) ? -1 : 1);

      goto done;
    }

    if ('\0' == c1)
      goto done;

    if (0 >= --n)
      goto done;
  }

done:
  return ret;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
xstrcasecmp (
  const char * s1,
  const char * s2)
#else
xstrcasecmp (s1, s2)
  const char * s1;
  const char * s2;
#endif
{
  int c1, c2;
  int ret = 0;

  for (;;) {
    c1 = xfold ((unsigned char)* s1++);
    c2 = xfold ((unsigned char)* s2++);

    if (c1 != c2) {
      ret = ((c1 < c2) ? -1 : 1);

      goto done;
    }

    if ('\0' == c1)
      goto done;
  }

done:
  return ret;
}

/******************************************************************************/

#ifdef __Z88DK
# ifdef __CPM__
#  ifndef CPM_WILDCARD
#   define CPM_WILDCARD
#  endif
# endif
#endif

/******************************************************************************/

#ifdef HI_TECH_CPM
# ifndef CPM_WILDCARD
#  define CPM_WILDCARD
# endif
# ifndef CPM_BDOS_WILDCARD
#  define CPM_BDOS_WILDCARD
# endif
#endif

/******************************************************************************/

#ifdef __ACK__
# ifdef CRC_CPM
#  ifndef CPM_WILDCARD
#   define CPM_WILDCARD
#  endif
#  ifndef CPM_BDOS_WILDCARD
#   define CPM_BDOS_WILDCARD
#  endif
# endif
#endif

/******************************************************************************/

#ifdef CPM_WILDCARD
static int
# ifdef ANSI_COMPILER
is_wildcard (
  const char * s)
# else
is_wildcard (s)
  const char * s;
# endif
{
  int c;

  if ((const char *)0 == s)
    return 0;

  while ('\0' != (c = * s++))
    if ('*' == c || '?' == c)
      return 1;

  return 0;
}
#endif

/******************************************************************************/

#ifdef USE_PSYSERROR
static int
# ifdef ANSI_COMPILER
pdiv10 (
  unsigned int * const u)
# else
pdiv10 (u)
  unsigned int * const u;
# endif
{
  unsigned int q = 0;
  unsigned int r = * u;

  while (10 <= r) {
    r -= 10;
    q++;
  }

  * u = q;

  return (int)r;
}
#endif

/******************************************************************************/

#ifdef USE_PSYSERROR
extern char * sys_errlist [];
extern int sys_nerr;

static const char *
# ifdef ANSI_COMPILER
psyserror (
  const int n)
# else
psyserror (n)
  const int n;
# endif
{
  static char buf [64];
  char tmp [48];
  char * q;
  int i, neg;
  unsigned int u;

  if (0 <= n && sys_nerr > n) {
    const char * p = sys_errlist [n];

    if ((const char *)0 != p)
      return p;
  }

  q = buf;
  * q++ = 'E';
  * q++ = 'r';
  * q++ = 'r';
  * q++ = 'o';
  * q++ = 'r';
  * q++ = ' ';

  neg = 0;

  if (0 > n) {
    neg = 1;
    u = (unsigned int)(-(n + 1)) + 1;
  } else
    u = (unsigned int)n;

  i = 0;

  do
    tmp [i++] = hexdigits [pdiv10 (& u)];
  while (0 != u && (int)sizeof (tmp) > i);

  if (0 != neg)
    * q++ = '-';

  while (0 < i) {
    i--;
    * q++ = tmp [i];
  }

  * q = '\0';

  return buf;
}
#endif

/******************************************************************************/

#ifndef ANSI_COMPILER
# ifndef FORCE_STRERROR
#  ifndef USE_PSYSERROR
#   ifndef USE_TRIM
#    define NO_TRIM
#   endif
#  endif
# endif
#endif

#ifndef NO_TRIM
# define TRIM_BUFSIZE 256
# define TRIM_RING 2 /* max reentrancy depth */

static char *
# ifdef ANSI_COMPILER
trim_str (
  const char * const s)
# else
trim_str (s)
  const char * const s;
# endif
{
  /*cppcheck-suppress constVariable*/
  static char bufs [TRIM_RING] [TRIM_BUFSIZE];
  static int idx = 0;

  /*cppcheck-suppress constStatement*/
  const char * p;
  /*cppcheck-suppress constStatement*/
  const char * q;
  /*cppcheck-suppress constStatement*/
  const char * last;

  char * buf;
  char * d;

  buf = bufs [idx];
  idx++;

  if (idx >= TRIM_RING)
    idx = 0;

  if (0 == s) {
    buf [0] = '\0';

    return buf;
  }

  p = s;

  while (' ' == * p || '\t' == * p || '\r' == * p || '\n' == * p)
    p++;

  if ('\0' == * p) {
    buf [0] = '\0';

    return buf;
  }

  q = p;
  last = p;

  while ('\0' != * q) {
    if (' ' != * q && '\t' != * q && '\r' != * q && '\n' != * q)
      last = q;

    q++;
  }

  d = buf;

  while (p <= last && d < buf + (TRIM_BUFSIZE - 1)) {
    if ('\r' == * p || '\n' == * p)
      * d++ = ' ';
    else
      * d++ = * p;

    p++;
  }

  * d = '\0';

  return buf;
}
#endif

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
error_msg (
  const char * m,
  const char * n,
  const int e)
#else
error_msg (m, n, e)
  const char * m;
  const char * n;
  const int e;
#endif
{
  out_err_check_int (
    fprintf (stderr, "ERROR: %s",
      m
    )
  );

  if (NULL != n)
    out_err_check_int (
      fprintf (stderr, " %s",
        n
      )
    );

  if (0 != e) {
    out_err_check_int (
      fprintf (stderr, " (Error %d",
        e
      )
    );
#ifdef ANSI_COMPILER
# ifndef NOSTRERROR
    out_err_check_int (
      fprintf (stderr, ": %s",
        trim_str (strerror (e))
      )
    );
# endif
#else
# ifdef FORCE_STRERROR
    out_err_check_int (
      fprintf (stderr, ": %s",
        trim_str (strerror (e))
      )
    );
# else
#  ifdef USE_PSYSERROR
    out_err_check_int (
      fprintf (stderr, ": %s",
        trim_str (psyserror (e))
      )
    );
#  endif
# endif
#endif
    out_err_check_int (
      fprintf (stderr, ")")
    );
  }

  out_err_check_int (
    fprintf (stderr, ".%s",
      CRC_EOL
    )
  );

  g_anyerr = 1;
  g_fileerr = 1;
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
init_crc_table (
  crc_t * const tbl)
#else
init_crc_table (tbl)
  crc_t * const tbl;
#endif
{
  tbl [0] = (crc_t)0x00000000;
  tbl [1] = (crc_t)0x51F9D3DE;
  tbl [2] = (crc_t)0xA3F3A7BC;
  tbl [3] = (crc_t)0xF20A7462;
  tbl [4] = (crc_t)0x161E9CA6;
  tbl [5] = (crc_t)0x47E74F78;
  tbl [6] = (crc_t)0xB5ED3B1A;
  tbl [7] = (crc_t)0xE414E8C4;
  tbl [8] = (crc_t)0x2C3D394C;
  tbl [9] = (crc_t)0x7DC4EA92;
  tbl [10] = (crc_t)0x8FCE9EF0;
  tbl [11] = (crc_t)0xDE374D2E;
  tbl [12] = (crc_t)0x3A23A5EA;
  tbl [13] = (crc_t)0x6BDA7634;
  tbl [14] = (crc_t)0x99D00256;
  tbl [15] = (crc_t)0xC829D188;
  tbl [16] = (crc_t)0x587A7298;
  tbl [17] = (crc_t)0x0983A146;
  tbl [18] = (crc_t)0xFB89D524;
  tbl [19] = (crc_t)0xAA7006FA;
  tbl [20] = (crc_t)0x4E64EE3E;
  tbl [21] = (crc_t)0x1F9D3DE0;
  tbl [22] = (crc_t)0xED974982;
  tbl [23] = (crc_t)0xBC6E9A5C;
  tbl [24] = (crc_t)0x74474BD4;
  tbl [25] = (crc_t)0x25BE980A;
  tbl [26] = (crc_t)0xD7B4EC68;
  tbl [27] = (crc_t)0x864D3FB6;
  tbl [28] = (crc_t)0x6259D772;
  tbl [29] = (crc_t)0x33A004AC;
  tbl [30] = (crc_t)0xC1AA70CE;
  tbl [31] = (crc_t)0x9053A310;
  tbl [32] = (crc_t)0xB0F4E530;
  tbl [33] = (crc_t)0xE10D36EE;
  tbl [34] = (crc_t)0x1307428C;
  tbl [35] = (crc_t)0x42FE9152;
  tbl [36] = (crc_t)0xA6EA7996;
  tbl [37] = (crc_t)0xF713AA48;
  tbl [38] = (crc_t)0x0519DE2A;
  tbl [39] = (crc_t)0x54E00DF4;
  tbl [40] = (crc_t)0x9CC9DC7C;
  tbl [41] = (crc_t)0xCD300FA2;
  tbl [42] = (crc_t)0x3F3A7BC0;
  tbl [43] = (crc_t)0x6EC3A81E;
  tbl [44] = (crc_t)0x8AD740DA;
  tbl [45] = (crc_t)0xDB2E9304;
  tbl [46] = (crc_t)0x2924E766;
  tbl [47] = (crc_t)0x78DD34B8;
  tbl [48] = (crc_t)0xE88E97A8;
  tbl [49] = (crc_t)0xB9774476;
  tbl [50] = (crc_t)0x4B7D3014;
  tbl [51] = (crc_t)0x1A84E3CA;
  tbl [52] = (crc_t)0xFE900B0E;
  tbl [53] = (crc_t)0xAF69D8D0;
  tbl [54] = (crc_t)0x5D63ACB2;
  tbl [55] = (crc_t)0x0C9A7F6C;
  tbl [56] = (crc_t)0xC4B3AEE4;
  tbl [57] = (crc_t)0x954A7D3A;
  tbl [58] = (crc_t)0x67400958;
  tbl [59] = (crc_t)0x36B9DA86;
  tbl [60] = (crc_t)0xD2AD3242;
  tbl [61] = (crc_t)0x8354E19C;
  tbl [62] = (crc_t)0x715E95FE;
  tbl [63] = (crc_t)0x20A74620;
  tbl [64] = (crc_t)0x301019BE;
  tbl [65] = (crc_t)0x61E9CA60;
  tbl [66] = (crc_t)0x93E3BE02;
  tbl [67] = (crc_t)0xC21A6DDC;
  tbl [68] = (crc_t)0x260E8518;
  tbl [69] = (crc_t)0x77F756C6;
  tbl [70] = (crc_t)0x85FD22A4;
  tbl [71] = (crc_t)0xD404F17A;
  tbl [72] = (crc_t)0x1C2D20F2;
  tbl [73] = (crc_t)0x4DD4F32C;
  tbl [74] = (crc_t)0xBFDE874E;
  tbl [75] = (crc_t)0xEE275490;
  tbl [76] = (crc_t)0x0A33BC54;
  tbl [77] = (crc_t)0x5BCA6F8A;
  tbl [78] = (crc_t)0xA9C01BE8;
  tbl [79] = (crc_t)0xF839C836;
  tbl [80] = (crc_t)0x686A6B26;
  tbl [81] = (crc_t)0x3993B8F8;
  tbl [82] = (crc_t)0xCB99CC9A;
  tbl [83] = (crc_t)0x9A601F44;
  tbl [84] = (crc_t)0x7E74F780;
  tbl [85] = (crc_t)0x2F8D245E;
  tbl [86] = (crc_t)0xDD87503C;
  tbl [87] = (crc_t)0x8C7E83E2;
  tbl [88] = (crc_t)0x4457526A;
  tbl [89] = (crc_t)0x15AE81B4;
  tbl [90] = (crc_t)0xE7A4F5D6;
  tbl [91] = (crc_t)0xB65D2608;
  tbl [92] = (crc_t)0x5249CECC;
  tbl [93] = (crc_t)0x03B01D12;
  tbl [94] = (crc_t)0xF1BA6970;
  tbl [95] = (crc_t)0xA043BAAE;
  tbl [96] = (crc_t)0x80E4FC8E;
  tbl [97] = (crc_t)0xD11D2F50;
  tbl [98] = (crc_t)0x23175B32;
  tbl [99] = (crc_t)0x72EE88EC;
  tbl [100] = (crc_t)0x96FA6028;
  tbl [101] = (crc_t)0xC703B3F6;
  tbl [102] = (crc_t)0x3509C794;
  tbl [103] = (crc_t)0x64F0144A;
  tbl [104] = (crc_t)0xACD9C5C2;
  tbl [105] = (crc_t)0xFD20161C;
  tbl [106] = (crc_t)0x0F2A627E;
  tbl [107] = (crc_t)0x5ED3B1A0;
  tbl [108] = (crc_t)0xBAC75964;
  tbl [109] = (crc_t)0xEB3E8ABA;
  tbl [110] = (crc_t)0x1934FED8;
  tbl [111] = (crc_t)0x48CD2D06;
  tbl [112] = (crc_t)0xD89E8E16;
  tbl [113] = (crc_t)0x89675DC8;
  tbl [114] = (crc_t)0x7B6D29AA;
  tbl [115] = (crc_t)0x2A94FA74;
  tbl [116] = (crc_t)0xCE8012B0;
  tbl [117] = (crc_t)0x9F79C16E;
  tbl [118] = (crc_t)0x6D73B50C;
  tbl [119] = (crc_t)0x3C8A66D2;
  tbl [120] = (crc_t)0xF4A3B75A;
  tbl [121] = (crc_t)0xA55A6484;
  tbl [122] = (crc_t)0x575010E6;
  tbl [123] = (crc_t)0x06A9C338;
  tbl [124] = (crc_t)0xE2BD2BFC;
  tbl [125] = (crc_t)0xB344F822;
  tbl [126] = (crc_t)0x414E8C40;
  tbl [127] = (crc_t)0x10B75F9E;
  tbl [128] = (crc_t)0x6020337C;
  tbl [129] = (crc_t)0x31D9E0A2;
  tbl [130] = (crc_t)0xC3D394C0;
  tbl [131] = (crc_t)0x922A471E;
  tbl [132] = (crc_t)0x763EAFDA;
  tbl [133] = (crc_t)0x27C77C04;
  tbl [134] = (crc_t)0xD5CD0866;
  tbl [135] = (crc_t)0x8434DBB8;
  tbl [136] = (crc_t)0x4C1D0A30;
  tbl [137] = (crc_t)0x1DE4D9EE;
  tbl [138] = (crc_t)0xEFEEAD8C;
  tbl [139] = (crc_t)0xBE177E52;
  tbl [140] = (crc_t)0x5A039696;
  tbl [141] = (crc_t)0x0BFA4548;
  tbl [142] = (crc_t)0xF9F0312A;
  tbl [143] = (crc_t)0xA809E2F4;
  tbl [144] = (crc_t)0x385A41E4;
  tbl [145] = (crc_t)0x69A3923A;
  tbl [146] = (crc_t)0x9BA9E658;
  tbl [147] = (crc_t)0xCA503586;
  tbl [148] = (crc_t)0x2E44DD42;
  tbl [149] = (crc_t)0x7FBD0E9C;
  tbl [150] = (crc_t)0x8DB77AFE;
  tbl [151] = (crc_t)0xDC4EA920;
  tbl [152] = (crc_t)0x146778A8;
  tbl [153] = (crc_t)0x459EAB76;
  tbl [154] = (crc_t)0xB794DF14;
  tbl [155] = (crc_t)0xE66D0CCA;
  tbl [156] = (crc_t)0x0279E40E;
  tbl [157] = (crc_t)0x538037D0;
  tbl [158] = (crc_t)0xA18A43B2;
  tbl [159] = (crc_t)0xF073906C;
  tbl [160] = (crc_t)0xD0D4D64C;
  tbl [161] = (crc_t)0x812D0592;
  tbl [162] = (crc_t)0x732771F0;
  tbl [163] = (crc_t)0x22DEA22E;
  tbl [164] = (crc_t)0xC6CA4AEA;
  tbl [165] = (crc_t)0x97339934;
  tbl [166] = (crc_t)0x6539ED56;
  tbl [167] = (crc_t)0x34C03E88;
  tbl [168] = (crc_t)0xFCE9EF00;
  tbl [169] = (crc_t)0xAD103CDE;
  tbl [170] = (crc_t)0x5F1A48BC;
  tbl [171] = (crc_t)0x0EE39B62;
  tbl [172] = (crc_t)0xEAF773A6;
  tbl [173] = (crc_t)0xBB0EA078;
  tbl [174] = (crc_t)0x4904D41A;
  tbl [175] = (crc_t)0x18FD07C4;
  tbl [176] = (crc_t)0x88AEA4D4;
  tbl [177] = (crc_t)0xD957770A;
  tbl [178] = (crc_t)0x2B5D0368;
  tbl [179] = (crc_t)0x7AA4D0B6;
  tbl [180] = (crc_t)0x9EB03872;
  tbl [181] = (crc_t)0xCF49EBAC;
  tbl [182] = (crc_t)0x3D439FCE;
  tbl [183] = (crc_t)0x6CBA4C10;
  tbl [184] = (crc_t)0xA4939D98;
  tbl [185] = (crc_t)0xF56A4E46;
  tbl [186] = (crc_t)0x07603A24;
  tbl [187] = (crc_t)0x5699E9FA;
  tbl [188] = (crc_t)0xB28D013E;
  tbl [189] = (crc_t)0xE374D2E0;
  tbl [190] = (crc_t)0x117EA682;
  tbl [191] = (crc_t)0x4087755C;
  tbl [192] = (crc_t)0x50302AC2;
  tbl [193] = (crc_t)0x01C9F91C;
  tbl [194] = (crc_t)0xF3C38D7E;
  tbl [195] = (crc_t)0xA23A5EA0;
  tbl [196] = (crc_t)0x462EB664;
  tbl [197] = (crc_t)0x17D765BA;
  tbl [198] = (crc_t)0xE5DD11D8;
  tbl [199] = (crc_t)0xB424C206;
  tbl [200] = (crc_t)0x7C0D138E;
  tbl [201] = (crc_t)0x2DF4C050;
  tbl [202] = (crc_t)0xDFFEB432;
  tbl [203] = (crc_t)0x8E0767EC;
  tbl [204] = (crc_t)0x6A138F28;
  tbl [205] = (crc_t)0x3BEA5CF6;
  tbl [206] = (crc_t)0xC9E02894;
  tbl [207] = (crc_t)0x9819FB4A;
  tbl [208] = (crc_t)0x084A585A;
  tbl [209] = (crc_t)0x59B38B84;
  tbl [210] = (crc_t)0xABB9FFE6;
  tbl [211] = (crc_t)0xFA402C38;
  tbl [212] = (crc_t)0x1E54C4FC;
  tbl [213] = (crc_t)0x4FAD1722;
  tbl [214] = (crc_t)0xBDA76340;
  tbl [215] = (crc_t)0xEC5EB09E;
  tbl [216] = (crc_t)0x24776116;
  tbl [217] = (crc_t)0x758EB2C8;
  tbl [218] = (crc_t)0x8784C6AA;
  tbl [219] = (crc_t)0xD67D1574;
  tbl [220] = (crc_t)0x3269FDB0;
  tbl [221] = (crc_t)0x63902E6E;
  tbl [222] = (crc_t)0x919A5A0C;
  tbl [223] = (crc_t)0xC06389D2;
  tbl [224] = (crc_t)0xE0C4CFF2;
  tbl [225] = (crc_t)0xB13D1C2C;
  tbl [226] = (crc_t)0x4337684E;
  tbl [227] = (crc_t)0x12CEBB90;
  tbl [228] = (crc_t)0xF6DA5354;
  tbl [229] = (crc_t)0xA723808A;
  tbl [230] = (crc_t)0x5529F4E8;
  tbl [231] = (crc_t)0x04D02736;
  tbl [232] = (crc_t)0xCCF9F6BE;
  tbl [233] = (crc_t)0x9D002560;
  tbl [234] = (crc_t)0x6F0A5102;
  tbl [235] = (crc_t)0x3EF382DC;
  tbl [236] = (crc_t)0xDAE76A18;
  tbl [237] = (crc_t)0x8B1EB9C6;
  tbl [238] = (crc_t)0x7914CDA4;
  tbl [239] = (crc_t)0x28ED1E7A;
  tbl [240] = (crc_t)0xB8BEBD6A;
  tbl [241] = (crc_t)0xE9476EB4;
  tbl [242] = (crc_t)0x1B4D1AD6;
  tbl [243] = (crc_t)0x4AB4C908;
  tbl [244] = (crc_t)0xAEA021CC;
  tbl [245] = (crc_t)0xFF59F212;
  tbl [246] = (crc_t)0x0D538670;
  tbl [247] = (crc_t)0x5CAA55AE;
  tbl [248] = (crc_t)0x94838426;
  tbl [249] = (crc_t)0xC57A57F8;
  tbl [250] = (crc_t)0x3770239A;
  tbl [251] = (crc_t)0x6689F044;
  tbl [252] = (crc_t)0x829D1880;
  tbl [253] = (crc_t)0xD364CB5E;
  tbl [254] = (crc_t)0x216EBF3C;
  tbl [255] = (crc_t)0x70976CE2;
}

/******************************************************************************/

#ifdef SELFTEST
static void
# ifdef ANSI_COMPILER
test_crc_table (
  const crc_t * const tbl,
  const crc_t mask32)
# else
test_crc_table (tbl, mask32)
  const crc_t * const tbl;
  const crc_t mask32;
# endif
{
  int i, j;
  const crc_t poly = tbl [1];
  const crc_t msb = (crc_t)0x80000000;

  for (i = 0; 256 > i; i++) {
    crc_t c = (crc_t)i;

    c <<= 24;

    for (j = 0; 8 > j; j++)
      if (0 != (c & msb))
        c = (c << 1) ^ poly;
      else
        c <<= 1;

    c &= mask32;

    if (c != tbl [i]) {
      out_err_check_int (
        fprintf (stderr,
          "FATAL: CRC table mismatch at index %d; expected %08lX, got %08lX.%s",
            i, (unsigned long)c, (unsigned long)tbl [i], CRC_EOL
        )
      );

      exit (EXIT_FAILURE);
    }
  }
}
#endif

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
char_bits (
  void)
#else
char_bits ()
#endif
{
  unsigned char x = 1;
  int bits = 0;

  while (0 != x) {
    bits++;
    x = (unsigned char)(x << 1);
  }

  return bits;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
unsigned_int_bits (
  void)
#else
unsigned_int_bits ()
#endif
{
  unsigned int x = 1;
  int bits = 0;

  while (0 != x) {
    bits++;
    x <<= 1;
  }

  return bits;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
crc_t_bits (
  void)
#else
crc_t_bits ()
#endif
{
  crc_t x = 1;
  int bits = 0;

  while (0 != (crc_t)x) {
    bits++;
    x <<= 1;
  }

  return bits;
}

/******************************************************************************/

static unsigned int
#ifdef ANSI_COMPILER
safe_batch_limit (
  void)
#else
safe_batch_limit ()
#endif
{
  int u_bits = unsigned_int_bits ();
  const int t_bits = crc_t_bits ();
  unsigned int limit;
  unsigned int max_inc;

  if (u_bits > 32)
    u_bits = 32;

  max_inc = ((8 < t_bits) ? (unsigned int)t_bits : 8);

  limit = 0;

  {
    int i;
    unsigned int rem = 0;

    for (i = 0; u_bits > i; i++) {
      rem = (rem << 1) | 1;
      limit <<= 1;

      if (rem >= max_inc) {
        rem -= max_inc;
        limit |= 1;
      }
    }
  }

  if (10000 < limit)
    limit = 10000;

  if (1 > limit)
    limit = 1;

  return limit;
}

/******************************************************************************/

static crc_t
#ifdef ANSI_COMPILER
make_mask (
  int bits)
#else
make_mask (bits)
  int bits;
#endif
{
  int i;
  int w = crc_t_bits ();
  crc_t m = 0;

  if (bits > w)
    bits = w;

  for (i = 0; bits > i; i++)
    m = m * 2 + 1;

  return m;
}

/******************************************************************************/

#ifndef HI_TECH_CPM
static crc_t
# ifdef ANSI_COMPILER
crc_update_byte (
  crc_t crc,
  const crc_t * const tbl,
  const crc_t mask32,
  const unsigned char b)
# else
crc_update_byte (crc, tbl, mask32, b)
  crc_t crc;
  const crc_t * const tbl;
  const crc_t mask32;
  const unsigned char b;
# endif
{
  crc_t idx, t;

  t = crc;
  t >>= 24;

  idx = t;
  idx ^= (crc_t)b;
  idx &= (crc_t)0xFF;

  t = crc;
  t <<= 8;

  crc = t;
  crc ^= tbl [idx];
  crc &= mask32;

  return crc;
}
#endif

/******************************************************************************/

#ifndef HI_TECH_CPM
static crc_t
# ifdef ANSI_COMPILER
crc_update_buffer (
  crc_t crc,
  const crc_t * const tbl,
  const crc_t mask32,
  const unsigned char * const buf,
  const long n)
# else
crc_update_buffer (crc, tbl, mask32, buf, n)
  crc_t crc;
  const crc_t * const tbl;
  const crc_t mask32;
  const unsigned char * const buf;
  const long n;
# endif
{
  long i;

  for (i = 0; n > i; i++)
    crc = crc_update_byte (crc, tbl, mask32, buf [i]);

  return crc;
}
#endif

/******************************************************************************/

#ifdef HI_TECH_CPM
static crc_t hitc_crc;
static crc_t hitc_idx;
static crc_t hitc_t;
#endif

/******************************************************************************/

#ifdef HI_TECH_CPM
static void
# ifdef ANSI_COMPILER
crc_update_byte_hitc (
  const crc_t * const tbl,
  const crc_t mask32,
  const unsigned char b)
# else
crc_update_byte_hitc (tbl, mask32, b)
  const crc_t * const tbl;
  const crc_t mask32;
  const unsigned char b;
# endif
{
  hitc_t = hitc_crc;
  hitc_t >>= 24;

  hitc_idx = hitc_t;
  hitc_idx ^= (crc_t)b;
  hitc_idx &= (crc_t)0xFF;

  hitc_t = hitc_crc;
  hitc_t <<= 8;

  hitc_crc = hitc_t;
  hitc_crc ^= tbl [hitc_idx];
  hitc_crc &= mask32;
}
#endif

/******************************************************************************/

#ifdef HI_TECH_CPM
static void
# ifdef ANSI_COMPILER
crc_update_buffer_hitc (
  const crc_t * const tbl,
  const crc_t mask32,
  const unsigned char * const buf,
  const long n)
# else
crc_update_buffer_hitc (tbl, mask32, buf, n)
  const crc_t * const tbl;
  const crc_t mask32;
  const unsigned char * const buf;
  const long n;
# endif
{
  long i;

  for (i = 0; n > i; i++)
    crc_update_byte_hitc (tbl, mask32, buf [i]);
}
#endif

/******************************************************************************/

#ifdef multics
static int
# ifdef ANSI_COMPILER
mgetc (
  FILE * fp)
# else
mgetc (fp)
  FILE * fp;
# endif
{
  const int c = fgetc (fp);

  if (-1 != c)
    return c;

  if (0 != feof (fp) || 0 != ferror (fp))
    return EOF;

  return 0x1FF;
}
#endif

/******************************************************************************/

static crc_t
#ifdef ANSI_COMPILER
compute_crc_fb (
  FILE * const fp,
  const char * const filename,
  const crc_t * const tbl,
  const int use_cb,
  const crc_t mask32,
  const crc_t inmask,
  const int pad,
  const counter_t * const lim_bits,
  counter_t * const processed_bits,
  counter_t * const processed_chars,
  int * const actually_padded,
  const unsigned int batch_limit,
  const counter_t * const expected_chars)
#else
compute_crc_fb (fp, filename, tbl, use_cb, mask32, inmask, pad, lim_bits,
                processed_bits, processed_chars, actually_padded, batch_limit,
                expected_chars)
  FILE * const fp;
  const char * const filename;
  const crc_t * const tbl;
  const int use_cb;
  const crc_t mask32;
  const crc_t inmask;
  const int pad;
  const counter_t * const lim_bits;
  counter_t * const processed_bits;
  counter_t * const processed_chars;
  int * const actually_padded;
  const unsigned int batch_limit;
  const counter_t * const expected_chars;
#endif
{
  unsigned char oct;
  int ch;
  crc_t tmp;
  crc_t crc = 0;
  crc_t buf = 0;
  int bib = 0;
  long nread = 0;
  long pos = 0;
  unsigned int acc_bits = 0;
  unsigned int acc_chars = 0;
  counter_t rem_bits;
  int limit_reached = 0;

#ifdef HI_TECH_CPM
  hitc_crc = 0;
#endif

  cb_copy (& rem_bits, lim_bits);

#ifndef multics
# ifndef __LINT__
#  ifdef _MSC_VER
  if (expected_chars != expected_chars) { }
#  else
#   ifndef __SDCC
#    ifndef HI_TECH_C
  /*LINTED E_FALSE_LOGICAL_EXPR*/
  if ((0) && (expected_chars)) { }
#    endif
#   else
  (void)expected_chars;
#   endif
#  endif
# else
#  ifdef _AIX
  /*cppcheck-suppress duplicateExpression*/
  if (expected_chars != expected_chars) { }
#  else
  (void)expected_chars;
#  endif
# endif
#endif

  for (;;) {
    while (8 > bib) {
      unsigned int bits_added;

      if (0 == cb_is_zero (lim_bits) &&
          0 != cb_is_zero (& rem_bits)) {
        limit_reached = 1;

        break;
      }

      if (0 == cb_is_zero (lim_bits) &&
          0 == cb_is_zero (& rem_bits)) {
        counter_t uc_cb;

        cb_zero (& uc_cb);

        if (0 == cb_add (& uc_cb, (unsigned int)use_cb)) {
          error_msg ("Counter logic error reading", filename, 0);

          goto done;
        }

        if (0 > cb_cmp (& rem_bits, & uc_cb))
          if (0 == pad && 0 == g_pad_auto) {
            out_err_check_int (
              fprintf (stderr, "WARNING: --limit ended mid %d-bit character;",
                (int)use_cb
              )
            );
            out_err_check_int (
              fprintf (stderr, " use --pad if needed.%s",
                CRC_EOL
              )
            );

            cb_zero (& rem_bits);

            goto done;
          }
      }

      if (nread <= pos) {
#ifndef USE_FREAD
        nread = 0;
#endif

#ifdef USE_FREAD
        nread = (long)fread (g_buf, 1, sizeof (g_buf), fp);
#else
        while ((long)sizeof (g_buf) > nread) {
# ifdef multics
          const int c = mgetc (fp);
# else
          const int c = fgetc (fp);
# endif

          if (EOF == c) {
# ifdef multics
            if (0 != expected_chars &&
                0 != cb_cmp (processed_chars, expected_chars)) {
              const int w = getw (fp);

              if (w != -1 || (0 == feof (fp) && 0 == ferror (fp))) {
                if (nread <= (long)(sizeof (g_buf) - 4)) {
                  g_buf [nread++] = (unsigned char)((w >> 27) & 0x1FF);
                  g_buf [nread++] = (unsigned char)((w >> 18) & 0x1FF);
                  g_buf [nread++] = (unsigned char)((w >> 9) & 0x1FF);
                  g_buf [nread++] = (unsigned char)(w & 0x1FF);
                  (void)clearerr (fp);

                  continue;
                }
              }
            }
# endif

            break;
          }

          g_buf [nread++] = (unsigned char)c;
        }
#endif

        if (0 != ferror (fp)) {
          error_msg ("Error reading", filename, errno);

          goto done;
        }

        if (0 == nread) {
          if (0 == feof (fp)) {
            if (0 != expected_chars) {
              counter_t cur_chars;

              cb_copy (& cur_chars, processed_chars);
              (void)cb_add (& cur_chars, acc_chars);

              if (0 == cb_cmp (& cur_chars, expected_chars)) {

                goto done;
              }
            }

            error_msg ("Error reading", filename, errno);
          }

          goto done;
        }

        pos = 0;
      }

      ch = g_buf [pos];
      pos++;

      acc_chars++;

      tmp = (crc_t)(unsigned char)ch;

      if (0 == cb_is_zero (lim_bits) &&
          0 == cb_is_zero (& rem_bits)) {
        counter_t uc_cb;

        cb_zero (& uc_cb);

        (void)cb_add (& uc_cb, (unsigned int)use_cb);

        if (0 > cb_cmp (& rem_bits, & uc_cb)) {
          int k;
          unsigned int rb_val = 0;

          for (k = MAX_CB_DIGITS - 1; 0 <= k; k--)
            rb_val = rb_val * 10 + (unsigned int)rem_bits.d [k];

          bits_added = rb_val;
          tmp &= make_mask ((int)bits_added);

          cb_zero (& rem_bits);
        } else {
          bits_added = (unsigned int)use_cb;
          tmp &= inmask;

          if (0 == cb_sub (& rem_bits, bits_added)) {
            error_msg ("Counter logic error reading", filename, 0);

            goto done;
          }
        }
      } else {
        bits_added = (unsigned int)use_cb;
        tmp &= inmask;
      }

      acc_bits += bits_added;

      if (acc_bits >= batch_limit) {
        if (0 == cb_add (processed_bits, acc_bits)) {
          error_msg ("Bit counter overflow reading", filename, 0);

          goto done;
        }

        if (0 == cb_add (processed_chars, acc_chars)) {
          error_msg ("Character counter overflow reading", filename, 0);

          goto done;
        }

        acc_bits = 0;
        acc_chars = 0;
      }

      tmp <<= bib;

      buf |= tmp;
      bib += (int)bits_added;

      if (0 == cb_is_zero (lim_bits) &&
          0 != cb_is_zero (& rem_bits)) {
        limit_reached = 1;

        break;
      }
    }

    if (0 != limit_reached && 8 > bib)
      break;

    oct = (unsigned char)(buf & (crc_t)0xFF);
#ifdef HI_TECH_CPM
    crc_update_byte_hitc (tbl, mask32, oct);
#else
    crc = crc_update_byte (crc, tbl, mask32, oct);
#endif

    buf >>= 8;
    bib -= 8;

    if (0 != limit_reached && 0 == bib)
      break;
  }

done:
  if (0 == g_fileerr) {
    if (0 < acc_bits)
      if (0 == cb_add (processed_bits, acc_bits))
        error_msg ("Bit counter overflow reading", filename, 0);

    if (0 < acc_chars)
      if (0 == cb_add (processed_chars, acc_chars))
        error_msg ("Character counter overflow reading", filename, 0);
  }

  if (0 == g_fileerr) {
    int hinted = 0;

    if (0 != pad || 0 != g_pad_auto) {
      while (0 == cb_is_zero (lim_bits) &&
             0 == cb_is_zero (& rem_bits)) {
        unsigned int bits_to_add;
        counter_t uc_cb;

        cb_zero (& uc_cb);

        (void)cb_add (& uc_cb, (unsigned int)use_cb);

        if (0 > cb_cmp (& rem_bits, & uc_cb)) {
          int k;
          unsigned int rb_val = 0;

          for (k = MAX_CB_DIGITS - 1; 0 <= k; k--)
            rb_val = rb_val * 10 + (unsigned int)rem_bits.d [k];

          bits_to_add = rb_val;

          cb_zero (& rem_bits);
        } else {
          bits_to_add = (unsigned int)use_cb;

          if (0 == cb_sub (& rem_bits, bits_to_add)) {
            error_msg ("Counter logic error reading", filename, 0);

            return (crc_t)0;
          }
        }

        bib += (int)bits_to_add;

        if (0 == cb_add (processed_bits, bits_to_add)) {
          error_msg ("Bit counter overflow reading", filename, 0);

          return (crc_t)0;
        }

        if (0 == cb_add (processed_chars, (unsigned int)1)) {
          error_msg ("Character counter overflow reading", filename, 0);

          return (crc_t)0;
        }

        while (8 <= bib) {
          oct = (unsigned char)(buf & (crc_t)0xFF);
#ifdef HI_TECH_CPM
          crc_update_byte_hitc (tbl, mask32, oct);
#else
          crc = crc_update_byte (crc, tbl, mask32, oct);
#endif

          buf >>= 8;
          bib -= 8;
          * actually_padded = 1;
        }
      }
    }

    if (0 < bib) {
      if (0 != pad || 0 != g_pad_auto) {
        oct = (unsigned char)(buf & (crc_t)0xFF);
#ifdef HI_TECH_CPM
        crc_update_byte_hitc (tbl, mask32, oct);
#else
        crc = crc_update_byte (crc, tbl, mask32, oct);
#endif
        * actually_padded = 1;
      } else {
        out_err_check_int (
          fprintf (stderr, "WARNING: %s with %d dangling bit%s ",
            (0 != limit_reached ? "Limit reached" : "File ended"),
              bib, (1 == bib ? "" : "s")
          )
        );
        out_err_check_int (
          fprintf (stderr, "(not a full 8-bit CRC input octet).%s",
            CRC_EOL
          )
        );
        hinted = 1;
      }
    }

    if (0 == cb_is_zero (lim_bits) &&
        0 == cb_is_zero (& rem_bits)) {
      counter_t used_bits;

      {
        int k;
        int borrow = 0;

        for (k = 0; MAX_CB_DIGITS > k; k++) {
          int diff = (int)lim_bits -> d [k] - (int)rem_bits.d [k] - borrow;

          if (0 > diff) {
            diff += 10;
            borrow = 1;
          } else
            borrow = 0;

          used_bits.d [k] = (unsigned char)diff;
        }
      }

      if (0 == pad && 0 == g_pad_auto) {
        out_err_check_int (
          fprintf (stderr, "WARNING: File ended after ")
        );
        cb_printf (stderr, & used_bits);
        out_err_check_int (
          fprintf (stderr, " bits, but --limit=")
        );
        cb_printf (stderr, lim_bits);
        out_err_check_int (
          fprintf (stderr, " was requested.%s",
            CRC_EOL
          )
        );
        hinted = 1;
      } else
        * actually_padded = 1;
    }

    if (0 != hinted && 0 == pad && 0 == g_pad_auto)
      out_err_check_int (
        fprintf (stderr, "         Use --pad to zero-fill remaining bits.%s",
          CRC_EOL
        )
      );
  }

#ifdef HI_TECH_CPM
  crc = hitc_crc;
#endif
  return crc;
}

/******************************************************************************/

static crc_t
#ifdef ANSI_COMPILER
compute_crc (
  FILE * const fp,
  const char * const filename,
  const crc_t * const tbl,
  const int cb,
  const int ub,
  const int use_cb,
  const crc_t mask32,
  const crc_t inmask,
  const int pad,
  const counter_t * const lim_bits,
  counter_t * const processed_bits,
  counter_t * const processed_chars,
  int * const actually_padded,
  const unsigned int batch_limit,
  const counter_t * const expected_chars)
#else
compute_crc (fp, filename, tbl, cb, ub, use_cb, mask32, inmask, pad,
             lim_bits, processed_bits, processed_chars, actually_padded,
             batch_limit, expected_chars)
  FILE * const fp;
  const char * const filename;
  const crc_t * const tbl;
  const int cb;
  const int ub;
  const int use_cb;
  const crc_t mask32;
  const crc_t inmask;
  const int pad;
  const counter_t * const lim_bits;
  counter_t * const processed_bits;
  counter_t * const processed_chars;
  int * const actually_padded;
  const unsigned int batch_limit;
  const counter_t * const expected_chars;
#endif
{
  if (NULL == fp) {
    out_err_check_int (
      fprintf (stderr, "FATAL: compute_crc called with NULL file pointer.%s",
        CRC_EOL
      )
    );

    exit (EXIT_FAILURE);
  }

  if (32 > ub) {
    out_err_check_int (
      fprintf (stderr, "FATAL: Need >=32-bit crc_t type, have %d bits.%s",
        ub, CRC_EOL
      )
    );

    exit (EXIT_FAILURE);
  }

  if (8 == use_cb && 8 == cb) {
    crc_t crc = 0;
    counter_t rem_bits;

#ifdef HI_TECH_CPM
    hitc_crc = 0;
#endif

    cb_zero (& rem_bits);

    cb_copy (& rem_bits, lim_bits);

    for (;;) {
      long nread, bytes_to_process;

#ifndef USE_FREAD
      nread = 0;
#endif

#ifdef USE_FREAD
      nread = (long)fread (g_buf, 1, sizeof (g_buf), fp);
#else
      while ((long)sizeof (g_buf) > nread) {
# ifdef multics
        const int c = mgetc (fp);
# else
        const int c = fgetc (fp);
# endif

        if (EOF == c) {
# ifdef multics
          if (0 != expected_chars &&
              0 != cb_cmp (processed_chars, expected_chars)) {
            const int w = getw (fp);

            if (w != -1 || (0 == feof (fp) && 0 == ferror (fp))) {
              if (nread <= (long)(sizeof (g_buf) - 4)) {
                g_buf [nread++] = (unsigned char)((w >> 27) & 0x1FF);
                g_buf [nread++] = (unsigned char)((w >> 18) & 0x1FF);
                g_buf [nread++] = (unsigned char)((w >> 9) & 0x1FF);
                g_buf [nread++] = (unsigned char)(w & 0x1FF);
                (void)clearerr (fp);

                continue;
              }
            }
          }
# endif

          break;
        }

        g_buf [nread++] = (unsigned char)c;
      }
#endif

      if (0 != ferror (fp)) {
        error_msg ("Error reading", filename, errno);

        return (crc_t)0;
      }

      if (0 == nread) {
        if (0 == feof (fp)) {
          if (0 != expected_chars &&
              0 == cb_cmp (processed_chars, expected_chars)) {

            break;
          }

          error_msg ("Error reading", filename, errno);
        }

        break;
      }

      bytes_to_process = nread;

      if (0 == cb_is_zero (lim_bits)) {
        long count = 0;
        counter_t eight;

        cb_zero (& eight);

        (void)cb_add (& eight, (unsigned int)8);

        while (0 <= cb_cmp (& rem_bits, & eight) && nread > count) {
          (void)cb_sub (& rem_bits, (unsigned int)8);
          count++;
        }

        bytes_to_process = count;
      }

      if (0 < bytes_to_process) {
        long remaining = bytes_to_process;

        while (remaining > 0) {
          unsigned int chunk = ((remaining > (long)batch_limit)
            ? batch_limit : (unsigned int)remaining);

          if (0 == cb_add (processed_bits, chunk * (unsigned int)8)) {
            error_msg ("Bit counter overflow reading", filename, 0);

            return (crc_t)0;
          }

          if (0 == cb_add (processed_chars, chunk)) {
            error_msg ("Character counter overflow reading", filename, 0);

            return (crc_t)0;
          }

          remaining -= (long)chunk;
        }

#ifdef HI_TECH_CPM
        crc_update_buffer_hitc (tbl, mask32, g_buf, bytes_to_process);
#else
        crc = crc_update_buffer (crc, tbl, mask32, g_buf, bytes_to_process);
#endif
      }

      if (0 == cb_is_zero (lim_bits) &&
          0 == cb_is_zero (& rem_bits) &&
          nread > bytes_to_process) {
        if (0 != pad || 0 != g_pad_auto) {
          unsigned char mask;
          int k;
          unsigned char final_byte = g_buf [bytes_to_process];
          unsigned long rb_val = 0;

          for (k = MAX_CB_DIGITS - 1; 0 <= k; k--)
            rb_val = rb_val * 10 + (unsigned long)rem_bits.d [k];

          mask = (unsigned char)make_mask ((int)rb_val);

          final_byte &= mask;

          if (0 == cb_add (processed_bits, (unsigned int)rb_val)) {
            error_msg ("Bit counter overflow reading", filename, 0);

            return (crc_t)0;
          }

          if (0 == cb_add (processed_chars, (unsigned int)1)) {
            error_msg ("Character counter overflow reading", filename, 0);

            return (crc_t)0;
          }

#ifdef HI_TECH_CPM
          crc_update_byte_hitc (tbl, mask32, final_byte);
#else
          crc = crc_update_byte (crc, tbl, mask32, final_byte);
#endif
          * actually_padded = 1;

          cb_zero (& rem_bits);
        } else {
          out_err_check_int (
            fprintf (stderr, "WARNING: Input --limit caused truncation")
          );
          out_err_check_int (
            fprintf (stderr, " mid-character; use --pad if needed.%s",
              CRC_EOL
            )
          );

          cb_zero (& rem_bits);
        }

        break;
      }

      if (0 != cb_is_zero (lim_bits) ||
          0 != cb_is_zero (& rem_bits))
        if (0 == cb_is_zero (lim_bits))
          break;
    }

    clearerr (fp);

    if (0 == cb_is_zero (lim_bits) &&
        0 == cb_is_zero (& rem_bits)) {
      if (0 != pad || 0 != g_pad_auto) {
        unsigned char zbuf [32];
        long k;
        counter_t eight;

        cb_zero (& eight);

        (void)cb_add (& eight, (unsigned int)8);

        for (k = 0; (long)sizeof (zbuf) > k; k++)
          zbuf [k] = 0;

        while (0 <= cb_cmp (& rem_bits, & eight)) {
          long chunk = 0;

          while ((long)sizeof (zbuf) > chunk &&
                 0 <= cb_cmp (& rem_bits, & eight)) {
            chunk++;
            (void)cb_sub (& rem_bits, (unsigned int)8);
          }

          if (0 < chunk) {
            long remaining = chunk;

            while (remaining > 0) {
              unsigned int c_chunk = ((remaining > (long)batch_limit)
                ? batch_limit : (unsigned int)remaining);

              if (0 == cb_add (processed_bits, c_chunk * (unsigned int)8)) {
                error_msg ("Bit counter overflow reading", filename, 0);

                return (crc_t)0;
              }

              if (0 == cb_add (processed_chars, c_chunk)) {
                error_msg ("Character counter overflow reading", filename, 0);

                return (crc_t)0;
              }

              remaining -= (long)c_chunk;
            }

            * actually_padded = 1;
#ifdef HI_TECH_CPM
            crc_update_buffer_hitc (tbl, mask32, zbuf, chunk);
#else
            crc = crc_update_buffer (crc, tbl, mask32, zbuf, chunk);
#endif
          }
        }

        if (0 == cb_is_zero (& rem_bits)) {
          if (0 == g_pad_auto) {
            out_err_check_int (
              fprintf (stderr, "WARNING: --limit not a multiple of 8; ")
            );
            out_err_check_int (
              fprintf (stderr, "trailing ")
            );
            cb_printf (stderr, & rem_bits);
            out_err_check_int (
              fprintf (stderr, " bit%s ignored in 8-bit mode.%s",
                (cb_is_one (& rem_bits) ? "" : "s"), CRC_EOL
              )
            );

            if (0 == cb_is_zero (processed_chars)) {
              out_err_check_int (
                fprintf (stderr, "         ")
              );
              out_err_check_int (
                fprintf (stderr,
                  "Result calculated only up to the last full character.%s",
                    CRC_EOL
                )
              );
            }
          }
        }
      } else {
        counter_t used_bits;

        cb_zero (& used_bits);

        {
          int k;
          int borrow = 0;

          for (k = 0; MAX_CB_DIGITS > k; k++) {
            int diff = (int)lim_bits -> d [k] - (int)rem_bits.d [k] - borrow;

            if (0 > diff) {
              diff += 10;
              borrow = 1;
            } else
              borrow = 0;

            used_bits.d [k] = (unsigned char)diff;
          }
        }

        out_err_check_int (
          fprintf (stderr, "WARNING: File ended after ")
        );
        cb_printf (stderr, & used_bits);
        out_err_check_int (
          fprintf (stderr, " bit%s, but --limit=",
            (cb_is_one (& used_bits) ? "" : "s")
          )
        );
        cb_printf (stderr, lim_bits);
        out_err_check_int (
          fprintf (stderr, " requested.%s",
            CRC_EOL
          )
        );
        out_err_check_int (
          fprintf (stderr, "         ")
        );
        out_err_check_int (
          fprintf (stderr, "Use --pad to zero-fill the remaining bit%s.%s",
            (cb_is_one (& rem_bits) ? "" : "s"), CRC_EOL
          )
        );
      }
    }

#ifdef HI_TECH_CPM
    crc = hitc_crc;
#endif
    return crc;
  }

  return compute_crc_fb (fp,
    filename, tbl, use_cb, mask32, inmask, pad, lim_bits, processed_bits,
      processed_chars, actually_padded, batch_limit, expected_chars
  );
}

/******************************************************************************/

#ifdef CRC_CPM

/*
 * Direct BDOS access for the LRBC query.  Z88DK has bdos() and the directory
 * helpers in <cpm.h> and uses an FCB address passed as an int.  Aztec C has
 * the bdos() function in the CP/M runtime and takes the FCB pointer directly.
 */

# ifdef __Z88DK
#  include <cpm.h>
#  define BDOS_FCB(p) ((int)(p))
# else
extern int bdos (); /* We provide our own for Ack CP/M-80 and Aztec CP/M-68K */
#  ifdef __ACK__
#   define BDOS_FCB(p) ((int)(p))
#  else
#   define BDOS_FCB(p) (p)
#  endif
# endif

/******************************************************************************/

/*
 * Build a CP/M File Control Block from a filename.  The 8.3 name is upper-
 * cased and space-padded; an optional "d:" drive prefix is skipped so CP/M
 * resolves the file on the default drive.
 */

static void
# ifdef ANSI_COMPILER
cpm_setfcb (
  unsigned char * const fcb,
  const char * const fn)
# else
cpm_setfcb (fcb, fn)
  unsigned char * const fcb;
  const char * const fn;
# endif
{
  const char * p = fn;
  int i;

  for (i = 0; 36 > i; i++)
    fcb [i] = 0;

  for (i = 1; 11 >= i; i++)
    fcb [i] = ' ';

  if ('\0' != p [0] && ':' == p [1])
    p += 2;

  i = 1;

  while ('\0' != * p && '.' != * p && 8 >= i) {
    int c = (int)(unsigned char)* p++;

    if ('a' <= c && 'z' >= c)
      c -= 32;

    fcb [i++] = (unsigned char)c;
  }

  while ('\0' != * p && '.' != * p)
    p++;

  if ('.' == * p)
    p++;

  i = 9;

  while ('\0' != * p && 11 >= i) {
    int c = (int)(unsigned char)* p++;

    if ('a' <= c && 'z' >= c)
      c -= 32;

    fcb [i++] = (unsigned char)c;
  }
}

/******************************************************************************/

static long
# ifdef ANSI_COMPILER
cpm_file_size (
  const char * const fn,
  const int isx,
  counter_t * const chars)
# else
cpm_file_size (fn, isx, chars)
  const char * const fn;
  const int isx;
  counter_t * const chars;
# endif
{
  unsigned char fcb [36];
  long records, total, base, exact, unused, last_ext;
  int lrbc;

# ifndef __CPM68K__
  if (0x30 > (bdos (12, 0) & 0x00ff))
    return -2L;
# endif

# ifdef __CPM68K__
#  ifndef BDOS_BIG_ENDIAN
#   define BDOS_BIG_ENDIAN
#  endif
# endif

/*
 * There may be other CP/M-like systems for 8080/Z80 out there that may or may
 * not support the CP/M-Plus-style LRBC metadata that I haven't investigated!
 */

  cpm_setfcb (fcb, fn);
  (void)bdos (35, BDOS_FCB (fcb));
  records =
# ifndef BDOS_BIG_ENDIAN
            ((long)fcb [33])
          | ((long)fcb [34] << 8)
          | ((long)fcb [35] << 16);
# else
            ((long)fcb [33] << 16)
          | ((long)fcb [34] << 8)
          | ((long)fcb [35]);
# endif

  if (0L >= records)
    return -1L;

  last_ext = (records - 1L) >> 7; /* 16 KiB extents, 128 records each */

  cpm_setfcb (fcb, fn);
  fcb [12] = (unsigned char)(last_ext & 0x1f);
  fcb [14] = (unsigned char)((last_ext >> 5) & 0x3f);

  fcb [32] = 0xff;

  if (0xff == (bdos (15, BDOS_FCB (fcb)) & 0x00ff))
    return -1L;

  lrbc = fcb [32] & 0xff;

  fcb [32] = 0x00;

  (void)bdos (16, BDOS_FCB (fcb));

  total = records * 128L;
  base = total - 128L;

  if (0 >= lrbc)
    exact = total;
  else if (0 != isx)
    exact = total - (long)lrbc;
  else
    exact = base + (long)lrbc;

  if (exact <= base || exact > total)
    exact = total;

  cb_zero (chars);

  (void)cb_add (chars, (unsigned int)((records >> 16) & 0xffL));
  (void)cb_mul (chars, (unsigned int)256);
  (void)cb_add (chars, (unsigned int)((records >> 8) & 0xffL));
  (void)cb_mul (chars, (unsigned int)256);
  (void)cb_add (chars, (unsigned int)(records & 0xffL));

  if (0 == cb_mul (chars, (unsigned int)128))
    return -1L;

  unused = total - exact;

  if (0L < unused)
    if (0 == cb_sub (chars, (unsigned int)unused))
      return -1L;

  return exact;
}

/******************************************************************************/

# ifdef CPM_BDOS_WILDCARD

/*
 * Build a CP/M FCB from a wildcard pattern.  Like cpm_setfcb() but expands
 * '*' into runs of '?' so BDOS Search First and Search Next match correctly.
 */

static void
#  ifdef ANSI_COMPILER
cpm_setfcb_wild (
  unsigned char * const fcb,
  const char * const fn)
#  else
cpm_setfcb_wild (fcb, fn)
  unsigned char * const fcb;
  const char * const fn;
#  endif
{
  const char * p = fn;
  int i, c;

  for (i = 0; 36 > i; i++)
    fcb [i] = 0;

  for (i = 1; 11 >= i; i++)
    fcb [i] = ' ';

  if ('\0' != p [0] && ':' == p [1])
    p += 2;

  i = 1;

  while ('\0' != * p && '.' != * p && 8 >= i) {
    if ('*' == * p) {
      while (8 >= i)
        fcb [i++] = '?';

      break;
    }

    c = (int)(unsigned char)* p++;

    if ('a' <= c && 'z' >= c)
      c -= 32;

    fcb [i++] = (unsigned char)c;
  }

  while ('\0' != * p && '.' != * p)
    p++;

  if ('.' == * p)
    p++;

  i = 9;

  while ('\0' != * p && 11 >= i) {
    if ('*' == * p) {
      while (11 >= i)
        fcb [i++] = '?';

      break;
    }

    c = (int)(unsigned char)* p++;

    if ('a' <= c && 'z' >= c)
      c -= 32;

    fcb [i++] = (unsigned char)c;
  }
}

/******************************************************************************/

/*
 * Convert a 32-byte CP/M directory entry into a NUL-terminated filename
 * string (up to 12 characters: "FILENAME.EXT").
 */

static void
#  ifdef ANSI_COMPILER
cpm_dir_to_name (
  char * const buf,
  const unsigned char * const entry)
#  else
cpm_dir_to_name (buf, entry)
  char * const buf;
  const unsigned char * const entry;
#  endif
{
  int i, j, last, c, has_ext;

  j = 0;
  last = 0;

  for (i = 1; 8 >= i; i++) {
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

  if (0 != has_ext) {
    buf [j++] = '.';
    last = j;

    for (i = 9; 11 >= i; i++) {
      c = entry [i] & 0x7f;
      buf [j++] = (char)c;

      if (' ' != c)
        last = j;
    }

    j = last;
  }

  buf [j] = '\0';
}

# endif

#endif

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
find_max_bits (
  const char * const filename,
  const int cb,
  int * const is_all_zeros,
  counter_t * const num_chars,
  const long max_chars)
#else
find_max_bits (filename, cb, is_all_zeros, num_chars, max_chars)
  const char * const filename;
  const int cb;
  int * const is_all_zeros;
  counter_t * const num_chars;
  const long max_chars;
#endif
{
  FILE * fp;
  crc_t aggregate = 0;
  int bits;
  long got = 0;

  * is_all_zeros = 1;

  cb_zero (num_chars);

  fp = fopen (filename, "rb");

  if (NULL == fp)
    fp = fopen (filename, "r");

  if (NULL == fp)
    return -1; /* Errors handled via process_file */

#ifdef USE_FREAD
  {
    for (;;) {
      const long n = (long)fread (g_buf, 1, sizeof (g_buf), fp);
      long use = n;

      if (0 >= n) {
        if (0 == feof (fp) && 0 == ferror (fp)) {
          error_msg ("Error reading", filename, errno);
          (void)fclose (fp);

          return -1;
        }

        break;
      }

      if (0 < max_chars && use > max_chars - got)
        use = max_chars - got;

      {
        long i;

        for (i = 0; use > i; i++) {
          const crc_t b = (crc_t)g_buf [i];

          aggregate |= b;
        }

        if (0 == cb_add (num_chars, (unsigned int)use)) {
          error_msg ("Character counter overflow reading", filename, 0);
          (void)fclose (fp);

          return -1;
        }
      }

      got += use;

      if (0 < max_chars && got >= max_chars)
        break;

      if ((long)sizeof (g_buf) > n) {
        if (0 != ferror (fp)) {
          error_msg ("Error reading", filename, errno);
          (void)fclose (fp);

          return -1;
        }

        break;
      }
    }
  }
#else
  {
    int hit_eof = 0;

    while (0 == hit_eof) {
      long nread = 0;

      while ((long)sizeof (g_buf) > nread) {
# ifdef multics
        const int c = mgetc (fp);
# else
        const int c = fgetc (fp);
# endif

        if (EOF == c) {
# ifdef multics
          const int w = getw (fp);

          if (w != -1 || (0 == feof (fp) && 0 == ferror (fp))) {
            if (nread <= (long)(sizeof (g_buf) - 4)) {
              g_buf [nread++] = (unsigned char)((w >> 27) & 0x1FF);
              g_buf [nread++] = (unsigned char)((w >> 18) & 0x1FF);
              g_buf [nread++] = (unsigned char)((w >> 9) & 0x1FF);
              g_buf [nread++] = (unsigned char)(w & 0x1FF);
              (void)clearerr (fp);

              continue;
            }
          }
# endif
          hit_eof = 1;

          break;
        }

        g_buf [nread++] = (unsigned char)c;
      }

      if (0 != ferror (fp))
        break;

      if (0 == nread) {
        if (0 == feof (fp) && 0 == ferror (fp)) {
          error_msg ("Error reading", filename, errno);
          (void)fclose (fp);

          return -1;
        }

        break;
      }

      {
        long i;
        long use = nread;

        if (0 < max_chars && use > max_chars - got)
          use = max_chars - got;

        for (i = 0; use > i; i++) {
          const crc_t b = (crc_t)g_buf [i];

          aggregate |= b;
        }

        if (0 == cb_add (num_chars, (unsigned int)use)) {
          error_msg ("Character counter overflow reading", filename, 0);
          (void)fclose (fp);

          return -1;
        }

        got += use;
      }

      if (0 < max_chars && got >= max_chars)
        break;
    }
  }
#endif

  if (0 != ferror (fp)) {
    error_msg ("Error reading", filename, errno);
    (void)fclose (fp);
    return -1;
  }

  if (0 != fclose (fp)) {
    error_msg ("Error closing", filename, errno);
    return -1;
  }

  if (0 == aggregate)
    return 0;

  * is_all_zeros = 0;

  for (bits = cb; bits > 0; bits--) {
    const crc_t bit_mask = (crc_t)1 << (bits - 1);

    if (0 != (aggregate & bit_mask))
      return bits;
  }

  return 0;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
check_is_directory (
  const char * const filename)
#else
check_is_directory (filename)
  const char * const filename;
#endif
{
#ifdef HAVE_SYS_STAT
# ifdef S_ISDIR
  struct stat st;

  if (0 ==
    stat (
#  ifdef DOS_MSC_VER
      (char *)(void *)filename,
#  else
#   ifdef __PACIFIC__
      (char *)(void *)filename,
#   else
      filename,
#   endif
#  endif
      & st
    )
  )
    if (0 != S_ISDIR (st.st_mode)) {
#  ifdef EISDIR
#   ifndef __BORLANDC__
#    ifdef __TURBOC__
      error_msg ("Is a directory:", filename, 0);
#    else
#     ifdef _MSC_VER
#      ifdef __MSDOS__
      error_msg ("Is a directory:", filename, 0);
#      else
      error_msg ("Error opening", filename, EISDIR);
#      endif
#     else
      error_msg ("Error opening", filename, EISDIR);
#     endif
#    endif
#   else
      error_msg ("Error opening", filename, EISDIR);
#   endif
#  else
      error_msg ("Is a directory:", filename, 0);
#  endif

      return 1;
    }
# endif
#endif

#ifndef __LINT__
# ifdef _MSC_VER
  if (filename != filename) { }
# else
#  ifndef __SDCC
#   ifndef HI_TECH_C
  /*LINTED E_FALSE_LOGICAL_EXPR*/
  if ((0) && (filename)) { }
#   endif
#  else
  (void)filename;
#  endif
# endif
#else
# ifdef _AIX
  /*cppcheck-suppress duplicateExpression*/
  if (filename != filename) { }
# else
  (void)filename;
# endif
#endif

  return 0;
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
process_file (
  const char * const filename,
  const crc_t * const tbl,
  const int cb,
  const int ub,
  const int use_cb,
  const crc_t mask32,
  const crc_t inmask,
  const int pad,
  const counter_t * const lim_bits,
  const unsigned int batch_limit)
#else
process_file (filename, tbl, cb, ub, use_cb, mask32, inmask, pad, lim_bits,
              batch_limit)
  const char * const filename;
  const crc_t * const tbl;
  const int cb;
  const int ub;
  const int use_cb;
  const crc_t mask32;
  const crc_t inmask;
  const int pad;
  const counter_t * const lim_bits;
  const unsigned int batch_limit;
#endif
{
  FILE * fp;
  char buf [9];
  crc_t v;
  int i;
  int is_all_zeros = 0;
  int bit_check_failed = 0;
  int local_use_cb = use_cb;
  int auto_v = 0;
  int actually_padded = 0;
  crc_t local_inmask = inmask;
  counter_t processed_bits;
  counter_t processed_chars;
  counter_t expected_chars;
  counter_t eff_lim;
  long lrbc_cap = 0L;
#ifdef CRC_CPM
  counter_t lrbc_chars;
  int lrbc_active = 0;
#endif

  cb_zero (& processed_bits);
  cb_zero (& processed_chars);
  cb_zero (& expected_chars);

  cb_copy (& eff_lim, lim_bits);

#ifdef CRC_CPM
  cb_zero (& lrbc_chars);
#endif

  g_fileerr = 0;

  /*cppcheck-suppress knownConditionTrueFalse*/
  if (0 != check_is_directory (filename))
    return;

#ifdef CRC_CPM
  if (0 != g_cpm_lrbc) {
    const long exact = cpm_file_size (filename,
                                      (2 == g_cpm_lrbc), & lrbc_chars);

    if (0L <= exact) {
      lrbc_cap = exact;
      lrbc_active = 1;
    } else if (0 != g_cpm_lrbc_explicit) {
      if (-2L == exact) {
        if (0 == g_lrbc_warned) {
          out_err_check_int (
            fprintf (stderr, "WARNING: --lrbc requires CP/M 3.0 or later; ")
          );
          out_err_check_int (
            fprintf (stderr, "processing all records.%s",
              CRC_EOL
            )
          );
          g_lrbc_warned = 1;
        }
      } else
        out_err_check_int (
          fprintf (stderr,
            "WARNING: Could not read LRBC for %s; processing all records.%s",
              filename, CRC_EOL
          )
        );
    }
  }
#endif

  {
    const int max_bits = find_max_bits (filename, cb,
      & is_all_zeros, & expected_chars, lrbc_cap);

    if (0 > max_bits) {
      if (0 != g_fileerr)
        return;

      is_all_zeros = 0;
      bit_check_failed = 1;
    }

    if (0 != g_bits_auto) {
      if (0 != bit_check_failed || 0 != is_all_zeros)
        local_use_cb = cb;
      else
        local_use_cb = max_bits;

      if (local_use_cb != cb || 0 != is_all_zeros)
        auto_v = 1;

      local_inmask = make_mask (local_use_cb);
    }
  }

#ifdef CRC_CPM
  if (0 != lrbc_active) {
    counter_t lrbc_bits;

    cb_copy (& lrbc_bits, & lrbc_chars);

    if (0 != cb_mul (& lrbc_bits, (unsigned int)local_use_cb))
      if (0 != cb_is_zero (& eff_lim) || 0 > cb_cmp (& lrbc_bits, & eff_lim))
        cb_copy (& eff_lim, & lrbc_bits);
  }
#endif

  fp = fopen (filename, "rb");

  if (NULL == fp)
    fp = fopen (filename, "r");

  if (NULL == fp) {
    error_msg ("Error opening", filename, errno);

    return;
  }

  {
#ifndef HI_TECH_CPM
    const
#endif
     crc_t crcval = compute_crc (fp,
      filename, tbl, cb, ub, local_use_cb, mask32, local_inmask, pad, & eff_lim,
        & processed_bits, & processed_chars, & actually_padded, batch_limit,
          & expected_chars);
#ifdef HI_TECH_CPM
    /*cppcheck-suppress redundantInitialization*/
    crcval = hitc_crc;
#endif

    if (0 != fclose (fp))
      error_msg ("Error closing", filename, errno);

    if (0 != g_fileerr)
      return;

    v = (crc_t)crcval & mask32;
  }

  if (0 != bit_check_failed) {
    out_err_check_int (
      fprintf (stderr, "WARNING: Significant bit check failed for %s; ",
        filename
      )
    );
    out_err_check_int (
      fprintf (stderr, "assuming %d-bit characters.%s",
        cb, CRC_EOL
      )
    );
  }

  if (0 != g_bits_auto && 0 != cb_is_zero (& eff_lim) &&
      0 == bit_check_failed && 0 == cb_is_zero (& expected_chars)) {
    counter_t expected_bits;

    cb_copy (& expected_bits, & expected_chars);

    if (0 == cb_mul (& expected_bits, (unsigned int)local_use_cb)) {
      error_msg ("Bit count overflow validating", filename, 0);

      return;
    } else if (0 != cb_cmp (& processed_bits, & expected_bits)) {
      const int cmp = cb_cmp (& processed_bits, & expected_bits);

      out_err_check_int (
        fprintf (stderr, "ERROR: %s: bit count discrepancy detected.%s",
          filename, CRC_EOL
        )
      );
      out_err_check_int (
        fprintf (stderr, "       Expected: ")
      );
      cb_printf (stderr, & expected_bits);
      out_err_check_int (
        fprintf (stderr, " bits, Got: ")
      );
      cb_printf (stderr, & processed_bits);
      out_err_check_int (
        fprintf (stderr, " bits.%s",
          CRC_EOL
        )
      );

      if (1 == cmp) {
        cb_sub_counter (& processed_bits, & expected_bits);
        out_err_check_int (
          fprintf (stderr, "       Bit count was above expected by ")
        );
        cb_printf (stderr, & processed_bits);
        out_err_check_int (
          fprintf (stderr, " bits.%s",
            CRC_EOL
          )
        );
      } else {
        cb_sub_counter (& expected_bits, & processed_bits);
        out_err_check_int (
          fprintf (stderr, "       Bit count was below expected by ")
        );
        cb_printf (stderr, & expected_bits);
        out_err_check_int (
          fprintf (stderr, " bits.%s",
            CRC_EOL
          )
        );
      }

      g_anyerr = 1;
      g_fileerr = 1;

      return;
    }
  }

  for (i = 0; 8 > i; i++) {
    const int ch = hexdigits [(int)(v & 0xF)];

    buf [(long)(7 - i)] = (char)ch;
    v >>= 4;
  }

  buf [8] = '\0';

  out_err_check_int (
    fprintf (stdout, "%s\t\tCRC=%s",
      filename, buf
    )
  );

  if (0 != g_verbose || 0 != auto_v ||
     (0 != actually_padded && 0 != g_pad_auto)) {
    out_err_check_int (
      fprintf (stdout, "\t# ")
    );
    cb_printf (stdout, & processed_bits);
    out_err_check_int (
      fprintf (stdout, " bits (")
    );
    cb_printf (stdout, & processed_chars);
    out_err_check_int (
      fprintf (stdout, " %d-bit character%s",
        local_use_cb, (cb_is_one (& processed_chars) ? "" : "s")
      )
    );

    if (0 != is_all_zeros && 0 == bit_check_failed &&
        0 != cb_is_zero (& processed_bits))
      out_err_check_int (
        fprintf (stdout, " - empty")
      );

    if (0 != actually_padded && (0 != pad || 0 != g_pad_auto))
      out_err_check_int (
        fprintf (stdout, " - padded")
      );

    out_err_check_int (
      fprintf (stdout, ")")
    );
  }

  out_err_check_int (
    fprintf (stdout, "%s",
      CRC_EOL
    )
  );
}

/******************************************************************************/

static void
#ifdef ANSI_COMPILER
usage (
  const char * const progname,
  const int cb)
#else
usage (progname, cb)
  const char * const progname;
  const int cb;
#endif
{
  out_err_check_int (
    fprintf (stderr, "Usage: %s [option(s)...] <file> [file(s)...]%s",
      progname, CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "Options:%s",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "  --limit=N        Stops processing after N bits%s",
        CRC_EOL
    )
  );
#ifdef CRC_CPM
  out_err_check_int (
    fprintf (stderr,
      "  --lrbc           Limits to the CP/M Last Record Byte Count%s",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "  --lrbc=isx       Use the ISX LRBC convention (unused bytes)%s",
        CRC_EOL
    )
  );
#endif
  out_err_check_int (
    fprintf (stderr,
      "  --bits=N         Reads as N bits per storage character%s",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "  --bits=auto      Automatically determines significant bits%s",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "  --pad            Pads trailing bits with zeros%s",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "  --pad=auto       Automatically pads bits when necessary%s",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "  --auto           Enables '--bits=auto --pad=auto --verbose'%s",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "  --verbose, -v    Verbose mode (shows processing details)%s",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      "  --help, -h       Shows this help and usage text%s",
        CRC_EOL
    )
  );

#ifdef CRC_CPM
  out_err_check_int (
    fprintf (stderr,
      "%sNOTE: On CP/M 3 or later (CP/M-Plus)",
        CRC_EOL
    )
  );
  out_err_check_int (
    fprintf (stderr,
      ", '--auto' also enables '--lrbc'.%s",
        CRC_EOL
    )
  );
#endif

  if (8 != cb)
    out_err_check_int (
      fprintf (stderr,
        "%sNOTE: This system has a character size of %d-bits.%s",
          CRC_EOL, cb, CRC_EOL
      )
    );

  if (8 < cb)
    out_err_check_int (
      fprintf (stderr,
        "Use '--bits=8' to process 8-bit input data on this system.%s",
          CRC_EOL
      )
    );
}

/******************************************************************************/

#ifdef __MSDOS__
# ifdef __AZTEC_C_52T__
#  ifdef ANSI_COMPILER
void aztec_expand_wildcards (int * argc, char * * * argv);
#  else
void aztec_expand_wildcards ();
#  endif
# endif
#endif

/******************************************************************************/

#ifdef __DMC__
# ifdef __MSDOS__
#  ifdef ANSI_COMPILER
void dos_expand_wildcards (int * argc, char * * * argv);
#  else
void dos_expand_wildcards ();
#  endif
# endif
#endif

/******************************************************************************/

#ifdef __WATCOMC__
# ifdef __MSDOS__
#  ifdef ANSI_COMPILER
void dos_expand_wildcards (int * argc, char * * * argv);
#  else
void dos_expand_wildcards ();
#  endif
# endif
#endif

/******************************************************************************/

#ifdef TCDWC
# ifdef ANSI_COMPILER
void tc_expand_wildcards (int * argc, char * * * argv);
# else
void tc_expand_wildcards ();
# endif
#endif

/******************************************************************************/

#ifdef __BCC__
void dev86_expand_wildcards (argc, argv);
#endif

/******************************************************************************/

#ifdef __IA16_SYS_MSDOS
# ifdef __GNUC__
#  ifdef ANSI_COMPILER
void ia16_expand_wildcards (int * argc, char * * * argv);
#  else
void ia16_expand_wildcards ();
#  endif
# endif
#endif

/******************************************************************************/

#ifdef _WIN32
# ifdef ANSI_COMPILER
void win32_expand_wildcards (int * argc, char * * * argv);
# else
void win32_expand_wildcards ();
# endif
#endif

/******************************************************************************/

#ifdef AOSDWC
# ifdef ANSI_COMPILER
void amiga_expand_wildcards (int * argc, char * * * argv);
# else
void amiga_expand_wildcards ();
# endif
#endif

/******************************************************************************/

#ifdef TOSDWC
# ifdef ANSI_COMPILER
void tos_expand_wildcards (int * argc, char * * * argv);
# else
void tos_expand_wildcards ();
# endif
#endif

/******************************************************************************/

int
#ifdef _MSC_VER
# ifndef _WIN32
#  if _MSC_VER < 900
cdecl
#  endif
# endif
#endif
#ifdef ANSI_COMPILER
main (
  int argc,
  char * argv [])
#else
main (argc, argv)
  int argc;
  char * argv [];
#endif
{
  static crc_t crc_table [256];
  crc_t inmask;
  int use_cb, j;
  int found = 0;
  int stop = 0;
  int pad = 0;
  int process_bits = 0;
#ifdef CRC_CPM
  int opt_auto = 0;
  int opt_lrbc = 0;
  int opt_lrbc_isx = 0;
#endif
#ifdef CPM_WILDCARD
  int processed = 0;
#endif
  counter_t lim_bits;
  const char * filename;
  crc_t mask32 = 0;
  crc_t v;
  const int cb = char_bits ();
  const int ub = crc_t_bits ();
  const int uib = unsigned_int_bits ();
  const unsigned int batch_limit = safe_batch_limit ();
  const char * const progname =
#ifdef CRC_CPM
    CRC_NAME;
#else
    (((char **)0 != argv && (char *)0 != argv [0] && '\0' != * argv [0])
      ? ('\0' == argv [0] [1] ? CRC_NAME : argv [0]) : CRC_NAME);
#endif

#ifdef __MSDOS__
# ifdef __AZTEC_C_52T__
  aztec_expand_wildcards (& argc, & argv);
# endif
# ifdef __DMC__
  dos_expand_wildcards (& argc, & argv);
# endif
# ifdef __WATCOMC__
  dos_expand_wildcards (& argc, & argv);
# endif
# ifdef __IA16_SYS_MSDOS
#  ifdef __GNUC__
  ia16_expand_wildcards (& argc, & argv);
#  endif
# endif
# ifdef __BCC__
  dev86_expand_wildcards (& argc, & argv);
# endif
#endif

#ifdef TCDWC
  tc_expand_wildcards (& argc, & argv);
#endif

#ifdef _WIN32
  win32_expand_wildcards (& argc, & argv);
#endif

#ifdef TOSDWC
  tos_expand_wildcards (& argc, & argv);
#endif

#ifdef AOSDWC
  amiga_expand_wildcards (& argc, & argv);
#endif

  cb_zero (& lim_bits);

#ifndef __Z88DK
# ifndef __COMPILER_KCC__
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);
# endif
#endif

  if (16 > uib) {
    out_err_check_int (
      fprintf (stderr,
        "FATAL: Non-conforming %d-bit unsigned int (must be >= 16).%s",
          uib, CRC_EOL
      )
    );

    return EXIT_FAILURE;
  }

  if (32 > ub) {
    out_err_check_int (
      fprintf (stderr,
        "FATAL: Non-conforming %d-bit crc_t type (must be >= 32).%s",
          ub, CRC_EOL
      )
    );

    return EXIT_FAILURE;
  }

  for (j = 0; 32 > j; j++)
    mask32 = (mask32 << 1) | 1;

  v = mask32;

  /*cppcheck-suppress knownConditionTrueFalse*/
  if (v == (v >> 1)) { /* //-V547 */
    out_err_check_int (
      fprintf (stderr,
        "FATAL: Broken compiler: logical right-shift is not logical.%s",
          CRC_EOL
      )
    );

    return EXIT_FAILURE;
  }

  init_crc_table (crc_table);

#ifdef SELFTEST
  test_crc_table (crc_table, mask32);
#endif

  if ((8 > cb) || (32 < cb)) {
    out_err_check_int (
      fprintf (stderr,
        "FATAL: Unsupported %d-bit character size (must be 8-32).%s",
          cb, CRC_EOL
      )
    );

    return EXIT_FAILURE;
  }

  if (2 > argc) {
    usage (progname, cb);

    return EXIT_FAILURE;
  }

  for (j = 1; argc > j; j++) {
    if (0 == stop && 0 == xstrcasecmp (argv [j], "--")) {
      stop = 1;

      continue;
    }

    if (0 == stop && (0 == xstrcasecmp (argv [j], "-h") ||
                      0 == xstrcasecmp (argv [j], "--help"))) {
      usage (progname, cb);

      return 0;
    }

    if (0 == stop && 0 == xstrncasecmp (argv [j], "--bits=", 7)) {
      if (0 == xstrcasecmp (argv [j] + 7, "auto")) {
        g_bits_auto = 1;
        process_bits = 0;

        continue;
      }

      {
        counter_t bits_cb;
        unsigned long bits = 0;

        if (0 == cb_parse (& bits_cb, argv [j] + 7))
          goto bits_error;

        {
          int k;

          for (k = MAX_CB_DIGITS - 1; 0 <= k; k--) {
            if (bits > (unsigned long)(ub - 8))
              break;

            bits = bits * 10 + (unsigned long)bits_cb.d [k];
          }
        }

        if (0 == bits || bits > (unsigned long)(ub - 8))
          goto bits_error;

        process_bits = (int)bits;
        g_bits_auto = 0;

        continue;

bits_error:
        out_err_check_int (
          fprintf (stderr, "FATAL: --bits must be a positive integer ")
        );
        out_err_check_int (
          fprintf (stderr, "between 1 and %d (or 'auto').%s",
            (ub - 8), CRC_EOL
          )
        );

        return EXIT_FAILURE;
      }
    }

    if (0 == stop && 0 == xstrcasecmp (argv [j], "--pad")) {
      pad = 1;
      g_pad_auto = 0;

      continue;
    }

    if (0 == stop && 0 == xstrcasecmp (argv [j], "--pad=auto")) {
      g_pad_auto = 1;
      pad = 0;

      continue;
    }

    if (0 == stop && 0 == xstrcasecmp (argv [j], "--auto")) {
      g_verbose = 1;
      g_bits_auto = 1;
      g_pad_auto = 1;
#ifdef CRC_CPM
      opt_auto = 1;
#endif

      continue;
    }

    if (0 == stop && (0 == xstrcasecmp (argv [j], "-v") ||
                      0 == xstrcasecmp (argv [j], "--verbose"))) {
      g_verbose = 1;

      continue;
    }

    if (0 == stop && 0 == xstrncasecmp (argv [j], "--limit=", 8)) {
      if (0 == cb_parse (& lim_bits, argv [j] + 8)) {
        out_err_check_int (
          fprintf (stderr, "FATAL: --limit must be a positive integer.%s",
            CRC_EOL
          )
        );

        return EXIT_FAILURE;
      }

      if (0 != cb_is_zero (& lim_bits)) {
        out_err_check_int (
          fprintf (stderr, "FATAL: --limit must be greater than zero.%s",
            CRC_EOL
          )
        );

        return EXIT_FAILURE;
      }

      continue;
    }

#ifdef CRC_CPM
    if (0 == stop && 0 == xstrcasecmp (argv [j], "--lrbc")) {
      opt_lrbc = 1;

      continue;
    }

    if (0 == stop && 0 == xstrncasecmp (argv [j], "--lrbc=", 7)) {
      if (0 == xstrcasecmp (argv [j] + 7, "isx")) {
        opt_lrbc_isx = 1;

        continue;
      }

      out_err_check_int (
        fprintf (stderr, "FATAL: --lrbc takes no value, or '=isx'.%s",
          CRC_EOL
        )
      );

      return EXIT_FAILURE;
    }
#endif

    if (0 == stop && '-' == argv [j] [0]) {
      out_err_check_int (
        fprintf (stderr, "FATAL: Unknown option: %s.%s",
          argv [j], CRC_EOL
        )
      );
      usage (progname, cb);

      return EXIT_FAILURE;
    }

    found++;
  }

  if (0 == found) {
    out_err_check_int (
      fprintf (stderr, "FATAL: No filename provided.%s",
        CRC_EOL
      )
    );
    usage (progname, cb);

    return EXIT_FAILURE;
  }

  use_cb = ((0 < process_bits) ? process_bits : cb);

  if (use_cb > cb) {
    out_err_check_int (
      fprintf (stderr, "WARNING: ")
    );
    out_err_check_int (
      fprintf (stderr,
        "Specified --bits=%d is greater than host %d-bit character size.%s",
          use_cb, cb, CRC_EOL
      )
    );
    out_err_check_int (
      fprintf (stderr, "         ")
    );
    out_err_check_int (
      fprintf (stderr,
        "Each %d-bit character read will be zero-filled to reach %d bits.%s",
          cb, use_cb, CRC_EOL
      )
    );
  }

  /*cppcheck-suppress knownConditionTrueFalse*/
  if ((1 > use_cb) || ((ub - 8) < use_cb)) { /* //-V560 */
    out_err_check_int (
      fprintf (stderr,
        "FATAL: Unsupported %d-bit processing with %d-bit crc_t type.%s",
          use_cb, ub, CRC_EOL
      )
    );

    return EXIT_FAILURE;
  }

  inmask = make_mask (use_cb);

#ifdef CRC_CPM
  if (0 != opt_lrbc_isx) {
    g_cpm_lrbc = 2;
    g_cpm_lrbc_explicit = 1;
  } else if (0 != opt_lrbc) {
    g_cpm_lrbc = 1;
    g_cpm_lrbc_explicit = 1;
  } else if (0 != opt_auto) {
    g_cpm_lrbc = 1;
    g_cpm_lrbc_explicit = 0;
  }
#endif

  stop = 0;

  for (j = 1; argc > j; j++) {
    if (0 == stop && 0 == xstrcasecmp (argv [j], "--")) {
      stop = 1;

      continue;
    }

    if (0 == stop && '-' == argv [j] [0])
      continue;

    filename = argv [j];

#ifdef __Z88DK
# ifdef __CPM__
    if (is_wildcard (filename)) {
      int x;
      int match_found = 0;

      if (0 == (x = dir_move_first ()))
        while (0 == x) {
          if (wcmatch ((char *)filename, (char *)dir_get_entry_name ()))
            if (0 == dir_get_entry_type ()) {
              match_found = 1;
              process_file (dir_get_entry_name (),
                crc_table, cb, ub, use_cb, mask32, inmask, pad, & lim_bits,
                  batch_limit);

              if (0 == g_fileerr)
                processed++;
            }

          x = dir_move_next ();
      }

      if (0 == match_found)
        out_err_check_int (
          fprintf (stderr, "WARNING: No wildcard match for %s.%s",
            filename, CRC_EOL
          )
        );
    } else
# endif
#endif
#ifdef CPM_BDOS_WILDCARD
    if (is_wildcard (filename)) {
      static unsigned char htc_dma [128];
      static unsigned char htc_fcb [36];
      static char htc_names [64] [13];
      int htc_cnt, htc_r, htc_i;
      int match_found = 0;

      cpm_setfcb_wild (htc_fcb, filename);
      (void)bdos (26, BDOS_FCB (htc_dma));
      htc_cnt = 0;
      htc_r = bdos (17, BDOS_FCB (htc_fcb)) & 0xff;

      while (0xff != htc_r && 64 > htc_cnt) {
        cpm_dir_to_name (htc_names [htc_cnt],
          & htc_dma [htc_r * 32]);
        htc_cnt++;
        htc_r = bdos (18, BDOS_FCB (htc_fcb)) & 0xff;
      }

      for (htc_i = 0; htc_cnt > htc_i; htc_i++) {
        match_found = 1;
        process_file (htc_names [htc_i],
          crc_table, cb, ub, use_cb, mask32, inmask, pad, & lim_bits,
            batch_limit);

        if (0 == g_fileerr)
          processed++;
      }

      if (0 == match_found)
        out_err_check_int (
          fprintf (stderr, "WARNING: No wildcard match for %s.%s",
            filename, CRC_EOL
          )
        );
    } else
#endif
    {
      process_file (filename,
        crc_table, cb, ub, use_cb, mask32, inmask, pad, & lim_bits,
          batch_limit);

#ifdef CPM_WILDCARD
      if (0 == g_fileerr)
        processed++;
#endif
    }
  }

#ifdef CPM_WILDCARD
  if (0 == processed) {
    out_err_check_int (
      fprintf (stderr, "ERROR: No files processed.%s",
        CRC_EOL
      )
    );
    g_anyerr = 1;
  }
#endif

  if (0 != g_out_err) {
    (void)fprintf (stderr, "%sWARNING: %ld output operation%s failed; ",
      CRC_EOL, g_out_err, (1 == g_out_err ? "" : "s")
    );
    (void)fprintf (stderr, "displayed results may not be reliable!%s",
      CRC_EOL
    );

    return EXIT_FAILURE;
  }

  return ((0 != g_anyerr) ? EXIT_FAILURE : EXIT_SUCCESS);
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
