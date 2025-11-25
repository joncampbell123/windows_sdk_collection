DIRLIST =  acdsmpl  \
           acdclnt


all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** win32\tapi\acd\$@ *** >>$(MSTOOLS)\samples\tapi_acd.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\tapi_acd.tmp
	cd ..
<<
