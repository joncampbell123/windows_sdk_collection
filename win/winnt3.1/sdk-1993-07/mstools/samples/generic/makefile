!IF "$(CPU)" != ""
OS=NT
ENV=WIN32
!ELSE
OS=DOS
ENV=WIN16
!ENDIF

# If this generates the error "Unable to open DOSWIN16.MAK" then you need
# to put a copy of that file (from the GENERIC sample code directory)
# into a directory on your INCLUDE environment variable

!include <$(OS)$(ENV).MAK>

proj = GENERIC

all: $(proj).exe

# Update the help file if necessary
$(proj).hlp : $(proj).rtf
    $(hc) -n $(proj).hpj

# Update the resource if necessary
$(proj).res: $(proj).rc $(proj).rcv $(proj).dlg $(proj).h
    $(rc) $(rcvars) -r -fo $(proj).res $(cvars) $(proj).rc
!IFDEF CPUTYPE
    cvtres -$(CPU) $(proj).res -o $(proj).rbj
!ENDIF


# Update the object file if necessary
$(proj).obj: $(proj).c $(proj).h
    $(cc) $(cflags) $(cvars) $(cdebug) $(proj).c

# Since the link line has some severe differences depending on what
# platform we are running on, we need to special case this so that
# we execute the correct commands:

$(proj).exe: $(proj).obj $(proj).res $(proj).def $(proj).hlp
!IFDEF CPUTYPE
# This is for Windows NT:
    $(link) $(linkdebug) $(guiflags) $(proj).obj  $(guilibs) VERSION.LIB $(proj).rbj -out:$(proj).exe
!ENDIF
!IFNDEF CPUTYPE
# This is for Windows DOS:
    $(link) $(guiflags) $(proj).obj $(guilibs) $(proj).DEF
    rc $(proj).res
!ENDIF
