#
# $Id: Makefile.am 150 2010-10-09 17:51:50Z Michael.McTernan $
#

AM_CFLAGS = -Wall
AM_YFLAGS = -d

BUILT_SOURCES = language.c language.h lexer.c

CLEANFILES = $(BUILT_SOURCES)

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = mscgen
mscgen_SOURCES = \
adraw.c      cmdparse.c  main.c      svg_out.c     language.y \
adraw.h      cmdparse.h  msc.c       ps_out.c      utf8.c \
adraw_int.h  gd_out.c    msc.h       safe.c        utf8.h \
bool.h       lexer.l     lexer.h     null_out.c    safe.h \
usage.c      usage.h

mscgen_CFLAGS =
mscgen_LDADD = -lm

# END OF FILE
