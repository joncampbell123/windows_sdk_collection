DIRS =	auto	\
	cxhndl	\
	usrdef	

all: $(DIRS)

$(DIRS):
	cd $@
	echo *** rpc\handles\$@ *** >>$(MSTOOLS)\samples\handles.tmp
	nmake -a -i 1>>$(MSTOOLS)\samples\handles.tmp 2>&1
	cd ..
	
