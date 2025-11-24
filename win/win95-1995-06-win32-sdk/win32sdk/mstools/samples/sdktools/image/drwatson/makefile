!include <ntwin32.mak>

all: ..\bin\drwtsn32.exe

messages.h messages.rc msg00001.bin: messages.mc
    mc -v messages.mc

cflags = $(cflags) -DWIN32 -DTARGET32 -DHOST32 -DADDR_MIXED -DNT_BUILD -D_CROSS_PLATFORM_

drwatson.res: messages.rc msg00001.bin drwatson.rc
    rc -r -i .\include -fo drwatson.res drwatson.rc 

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $(scall) -I..\include -I.\include -I. -D_PORTABLE_32BIT_CONTEXT $*.c


!if "$(CPU)" == "ALPHA"
..\bin\drwtsn32.exe: drwatson.res   \
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
         browse.obj    \
         dump.obj
!else
..\bin\drwtsn32.exe: drwatson.res   \
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
	     browse.obj \
         dump.obj
!endif
  $(link) $(linkdebug) $(conflags) -machine:$(CPU) -out:$@ \
  $** $(guilibs) ..\lib\imagehlp.lib winmm.lib shell32.lib \
  comdlg32.lib advapi32.lib crashlib.lib $(int64lib)
