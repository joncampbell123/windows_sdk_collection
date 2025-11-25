DIRLIST =	aniedit		\
		fontedit	\
		image           \
		imagedit	\
		mc              \
		scrnsave        \
		spy    	        \
		porttool	\
		windiff		\
		zoomin		

!IF EXIST (WINNT)
all: $(DIRLIST) winnt
!ELSE
all: $(DIRLIST)
!ENDIF

$(DIRLIST):
	cd $@
	@echo *** sdktools\$@ *** >>$(MSTOOLS)\samples\sdktools.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\sdktools.tmp
	cd ..

winnt:
	cd $@
	@nmake -a -i -nologo
	cd ..
	
