
# Nmake macros for building Windows 32-Bit apps

TARGETOS=WINNT
APPVER=4.0
U_FLAGS=-DUNICODE=1 -D_UNICODE=1

all:client.exe oleimp.exe

!include <win32.mak>

#
# OLEIMP Client
#

client.obj: client.cpp
    $(cc) $(cflags) $(cvarsmt) $(cdebug) -Zi -Fdclient.pdb client.cpp $(U_FLAGS)

client.exe: client.obj
    $(link) -debug:full -debugtype:cv -pdb:client.pdb -out:client.exe client.obj $(olelibsmt) /NODEFAULTLIB -subsystem:console

#
# OLEIMP Server
#

oleimp.obj: oleimp.cpp
	$(cc) $(cflags) $(cvarsmt) $(cdebug) -GX -Zi -Fdoleimp.pdb oleimp.cpp $(U_FLAGS)

util.obj: util.cpp
	$(cc) $(cflags) $(cvarsmt) $(cdebug) -GX -Zi -Fdoleimp.pdb util.cpp $(U_FLAGS)

oleimp.exe: oleimp.obj util.obj
	$(link) -debug:full -debugtype:cv -pdb:oleimp.pdb -out:oleimp.exe $** $(olelibsmt) /NODEFAULTLIB -subsystem:windows,4.0

clean:
    del *.exe
    del *.obj
    del *.pdb
    del *.ilk
	del *.bak
