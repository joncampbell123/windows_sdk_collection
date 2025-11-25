Proj = Echo

!include <Win32.Mak>

All: Echo.Dll
!IF EXIST (EchoSrv)
 cd EchoSrv
 @$(MAKE) -nologo  -i
 cd ..
!ENDIF # exist (EchoSrv)
 copy EchoSrv\EchoSrv.Exe

Parmsblk.obj: Parmsblk.cpp
  $(cc) $(cdebug) $(cflags) $(cvars) Parmsblk.cpp

ConnMgmt.obj: ConnMgmt.cpp
  $(cc) $(cdebug) $(cflags) $(cvars) ConnMgmt.cpp

Echo.Obj: Echo.cpp
  $(cc) $(cdebug) $(cflags) $(cvarsdll) echo.cpp

Echo.Exp: Echo.Def Echo.Obj Parmsblk.Obj ConnMgmt.Obj
  $(implib) -machine:$(CPU) -nologo -Def:Echo.Def -Out:Echo.Lib \
    Echo.Obj Parmsblk.Obj ConnMgmt.Obj

Echo.Dll: Echo.Obj Parmsblk.Obj ConnMgmt.Obj Echo.Exp
     $(link) $(linkdebug) $(dlllflags) -base:0x1C000000 -out:echo.dll  \
      Echo.Obj Parmsblk.Obj ConnMgmt.Obj Echo.Exp  $(guilibsdll)

Clean:
    del *.obj

Cleanall:
    del  *.obj
    del *.exp
    del *.obj
    del *.dll
