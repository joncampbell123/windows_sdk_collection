proj=DS3DView

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

!include "$(DXSDKROOT)\include\dxsdk.inc"

mycflags=

OBJ_LIST = $(OBJ_DIR)\dsutil3d.obj $(OBJ_DIR)\file.obj \
	$(OBJ_DIR)\rlds3d.obj $(OBJ_DIR)\rodcone.obj $(OBJ_DIR)\viewer.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\dsutil3d.obj:  dsutil3d.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\dsutil3d.obj dsutil3d.c

$(OBJ_DIR)\file.obj:  file.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\file.obj file.cpp

$(OBJ_DIR)\rlds3d.obj:  rlds3d.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\rlds3d.obj rlds3d.cpp

$(OBJ_DIR)\rodcone.obj:  rodcone.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\rodcone.obj rodcone.cpp

$(OBJ_DIR)\viewer.obj:  viewer.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\viewer.obj viewer.cpp

$(OBJ_DIR)\$(proj).res: viewer.rc resource.h
   $(rc) $(rcflags) $(OBJ_DIR)\$(proj).res viewer.rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(OBJ_DIR)\$(proj).res
    $(link) $(linkdebug) $(linkflags) $(OBJ_LIST) $(borobjs) $(commas) $(outexe)$(OBJ_DIR)\$(proj).exe $(commas) $(commas) $(deflibs) winmm.lib d3drm.lib ddraw.lib dsound.lib $(commas) $(commas) $(OBJ_DIR)\$(proj).res

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
