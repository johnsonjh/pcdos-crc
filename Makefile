all: crc crc-ow2.com crc-djgpp.exe

##############################################################################
# POSIX-like (tested on Linux with GCC):

crc: crc.c
	$(CC) -Os $(CFLAGS) crc.c -o crc $(LDFLAGS)

##############################################################################
# OpenWatcom V2 (tested with version 2.0 beta Oct 25 2025):

WATCOM_DIR=/opt/watcom

crc-ow2.com: crc.c
	export PATH="$(WATCOM_DIR)/binl64:$${PATH:-}" && \
	export WATCOM="$(WATCOM_DIR)" && \
	export INCLUDE="$(WATCOM_DIR)/h" && \
	  $(WATCOM_DIR)/binl64/owcc -c -bcom -march=i86 -fsigned-char \
	    -mcmodel=t -g0 -frerun-optimizer -Os -fno-stack-check \
	    -o ./crc-ow2.obj ./crc.c && \
          $(WATCOM_DIR)/binl64/owcc -bcom -s \
	    -o ./crc-ow2.com ./crc-ow2.obj && \
        rm -f ./crc-ow2.obj

##############################################################################
# DJGPP (tested with DJGPP version 14.3.0 and CWSDPMI r7a):

DJGPP_DIR=/opt/djgpp
DJGPP_ARCH=i586-pc-msdosdjgpp
CWSDSTUB=/opt/cwspdmi/cwsdstub.exe

crc-djgpp.exe: crc.c $(CWSDSTUB) $(DJGPP_DIR)/$(DJGPP_ARCH)/bin/exe2coff
	rm -f crc-djgpp.exe crc-djgpp
	env PATH="$(DJGPP_DIR)/$(DJGPP_ARCH)/bin:$(DJGPP_DIR)/bin:$${PATH:-}" \
	  $(DJGPP_DIR)/bin/$(DJGPP_ARCH)-gcc -march=i386 -Os \
	    -o ./crc-djgpp.exe ./crc.c
	$(DJGPP_DIR)/$(DJGPP_ARCH)/bin/exe2coff ./crc-djgpp.exe && \
	  rm -f ./crc-djgpp.exe && \
	  cat $(CWSDSTUB) ./crc-djgpp > ./crc-djgpp.exe && \
	  rm -f ./crc-djgpp
	@chmod a+x crc-djgpp.exe > /dev/null 2>&1 || :

##############################################################################
# Linting (just Cppcheck and Oracle)

# NOTE: For Clang Analyzer: `make clean && scan-build make crc`

lint: crc.c
	cppcheck --force --check-level=exhaustive crc.c
	/opt/oracle/developerstudio12.6/bin/lint \
	  -errshort=tags -errtags=yes crc.c

##############################################################################
# Cleanup

clean:
	rm -f crc crc-ow2.obj crc-ow2.com crc-djgpp crc-djgpp.exe a.out

##############################################################################

.PHONY: all clean lint

##############################################################################
