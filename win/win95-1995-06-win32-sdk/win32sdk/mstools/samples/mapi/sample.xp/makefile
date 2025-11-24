# Makefile to build the MAPI Sample Transport Provider

!include <win32.mak>

PROJ     = SMPXP32
BASEFILE = ..\common\mapibase.txt

# Main target dependencies

all: $(PROJ).dll

# Extra libraries needed, not defined in win32.mak

extralibs = mapi32.lib ole32.lib ..\common\ctl3d32.lib

# Object files we need to build

OBJS =  XPBASE.OBJ XPDIALOG.OBJ XPGUID.OBJ XPLOGGER.OBJ XPOPTION.OBJ XPPCH.OBJ \
	XPQUEUE.OBJ XPRCVMSG.OBJ XPSNDMSG.OBJ XPSOF.OBJ XPSTATUS.OBJ

# Build the object files

.c.obj:
  $(cc) /I..\common $(cdebug) $(cflags) $(cvars) $*.c

# Build the resources

$(PROJ).res: XPRESRC.RC XPRESRC.DLG
  $(rc) /I..\common $(rcflags) $(rcvars) /fo $(PROJ).res XPRESRC.RC

# Link it together to make the executable image

$(PROJ).dll: $(OBJS) $(PROJ).res $(COMMON_OBJS)
  $(link) $(linkdebug) $(lflags) -dll -subsystem:windows,$(APPVER) $(OBJS) \
  $(COMMON_OBJS) $(PROJ).res $(guilibsdll) $(extralibs) -def:$(PROJ).def -out:$(PROJ).dll \
  -base:@$(BASEFILE),$(PROJ)

# Clean up the intermediate files

clean:
  del *.obj *.res *.lib

