Platform SDK support for Win64(tm)

This release of the Platform SDK will allow you to test-compile your
code for Win64. It supports the new 64-bit data types and pre-beta
releases of an IA64 compiler and an ALPHA64 compiler and linker.

In order to get the correct compile-time flags to test your code, you
should include Win32.mak in your makefiles. Win32.mak is in the
%mssdk%\Include directory of the CD and will be in %mssdk%\Include
after you install the SDK.  

If you do not use Win32.mak, you will need to modify your compiler command
line appropriately. "-Wp64" and "-Ap64" compiler switches are introduced to detect
64-bit specific warnings. They are usually used with "-W4" compiler switch for 
Win64.  
On x86, the "-Zs" switch specifies to check syntax only so it does not 
generate OBJ file. 
When using x86(IA-64) build environment, you need to setup "-Zs" flag because only
the IA-64 front-end compiler is being shipped now.

Please see Win32.mak for details. 

Note that many SDK samples do not yet use Win32.mak (directly or via BkOffice.mak,
INetSDK.mak, etc.) and therefore cannot be compiled for Win64 without makefile 
changes.

To use the Win64 compiler (assuming the SDK is installed to D:\MSSDK), you have to 
install:

*	Windows 2000 Beta 3 RC 1 or later.
*	Visual C++ 6.0

You will find D:\MSSDK\SetWin64.Bat. Run these commands from a command window.

    cd /D D:\MSSDK
    SetWin64 D:\MSSDK

You should see this message on x86:

D:\MSSDK>SetWin64

  Setting SDK environment relative to D:\MSSDK targeting IA64 for Win64 testing.

On Alpha, you will get the same message but targeting ALPHA64. 

If you do not see this message, follow the instructions in the message
to address the problem. Re-run SetWin64.bat to get this message.

Running SetWin64.bat sets the CPU environment variable to IA64 when
testing on x86 and ALPHA64 when testing on Alpha. It also sets your
environment variables so that \Mssdk\Bin\Win64 is first on the
executable path and \Mssdk\Include is first on the include path.

-----------------------------------------------------------------------------------------------------------------------------

There is a new tool, Cole (Porter), that can be used to look for Win64 porting issues. 
After installing the SDK please read %mssdk%\Bin\Win64\Cole\Cole.htm and 
%mssdk%\Bin\Win64\Cole\ReadMe.Txt

-----------------------------------------------------------------------------------------------------------------------------
 
Some of the MSSDK samples have been ported to be Win64-compatible.  To view a list of 
the samples that have been ported to Win64, see \Mssdk\Samples\Makefile64.
To compile this list of samples, issue the command: 

	nmake /f makefile64

after setting up your Win64 development environment as described in this document.

Note that many SDK samples do not yet use Win32.mak (directly or via 
BkOffice.mak, INetSDK.mak, etc.) and therefore cannot be compiled for Win64 
without makefile changes.

-----------------------------------------------------------------------------------------------------------------------------

During compilation, you'll typically see typical warnings where code is not
properly handling 64-bit data.

warning C4305: 'return' : truncation from 'unsigned __int64 ' to 'long ' 
warning C4311: 'type cast' : pointer truncation from 'int *__ptr64 ' to 'int ' 
warning C4312: 'type cast' : conversion from 'int ' to 'int *__ptr64 ' of greater size 
warning C4313: 'printf' : '%p' in format string conflicts with argument 2 of type '__int64 ' 
warning C4318: passing constant zero as the length to memset 
warning C4319: '~' : zero extending 'unsigned long ' to 'unsigned __int64 ' of greater size 
warning C4242: '=' : conversion from 'unsigned int' to 'unsigned short', possible loss of data 
warning C4244: 'return' : conversion from '__int64' to 'unsigned int', possible loss of data 

-----------------------------------------------------------------------------------------------------------------------------

Applications created in this environment will NOT work on the 32-bit version of Windows 2000. 
The compiler is included so that you can evaluate your Win64 code syntax.

-----------------------------------------------------------------------------------------------------------------------------

Please send your questions and feedback to nt64feed@microsoft.com
There are also several topics in the Platform SDK documentation on
porting to Win64 and targeting Win64 using MIDL.

-----------------------------------------------------------------------------------------------------------------------------
