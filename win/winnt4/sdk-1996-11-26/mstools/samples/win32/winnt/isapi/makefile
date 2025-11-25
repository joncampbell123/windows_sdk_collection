!ifdef clean
makeopts = clean=1
!endif # clean

!ifdef nodebug
makeopts = $(makeopts) nodebug=1
!endif

# If sample is installed, cd into that directory and recursively start nmake.

DIRLIST =   async    \
            AuthFilt \
            CGIWrap  \
            CvtDoc   \
            FormDump \
            HomeFilt \
            IS2WCGI \
            IS2WCGI\testapp \
            Echo \
            ISrvMon \
            Lottery \
            NewLog \
            Redir \
            UpCase \
            WebRun

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** Win32\Winnt\$@ *** >>$(MSTOOLS)\samples\Isapi.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\Isapi.tmp
	cd ..
<<

Clean:
    @nmake -nologo /$(MAKEFLAGS) $(makeopts) clean=1
