DIRLIST =   cfiler   \
            encrypt  \
            enumalgs \
            inituser \
            sign

all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** win32\crypto\$@ *** >>$(MSTOOLS)\samples\crypt.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\crypt.tmp
	cd ..
	
