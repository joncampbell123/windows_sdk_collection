DIRLIST =   fullime \
            halfime  \
            imeapps


all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** win32\win95\ime\$@ *** >>$(MSTOOLS)\samples\ime.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\ime.tmp
	cd ..
	
