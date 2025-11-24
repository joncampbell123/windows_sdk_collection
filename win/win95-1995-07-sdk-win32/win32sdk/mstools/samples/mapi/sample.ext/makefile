DIRLIST =   command events propshet

all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** mapi\sample.ext\$@ *** >>$(MSTOOLS)\samples\mapiEXT.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\mapiEXT.tmp
	cd ..
	
