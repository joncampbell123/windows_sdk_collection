This sample shows how to use Universal Thunks to call APIs that are not
supported directly by Win32s. It also demonstrates how to set up a callback
from the 16-bit side to the 32-bit side. Right now, this function does nothing,
it is simply there to show how this might be done.

This sample detects at run time if it's running under Win32s or Windows NT,
then checks to see if the APIs it calls are supported or not. If they are
not supported, the application will call the appropriate 16-bit APIs via
Universal Thunks.

This sample contains 16-bit code, therefore 16-bit development tools (not
included in the Win32 SDK or MSVCNT) are required. The included makefile for
the 16-bit code, makefile.16, assumes Microsoft Visual C/C++, but it should be
easily adaptable to other compilers.

This sample requires some components from the \win32s\ut directory from your
Win32 SDK or MSVCNT CD. In particular the W32SUT.h needs to go in the project
directory or a directory in the INCLUDE path. W32SUT32.Lib and W32SUT16.Lib
need to go in a directory in the LIB path.


Lee Hart
Microsoft Developer Support
