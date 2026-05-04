# Makefile
# Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
# SPDX-License-Identifier: MIT-0
# scspell-id: 9d10afe6-4583-11f1-bfc5-80ee73e9b8e7
# vim: set ft=make ts=8 ai noexpandtab list listchars=tab\:\>\- cc=80 :

CC=`command -v cc 2> /dev/null || command -v gcc 2> /dev/null || \
	command -v clang 2> /dev/null || echo cc`

all: crc

crc: crc.c
	@eval echo \
		"$${CC-$(CC)}" $${CFLAGS--O3} $${LDFLAGS-} -o $@ crc.c || :
	@eval \
		"$${CC-$(CC)}" $${CFLAGS--O3} $${LDFLAGS-} -o $@ crc.c

clean:
	rm -f crc

distclean: clean
	rm -f .test_results.log tags cscope.out GPATH GRTAGS GTAGS TAGS
	git clean -ndx 2> /dev/null || :

test: .test.sh
	@sh .test.sh

lint: .lint.sh
	@sh .lint.sh

tags etags ctags gtags TAGS GPATH GRTAGS GTAGS cscope cscope.out: crc.c
	@command -v etags > /dev/null 2>&1 && \
		{ { echo etags...; etags crc.c && exit 0; }; \
			exit 1; } || :
	@command -v ctags > /dev/null 2>&1 && \
		{ { echo ctags...; ctags crc.c && exit 0; }; \
			exit 1; } || :
	@command -v gtags > /dev/null 2>&1 && \
		{ { echo gtags...; gtags --single-update crc.c && exit 0; }; \
			exit 1; } || :
	@command -v cscope > /dev/null 2>&1 && \
		{ { echo cscope...; cscope -b crc.c && exit 0; }; \
			exit 1; } || :

.PHONY: clean distclean test lint tags etags ctags gtags TAGS GPATH GRTAGS \
	GTAGS cscope cscope.out
.NOTPARALLEL:
