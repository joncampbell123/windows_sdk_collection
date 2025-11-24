SHESAMP - Shell Extension Sample

This sample shows the IExtractIcon, IContextMenu, and IShellPropSheetExt interfaces. For information on these interfaces please read shellext.doc in the Windows 95 SDK documentation directory and the shlobj.h file in the Windows 95 SDK inc32 directory. 

All these extensions use one CLSID, and the sample creates a DLL that implements the interfaces. You must use the shesamp.reg file to register the CLSID of the sample. If you do not install the Windows 95 SDK to c:\sdk be sure to edit the shesamp.reg file to account for the directory that contains the shesamp.dll file. 

If Windows 95 is not installed in c:\WINDOWS, you should edit the two .she files, sample.she and samp2.she, to reflect your Windwos 95 system direcory. SHESAMP reads these files to choose an icon for the files.

You can build the sample by using the makefile and the NMAKE utility, or you can use the shesamp.mak file in Visual C++ 2.0. If you use Visual C++ 2.0, be sure to set the include and lib directories in Tools, Options to search the Windows 95 SDK  inc32 and lib32 directories before the include and lib directories of Visual C++ 2.0.




