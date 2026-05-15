# Makefile
# Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
# SPDX-License-Identifier: MIT-0
# scspell-id: 9d10afe6-4583-11f1-bfc5-80ee73e9b8e7

################################################################################

CC=`command -v cc 2> /dev/null || command -v gcc 2> /dev/null || \
	command -v clang 2> /dev/null || echo cc`

################################################################################

all: crc

################################################################################

crc: crc.c
	@eval echo \
		"$${CC-$(CC)}" $${CFLAGS--O3} $${LDFLAGS-} -o $@ crc.c || :
	@eval \
		"$${CC-$(CC)}" $${CFLAGS--O3} $${LDFLAGS-} -o $@ crc.c

################################################################################

clean:
	rm -f crc ./*.o ./*.obj ./*.sym

################################################################################

distclean: clean
	rm -f .test_results.log tags cscope.out GPATH GRTAGS GTAGS TAGS
	git clean -ndx 2> /dev/null || :

################################################################################

test: .test.sh .common.sh
	@sh .test.sh

################################################################################

lint: .lint.sh .common.sh
	@sh .lint.sh

################################################################################

tags etags ctags gtags TAGS GPATH GRTAGS GTAGS cscope cscope.out tag: crc.c
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

################################################################################

.PHONY: all clean distclean test lint tags etags ctags gtags TAGS GPATH GRTAGS \
	GTAGS cscope cscope.out tag

################################################################################

.NOTPARALLEL:

################################################################################

# Local Variables:
# mode: makefile
# indent-tabs-mode: t
# tab-width: 8
# whitespace-style: (tabs tab-mark)
# whitespace-display-mappings: ((tab-mark 9 [45] [45]))
# fill-column: 80
# eval: (setq-local whitespace-display-mappings
#                   '((tab-mark 9
#                               [45 45 45 45 45 45 62]
#                               [45 45 45 45 45 45 62])))
# eval: (whitespace-mode 1)
# eval: (setq-local display-fill-column-indicator-column 80)
# eval: (display-fill-column-indicator-mode 1)
# End:

################################################################################
# vim: set ft=make ts=8 ai noexpandtab list listchars=tab\:\>\- cc=80 :
################################################################################
