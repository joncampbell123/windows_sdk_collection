#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   doctor Application                    **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $(DEBUG) $<

all : doctorc doctors

# Make the doctor client executable
doctorc : doctorc.exe
doctorc.exe : doctorc.obj doctor_c.obj doctor_x.obj
    $(link) $(linkdebug) $(conflags) -out:doctorc.exe \
      doctorc.obj doctor_c.obj doctor_x.obj \
      rpcrt4.lib $(conlibs)

# client main program
doctorc.obj : doctorc.c doctor.h

# client stub
doctor_c.obj : doctor_c.c doctor.h

# client auxiliary file
doctor_x.obj : doctor_x.c doctor.h

# Make the doctor server executable
doctors : doctors.exe
doctors.exe : doctors.obj doctorp.obj doctor_s.obj doctor_y.obj
    $(link) $(linkdebug) $(conflags) -out:doctors.exe \
      doctors.obj doctor_s.obj doctorp.obj doctor_y.obj \
      rpcrt4.lib $(conlibs)

# server main loop
doctors.obj : doctors.c doctor.h

# remote procedures
doctorp.obj  : doctorp.c doctor.h

# server stub file
doctor_s.obj : doctor_s.c doctor.h

# server auxiliary file
doctor_y.obj : doctor_y.c doctor.h

# Stubs, auxiliary and header file from the IDL file
doctor.h doctor_c.c doctor_x.c doctor_s.c doctor_y.c : doctor.idl doctor.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" -ms_ext doctor.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del doctor_c.c
    -del doctor_x.c
    -del doctor_s.c
    -del doctor_y.c
    -del doctor.h
