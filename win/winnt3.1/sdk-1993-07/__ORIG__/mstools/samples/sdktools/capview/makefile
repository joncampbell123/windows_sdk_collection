!include "..\..\..\mfc\samples\ntsample.mak"
ARRAY_CTT = ..\..\..\mfc\samples\templdef\array.ctt

!ifdef PROFILE
CPROF = -Gh
LINKDEBUG = -debug:full -debugtype:both
!else
CPROF =
!endif

.cpp.obj: ; $(CPP) $(CPPFLAGS) $(CPROF) -I ..\image\include /c $*.cpp

OBJS = capview.obj tree.obj ranges.obj timer.obj dispobj.obj listwin.obj list.obj capview.res array_x.obj

all: capview.exe capview.hlp

capview.exe:  $(OBJS)
	$(LINK) $(GUIFLAGS) -out:capview.exe $(OBJS) cap.lib shell32.lib ..\image\lib\imagehlp.lib $(MFCLIB) $(GUILIBS)

capview.res: resource.h capview.dlg capview.rc

capview.obj: capview.cpp capview.h resource.h dispobj.h tree.h timer.h \
	ranges.h listwin.h list.h types.h

tree.obj: tree.cpp tree.h resource.h dispobj.h timer.h ranges.h types.h

ranges.obj: ranges.cpp ranges.h types.h

timer.obj: timer.cpp timer.h dispobj.h types.h

dispobj.obj: dispobj.cpp dispobj.h types.h

listwin.obj: listwin.cpp listwin.h list.h types.h

list.obj: list.cpp list.h types.h

capview.hlp: capview.hpj capview.rtf
	hc -n capview.hpj

array_x.obj: array_x.cpp array_x.h

#
# The following is an example of MFC templdef usage.
# This step has already been performed on the version of
# array_x.cpp which ships with the Win32 SDK.
#
#array_x.cpp: $(ARRAY_CTT)
#	templdef "CArray<AAA, AAA, 0, 0> CAaaArray" $(ARRAY_CTT) array_x.h array_x.cpp

dispobj.h: array_x.h
