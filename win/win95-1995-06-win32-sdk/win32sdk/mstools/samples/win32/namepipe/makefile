DIRS =	npclient	\
	npserver        \
        console

all: $(DIRS)

$(DIRS):
	cd $@
	echo *** $@ *** >>$(MSTOOLS)\samples\namepipe.tmp
	nmake -a -i -nologo >>$(MSTOOLS)\samples\namepipe.tmp
	cd ..
	
