This directory contains several sample RPC distributed
applications. 

Directory        Description

CALLBACK         Fibonacci number implemented as static callback
CLUUID           Manager entry point vector sample
DATA\INOUT       Directional attributes
DATA\DUNION      Discriminated union
DATA\XMIT        Transmit as attribute
DICT             Client-server transaction processing                        
DOCTOR           RPC version of Eliza-like therapist program                    
HANDLES\AUTO     Auto handles
HANDLES\USRDEF   User-defined handles
HANDLES\CXHNDL   Context handles
HELLO            Print a string "hello, world" on the server
MANDEL           Mandelbrot set Windows sample                       
NS\CDS           Gateway protocol for DCE CDS nane service
NS\NHELLO        "hello, world" sample using name service
WHELLO           Windows version of "hello, world" sample       
YIELD            Demonstrate yield capability for MS Windows 3.x

The source files within each directory (with the exception
of the Windows samples) follow the naming convention:

README.TXT     Instructions to build, run the sample
<dirname>.IDL  Interface definition language file
<dirname>.ACF  Application configuration file
<dirname>C.C   Client main program
<dirname>S.C   Server main program
<dirname>P.C   Remote procedures
MAKEFILE       Nmake file to build NT version
MAKEFILE.DOS   Nmake file to build MS-DOS version
MAKEFILE.WIN   Nmake file to build Win 3.x version

The Microsoft Windows sample programs use several additional
files and do not conform to this naming convention. See the
README.TXT file in each directory for specific information
about the files.

*** NOTE for building client applications on MS-DOS / WIN 3.x
There are two different versions of RPC.H for MS-DOS and WIN 3.x.
Please add the proper path to your environment variable INCLUDE.
