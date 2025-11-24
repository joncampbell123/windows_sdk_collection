This directory contains the files discussed in the tutorial.
These files build three different programs: DMTEST.EXE,
CMDBOOK.EXE, and PHBOOK.EXE.

DMTEST.EXE is a DOS program that tests the implementation of
the CPerson and CPersonList classes.

CMDBOOK.EXE is a DOS program that provides a character-based
interface to the phone book database.  This program has many
of the capabilities of the final Windows program, with the
exception of print support and (naturally) a graphical user
interface.

PHBOOK.EXE is a Windows 3.0 program; the final result.  This is
a simple but complete application that implements a phone book
database of CPerson objects.

All three programs share a common source file, PERSON.CPP.  CMDBOOK.EXE
and PHBOOK.EXE also share the database implementation in DATABASE.CPP.

The makefiles used to build the samples in this directory put object
files and executables in target-specific subdirectories.  CMDBOOK and
DMTEST use the CHAR subdirectory, while PHBOOK uses the WIN subdirectory.

Note: If you switch between building DEBUG and RETAIL versions of
a particular application, you should do a clean build.  This is required
since retail and debug object files are stored in the same target
subdirectory.  If you fail to delete retail-built object files before
switching to a debug build, or vice versa, you will likely have link-time
errors.

PWB will take care of this for you when you switch between build
types using the Options/Build Options dialog.  If you use NMAKE,
you must do the 'clean' step yourself.

For example:

nmake -f phbook DEBUG=1
nmake -f phbook clean
nmake -f phbook DEBUG=0
