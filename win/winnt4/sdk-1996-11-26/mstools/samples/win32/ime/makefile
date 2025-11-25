DIRLIST =   fullime  \
            halfime  \
            imeapps  \
            multiui



all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** win32\ime\$@ *** >>$(MSTOOLS)\samples\ime.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\ime.tmp
	cd ..
	
