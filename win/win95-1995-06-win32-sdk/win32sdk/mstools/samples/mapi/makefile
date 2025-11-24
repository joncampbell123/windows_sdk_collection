DIRLIST =   CHKRFORM   \
            CMCCLI     \
            SAMPLE.AB  \
            sample.ext \
            SAMPLE.MS  \
            SAMPLE.XP  \
            SMH        \
            SMPCLI

all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** mapi\$@ *** >>$(MSTOOLS)\samples\mapi.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\mapi.tmp
	cd ..
	
