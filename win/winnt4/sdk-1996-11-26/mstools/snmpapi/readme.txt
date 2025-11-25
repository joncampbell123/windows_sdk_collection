The statically linked SNMP.LIB that appeared on earlier releases of the
Win32 SDK has been replaced by the dynamically linked SNMPAPI.DLL.  This
DLL is built into Windows NT 4.0.  For ISVs that need to run on Windows
NT 3.51, a version of SNMPAPI.DLL, that is compatible with that release,
has been provided in this directory.

In order to install this SNMPAPI.DLL on Windows NT 3.51 copy the DLL into
the %systemroot%\system32 directory.  The SNMPAPI.DLL should never be
installed over top of an existing copy of the same DLL that has a later
version resource.  The SNMPAPI.DLL should never be installed onto a
Windows NT 4.0 system.

The i386 version of SNMPAPI.DLL has been tested and will work on Windows 95.
It should be installed to the %windir% directory.

Both debug and retail versions of SNMPAPI.DLL for Windows NT 3.51 have
been provided here.  The debug versions are for ISV debugging purposes
only.  They may not be redistributed under any circumstances.
