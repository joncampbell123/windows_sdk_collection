This directory contains a preliminary release of DirectInput 3.0 for NT.  Please note that this preliminary release is being provided on the DirectX 3.0 SDK for development purposes only.  It may not be redistributed by ISVs under any circumstance.

To install this version, copy dinput.dll to the SYSTEM32 directory and then right-click on dinput.inf and select "Install" to install the dinput.dll library into the registry.

Note: The provided dinput.dll file will also run on the Windows 95 Operating System.  If you copy dinput.dll to your Windows 95 SYSTEM directory, DirectInput will behave in a manner similarly to the way it behaves under Windows NT.  (Make sure to save the old version of dinput.dll.)  This technique is provided as a convenience for developers who prefer to debug their applications on Windows 95 instead of Windows NT.  It is nevertheless no substitute for actually testing the program on Windows NT.

The implementation of DirectInput 3.0 for Windows NT has the following limitations:

* The keyboard can be acquired only in non-exclusive foreground mode.
* The mouse can be acquired only in exclusive foreground mode

Only the i386 version of dinput.dll for NT is provided at this time.