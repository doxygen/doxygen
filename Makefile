# this Makefile need GNU make

include Makefile.config

variables: Makefile
	@echo
	@echo "Select one of the following targets:"
	@echo
	@echo "aix-g++          aix-xlc        "
	@echo "dgux-g++         freebsd-g++    "
	@echo "gnu-g++          hpux-acc       "
	@echo "hpux-cc          hpux-g++       "
	@echo "irix-64          irix-dcc       "
	@echo "irix-g++         irix-n32       "
	@echo "linux-g++        netbsd-g++     "
	@echo "openbsd-g++      osf1-cxx       "
	@echo "osf1-g++         qnx-g++        "
	@echo "sco-g++          solaris-cc     "
	@echo "solaris-g++      sunos-g++      "
	@echo "ultrix-g++       unixware-g++   "
	@echo 
	@echo "Make any of them to build Doxygen."
	@echo "Make will now abort with an error."
	@exit 1

aix-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/aix-g++ \
	$(MAKE)

aix-xlc: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/aix-xlc \
	$(MAKE)

dgux-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/dgux-g++ \
	$(MAKE)

gnu-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/gnu-g++ \
	$(MAKE)

freebsd-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/freebsd-g++ \
	$(MAKE)

hpux-acc: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/hpux-acc \
	$(MAKE)

hpux-cc: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/hpux-cc \
	$(MAKE)

hpux-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/hpux-g++ \
	$(MAKE)

irix-64: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/irix-64 \
	$(MAKE)

irix-dcc: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/irix-dcc \
	$(MAKE)

irix-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/irix-g++ \
	$(MAKE)

irix-n32: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/irix-n32 \
	$(MAKE)

linux-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/linux-g++ \
	$(MAKE)

netbsd-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/netbsd-g++ \
	$(MAKE)

openbsd-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/openbsd-g++ \
	$(MAKE)

osf1-cxx: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/osf1-cxx \
	$(MAKE)

osf1-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/osf1-g++ \
	$(MAKE)

qnx-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/qnx-g++ \
	$(MAKE)

sco-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/sco-g++ \
	$(MAKE)

solaris-cc: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/solaris-cc \
	$(MAKE)

solaris-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/solaris-g++ \
	$(MAKE)

sunos-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/sunos-g++ \
	$(MAKE)

ultrix-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/ultrix-g++ \
	$(MAKE)

unixware-g++: src/version.cpp
	cd src; \
	TMAKEPATH=../tmake/lib/unixware-g++ \
	$(MAKE)


clean: FORCE
	cd examples ; $(MAKE) clean
	cd doc ; $(MAKE) clean
	-rm -f  src/scanner.cpp \
		src/code.cpp \
		src/config.cpp \
		src/pre.cpp \
		src/tag.cpp \
		src/ce_lex.cpp \
		src/ce_parse.cpp \
		src/ce_parse.h \
		src/doxytag.cpp \
		src/declinfo.cpp \
		src/defargs.cpp \
		src/Makefile.doxygen \
		src/Makefile.doxytag \
		src/Makefile.doxysearch \
		src/version.cpp
	-rm -f bin/doxy*
	-rm -f objects/*.o

docs: FORCE
	cd examples ; $(MAKE)
	cd doc ; $(MAKE)

ps: docs
	cd latex ; $(MAKE)

archive: clean
	$(TAR) zcvf backup/dx`date +%y%m%d`.tgz tmake doc examples bin objects \
	       src Makefile Makefile.windows INSTALL make.bat Makefile.config \
	       LANGUAGE.HOWTO LICENSE

src/version.cpp: FORCE
	echo "char versionString[]=\"$(VERSION)\";" > src/version.cpp

FORCE:
