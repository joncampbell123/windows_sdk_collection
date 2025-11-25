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
            iconpro   \
            lsapi     \
            mandel    \
            mapi      \
            mazelord  \
            memory    \
            mfedit    \
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
            setdisp   \
            showdib   \
            simple    \
            spincube  \
            wdbgexts  \
            wincap32  \
            listknj   \
            tate      \
            helpex    \
            cabview   \
            comdlg32  \
            dropext   \
            fileview  \
            shellext  \
            traynot   \
            wizard

OPTDIRS =   winnt     \
            win95     \
            winsock2  \
            tapi      \
            ime

all: $(DIRLIST) $(OPTDIRS)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** win32\$@ *** >>$(MSTOOLS)\samples\win32.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\win32.tmp
	cd ..
<<

$(OPTDIRS):
        IF EXIST $@\nul <<nmaketmp.bat
        cd $@
        @nmake -a -i -nologo
        cd ..
<<
