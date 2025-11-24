DIRS =	dunion	\
	inout	\
	xmit    \
	repas

all: $(DIRS)

$(DIRS):
	cd $@
	echo *** rpc\data\$@ *** >>$(MSTOOLS)\samples\data.tmp
	nmake -a -i 1>>$(MSTOOLS)\samples\data.tmp 2>&1
	cd ..
	
