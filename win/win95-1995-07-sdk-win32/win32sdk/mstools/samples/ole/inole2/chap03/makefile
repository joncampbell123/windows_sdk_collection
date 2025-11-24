DIRLIST =   enumc       \
            enumcpp

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap03\$@ *** >>$(MSTOOLS)\samples\chap03.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap03.tmp
        cd ..

