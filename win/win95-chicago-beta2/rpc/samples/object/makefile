DIRS = \
    ohello \
    callas

all: $(DIRS)

$(DIRS):
	cd $@
	echo *** rpc\object\$@ *** >>$(MSTOOLS)\samples\object.tmp
	nmake -a -i 1>>$(MSTOOLS)\samples\object.tmp 2>&1
	cd ..
	
