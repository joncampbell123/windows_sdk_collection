DIRS =	auxdemo	 \
	backtrce \
	gengl	 \
	stonehng

all: $(DIRS)

$(DIRS):
	cd $@
	@echo *** opengl\demos\$@ *** >>$(MSTOOLS)\samples\demos.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\demos.tmp
	cd ..
	
