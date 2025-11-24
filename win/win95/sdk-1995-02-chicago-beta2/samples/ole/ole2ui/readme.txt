			  OLE2UI
		  OLE 2 User Interface

The file OLE2UI.HLP, in the \DOCS directory of the OLE 2.01 SDK,
contains full documentation for using the OLE2UI libraries.

Foreign language versions of the dialogs are available on the SDK
CD-ROM.  These files can be found under the \OLE2UI\RES directory
and are not copied to your system during setup.

Please see the files MAKEDLL and MAKELIB in the \OLE2UI directory
for building instructions.  You may also find useful information in
the README.TXT in the \OUTLINE directory.

MAKEDLL is set to build a private user interface library, OUTLUI.DLL, 
for the OUTLINE samples in the OLE 2.01 SDK.  If you build the OLE2UI
library as a DLL, you MUST give the DLL a unique name.  You must 
redefine the LIBNAME variable inside MAKEDLL to do this.

If you are using the Microsoft VC++ 1.0 compiler, you must have the
maintenance patch which is available over Compuserve or the Microsoft
Developers Network CD-ROM.
