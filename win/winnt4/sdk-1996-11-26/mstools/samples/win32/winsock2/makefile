DIRLIST =  bcast      \
           dt_dll     \
           iocomplt   \
           ipxchat    \
           layered    \
           nullprov   \
           overlap    \
           ping       \
           simple     \
           sockspx    \
           ws2chat    \
           wsock

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** win32\winsock2\$@ *** >>$(MSTOOLS)\samples\winsock2.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\winsock2.tmp
	cd ..
<<
	
