
#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                 Dictionary Application                  **#
#**            Copyright(c) Microsoft Corp. 1991            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

!if "$(CPU)" == "i386"
cflags = $(cflags:G3=Gz)
!endif

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : client.exe server.exe play.exe

# Make the client
client : client.exe
client.exe : client.obj util0.obj replay_c.obj replay_x.obj
    $(link) $(linkdebug) $(conflags) -out:client.exe -map:client.map \
      client.obj util0.obj replay_c.obj replay_x.obj \
      rpcrt4.lib $(conlibs)

# dict client main program
client.obj : client.c util0.h replay.h

# dict client stub
replay_x.obj : replay_x.c replay.h

# dict client auxiliary file
replay_c.obj : replay_c.c replay.h

# Make the server executable
server : server.exe
server.exe : server.obj util0.obj replay_s.obj replay_y.obj replay.obj dict0.obj
    $(link) $(linkdebug) $(conflags) -out:server.exe -map:server.map \
      server.obj util0.obj replay_s.obj replay_y.obj replay.obj dict0.obj \
      rpcrt4.lib $(conlibs)

# dict server main program
server.obj : server.c util0.h replay.h

# dict remote procedures
replay.obj : replay.c util0.h replay.h

# dict server stub
replay_y.obj : replay_y.c replay.h

# dict server auxiliary file
replay_s.obj : replay_s.c replay.h

# make the local executable
play : play.exe
play.exe : play.obj lutil0.obj dict0.obj
    $(link) $(linkdebug) $(conflags) -out:play.exe -map:play.map \
      play.obj lutil0.obj dict0.obj \
      $(conlibs)

# dict local main program
play.obj : play.c util0.h dict0.h

dict0.obj : dict0.c dict0.h

util0.obj : util0.c util0.h

lutil0.obj : util0.c util0.h
   copy util0.c lutil0.c
   $(cc) $(cdebug) $(cflags) $(cvars) -D_LOCAL lutil0.c
   del lutil0.c

# Stubs, auxiliary and header file from the IDL file
replay.h replay_s.c replay_c.c replay_y.c replay_x.c : replay.idl replay.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" replay.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del replay_?.c
    -del replay.h

