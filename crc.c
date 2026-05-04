/*
 * IBM PC-DOS CRC.EXE-compatible CRC calculator
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

#ifdef multics
typedef unsigned int crc_t;
#else
typedef unsigned long crc_t;
#endif

/******************************************************************************/

#ifdef multics
# ifdef ANSI_COMPILER
#  undef ANSI_COMPILER
# endif
# ifdef USE_PSYSERROR
#  undef USE_PSYSERROR
# endif
# define USE_PSYSERROR
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

#ifdef __Z88DK
# ifdef __CPM__
#  include <unistd.h>
#  ifdef ANSI_COMPILER
#   undef ANSI_COMPILER
#  endif
#  ifdef USE_PSYSERROR
#   undef USE_PSYSERROR
#  endif
#  ifndef BUFSIZ
#   define BUFSIZ 128
#  endif
#  ifndef MAX_CB_DIGITS
#   define MAX_CB_DIGITS 8 /* ~12 MiB */
#  endif
# endif
#endif

/******************************************************************************/

#ifndef MAX_CB_DIGITS
# define MAX_CB_DIGITS 12 /* ~117 GiB */
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

#include <stdio.h>

#ifndef multics
# include <stdlib.h>
#endif

#ifdef USE_ERRNO
# include <errno.h>
#else
# ifdef errno
#  undef errno
# endif
# define errno 0
#endif

#ifdef ANSI_COMPILER
# include <string.h>
#else
# ifdef FORCE_STRERROR
extern char * strerror ();
# endif
#endif

/******************************************************************************/

#ifndef ANSI_COMPILER
# ifndef __CPPCHECK__
#  ifdef const
#   undef const
#  endif
#  define const /* //-V1059 */
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

typedef struct {
  unsigned char d [MAX_CB_DIGITS];
} counter_t;

/******************************************************************************/

static int g_anyerr = 0;
static int g_fileerr = 0;
static int g_verbose = 0;
static int g_bits_auto = 0;
static int g_pad_auto = 0;

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
  int i;

  for (i = 0; MAX_CB_DIGITS > i; i++)
    c -> d [i] = 0;
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
  int i;

  for (i = 0; MAX_CB_DIGITS > i; i++)
    dst -> d [i] = src -> d [i];
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
  int i = 0;
  unsigned int val = v;

  while (0 != val) {
    unsigned int digit_to_sub;

    {
      unsigned int q = 0;
      unsigned int r = val;

      while (10 <= r) {
        r -= 10;
        q++;
      }

      digit_to_sub = r;
      val = q;
    }

    if (MAX_CB_DIGITS <= i)
      return 0;

    if (c -> d [i] >= digit_to_sub)
      c -> d [i] -= (unsigned char)digit_to_sub;
    else {
      int j = i + 1;

      while (MAX_CB_DIGITS > j && 0 == c -> d [j]) {
        c -> d [j] = 9;
        j++;
      }

      if (MAX_CB_DIGITS <= j)
        return 0;

      c -> d [j]--;
      c -> d [i] = (unsigned char)(10 + c -> d [i] - digit_to_sub);
    }

    i++;
  }

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
  int started = 0;

  while (0 < i && 0 == c -> d [i])
    i--;

  for (; 0 <= i; i--) {
    const int ch = hexdigits [c -> d [i]];

    /* cppcheck-suppress knownConditionTrueFalse */
    if (NULL != fp || (FILE *)0 != fp) /* //-V560 */
      (void)fputc (ch, fp);
    else
      (void)printf ("%c", ch);

    started = 1;
  }

  if (0 == started) {
    /* cppcheck-suppress knownConditionTrueFalse */
    if (NULL != fp || (FILE *)0 != fp) /* //-V560 */
      (void)fputc ('0', fp);
    else
      (void)printf ("0");
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
  /* cppcheck-suppress constStatement */
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
      ret = (c1 < c2) ? -1 : 1;
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
      ret = (c1 < c2) ? -1 : 1;
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
static int
#  ifdef ANSI_COMPILER
is_wildcard (
  const char * s)
#  else
is_wildcard (s)
  const char * s;
#  endif
{
  int c;

  if ((const char *)0 == s)
    return 0;

  while ('\0' != (c = * s++))
    if ('*' == c || '?' == c)
      return 1;

  return 0;
}
# endif
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
# define TRIM_RING 2

static char *
# ifdef ANSI_COMPILER
trim_str (
  const char * const s)
# else
trim_str (s)
  const char * const s;
# endif
{
  /* cppcheck-suppress constVariable */
  static char bufs [TRIM_RING] [TRIM_BUFSIZE];
  static int idx = 0;

  /* cppcheck-suppress constStatement */
  const char * p;
  /* cppcheck-suppress constStatement */
  const char * q;
  /* cppcheck-suppress constStatement */
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

  while (' ' == * p || '\t' == * p)
    p++;

  if ('\0' == * p) {
    buf [0] = '\0';
    return buf;
  }

  q = p;
  last = p;

  while ('\0' != * q) {
    if (' ' != * q && '\t' != * q)
      last = q;

    q++;
  }

  d = buf;

  while (p <= last && d < buf + (TRIM_BUFSIZE - 1))
    * d++ = * p++;

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
  (void)fprintf (stderr, "ERROR: %s", m);

  /* cppcheck-suppress knownConditionTrueFalse */
  if (NULL != n && (char *)0 != n) /* //-V560 */
    (void)fprintf (stderr, " %s", n);

  if (0 != e) {
    (void)fprintf (stderr, " (Error %d", e);
#ifdef ANSI_COMPILER
    (void)fprintf (stderr, ": %s", trim_str (strerror (e)));
#else
# ifdef FORCE_STRERROR
    (void)fprintf (stderr, ": %s", trim_str (strerror (e)));
# else
#  ifdef USE_PSYSERROR
    (void)fprintf (stderr, ": %s", trim_str (psyserror (e)));
#  endif
# endif
#endif
    (void)fprintf (stderr, ")");
  }

  (void)fprintf (stderr, ".\n");

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
  tbl   [0] = 0x00000000;  tbl   [1] = 0x51F9D3DE;
  tbl   [2] = 0xA3F3A7BC;  tbl   [3] = 0xF20A7462;
  tbl   [4] = 0x161E9CA6;  tbl   [5] = 0x47E74F78;
  tbl   [6] = 0xB5ED3B1A;  tbl   [7] = 0xE414E8C4;
  tbl   [8] = 0x2C3D394C;  tbl   [9] = 0x7DC4EA92;
  tbl  [10] = 0x8FCE9EF0;  tbl  [11] = 0xDE374D2E;
  tbl  [12] = 0x3A23A5EA;  tbl  [13] = 0x6BDA7634;
  tbl  [14] = 0x99D00256;  tbl  [15] = 0xC829D188;
  tbl  [16] = 0x587A7298;  tbl  [17] = 0x0983A146;
  tbl  [18] = 0xFB89D524;  tbl  [19] = 0xAA7006FA;
  tbl  [20] = 0x4E64EE3E;  tbl  [21] = 0x1F9D3DE0;
  tbl  [22] = 0xED974982;  tbl  [23] = 0xBC6E9A5C;
  tbl  [24] = 0x74474BD4;  tbl  [25] = 0x25BE980A;
  tbl  [26] = 0xD7B4EC68;  tbl  [27] = 0x864D3FB6;
  tbl  [28] = 0x6259D772;  tbl  [29] = 0x33A004AC;
  tbl  [30] = 0xC1AA70CE;  tbl  [31] = 0x9053A310;
  tbl  [32] = 0xB0F4E530;  tbl  [33] = 0xE10D36EE;
  tbl  [34] = 0x1307428C;  tbl  [35] = 0x42FE9152;
  tbl  [36] = 0xA6EA7996;  tbl  [37] = 0xF713AA48;
  tbl  [38] = 0x0519DE2A;  tbl  [39] = 0x54E00DF4;
  tbl  [40] = 0x9CC9DC7C;  tbl  [41] = 0xCD300FA2;
  tbl  [42] = 0x3F3A7BC0;  tbl  [43] = 0x6EC3A81E;
  tbl  [44] = 0x8AD740DA;  tbl  [45] = 0xDB2E9304;
  tbl  [46] = 0x2924E766;  tbl  [47] = 0x78DD34B8;
  tbl  [48] = 0xE88E97A8;  tbl  [49] = 0xB9774476;
  tbl  [50] = 0x4B7D3014;  tbl  [51] = 0x1A84E3CA;
  tbl  [52] = 0xFE900B0E;  tbl  [53] = 0xAF69D8D0;
  tbl  [54] = 0x5D63ACB2;  tbl  [55] = 0x0C9A7F6C;
  tbl  [56] = 0xC4B3AEE4;  tbl  [57] = 0x954A7D3A;
  tbl  [58] = 0x67400958;  tbl  [59] = 0x36B9DA86;
  tbl  [60] = 0xD2AD3242;  tbl  [61] = 0x8354E19C;
  tbl  [62] = 0x715E95FE;  tbl  [63] = 0x20A74620;
  tbl  [64] = 0x301019BE;  tbl  [65] = 0x61E9CA60;
  tbl  [66] = 0x93E3BE02;  tbl  [67] = 0xC21A6DDC;
  tbl  [68] = 0x260E8518;  tbl  [69] = 0x77F756C6;
  tbl  [70] = 0x85FD22A4;  tbl  [71] = 0xD404F17A;
  tbl  [72] = 0x1C2D20F2;  tbl  [73] = 0x4DD4F32C;
  tbl  [74] = 0xBFDE874E;  tbl  [75] = 0xEE275490;
  tbl  [76] = 0x0A33BC54;  tbl  [77] = 0x5BCA6F8A;
  tbl  [78] = 0xA9C01BE8;  tbl  [79] = 0xF839C836;
  tbl  [80] = 0x686A6B26;  tbl  [81] = 0x3993B8F8;
  tbl  [82] = 0xCB99CC9A;  tbl  [83] = 0x9A601F44;
  tbl  [84] = 0x7E74F780;  tbl  [85] = 0x2F8D245E;
  tbl  [86] = 0xDD87503C;  tbl  [87] = 0x8C7E83E2;
  tbl  [88] = 0x4457526A;  tbl  [89] = 0x15AE81B4;
  tbl  [90] = 0xE7A4F5D6;  tbl  [91] = 0xB65D2608;
  tbl  [92] = 0x5249CECC;  tbl  [93] = 0x03B01D12;
  tbl  [94] = 0xF1BA6970;  tbl  [95] = 0xA043BAAE;
  tbl  [96] = 0x80E4FC8E;  tbl  [97] = 0xD11D2F50;
  tbl  [98] = 0x23175B32;  tbl  [99] = 0x72EE88EC;
  tbl [100] = 0x96FA6028;  tbl [101] = 0xC703B3F6;
  tbl [102] = 0x3509C794;  tbl [103] = 0x64F0144A;
  tbl [104] = 0xACD9C5C2;  tbl [105] = 0xFD20161C;
  tbl [106] = 0x0F2A627E;  tbl [107] = 0x5ED3B1A0;
  tbl [108] = 0xBAC75964;  tbl [109] = 0xEB3E8ABA;
  tbl [110] = 0x1934FED8;  tbl [111] = 0x48CD2D06;
  tbl [112] = 0xD89E8E16;  tbl [113] = 0x89675DC8;
  tbl [114] = 0x7B6D29AA;  tbl [115] = 0x2A94FA74;
  tbl [116] = 0xCE8012B0;  tbl [117] = 0x9F79C16E;
  tbl [118] = 0x6D73B50C;  tbl [119] = 0x3C8A66D2;
  tbl [120] = 0xF4A3B75A;  tbl [121] = 0xA55A6484;
  tbl [122] = 0x575010E6;  tbl [123] = 0x06A9C338;
  tbl [124] = 0xE2BD2BFC;  tbl [125] = 0xB344F822;
  tbl [126] = 0x414E8C40;  tbl [127] = 0x10B75F9E;
  tbl [128] = 0x6020337C;  tbl [129] = 0x31D9E0A2;
  tbl [130] = 0xC3D394C0;  tbl [131] = 0x922A471E;
  tbl [132] = 0x763EAFDA;  tbl [133] = 0x27C77C04;
  tbl [134] = 0xD5CD0866;  tbl [135] = 0x8434DBB8;
  tbl [136] = 0x4C1D0A30;  tbl [137] = 0x1DE4D9EE;
  tbl [138] = 0xEFEEAD8C;  tbl [139] = 0xBE177E52;
  tbl [140] = 0x5A039696;  tbl [141] = 0x0BFA4548;
  tbl [142] = 0xF9F0312A;  tbl [143] = 0xA809E2F4;
  tbl [144] = 0x385A41E4;  tbl [145] = 0x69A3923A;
  tbl [146] = 0x9BA9E658;  tbl [147] = 0xCA503586;
  tbl [148] = 0x2E44DD42;  tbl [149] = 0x7FBD0E9C;
  tbl [150] = 0x8DB77AFE;  tbl [151] = 0xDC4EA920;
  tbl [152] = 0x146778A8;  tbl [153] = 0x459EAB76;
  tbl [154] = 0xB794DF14;  tbl [155] = 0xE66D0CCA;
  tbl [156] = 0x0279E40E;  tbl [157] = 0x538037D0;
  tbl [158] = 0xA18A43B2;  tbl [159] = 0xF073906C;
  tbl [160] = 0xD0D4D64C;  tbl [161] = 0x812D0592;
  tbl [162] = 0x732771F0;  tbl [163] = 0x22DEA22E;
  tbl [164] = 0xC6CA4AEA;  tbl [165] = 0x97339934;
  tbl [166] = 0x6539ED56;  tbl [167] = 0x34C03E88;
  tbl [168] = 0xFCE9EF00;  tbl [169] = 0xAD103CDE;
  tbl [170] = 0x5F1A48BC;  tbl [171] = 0x0EE39B62;
  tbl [172] = 0xEAF773A6;  tbl [173] = 0xBB0EA078;
  tbl [174] = 0x4904D41A;  tbl [175] = 0x18FD07C4;
  tbl [176] = 0x88AEA4D4;  tbl [177] = 0xD957770A;
  tbl [178] = 0x2B5D0368;  tbl [179] = 0x7AA4D0B6;
  tbl [180] = 0x9EB03872;  tbl [181] = 0xCF49EBAC;
  tbl [182] = 0x3D439FCE;  tbl [183] = 0x6CBA4C10;
  tbl [184] = 0xA4939D98;  tbl [185] = 0xF56A4E46;
  tbl [186] = 0x07603A24;  tbl [187] = 0x5699E9FA;
  tbl [188] = 0xB28D013E;  tbl [189] = 0xE374D2E0;
  tbl [190] = 0x117EA682;  tbl [191] = 0x4087755C;
  tbl [192] = 0x50302AC2;  tbl [193] = 0x01C9F91C;
  tbl [194] = 0xF3C38D7E;  tbl [195] = 0xA23A5EA0;
  tbl [196] = 0x462EB664;  tbl [197] = 0x17D765BA;
  tbl [198] = 0xE5DD11D8;  tbl [199] = 0xB424C206;
  tbl [200] = 0x7C0D138E;  tbl [201] = 0x2DF4C050;
  tbl [202] = 0xDFFEB432;  tbl [203] = 0x8E0767EC;
  tbl [204] = 0x6A138F28;  tbl [205] = 0x3BEA5CF6;
  tbl [206] = 0xC9E02894;  tbl [207] = 0x9819FB4A;
  tbl [208] = 0x084A585A;  tbl [209] = 0x59B38B84;
  tbl [210] = 0xABB9FFE6;  tbl [211] = 0xFA402C38;
  tbl [212] = 0x1E54C4FC;  tbl [213] = 0x4FAD1722;
  tbl [214] = 0xBDA76340;  tbl [215] = 0xEC5EB09E;
  tbl [216] = 0x24776116;  tbl [217] = 0x758EB2C8;
  tbl [218] = 0x8784C6AA;  tbl [219] = 0xD67D1574;
  tbl [220] = 0x3269FDB0;  tbl [221] = 0x63902E6E;
  tbl [222] = 0x919A5A0C;  tbl [223] = 0xC06389D2;
  tbl [224] = 0xE0C4CFF2;  tbl [225] = 0xB13D1C2C;
  tbl [226] = 0x4337684E;  tbl [227] = 0x12CEBB90;
  tbl [228] = 0xF6DA5354;  tbl [229] = 0xA723808A;
  tbl [230] = 0x5529F4E8;  tbl [231] = 0x04D02736;
  tbl [232] = 0xCCF9F6BE;  tbl [233] = 0x9D002560;
  tbl [234] = 0x6F0A5102;  tbl [235] = 0x3EF382DC;
  tbl [236] = 0xDAE76A18;  tbl [237] = 0x8B1EB9C6;
  tbl [238] = 0x7914CDA4;  tbl [239] = 0x28ED1E7A;
  tbl [240] = 0xB8BEBD6A;  tbl [241] = 0xE9476EB4;
  tbl [242] = 0x1B4D1AD6;  tbl [243] = 0x4AB4C908;
  tbl [244] = 0xAEA021CC;  tbl [245] = 0xFF59F212;
  tbl [246] = 0x0D538670;  tbl [247] = 0x5CAA55AE;
  tbl [248] = 0x94838426;  tbl [249] = 0xC57A57F8;
  tbl [250] = 0x3770239A;  tbl [251] = 0x6689F044;
  tbl [252] = 0x829D1880;  tbl [253] = 0xD364CB5E;
  tbl [254] = 0x216EBF3C;  tbl [255] = 0x70976CE2;
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
      (void)fprintf (stderr,
        "FATAL: CRC table mismatch at index %d (expected %08lX, got %08lX).\n",
        i, (unsigned long)c, (unsigned long)tbl [i]);
      exit (EXIT_FAILURE);
    }
  }
}
#endif

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
charbits (void)
#else
charbits ()
#endif
{
  unsigned char c = 1;
  unsigned char last_c = 0;
  int bits = 0;

  while (c > last_c) {
    bits++;

    last_c = c;
    c = (unsigned char)(c << 1);
    c |= 1;
  }

  return bits;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
unsigned_int_bits (void)
#else
unsigned_int_bits ()
#endif
{
  unsigned int u = 1;
  unsigned int last_u = 0;
  int bits = 0;

  while (u > last_u) {
    bits++;

    last_u = u;
    u = u * 2 + 1;
  }

  return bits;
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
crc_t_bits (void)
#else
crc_t_bits ()
#endif
{
  crc_t v = 1;
  crc_t last_v = 0;
  int bits = 0;

  while (v > last_v) {
    bits++;

    last_v = v;
    v = v * 2 + 1;
  }

  return bits;
}

/******************************************************************************/

static unsigned int
#ifdef ANSI_COMPILER
safe_batch_limit (void)
#else
safe_batch_limit ()
#endif
{
  const int u_bits = unsigned_int_bits ();
  const int t_bits = crc_t_bits ();
  unsigned int u_max = 0;
  unsigned int limit;
  unsigned int max_inc;
  int i;

  for (i = 0; u_bits > i; i++)
    u_max = u_max * 2 + 1;

  max_inc = (8 < t_bits) ? (unsigned int)t_bits : 8;

  limit = 0;

  {
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

static crc_t
#ifdef ANSI_COMPILER
crc_update_byte (
  crc_t crc,
  const crc_t * const tbl,
  const crc_t mask32,
  const unsigned char b)
#else
crc_update_byte (crc, tbl, mask32, b)
  crc_t crc;
  const crc_t * const tbl;
  const crc_t mask32;
  const unsigned char b;
#endif
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

/******************************************************************************/

static crc_t
#ifdef ANSI_COMPILER
crc_update_buffer (
  crc_t crc,
  const crc_t * const tbl,
  const crc_t mask32,
  const unsigned char * const buf,
  const long n)
#else
crc_update_buffer (crc, tbl, mask32, buf, n)
  crc_t crc;
  const crc_t * const tbl;
  const crc_t mask32;
  const unsigned char * const buf;
  const long n;
#endif
{
  long i;

  for (i = 0; n > i; i++)
    crc = crc_update_byte (crc, tbl, mask32, buf [i]);

  return crc;
}

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
  const unsigned int batch_limit)
#else
compute_crc_fb (fp, filename, tbl, use_cb, mask32, inmask, pad, lim_bits,
                processed_bits, processed_chars, actually_padded, batch_limit)
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
#endif
{
  unsigned char rbuf [BUFSIZ];
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

  cb_copy (& rem_bits, lim_bits);

  for (;;) {
    while (8 > bib) {
      unsigned int bits_added;

      if (0 == cb_is_zero (lim_bits) && 0 != cb_is_zero (& rem_bits))
        break;

      if (0 == cb_is_zero (lim_bits) && 0 == cb_is_zero (& rem_bits)) {
        counter_t uc_cb;

        cb_zero (& uc_cb);
        if (0 == cb_add (& uc_cb, (unsigned int)use_cb)) {
          error_msg ("Counter logic error reading", filename, 0);
          goto done;
        }

        if (0 > cb_cmp (& rem_bits, & uc_cb))
          if (0 == pad && 0 == g_pad_auto) {
            (void)fprintf (stderr,
              "WARNING: --limit ended mid 8-bit octet; use --pad if needed.\n");
            cb_zero (& rem_bits);
            goto done;
          }
      }

      if (nread <= pos) {
        if (0 != feof (fp))
          goto done;

#ifndef USE_FREAD
        nread = 0;
#endif

#ifdef USE_FREAD
        nread = (long)fread (rbuf, 1, sizeof (rbuf), fp);
#else
        while ((long)sizeof (rbuf) > nread) {
          const int c = fgetc (fp);

          if (EOF == c)
            break;

          rbuf [nread] = (unsigned char)c;
          nread++;
        }
#endif

        if (0 != ferror (fp)) {
          error_msg ("Error reading", filename, errno);
          goto done;
        }

        if (0 == nread)
          goto done;

        pos = 0;
      }

      ch = rbuf [pos];
      pos++;

      acc_chars++;

      tmp = (crc_t)(unsigned char)ch;

      if (0 == cb_is_zero (lim_bits) && 0 == cb_is_zero (& rem_bits)) {
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

      if (0 == cb_is_zero (lim_bits) && 0 != cb_is_zero (& rem_bits))
        break;
    }

    if (0 == cb_is_zero (lim_bits) && 0 != cb_is_zero (& rem_bits))
      if (8 > bib)
        break;

    oct = (unsigned char)(buf & (crc_t)0xFF);
    crc = crc_update_byte (crc, tbl, mask32, oct);

    buf >>= 8;
    bib  -= 8;

    if (0 == cb_is_zero (lim_bits) && 0 != cb_is_zero (& rem_bits))
      if (0 == bib)
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

    if (0 < bib) {
      if (0 != pad || 0 != g_pad_auto) {
        oct = (unsigned char)(buf & (crc_t)0xFF);
        crc = crc_update_byte (crc, tbl, mask32, oct);
        * actually_padded = 1;
      } else {
        (void)fprintf (stderr, "WARNING: File ended with %d dangling bit%s ",
          bib, 1 == bib ? "" : "s");
        (void)fprintf (stderr, "(not a full 8-bit octet).\n");
        hinted = 1;
      }
    }

    if (0 == cb_is_zero (lim_bits) && 0 == cb_is_zero (& rem_bits)) {
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
        (void)fprintf (stderr, "WARNING: File ended after ");
        cb_printf (stderr, & used_bits);
        (void)fprintf (stderr, " bits, but --limit=");
        cb_printf (stderr, lim_bits);
        (void)fprintf (stderr, " was requested.\n");
        hinted = 1;
      } else {
        * actually_padded = 1;
      }
    }

    if (0 != hinted && 0 == pad && 0 == g_pad_auto)
      (void)fprintf (stderr,
        "         Use --pad to zero-fill remaining bits.\n");
  }

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
  const unsigned int batch_limit)
#else
compute_crc (fp, filename, tbl, cb, ub, use_cb, mask32, inmask, pad,
             lim_bits, processed_bits, processed_chars, actually_padded,
             batch_limit)
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
#endif
{
  /* cppcheck-suppress knownConditionTrueFalse */
  if (NULL == fp || (FILE *)0 == fp) { /* //-V560 */
    (void)fprintf (stderr,
      "FATAL: compute_crc called with NULL file pointer.\n");
    exit (EXIT_FAILURE);
  }

  if (32 > ub) {
    (void)fprintf (stderr,
      "FATAL: Need >=32-bit crc_t type, have %d bits.\n", ub);
    exit (EXIT_FAILURE);
  }

  if (8 == use_cb && 8 == cb) {
    unsigned char rbuf [BUFSIZ];
    crc_t crc = 0;
    counter_t rem_bits;

    cb_copy (& rem_bits, lim_bits);

    for (;;) {
      long nread, bytes_to_process;

      if (0 != feof (fp))
        break;

#ifndef USE_FREAD
      nread = 0;
#endif

#ifdef USE_FREAD
      nread = (long)fread (rbuf, 1, sizeof (rbuf), fp);
#else
      while ((long)sizeof (rbuf) > nread) {
        const int c = fgetc (fp);

        if (EOF == c)
          break;

        rbuf [nread++] = (unsigned char)c;
      }
#endif

      if (0 != ferror (fp)) {
        error_msg ("Error reading", filename, errno);
        return (crc_t)0;
      }

      if (0 == nread)
        break;

      bytes_to_process = nread;

      if (0 == cb_is_zero (lim_bits)) {
        long count = 0;
        counter_t eight;

        cb_zero (& eight);
        (void)cb_add (& eight, 8);

        while (0 <= cb_cmp (& rem_bits, & eight) && nread > count) {
          (void)cb_sub (& rem_bits, 8);
          count++;
        }

        bytes_to_process = count;
      }

      if (0 < bytes_to_process) {
        long remaining = bytes_to_process;

        while (remaining > 0) {
          unsigned int chunk = (remaining > (long)batch_limit) ?
            batch_limit : (unsigned int)remaining;

          if (0 == cb_add (processed_bits, chunk * 8)) {
            error_msg ("Bit counter overflow reading", filename, 0);
            return (crc_t)0;
          }

          if (0 == cb_add (processed_chars, chunk)) {
            error_msg ("Character counter overflow reading", filename, 0);
            return (crc_t)0;
          }

          remaining -= (long)chunk;
        }

        crc = crc_update_buffer (crc, tbl, mask32, rbuf, bytes_to_process);
      }

      if (0 == cb_is_zero (lim_bits) &&
          0 == cb_is_zero (& rem_bits) &&
          nread > bytes_to_process) {
        if (0 != pad || 0 != g_pad_auto) {
          unsigned char mask;
          int k;
          unsigned char final_byte = rbuf [bytes_to_process];
          unsigned long rb_val = 0;

          for (k = MAX_CB_DIGITS - 1; 0 <= k; k--)
            rb_val = rb_val * 10 + (unsigned long)rem_bits.d [k];

          mask = (unsigned char)make_mask ((int)rb_val);

          final_byte &= mask;

          if (0 == cb_add (processed_bits, (unsigned int)rb_val)) {
            error_msg ("Bit counter overflow reading", filename, 0);
            return (crc_t)0;
          }

          if (0 == cb_add (processed_chars, 1)) {
            error_msg ("Character counter overflow reading", filename, 0);
            return (crc_t)0;
          }

          crc = crc_update_byte (crc, tbl, mask32, final_byte);
          * actually_padded = 1;
          cb_zero (& rem_bits);
        } else {
          (void)fprintf (stderr, "WARNING: Input --limit caused truncation");
          (void)fprintf (stderr, " mid-character; use --pad if needed.\n");

          cb_zero (& rem_bits);
        }

        break;
      }

      if (0 != cb_is_zero (lim_bits) || 0 != cb_is_zero (& rem_bits))
        if (0 == cb_is_zero (lim_bits))
          break;
    }

    clearerr (fp);

    if (0 == cb_is_zero (lim_bits) && 0 == cb_is_zero (& rem_bits)) {
      if (0 != pad || 0 != g_pad_auto) {
        unsigned char zbuf [32];
        long k;
        counter_t eight;

        cb_zero (& eight);
        (void)cb_add (& eight, 8);

        for (k = 0; (long)sizeof (zbuf) > k; k++)
          zbuf [k] = 0;

        while (0 <= cb_cmp (& rem_bits, & eight)) {
          long chunk = 0;

          while ((long)sizeof (zbuf) > chunk &&
                 0 <= cb_cmp (& rem_bits, & eight)) {
            chunk++;
            (void)cb_sub (& rem_bits, 8);
          }

          if (0 < chunk) {
            long remaining = chunk;

            while (remaining > 0) {
              unsigned int c_chunk = (remaining > (long)batch_limit) ?
                batch_limit : (unsigned int)remaining;

              if (0 == cb_add (processed_bits, c_chunk * 8)) {
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
            crc = crc_update_buffer (crc, tbl, mask32, zbuf, chunk);
          }
        }

        if (0 == cb_is_zero (& rem_bits)) {
          * actually_padded = 1;

          if (0 == g_pad_auto) {
            (void)fprintf (stderr, "WARNING: --limit not a multiple of 8; ");
            (void)fprintf (stderr, "trailing ");
            cb_printf (stderr, & rem_bits);
            (void)fprintf (stderr, " bit%s ignored in 8-bit mode.\n",
              (1 == rem_bits.d [0] && 0 == rem_bits.d [1]) ? "" : "s");
            (void)fprintf (stderr, "         ");
            (void)fprintf (stderr,
              "Result calculated only up to the last full character.\n");
          }
        }
      } else {
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

        (void)fprintf (stderr, "WARNING: File ended after ");
        cb_printf (stderr, & used_bits);
        (void)fprintf (stderr, " bit%s, but --limit=",
           (1 == used_bits.d [0] && 0 == used_bits.d [1]) ? "" : "s");
        cb_printf (stderr, lim_bits);
        (void)fprintf (stderr, " requested.\n");
        (void)fprintf (stderr, "         ");
        (void)fprintf (stderr, "Use --pad to zero-fill the remaining bit%s.\n",
          (1 == rem_bits.d [0] && 0 == rem_bits.d [1]) ? "" : "s");
      }
    }

    return crc;
  }

  return compute_crc_fb (fp,
    filename, tbl, use_cb, mask32, inmask, pad, lim_bits, processed_bits,
    processed_chars, actually_padded, batch_limit);
}

/******************************************************************************/

static int
#ifdef ANSI_COMPILER
find_max_bits (
  const char * const filename,
  int * const is_all_zeros)
#else
find_max_bits (filename, is_all_zeros)
  const char * const filename;
  int * const is_all_zeros;
#endif
{
  FILE * fp;
  crc_t aggregate = 0;
#ifndef USE_FREAD
  int ch;
#endif
  int bits = 0;

  * is_all_zeros = 1;

  fp = fopen (filename, "rb");

  /* cppcheck-suppress knownConditionTrueFalse */
  if (NULL == fp || (FILE *)0 == fp) /* //-V560 */
    return 0; /* Errors handled via process_file */

#ifdef USE_FREAD
  {
    unsigned char mbuf [BUFSIZ];

    for (;;) {
      if (0 != feof (fp) || 0 != ferror (fp)) {
        break;
      }

      {
        const long n = (long)fread (mbuf, 1, sizeof (mbuf), fp);

        if (0 >= n) {
          break;
        }

        {
          long i;

          for (i = 0; n > i; i++) {
            const crc_t b = (crc_t)mbuf [i];

            aggregate |= b;
          }
        }
      }
    }
  }
#else
  while (EOF != (ch = fgetc (fp)))
    aggregate |= (crc_t)(unsigned char)ch;
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

  for (bits = 32; bits > 0; bits--) {
    const crc_t bit_mask = (crc_t)1 << (bits - 1);

    if (0 != (aggregate & bit_mask))
      return bits;
  }

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
  int local_use_cb = use_cb;
  int auto_v = 0;
  int actually_padded = 0;
  crc_t local_inmask = inmask;
  counter_t processed_bits;
  counter_t processed_chars;

  cb_zero (& processed_bits);
  cb_zero (& processed_chars);
  g_fileerr = 0;

  {
    const int max_bits = find_max_bits (filename, & is_all_zeros);

    if (0 > max_bits)
      return;

    if (0 != g_bits_auto) {
      if (0 != is_all_zeros)
        local_use_cb = cb;
      else
        local_use_cb = max_bits;

      if (local_use_cb != cb || 0 != is_all_zeros)
        auto_v = 1;

      local_inmask = ((crc_t)1 << local_use_cb) - (crc_t)1;
    }
  }

  fp = fopen (filename, "rb");

  /* cppcheck-suppress knownConditionTrueFalse */
  if (NULL == fp || (FILE *)0 == fp) { /* //-V560 */
    error_msg ("Error opening", filename, errno);
    return;
  }

  {
    const crc_t crcval = compute_crc (fp,
      filename, tbl, cb, ub, local_use_cb, mask32, local_inmask, pad, lim_bits,
      & processed_bits, & processed_chars, & actually_padded, batch_limit);

    if (0 != fclose (fp))
      error_msg ("Error closing", filename, errno);

    if (0 != g_fileerr)
      return;

    v = (crc_t)crcval & mask32;
  }

  for (i = 0; 8 > i; i++) {
    const int ch = hexdigits [(int)(v & 0xF)];

    buf [(long)(7 - i)] = (char)ch;
    v >>= 4;
  }

  buf [8] = '\0';

  (void)fprintf (stdout, "%s\t\tCRC=%s", filename, buf);

  if (0 != g_verbose || 0 != auto_v ||
     (0 != actually_padded && 0 != g_pad_auto)) {
    (void)fprintf (stdout, "\t# ");
    cb_printf (stdout, & processed_bits);
    (void)fprintf (stdout, " bits (");
    cb_printf (stdout, & processed_chars);
    (void)fprintf (stdout, " %d-bit character%s", local_use_cb,
      (1 == processed_chars.d [0] && 0 == processed_chars.d [1]) ? "" : "s");

    if (0 != is_all_zeros)
      (void)fprintf (stdout, " - empty");

    if (0 != actually_padded && (0 != pad || 0 != g_pad_auto))
      (void)fprintf (stdout, " - padded");

    (void)fprintf (stdout, ")");
  }

  (void)fprintf (stdout, "\n");
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
  (void)fprintf (stderr, "Usage: %s [option(s)...] <file> [file(s)...]\n",
    progname);
  (void)fprintf (stderr,
    "Options:\n");
  (void)fprintf (stderr,
    "  --limit=N        Stops processing after N bits\n");
  (void)fprintf (stderr,
    "  --bits=N         Reads as N bits per storage character\n");
  (void)fprintf (stderr,
    "  --bits=auto      Automatically determines significant bits\n");
  (void)fprintf (stderr,
    "  --pad            Pads trailing bits with zeros\n");
  (void)fprintf (stderr,
    "  --pad=auto       Automatically pads bits when necessary\n");
  (void)fprintf (stderr,
    "  --auto           Enables '--bits=auto --pad=auto --verbose'\n");
  (void)fprintf (stderr,
    "  --verbose, -v    Verbose mode (shows processing details)\n");
  (void)fprintf (stderr,
    "  --help, -h       Shows this help and usage text\n");

  if (8 != cb)
    (void)fprintf (stderr,
      "\nNOTE: This system has a character size of %d-bits.\n", cb);

  if (8 < cb)
    (void)fprintf (stderr,
      "Use '--bits=8' to process 8-bit input data on this system.\n");
}

/******************************************************************************/

int
#ifdef ANSI_COMPILER
main (
  const int argc,
  char * const argv [])
#else
main (argc, argv)
  const int argc;
  char * const argv [];
#endif
{
  static crc_t crc_table [256];
  crc_t inmask;
  int use_cb, j;
  int found = 0;
  int stop = 0;
  int pad = 0;
  int process_bits = 0;
#ifdef __Z88DK
# ifdef __CPM__
  int processed = 0;
# endif
#endif
  counter_t lim_bits;
  const char * filename = (char *)0;
  crc_t mask32 = 0;
  crc_t v;
  const int cb = charbits ();
  const int ub = crc_t_bits ();
  const int uib = unsigned_int_bits ();
  const unsigned int batch_limit = safe_batch_limit ();
  const char * const progname =
    ((char *)0 != argv [0] && '\0' != * argv [0]) ? argv [0] : "crc";

  cb_zero (& lim_bits);

#ifndef __Z88DK
# ifndef __COMPILER_KCC__
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);
# endif
#endif

  if (16 > uib) {
    (void)fprintf (stderr,
      "FATAL: Non-conforming %d-bit unsigned int (must be >= 16).\n", uib);
    return EXIT_FAILURE;
  }

  for (j = 0; 32 > j; j++)
    mask32 = (mask32 << 1) | 1;

  v = mask32;

  /* cppcheck-suppress knownConditionTrueFalse */
  if (v == (v >> 1)) { /* //-V547 */
    (void)fprintf (stderr,
      "FATAL: Broken compiler: logical right-shift is not logical.\n");
    return EXIT_FAILURE;
  }

  init_crc_table (crc_table);

#ifdef SELFTEST
  test_crc_table (crc_table, mask32);
#endif

  if ((8 > cb) || (32 < cb)) {
    (void)fprintf (stderr,
      "FATAL: Unsupported %d-bit character size (must be 8-32).\n", cb);
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

    if (0 == stop && (0 == xstrcasecmp (argv [j], "--help") ||
                      0 == xstrcasecmp (argv [j], "-h"))) {
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
            if (bits > (unsigned long)ub)
              break;

            bits = bits * 10 + (unsigned long)bits_cb.d [k];
          }
        }

        if (0 == bits || bits > (unsigned long)ub)
          goto bits_error;

        process_bits = (int)bits;
        g_bits_auto = 0;
        continue;

bits_error:
        (void)fprintf (stderr, "FATAL: --bits must be a positive integer ");
        (void)fprintf (stderr, "between 1 and %d (or 'auto').\n", ub);
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
      continue;
    }

    if (0 == stop && (0 == xstrcasecmp (argv [j], "-v") ||
                      0 == xstrcasecmp (argv [j], "--verbose"))) {
      g_verbose = 1;
      continue;
    }

    if (0 == stop && 0 == xstrncasecmp (argv [j], "--limit=", 8)) {
      if (0 == cb_parse (& lim_bits, argv [j] + 8)) {
        (void)fprintf (stderr, "FATAL: --limit must be a positive integer.\n");
        return EXIT_FAILURE;
      }

      if (0 != cb_is_zero (& lim_bits)) {
        (void)fprintf (stderr, "FATAL: --limit must be greater than zero.\n");
        return EXIT_FAILURE;
      }

      continue;
    }

    if (0 == stop && '-' == argv [j] [0]) {
      (void)fprintf (stderr, "FATAL: Unknown option: %s.\n", argv [j]);
      usage (progname, cb);
      return EXIT_FAILURE;
    }

    found++;
  }

  if (0 == found) {
    (void)fprintf (stderr, "FATAL: No filename provided.\n");
    usage (progname, cb);
    return EXIT_FAILURE;
  }

  use_cb = (process_bits > 0) ? process_bits : cb;

  if (use_cb > cb) {
    (void)fprintf (stderr, "WARNING: ");
    (void)fprintf (stderr,
      "Specified --bits=%d is greater than host %d-bit character size.\n",
      use_cb, cb);
    (void)fprintf (stderr, "         ");
    (void)fprintf (stderr,
      "Each %d-bit character read will be zero-filled to reach %d bits.\n",
      cb, use_cb);
  }

  /* cppcheck-suppress knownConditionTrueFalse */
  if ((1 > use_cb) || ((ub - 8) < use_cb)) { /* //-V560 */
    (void)fprintf (stderr,
      "FATAL: Unsupported %d-bit processing with %d-bit crc_t type.\n",
      use_cb, ub);
    return EXIT_FAILURE;
  }

  inmask = ((crc_t)1 << use_cb) - (crc_t)1;

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
          if (wcmatch (filename, dir_get_entry_name ()))
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
        (void)fprintf (stderr, "WARNING: No wildcard match for %s\n", filename);
    } else
# endif
#endif
    {
      process_file (filename,
        crc_table, cb, ub, use_cb, mask32, inmask, pad, & lim_bits,
        batch_limit);

#ifdef __Z88DK
# ifdef __CPM__
      if (0 == g_fileerr)
        processed++;
# endif
#endif
    }
  }

#ifdef __Z88DK
# ifdef __CPM__
  if (0 == processed) {
    (void)fprintf (stderr, "ERROR: No files processed.\n");
    g_anyerr = 1;
  }
# endif
#endif

  return (0 != g_anyerr) ? EXIT_FAILURE : EXIT_SUCCESS;
}

/******************************************************************************/
/* vim: set ft=c ts=2 sw=2 tw=0 ai expandtab cc=80 : */
/******************************************************************************/
