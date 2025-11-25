SAMPLES = \
        bellhop \
        boids \
        d3dex1 \
        d3dex2 \
        ddex1 \
	ddex2 \
	ddex3 \
	ddex4 \
	ddex5 \
        diex1 \
        diex2 \
        diex3 \
        diex4 \
        diff1 \
        donut \
	donuts \
        dpchat \
	dplaunch \
        dpslots \
        ds3dview \
	dsshow \
	dsshow3d \
	dsstream \
	duel \
	dxview \
	egg \
	faces \
	fastfile \
	fdfilter \
        ffdonuts \
	flip2d \
        flip3d \
	fly \
	foxbear \
	globe \
	hier1 \
	hier2 \
	memtime \
	mid2strm \
        morph \
        mosquito \
	mstream \
        override \
	palette \
        pplane \
	quat \
	rockem \
	scrawl \
	setup \
	shadow \
	stretch \
        stretch2 \
        stretch3 \
	tex1 \
	tex3 \
	tex4 \
	tex5 \
	trans \
        tri3 \
	uvis \
	viewer \
	wormhole

$(SAMPLES): $(@R)\nul
        @cd $@
        @echo *** DirectX\$@ $(DOING)***
        @echo *** DirectX\$@ $(DOING)*** >>..\DirectX.tmp
        @$(MAKE) $(DO) >>..\DirectX.tmp
        @cd ..

default: retail

clean:
        $(MAKE) DO="-i clean" DOING="Clean"

retail:
        $(MAKE) DO="-i nodebug=1" DOING="Retail"

debug:
        $(MAKE) DO="-i" DOING="Debug"
        
