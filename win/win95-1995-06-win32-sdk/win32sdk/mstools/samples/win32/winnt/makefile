DIRLIST =   filer     \
            floppy    \
            gina      \
            gridfont  \
            netapi    \
            plgblt    \
            pop3      \
            prperf    \
            regmpad   \
            rnr       \
            secfile   \
            service   \
            sidcln    \
            simplex   \
            snmp      \
            sockets   \
            takeown   \
            unbufcpy  \
            wxform

all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** win32\winnt\$@ *** >>$(MSTOOLS)\samples\win32nt.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\win32nt.tmp
	cd ..
	
