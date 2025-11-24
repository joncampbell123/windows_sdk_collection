#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   usrdef Application                    **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : usrdefc usrdefs

# Make the usrdefc
usrdefc : usrdefc.exe
usrdefc.exe : usrdefc.obj usrdef_c.obj usrdef_x.obj
    $(link) $(linkdebug) $(conflags) -out:usrdefc.exe \
      usrdefc.obj usrdef_c.obj usrdef_x.obj \
      rpcrt4.lib $(conlibs)

# usrdefc main program
usrdefc.obj : usrdefc.c usrdef.h

# usrdefc stub
usrdef_c.obj : usrdef_c.c usrdef.h

# usrdefc auxiliary file
usrdef_x.obj : usrdef_x.c usrdef.h

# Make the usrdefs
usrdefs : usrdefs.exe
usrdefs.exe : usrdefs.obj usrdefp.obj usrdef_s.obj usrdef_y.obj
    $(link) $(linkdebug) $(conflags) -out:usrdefs.exe \
      usrdefs.obj usrdef_s.obj usrdefp.obj usrdef_y.obj \
      rpcrt4.lib $(conlibs)

# usrdefs main loop
usrdefs.obj : usrdefs.c usrdef.h

# remote procedures
usrdefp.obj : usrdefp.c usrdef.h

# usrdefs stub file
usrdef_s.obj : usrdef_s.c usrdef.h

# usrdefs auxiliary file
usrdef_y.obj : usrdef_y.c usrdef.h

# Stubs, auxiliary and header file from the IDL file
usrdef.h usrdef_c.c usrdef_x.c usrdef_s.c usrdef_y.c : usrdef.idl usrdef.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" usrdef.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del usrdef_c.c
    -del usrdef_x.c
    -del usrdef_s.c
    -del usrdef_y.c
    -del usrdef.h
