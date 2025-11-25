DIRLIST =  acd        \
           atsp32     \
           dialer     \
           tapicomm

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** win32\tapi\$@ *** >>$(MSTOOLS)\samples\tapi.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\tapi.tmp
	cd ..
<<
	
