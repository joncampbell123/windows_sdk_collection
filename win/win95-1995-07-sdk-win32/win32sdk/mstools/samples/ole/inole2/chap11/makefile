DIRLIST =   hcosmo \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap11\$@ *** >>$(MSTOOLS)\samples\chap11.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap11.tmp
        cd ..

