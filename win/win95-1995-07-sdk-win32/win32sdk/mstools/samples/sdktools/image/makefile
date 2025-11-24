DIRS =	imagehlp	\
	drwatson	\
	symcvt          \
	symedit

all: $(DIRS) pfmon

$(DIRS):
	cd $@
	echo *** sdktools\image\$@ *** >>$(MSTOOLS)\samples\image.tmp
	nmake -a -i -nologo >>$(MSTOOLS)\samples\image.tmp
	cd ..

pfmon:
	cd winnt\$@
	echo *** sdktools\image\winnt\$@ *** >>$(MSTOOLS)\samples\image.tmp
	nmake -a -i -nologo >>$(MSTOOLS)\samples\image.tmp
	cd ..\..
	
