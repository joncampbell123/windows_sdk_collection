SAMPLES = \
	src\DDEnum \
	src\DDex1 \
	src\DDex2 \
	src\DDex3 \
	src\DDex4 \
	src\DDex5 \
	src\DDoverlay \
	src\Donut \
	src\Donuts \
	src\Flip2D \
	src\Font \
	src\MemTime \
	src\Mosquito \
	src\MultiNut \
	src\Stretch \
	src\Stretch2 \
	src\Stretch3 \
	src\Switcher \
	src\Wormhole

!IFDEF clean
do=-i clean
DOING=Clean
!ELSE

!IFDEF nodebug
do=nodebug=1
DOING=Release
!ELSE
DOING=Debug
!ENDIF
!ENDIF

$(SAMPLES): $(@R)\makefile
        @cd $@
        @echo *** DirectX\$@ $(DOING)***
        @echo *** DirectX\$@ $(DOING)*** >>..\DirectX.tmp
        @nmake $(do) >>..\DirectX.tmp
        @cd ..\..
