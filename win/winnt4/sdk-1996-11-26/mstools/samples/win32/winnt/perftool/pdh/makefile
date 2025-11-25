DIRLIST =   proclist  \
            statlist  \
            typeperf

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** win32\winnt\perftool\$@ *** >>$(MSTOOLS)\samples\perftool.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\perftool.tmp
	cd ..
<<
	
