DIRS =	dll	\
        app

all: $(DIRS)

$(DIRS):
	cd $@
	echo *** $@ *** >>$(MSTOOLS)\samples\spy.tmp
	nmake -a -i -nologo >>$(MSTOOLS)\samples\spy.tmp
	cd ..
	
