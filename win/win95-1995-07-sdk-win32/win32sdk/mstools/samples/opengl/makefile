DIRLIST =	book	\
		demos	\
		glaux	\
		pipes	\
		saver	\
		samples

all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** opengl\$@ *** >>$(MSTOOLS)\samples\opengl.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\opengl.tmp
	cd ..
	
