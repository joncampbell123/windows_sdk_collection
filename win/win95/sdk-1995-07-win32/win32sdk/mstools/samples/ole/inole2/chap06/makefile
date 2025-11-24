DIRLIST =   datauser    \
            cocosmo     \
            ddataobj    \
            edataobj    \
            freeload    \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap06\$@ *** >>$(MSTOOLS)\samples\chap06.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap06.tmp
        cd ..

