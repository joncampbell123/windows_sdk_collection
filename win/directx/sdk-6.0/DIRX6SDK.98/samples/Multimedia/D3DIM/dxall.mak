SAMPLES = \
	src\Bend \
	src\Billboard \
	src\Boids \
	src\bumpmap \
	src\compress \
	src\Filter \
	src\Fireworks \
	src\Flare \
	src\Fog \
	src\lightmap \
	src\Lights \
	src\MipMap \
	src\MTexture \
	src\PPlane \
	src\shadowvol \
	src\shadowvol2 \
	src\SphereMap \
	src\TunnelDP \
	src\TunnelEB \
	src\vbuffer \
	src\videotex \
	src\wbuffer \
	src\xfile \
	src\D3dFrame

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
