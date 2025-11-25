OPTDIRS =   comdlg32 \
            dropext \
            fileview \
            fparser  \
            icmtest  \
            shellext \
            traynot  \
            wizard   \
            animate  \
            hform    \
            inkput   \
            jform    \
            pkpd     \
            tapicomm


all:  $(OPTDIRS)

	
$(OPTDIRS):
        IF EXIST $@\nul <<nmaketmp.bat
        cd $@
        @echo *** win32\win95\$@ *** >>$(MSTOOLS)\samples\win95opt.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\win95opt.tmp
        cd ..
<<
