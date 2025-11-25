DIRLIST =  chngpass   \
           netmsg     \
           netshare   \
           netsmple   \
           nullsess

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** win32\winnt\netapi\$@ *** >>$(MSTOOLS)\samples\netapi.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\netapi.tmp
	cd ..
<<
