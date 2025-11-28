proj=Stretch3

INCLUDE=$(INCLUDE);$(DXSDKROOT)\samples\multimedia\ddraw\include;

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

!include "$(DXSDKROOT)\include\dxsdk.inc"

mycflags=

OBJ_LIST = $(OBJ_DIR)\stretch.obj $(OBJ_DIR)\ddutil2.obj $(OBJ_DIR)\ddmm.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\stretch.obj:  stretch.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\stretch.obj stretch.cpp

$(OBJ_DIR)\ddutil2.obj:  ddutil2.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\ddutil2.obj ddutil2.cpp

$(OBJ_DIR)\ddmm.obj:  ddmm.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\ddmm.obj ddmm.cpp

$(OBJ_DIR)\stretch.res: stretch.rc resource.h
   $(rc) $(rcflags) $(OBJ_DIR)\stretch.res stretch.rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(OBJ_DIR)\stretch.res
    $(link) $(linkdebug) $(linkflags) $(OBJ_LIST) $(borobjs) $(commas) $(outexe)$(OBJ_DIR)\$(proj).exe $(commas) $(commas) $(deflibs) ddraw.lib $(commas) $(commas) $(OBJ_DIR)\stretch.res

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
