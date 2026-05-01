# Makefile
# Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
# SPDX-License-Identifier: MIT-0
# scspell-id: 9d10afe6-4583-11f1-bfc5-80ee73e9b8e7
# vim: set ft=make ts=8 ai noexpandtab list listchars=tab\:\>\- cc=80 :

crc: crc.c
	cc -O -o crc crc.c

clean:
	rm -f crc

distclean: clean
	rm -f .test_results.log
	command -v git > /dev/null 2>&1 && git clean -ndx 2> /dev/null || :

test: .test.sh
	sh .test.sh

lint: .lint.sh
	sh .lint.sh

.PHONY: clean distclean test lint
