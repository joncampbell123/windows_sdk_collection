!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

INCLUDE=.;$(INCLUDE)


!IF "$(CPU)" == "PPC"
all:  WARN_MSG
!ELSE
all: oleapt.exe
!ENDIF

WARN_MSG:
   @echo "Warning:  This ppc compiler does not handle C++ catch/throw,"
   @echo "Warning:   thus we can not build the oleapt sample for ppc."

oleapt.exe: oleapt.obj
  $(link) $(linkdebug) $(guiflags) -machine:$(CPU) -out:$*.exe $** $(olelibs)

.cpp.obj:
    $(cc) -GX $(cdebug) -W3 -c -D_$(CPU)_=1 $(cvars) $*.cpp
