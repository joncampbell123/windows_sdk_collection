July 29, 1996


Microsoft SQL Server 6.5 Service Pack 1 for Windows NT

This service pack is for use only by holders of a current valid Microsoft 
SQL Server 6.5 license.  It addresses some specific problems that were 
discovered in SQL Server 6.5 after it began shipping.  

The updated SQL Server files for this service pack include:
	readme.txt	This set of instructions.
	fixlist.txt	The list of bugs fixed in this service pack
	sqlservr.exe	The SQL Server executable
	sqlmaint.exe	SQL maintenance utility
	opends60.dll	Open Data Services DLL
	sqlevn60.dll	Event logging DLL
	sqlmap60.dll	SQLMAPI DLL
	sqlrepl.dll	Replication services DLL
	ntwdblib.dll	Win32 DB-Library DLL
	xpsqlweb.dll	Web page procedures DLL
	xplog60.dll	Integrated security extended procedures DLL
	w3dblib.dll	Win16 DB-Library DLL (Intel only)
	msdblib3.dll	Win16 DB-Library DLL (Intel only)
	sqlsrv32.dll	Win32 ODBC driver
	sqlsrvr.dll	Win16 ODBC driver (Intel only)
	dbmsspx3.dll	Win16 Novell SPX Net-Library (Intel only)
	dbnmpntw.dll	Win32 named pipes Net-Library
	odsgate.dll	Win16 ODBC driver for Open Data Services gateways (Intel only)
	odsgt32.dll	Win32 ODBC driver for Open Data Services gateways
	msdtcprx.dll	MS DTC interface proxy DLL
	dacdll.dll	MS DTC interface DLL
	xolehlp.dll	MS DTC helper APIs DLL
	dtccm.dll	MS DTC connection manager DLL
	dtccfg.cpl	MS DTC remote client configuration utility
	sqlole65.sql	Script for SQLOLE stored procedures
	instrepl.sql	Script for replication stored procedures
	procsyst.sql	Script for system stored procedures
	proc65_1.sql	Script to install modified system stored procedures
	u_tables.sql	Script for system tables
	setup.exe	Service Pack Setup program
	setup.inf	Used by Service Pack Setup program
	sqlsetup.dll	Used by Service Pack Setup program
	sqlgui32.dll	Used by Service Pack Setup program
	sqlsvc32.dll	Used by Service Pack Setup program
	splash.dll	Used by Service Pack Setup program
	sqlsetup.hlp	Used by Service Pack Setup program


Installation
------------

NOTE:  It is not necessary to upgrade to all of the files here.  You may simply 
upgrade the files based on the problem you need correction on.  For example, if 
your issue relates to the Win32 ODBC driver, you only need to upgrade the 
SQLSRV32.DLL DLL.  If you do not want to upgrade all files, do not run Setup in 
step #5 below.  Instead, you should search your hard drive and remove all 
occurrances of the file(s) you wish to upgrade, then copy the new file(s) from 
this service pack to the appropriate directory on the server.

However, the following files should always be upgraded together, or not at all.

 	 sqlservr.exe	
	 opends60.dll	
	 sqlevn60.dll


To apply the entire service pack to your server, use the following steps:

1) Place all unzipped files from the service pack into a temporary directory 
   on the computer running SQL Server.

2) Shutdown SQL Server and any applications using client libraries if they are 
   currently running.

3) Backup your existing copies of the files listed above, which are to be 
   replaced, or be certain you have your original media for these files.

4) Backup your databases.  (This is precautionary.  Nothing here will directly 
   touch your databases - but as good practice, before changing software you 
   should be sure you have an appropriate backup.)

5) From the directory containing the unzipped service pack files, run 
   SETUP.EXE.  Setup will replace the existing SQL Server files with the new 
   ones from this service pack.  It also runs three .SQL script files to 
   update several system stored procedures.

6) Restart SQL Server and SQL Executive, if necessary.

7) If you require any of the updated Win16 components, you will need to copy 
   the appropriate file(s) directly to the client.  Note that the client may 
   have the same SQL Server 6.5 DLLs in multiple directories, so it is 
   recommended that you first search your hard drive for all occurrances of the
   file you intend to replace.  Remove all copies of the old DLL before copying
   the new DLL to the client.

8) If necessary for any reason, you can simply copy back in the SQL Server 6.5 
   files to replace the Service Pack 1 versions.



Documentation Notes:
--------------------
I.  With Service Pack 1, the sp_tableoption system stored procedure includes a 
new option, "table lock on bulk load".  When this option is enabled for a 
table, bulk loading of data (from the BCP utility or the BCP library APIs) will 
only require an exclusive table lock on the table.  With the option disabled 
(the default behavior), each newly-allocated extent requires an extent lock for 
the duration of the batch.  Enabling the option allows you to specify larger 
batch sizes without needing to increase the sp_configure value for "locks".  
When using the "fast" (non-logged) bulk copy, this option may significantly 
improve the speed of bulk copy.  For "slow" (logged) bulk copy, this option may 
only show marginal improvement in the speed of bulk copy.

The following example enables this option for the Sales table:

	EXEC sp_tableoption 'Sales', 'table lock on bulk load', 'true'


II. The Microsoft SQL Server 6.5 documentation states that a SQL Server 6.5 
database dump can be loaded into SQL Server 6.0 if the 6.0 server has Service 
Pack 3 installed.  At the time version 6.5 was released, this was the intended 
goal.  Subsequently, however, it was determined that this was not technically 
feasible due to a number of changes to internal structures which were not 
backward compatible with version 6.0.  As a result, Service Pack 3 disallows the 
loading of version 6.5 database dumps.  If you attempt to load a SQL Server 6.5 
database dump into SQL Server 6.0 with Service Pack 3 applied, the load will be 
rejected and an error will be raised indicating that the dump is from an 
incorrect version.  We regret the oversight and apologize for any inconvenience.

