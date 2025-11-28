proj=d3dframe

INCLUDE=$(INCLUDE);$(DXSDKROOT)\samples\multimedia\d3dim\include;

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

!include "$(DXSDKROOT)\include\dxsdk.inc"

mycflags=

OBJ_LIST = $(OBJ_DIR)\$(proj).obj $(OBJ_DIR)\d3denum.obj $(OBJ_DIR)\d3dmath.obj $(OBJ_DIR)\d3dtextr.obj $(OBJ_DIR)\d3dutil.obj $(OBJ_DIR)\userdlg.obj

all: mkdir $(OBJ_DIR)\$(proj).lib

$(OBJ_DIR)\$(proj).obj:  $(proj).cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\$(proj).obj $(proj).cpp

$(OBJ_DIR)\d3denum.obj:  d3denum.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\d3denum.obj d3denum.cpp

$(OBJ_DIR)\d3dmath.obj:  d3dmath.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\d3dmath.obj d3dmath.cpp

$(OBJ_DIR)\d3dtextr.obj:  d3dtextr.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\d3dtextr.obj d3dtextr.cpp

$(OBJ_DIR)\d3dutil.obj:  d3dutil.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\d3dutil.obj d3dutil.cpp

$(OBJ_DIR)\userdlg.obj:  userdlg.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\userdlg.obj userdlg.cpp

#$(OBJ_DIR)\winmain.res: winmain.rc resource.h
#   $(rc) $(rcflags) $(OBJ_DIR)\winmain.res winmain.rc

$(OBJ_DIR)\$(proj).lib: $(OBJ_LIST)
    $(bldlib) $(libflags) $(borlib)$(OBJ_DIR)\$(proj).obj $(borlib)$(OBJ_DIR)\d3denum.obj $(borlib)$(OBJ_DIR)\d3dmath.obj $(borlib)$(OBJ_DIR)\d3dtextr.obj $(borlib)$(OBJ_DIR)\d3dutil.obj $(borlib)$(OBJ_DIR)\userdlg.obj

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
