<-------------( recommended minimum viewing width )------------------->

=======================================================================
MSCLUS.DLL for Microsoft Windows NT 4.0 Server
(c) Microsoft Corporation, 1999. All rights reserved.
=======================================================================

	Setup instructions for the Cluster Automation Server on
	Microsoft® Windows NT® 4.0 Server systems.


	CONTENTS

	I.	Introduction

	II.	Requirements

	III.	Setup Instructions

	IV.	FAQs

	V.	Further Information


=======================================================================
I.	INTRODUCTION
=======================================================================

	The Cluster Automation Server, implemented through MSCLUS.DLL,
	provides a set of scriptable Automation objects that can be
	used to create remote, web-based, interactive administration
	tools and interfaces for server clusters(1).

	The Cluster Automation Server is fully documented in the
	Platform SDK.  See "Further Information".

	(1) Other names for server clusters include Wolfpack, MSCS,
	    Clustering Service, and Cluster Server.


=======================================================================
II.	REQUIREMENTS
=======================================================================

	The Cluster Automation Server can be used on any Microsoft
	Windows NT 4.0 SP3 (or higher) Server, even if the server is
	not configured as a server cluster node.  However, note the
	following restrictions:

	1.	The server must have the following DLLs registered:
		a.	MSCLUS.DLL
		b.	CLUSAPI.DLL
		c.	RESUTILS.DLL.
		d.	ATL.DLL

	2.	The server must have access to a domain in which
		a server cluster resides.

	3.	The server must have administrative access to one or
		more clusters. Refer to the "Microsoft Cluster Server
		Administrator's Guide" or Windows NT Help for more
		information on the permissions required to administer
		a cluster.

	Refer to "Further Information" at the end of this file for
	pointers to additional information.


=======================================================================
II.	SETUP INSTRUCTIONS
=======================================================================

>>>	To create a setup disk or folder:

	1-1.	Locate MSCLUS.DLL in one of the following directories
		of the the Platform SDK tree:
		   	%PSDK%\Redist\Cluster\NT4\i386\Retail
			%PSDK%\Redist\Cluster\NT4\i386\Debug
			%PSDK%\Redist\Cluster\NT4\Alpha\Retail
			%PSDK%\Redist\Cluster\NT4\Alpha\Debug

	1-2.	Copy MSCLUS.DLL to a diskette or a shared folder.

	1-3.	On a cluster node, locate CLUSAPI.DLL and RESUTILS.DLL
		in the system directory.  These files contain code
		libraries necessary for MSCLUS.

	1-4.	Copy CLUSAPI.DLL and RESUTILS.DLL to the diskette or
		shared folder used in step 2.

	You can now use the disk or shared folder to copy the files to
	other systems as needed.

>>>	To enable Cluster Automation Server on a system:

	2-1. 	Copy MSCLUS.DLL to your system directory (%SYSTEM%),
   		for example C:\WINNT\SYSTEM32.

	2-2.	Verify that CLUSAPI.DLL and RESUTILS.DLL are present
		on the system.  If not, copy them from the setup
		disk or folder to the system directory.

	2-3. 	For any DLL copied in steps 1 and 2, run REGSRV32
		to register the DLL:

		a.	Open a command prompt window.

		b.	Type in the following command, substituting the
			actual path and DLL name for <path>\<dll name>:

				regsrv32 <path>\<dll name>

			For example:

				regsrv32 c:\winnt\system32\msclus.dll


	Repeat steps 2-1 through 2-3 for any system you want to use as
	a Cluster Automation Server script host, subject to the
	limitations specified in the "Requirements" section of this
	file.


=======================================================================
IV.	FAQs
=======================================================================

* Why is the setup process manual?
* What about Windows 2000?
* Why does the Platform SDK say that Windows 2000 is required?

	The Cluster Automation Server was originally implemented
	for the Microsoft® Windows® 2000 Advanced Server/Data Center
	platform. In Windows 2000, setup is performed automatically.

	The MSCLUS.DLL discussed here was created specifically to
	provide the Cluster Automation Server to Windows NT 4.0 Server
	systems.  Manual setup is necessary because the Windows NT 4.0
	cluster	setup procedure could not be modified.

	The Platform SDK content was locked down before the new DLL
	was available. Therefore the "Requirements" information
	it presents is outdated.


* Is the Cluster Automation Server for Windows NT 4.0 functionally
  the same as MSCLUS for Windows 2000?

	Yes - but the DLLs are NOT interchangeable due to
	platform-specific preprocessor differences.


* Can the different versions interact?

	Yes.


=======================================================================
V.	Further Information
=======================================================================

For complete reference information, as well as example code and
additional platform requirements, refer to the following
sections of the Platform SDK documentation:
	"Using Cluster Automation Server"
	"Cluster Automation Server Reference"
For best results, perform a full text search for the exact topic
title.  Check "Search titles only" and uncheck "Match similar words."

For more information on Automation and Automation objects, refer to
"Automation" in the Platform SDK documentation.

There are many sources of information on scripting.  In the Platform
SDK, "Scripting with COM Objects" provides a good starting point for
cluster scripting.

=======================================================================
(end)