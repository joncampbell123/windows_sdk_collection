DIRLIST =   acmapp    \
            aviedit   \
            aviview   \
            captest   \
            dseqfile  \
            icmapp    \
            icwalk    \
            langplay  \
            lowpass   \
            mciapp    \
            mciplay   \
            mcipuzzl  \
            midimon   \
            mixapp    \
            mmcaps    \
            movplay   \
            mplay     \
            palmap    \
            reverse   \
            soundapp  \
            textout   \
            vidcap    \
            wavefile  \
            writeavi  \
            midiplyr \
            idfedit  \
            joytoy

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
        cd $@
        @echo *** win32\$@ *** >>$(MSTOOLS)\samples\mm.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\mm.tmp
        cd ..
<<
	
