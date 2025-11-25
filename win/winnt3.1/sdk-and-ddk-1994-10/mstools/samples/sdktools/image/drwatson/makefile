!include <ntwin32.mak>

all: ..\bin\drwtsn32.exe

messages.h messages.rc msg00001.bin: messages.mc
    mc -v messages.mc

drwatson.rbj: messages.rc msg00001.bin drwatson.rc
    rc -r -i .\include -fo drwatson.res drwatson.rc 
    cvtres -$(CPU) drwatson.res -o drwatson.rbj

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $(scall) -I..\include -I.\include -D_PORTABLE_32BIT_CONTEXT $*.c


!if "$(CPU)" == "ALPHA"
..\bin\drwtsn32.exe: drwatson.rbj   \
         controls.obj   \
         debug.obj      \
         error.obj      \
         eventlog.obj   \
         log.obj        \
         module.obj     \
         notify.obj     \
	 optable.obj	\
         process.obj    \
         registry.obj   \
         symbols.obj    \
         ui.obj         \
         util.obj       \
         walk.obj      \
         regs.obj      \
         disasm.obj    \
         context.obj   \
         drwtsn32.obj   \
	 browse.obj
!else
..\bin\drwtsn32.exe: drwatson.rbj   \
         controls.obj   \
         debug.obj      \
         error.obj      \
         eventlog.obj   \
         log.obj        \
         module.obj     \
         notify.obj     \
         process.obj    \
         registry.obj   \
         symbols.obj    \
         ui.obj         \
         util.obj       \
         walk.obj      \
         regs.obj      \
         disasm.obj    \
         context.obj   \
         drwtsn32.obj   \
	 browse.obj
!endif
  $(link) $(linkdebug) $(conflags) -out:$@ $** $(guilibs) ..\lib\imagehlp.lib winmm.lib shell32.lib comdlg32.lib advapi32.lib 

