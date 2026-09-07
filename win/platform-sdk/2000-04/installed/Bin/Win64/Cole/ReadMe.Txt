This is the Cole 1.6 release.

Differences from Cole 1.5:
--------------------------

Add checks for detecting assignments of unaligned addresses to aligned pointers. Unaligned
addresses come from taking the address of (&) fields of a struct with non default packing size.
False positives are reported if these fields happen to be on aligned boundaries due to the
sum of the sizes of the previous fields. e.g. the address of the first field is aligned
regardless of the packing size.

Environment variables:
----------------------

ColeLogFileName - name of the a log file where the Cole compiler keeps the list of obj files
                  generated and the directories where they are located. Cole appends to this
                  file each time it is invoked, hence you will have to manually delete the
                  log file before new compilations.

ColePrintfList  - name of the file containing the user defined printf wrappers and the zero
                  based index of the format string argument.

ColeErrorFilename - name of the file that receives the error logs in the Visual C++ error
                    reporting format.

Setup and Use:
--------------

To lauch Cole, use SetCole.Bat as you would use SetEnv.Bat (for Win32
development) or SetWin64 (for Win64 development).

If your makefiles use Win32.Mak (or a derivative like BkOffice.Mak)
then you should be able to run NMake in each of your directories and
view the error log.

Port64.exe is the name of the tool.  It is a plug-compatible
replacement for cl.exe. Tell your makefile to use port64.exe instead of
CL when compiling your app.  It writes an OBJ file that contains the
warnings it generates.  Of course, the linker will keel over when it
tries to link these "objs", so it's best if your make script doesn't
die on warnings.  Port64.exe writes a log file to "colelog.txt"
(changable by setting the environment variable "ColeLogFilename").
Make sure you delete this file before each Cole build of your
application.

The advantage to producing files with extensions of ".obj" is that
this works well with the build process.  If you change a single file,
then do a new cole build, only that file (and those that depend on it)
will be reanalyzed for errors.  Of course, if you change a common
header file (which you will do a lot during the start of the port).

Getting the fancy error display requires that Perl be installed.
Most distributions of Perl work; Cole was tested with the ActiveState
distribution from <http://www.activestate.com/>

To get the fancy error display, run cole.bat giving it two arguments,
the name of the log file built by port64.exe, and the name of directory
(which it will create for you) to stuff the .htm and .err files it
generates into.  Cole.bat will invoke your browser on the root error
page for you.

Bug reporting and feature requests:
-----------------------------------

Please send feedback and bug reports to SdkFdbk@Microsoft.Com
