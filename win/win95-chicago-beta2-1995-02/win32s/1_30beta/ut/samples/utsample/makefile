# *********************************************************
#  makefile: for UTSamp sample
#
#
# *********************************************************

!include <ntwin32.mak>

proj = utsamp

all: $(proj)32.dll $(proj).exe

#
# utsamp16.dll must be built using makefile.16 16-bit tools
#

# Update the resource if necessary

$(proj).rbj: $*.rc $*.h $*.dlg $*.ico
    rc -r -fo $*.res $(cvars) $*.rc

# Update the object file if necessary

$(proj).obj: $*.c
    $(cc) $(cdebug) $(cflags) $(cvars) $*.c

# Update the executable file if necessary, and if so, add the resource
# back in

$(proj).exe: $*.obj $*.res $(proj)32.lib
    $(link) $(linkdebug) $(guiflags) \
    $*.obj $*.res \
    $(guilibs) $(proj)32.lib \
    -out:$@

$(proj)32.obj: $*.c $(proj).h
    $(cc) $(cflags) $(cvars) $(cdebug) $*.c

$(proj)32.lib $(proj)32.exp: $*.obj $*.def
    $(implib) -machine:$(CPU) -def:$*.def $*.obj -out:$*.lib

$(proj)32.dll: $*.obj $*.def $*.exp
    $(link) $(dlllflags) $(ldebug) \
    -base:0x20000000 \
    -out:$@ \
    $*.exp $*.obj \
    $(guilibs) w32sut32.lib mpr.lib
