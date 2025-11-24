DIRS =	picklp	\
	picklt	

all: $(DIRS)

$(DIRS):
	cd $@
	echo *** rpc\pickle\$@ *** >>$(MSTOOLS)\samples\pickle.tmp
	nmake -a -i 1>>$(MSTOOLS)\samples\pickle.tmp 2>&1
	cd ..
	
