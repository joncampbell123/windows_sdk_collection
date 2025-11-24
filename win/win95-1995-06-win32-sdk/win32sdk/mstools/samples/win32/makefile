DIRLIST =   comm      \
            console   \
            congui    \
            cdtest    \
            deb       \
            ddeml     \
            dyndlg    \
            fontview  \
            gdidemo   \
            globchat  \
            generic   \
            help      \
            ipxchat   \
            mandel    \
            mapi      \
            mazelord  \
            memory    \
            mfedit    \
            mltithrd  \
            mypal     \
            namepipe  \
            nwlink    \
            rasberry  \
            reitp     \
            reversi   \
            textfx    \
            ttfonts   \
            pdc       \
            printer   \
            registry  \
            select    \
            showdib   \
            simple    \
            spincube  \
            wdbgexts  \
            wincap32  \
            wsock

all: $(DIRLIST) winnt win95

$(DIRLIST):
	cd $@
	@echo *** win32\$@ *** >>$(MSTOOLS)\samples\win32.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\win32.tmp
	cd ..
	
winnt:
!IF EXIST (winnt)
    cd winnt
    @nmake -a -i -nologo
    cd ..
!ENDIF

win95:
!IF EXIST (win95)
    cd win95
    @nmake -a -i -nologo
    cd ..
!ENDIF
