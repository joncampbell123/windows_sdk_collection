                                                        01-August-1988

Dear OEM,                  

        Due to an error in Microsoft's build procedure, the Latin American
keyboard driver for Windows/286 Version 2.1 was built with the file
XLATUS.INC instead of XLATSP.INC.  To assure that your sources build to
the same binaries as the Retail Kit, we have left the error in the Makefile and
KBDLA.LNK.  As soon as you're assured that you can reproduce the same .DRV
file as that in the Retail Kit, you should correct the Makefile and KBDLA.LNK
files to use XLATSP.INC instead of XLATUS.INC.

Thanks for your understanding,

Microsoft Windows OEM Group
