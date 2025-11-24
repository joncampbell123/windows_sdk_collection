This sample shows how to use Universal Thunks to call APIs that are not
supported directly by Win32s.

This sample detects at run-time if it's running under Win32s or Windows NT,
and checks to see if the APIs it calls are supported or not.  If they are
not supported, it will call (via Universal Thunks) the appropriate 16 bit APIs.

Since this sample uses 16 bit code, you will need a 16 bit compiler (not
included in the Win32 SDK).  The included makefile.16 uses Microsoft C/C++
version 7.0, but it should be easily adaptable to other compilers.

Also, you will need to get some components from the \mstools\win32s\ut
directory from your Win32 SDK CD-Rom.  In particular the W32sUT.h needs
to go in %mstools%\h (and also in a directory in the INCLUDE path for
your Win16 development environment.)  W32sUT32.Lib needs to go in
%mstools%\h and W32sUT16.Lib needs to go in a directory in the LIB
path for your Win16 development environment.

Questions or comments about this Win32s sample are welcomed in the
API-Win32s section (Section 14) of the MSWIN32 forum on Compuserve.

Lee Hart
Microsoft Developer Support
