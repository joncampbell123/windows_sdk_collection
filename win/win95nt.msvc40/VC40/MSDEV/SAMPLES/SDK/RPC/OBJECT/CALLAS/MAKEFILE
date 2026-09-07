#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   callas Application                    **#
#**            Copyright(c) Microsoft Corp. 1992-1995       **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

!ifndef MIDL
MIDL            = midl
!endif

MIDLFLAGS    = /ms_ext /c_ext /cpp_cmd $(CC)


.cxx.obj:
    $(CC) $(cdebug) $(CFLAGS) $(cvars) /c $<

.c.obj:
    $(CC) $(cdebug) $(CFLAGS) $(cvars) /c $<

all:  callas.dll client.exe server.exe

# Clean up everything
cleanall: clean
        @-del *.exe

# Clean up everything but the .EXEs
clean:
        @-del *.obj
        @-del callas.dll
        @-del callas.h
        @-del callas.lib
        @-del callas.exp
        @-del dlldata.c
        @-del callas_?.*

#link the server application
server.exe:     server.obj callas_i.obj
        $(link) $(linkdebug) $(conlflags) -OUT:server.exe \
        server.obj callas_i.obj rpcrt4.lib $(ole2libs)

#link the client application
client.exe:     client.obj callas_i.obj
        $(link) $(linkdebug) $(conlflags) -OUT:client.exe \
        client.obj callas_i.obj rpcrt4.lib $(ole2libs)

#the files that make up the dll
callas_i.obj : callas_i.c

callas_p.obj : callas_p.c callas.h

# this provides the DLL entry points
dlldata.obj : dlldata.c

call_as.obj : call_as.c callas.h

DLLOBJS= call_as.obj callas_p.obj callas_i.obj dlldata.obj


# build proxy dll
callas.dll: $(DLLOBJS) callas.def
        $(link) $(linkdebug) -DLL -OUT:callas.dll -DEF:callas.def \
        $(DLLOBJS) rpcrt4.lib $(ole2libs)

# run midl to produce the header files and the proxy file
callas.h dlldata.c callas_p.c callas_i.c : callas.idl callas.acf
                $(MIDL) $(MIDLFLAGS) -Oi callas.idl

