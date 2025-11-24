
ALLDIRS = BNDBUF \
	  BOB \
	  BPOINTER \
	  CHECK_SD \
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
	  MSGTABLE \
	  NETDDE \
	  NTSD \
	  OTHRPROC \
	  OWNCOMBO \
	  PATHS \
	  PLATFORM \
	  POLYDRAW \
	  READWRIT \
	  REBOOT \
	  REGIONS \
	  RESDLL \
	  SCRNSAVE \
	  SD_FLPPY \
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
