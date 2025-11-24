# *********************************************************
#  makefile: for UTSamp sample
#
#
# *********************************************************

!include <ntwin32.mak>

proj = utsamp

all: $(proj)32.dll $(proj).exe

#
# utsamp16.dll must be built using makefile.16 with a 16 bit compiler
#

# Update the resource if necessary

$(proj).rbj: $*.rc $*.h $*.dlg $*.ico
    rc -r -fo $*.res $(cvars) $*.rc
    cvtres -$(CPU) $*.res -o $@

# Update the object file if necessary

$(proj).obj: $*.c
    $(cc) $(cdebug) $(cflags) $(cvars) $*.c

# Update the executable file if necessary, and if so, add the resource
# back in

$(proj).exe: $*.obj $*.rbj $(proj)32.lib
    $(link) $(linkdebug) $(guiflags) $*.obj $*.rbj $(guilibs) $(proj)32.lib -out:$@

$(proj)32.obj: $*.c $(proj).h
    $(cc) $(cflags) $(cvarsdll) $(cdebug) $*.c

$(proj)32.lib $(proj)32.exp: $*.obj $*.def
    $(implib) -machine:$(CPU) -def:$*.def $*.obj -out:$*.lib

$(proj)32.dll: $*.obj $*.def $*.exp
    $(link) $(conflags) $(ldebug) -dll -entry:DllInit$(DLLENTRY) -base:0x20000000 -out:$@ $*.exp $*.obj $(guilibsdll) w32sut32.lib mpr.lib
