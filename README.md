# pcdos-crc

<!-- Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com -->
<!-- SPDX-License-Identifier: MIT-0 -->
<!-- scspell-id: dea16a3a-40d9-11f1-8a31-80ee73e9b8e7 -->

<!-- toc -->

- [Overview](#overview)
- [Usage](#usage)
  * [Interaction of `--limit` and `--pad`](#interaction-of---limit-and---pad)
    + [8-bit mode (default)](#8-bit-mode-default)
    + [Fallback mode](#fallback-mode)
- [Building](#building)
- [Platform specifics](#platform-specifics)
  * [Building for Multics](#building-for-multics)
    + [Multics notes](#multics-notes)
  * [Building for TOPS-20](#building-for-tops-20)
    + [TOPS-20 notes](#tops-20-notes)
  * [Building for CP/M-80](#building-for-cpm-80)
    + [CP/M-80 notes](#cpm-80-notes)
  * [Building for ELKS](#building-for-elks)
  * [Building for MS-DOS](#building-for-ms-dos)
- [License](#license)

<!-- tocstop -->

## Overview

This program computes the same 32‑bit CRC values as those produced by the
IBM PC‑DOS `CRC.EXE` utility.

It was carefully constructed to be portable and correct on every platform
with a C compiler, such as:

* ancient pre‑ANSI / "C86" / K&R C compilers,
* environments providing deficient `stdio` implementations,
* environments with broken (or completely missing) division or modulo
  math operations,
* systems where `NULL` is not equal to zero (*e.g.*
  Honeywell 600/6000‑series),
* systems using one's‑complement integer representation (*e.g.*, Unisys
  ClearPath Dorado / OS 2200),
* systems with byte sizes other than 8 bits (*e.g.*, DEC PDP‑10, H6000,
  Unisys), and
* systems using non‑ASCII character sets (*e.g.*, IBM mainframes, Unisys MCP),
  or even
* systems using non-EBCDIC character sets.

The only current requirements are:

* the C compiler must provide some storage type with a width at least as wide
  as the 32‑bit CRC,
* a character type with a width of at least 8 but no more than 32 bits, and
* a minimal `stdio` implementation (only needing to support `fprintf` or
  `printf`, `fopen`, `fclose`, `fgetc`, `ferror`, `feof`, and `clearerr`).

It has been tested on various exotic and retro platforms including
**Multics**
([Multics C](https://www.bitsavers.org/pdf/honeywell/large_systems/multics/HH07-01_C_UsersGuide_Nov87.pdf)),
**TOPS-20** (KCC), **CP/M-80** ([z88dk](https://z88dk.org/)),
**MS-DOS** ([IA16-GCC](https://gitlab.com/tkchia/build-ia16/), Watcom,
Microsoft C, [DJGPP](https://www.delorie.com/djgpp/)), **ELKS** (IA16-GCC),
**Atari ST** (TOS/MINT using [CrossMINT](https://tho-otto.de/crossmint.php)),
and systems supported by
[SoftIntegration **Ch**](https://www.softintegration.com/), but should be
able to be built anywhere else with little to no porting effort required.

## Usage

```
Usage: CRC [option(s)...] <file> [file(s)...]
Options:
  --bits=N         Reads as N-bit packed bitstream
  --pad            Pads trailing bits with zeros
  --limit=N        Stops processing after N bits
  --verbose, -v    Verbose (prints reproduction flags)
  --help, -h       Shows this help and usage text
```

* If multiple `--bits` or `--limit` options are provided, only the last value
  is effective.

### Interaction of `--limit` and `--pad`

The behavior of these options depends on the CRC processing mode:

| Mode | `limit` > file size (no `pad`) | `limit` > file size (with `pad`) |
| :--- | :--- | :--- |
| **8-bit mode** (*default*) | Warns, stops at end of file. | **Zero-fills to the limit**. |
| **Fallback mode** (`bits`≠`8`) | Warns, stops at end of file. | Warns, stops at end of file. |

#### 8-bit mode (default)

When processing 8-bit characters on a system with a native 8-bit character
size, the `--pad` option affects behavior in two ways:

1. **Synthesizing data**: If the specified `--limit` exceeds the file size,
   the program will synthesize zero-filled **full** 8-bit characters to reach
   the limit.  If the `--limit` is not a multiple of 8, any trailing bits
   that cannot form a complete octet are **discarded**.

2. **Partial character processing**: If the `--limit` is reached **within**
   the file and falls mid-octet, the `--pad` option allows the program to
   process that final partial octet by zero-padding the remaining bits.
   Without `--pad`, the program warns and truncates to the last full octet.

#### Fallback mode

When using a non-8-bit character size via `--bits` **or when running on a
system with a non-8-bit native character size**, the program operates in a
bit-by-bit "fallback" mode. In this mode:

* The `--limit` is effectively **rounded down** to the nearest multiple of
  the requested character processing size (`--bits`) unless the `--pad`
  option is used.

* The `--pad` option is used to zero-fill any remaining bits of
  the *final character read from the file* if the input ends mid-character,
  or to allow processing a final partial character to satisfy a `--limit`.

* The program **does not** synthesize data to reach a specified `--limit`.
  If the file ends before the limit is reached, a warning is displayed.

## Building

The `crc.c` source code should build easily anywhere with no changes needed.

* If you are using a non-ANSI C compiler, you may need to define `NOANSI`
  (*i.e.*, `-DNOANSI`) or modify the source code to comment out the
  `#define ANSI_COMPILER` directive.

* If your environment does not have the `errno.h` header file, you may need
  to define `NOERRNO` or modify the source code to comment out the
  `#define USE_ERRNO` directive.

* For non-ANSI compilers or environments offering an ANSI-conforming
  `strerror` function, you should define `FORCE_STRERROR` to use it.  If you
  have the pre-ANSI BSD/System V `sys_errlist` / `sys_nerr` interface, you
  should define `USE_PSYSERROR`.

* Defining `SELFTEST` adds a (rather heavyweight) startup test which verifies
  the CRC lookup table in the source code is uncorrupted and consistent with
  the polynomial (`0x51F9D3DE`).

* If you are trying to build in an environment providing a C preprocessor
  that does not deal with indentation, you can "flatten" the source code
  using POSIX `sed`:
  ```sh
  sed 's|^[[:space:]]*#[[:space:]]*|#|' crc.c > flat.c
  ```

Most users won't need to do any of these things.

## Platform specifics

### Building for Multics

To build a binary for [Multics](https://multics-wiki.swenson.org/) using
[Multics C](https://www.bitsavers.org/pdf/honeywell/large_systems/multics/HH07-01_C_UsersGuide_Nov87.pdf):
```
>sl3p>cc>e>cc -lg -of crc crc.c
```

#### Multics notes

Multics can be considered to run on an exotic platform, the Honeywell
6000-series of 36-bit "large systems" mainframes.  This system uses 9 bits
per character, where most systems use 8 bits per character.  Multics stores
data in big-endian format, that is, "foreign" 8-bit data would be stored in
the first most significant positions (starting at bit 1 and ending with
bit 8) and the 9th or least most significant bit would remain unused in
each character.

When run on Multics or any other environment not using an 8-bit character
size, some instructional text will be appended to the `--help` output:

```
NOTE: This system has a character size of 9-bits.
Use '--bits=8' to process 8-bit input data on this system.
```

For example, assume `DATA.DAT` is a file of 174,344 bits (21,793 8-bit octets)
which produces a CRC of `0D03ABFA` on MS-DOS or UNIX systems.  On a Multics
system, this file will be stored as 196,137 bits (21,793 9-bit nonets).
The CRC on such a system will be calculated based on processing the file as a
stream of bits, handing off octets (of 8‑bits each) to the CRC routines, which
includes the unused 9th bit in each character.  Note that the `--bits` option
works by packing the specified number of bits from each character into a
continuous stream; changing the number of bits processed per character will
therefore shift the bit-alignment of subsequent characters and result in a
different CRC value, even if the discarded bits were zero.
This means the CRC calculated will not match that of the DOS or UNIX system
unless the `--bits=8` option is used to specify that only 8 bits per character
should be considered.

As an additional hint, if the processing of a file ends with "dangling" bits
(not a full character) then a warning message is displayed.

The Multics filesystem includes the concept of multi-segment files (MSFs),
which are indicated by a directory tagged with a non-zero bit count equal to
the number of component segments of the MSF.  This CRC program does not
provide special treatment for MSFs; each component segment should be processed
independently.

### Building for TOPS-20

To build a binary for TOPS-20 for PDP-10 systems using the KCC compiler, you
need to transform the source code appropriately.  This is easy to do on any
system with a POSIX-conforming `sed` implementation available:
```
sed -e 's|fprintf[^(]*(std[oe][ur][tr],[[:space:]]*|printf (|g' \
    -e 's|cb_printf[^(]*(std[oe][ur][tr],[[:space:]]*|cb_printf (NULL, |g' \
    -e 's|^#define ANSI_COMPILER$||' crc.c > crckcc.c
```

You should ensure that the transformed source code file (`crckcc.c`) is
transferred to the PDP-10 system as text (7-bit ASCII with `<CR><LF>` line
endings).  The appropriate conversion should happen automatically if you use
Kermit or ASCII-mode FTP for the file transfer, but if you plan to transfer
the file via other means, you *might* need to convert the line endings first.
You can do this with the [`unix2dos`](https://dos2unix.sourceforge.io/)
utility or any POSIX-conforming `awk` implementation:
```
awk '{ sub(/\r?$/, "\r"); print }' <infile.txt >outfile.txt
```

Once you have the source code on the PDP-10 in the appropriate format, it
can be compiled with the KCC compiler (usually installed as `CC`):
```
CC -o CRC CRCKCC.C
```

#### TOPS-20 notes

The PDP-10 mainframe is a big-endian 36-bit word-addressed system.  Although
it uses various ways of encoding data, CRC uses the standard C I/O library
provided by KCC, which treats characters as 9-bit nonets.  You'll need to
specify an appropriate `--bits` argument to get matching calculations for
most foreign data (depending on how it's stored) on the system.

### Building for CP/M-80

To build a binary for CP/M-80 for **Z80** systems, use any recent version
of [z88dk](https://z88dk.org/):
```
zcc +cpm -O3 -vn crc.c -clib=ixiy -o crc.com
```

To build a binary for CP/M-80 for **8080** systems, use a version
of [z88dk](https://z88dk.org/) from **2026-05-01** or later (earlier versions
have a bug which causes the CRC to be miscalculated on 8080 processors):
```
zcc +cpm -O3 -vn crc.c -clib=8080 -o crc.com
```

If you are using a Linux system with Docker you can use the `z88dk/z88dk`
Docker container to build without needing to locally compile and install
the current `z88dk`:
```
docker run --rm -v "$(pwd -P)":/src -w /src z88dk/z88dk:latest \
  zcc +cpm -O3 -vn crc.c -clib=ixiy -o crc.com
```
```
docker run --rm -v "$(pwd -P)":/src -w /src z88dk/z88dk:latest \
  zcc +cpm -O3 -vn crc.c -clib=8080 -o crc.com
```

* CP/M builds for Z80 will execute about 25% faster than 8080 builds.

* The
  [PopCom!](https://github.com/johnsonjh/VEDIT/raw/refs/heads/master/dev/popcom.com)
  utility can compress the generated CP/M executable, reducing its on‑disk size
  by approximately 50% and slightly lowering its memory usage, at the cost of a
  small increase in load time.

#### CP/M-80 notes

CP/M-80 builds support internal wildcard expansion (*i.e.*, `*` and `?`).

If you need to verify CRCs on CP/M that were created on other systems, you
should always constrain processing to the actual number of significant bits.

On CP/M-80 systems, files do not have exact sizes but are stored on disk in
fixed-size records of 1024 bits (*i.e.,* 128 8-bit octets) each.  Files
transferred from other systems that are not a multiple of the CP/M record size
will be padded with undefined data to fill a complete record, and there is no
universal EOF marker that can be used to find the true end of file.

By default, the `CRC.COM` program will calculate the CRC for all records on
disk associated with the file.  If you transferred the file from another type
of system that does support exact lengths, you can specify the number of bits
to process using the `--limit` flag.

For example, assume `DATA.DAT` is file of 174,344 bits (21,793 8-bit octets)
which produces a CRC of `0D03ABFA` on MS-DOS or UNIX systems.  On a CP/M-80
system this file will utilize 171 records of storage.  Since 21,793 octets is
not a multiple of 128, the CRC calculation will not match unless constrained
to process only 174,344 bits (*i.e.*, `CRC --limit=174344 DATA.DAT`).  With
no limit applied, all 175,104 bits (171 records × 128 octets × 8 bits) on
disk would be processed.

CP/M-80 3.0 added a new filesystem metadata field: Last Record Byte Count (or
LRBC).  Unfortunately, the LRBC is stored as a number between 0 and 255, with
no official documented interpretation.  The DRI ISX software uses this field
to indicate the number of **unused** octets in the last record, while DRI
DOS-PLUS uses the field to indicate the number of **used** octets in the last
record, with a count of zero indicating 128.  Because of this ambiguity and
because accessing the LRBC metadata requires the use of non-portable
programming constructs (direct BDOS function calls) the LRBC is not currently
utilized, but might be supported in a future release.

### Building for ELKS

To build a binary for [ELKS](https://github.com/ghaerr/elks) using IA16-GCC:
```sh
ia16-elf-gcc -march=i8086 -std=c89 -Os -mregparmcall -melks -o crc crc.c
```

### Building for MS-DOS

To build a binary for MS-DOS using IA16-GCC:
```sh
ia16-elf-gcc -march=i8086 -std=c89 -Os -mregparmcall -mcmodel=tiny -o crc.com crc.c
```

## License

This software is distributed under the terms of the permissive
[MIT No Attribution (MIT-0)](LICENSE) license.

<!--
Local Variables:
mode: markdown
End:
-->
<!-- vim: set ft=markdown expandtab cc=80 : -->
<!-- EOF -->
