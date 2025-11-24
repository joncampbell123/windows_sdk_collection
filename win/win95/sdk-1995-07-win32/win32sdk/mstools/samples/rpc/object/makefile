DIRS = \
    ohello \
    callas

all: $(DIRS)

$(DIRS):
        cd $@
        echo *** rpc\object\$@ *** >>$(MSTOOLS)\samples\object.tmp
        nmake -a -i >>$(MSTOOLS)\samples\object.tmp
        cd ..

