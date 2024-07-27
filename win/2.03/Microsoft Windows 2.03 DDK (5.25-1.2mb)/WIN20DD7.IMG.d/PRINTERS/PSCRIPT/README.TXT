                                                        11-January-1988
                                                                       
        Because the sources to the PostSript driver are so large, we've had
to split them over two disks.  Combine the sources from both disks into a
single build directory.  Also, make sure to copy the .LIB file(s) from the
\PRINTERS\LIB directory on disk 7 into your LIB environment (as well as the
standard libraries on disk 1).  Then, use the MAKEIT.BAT file to build the
PostScript driver.
                  
        After initially building the PostScript driver using MAKEIT.BAT, 
you can build the debugging version (which contains PRINTF statements)
by executing the MAKE file DEBUG.  In order to use the debugging version,
you must copy the file PRINTF.EXE (which is built by MAKE DEBUG) into 
your WINDOWS executable directory. When you use printf(), you MUST 
specify all pointers as long pointers, e.g.

           printf((LPSTR)"test %d, %ls",num,(LPSTR)&string[0]);


        Note: Portions of this driver were authored by Aldus Corporation.
Aldus messages in dialogue boxes MUST remain in any modified drivers that
you make.
