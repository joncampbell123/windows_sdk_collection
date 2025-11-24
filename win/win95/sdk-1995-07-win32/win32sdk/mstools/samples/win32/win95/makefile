DIRLIST =   comdlg32 \
            dropext  \
            fileview \
            fparser  \
            icmtest  \
            pkpd     \
            shellext \
            traynot  \
            wizard

all: $(DIRLIST) hform inkput tapicomm ime

$(DIRLIST):
	cd $@
	@echo *** win32\win95\$@ *** >>$(MSTOOLS)\samples\win3295.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\win3295.tmp
	cd ..
	

hform:
!IF EXIST (hform)
    @echo *** win32\win95\hform *** >>$(MSTOOLS)\samples\hform.tmp
    cd hform
    @nmake -a -i -nologo >>$(MSTOOLS)\samples\hform.tmp
    cd ..
!ENDIF


inkput:
!IF EXIST (inkput)
    @echo *** win32\win95\inkput *** >>$(MSTOOLS)\samples\inkput.tmp
    cd inkput
    @nmake -a -i -nologo >>$(MSTOOLS)\samples\inkput.tmp
    cd ..
!ENDIF


tapicomm:
!IF EXIST (tapicomm)
    @echo *** win32\win95\tapicomm *** >>$(MSTOOLS)\samples\tapicomm.tmp
    cd tapicomm
    @nmake -a -i -nologo >>$(MSTOOLS)\samples\tapicomm.tmp
    cd ..
!ENDIF


ime:
!IF EXIST (ime)
    cd ime
    @nmake -a -i -nologo
    cd ..
!ENDIF
