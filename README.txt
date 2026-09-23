# CRC

- Overview
- Usage
  - Automatic bit-width detection
  - Automatic padding
  - Verbose output
  - Cross-platform consistency and bitstreams
    - Bit-for-bit transfers
    - Interaction of --limit and --pad
    - 8-bit mode (default)
    - Fallback mode
- Binary builds
  - Extra builds
- Building from source
  - Porting tips
  - Developer notes
  - GitLab CI/CD
- Platform specifics
  - Building for Multics
    - Multics notes
  - Building for TOPS-20
    - TOPS-20 notes
  - Building for CP/M-80
    - Building with z88dk
    - Building with Ack
    - Building with HI-TECH C Z80
    - CP/M-80 notes
      - CP/M-80 memory usage
      - CP/M-80 performance
      - CP/M-80 LRBC (Last Record Byte Count)
  - Building for CP/M-86
    - CP/M-86 notes
  - Building for CP/M-68K
    - CP/M-68K notes
  - Building for ELKS
  - Building for AmigaOS
    - AmigaOS notes
  - Building for Atari ST
    - Atari ST notes
  - Building for MS-DOS
    - MS-DOS notes
  - Building for 2.11BSD
    - 2.11BSD notes
- Security
- SAST and linters
- License

# Overview

This program computes the same 32-bit CRC values as those produced by
the IBM PC-DOS CRC.EXE utility.

It was carefully constructed to be portable and correct on every
platform with a C compiler, such as:

- ancient pre-ANSI / "C86" / K&R C compilers,
- environments providing deficient stdio implementations,
- environments with broken (or completely missing) division or modulo
  math operations,
- systems where NULL is not equal to zero (e.g. Honeywell
  600/6000-series),
- systems using one's-complement integer representation (e.g., Unisys
  ClearPath Dorado / OS 2200),
- systems with character sizes other than 8 bits (e.g., DEC PDP-10,
  H6000, Unisys), and
- systems using non-ASCII character sets (e.g., IBM mainframes, Unisys
  MCP), or even
- systems using non-EBCDIC character sets.

The only current requirements are:

- the C compiler must provide some storage type with a width at least as
  wide as the 32-bit CRC,
- a character type with a width of at least 8 but no more than 32 bits,
  and
- a minimal stdio implementation only needing to support 7 functions:
  - (1) fprintf or printf, (2) fopen, (3) fclose, (4) fgetc, (5) ferror, (6) feof,
    (7) clearerr,
  - and optionally: setbuf and fread.

It has been tested on various exotic and retro platforms including
Multics (Multics C), TOPS-20 (KCC), CP/M-80 (z88dk, Ack, HI-TECH C),
CP/M-86 (Aztec C86), CP/M-68K (Aztec C68K), MS-DOS (IA16-GCC, dev86,
Watcom C, Turbo C, Borland C++, Aztec C86, Microsoft C,
Digital Mars C/C++, Pacific C, CI C86PLUS, DJGPP), Windows (MSVC,
OrangeC, Digital Mars C/C++, GCC, Clang, Pelles C, lcc-win), ELKS
(IA16-GCC), Atari ST (Vbcc, CrossMINT), AmigaOS (Vbcc, Aztec C68K,
Amiga-GCC), UNIX (any POSIX-like system, 2.11BSD with the native
toolchain, UNIX V7 cross-compiled with Ack), and systems supported by
SoftIntegration Ch, but should be able to be built anywhere else with
little to no porting effort required.

- NOTE: The Small-C and Micro-C compilers/C-language subsets are not
  sufficient to build this software (for various reasons). You need a
  real C compiler and preprocessor, which is something that at least
  resembles 1978 K&R C and provides 1979 UNIX V7-style stdio. The
  unrelated Smaller C compiler is sufficient (but only for 32-bit
  targets).

It is hoped that this 'AI-slop-free' program will serve both as a
practical utility and as a template for writing ultra-portable C code.

# Usage

    Usage: CRC [option(s)...] <file> [file(s)...]
    Options:
      --limit=N        Stops processing after N bits
      --lrbc           Limits to the CP/M Last Record Byte Count
      --lrbc=isx       Use the ISX LRBC convention (unused bytes)
      --bits=N         Reads as N bits per storage character
      --bits=auto      Automatically determines significant bits
      --pad            Pads trailing bits with zeros
      --pad=auto       Automatically pads bits when necessary
      --auto           Enables '--bits=auto --pad=auto --verbose'
      --verbose, -v    Verbose mode (shows processing details)
      --help, -h       Shows this help and usage text

- The --lrbc options are available only on CP/M builds (CP/M-80,
  CP/M-86, CP/M-68K); see the CP/M-80 notes. On CP/M 3.0 or later,
  --auto also enables --lrbc.

- If multiple --bits, --pad, or --limit options are provided, only the
  last value is effective.

- If the specified --bits value is larger than the native character size
  a warning is displayed at startup to indicate that each character read
  from the file will be zero-filled to the requested size.

# Automatic bit-width detection

When using --bits=auto, the program performs two passes on each file:

1.  The first pass will scan the file to determine the number of
    significant bits actually used in the storage characters.

2.  The second pass actually calculates the CRC (using the detected
    bit-width).

If a file contains only 7-bit ASCII data, --bits=auto will automatically
process it using 7 bits per character, ensuring a consistent CRC
regardless of whether the file is stored on system with a wider native
character size, like a mainframe running Multics (which stores text
7-bit ASCII characters in 9-bit nonets).

If the detected bit-width differs from the host's native character size,
the verbose output option is automatically enabled for that file.

If a file is empty, the program will default to using the native
character width of the host system and add an empty note to the verbose
output.

This option is especially useful when combined with the --pad=auto
option, (and both options are enabled when the program is invoked with
--auto).

# Automatic padding

When using --pad=auto, the program automatically applies zero-padding to
the bitstream in the following scenarios:

1.  Dangling bits: If the file ends mid-character (e.g., a file with 11
    bits when reading 8 bits per character), the final partial character
    is zero-padded.

2.  Truncated limits: If an input --limit is reached mid-character, the
    program pads the remaining bits of that character.

3.  Synthesis: If the specified --limit exceeds the file size, the
    program zero-fills the stream to the limit.

If automatic padding is applied the program automatically enables the
verbose output option for that file and adds a padded note to the
details.

# Verbose output

When using the --verbose (or -v) option, the program appends detailed
processing information to the output (after a # character):

    DATA.DAT        CRC=0D03ABFA    # 174344 bits (21793 8-bit characters)

# Cross-platform consistency and bitstreams

This program calculates a CRC based on a continuous bitstream. To ensure
the same CRC value is obtained for the same data across different
platforms, you must understand how the bitstream is constructed from the
host's storage characters.

The --bits=N option specifies how many bits to extract from each
"storage character" (which is the native size of the "byte" or character
type of the C compiler on that system).

# Bit-for-bit transfers

If a file is transferred bit-for-bit between systems with different
native character sizes, the CRC will match if you use the native
character size of the current host.

For example, consider a file that contains 72-bits of data stored
sequentially:

- On an 8-bit system (like modern machines), the file has 9 characters.

- On a 9-bit system (like a Honeywell mainframe running Multics), the
  same 72 bits occupy 8 characters.

Using --bits=9 on a system that uses 8-bit characters to process such a
file stored locally results in pulling 9 bits from each native 8-bit
character (thus injecting a zeroed 9th bit into every 8-bit byte read)
resulting in an 81-bit stream (and a different CRC).

# Interaction of --limit and --pad

The behavior of these options depends on the CRC processing mode:

# 8-bit mode (default)

When processing 8-bit characters on a system with an 8-bit native
character size, the --pad option affects behavior in two ways:

1.  Synthesizing data: If the specified --limit exceeds the file size,
    the program will synthesize zero-filled full 8-bit characters to
    reach the limit. If the --limit is not a multiple of 8, any trailing
    bits that cannot form a complete octet are discarded.

2.  Partial character processing: If the --limit is reached within the
    file and falls mid-octet, the --pad option allows the program to
    process that final partial octet by zero-padding the remaining bits.
    Without --pad, the program warns and truncates to the last full
    octet.

# Fallback mode

When using a non-8-bit character size via --bits or when running on a
system with a non-8-bit native character size, the program operates in a
bit-by-bit "fallback" mode.

In this mode:

- The --limit is effectively rounded down to the nearest multiple of the
  requested character processing size (--bits) unless the --pad option
  is used.

- The --pad option is used to zero-fill any remaining bits of the final
  character read from the file if the input ends mid-character, or to
  allow processing final partial characters to satisfy a --limit.

- In this mode, the program will synthesize zero data to reach a
  specified --limit if the --pad option is specified. Without --pad, if
  the file ends before the limit is reached, a warning is displayed.

# Binary builds

  These are not necessarily the best builds overall, but they are the
  "best" builds (for each platform) currently produced by GitLab CI/CD.

              Platform Toolchain
  -------------------- ---------------------
           AmigaOS/68K Vbcc
       AmigaOS/PowerUp Vbcc
        AmigaOS/WarpOS Vbcc
             AmigaOS 4 Vbcc
              Atari ST Vbcc
              CP/M-68K Aztec C68K/ROM 3.6b
          CP/M-80 8080 Ack
           CP/M-80 Z80 HI-TECH C Z80 4.11
     Linux/m68k 32-bit Ack
     Linux/MIPS 32-bit Ack
      Linux/PPC 32-bit Ack
      Linux/x86 32-bit Open Watcom V2
      Linux/x86 64-bit GCC (musl)
         MS-DOS 16-bit Microsoft C 8.00c
        UNIX V7/PDP-11 Ack
    Windows/x86 32-bit GCC (MinGW)
    Windows/x86 64-bit GCC (MinGW)

# Extra builds

  The following are additional GitLab CI/CD builds.

              Platform Toolchain
  -------------------- -----------------------------------
           AmigaOS/68K Aztec C68K/Amiga 5.2a
      Linux/x86 32-bit Ack
      Linux/x86 32-bit Vbcc
         MS-DOS 16-bit Ack
         MS-DOS 16-bit Aztec C86 5.2a
         MS-DOS 16-bit Computer Innovations C86PLUS 1.10
         MS-DOS 16-bit dev86
         MS-DOS 16-bit Digital Mars C/C++ 8.57
         MS-DOS 16-bit Microsoft C 5.10
         MS-DOS 16-bit Microsoft C 6.00A
         MS-DOS 16-bit Open Watcom V2
         MS-DOS 16-bit HI-TECH Pacific C 7.51
         MS-DOS 16-bit Turbo C 1.0
         MS-DOS 16-bit Turbo C 1.5
         MS-DOS 16-bit Turbo C 2.01
         MS-DOS 16-bit Turbo C++ 1.01
         MS-DOS 16-bit Turbo C++ 3.00
         MS-DOS 16-bit Borland C++ 3.1
         MS-DOS 16-bit Turbo C++ 4.02J
         MS-DOS 32-bit Ack
    Windows/x86 32-bit Open Watcom V2

# Building from source

The crc.c source code should build easily anywhere with no changes
needed:

- Build using make (the CC, CFLAGS, and LDFLAGS variables are
  respected):

      make

- Build using cc (or c89, gcc, clang, etc.):

      cc -O3 -o crc crc.c

- To build a native binary on Windows using the Microsoft Visual Studio
  C/C++ compiler, from a Developer Command Prompt for Visual Studio
  window, run:

      msvcbuild.bat

# Porting tips

- If you are using a non-ANSI C compiler, you may need to define NOANSI
  (i.e., -DNOANSI) or modify the source code to comment out the
  #define ANSI_COMPILER directive. If you don't have stdlib.h you should
  also define NOSTDLIB. If your non-ANSI compiler does support the const
  keyword, then you should additionally define USE_CONST.

- If your environment has a missing or broken fread function, you may
  need to define NOFREAD or modify the source code to comment out the
  #define USE_FREAD directive. This will cause the program to use much
  slower (and in some environments, less reliable)
  character-by-character file reading routines.

- If your environment does not have the errno.h header file, you may
  need to define NOERRNO or modify the source code to comment out the
  #define USE_ERRNO directive.

- For non-ANSI compilers or environments offering an ANSI-conforming
  strerror function, you should define FORCE_STRERROR to use it. If you
  define FORCE_STRERROR but don't need to declare strerror yourself,
  define NO_DCL_STRERROR. If you have the pre-ANSI BSD/System V
  sys_errlist / sys_nerr interface, you should define USE_PSYSERROR.

  - For purported ANSI compilers or environments lacking the C89/ANSI
    defined strerror function or string.h, you should define NOSTRERROR.

- Defining HAVE_SYS_STAT indicates the availability of the POSIX stat
  function and the existence of the sys/types.h and sys/stat.h header
  files, if not automatically detected.

- Defining SELFTEST adds a (rather heavyweight) startup test which
  verifies the CRC lookup table in the source code is uncorrupted and
  consistent with the polynomial (0x51F9D3DE).

- If you are trying to build in an environment providing a C
  preprocessor that does not deal with indentation, you can "flatten"
  the source code using POSIX sed:

      sed 's|^[[:space:]]*#[[:space:]]*|#|' crc.c | \
        { out=$(cat) || exit 1; : > crc.c && printf '%s\n' "$out" > crc.c; }

- If you have a C preprocessor that won't allow you to define away const
  even when the compiler doesn't support it, like some older versions of
  Aztec C, you should remove const from the source code using a global
  search and replace operation (and somewhat unintuitively, you may need
  to define USE_CONST when compiling). The required transformation is
  easily performed using POSIX sed:

      sed 's|const||g' crc.c | \
        { out=$(cat) || exit 1; : > crc.c && printf '%s\n' "$out" > crc.c; }

Most users won't need to do any of these things.

# Developer notes

NB: Submission of AI (artificial intelligence) generated code by
contributors is NOT permitted. There will be zero clankerslop in this
project!

The Makefile provides three convenience targets for developers:

1.  make tags generates source code tags using etags, ctags, gtags, and
    cscope, if those programs are available.

2.  make lint runs the .lint.sh shell script, which checks the source
    code for many issues. Anyone interested in modifying the program
    should install all the optional tools and ensure that all the tests
    pass.

3.  make test runs the .test.sh shell script, which runs a comprehensive
    test suite.

The linting script and test suite are designed to run on any
POSIX-conforming shell environment and are regularly tested on AIX,
FreeBSD, Haiku, Linux, NetBSD, and OpenBSD.

# GitLab CI/CD

- You can examine the output of the latest GitLab CI/CD job which
  includes build output from various legacy compilers.

# Platform specifics

The following sections document platform specific differences, as well
as thoroughly validated and supported compilation recipes for various
compilers and platforms.

It is highly recommended to use these exact compiler flags and tested
compiler versions. In many cases (especially with older compilers),
using different flags or more aggressive optimizations triggers nasty
compiler bugs that result in subtle miscompilations that are very tricky
to detect.

# Building for Multics

- To build a binary for Multics using Multics C:

      >sl3p>cc>e>cc -lg -of crc crc.c

# Multics notes

Multics can be considered to run on an exotic platform, the Honeywell
6000-series of 36-bit "large systems" mainframes. This system uses 9
bits per character, where most systems use 8 bits per character.

When run on Multics or any other environment not using an 8-bit
character size, some instructional text will be appended to the --help
output:

    NOTE: This system has a character size of 9-bits.
    Use '--bits=8' to process 8-bit input data on this system.

As explained above, the --bits option specifies how many bits to extract
from each native storage character. On Multics, the native character
size is 9 bits. If a file was transferred "bit-for-bit" from an 8-bit
system, you should use --bits=9 on Multics (which is the default on
Multics if --bits is not specified) to pull all 9 bits from each storage
nonet. Conversely, if you are processing 8-bit data that was stored "one
byte per nonet" (leaving the 9th bit unused), you must use --bits=8 to
skip that unused bit.

Note that changing the number of bits processed per character will shift
the bit-alignment of subsequent characters and result in a different CRC
value.

As an additional hint, if the processing of a file ends with "dangling"
bits (not a full character) then a warning message is displayed.

The Multics filesystem includes the concept of multi-segment files
(MSFs), which are indicated by a directory tagged with a non-zero bit
count equal to the number of component segments of the MSF. This CRC
program does not provide special treatment for MSFs; each component
segment should be processed independently.

To ensure proper functionality on Multics, the program uses a very
specific read strategy to bypass several known bugs in the Multics C
stdio library. The primary workaround addresses sign-extension bugs when
detecting EOF by batching reads of full 36-bit words (whenever possible)
as this is immune to sign-extension issues. For non-word-aligned data,
we fall back on a "slow but safe" algorithm to process any remaining
characters. Additionally, when using --bits=auto, the bit count as
determined during the initial pass is compared with the actual number of
bits processed during the CRC calculation, ensuring that system library
bugs that result in short reads are detected and can be recovered.

# Building for TOPS-20

1.  To build a binary for TOPS-20 for PDP-10 systems using the KCC
    compiler, you need to transform the source code appropriately. This
    is easy to do on any system with a POSIX-conforming sed
    implementation available:

        sed -e 's|fprintf[^(]*(std[oe][ur][tr],[[:space:]]*|printf (|g' \
            -e 's|cb_printf[^(]*(std[oe][ur][tr],[[:space:]]*|cb_printf (NULL, |g' \
            -e 's|^#define ANSI_COMPILER$||' crc.c | \
          { out=$(cat) || exit 1; : > crc.c && printf '%s\n' "$out" > crc.c; }

2.  You should ensure that the transformed source code file is
    transferred to the PDP-10 system as text (7-bit ASCII with <CR><LF>
    line endings). The appropriate conversion should happen
    automatically if you use Kermit or ASCII-mode FTP for the file
    transfer. If you plan to transfer the file via other means, you
    might need to convert the line endings first. You can do this easily
    with the unix2dos utility or any POSIX-conforming awk
    implementation:

        awk '{ sub(/\r?$/, "\r"); print }' crc.c | \
          { out=$(cat) || exit 1; : > crc.c && printf '%s\n' "$out" > crc.c; }

    NB: You should perform the above step only if necessary, as a
    "double-conversion" will result in a slower compilation, with the
    KCC compiler emitting many thousands of warning messages (one for
    each line of source compiled).

3.  Once you have the source code on the PDP-10 in the appropriate
    format, it can be compiled with the KCC compiler (usually installed
    as CC):

        CC -o CRC CRC.C

# TOPS-20 notes

The PDP-10 mainframe is a big-endian 36-bit word-addressed system.
Although TOPS-20 has various ways of encoding data, CRC uses the
standard C I/O library provided by KCC, which treats characters as 9-bit
nonets. You'll need to specify an appropriate --bits (or use
--bits=auto) and --pad (or use --pad=auto) to get matching calculations
for most foreign data (depending on how its stored) on the system.

# Building for CP/M-80

# Building with z88dk

- To build a binary for CP/M-80 for Z80 systems, using a version of
  z88dk from 2026-07-10 or later (earlier versions have a bug which
  causes the character count to be miscalculated when using -SO3):

      zcc +cpm -compiler=sdcc -SO3 -O3 -vn crc.c -clib=ixiy -o crc.com

- To build a binary for CP/M-80 for 8080 systems, using a version of
  z88dk from 2026-05-01 or later (earlier versions have a bug which
  causes the CRC to be miscalculated on 8080 processors):

      zcc +cpm -SO3 -O3 -vn crc.c -clib=8080 -o crc.com

If you are using a Linux system with Docker you can use the z88dk/z88dk
Docker container to build without needing to locally compile and install
the current z88dk.

- To build for Z80 CP/M-80 using z88dk via Docker:

      docker run --rm -v "$(pwd -P)":/src -w /src z88dk/z88dk:latest \
        zcc +cpm -compiler=sdcc -SO3 -O3 -vn crc.c -clib=ixiy -o crc.com

- To build for 8080 CP/M-80 using z88dk via Docker:

      docker run --rm -v "$(pwd -P)":/src -w /src z88dk/z88dk:latest \
        zcc +cpm -SO3 -O3 -vn crc.c -clib=8080 -o crc.com

# Building with Ack

- To build for 8080 CP/M-80 using Ack:

      ack -mcpm -O3 -DCRC_CPM -D__ACK__ -DCPMDWC bdosack.s crc.c cpmdwc.c -o crc.com

# Building with HI-TECH C Z80

- To build for Z80 CP/M-80 using (MS-DOS hosted) HI-TECH C Z80 4.11:

  First, transform the source appropriately using POSIX sed or a similar
  tool (adjusting the #if HAS_INCLUDE(...) lines to #if 0) to work around
  a bug in the HI-TECH C preprocessor:

      sed 's|if HAS_INCLUDE.*$|if 0|' crc.c | \
        { out=$(cat) || exit 1; : > crc.c && printf '%s\n' "$out" > crc.c; }

  Then cross-compile the source using zc:

      zc -CPM -O crc.c

# CP/M-80 notes

- CP/M-80 builds using supported compilers (z88dk, HI-TECH C Z80, and
  Ack) support internal wildcard expansion (i.e., * and ?). Other
  compilers may need adaptations.

- The LZPACK utility can be used to transparently compress the generated
  CP/M executable, reducing its on-disk size by approximately 50% and
  slightly lowering its memory usage, at the cost of a small increase in
  load time.

# CP/M-80 memory usage

- z88dk Z80 builds require ~32K TPA.
- HI-TECH C Z80 builds require ~38K TPA.
- z88dk 8080 builds require ~35K TPA.
- Ack 8080 builds require ~45K TPA.

# CP/M-80 performance

- Z80 builds: z88dk binaries execute about ~2-8% faster than HI-TECH C
  Z80 binaries.
  - For Z80 builds using the z88dk SDCC compiler, adding the
    --max-allocs-per-node250000 option improves performance by ~6%, but
    increases compilation time (from several seconds to a few minutes).
- 8080 builds: z88dk binaries execute ~20% faster than Ack binaries.
- z88dk Z80 binaries execute about twice as fast as z88dk 8080 binaries.

# CP/M-80 LRBC (Last Record Byte Count)

NB: If you need to verify CRCs on CP/M that were created on other
systems, you should always constrain processing to the actual number of
significant bits. This can be done automatically only on CP/M 3.0 and
later!

On CP/M-80 systems, files do not have exact sizes but are stored on disk
in fixed-size records of 1024 bits (i.e., 128 8-bit octets) each. Files
transferred from other systems that are not a multiple of the CP/M
record size will be padded with undefined data to fill a complete
record, and there is no universal EOF marker that can be used to find
the true end of file.

By default, the CRC.COM program will calculate the CRC for all records
on disk associated with the file. If you transferred the file from
another type of system that does support exact lengths (which is
likely), you can specify the number of bits to process using the --limit
flag.

For example, assume DATA.DAT is file of 174,344 bits (21,793 8-bit
octets) which produces a CRC of 0D03ABFA on an MS-DOS or UNIX system. On
a CP/M-80 system this file will utilize 171 records of storage. Since
21,793 octets is not a multiple of 128, the CRC calculation will not
match unless constrained to process only 174,344 bits (i.e.,
CRC --limit=174344 DATA.DAT). With no limit applied, all 175,104 bits
(171 records x 128 octets x 8 bits) of data on disk would be processed.

CP/M-80 3.0 added a new filesystem metadata field: Last Record Byte
Count (or LRBC). Unfortunately, the LRBC is stored as a number between 0
and 255, with no official documented interpretation. The DRI ISX
software uses this field to indicate the number of unused octets in the
last record, while DRI DOS-PLUS uses the field to indicate the number of
used octets in the last record, with a count of zero indicating 128 (a
full final record) under either interpretation.

When running on CP/M 3.0 or later, the --lrbc option uses this metadata
to constrain processing to the file's exact length automatically, so no
manual --limit is needed. Continuing the example above,
CRC --lrbc DATA.DAT processes exactly 174,344 bits and matches the
MS-DOS or UNIX result. Because the DOS-PLUS interpretation is the more
common one, --lrbc assumes it by default; pass --lrbc=isx instead to
select the DRI ISX (unused octets) interpretation. When the LRBC
indicates a full final record or when running under CP/M 2.x, which has
no LRBC all records are processed as before.

For convenience, --auto also enables --lrbc (DOS-PLUS interpretation) on
CP/M 3.0 and later. When --lrbc is requested explicitly on a host that
predates CP/M 3.0, a warning is printed and all records are processed.
If both --lrbc and --limit are given, the smaller (most restrictive) of
the two limits applies. The LRBC is read using direct BDOS function
calls, so this feature is only enabled when compiling for CP/M targets.

# Building for CP/M-86

To build the program for CP/M-86 we are using the most recent versions
of the Aztec C cross-compiler from tsupplis.

- To build a binary for CP/M-86 using cross-Aztec C 4.2 (recommended):

      aztec42_cc "+FA" -D__AZTEC_C_42T__ crc.c
      aztec42_cc "+FA" cpmdwc.c
      aztec42_sqz crc.o
      aztec42_sqz cpmdwc.o
      aztec42_link -o crc.cmd crc.o cpmdwc.o -lc86
      pcdev_cmdinfo crc.cmd

- To build a binary for CP/M-86 using cross-Aztec C 3.4:

      sed 's|const||g' crc.c | \
        { out=$(cat) || exit 1; : > crc.c && printf '%s\n' "$out" > crc.c; }
      aztec34_cc "+FA" -E90 -DNOANSI -DNOSTDLIB -DUSE_CONST -D__AZTEC_C_34T__ crc.c
      aztec34_cc "+FA" -E90 -DNOANSI cpmdwc.c
      aztec34_sqz crc.o
      aztec34_sqz cpmdwc.o
      aztec34_link -o crc.cmd crc.o cpmdwc.o -lc86
      pcdev_cmdinfo crc.cmd

# CP/M-86 notes

- CP/M-86 builds using supported compilers (Aztec C) support internal
  wildcard expansion (i.e., * and ?). Other compilers may need
  adaptations.

- Builds using Aztec C 4.2 will execute about 10% faster than Aztec C
  3.4 builds.

- All of the CP/M-80 notes, with the exception of the LZPACK executable
  compressor, also apply to CP/M-86.

  - Instead of LZPACK, the UPX executable compressor, version 5.2.0 (or
    later) can be used to pack CP/M-86 binaries.

# Building for CP/M-68K

To build the program for CP/M-68K we are using Aztec C68K/ROM 3.6b
(which includes basic CP/M-68K support by David Lee).

- To build a binary for CP/M-68K using cross-Aztec C68K/ROM 3.6b:

      sed -e 's|const||g' \
          -e 's|fprintf[^(]*(std[oe][ur][tr],[[:space:]]*|printf (|g' \
          -e 's|cb_printf[^(]*(std[oe][ur][tr],[[:space:]]*|cb_printf (NULL, |g' \
        crc.c | { out=$(cat) || exit 1; : > crc.c && printf '%s\n' "$out" > crc.c; }
      as68 -o bdos.r bdos68k.a68
      c68 -o crc.r -DNOANSI -DNOSTDLIB -DUSE_CONST -D__CPM68K__ -D__AZTEC_C_36T__ crc.c
      c68 -o cpmdwc.r -DNOANSI cpmdwc.c
      ln68 +C 8100 -t cpm68k/cpm.r bdos.r crc.r cpmdwc.r -lcpm68k/c68k -T -O CRC.68K

# CP/M-68K notes

  CP/M-68K is currently a work-in-progress!

- CP/M-68K builds using supported compilers (Aztec C) support internal
  wildcard expansion (i.e., * and ?). Other compilers may need
  adaptations.

- All of the CP/M-80 notes, with the exception of executable
  compression, apply to CP/M-68K builds.

# Building for ELKS

- To build a binary for ELKS using IA16-GCC:

      ia16-elf-gcc -march=i8086 -std=c89 -O3 -mregparmcall -melks -o crc crc.c

# Building for AmigaOS

- To build a binary for AmigaOS using Aztec C68K/Amiga 5.2a:

      cc -sf -sn -sp -sr -ss -pa -pl -sa -sb -mc -md -DAOSDWC -o crc.o crc.c
      cc -sf -sn -sp -sr -ss -pa -pl -sa -sb -mc -md -o amigadwc.o amigadwc.c
      ln -t crc.o amigadwc.o -T -O crc -lc

- To build a binary for AmigaOS using Vbcc:

      vc "+aos68k" -cpu=68000 -c89 -speed -O4 -maxoptpasses=40 -short-push -sd -DAOSDWC -o crc crc.c amigadwc.c -lamiga

- To build a binary for AmigaOS using Amiga-GCC:

      m68k-amigaos-gcc -m68000 -mcrt=nix13 -O3 -std=gnu90 -DAOSDWC crc.c amigadwc.c -s -o crc

# AmigaOS notes

- AmigaOS "classic" builds using supported compilers (including Vbcc
  +warpos and +powerup targets) perform UNIX-style internal wildcard
  expansion (i.e., * and ?). Builds using other classic Amiga toolchains
  may need adjustments.

- AmigaOS 4 builds do not support wildcard expansion at this time.

# Building for Atari ST

- To build a binary for Atari ST (TOS) using Vbcc:

      vc "+tos" -cpu=68000 -c89 -speed -O4 -maxoptpasses=40 -short-push -sd -DTOSDWC -o crc.ttp crc.c tosdwc.c

- To build a binary for Atari ST (TOS) using CrossMINT:

      m68k-atari-mintelf-gcc -march=68000 -std=c89 -O3 -mfastcall -DTOSDWC -s -o crc.ttp crc.c tosdwc.c

# Atari ST notes

- Atari ST builds using supported compilers perform internal wildcard
  expansion (i.e., * and ?). Builds using other Atari ST toolchains may
  need adjustments.

- Only the Vbcc and CrossMINT (GCC) toolchains are currently supported.

# Building for MS-DOS

- To build a binary for MS-DOS using IA16-GCC:

      ia16-elf-gcc -march=i8086 -std=c89 -O3 -mregparmcall -mcmodel=small -o crc.exe crc.c ia16dwc.c

- To build a binary for MS-DOS using Microsoft C 5.10 (1988):

      cl /AS /O /Ot /Ol /Oi /Oa /Gs /G0 /Dconst= /Fecrc.exe crc.c "lib\setargv.obj" /link /NOE

- To build a binary for MS-DOS using Microsoft C 6.00A (1990):

      cl /AT /O /Ot /Ol /Og /Oi /Oa /Gr /Gs /G0 /Fecrc.com crc.c "lib\setargv.obj" /link /NOE

- To build a binary for MS-DOS using Microsoft C/C++ 8.00c (1993):

      cl /AT /O /Ot /Ol /Og /Oi /Oa /Oc /Oe /Gr /Gs /Ob2 /Oz /G0 /Fecrc.com crc.c "lib\setargv.obj" /link /NOE

- To build a binary for MS-DOS using Computer Innovations C86PLUS 1.10
  (1987):

      sed 's|const||g' crc.c | \
        { out=$(cat) || exit 1; : > crc.c && printf '%s\n' "$out" > crc.c; }
      cc -c -AS -Za -G0 -Oa -Ox crc.c
      carole crc.obj "lib\cwildsnd.obj" -s2000

- To build a binary for MS-DOS using HI-TECH Pacific C 7.51:

      pacc -R -O crc.c

- To build a binary for MS-DOS using Open Watcom V2:

      owcc -bcom -march=i86 -mcmodel=t -frerun-optimizer -O3 -o crc.com crc.c dosdwc.c

- To build a binary for MS-DOS using Watcom C:

      wcc -bt=dos -ms -oh -onatxl+ -0 -fo=crc.obj -fr crc.c
      wcc -bt=dos -ms -oh -onatxl+ -0 -fo=dosdwc.obj -fr dosdwc.c
      wlink system com file crc.obj file dosdwc.obj name crc.com

- To build a binary for MS-DOS using Digital Mars C/C++ 8.57:

      dmc -0 -o -mt crc.c dosdwc.c

- To build a binary for MS-DOS using dev86 0.16.21+:

      bcc -Md -O -o crc.com crc.c

- To build a binary for MS-DOS using Aztec C86 5.2a:

      cc +FA -D__AZTEC_C_52T__ -D__MSDOS__ crc.c
      cc +FA aztecdwc.c
      sqz crc.o
      sqz aztecdwc.c
      ln -o crc.com crc.o aztecdwc.c -lc

- To build a binary for MS-DOS using Turbo C 1.0 (1987) or Turbo C 1.5
  (1988):

      tcc -G -O -Z -f- -mt -d -DTCDWC crc.c tcdwc.c
      exe2bin crc.exe crc.com

- To build a binary for MS-DOS using Turbo C 2.01 (1989) or Turbo C++
  1.01 (1990):

      tcc -G -O -Z -f- -mt -lt -d crc.c "lib\wildargs.obj"

- To build a binary for MS-DOS using Turbo C++ 3.00 (1991):

      tcc -G -Z -f- -mt -lt -d crc.c "lib\wildargs.obj"

- To build a binary for MS-DOS using Borland C++ 3.1 (1992):

      bcc -O2 -f- -mt -lt -d crc.c "lib\wildargs.obj"

- To build a binary for MS-DOS using Turbo C++ 4.02(J) (1994):

      tcc -G -O -Z -f- -g0 -mt -lt -w-pro -d -3- -2- -1- -d crc.c "lib\16bit\wildargs.obj"

- To build a binary for MS-DOS using Ack:

      ack -mmsdos86 -O3 -D__ACK__ crc.c -o crc.com

- To build a binary for MS-DOS (386+) using Ack:

      ack -mmsdos386 -O3 -D__ACK__ crc.c -o crc.exe

- To build a binary for MS-DOS (386+) using DJGPP:

      ix86-pc-msdosdjgpp-gcc -s -march=i386 -O3 -o crc.exe crc.c

# MS-DOS notes

- MS-DOS builds using supported compilers perform internal wildcard
  expansion (i.e., * and ?). Builds using other MS-DOS toolchains may
  need some adjustments.

- The aPACK or UPX utilities can be used to compress the generated
  MS-DOS executables, reducing their on-disk size by approximately 60%,
  at the cost of a small increase in load time.

- When compiling with Turbo C++ 3.00, ensure that the -O (jump
  optimization) option is not enabled. This avoids a compiler bug that
  results in the CRC code being miscompiled.

- Borland C++ versions later than 3.1 may be unable to build binaries
  that are 8086/8088-compatible due to the use of 186/286 instructions
  in their standard libraries. It should be possible to rebuild the
  runtime libraries from source if 8086/8088 compatibility is required
  when using these compilers.

- Ack versions before 2026-07-16 have a bug that prevents binary file
  I/O from working on MS-DOS targets. The problem was identified and a
  patch fixing the bug was accepted. It is highly recommended to always
  use the very latest version of Ack (on all platforms, not just MS-DOS)
  due to other bugs that have been recently identified and fixed
  upstream.

# Building for 2.11BSD

- To build a binary for 2.11BSD:

      cc -O -o crc crc.c

# 2.11BSD notes

- Tested with 2.11BSD PL482 (on a PDP-11/93).

- Older distributions may require building with the -DNOANSI option.

# Security

- The canonical home of this software is https://gitlab.com/dps8m/crc,
  with a mirror on GitHub.
- This software is intended to be secure ?.
- If you find any security-related problems, please don't hesitate to
  open a GitLab Issue (or send an email to the author).

# SAST and linters

The following static analysis and dynamic verification tools are used as
part of the comprehensive testing process (with many invoked
automatically via the .lint.sh script):

                           Tool Usage
  ----------------------------- --------------------------------------------------------------------------
                     PVS-Studio Static analysis tool for C, C++, C#, and Java code
     Clang Analyzer, Sanitizers Static and dynamic analysis tools for C, C++, and Objective-C code
                         CodeQL Semantic code analysis engine
                       Cppcheck Static analysis tool for C and C++ code
                     Dr. Memory Memory debugging tool for Windows, Linux, macOS, and Android
                           DUMA Detect Unintended Memory Access, a memory debugger
                     Flawfinder Scans C and C++ source code for potential security weaknesses
                       Funcheck A tool for checking function call return protections
            GCC Static Analyzer Coverage-guided symbolic execution static analyzer for C code
                       GNU Cppi C preprocessor directive linting, indenting, and regularization
                     GNU Global Source code indexing and tagging system
                   IBM AIX lint Checks C and C++ language programs for potential problems
                 NetBSD lint(1) A C (C90/C99/C11/C17/C23) program verifier
        Oracle Developer Studio Performance, security, and thread analysis tools for C, C++, and FORTRAN
                     PurifyPlus Run-time analysis tools for application reliability and performance
                          REUSE Verifies compliance with the REUSE software licensing guidelines
                        Semgrep A fast, open-source, static analysis engine for many languages
                     ShellCheck A static analysis tool for Unix shell scripts
                         Smatch Smatch (Source Matcher) is a static analysis tool for C code
             SoftIntegration Ch C/C++ interpreter and interactive platform for scientific computing
                       Valgrind Tools for memory debugging, memory leak detection, and profiling
    Visual Studio Code Analyzer Tools to analyze and improve C/C++ source code quality



# License

- This software is distributed under the terms of the permissive MIT No
  Attribution (MIT-0) License.

- While not legally required, giving me credit if you benefit from this
  code is highly appreciated.
