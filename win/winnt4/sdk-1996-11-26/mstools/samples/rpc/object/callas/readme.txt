CALLAS


The CALLAS program uses the call_as attribute to transmit a non-remoteable, 
OLE custom interface. The distributed application provides a class factory 
to manufacture a server object and the implementation of the callas server
object.

The directory samples\rpc\object\callas contains the following files for
building the distributed application CALLAS:

File          Description

README.TXT    Readme file for the CALLAS sample
CALLAS.IDL    Interface definition language file
CALLAS.ACF    Attribute configuration file
CALLAS.DEF    Module definitions file
CALL_AS.C     Glue routines for proxy DLL
CLIENT.C      Client application
SERVER.CXX    Server application
MAKEFILE      Nmake file to build for Windows NT or Windows 95
