DIRLIST =	cacls           \
		compact         \
		dbmon           \
        ddespy          \
        dlgedit         \
		tlist           \
		perfmon		\
		netwatch	\
		pviewer		\
		remote		\
                switcher        \
		uconvert	\
		winat		\
		walker

all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** sdktools\winnt\$@ *** >>$(MSTOOLS)\samples\sdktlsnt.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\sdktlsnt.tmp
	cd ..
	
