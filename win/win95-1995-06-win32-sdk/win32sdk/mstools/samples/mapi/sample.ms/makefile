#Makefile to build the MAPI Sample Message Store Provider

!include <win32.mak>

PROJ     = SMPMS32
BASEFILE = ..\common\mapibase.txt

# Main target dependencies

all: $(PROJ).dll

# Extra libraries needed, not defined in win32.mak

extralibs = mapi32.lib ole32.lib ..\common\ctl3d32.lib

# Object files we need to build

OBJS = msp.obj mspatch.obj mspfld.obj mspgle.obj mspguid.obj mspinit.obj \
       msplogon.obj mspmisc.obj mspms.obj mspmsg.obj mspntfy.obj mspobj.obj \
       msprfs.obj msptbl.obj

COMMON_OBJS = imem.obj

# Build the general object files

.c.obj:
  $(cc) /I..\common $(cdebug) $(cflags) $(cvars) $*.c

# Build the specific common object file

imem.obj: ..\common\imem.c
  $(cc) /I..\common $(cdebug) $(cflags) $(cvars) ..\common\imem.c

# Build the resources

$(PROJ).res: MSP.RC MSP.DLG
  $(rc) /I..\common $(rcflags) $(rcvars) /fo $(PROJ).res MSP.RC

# Link it together to make the executable image

$(PROJ).dll: $(OBJS) $(PROJ).res $(COMMON_OBJS)
  $(link) $(linkdebug) $(lflags) -dll -subsystem:windows,$(APPVER) $(OBJS) \
  $(COMMON_OBJS) $(PROJ).res $(guilibsdll) $(extralibs) -def:$(PROJ).def -out:$(PROJ).dll \
  -base:@$(BASEFILE),$(PROJ)

# Clean up the intermediate files

clean:
  del *.obj *.res

