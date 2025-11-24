#============================================
#
#   RPCINC.MAK
#   RPC Samples Common Definition File 
#
# For No Debugging Info                nmake nodebug=1
#
#
#============================================

link=link
cc=cl

#============================================
#
#  Compile Flags - must be specified after $(cc)
#
#============================================
#  ccommon - common c-compiler flags
#
#  /FR             - Generate Browser Files
#  /ML???
#============================================
ccommon = -c -W3

cflags=$(ccommon) /D"WIN32" /D"_X86_" /D"_WINDOWS" /FR /ML

#============================================
#  cdebug -
#
#  /Ox             - Use Maximum Optimiztion (NODEBUG)
#  /Zi             - Generate complete debugging info (DEBUG)
#
#============================================

!IFDEF NODEBUG
cdebug=/Ox /D"NDEBUG"
!ELSE
cdebug=/Zi /D"_DEBUG" /Fd$*.pdb
!ENDIF

#============================================
#
#  linkcommon - common linker switches
#
#============================================

linkcommon=-machine:i386

#============================================
#
# Target Module Dependent Link Debug Flags - must be specified after $(link)
#
#============================================

!IFDEF NODEBUG
linkdebug= $(linkcommon) 
!ELSE
linkdebug= $(linkcommon) -debug:notmapped,full -debugtype:cv
!ENDIF

#============================================
#
# Subsystem Dependent Link Flags - must be specified after $(link)
#
#============================================


conflags=-subsystem:console
guiflags=-subsystem:windows

#============================================
#
# Library Definitions
#
#============================================

conlibs=kernel32.lib libc.lib
guilibs=kernel32.lib user32.lib gdi32.lib
clientlibs=rpcrt4.lib
serverlibs=rpcrt4.lib
