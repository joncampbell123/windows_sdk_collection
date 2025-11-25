@echo *
@echo ***Batch File to build examples with Visual C++
@echo *
cd ddex1
nmake %1 debug retail
cd ..\ddex2
nmake %1 debug retail
cd ..\ddex3
nmake %1 debug retail
cd ..\ddex4
nmake %1 debug retail
cd ..\ddex5
nmake %1 debug retail
cd ..\donut
nmake %1 debug retail
cd ..\roids
nmake %1 debug retail
cd ..\fastfile
nmake %1 debug retail
cd ..\foxbear
nmake %1 debug retail
cd ..\palette
nmake %1 debug retail
cd ..\dsshow
nmake %1 debug retail
cd ..\dsstream
nmake %1 debug retail
cd ..\mid2strm
nmake %1 debug retail 
cd ..\mstream
nmake %1 debug retail 
cd ..\dptest
nmake %1 debug retail
cd ..\duel
nmake %1 debug retail
cd ..\wormhole
nmake %1 debug retail
cd ..\stretch
nmake %1 debug retail
cd ..\memtime
nmake %1 debug retail
cd ..\dxview
nmake %1 debug retail
cd ..\flipcube
nmake %1 debug retail
cd ..\setup
nmake %1 debug retail
cd ..\cgutil
nmake %1 debug retail
cd ..\iklowns
nmake %1 debug retail

