#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**		     Callback Application		    **#
#**	       Copyright(c) Microsoft Corp. 1992	    **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : callc calls

# Make the client side application callc
callc : callc.exe
callc.exe : callc.obj call_c.obj call_x.obj
    $(link) $(linkdebug) $(conflags) -out:callc.exe \
      callc.obj call_c.obj call_x.obj \
      rpcrt4.lib $(conlibs)

# callc main program
callc.obj : callc.c call.h

# callc stub
call_c.obj : call_c.c call.h

# callc auxiliary file
call_x.obj : call_x.c call.h

# Make the server side application
calls : calls.exe
calls.exe : calls.obj callp.obj call_s.obj call_y.obj
    $(link) $(linkdebug) $(conflags) -out:calls.exe \
      calls.obj call_s.obj callp.obj call_y.obj \
      rpcrt4.lib $(conlibs)

# call server main program
calls.obj : calls.c call.h

# remote procedures
callp.obj : callp.c call.h

# calls stub file
call_s.obj : call_s.c call.h

# calls auxiliary file
call_y.obj : call_y.c call.h

# Stubs, auxiliary and header file from the IDL file
call.h call_c.c call_x.c call_s.c call_y.c : call.idl call.acf
    midl -ms_ext -cpp_cmd $(cc) -cpp_opt "-E" call.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del call_c.c
    -del call_x.c
    -del call_s.c
    -del call_y.c
    -del call.h
