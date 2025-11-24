# *********************************************************
#  makefile: for WORLD sample
#
#
# *********************************************************


# Nmake macros for building Windows 32-Bit applications

!include <ntwin32.mak>

proj = WORLD

all: $(proj).exe

# Update the resource if necessary

$(proj).rbj: $(proj).rc $(proj).h $(proj).dlg
    rc -r -fo $(proj).res $(cvars) $(proj).rc
    cvtres -$(CPU) $(proj).res -o $(proj).rbj

# Update the object file if necessary

$(proj).obj: $(proj).c
    $(cc) $(cdebug) $(cflags) $(cvars) $(proj).c

# Update the executable file if necessary, and if so, add the resource 
# back in

$(proj).exe: $(proj).obj $(proj).rbj $(proj).def
    $(link) $(linkdebug) $(guiflags) $(proj).obj $(proj).rbj \
       $(guilibs) -out:$(proj).exe
