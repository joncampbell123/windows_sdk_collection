#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                Object Hello Application                 **#
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

all:  ohello.dll client.exe server.exe

# Clean up everything
cleanall: clean
        @-del *.exe

# Clean up everything but the .EXEs
clean:
        @-del *.obj
        @-del ohello.dll
        @-del ohello.h
        @-del ohello.lib
        @-del ohello.exp
        @-del dlldata.c
        @-del ohello_?.c

#link the server application
server.exe:     server.obj ohello_i.obj
        $(link) $(linkdebug) $(conlflags) -OUT:server.exe \
        server.obj ohello_i.obj rpcrt4.lib $(ole2libs)

#link the client application
client.exe:     client.obj ohello_i.obj
        $(link) $(linkdebug) $(conlflags) -OUT:client.exe \
        client.obj ohello_i.obj rpcrt4.lib $(ole2libs)

#the files that make up the dll
ohello_i.obj : ohello_i.c

ohello_p.obj : ohello_p.c ohello.h

# this provides the DLL entry points
dlldata.obj : dlldata.c

DLLOBJS= ohello_p.obj ohello_i.obj dlldata.obj


# build proxy dll
ohello.dll: $(DLLOBJS) ohello.def
        $(link) $(linkdebug) -DLL -OUT:ohello.dll -DEF:ohello.def \
        $(DLLOBJS) rpcrt4.lib $(ole2libs)

# run midl to produce the header files and the proxy file
ohello.h dlldata.c ohello_p.c ohello_i.c : ohello.idl
                $(MIDL) $(MIDLFLAGS) -Oi ohello.idl

