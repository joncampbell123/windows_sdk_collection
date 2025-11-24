##########################################################################
#
# Makefile for OUTLUI.DLL
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

#
#If you change the library name, change it also in OLE2UI.DEF
#
!ifndef LIBRARY
LIBRARY=ole2ui
!endif

RESOURCE=res

all: $(LIBRARY).dll

# ----------------------------------------------------------------------------
#                      O B J E C T   F I L E   L I S T
# ----------------------------------------------------------------------------

UI_COBJS = busy.obj     \
           common.obj   \
           convert.obj  \
           dballoc.obj  \
           dbgutil.obj  \
           dllfuncs.obj \
           drawicon.obj \
           geticon.obj  \
           hatch.obj    \
           icon.obj     \
           iconbox.obj  \
           insobj.obj   \
           links.obj    \
           msgfiltr.obj \
           enumfetc.obj \
           enumstat.obj \
           objfdbk.obj  \
           ole2ui.obj   \
           olestd.obj   \
           targtdev.obj \
           oleutl.obj   \
           pastespl.obj \
           regdb.obj    \
           resimage.obj \
           stdpal.obj   \
           suminfo.obj  \
           utility.obj

# ----------------------------------------------------------------------------
#                       R E S O U R C E   L I S T
# ----------------------------------------------------------------------------
RES =      \
           busy.h                           \
           common.h                         \
           convert.h                        \
           edlinks.h                        \
           geticon.h                        \
           icon.h                           \
           iconbox.h                        \
           insobj.h                         \
           enumfetc.h                       \
           pastespl.h                       \
           resimage.h                       \
           $(RESOURCE)\STATIC\default.ico   \
           $(RESOURCE)\STATIC\bang.ico      \
           $(RESOURCE)\STATIC\egares.bmp    \
           $(RESOURCE)\STATIC\hivgares.bmp  \
           $(RESOURCE)\STATIC\vgares.bmp    \
           $(RESOURCE)\$(LANG)\strings.rc   \
           $(RESOURCE)\$(LANG)\busy.dlg     \
           $(RESOURCE)\$(LANG)\convert.dlg  \
           $(RESOURCE)\$(LANG)\fileopen.dlg \
           $(RESOURCE)\$(LANG)\icon.dlg     \
           $(RESOURCE)\$(LANG)\insobj.dlg   \
           $(RESOURCE)\$(LANG)\links.dlg    \
           $(RESOURCE)\$(LANG)\pastespl.dlg \
           $(RESOURCE)\$(LANG)\prompt.dlg   \
           $(RESOURCE)\ole2ui.rcv   \
           $(RESOURCE)\$(LANG)\verlocal.h

# ----------------------------------------------------------------------------
#                 D E B U G   M A K E   P A R A M E T E R S
# ----------------------------------------------------------------------------

OLE_FLAGS = /DDLL_VER /D_DLL /DWIN32S /DWIN32

!ifndef NODEBUG
OLE_FLAGS = $(OLE_FLAGS) /DDBG /D_DEBUG /D_DEBUGTRACE=0 /DOLE2SHIP
!endif



LANG      = $(LANG)

#select language for version resource if localized version
!if ("$(LANG)"!="USA") && ("$(LANG)"!="usa")
RFLAGS=$(RFLAGS) -D VER_LOC
!endif

.SUFFIXES: .c .asm .obj .res .rc .def .bmp .ico .exe .dll .cod .str

# ----------------------------------------------------------------------------
#                     I N F E R E N C E   R U L E S
# ----------------------------------------------------------------------------
# compile C file without precompiled headers into object directory\NOPC
# dont compile c files etc for lcoalized builds.

.c.obj:
    @echo Compiling $(@B).c
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $(@B).c

.rc.res:
    @echo Resource Compiling $(@B).res
    $(rc) -I $(RESOURCE)\$(LANG);$(RESOURCE)\static;$(RESOURCE) -FO $(@B).res -R $(RFLAGS) $(@B).rc

.c.cod:
    @echo Making $(@B).cod
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) -f- -Fc $(@B).c

# ----------------------------------------------------------------------------
#                        G O A L   T A R G E T S
# ----------------------------------------------------------------------------
suminfo.obj: suminfo.cpp suminfo.h wn_dos.h
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $(@B).cpp

dballoc.obj: dballoc.cpp dballoc.h
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $(@B).cpp

ole2ui.res: ole2ui.rc $(RES)

ole2ui.rc : $(RESOURCE)\$(LANG)\strings.rc


#
# Build .LIB static library
#

$(LIBRARY).dll: ole2ui.def $(UI_COBJS) ole2ui.res
    $(implib)  -out:$*.lib -def:ole2ui.def -machine:$(CPU) $(UI_COBJS)
    $(link) $(linkdebug) $(dlllflags) $(LIBRARY).exp $(UI_COBJS) ole2ui.res -out:$*.dll -map:$*.map -dll $(olelibsdll) advapi32.lib shell32.lib
    if not exist ..\lib mkdir ..\lib
    if not exist ..\bin mkdir ..\bin
    copy $(LIBRARY).LIB ..\lib
    copy $(LIBRARY).dll ..\bin

clean:
    -del *.obj
    -del *.dll
    -del *.res
    -del *.lib
    -del *.exp
    -del *.map
