DIRS =	 nhello	

all: $(DIRS)

$(DIRS):
	cd $@
	echo *** rpc\ns\$@ *** >>$(MSTOOLS)\samples\ns.tmp
	nmake -a -i 1>>$(MSTOOLS)\samples\ns.tmp 2>&1
	cd ..
	
