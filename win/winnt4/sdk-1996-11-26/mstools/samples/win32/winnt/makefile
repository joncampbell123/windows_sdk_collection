DIRLIST =   crypto    \
            fibers    \
            filer     \
            floppy    \
            fparser   \
            fwatch    \
            gridfont  \
            mousinfo  \
            mpheap    \
            msgtable  \
            netapi    \
            perftool  \
            plgblt    \
            pop3      \
            prperf    \
            rashost   \
            rassapi   \
            regmpad   \
            rnr       \
            security  \
            service   \
            simplex   \
            snmp      \
            unbufcpy  \
            wxform    \
            gridmap \
            isapi

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** win32\winnt\$@ *** >>$(MSTOOLS)\samples\win32nt.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\win32nt.tmp
	cd ..
<<
	
