DIRS =	npclient	\
	npserver

all: $(DIRS)

$(DIRS):
	cd $@
	echo *** $@ *** >>$(MSTOOLS)\samples\namepipe.tmp
	nmake -a -i 1>>$(MSTOOLS)\samples\namepipe.tmp 2>&1
	cd ..
	
