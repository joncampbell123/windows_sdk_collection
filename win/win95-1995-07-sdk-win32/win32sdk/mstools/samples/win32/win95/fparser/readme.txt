The default file extension for this sample is TST.  To use this 
file parser for ASCII files of different extensions, the registry must 
be changed.  The registry entry for the new extension must be given a 
new key named "QuickViewAddOn" under the subkey that contains our 
viewer class id.  Within this key, the registry should have an item 
named "DllName", whose value is the name of your DLL.

As an example, a filter whose file extension is ".XXX", would need the
following reg file contents merged into the registry:

------------------

REGEDIT4

[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\QuickView\.XXX\{F0F08735-0C36-101B-B086-0020AF07D0F4}\QuickViewAddOn]
"DllName"="vsxxx.dll"

--------------------

This would enable a user to right-click on a file of extension XXX and
have the vsxxx.dll file parser be used.

This approach will override any built-in file identification used by
SCC's DLLs, using the extension only to choose the specified file parser.

NOTE: no other ID can be used in the above registry entry.

This sample provides a template for an ASCII file parser.  It is configured
to work with the file extension ".tst", but this can be changed by modifying
FPARSER.REG to the extension you wish.

To use this parser use the following steps:

1. import FPARSER.REG into your registry
2. set the [HKEY_LOCAL_MACHINE\SOFTWARE\SCC\Viewer Technology\MS1] "Verify"
	key to a non-zero value (this will reset the list of parsers the next
	time a file is viewed).
3. build VS_ASC.DLL
4. copy VS_ASC.DLL to <windows path>\SYSTEM\VIEWERS
5. rename or copy an ASCII file to a .TST extension
6. from the Explorer, right-click the file
7. select "Quick View"
