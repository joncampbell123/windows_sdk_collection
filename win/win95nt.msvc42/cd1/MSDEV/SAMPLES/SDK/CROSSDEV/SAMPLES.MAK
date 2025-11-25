#* SAMPLES.MAK

#* FILE-DESCRIPTION:                                                       *#
#*                                                                         *#
#*  Makefile used to build WLM samples applications.                       *#
#*                                                                         *#
#*  Options:                                                               *#
#*                                                                         *#
#*      PLATFORM:                                                          *#
#*          INTEL       Window NT Intel platform.                          *#
#*          MIPS        Window NT Mips platform.                           *#
#*          M68K        WLM / 68K platform.                                *#
#*          MPPC        WLM / Power PC platform.                           *#
#*                                                                         *#
#*      MACNAME:                                                           *#
#*          name of remote location (ex. MacHd:MacFolder)                  *#
#*          if defined the result will be downloaded (on M68K & MPPC only) *#
#*      DEBUG:                                                             *#
#*          1                                                              *#
#*          0 or not defined                                               *#
#*                                                                         *#
#*      CODEVIEW:                                                          *#
#*          1                                                              *#
#*          0 or not defined                                               *#
#*                                                                         *#
#*      DLL:                                                               *#
#*          1                                                              *#
#*          0                                                              *#
#*          not defined  (Use default for platform)                        *#
#*                                                                         *#
#* END-DESCRIPTION                                                         *#


#* ----------------------------------------------------------------------- *#
#* Set up suffixes.                                                        *#
#* ----------------------------------------------------------------------- *#
.SUFFIXES : 
.SUFFIXES : .obj .c .cpp .cxx .rsc .r .res .rc .exe .asm


#* ----------------------------------------------------------------------- *#
#* Standard definitions.
#* ----------------------------------------------------------------------- *#
CC          = cl
CPP         = cl
CFLAGS      = $(CFLAGS) $(CDEFINES) /Fo$(OUTDIR)/ $(INCLUDES)
RC          = rc
RC_FLAGS    = $(RC_FLAGS) /i $(RES_DIR)
MRC         = mrc
LINK        = link
LK_FILES    = $(LK_FILES) $(LIBS)
MFILE       = mfile
MFILE_FLAGS = copy -v

#* ----------------------------------------------------------------------- *#
#* Common definitions.                                                     *#
#* ----------------------------------------------------------------------- *#

CFLAGS      = $(CFLAGS) /c /YX /W3 /GX /nologo
CDEFINES    = /D "_MBCS" /D "_WINDOWS" /D "WIN32"
CDEBUG      = /Zi /Od /D "_DEBUG" /D "DEBUG"
CNDEBUG     = /O2 /D "_NDEBUG" /D "NDEBUG"
RC_DEBUG    = /d "_DEBUG" /d "DEBUG"
RC_NDEBUG   = /d "_NDEBUG" /d "NDEBUG"
LK_DEBUG    = /DEBUG
LK_NDEBUG   = 
LK_FLAGS    = $(LK_FLAGS) /MACHINE:$(MACHINE) /OUT:$(TARGET) /nologo


#* ----------------------------------------------------------------------- *#
#* Transform sources and append the resource path.                         *#
#* ----------------------------------------------------------------------- *#

OBJECTS = $(SOURCES:.c=.obj)
RES_DEP = $(RC_SOURCES) $(RES_DEP:  = ./res/)
RES_DIR=res

!ifdef PROJECT
PROJ = $(PROJECT)
!elseifdef PROJ
PROJECT = $(PROJ)
!else
!ERROR Project undefined:  define a project using the PROJECT variable.
!endif

!if "$(DEBUG)" == "1"
BLDTYPE = DEBUG
CRTDLL = msvcrtd.lib
!else
CODEVIEW = 0
BLDTYPE = RELEASE
CRTDLL = msvcrt.lib
!endif

!ifdef MACSYSTEM
MFILE_FLAGS = $(MFILE_FLAGS) -n $(MACSYSTEM)
!endif

!ifdef MACZONE
MFILE_FLAGS = $(MFILE_FLAGS) -z $(MACZONE)
!endif

!ifdef MACPSWD
MFILE_FLAGS = $(MFILE_FLAGS) -p $(MACPSWD)
!endif


#* ----------------------------------------------------------------------- *#
#* Intel build options.                                                    *#
#* ----------------------------------------------------------------------- *#

!if "$(PLATFORM)" == "INTEL"
MACHINE=i386
LIBS = $(LIBS) kernel32.lib user32.lib gdi32.lib winspool.lib \
comdlg32.lib advapi32.lib shell32.lib
!if "$(DEBUG)" == "1"
OUTDIR=WinDbg
OBJECTS = $(OBJECTS:  = WinDbg/)
CFLAGS = $(CFLAGS) $(CDEBUG)
RC_FLAGS = $(RC_FLAGS) $(RC_DEBUG)
LK_FLAGS = $(LK_FLAGS) $(LK_DEBUG)
PDBFLAG = /PDB:$(OUTDIR)/$(PROJ).pdb
!else
OUTDIR=WinRel
OBJECTS = $(OBJECTS:  = WinRel/)
CFLAGS = $(CFLAGS) $(CNDEBUG)
RC_FLAGS = $(RC_FLAGS) $(RC_NDEBUG)
LK_FLAGS = $(LK_FLAGS) $(LK_NDEBUG)
PDBFLAG =
!endif

#* ----------------------------------------------------------------------- *#
#* Mips build options.                                                     *#
#* ----------------------------------------------------------------------- *#

!elseif "$(PLATFORM)" == "MIPS"
MACHINE=MIPS
LIBS = $(LIBS) kernel32.lib user32.lib gdi32.lib winspool.lib \
comdlg32.lib advapi32.lib shell32.lib
!if "$(DEBUG)" == "1"
OUTDIR=MipsDbg
OBJECTS = $(OBJECTS:  = MipsDbg/)
CFLAGS = $(CFLAGS) $(CDEBUG)
RC_FLAGS = $(RC_FLAGS) $(RC_DEBUG)
LK_FLAGS = $(LK_FLAGS) $(LK_DEBUG)
PDBFLAG = /PDB:$(OUTDIR)/$(PROJ).pdb
!else
OUTDIR=MipsRel
OBJECTS = $(OBJECTS:  = MipsRel/)
CFLAGS = $(CFLAGS) $(CNDEBUG)
RC_FLAGS = $(RC_FLAGS) $(RC_NDEBUG)
LK_FLAGS = $(LK_FLAGS) $(LK_NDEBUG)
PDBFLAG =
!endif

#* ----------------------------------------------------------------------- *#
#* Macintosh 68K build options.
#* ----------------------------------------------------------------------- *#

!elseif "$(PLATFORM)" == "M68K"

MACHINE=m68k
CFLAGS = $(CFLAGS) /AL /FR /Q68s 
CDEFINES = $(CDEFINES) /D "_MAC" /D "_68K_"
RC_FLAGS = $(RC_FLAGS) /m /d "_MAC" /d "_68K_" /d "_WIN32NLS"
RSC_FILE = 1
MRC_FLAGS = $(MRC_FLAGS) /D "_MAC" /D "_68K_" /o$@

!if "$(DEBUG)" == "1" || "$(CODEVIEW)" == "1"
OUTDIR=$(MACHINE)Dbg
OBJECTS = $(OBJECTS:  = m68kDbg/)
WLMX=D
CFLAGS = $(CFLAGS) $(CDEBUG) /Q68m
RC_FLAGS = $(RC_FLAGS) $(RC_DEBUG)
LK_FLAGS = $(LK_FLAGS) $(LK_DEBUG)
PDBFLAG = /PDB:$(OUTDIR)/$(PROJ).pdb
LIBS = $(LIBS) oldnames.lib
WLMTARGET = ":$(MACNAME):$(PROJ)"
PDBFLAG = /PDB:$(OUTDIR)/$(PROJ).pdb
!else
WLMX=
CFLAGS = $(CFLAGS) $(CNDEBUG)
RC_FLAGS = $(RC_FLAGS) $(RC_NDEBUG)
LK_FLAGS = $(LK_FLAGS) $(LK_NDEBUG)
PDBFLAG =
LIBS = $(LIBS) oldnames.lib
OUTDIR=$(MACHINE)Rel
OBJECTS = $(OBJECTS:  = m68kRel/)
WLMTARGET = ":$(MACNAME):$(PROJ)"
!endif

LK_FLAGS = $(LK_FLAGS) /MAC:type="APPL" /MAC:creator="$(CREATOR)"
LK_FLAGS = $(LK_FLAGS) /MAC:bundle $(PDBFLAG)
LK_FILES = $(LK_FILES) wlm$(WLMX).lib

!if "$(DLL)" == "1"
!error 68K DLLs not supported
LIBTYPE = DLL
!else
LIBTYPE = STATIC
!endif


#* ----------------------------------------------------------------------- *#
#* Macintosh Power PC build options.
#* ----------------------------------------------------------------------- *#

!elseif "$(PLATFORM)" == "MPPC"

MACHINE=Mppc
CFLAGS      = $(CFLAGS)
CDEFINES    = $(CDEFINES) /D "_MAC" /D "_MPPC_"
RC_FLAGS    = $(RC_FLAGS) /m /d "_MAC" /d "_MPPC_" /d "_WIN32NLS"
RSC_FILE    = 1
MRC_FLAGS   = $(MRC_FLAGS) /D "_MPPC_"

LK_FLAGS    = $(LK_FLAGS) /MAC:type="APPL" /MAC:creator="$(CREATOR)"
LK_FLAGS    = $(LK_FLAGS) /MAC:bundle $(PDBFLAG)
LIBS            =


!if "$(DEBUG)" == "1" || "$(CODEVIEW)" == "1"
OUTDIR=$(MACHINE)Dbg
OBJECTS = $(OBJECTS:  = mppcDbg/)
WLMX=D
EXT=DEBUG
CDEFINES = $(CDEFINES) /Zi /Od /D "_DEBUG"
RCFLAGS = $(RCFLAGS) /d "_DEBUG"
MRC_FLAGS = $(MRC_FLAGS) /D "_DEBUG"
WLMTARGET = ":$(MACNAME):$(PROJ)"
PDBFLAG = /PDB:$(OUTDIR)/$(PROJ).pdb
!else
OUTDIR=$(MACHINE)Rel
OBJECTS = $(OBJECTS:  = mppcRel/)
WLMX=
CFLAGS = $(CFLAGS) $(CNDEBUG) /O2
RC_FLAGS = $(RC_FLAGS) $(RC_NDEBUG)
LK_FLAGS = $(LK_FLAGS) $(LK_NDEBUG)
WLMTARGET = ":$(MACNAME):$(PROJ)"
!endif


!if !defined(DLL) || "$(DLL)" == "1"
CDEFINES = $(CDEFINES) /D "_WLMDLL"
LK_FILES = $(LK_FILES) $(CRTDLL) msvcwlm$(WLMX).lib
LIBTYPE = DLL
!else
LK_FILES = $(LK_FILES)
LK_FILES = $(LK_FILES) wlm$(WLMX).lib libc.lib
LIBTYPE = STATIC
!endif


#* ----------------------------------------------------------------------- *#
#* Unsupported platform error message.
#* ----------------------------------------------------------------------- *#

!else
!ERROR $(PLATFORM) is not supported
!endif


#* ----------------------------------------------------------------------- *#
#* Set up additional variables.
#* ----------------------------------------------------------------------- *#

OBJECTS=$(OBJECTS:*= )
HEADERS=$(HEADERS:*= )
RC_SOURCES=$(RC_SOURCES:*= )
RES_DEP=$(RES_DEP:*= )
TARGET = $(OUTDIR)/$(PROJ).exe
RES_FILE = $(OUTDIR)/$(PROJ).res
!if "$(RSC_FILE)" == "1"
RSC_FILE = $(OUTDIR)/$(PROJ).rsc
MRC_CMD = $(MRC) $(MRC_FLAGS) /o$@ $**
!else
RSC_FILE =
MRC_CMD =
!endif


#* ----------------------------------------------------------------------- *#
#* Inference rules.                                                        *#
#* ----------------------------------------------------------------------- *#

.c{$(OUTDIR)}.obj:
    $(CPP) $(CFLAGS) $<

{$(SRCDIR)}.c{$(OUTDIR)}.obj:
    $(CPP) $(CFLAGS) $<

.cpp{$(OUTDIR)}.obj:
    $(CPP) $(CFLAGS) $<

.cxx{$(OUTDIR)}.obj:
    $(CPP) $(CFLAGS) $<

.rc{$(OUTDIR)}.res:
    $(RC) $(RC_FLAGS) /fo$@ $<

{$(RES_DIR)}.rc{$(OUTDIR)}.res:
    $(RC) $(RC_FLAGS) /fo$@ $<

.r{$(OUTDIR)}.rsc:
    $(MRC) $(MRC_FLAGS) /o$@ $<

#* ----------------------------------------------------------------------- *#
#* Build the application.                                                  *#
#* ----------------------------------------------------------------------- *#

ALL : $(TARGET) DOWNLOAD
!MESSAGE --------------------------------------------------------------------
!if "$(PLATFORM)" != "INTEL"
!MESSAGE Building $(BLDTYPE) version of $(PROJECT) for $(PLATFORM) using $(LIBTYPE) library.
!endif
!MESSAGE --------------------------------------------------------------------

$(OUTDIR) :
    -if not exist $(OUTDIR) mkdir $(OUTDIR)

DOWNLOAD : $(TARGET)
!if "$(PLATFORM)" != "INTEL"
!if "$(MACNAME)" != ""
    $(MFILE) $(MFILE_FLAGS) $(TARGET) $(WLMTARGET)
!endif
!endif

$(RES_FILE) : $(RES_DEP)

!if "$(RSC_FILE)" != ""
$(RSC_FILE) : $(RSC_DEP)
    $(MRC_CMD)
!endif

$(OBJECTS) : $(HEADERS)

###$(OBJECTS) : .\tools\apitest.c

$(TARGET) : $(OUTDIR) $(OBJECTS) $(RES_FILE) $(RSC_FILE)
    $(LINK) @<<
$(LK_FLAGS)
$(OBJECTS)
$(RES_FILE)
$(RSC_FILE)
$(LK_FILES)
<<
    

#* ----------------------------------------------------------------------- *#
#* Cleanup files.                                                          *#
#* ----------------------------------------------------------------------- *#

clean:
    -delnode /q $(OUTDIR)
    -if exist *.pch del *.pch
    -if exist *.sbr del *.sbr
    -if exist *.pdb del *.pdb
