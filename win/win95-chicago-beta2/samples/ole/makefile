DIRLIST =   common          \
            bttncur         \
            gizmobar        \
            ole2ui          \
            dispcalc        \
            dispdemo        \
            dspcalc2        \
            hello           \
            spoly           \
            spoly2          \
            tibrowse        \
            outline         \
            simpcntr        \
            simpdnd         \
            simpsvr         \
            winhlprs        \
            mfract          \
            dfview          \
            defo2v          \
            inole2


all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\$@ *** >>$(SDKROOT)\samples\ole.tmp
        @nmake -a -i >>$(SDKROOT)\samples\ole.tmp
        cd ..

