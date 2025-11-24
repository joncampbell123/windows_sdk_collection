DIRS =	client	\
	server

all: $(DIRS)

$(DIRS):
	cd $@
	@echo *** win32\sockets\$@ *** >>$(MSTOOLS)\samples\sockets.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\sockets.tmp
	cd ..
	
