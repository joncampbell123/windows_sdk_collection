DIRLIST =   datauser    \
            cocosmo     \
            ddataobj    \
            edataobj    \
            freeload    \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap06\$@ *** >>$(SDKROOT)\samples\chap06.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap06.tmp
        cd ..

