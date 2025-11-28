proj=ConvMIDI

# Define main so that dxsdk.inc knows we want a console app
main=1

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

!include "$(DXSDKROOT)\include\dxsdk.inc"

mycflags=

OBJ_LIST = $(OBJ_DIR)\mid2strm.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\mid2strm.obj:  mid2strm.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\mid2strm.obj mid2strm.c

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST)
    $(link) $(linkdebug) $(linkflags) $(OBJ_LIST) $(borobjs) $(commas) $(outexe)$(OBJ_DIR)\$(proj).exe $(commas) $(commas) $(deflibs) ddraw.lib dsound.lib $(commas) $(commas)

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
