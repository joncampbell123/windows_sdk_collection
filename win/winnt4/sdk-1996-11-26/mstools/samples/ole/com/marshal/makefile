#/*+=========================================================================
#  File:       MAKEFILE
#
#  Summary:    Makefile for building the MARSHAL.DLL marshaling server
#              that provides the necessary Proxys and Stubs for the
#              ICar, IUtility, and ICruise interfaces that are to be
#              used across process boundaries in the LOCSERVE and
#              LOCCLIEN local server and client code samples.  The
#              MIDL compiler is used for this.  MICARS.IDL is the
#              primary source to this lesson.  It contains the the
#              interface specifications in the MIDL language.  During
#              the build MIDL creates the following intermediate source
#              files that are compiled to produce the final MARSHAL.DLL.
#
#              This Makefile creates a subdirectory (TEMP) for the
#              .OBJ and .RES files used during the build process.  It also
#              automatically registers the newly built DLL server in the
#              system Registry.  Since the build of this makefile does this
#              registration you must build the REGISTER.EXE utility first
#              (in sibling directory REGISTER).
#
#              For a comprehensive tutorial code tour of MARSHAL's
#              contents and offerings see the accompanying MARSHAL.TXT
#              file.  For more specific technical details see the comments
#              dispersed throughout the MARSHAL source code.
#
#              See also LOCCLIEN.TXT (in the sibling LOCCLIEN directory)
#              for more details on the LOCCLIEN app and how it works with
#              MARSHAL.DLL itself.
#
#              In general, to set up your system to build and test the
#              Win32 code samples in this OLE Tutorial series, see the
#              global TUTORIAL.TXT file for details.  This MAKEFILE is
#              Microsoft NMAKE compatible and the 'debug' build can be
#              achieved by simply issuing the NMAKE command in a command
#              prompt window.
#
#  Builds:     MARSHAL.DLL, MARSHAL.LIB, MICARS.H, MICARS_I.C, MICARS_P.C,
#              DLLDATA.C.
#
#  Origin:     11-24-95: atrent - Editor-inheritance from DLLSERVE source.
#
#--Usage:-------------------------------------------------------------------
#  NMAKE Options
#
#  Use the table below to determine the additional options for NMAKE to
#  generate various application debugging, profiling and performance tuning
#  information.
#
#  Application Information Type         Invoke NMAKE
#  ----------------------------         ------------
#  For No Debugging Info                nmake nodebug=1
#  For Working Set Tuner Info           nmake tune=1
#  For Call Attributed Profiling Info   nmake profile=1
#
#  Note: The three options above are mutually exclusive (you may use only
#        one to compile/link the application).
#
#  Note: creating the environment variables NODEBUG, TUNE, and PROFILE
#        is an alternate method to setting these options via the nmake
#        command line.
#
#  Additional NMAKE Options             Invoke NMAKE
#  ----------------------------         ------------
#  For No ANSI NULL Compliance          nmake no_ansi=1
#    (ANSI NULL is defined as PVOID 0)
#  To compile for Unicode               nmake unicode=1
#    (Default is ANSI)
#  To clean up temporary binaries       nmake clean
#  To clean up all generated files      nmake cleanall
#  To register DLL                      nmake register
#  To unregister DLL                    nmake unregister
#
#---------------------------------------------------------------------------
#  This file is part of the Microsoft OLE Tutorial Code Samples.
#
#  Copyright (C) Microsoft Corporation, 1996.  All rights reserved.
#
#  This source code is intended only as a supplement to Microsoft
#  Development Tools and/or on-line documentation.  See these other
#  materials for detailed information regarding Microsoft code samples.
#
#  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
#  PARTICULAR PURPOSE.
#=========================================================================+*/

#  WIN32.MAK should be included at the front of every Win32 makefile.
#
#  Define APPVER = [ 3.50 | 3.51 | 4.0 ] prior to including win32.mak to get
#  build time checking for version dependencies and to mark the executable
#  with version information.
#
#  Define TARGETOS = [ WIN95 | WINNT | BOTH ] prior to including win32.mak
#  to get some build time checking for platform dependencies.
#
APPVER=4.0
TARGETOS=BOTH
!include <win32.mak>

# Assign the main program name macros.
DLL = marshal

# Use a temporary sub-directory to store intermediate
# binary files like *.obj, *.res, *.map, etc.
TDIR = TEMP

# Assign destination and consumer sibling directories.
IDIR = ..\inc
LDIR = ..\lib

REGEXE = ..\register\register.exe

# The sibling ..\INC and ..\LIB directories are added to the front of
# the INCLUDE and LIB macros to inform the compiler and linker of
# these application-specific locations for include and lib files.
INCLUDE=$(IDIR);$(INCLUDE)
LIB=$(LDIR);$(LIB)

LINK = $(link)

# If UNICODE=1 is defined then define UNICODE during Compiles.
# The default is to compile with ANSI for running under both
# Win95 and WinNT.
!IFDEF UNICODE
LINKFLAGS = $(ldebug)
CDBG=$(cdebug) -DUNICODE -D_UNICODE
RCFLAGS = -DWIN32 -DRC_INCLUDE -DUNICODE
!ELSE
LINKFLAGS = $(ldebug)
CDBG=$(cdebug)
RCFLAGS = -DWIN32 -DRC_INCLUDE
!ENDIF

# If NODEBUG is not defined then define DEBUG during Compiles.
# The default is to compile with debug symbols in the binaries.
!IFNDEF NODEBUG
CDBG = $(CDBG) -DDEBUG
RCFLAGS = $(RCFLAGS) -DDEBUG
!ENDIF

# APPLIBS are libraries used by this application that are outside
# of its indigenous file set and are needed during the final link.
APPLIBS = apputil.lib rpcrt4.lib

# DLLOBJS is a macro that defines the object files for the DLL.
DLLOBJS = $(TDIR)\$(DLL).obj $(TDIR)\micars_p.obj $(TDIR)\micars_i.obj \
  $(TDIR)\dlldata.obj

# The final target.
all: input tempdir output

# Check if prior builds were done.
input:
  @IF NOT EXIST $(REGEXE) echo !!!!!! You must build REGISTER first !!!!!!

# Make the temporary work sub-directory.
tempdir:
  -mkdir $(TDIR)

# Generate the proxy/stub source from the .IDL file.
micars.h micars_p.c micars_i.c dlldata.c: micars.idl
  midl /ms_ext /app_config /c_ext micars.idl

# The actual output products.
# Register the server after it's DLL is built.
output: micars.h micars_p.c micars_i.c dlldata.c $(DLL).dll
  if exist $(DLL).DLL if exist $(REGEXE) $(REGEXE) $(DLL).dll
  if exist micars.h copy micars.h $(IDIR)

# Compilation/Dependency rules for the .DLL source files.
#
$(TDIR)\micars_i.obj: micars.h
  $(cc) $(cvarsdll) $(cflags) $(CDBG) -Fo$@ micars_i.c

$(TDIR)\micars_p.obj: micars.h
  $(cc) $(cvarsdll) $(cflags) $(CDBG) -Fo$@ micars_p.c

$(TDIR)\dlldata.obj: micars.h
  $(cc) $(cvarsdll) $(cflags) $(CDBG) -Fo$@ dlldata.c

$(TDIR)\$(DLL).obj: $(DLL).cpp
  $(cc) $(cvarsdll) $(cflags) $(CDBG) -Fo$@ $(DLL).cpp

# Compile the DLL resources.
$(TDIR)\$(DLL).res: $(DLL).rc $(DLL).ico
  rc $(RCFLAGS) -r -fo$@ $(DLL).rc

# Link the object and resource binaries into the target DLL binary.
# Build the import LIB file so apps can link to and use this DLL.
$(DLL).dll: $(DLLOBJS) $(TDIR)\$(DLL).res
    $(LINK)  @<<
    $(LINKFLAGS) $(dlllflags)
    -export:DllGetClassObject
    -export:DllCanUnloadNow
    -export:GetProxyDllInfo
    -export:DllRegisterServer
    -export:DllUnregisterServer
    -out:$@
    -base:0x1C000000
    -implib:$*.lib
    -map:$(TDIR)\$*.map
    $(DLLOBJS)
    $(TDIR)\$*.res
    $(olelibsdll) $(APPLIBS)
<<


# Target to register the server
register:
  if exist $(DLL).DLL if exist $(REGEXE) $(REGEXE) $(DLL).dll

# Target to unregister the server
unregister:
  if exist $(DLL).DLL if exist $(REGEXE) $(REGEXE) /u $(DLL).dll

# Target to clean up binaries.
clean:
  -del $(DLL).exp
  -del $(DLL).lib
  -deltree /y $(TDIR)
  -rmdir /s /q $(TDIR)

# Target to clean up all generated files.
cleanall:
  if exist $(DLL).DLL if exist $(REGEXE) $(REGEXE) /u $(DLL).dll
  -del *.exe
  -del *.dll
  -del *.lib
  -del *.exp
  -del *.obj
  -del *.res
  -del *.map
  -del *.pdb
  -del *.vcp
  -del *.aps
  -del *.pch
  -del *.sbr
  -del *.bsc
  -del *.mdp
  -del *.ncb
  -del *.mak
  -del micars.h
  -del micars_i.c
  -del micars_p.c
  -del dlldata.c
  -del makefile.exe
  -del resource.h
  -deltree /y windebug
  -rmdir /s /q windebug
  -deltree /y winrel
  -rmdir /s /q winrel
  -deltree /y $(TDIR)
  -rmdir /s /q $(TDIR)
