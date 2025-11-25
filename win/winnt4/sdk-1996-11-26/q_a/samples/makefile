
ALLDIRS = ASYN_IO \
          BNDBUF \
	  BOB \
	  BPOINTER \
	  CLIPTEXT \
	  CMNDLG \
	  CONSOLEC \
	  CPL \
	  CURSOR \
	  DRIVES \
	  EVENT \
	  GETDEV \
	  GETSYS \
	  GLTHREAD \
	  GUIGREP \
	  HOOKS \
	  INHERIT \
	  INTEROP \
	  IOCOMP \
	  LOCALE \
	  LOGGING \
	  MASKBLT \
	  MDI_ \
	  MENU \
	  MINREC \
	  NETDDE \
	  NTSD \
	  OTHRPROC \
	  OWNCOMBO \
          OVCOMM \
	  PATHS \
	  PLATFORM \
	  POLYDRAW \
	  READWRIT \
	  REBOOT \
	  REGIONS \
	  RESDLL \
	  SCRNSAVE \
	  SEMAPHOR \
	  SERVENUM \
	  SETINFO \
	  SHAREMEM \
	  SIMPLDLL \
	  STARTP \
	  STREBLT \
	  SUBCLASS \
	  TERMPROC \
	  THREADS \
	  TIMERS \
	  TLS \
	  UNIPUT \
	  VIRTMEM \
	  WALK \
	  WINNET \
	  WORLD


all: $(ALLDIRS)

$(ALLDIRS):
		cd $@
		@nmake -a -i >>..\$@.tmp
		cd ..
		@if exist samples.log copy samples.log+*.tmp samples.log >nul
		@if not exist samples.log copy *.tmp samples.log >nul
		@del *.tmp >nul
