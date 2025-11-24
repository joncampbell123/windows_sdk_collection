##########################################################################
#
# Makefile for BOOKUI32.DLL
#
# Usage:    NMAKE           (builds DEBUG library)
#    or:    NMAKE RELEASE=1 (builds RELEASE library -- no debug symbols)
#
# Environment variables:
#       DEVROOT_DIR=<path>  (root dir for sample code development)
#
##########################################################################


!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

!ifndef LANG
LANG=USA
!endif

INCLUDE=$(INCLUDE);$(SDKROOT)\samples\ole\inole2\inc

#
#If you change the library name, change it also in OLE2UI.DEF
#
!ifndef LIBRARY
LIBRARY=BOOKUI32
!endif

RESOURCE=res

all: $(LIBRARY).dll

# ----------------------------------------------------------------------------
#                      O B J E C T   F I L E   L I S T
# ----------------------------------------------------------------------------

UI_COBJS = ..\..\ole2ui\busy.obj     \
           ..\..\ole2ui\common.obj   \
           ..\..\ole2ui\convert.obj  \
           ..\..\ole2ui\dballoc.obj  \
           ..\..\ole2ui\dbgutil.obj  \
           ..\..\ole2ui\dllfuncs.obj \
           ..\..\ole2ui\drawicon.obj \
           ..\..\ole2ui\geticon.obj  \
           ..\..\ole2ui\hatch.obj    \
           ..\..\ole2ui\icon.obj     \
           ..\..\ole2ui\iconbox.obj  \
           ..\..\ole2ui\insobj.obj   \
           ..\..\ole2ui\links.obj    \
           ..\..\ole2ui\msgfiltr.obj \
           ..\..\ole2ui\enumfetc.obj \
           ..\..\ole2ui\enumstat.obj \
           ..\..\ole2ui\objfdbk.obj  \
           ..\..\ole2ui\ole2ui.obj   \
           ..\..\ole2ui\olestd.obj   \
           ..\..\ole2ui\targtdev.obj \
           ..\..\ole2ui\oleutl.obj   \
           ..\..\ole2ui\pastespl.obj \
           ..\..\ole2ui\regdb.obj    \
           ..\..\ole2ui\resimage.obj \
           ..\..\ole2ui\stdpal.obj   \
           ..\..\ole2ui\suminfo.obj  \
           ..\..\ole2ui\utility.obj


#
# Build .LIB static library
#
#Build a linker response file depending on build flags
$(LIBRARY).dll: $(LIBRARY).lib $(OBJS) bookui32.def
    echo ++++++++++
    echo Linking $@
    $(link) @<<
        $(ldebug) $(dlllflags)
        /Out:$(LIBRARY).dll
        $(UI_COBJS)
        ..\..\ole2ui\ole2ui.res
        $(PRECOMPOBJ)
        $(olelibsdll)
        advapi32.lib shell32.lib
        $(LIBRARY).exp
<<

$(LIBRARY).lib: bookui32.def $(UI_COBJS) ..\..\ole2ui\ole2ui.res
    $(implib)  -out:$*.lib -def:bookui32.def -machine:$(CPU) $(UI_COBJS)
