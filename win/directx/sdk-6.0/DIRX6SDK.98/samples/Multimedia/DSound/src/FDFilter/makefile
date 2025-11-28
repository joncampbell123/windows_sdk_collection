proj=FDFilter

INCLUDE=$(INCLUDE);wstream\;

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

!include "$(DXSDKROOT)\include\dxsdk.inc"

mycflags=

OBJ_LIST = $(OBJ_DIR)\$(proj).obj $(OBJ_DIR)\debug.obj $(OBJ_DIR)\fdaudio.obj $(OBJ_DIR)\filter.obj \
	$(OBJ_DIR)\gargle.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\$(proj).obj:  $(proj).cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\$(proj).obj $(proj).cpp

$(OBJ_DIR)\debug.obj:  debug.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\debug.obj debug.c

$(OBJ_DIR)\fdaudio.obj:  fdaudio.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\fdaudio.obj fdaudio.cpp

$(OBJ_DIR)\filter.obj:  filter.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\filter.obj filter.cpp

$(OBJ_DIR)\gargle.obj:  gargle.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\gargle.obj gargle.cpp

$(OBJ_DIR)\$(proj).res: $(proj).rc resource.h
   $(rc) $(rcflags) $(OBJ_DIR)\$(proj).res $(proj).rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(OBJ_DIR)\$(proj).res
    $(link) $(linkdebug) $(linkflags) $(OBJ_LIST) $(borobjs) $(commas) $(outexe)$(OBJ_DIR)\$(proj).exe $(commas) $(commas) $(deflibs) dsound.lib uuid.lib $(commas) $(commas) $(OBJ_DIR)\$(proj).res

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
