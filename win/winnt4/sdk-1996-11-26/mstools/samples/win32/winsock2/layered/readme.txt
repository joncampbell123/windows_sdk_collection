The WinSock2 service provider interface includes provisions for
"layering" service providers.  This sample code demonstrates how
that is done.

When the sample code is built, a DLL and an EXE result.  The DLL
is the actual service provider.  The EXE is a utility that a user
can use to install and remove the layered provider.
