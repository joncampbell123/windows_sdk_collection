Proj = EchoSrv

!include <Win32.Mak>

All: EchoSrv.Exe

EchoSrv.obj: EchoSrv.cpp
  $(cc) $(cdebug) $(cflags) $(cvars) EchoSrv.cpp

Threads.obj: Threads.cpp
  $(cc) $(cdebug) $(cflags) $(cvars) Threads.cpp

Socket.obj:  Socket.cpp
  $(cc) $(cdebug) $(cflags) $(cvars)  Socket.cpp

EchoSrv.Exe:  EchoSrv.obj Threads.obj Socket.obj
  $(link) $(ldebug) $(conflags) -out:$*.exe  \
   EchoSrv.obj Threads.obj Socket.obj user32.lib $(conlibs)

Clean:
    del *.obj

Cleanall:
    del  *.obj
    del *.exp
    del *.obj
    del *.dll
    del *.exe
