# pcdos-crc

<!-- Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com -->
<!-- SPDX-License-Identifier: MIT-0 -->
<!-- scspell-id: dea16a3a-40d9-11f1-8a31-80ee73e9b8e7 -->

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
* a minimal `stdio` implementation (providing support for `fprintf`,
  `fopen`, `fclose`, `fgetc`, `ferror`, `feof`, and `clearerr`).

It has been tested on various exotic and retro platforms including
**Multics**
([Multics C](https://www.bitsavers.org/pdf/honeywell/large_systems/multics/HH07-01_C_UsersGuide_Nov87.pdf)),
**CP/M-80** ([z88dk](https://z88dk.org/)),
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
  --bits=N    Process N bits per character
  --pad       Pad trailing bits with zeros
  --limit=N   Stop processing after N bits
  --help, -h  Show the help and usage text
```

### Interaction of `--limit` and `--pad`

The behavior of these options depends on the CRC processing mode:

| Mode | `limit` > file size (without `pad`) | `limit` > file size (with `pad`) |
| :--- | :--- | :--- |
| **8-bit mode** (default) | Warns and stops at end of file. | **Zero-fills up to the limit.** |
| **Fallback mode** (`--bits` ≠ 8) | Warns and stops at end of file. | Warns and stops at end of file. |

#### 8-bit mode (default)

When processing 8-bit characters on a system with a native 8-bit characters
size, the `--pad` option allows the program to synthesize zero-filled data
to reach the specified `--limit`.  If the `--limit` is **not** a multiple
of 8, any trailing bits are **discarded** and the resulting CRC is calculated
using input only up to the last full character.

#### Fallback mode

When using a non-8-bit character size via `--bits` **or when running on a
system with a non-8-bit character size**, the program operates in a bit-by-bit
"fallback" mode.  In this mode:
* The `--pad` option is used **only** to zero-fill any remaining bits of the
  *final character* if the file input ends mid-character.
* The program **does not** synthesize data to reach a specified `--limit`.
  If the file ends before the limit is reached, a warning is displayed.

## Building

The `crc.c` source code should build easily anywhere.  If you are using a
non-ANSI C compiler, you should define `NOANSI` (*i.e.*, `-DNOANSI`) or
modify the code to comment out the `#define ANSI_COMPILER` directive.

If you are trying to build in a marginal environment using a C preprocessor
that does not deal with indentation, you can "flatten" the source using POSIX
`sed`:
```sh
sed 's|^[[:space:]]*#[[:space:]]*|#|' crc.c > flat.c
```

### Building for Multics

To build a binary for [Multics](https://multics-wiki.swenson.org/) using
[Multics C](https://www.bitsavers.org/pdf/honeywell/large_systems/multics/HH07-01_C_UsersGuide_Nov87.pdf):
```
>sl3p>cc>e>cc -lg -of crc crc.c
```

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
stream of bits, handing off octets (of 8-bits each) to the CRC routines, which
includes the unused 9th bit in each character.  This means the CRC calculated
will not match that of the DOS or UNIX system unless the `--bits=8` option is
used to specify that only 8 bits per character should be considered.

As an additional hint, if the processing of a file ends with "dangling" bits
(not a full character) then a warning message is displayed.

The Multics filesystem includes the concept of multi-segment files (MSFs), which
are indicated by a directory tagged with a non-zero bit count equal to the
number of component segments of the MSF.  This CRC program does not provide
special treatment for MSFs; each component segment should be processed
independently.

### Building for CP/M-80

To build a binary for CP/M-80, use a recent version
of [z88dk](https://z88dk.org/):
```
zcc +cpm -O3 -vn crc.c -o CRC.COM -DBUFSIZ=128 -DNOANSI
```

If you need to verify files on CP/M that were created on other systems, you
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
utilized.

### Building for ELKS

To build a binary for [ELKS](https://github.com/ghaerr/elks) using IA16-GCC:
```
ia16-elf-gcc -march=i8086 -std=c89 -Os -mregparmcall -melks -o crc crc.c
```

### Building for MS-DOS

To build a binary for MS-DOS using IA16-GCC:
```
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
