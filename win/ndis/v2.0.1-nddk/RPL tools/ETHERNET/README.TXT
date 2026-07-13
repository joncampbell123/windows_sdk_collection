SETTING UP THE ETHERNET DLL's
-----------------------------

To provide support for testing the Remoteboot over the ethernet environment,
the RPL Kit includes 3 ethernet DLL's and a corresponding message file.
To allow Lan Manager to to perform a RPL boot over ethernet, copy the files
ETHNET1.DLL, ETHNET2.DLL and ETHOEM.DLL to C:\LANMAN\NETLIB, and copy
ETHDLL.MSG to C:\LANMAN\NETPROG.  Also, modify the "rpl1 =" entry in
LANMAN.INI to read:

           rpl1 = ethnet1.dll ethnet2.dll ethoem.dll
