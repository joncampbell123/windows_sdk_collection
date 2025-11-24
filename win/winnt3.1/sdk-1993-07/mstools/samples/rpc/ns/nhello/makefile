#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   nhello Application                    **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : nhelloc nhellos

# Make the nhelloc
nhelloc : nhelloc.exe
nhelloc.exe : nhelloc.obj nhello_c.obj nhello_x.obj
    $(link) $(linkdebug) $(conflags) -out:nhelloc.exe \
      nhelloc.obj nhello_c.obj nhello_x.obj \
      rpcrt4.lib rpcns4.lib $(conlibs)

# nhelloc main program
nhelloc.obj : nhelloc.c nhello.h

# nhelloc stub
nhello_c.obj : nhello_c.c nhello.h

# nhelloc auxiliary file
nhello_x.obj : nhello_x.c nhello.h

# Make the nhellos
nhellos : nhellos.exe
nhellos.exe : nhellos.obj nhellop.obj nhello_s.obj nhello_y.obj
    $(link) $(linkdebug) $(conflags) -out:nhellos.exe \
      nhellos.obj nhello_s.obj nhellop.obj nhello_y.obj \
      rpcrt4.lib rpcns4.lib $(conlibs)

# nhellos main loop
nhellos.obj : nhellos.c nhello.h

# remote procedures
nhellop.obj  : nhellop.c nhello.h

# nhellos stub file
nhello_s.obj : nhello_s.c nhello.h

# nhellos auxiliary file
nhello_y.obj : nhello_y.c nhello.h

# Stubs, auxiliary and header file from the IDL file
nhello.h nhello_c.c nhello_x.c nhello_s.c nhello_y.c : nhello.idl
    midl -ms_ext -cpp_cmd $(cc) -cpp_opt "-E" nhello.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del nhello_c.c
    -del nhello_x.c
    -del nhello_s.c
    -del nhello_y.c
    -del nhello.h
