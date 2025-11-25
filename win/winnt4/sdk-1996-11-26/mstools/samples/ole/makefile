DIRLIST =   com             \
            common          \
            cmallspy        \
            browse          \
            browseh         \
            bttncur         \
            dcom            \
            decode          \
            gizmobar        \
            olestd          \
            winhlprs        \
            dfview          \
            dispcalc        \
            dispdemo        \
            dspcalc2        \
            hello           \
            lines           \
            mfract          \
            oleapt          \
            outline         \
            simpcntr        \
            simpdnd         \
            simpsvr         \
            spoly           \
            spoly2          \
            tibrowse        \
            inole2


all: $(DIRLIST)


$(DIRLIST):
        cd $@
        @echo *** ole\$@ *** >>$(MSTOOLS)\samples\ole.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\ole.tmp
        cd ..
