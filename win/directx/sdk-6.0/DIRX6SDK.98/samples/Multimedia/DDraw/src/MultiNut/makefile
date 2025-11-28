proj=MultiNut

INCLUDE=$(INCLUDE);$(DXSDKROOT)\samples\multimedia\ddraw\include;

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

!include "$(DXSDKROOT)\include\dxsdk.inc"

mycflags=

OBJ_LIST = $(OBJ_DIR)\main.obj $(OBJ_DIR)\monitor.obj $(OBJ_DIR)\util.obj $(OBJ_DIR)\donuts.obj $(OBJ_DIR)\input.obj $(OBJ_DIR)\ddutil.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\main.obj:  main.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\main.obj main.cpp

$(OBJ_DIR)\ddutil.obj:  ddutil.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\ddutil.obj ddutil.cpp

$(OBJ_DIR)\monitor.obj:  monitor.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\monitor.obj monitor.cpp

$(OBJ_DIR)\util.obj:  util.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\util.obj util.cpp

$(OBJ_DIR)\donuts.obj:  donuts.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\donuts.obj donuts.cpp

$(OBJ_DIR)\input.obj:  input.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\input.obj input.cpp

$(OBJ_DIR)\$(proj).res: $(proj).rc resource.h
   $(rc) $(rcflags) $(OBJ_DIR)\$(proj).res $(proj).rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(OBJ_DIR)\$(proj).res
    $(link) $(linkdebug) $(linkflags) $(OBJ_LIST) $(borobjs) $(commas) $(outexe)$(OBJ_DIR)\$(proj).exe $(commas) $(commas) $(deflibs) ddraw.lib dinput.lib $(commas) $(commas) $(OBJ_DIR)\$(proj).res

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
