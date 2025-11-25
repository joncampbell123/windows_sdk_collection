!include "..\win32.mak"

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

proj=ds3dview

OBJ_LIST = $(OBJ_DIR)\viewer.obj $(OBJ_DIR)\dsutil3d.obj $(OBJ_DIR)\file.obj \
           $(OBJ_DIR)\rodcone.obj $(OBJ_DIR)\rlds3d.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\viewer.obj:  viewer.cpp ds3dvi.h resource.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\viewer.obj $(cvars) viewer.cpp

$(OBJ_DIR)\file.obj:  viewer.cpp file.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\file.obj $(cvars) file.cpp
  
$(OBJ_DIR)\rodcone.obj:  rodcone.cpp rodcone.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\rodcone.obj $(cvars) rodcone.cpp
  
$(OBJ_DIR)\rlds3d.obj:  rlds3d.cpp rlds3d.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\rlds3d.obj $(cvars) rlds3d.cpp
  
$(OBJ_DIR)\dsutil3d.obj:  dsutil3d.c dsutil3d.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\dsutil3d.obj $(cvars) dsutil3d.c
      
$(OBJ_DIR)\viewer.res: viewer.rc resource.h
   rc -r -fo $(OBJ_DIR)\viewer.res viewer.rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(proj).def $(OBJ_DIR)\viewer.res
    $(link) $(linkdebug) $(guiflags) -out:$(OBJ_DIR)\$(proj).exe \
           $(OBJ_LIST) $(OBJ_DIR)\viewer.res \
           $(guilibs) ddraw.lib dsound.lib winmm.lib d3drm.lib

mkdir:
    if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -echo y | del Retail
    -echo y | del Debug
    -rd Retail
    -rd Debug
