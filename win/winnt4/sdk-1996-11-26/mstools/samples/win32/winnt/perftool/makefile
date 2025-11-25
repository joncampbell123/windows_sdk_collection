DIRLIST =   pdh    \
            perfdlls

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@nmake -a -i -nologo
	cd ..
<<
	
