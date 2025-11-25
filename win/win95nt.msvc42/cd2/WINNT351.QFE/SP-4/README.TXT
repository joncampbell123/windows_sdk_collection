---------------------------------------------------------------------------
              Microsoft Windows NT 3.51 Workstation and Server
                            U.S. Service Pack 4
---------------------------------------------------------------------------


Contents
--------
1.0  Introduction
2.0  Installation Instructions for the Windows NT 3.51 U.S. Service Pack 4
2.1  User Notes
2.2  List of Bugs Fixed in Windows NT 3.51 U.S. Service Pack 4

1.0  Introduction
-----------------
This release of Microsoft(R) Windows NT(TM) 3.51 U.S. Service Pack 4 is
easy to apply from within Windows NT and changes only those files that
were originally set up on the Windows NT Workstation or Windows NT Server
system.  Service Pack releases are cumulative: they contain all previous
fixes, as well as any new fixes made to the system.


2.0  Installation Instructions for the Windows NT U.S. Service Pack
-------------------------------------------------------------------
Installing the Service Pack from a floppy disk:
1.  Insert Service Pack Disk 1 into the floppy drive.
2.  At the Windows NT Command Prompt, change to that floppy drive (that is,
    A: or B:) and type UPDATE.
3.  Follow the instructions given on the screen.

Installing the Service Pack from a CD-ROM:
1.  Insert the Service Pack CD-ROM disk into the CD-ROM drive.
2.  At the Windows NT Command Prompt, change to that CD-ROM drive, then
    change to the winnt351.qfe\sp-4\i386 directoy, and type UPDATE.
3.  Follow the instructions given on the screen.

Installing the Service Pack from a network drive:
1.  At the Windows NT Command Prompt, type the command to connect to the
    network drive on which the Service Pack files reside.
2.  Change to that network drive and type UPDATE.
3.  Follow the instructions given on the screen.


2.1  User Notes
---------------
*** New ROUTE.EXE Command Line Argument

A new command line argument for ROUTE.EXE has been added in Service Pack 4.
The new parameter is METRIC, and is described below:

ROUTE [-f] [command [destination] [MASK netmask] [gateway] [METRIC metric]]

METRIC
   The metric option is used to associate a cost/hop count for the
   destination specified by the route entry. Generally this specifies the
   distance in number of hops from the destination. If not specified, the
   metric is set to 1 by default.


*** Emergency Repair Disk

If it is necessary to use the Windows NT Emergency Repair Disk to repair
your Windows NT system at some time after you apply the Service Pack, you
will need to re-apply the Service Pack after the repair is completed.  This
is because the Emergency Repair Disk repairs your system by restoring your
original Windows NT setup.  After the repair has completed, simply follow
the above Installation Instructions to reapply the Service Pack.

IMPORTANT NOTICE:
When using the Windows NT Emergency Repair Disk to repair your Windows NT
system after applying the Service Pack, do not repair the file SAMSRV.DLL.
If it is necessary to repair this file because it has become corrupt, you
must replace it with the Service Pack version of the file.

This is necessary because the Service Pack update process changes the user
database, and the Service Pack version of SAMSRV.DLL is needed to allow
the system to function properly.


*** Adding New Components to the System

If new software or hardware components are changed or added to your system
after you have applied the Service Pack, it will be necessary to reapply
the Service Pack.  This is because the files taken from the original
Windows NT disk set may not be equivalent to the files on the Service Pack
disk set.  It is not possible to install new components directly from the
Service Pack media (such as a new keyboard or printer driver).  New
components must be installed from the original product media.


*** Installing Symbol Files from the CD-ROM

To install the symbol files corresponding to the new binaries in Service
Pack 4, do the following (assuming your CD-ROM drive is D:, your symbol
files are located in the C:\WINNT\SYMBOLS directory, and you are installing
the files for an x86 machine):

    XCOPY /S /U /D D:\SUPPORT\DEBUG\I386\SYMBOLS C:\WINNT\SYMBOLS

This will copy the Service Pack 4 .DBG files over the existing versions of
these files.  The XCOPY command shown will copy only those .DBG files that
are already installed (/U switch), and only those with a more recent time-
date stamp (/D switch).


*** Verifying the Proper Firmware for Powerized Systems

To install Service Pack 4 on your Powerized system, you should first verify
your Firmware is up-to-date.  Your Firmware should have a date of Oct. 5,
1995, or later.  This information is displayed during the system's boot.
Optionally, you may use the fw-title command to verify this.  If you have
an earlier Firmware date, contact your hardware manufacturer to get the
latest release of the Firmware.


*** New APIs Available with Service Pack 4

Service Pack 4 contains 3 additions to the Win32 API:

Fibers
------
A fiber is a lightweight thread that is manually scheduled.  Fibers do not
provide advantages over a well-designed multithreaded application.
However, fibers can make it easier to port applications that were designed
to schedule their own threads.

Specific fiber APIs include:

ConvertThreadToFiber()
CreateFiber()
DeleteFiber()
GetCurrentFiber()
GetFiberData()
SwitchToFiber()

Winsock Enhancement
-------------------
Two new APIs, AcceptEx() and GetAcceptExSockaddrs(), have been added to the
Windows Sockets family.  AcceptEx() provides a way to asynchronously accept
a connection, obtain the local and remote addresses for the connection, and
receive the first block of data, all within a single call.

Directory Monitoring
--------------------
One new API, ReadDirectoryChangesW(), has been added to enhance an
application's ability to monitor directories.  It allows an application to
receive notification whenever a change occurs in a directory.  These
changes include file modifications, renames, and deletions.  Unlike
FindFirstChangeNotification(), this API will return the full name of the
affected file.

For complete details on these APIs, including headers, libraries and sample
code, see the Win32 SDK Addendum for Service Pack 4, available in the "For
Developers Only" section of the Microsoft Web site at
http://www.microsoft.com/devonly.


2.2  List of Bugs Fixed in Windows NT 3.51 U.S. Service Pack 4
--------------------------------------------------------------
NOTE: Use the Qxxxxxx number that precedes the title of the bug fix to
query the Microsoft Knowledge Base to find an article about that bug.

Service Pack 1
--------------
Q128453: Windows NT 3.51 Hangs (Memory Access Violation) Running Exchange
Q128454: Windows NT 3.51 Hangs with Office 95 (Word) Help File Wizard
Q130093: Err Msg: Incorrect Response from the Network
Q130292: Windows NT Win16 Subsystem Crashes Printing from Quicken 4.0
Q130677: TCP/IP Generates Frames with Loopback Address as Source Address
Q131343:  "Invalid Page Fault" Running Office 95 Help Wizard
Q131427: Windows NT 3.51 TCP/IP System Network Interface Hang
Q131683: Help File Opens Very Slowly with Office 95 Applications
Q131779: Help Fails for VB Modules in Excel
Q131865: SMC Elite Ultra NIC Causes UNIX Computers to Disconnect TCP/IP
Q132198: Middle Mouse Button Does Not Work Under Windows NT 3.51
Q132466: Problems Printing From DCA IRMA Workstation for NT 1.1
Q132470: STOP Msg: 0x0000000A or 0x0000001E When Using PING
Q132858: PowerStack with Cirrus Video Adapter Hangs During Startup

Service Pack 2
--------------
Q112665: UPDATE.EXE Starts Setup Help If Space Exists in Directory Path
Q128567: Landscape Orientation Reversed With Postscript Driver
Q129670: GSNW Not Releasing Session to NetWare Server
Q129724: Macintosh Client Has Slow Access to Windows NT 3.5x SFM Volume
Q130226: Banner Always Printed When Using GSNW Print Gateway
Q130783: STOP Msg: 0x0000003F NO_MORE_SYSTEMS_PTES Repetitive I/O on MIPS
Q130932: Desktop Remains Active At Logoff
Q130979: User Environment Variables Set Before Default Home Directory
Q131073: Datagram Sends Fail if Route is Not in IPX Cache
Q131241: FTPSVC Orphans Connections, Uses Up Virtual Memory
Q131428: DHCPADMN Reports Error 14 After You Select Local Machine
Q131689: Postscript Jobs Do Not Print Correctly Over SFM and AppleTalk
Q132085: Applications Hang When Opening Files when CSNW is Installed
Q132394: Streaming Mode NPMCA.SYS NIC Sleeps on Transmit.
Q132511: Windows NT 3.51 Hangs on Shutdown With Some S3 Based Video Cards
Q132722: Server Instability After Reboot Caused by NDIS Driver Problem
Q132896: FTP Client Scripts Terminate Without Completing
Q132903: Err Msg Using NetBIOS over TCP/IP (NETBT.SYS): STOP 0x0000000A
Q133112: NetWkstaSetUid2 API Returns Access Denied
Q133128: Printing From Windows NT 3.51 to an HP4 at 600 DPI is Slow
Q133252: Windows NT 3.51 GSNW Help Reports The File Is Corrupted
Q133280: FTPSVC: Delay Receiving FTP Directory Annotation and Prompt
Q133303: WINFILE.EXE Application Error When Associating a Searched File
Q133306: Maximum Disk Space of 1.99 GB Displayed for NetWare Volume
Q133384: Event ID 2019: Nonpaged Memory Pool Empty
Q133410: The Breakpoint "{,<filename>,} .<line>" Cannot Be Set
Q133488: LPR Printing Fails After Setting Up Security
Q133701: Forward Slash May Be Ignored As A Path Delimiter In 3.51
Q133757: Performance Monitor SQLServer-Log Object Corrupted
Q134250: No Compaq Netflex Drivers in Windows NT 3.51 for RISC Platforms
Q134285: STOP Msg: 0x0000007B or "0x4,0,0,0 Error" in WinNT 3.51 Setup
Q134286: Windows NT 3.51 LsarLookupSids errors from Big Endian Servers
Q134386: Computers Using QVision Display Driver Lock Up
Q134427: Dr. Watson Access Violation Occurs Sending Mail Attachments
Q134701: Uninitialized Pointers in DHCPSSVC.DLL Cause Access Violation
Q134765: Unknown Software Exception When Application Calls OpenGL
Q134959: Cannot copy Icons From a Common Group to a Personal Group
Q134968: NetWare Connections Remain Connected After You Log
Q134969: Faxing from 16-bit Program Using Separate Memory Space Fails
Q134985: Browsing & Other Traffic Incur High Costs over ISDN Routers
Q134988: Access Violation in glsbCreateAndDuplicateSection API on PowerPC
Q135065: Windows NT 3.51 Hangs on Shutdown
Q135275: Windows NT Backup : Incorrect Date in "Tape Name" Text
Q135277: WINS Records of Multi-homed Computers Do Not Replicate
Q135291: Print Manager: Owner Appears as System Printing from Macintosh
Q135308: Disk Administrator Corrupts Partitions
Q135471: Cannot Reconnect to OS/2 Name Space Resources on NetWare Server
Q135548: PIF Editor Reserve Shortcut Keys Settings May Be Ineffective
Q135553: IOCTL_NDIS_QUERY_ALL_STATS Causes STOP Msg in Windows NT 3.51
Q135621: NetWare Transaction Tracking System (TTS) Not Detected
Q135667: STOP 1E When Using File Manager and Services for Macintosh (SFM)
Q135692: "List Name From" List Box Shows Only 20 Trusted Domains
Q135724: Deleted Disk Space Not Released on Converted NTFS Volume
Q135774: Performance Monitor Counters Produce Unlikely Results
Q135777: Unable to Connect Using Cabletron EISA F70XX FDDI NIC
Q135856: RIP Table Does Not Update When New RIPX Response Is Received
Q136023: Batch Files Stop Executing with Windows NT version 3.51
Q136024: Tape Hardware Data Compression Disabled After Running NTBackup
Q136334: Access Violation in LSASS.EXE During User Password Change
Q136336: Windows NT Fails Because of an Access Violation in WINLOGON
Q136375: NTFS directory corruption with more than 100,000 files
Q136402: IniFileMapping for 16-Bit Windows Apps Fail in Windows NT 3.51
Q136472: NE3200 NIC Driver Can Lose Locally Administered Address
Q136627: Layered Drivers Never See WINSOCK IRPs for the TCP/IP Device
Q136780: WinFax Pro Software Causes 16-bit Applications To Hang
Q136782: Unable To Connect To AT&T Advanced Server For UNIX Printer Share

Service Pack 3
--------------
Q126688: Stack Overflow with Windows NT 3.51 RAS
Q126689: STOP Msg: c000021a Logging on a Second Time to WinNT 3.51 w/ SP2
Q126967: New TCP/IP Registry Parameter Ignores Push Bit on Receives
Q139281: STOP Msg: 0x0000004E or 0x0000000A Under Heavy Computer Usage
Q139535: Some TrueType Fonts Do Not Produce Glyphs on Windows NT 3.51
Q139635: RAS Authentication of Third Party PPP SPAP Clients Fail
Q139714: RAS Stop 0x000000a 6194ea98 00000002 00000001 80115534

Service Pack 4
--------------
Q134959: Cannot copy Icons From a Common Group to a Personal Group
Q137857: Errorlevel Paradigm Behaves Differently in Windows NT 3.51
Q137968: Perfmon and Network Monitor Counters Show Incorrect Values
Q138415: Windows NT Fails to Check for Low Disk Space (No Admin Alerts)
Q138700: ARP -s Fails After Applying SP2 to Windows NT 3.51
Q138737: Directory Synchronization May Fail with Windows NT 3.51 Mail
Q138794: Out of Memory Error Installing Windows NT 3.51 Service Pack 2
Q138854: Connecting Using NBT.SYS Causes Windows NT Session to Hang
Q138987: Novell Clients are Denied Access Logging on to FPNW Servers
Q138995: Updating to WinNT 3.51 SP2 Causes Loss of Persistent IP Routes
Q139015: Trap 0xA in ExFreePool of NTOSKRNL.EXE
Q139057: NET STOP WINS Fails to Stop WINS Service
Q139058: Battery Shutdown Signal Delayed Using Windows NT UPS Service
Q139065: Services For Macintosh on Windows NT 3.51 Service Pack 2 Hangs
Q139171: Compaq System Hangs with Incomplete IRP in Cpqarray
Q139207: S3 Driver Doesn't Correctly Support 72Hz on Some IBM PS/2 Models
Q139208: Instability in RAS using TAPI X25 Ndiswan Driver
Q139274: Updated System Environment Variables Result in STOP 0x0000021a
Q139338: WINS Counters Disappear from Performance Monitor
Q139350: WinNT General Protection Fault Exiting 16-bit Access Version 2.0
Q139351: STOP Msg: c000021a - Using Mandatory Profile w/o Access Rights
Q139380: Multi-homed WINS Server Replication Partner Failures
Q139415: New TCP/IP ArpCacheLife Parameter in Windows NT 3.51
Q139494: Multiple CRC Errors and Hardware Overruns Using RAS
Q139619: Printing From a Service To A Network Printer Fails
Q139649: Windows NT Service Can't Connect to Network Printer on 3.51
Q139691: International Characters in User Names Not Handled Properly
Q139929: SNMP Queries Of Very Long OID's May Cause SNMP Service To Hang
Q139956: RPC from a Big Endian Computer Causes GP Fault
Q139985: WINS Client Fails to Reach a Multi-homed Server
Q140008: Seed Routing Network Range Options Grayed Out with DEC FDDI
Q140258: Incomplete Server List in File Manager's Connection Dialog Box
Q140329: Trust Relationships Fail with Large Number of Trusted Domains
Q140364: Registry Size Limit Change Results in Pagedpoolsize Change
Q140400: GSNW/CSNW Creates 8.3-Format Directory Names in Uppercase Only
Q140408: Access Denied Attempting to Rename File Across the Network
Q140463: SNMP Agent Hangs on Very Long Queries
Q140506: Print Jobs Sent to SFM Printer Hang in Spooler After Printing
Q140603: SNMP Trap Frames Appear to be Dropped
Q140639: Daylight Savings Time Not Advancing
Q140675: Windows NT Doesn't Show All Files on OS/2 Server
Q140685: MAC Clients Get Access Denied on Newly Created Folders
Q140783: Access Violation on RAS Client Dialing Into Windows 95
Q140784: Call to NetUserGetLocalGroups results in NERR_UserNotFound
Q140818: STOP Message: After "DIR ..\" is Issued from a Samba Client
Q140973: Inaccessible Floppy Disk Drive on Toshiba Portege 610CT
Q140978: WINS Does Not Replicate <1c> Names Properly
Q141019: SNMP Debug Messages Are Written to Eventlog
Q141156: STOP Msg: 0x0000000A in SFMATALK.SYS After Receiving ATP Packet
Q141344: Network Connections Refused Over NetBT
Q141371: STOP Msg: STOP 0x0000000A from DLC.SYS
Q141467: Macintosh RPC Client Fails Across AppleTalk Zone
Q141520: Generic Text Driver Prints Control Characters and Blank Lines
Q141732: Adaptec AHA154x Driver Fails to Install Second Adapter
Q141753: Memory Violation Running Attachmate Extra! for Windows in WOW
Q142060: BackupExec 6.0 Not Backing Up WFW in Windows NT 3.51 SP3
Q142204: CSNW Drive Mappings Incorrect
Q142371: Perfmon Counter DISK QUEUE LENGTH Gives Incorrect Report
Q142695: SP3 Localized SERVER.HLP (On-line Books) Causes Dr. Watson Error
Q142696: WinNT 3.51 SP3 GARAM4.TTF Font Not Displayed Properly
Q142697: Unlocked Workstation Not Returned to Full Screen Application
Q142698: Service Pack Overwrites Localized Version of Windows NT
Q142699: WinNT 3.51 w/ CPU Maxed Prints Very Slowly to Banding Printers
Q142700: WinNT 3.51 SP2 SFM Share Can Only be Created on NTFS
Q142701: SAM Does Not Replicate Members of the Administrators Alias
Q142704: Windows NT Mail Client Does Not Handle Long Filenames Correctly
Q142708: WinNT 3.51 SP2 w/ UK Keyboard Gives Wrong Accented Characters
Q142709: Big Files Copy Slowly from Mac to SFM Over AppleTalk Router
Q142710: Lock Violation Opening NetWare Server File w/ Shareable Bit Set
Q142711: Windows NT Hangs on Alpha with > 1 GB System Memory
Q142714: RAS Does Not Hang Up Immediately If Usernames Are Different
Q142866: Alpha Computer w/ Proteon P139x-Plus Revision J NIC Doesn't Work
