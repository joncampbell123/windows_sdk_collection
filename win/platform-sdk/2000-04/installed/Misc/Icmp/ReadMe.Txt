[DISCLAIMER]

We have had requests in the past to expose the functions exported from 
icmp.dll.  The files in this directory are provided for your convenience
in building applications which make use of ICMPSendEcho().

Notice that the functions in icmp.dll are not considered part of the 
Win32 API and will not be supported in future releases.  Once we have
a more complete solution in the operating system, this DLL, and the 
functions it exports, will be dropped.


[DOCUMENTATION]

The ICMPSendEcho() function sends an ICMP echo request to the specified 
destination IP address and returns any replies received within the timeout 
specified. The API is synchronous, requiring the process to spawn a thread 
before calling the API to avoid blocking. An open IcmpHandle is required 
for the request to complete. IcmpCreateFile() and IcmpCloseHandle() functions 
are used to create and destroy the context handle.

See also icmpapi.h.