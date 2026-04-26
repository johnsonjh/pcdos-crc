# pcdos-crc

<!-- Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com -->
<!-- SPDX-License-Identifier: MIT-0 -->
<!-- scspell-id: dea16a3a-40d9-11f1-8a31-80ee73e9b8e7 -->

## Overview

This program computes the same 32‑bit CRC values as those produced by the
IBM PC‑DOS `CRC.EXE` utility.

It was carefully constructed to be portable and correct on every platform
with a C compiler, such as:

* ancient pre‑ANSI / “C86” / K&R C compilers,
* environments providing deficient `stdio` implementations,
* environments with broken (or completely missing) division or modulo
  math operations,
* systems with byte sizes other than 8 bits (*e.g.*, DEC PDP‑10),
* systems where `NULL` is not equal to zero (*e.g.*,
  Honeywell 600/6000‑series),
* systems using one's‑complement integer representation (*e.g.*, Unisys
  ClearPath Dorado / OS 2200), and
* systems using non‑ASCII character sets (*e.g.*, IBM mainframes).

The only current requirements are:

* the C compiler must provide some storage type with a width at least as wide
  as the 32‑bit CRC,
* a character type with a width of at least 8 but no more than 32 bits, and
* a minimal, but working, `stdio` implementation (supporting at least
  `fprintf`, `fopen`, `fclose`, `fgetc`, `ferror`, `feof`, and `clearerr`).

It has been tested on various exotic and retro platforms including **Multics**
(Multics C), **CP/M-80** (z88dk), **MS-DOS** (IA16-GCC, Watcom, Microsoft C),
and **ELKS** (IA16-GCC), and should be able to be built anywhere else with
little to no porting effort required.

## Usage

```
Usage: CRC [options] <file>
Options:
  --bits=N    Process N bits per character
  --pad       Pad trailing bits with zeros
  --limit=N   Stop processing after N bits
  --help, -h  Show the help and usage text
```

## Building

The `crc.c` source code should build easily anywhere.  If you are using a
non-ANSI C compiler, you should define `NOANSI` (*i.e.*, `-DNOANSI`) or
modify the code to comment out the `#define ANSI_COMPILER` directive.

If you are trying to build in marginal environment using a C preprocessor that
does not deal with indentation, you can "flatten" the source using POSIX `sed`:
```sh
sed 's|^[[:space:]]*#[[:space:]]*|#|' crc.c > flat.c
```

### Multics

To build a binary for Multics using Multics C:
```sh
>sl3p>cc>e>cc -lg -of crc crc.c
```

Multics can be considered to run on exotic platform, the Honeywell 6000-series
36-bit large system mainframe.  This system uses 9 bits per character, where
most systems use 8 bits per character.  Multics stores data in "big-endian"
format, that is, "foreign" 8-bit data would be stored in the first most
significant positions (starting at bit 1 and ending with bit 8) and the 9th
or least most significant bit would remain unused.

When run on Multics or any other environment not using an 8-bit character
size, some instructional text will be appended to the `--help` output:

```
NOTE: This system has an 9-bit character size.
Use '--bits=8' to process 8-bit input data on this system.
```

For example, assume `DATA.DAT` is a file of 174,344 bits (21,793 8-bit octets)
which produces a CRC of `0D03ABFA` on MS-DOS or UNIX systems.  On a Multics
system, this file will be stored as 196,137 bits (21,793 9-bit nonets).  The
CRC on such a system will be calculated based on processing the file as a
stream of bits, handing off octets (of 8-bits each) to the CRC routines, which
includes the unused 9th bit in each character.  This means the CRC calculated
will not match that of the DOS or UNIX system unless the `--bits=8` option is
used to specify that only 8 bits per character should be considered.

As an additional hint, if the processing of a file ends with "dangling" bits
(not a full character) then a warning message is displayed.

Optionally, the `--pad` option can be used to zero-fill any remaining bits to
intentionally allow processing streams that end mid-character, or if a
`--limit` was specified that is more than the number of bits provided by the
input file.

### CP/M

To build a binary for CP/M-80, use a recent version of z88dk:
```sh
zcc +cpm -O3 -vn crc.c -o CRC.COM -DBUFSIZ=128 -DNOANSI
```

On CP/M-80 systems, files do not have exact sizes but are stored on disk
in fixed-size records of 1024 bits (*i.e.,* 128 8-bit octets) each.  Files
transferred from other systems that are not a multiple of the CP/M record size
will be padded with undefined data to fill a complete record, and there is no
universal EOF marker that can be used to find the true end of file.

By default, the `CRC.COM` program will calculate the CRC for all records on
disk associated with the file.  If you transferred the file from another type
of system that does support exact lengths, you can specify the number of bits
to process using the `--limit` flag.

For example, assume `DATA.DAT` is file of 174,344 bits (21,793 8-bit octets)
which produces a CRC of `0D03ABFA` on MS-DOS or UNIX systems.  On a CP/M-80
system the file will utilize 171 records of storage.  Since 21793 octets is not
a multiple of 128, the same CRC calculated will not match unless constrained
to process only 174,344 bits (*i.e.*, `CRC --limit=174344 DATA.DAT`).

CP/M-80 3.0 added a new filesystem metadata field: Last Record Byte Count (or
LRBC).  Unfortunately, the LRBC is stored as a number between 0 and 255, with
no official documented interpretation.  The DRI ISX software uses this field
to indicate the number of **unused** octets in the last record, while DRI
DOS-PLUS uses the field to indicate the number of **used** octets in the last
record, with a count of zero indicating 128.  Because of this ambiguity and
because accessing records requires the use of non-portable programming
constructs (direct BDOS function calls) the LRBC is not currently utilized.

If you need to verify files on CP/M that were created on other systems, you
should always constrain processing to the actual number of significant bits.

### ELKS

To build a binary for ELKS using IA16-GCC:
```sh
ia16-elf-gcc -std=c89 -Os -mregparmcall -melks crc.c -o crc
```

### DOS

To build a binary for MS-DOS using IA16-GCC:
```sh
ia16-elf-gcc -march=i8086 -std=c89 -Os -mregparmcall -mcmodel=tiny crc.c -o crc.com
```

## License

This software is distributed under the terms and conditions of the permissive
[MIT No Attribution (MIT-0)](LICENSE) license.

<!--
Local Variables:
mode: markdown
End:
-->
<!-- vim: set ft=markdown expandtab cc=80 : -->
<!-- EOF -->
