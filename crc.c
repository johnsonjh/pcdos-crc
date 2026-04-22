#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/

#if defined (__DOS__) || defined (__MSDOS__) || defined (_DOS)
# if !defined (NO_LOCALE)
#  define NO_LOCALE
# endif
# if !defined (DJGPP)
#  if !defined (DOSLIKE)
#   define DOSLIKE
#  endif
# endif
#endif

/*****************************************************************************/

#if defined (DJGPP)
# include <dpmi.h>
# include <go32.h>
#elif defined (__WATCOMC__)
# include <i86.h>
#endif

/*****************************************************************************/

#if !defined (DOSLIKE) && !defined (DJGPP)
# include <sys/ioctl.h>
# include <unistd.h>
# if !defined (POSIXY)
#  define POSIXY
# endif
#endif

/*****************************************************************************/

#if !defined (DOSLIKE) && !defined (NO_LOCALE)
# include <locale.h>
# if defined (__APPLE__)
#  include <xlocale.h>
# endif
#endif

/*****************************************************************************/

#if !defined (NO_LOCALE)
# define XSTR_EMAXLEN 32767

static const char *
xstrerror_l
(
  int errnum
)
{
  int saved = errno;
  const char * ret = NULL;
  static /* __thread */ char buf [XSTR_EMAXLEN];

# if defined (__APPLE__) || defined (_AIX) || \
     defined (__MINGW32__) || defined (__MINGW64__)
#  if defined (__MINGW32__) || defined (__MINGW64__)
  if (0 == strerror_s (buf, sizeof (buf), errnum)) /*LINTOK: xstrerror_l*/
    ret = buf;
#  else
  if (0 == strerror_r (errnum, buf, sizeof (buf))) /*LINTOK: xstrerror_l*/
    ret = buf;
#  endif
# else
#  if defined (__NetBSD__)
  locale_t loc = LC_GLOBAL_LOCALE;
#  else
  locale_t loc = uselocale ((locale_t)0);
#  endif
  locale_t copy = loc;

  if (LC_GLOBAL_LOCALE == copy)
    copy = duplocale (copy);

  if ((locale_t)0 != copy)
    {
      ret = strerror_l (errnum, copy); /*LINTOK: xstrerror_l*/

      if (LC_GLOBAL_LOCALE == loc)
        {
          freelocale (copy);
          copy = (locale_t)0;
        }
    }
# endif

  if (! ret)
    {
      int n_buf = snprintf (buf, sizeof (buf), "Unknown error %d", errnum);

      if (0 > n_buf || (size_t)n_buf >= sizeof (buf))
        { /* cppcheck-suppress syntaxError */
          (void)fprintf (stderr,
		         "FATAL: snprintf buffer overflow at %s[%s:%d]\n",
                         __func__, __FILE__, __LINE__);
          exit (EXIT_FAILURE);
        }

      ret = buf;
    }

  errno = saved;

  return ret;
}
#else
# define xstrerror_l strerror
#endif

/*****************************************************************************/

static unsigned long
compute_crc
(
  FILE *fp
)
{
  unsigned long crc = 0UL;
  unsigned long poly = 0x51F9D3DEUL;
  int ch;
  int j;

  while (EOF != (ch = fgetc (fp)))
  {
    crc ^= ((unsigned long)ch << 24);

    for (j = 0; j < 8; ++j)
      {
        if (crc & 0x80000000UL)
          crc = ((crc << 1) & 0xFFFFFFFFUL) ^ poly;
        else
          crc = (crc << 1) & 0xFFFFFFFFUL;
      }
  }

  return crc;
}

/*****************************************************************************/

int
main
(
  int argc,
  char * argv []
)
{
  FILE * fp;
  unsigned long crcval;

#if !defined (NO_LOCALE)
  (void)setlocale (LC_ALL, "");
#endif

  if (2 != argc)
    {
      (void)fprintf (stderr, "Usage: %s <file>\n", argv [0]);

      return EXIT_FAILURE;
    }

  fp = fopen (argv [1], "rb");

  if (NULL == fp)
    {
      (void)fprintf (stderr, "FATAL: Error %d reading %s: %s\n",
	             errno, argv [1], xstrerror_l (errno));

      return EXIT_FAILURE;
    }

  crcval = compute_crc (fp);

  (void)fclose (fp);

  if (fprintf (stdout, "%s\t\tCRC=%08lX\r\n", argv [1], crcval))
    return EXIT_SUCCESS;
  else
    {
      if (fprintf (stderr, "FATAL: Error writing stdout!\n"))
        return EXIT_FAILURE;
      else
	abort();
    }

  /*NOTREACHED*/ /* unreachable */
  abort();

  /*LINTED: E_STMT_NOT_REACHED*/
  return EXIT_FAILURE;
}

/*****************************************************************************/
