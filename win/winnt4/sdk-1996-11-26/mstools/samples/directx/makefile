DIRLIST =   ddex1 \
            ddex2 \
            ddex3 \
            ddex4 \
            ddex5 \
            donut \
            dsshow \
            dsstream \
            duel \
            dxview \
            egg \
            faces \
            fastfile \
            flip2d \
            flipcube \
            fly \
            foxbear \
            globe \
            hier1 \
            hier2 \
            memtime \
            mid2strm \
            mstream \
            oct1 \
            oct2 \
            palette \
            quat \
            rockem \
            setup \
            shadow \
            sphere \
            stretch \
            tex1 \
            tex3 \
            tex4 \
            tex5 \
            trans \
            triangle \
            tunnel \
            twist \
            uvis \
            viewer \
            wormhole

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
	cd $@
	@echo *** DirectX\$@ *** >>$(MSTOOLS)\samples\DirectX.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\DirectX.tmp
	cd ..
<<
