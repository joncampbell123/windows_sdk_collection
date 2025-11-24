	    * Windows 95 Beta-2 Release Notes *

Table Of Contents
-----------------
Installation Instructions
Install Notes for Particular Devices
On Line Registration
Notes for Setting Up Networks
Networking
Plug and Play
Power Management
File System/Memory Management/Device Drivers
Info Center & Microsoft Exchange
The Microsoft Network
Application Compatibility
Applets
Display
Modem Communications
Mouse
Printing
Control Panel
Pen Windows
Accessibility
SDK/DDK
WinHelp
KeyBoard Layouts


INSTALLATION INSTRUCTIONS
=========================
If you do not have a previous beta version of Windows 95 on your
computer, you should run Setup from Windows 3.x or Windows for
Workgroups 3.x, and upgrade it if possible.  If you have an
earlier version of Windows 95, see the instructions below.

Setup now checks the hard disks on which Windows 95 will
be installed for damage.  It is actually only checking for
cross-linked files.  If Setup finds any cross-linked files, it
will warn the user and exit.  You will need to run ScanDisk
to repair the cross-links and re-run Setup.  There is a copy of
ScanDisk on Disk 1 or the Retail directory on the CD.


If you have a Compaq LTE Elite with a Docking Station:
------------------------------------------------------
We recommend that you disable the built in ethernet controller before
running Windows 95 Setup and use a different network card.

From CD-ROM
-----------
Run SETUP.EXE from the RETAIL directory on the CD-ROM.  Be sure to
read all of the following release notes first.

From Floppy Disks
-----------------
Run SETUP.EXE from Disk 1.  Note that Windows 95 uses Microsoft's
new Distribution Media Format (DMF) which allows more data to be
stored on each disk.  Normal MS-DOS disk commands (COPY, DISKCOPY,
etc.) will not work on disks 2 and above.  You can use the EXTRACT
utility on disk 1 to extract files from DMF Disks (type EXTRACT /?
for instructions).

If you create disks from the CD-ROM using TRANSFER.BAT, and you
encounter GPF's during Setup, you may need to run Setup from MS-DOS
to get around these problems.

Setup over previous versions of Windows 95
------------------------------------------
IMPORTANT NOTE FOR USERS OF DOUBLESPACE OR DRIVESPACE:  If you have drives
compressed with DoubleSpace or DriveSpace and have used an earlier version
of Windows 95 on your system, you are strongly advised to run the enclosed
version of ScanDisk on all your compressed drives BEFORE setting up Beta 2.
Although you may not have noticed any problems, the folders on your
compressed drives may contain damaged entries that can cause your drive to
become corrupted during Setup.  An updated version of ScanDisk that can
detect and correct this problem is included with Beta 2 (located on Disk 1
or on the CD-ROM).  Earlier versions of ScanDisk and third-party utilities
will not detect this problem and could cause data loss.

You can upgrade Windows 95 versions 122 and higher.  To upgrade Build 122
(M6/Beta 1) or later, you will have to run Setup from real-mode (it will
not run from Windows 95 GUI build 122 or higher).

If you are running Windows 95, you can get to real-mode by placing a
"Pause" statement at the end of your AUTOEXEC.BAT file, and hitting CTRL-C
when asked to "Press any key to continue . . ."  Or you can Interactive
Boot by hitting F8 when at the "Starting Windows..." message at boot time and
answer NO to WIN at the end of AUTOEXEC.BAT processing.  You should then be
able to run Setup from the C:\ prompt.  If you are installing from a
CD-ROM, you will need to have real-mode CD-ROM drivers loaded to access
the CD-ROM from real-mode (the autoexec.dos and config.dos files may
contain the lines that load these drivers).

Windows 95 no longer preserves your hardware settings on an upgrade.
Please note your hardware settings prior to install.  This applies
to your network settings as well, so you may have to manually configure your
network settings.  A "Custom" install will show all the wizard pages so the
settings can be verified.

NOTE: if you upgrade a previous version of Windows 95 and don't install the
same optional components that were originally on the system (i.e. WordPad,
WinPad, Dialup Networking, etc.), you will encounter problems running the
old versions of these programs on Beta 2.  You should go to the
Control Panel/Add Remove Programs applet and use the "Windows Setup" tab
to install the newest versions of the optional components.

Setup over Windows NT (including Daytona)
-----------------------------------------
If you are multi-booting MS-DOS and Windows NT, boot to MS-DOS, and
run Setup from either MS-DOS or Win 3.x.  You will not be able to
install Windows 95 to a directory with a shared Win 3.x/Windows NT
configuration, you will need to install Windows 95 to a different
directory.

If you are not multi-booting MS-DOS and Windows NT, you should
configure your machine to multi-boot MS-DOS and Windows NT first, and
then follow the instructions above.

If you boot MS-DOS from a floppy and run Setup, you will no longer be
able to boot Windows NT (however, you can restore Windows NT by
booting from the Windows NT boot/repair disk, and selecting the
"Repair" option).

Setup over OS/2
---------------
Setup will not run on OS/2.  You will need to boot to MS-DOS and run
Setup from the MS-DOS prompt.

If you are using OS/2 Boot Manager to choose operating systems at
startup, Setup will disable Boot Manager to insure that Windows 95 can
reboot the system and complete its installation. Boot Manager can be
reactivated by booting with an OS/2 boot disk and using the OS/2
FDISK utility.

If you are not using Boot Manager, you should configure your machine
to use Boot Manager, and then follow the instructions above.

If you boot MS-DOS from a floppy and run Setup, you will no longer be
able to boot OS/2 after Windows 95 has been installed.  You will need to
delete the AUTOEXEC.BAT and CONFIG.SYS files that OS/2 uses before
running Windows 95 Setup.

For users with MultiConfig config.sys and autoexec.bat files
-------------------------------------------------------------
Beta 2 Setup now has complete support for upgrading config.sys and
autoexec.bat files that use MS-DOS 6.x style MultiConfig menu support.
Please upgrade over your existing autoexec.bat and config.sys and
report any problems.

Installing Optional Components After Windows 95 is Installed
------------------------------------------------------------
Optional components can now be added/removed after Windows 95 is installed
without re-running Setup.  The "Add/Remove Programs" Control Panel has a
"Windows Setup" tab that will let you add and remove optional components from
inside Windows 95.  Note that uninstall for some of the components is not
supported in this release.  In these cases, once the component has been
installed, it will no longer show up in the list in the Control Panel page.

Location of the Programs Folder
-------------------------------
As of build 189, the Programs folder was moved from the Windows folder to
the Start Menu folder.  Setup will attempt to move any links that you had
in your old Programs folder to the new Programs folder in the Start Menu
folder.  You may need to manually move any links from the Old Programs folder
in the Windows directory to the new Programs folder in the Start Menu folder.

Release Exipration
------------------
The M7 and subsequent M7.x releases will stop working on 3/31/95.

Known Setup Problems
--------------------
- Some settings in PIF files will be lost on an upgrade
  over build 122.

- Shortcuts to Control Panel Applets will be lost on an
  upgrade over build 122.

- The WINBOOT directory that was created in build 122
  is no longer used, you can delete it.

- WINBOOT.SYS is no longer used (and should be deleted by Setup).
  It has been replaced with IO.SYS.

- WINBOOT.INI is no longer used.  It has been renamed to MSDOS.SYS
  for compatibility reasons.  MSDOS.SYS should NOT be deleted.

- COMMAND.COM is no longer in the Windows\Command folder.  It is now
  copied to the Windows folder.

- COMMAND.PIF will be deleted when upgrading over a previous version
  of Windows 95.

- If you have a version of the backup applet on your hard disk and
  you are upgrading, you will get a dialog saying the version on
  your hard disk is newer than the one which you are installing with
  the setup program for this Beta 2 release of Windows 95.  Just
  select "replace all" and continue with setup.

- If you receive the following error message during setup:

    Generic Installer Error
    UpdateInis= processing error
    exiting with error code = 405

  File a bug report, then remove the Device=EMM386.EXE line from
  CONFIG.SYS and re-run Setup.  Be sure to include the setuplog.txt,
  win.ini and system.ini files in the bug report.

- If you are using DblSpace or DrvSpace, and Setup reports that you need
  more free space on Drive C:'s host drive, you can delete the DRVSPACE.TMP
  directory from that drive.  If there is no DRVSPACE.TMP, you may have to
  move your registry files to drive C from its host drive.  To do so, boot
  your system to real-mode (see above) and copy SYSTEM.* (and USER.*, if
  present) from the \windows folder on your host drive to your Windows 95
  folder on drive C. Then delete those files from your host drive and reboot.

INSTALL NOTES FOR PARTICULAR DEVICES
====================================

Logitech Mouse
--------------
If you have an older Logitech mouse ("C series") and run Setup from MS-DOS,
your mouse may not work during Setup.  This is a known problem and will be
fixed in a future build.

PS/2 Mouse
----------
Anyone installing build 166 over 165 on a machine with a PS/2 mouse should
change the line DOS=LOW,UMB in CONFIG.SYS to DOS=HIGH,UMB before
installing. This will fix the problem of not being able to move the
mouse (and probably use the keyboard) during setup.

Computers with a PCI Bus
------------------------
If you have a computer with a PCI bus, you must remove any memory managers
(such as EMM386, QEMM, etc) before running setup.  The protect mode disk
driver will take over only on the second boot (and not on the first boot).


Intel EtherExpress16
--------------------
If you have an Intel EtherExpress16 network adapter, setup may detect that
it is using IRQ 3.   This setting will conflict with your mouse or serial
ports.  While setting up, go through Custom setup and verify the net card
settings.  On the network screen, bring up the properties for the Intel
EtherExpress16 and go to the Resources page.  Set the IRQ to 5 (or any value
other than 3 that does not conflict with other devices on your system).

Xircom PE3 Parallel Port Ethernet Adapter
-----------------------------------------
On some machines, the first boot following setup might say your Xircom PE3
adapter is not functioning.  Machines that are affected have lpt ports that
use an Input/Output range other than 378, or an Interrupt of other than 7.
Examples of machines that have this characteristic are IBM Thinkpad laptops
and some Compaq laptops.

If you do not know what value your lpt port uses, look in the System control
panel, Device Manager page, Ports\Printer Port Properties, Resources page.
The Input/Output range should be 378, 3BC, or 278.  Also, if the Printer Port
is LPT2, you will want to use Interrupt 5.

You will want to set your PE3 to use these values.  To do this, look in the
Network Control Panel, Xircom PE3 properties, Advanced Properties page.  Set
I/O Base Address to the value you found in the Input/Output box for your
Printer Port, and set the Interrupt Level to 5 if you are using LPT2.

Reboot your system, and your network should become active.

Disk & CD related issues
------------------------

Please see the "FILE SYSTEM/MEMORY MANAGEMENT/DEVICE DRIVER" section for more
information on Disk and CD related specific device issues.

ONLINE REGISTRATION
===================

If you performed a Custom installation of Windows 95 and chose to
install The Microsoft Network Online service, you will also have the
opportunity to register your Beta copy of Windows 95 using your
modem.  You can do this by either choosing the Online Registration
button which appears on the Welcome to Windows 95 dialog, or by
running the Online Registration program.  Run this program by
choosing the Start menu, then choosing Online Registration in the
Accessories folder in the Programs menu.


NOTES FOR SETTING UP NETWORKS
==============================

General
-------
If you are setting up Windows 95 for a network, ensure that the network client
software is correctly installed and is running.  Verify network connectivity
by connecting to a server before you run Setup.  Also be sure that the
directory that contains your network software is in the PATH= environment
variable.

As of build 170, Windows 95 no longer preserves your hardware settings on an
upgrade over previous builds of Windows 95. Please note your hardware settings prior
to install.  This applies  to your network settings as well, so you may have to
manually configure your network settings.  A "Custom" install will show all the
wizard pages so the settings can be verified.

During the net part of setup, if you double-click on the client you must
either disable "Log on to Windows NT domain" or enter a value in the domain
field (it's blank by default).

Frame Type
----------
The Microsoft IPX/SPX compatible protocol now automatically detects
the Frame Type.  The Frame Type should be set to Auto.

Novell NetWare (VLM)
--------------------
If you are upgrading over a machine running VLM but not running Windows, you
will have to run the Novell VLM install program.  This program adds lines to
your system.ini that are no longer necessary.  When you install VLM support in
the network control panel, it will remove these lines.  Until you do so,
you will get error messages about vnetbios not being present when you boot.

DEC Pathworks
-------------
If you use DLLNDIS.EXE, you must use a version dated 10/06/93 or later to work
with Windows 95.

Artisoft Lantastic
------------------
The Lantastic server cannot be run when Windows 95 is setting up.

Artisoft Lantastic Server must be loaded in WINSTART.BAT
--------------------------------------------------------
In order to run the Artisoft Lantastic server, you must load the server
in the WINSTART.BAT file.  You may need to create this file; it belongs
in the Windows directory.  If you load the server in another batch file,
you machine will hang while booting.

Lantastic 6 support
-------------------
To get support for Lantastic 6, choose Lantatsic 5 support in the network
control panel.

Banyan VINES
------------
When you first boot into Windows 95, the Banyan drivers will report that
the VINES version is not the latest.   Follow these steps:

1. Check the "Keep files on network" option.  If you do not have this
option, edit your vines.ini and remove the WindowsVersion line.
2. Click Cancel
3. Windows will restart. When it does, click Install.
4. Windows will restart again.   VINES should now be configured properly.

Cancelling copying files
------------------------
If you go into the net control panel, add something, setup will save the changes
even if you press cancel while Windows is copying files.  When you reboot, you
may receive error messages about missing components.   This is a known problem.
To fix the problem, go back to the net control panel and remove the component
that you had added.

ODI Drivers and IRQ2
--------------------
If you have an ODI driver that you want to run on IRQ2, choose IRQ9/2 in the
network control panel.  The control panel set INTERRUPT to 9 in your net.cfg.
You will have to edit your net.cfg and changed INTERRUPT to 2.   Note, this will
get overwritten any time you press OK in the control panel.

Net0111 error after upgrading from real mode Windows for Workgroups TCP/IP
--------------------------------------------------------------------------
Windows 95 may give you this error.  Your network will still function, but net
setup does not remove the old version of TCP/IP.  To remove the old version of
TCP/IP, remove the following lines from autoexec.bat or config.sys:
  tcptsr
  tinyrfc
  nmstsr
  emsbrf
  sockets
  USER=c:\machine
  INIT=c:\machine

Setup does not migrate the protocols that were running under Windows fo Workgroups
----------------------------------------------------------------------------------
You may have to go to the network control panel and add the protocols again.

Printers not being shared after restarting Windows 95
-----------------------------------------------------
In cases where a printer is shared, and an attribute of the
printer is changed, the printer will not be shared upon the next
Windows 95 session until the printers folder is opened or a job
is printed from the local machine. Note this is only for the very
first session after the printer settings have been changed,
subsequent sessions will allow the printer to be shared as expected.
This problem will be corrected in the next release.

Removing Microsoft client for NetWare networks
----------------------------------------------
Windows 95 setup automatically installs the Microsoft client for NetWare
networks whenever it detects a netware client (Novell or Microsoft).  If
you remove the Microsoft client to reinstall the Novell client, follow
these steps:

1) Remove the Microsoft client and IPX/SPX compatible protocol from the net
control panel.

   Do *NOT* remove the net adapter.

2) Add your Novell client and Novell IPXODI in the network control panel.

3) Check the following:

* For either Novell client, make sure all the lines have been added back
to your autoexec.bat (LSL, MLID, IPXODI, NETX or VLM).

* For NETX clients, you will need to add your lastdrive= line back to
config.sys.

DOS LAN Manager and DLC
-----------------------
Setup may leave a "load msdlc" in your autoexec.bat when you upgrade from
DOS LAN Manager with DLC installed.   Simply delete this line from your
autoexec.bat and install DLC via the network control panel.


NETWORKING
==========

Server based setup
------------------
  M7 is the first beta of Windows 95 that allows you to run Windows over the
  network.  The beta Windows 95 Resource Kit contains complete instructions
  in for installing Windows in this environment (see chapter 4, "Server Based
  Setup for Windows 95").  The Resource Kit can be downloaded from the WINBTU
  forum on Compuserve.

  There are a few limitations in this release:

  * Windows 95 currently only runs over the client for Microsoft Networks,
    and the Microsoft client for NetWare Networks.  You cannot run with
    any real mode clients in this release.

  * You must boot off a hard disk.  Booting off a floppy disk or remote boot
    (RPL) server is not supported in this release.   Note, the options are
    available in the setup UI, but the do not work.

  * Your network card must have an NDIS2 (16 bit)  *AND* and NDIS3 (32 bit)
    driver.

  Known bugs:

  * During first boot, you will get a message that MSBATCH.INF is missing -
    this is a harmless message.  You can ignore it.

  * Shutdown does not get all the way to the Windows 95 "moon" screen.  This
    makes it appear that the machine is hung.  When shutting down these
    machines, wait for a few moments after disk activity has stopped, and
    then you may reboot or turn off the machine.

  * After running netsetup.exe to copy files to the server, you must
    manually copy the file netsetup.lay to your server.  Rename it on the
    server to layout.inf.  Also copy this layout.inf to the INF subdirectory
    on the server.

  * You cannot run login scripts in real mode before you get into the GUI.
    Once in the GUI, your login scripts will run.

  * If you encounter a Windows Protection Error on startup, edit the msdo.sys
    file in the root directory of your hard drive (it is a hidden text file
    in Windows 95), and remove the LoadTop= line.

  Additional instructions for installing Windows 95 on the server:

  1) Use netsetup.exe to install Windows on the server.
  2) Copy netsetup.lay from the NETADMIN directory on the CD to layout.inf
     in the same directory you installed Windows to.
  3) Copy nwredir.vxd from the NETADMIN directory on the CD to the SYSTEM
     subdirectory.

  Installing a client for NetWare Networks:

  1) Start your existing real mode network, connect to the server, and run
     setup.
  2) Choose a Custom Setup
  3) In the Network page, choose properties of the Client for NetWare
     Networks and enter a preferred server.
  4) Choose the properties of the IPX/SPX compatible protocol and set your
     network ID and Frame type.
  5) Continue with setup.

  Installing a client for Microsoft Networks:

  Simply follow the instructions outlined in the resource kit.

LOGONOFF.EXE
------------
LOGONOFF.EXE has been removed starting with build 170.  The only way to log
off in Windows 95 is from the tray, choose Start, Shut Down..., and the third
radio button (currently worded as "Close all applications and log on as a
different user").


MICROSOFT FILE AND PRINT SHARING FOR NETWARE
============================================

NetWare server must have account named WINDOWS_PASSTHRU
-------------------------------------------------------
To run MS file and print sharing for NetWare, you must enter the name of a
NetWare server on your network that will provide a user list and user
authentication.  (This is entered in the Network Control Panel, Security
tab - see setup notes above).  You must create an account named
WINDOWS_PASSTHRU on that NetWare server.  This account must not have a
password, and does not need any permissions.

Must be configured for SAP for NETX and VLM clients to access
-------------------------------------------------------------
In order for NETX and VLM clients to see and connect to Windows 95 computers
running MS file and print sharing for NetWare, the Win 95 computer must be
configured to use SAP in the network control panel.  Win 95 computers
running the Microsoft Client for NetWare can connect to computers running
either the SAP Browsing or the Workgroup browsing.

NETX and VLM clients cannot capture shared printer on Windows 95
-------------------------------------------------------------
NETX and VLM clients cannot connect to a shared printer on a Win 95
computer running MS file and print sharing for NetWare.


MICROSOFT CLIENT FOR NETWARE
============================

Login.exe not supported
-----------------------
Login.exe (command-line login in the login directory of the NetWare server)
is not supported by the Microsoft client for NetWare.  You can use the
cabinet to log into a server.  If the MS Client for NetWare is your default
client, you can log into the first NetWare server via the login prompt that
appears when you start Windows 95.

First Drive behavior is different from NETX or VLM
--------------------------------------------------
The Microsoft client for NetWare does not map the first network driver to the
 preferred server.  However, it does make a connection to the preferred
server, so that this server shows up in your Network Neighborhood.

Login Scripts: cannot load TSR's
--------------------------------
The login script processor with the MS Client for NetWare cannot handle
loading TSR's from a login script.  You should load TSR's in WINSTART.BAT
(TSR will be available to Windows apps only) or AUTOEXEC.BAT.  If you must
load a TSR in a login script, use Novell's NETX or VLM client.

Login Scripts: other problems
-----------------------------
The login script processor does not currently handle conditionals with
many "ands" and "ors".  It also does not handle statements with GREATER
THAN OR EQUAL TO (or LESS THAN).  Finally, it does not handle the write
flags \r, \7, \n, and \".

Must load OS/2 namespace on NetWare server to have long file names
------------------------------------------------------------------
In order to create files or directories on a NetWare server that have long
names, you must have the OS/2 namespace loaded at the server.  (Note: this
is by design)  To load the OS/2 namespace on a NetWare server, consult a
NetWare manual.

NWPopoup not supported; use WinPopup
------------------------------------
The NWPopUp messaging utility is not supported by the Microsoft Client for
NetWare.  The WinPopUp utility may be used instead.


NOVELL NETWARE (NETX and VLM Clients)
=====================================

Mapping a network drive in the GUI for VLM clients
--------------------------------------------------
With certain versions of the VLM client, you will be unable to map a
network drive using the GUI.  If you go out to a dos box, you will still be
able to map the drive.  The latest upgrade of vlm (09/22/94, 1.200) fixes
this problem.

Problem with NETX.VLM dated 12-09-93
------------------------------------
There are known problems running Windows 95 with the Novell VLM component
file NETX.VLM that is dated 12-09-93.

First Boot-up in NETX/VLM:
---------------------------
If you run NETX/VLM, Windows 95 will reboot your computer an additional time
as part of setup.

Not able to restablish connection to NetWare Print Queue
---------------------------------------------------------
If a client is connected to a NetWare server print queue and the NetWare server
is rebooted, the client cannot reestablish the connection to the queue.  The
client then needs to be rebooted in order to reestablish this connection.

Printing from DOS box does not make it to the print queue
----------------------------------------------------------
You'll not see the job at all via print manager but you will see it queue up
and despool in PCONSULE, but nothing is printed.


OTHER THIRD PARTY NETWORKS
==========================

Banyan VINES
------------
When you first boot into Windows 95, the Banyan drivers will report that
the VINES version is not the latest.   Follow these steps:

1. Check the "Keep files on network" option.  If you do not have this
option, edit your vines.ini and remove the WindowsVersion line.
2. Click Cancel
3. Windows will restart. When it does, click Install.
4. Windows will restart again.   VINES should now be configured properly.

TCS 10-Net, Artisoft Lantastic, Beame and Whiteside BW-NFS 3.0
--------------------------------------------------------------
You cannot run any of these three networks along with
a second network.  With other supported third-party networks, you can run any
Windows 95 32-bit networking components at the same time.

Printing to network printer
---------------------------
Setting up for a printer that is shared via a 3rd party server may fail.
The workaround is to redirect lpt1: through a MS-DOS window (command prompt)
to the 3rd party share, then use printer setup for lpt1.  For example, if
a network printer is connected to "lpt1:" type:
"net use lpt1: \\servername\sharename" at a DOS command prompt (this command
depends on the network you are using; please consult the product documentation
to find out how to redirect a LPT port).  Then use printer setup in the Control Panel.
Alternatively, you can try selecting the printer from the printer folder,
choose "Map Printer Port" from the "File" menu, and select the redirection
for the dialog that is displayed.

TCS 10-NET
----------
If you are running TCS 10Net, and you encounter an error message from MS-DOS
"Invalid Drive Specification...", you need to use MS-DOS's SETVER command on
the file 10NET.COM.  (Type SETVER /? at a DOS command prompt for help
regarding this command).


NETWORK BROWSING:
=================

Empty Network Neighborhoods
---------------------------
We have seen several situations where the Network Neighborhood is empty though
you are able to connect to a network resource.  Make sure that you have allowed
the machine that you are trying to connect to, to act as a Browse Master.  Go
to the Network control panel, configuration tab, and double-click on File & Print
Sharing for Microsoft Networks.  Set the Browse Master Value to "Enabled."

Browsing Mapped Drives From The RUN command: No Path
----------------------------------------------------
If you select Start->Run->Browse and bring up the "Look In" dropdown list box,
no drive labels and UNC names are displayed after the drive letters.  This is
a known bug that will be fixed in a later beta release.


NETWORK MANAGEMENT
==================

System Policy Editor
--------------------
There are several known bugs in the Policy Editor.  The resulting behaviour
may make it difficult for you to change the registry setting back to its
prior state.  Please try out the policies on your own machine before
you try it out on your friends.

Remote Registry Service
-----------------------
Remote Registry Service is used for remotely editting the registry or
using SysMon remotely. You must do the following:
	> install on both PCs
	> both PCs are using user level security
	> both PCs are using NetBeui or IPX/SPX compatible protocol with
NetBios support enabled.
	> the PC that you wish to connect to must have remote administration
enabled and you must have priviledges (Remote Administration tab in
Passwords cpl)


NETWATCHER
----------
If you want to run Netwatcher against a remote computer that is running
user-level security (choose "User-level access control" on the Access Control
tab in the network control panel), the local computer must also be running
user-level security.  In addition, the local computer and the remote computer must
use the same type of user-level security.  This means that if the remote
computer uses a Windows NT domain or server to provide the user list, then
the local computer must use that same Windows NT domain or server to
provide its userlist.  If the remote computer is using a NetWare server
for security, then the local computer must also use that same NetWare server
for its security.

SNMP AGENT
----------
In order to allow an SNMP management console to contact a Windows 95 SNMP
Agent,you must add a setting via the System Policy Editor.  Using the
"regedit" utility, locate the key:
HKLM\System\CurrentControlSet\Services\snmp\Parameters\PermittedManagers.
Add a new string value whose name is "1" and value is the IP address of the
management console.


NETWORK BACKUP
==============

Arcada backup agent should not be installed during setup
--------------------------------------------------------
There is currently a problem with installing the Arcada backup agent during
Windows 95 setup.  If you install it during setup, you may find (post setup)
that the other components in the network control panel have been removed.
You can install the Arcada backup agent after setup, from the network
control panel, without any problems.

Arcada agent requires updated NLMs from Arcada Software
-------------------------------------------------------
In order to use the Arcada backup agent, you must (1) have Arcada's
BackupExec for NetWare (single server or enterprise edition), and (2) have
two updated Arcada NLMs.  Please contact Arcada Software to obtain the NLMs.

Cheyenne agent requires updated NLMs from Cheyenne Software
-----------------------------------------------------------
In order to use the Cheyenne backup agent, you must (1) have Cheyenne's
ARCserve for NetWare product (version 5., and (2) have two updated Cheyenne
NLMs.  Please contact Cheyenne Software to obtain the NLMs.


DIAL-UP NETWORKING (REMOTE ACCESS)
==================================

Location and Name
-----------------

Remote Access has been renamed to Dial-Up Networking and is now located under the
My Computer folder.  You can now use the Add/Remove Programs control panel to install
or remove Dial-Up Networking Components.  Several UI enhancements, interop improvements
were added.  Dial-Up Networking now supports SLIP servers, to select, open the connection
property sheet and select SLIP from the "Server Type" Dialog.


TCP/IP when using LMHOSTS
-------------------------

The directory may get set to wininst.400 in the registry
HKEY_LOCAL_MACHINE\CurrentControlSet\VXD\MSTCP\LMHostFile would be set to
C:\WINST0.400\LMHOSTS. This entry will have to be changed to use LMHOSTS using regedit.


Diconnect time
--------------

Clicking the disconnect button may not respond immediately.
It will eventually respond.


NetBIOS over IPX
----------------

IPX with Netbios will not work when bound to the DialUp driver and another
adapter. You will able to connect, but won't be able to use the protocol properly.


Quick connection/disconnection and TCP/IP
-----------------------------------------

With TCP/IP installed and bound to the Dial-up Adapter, connecting and disconnecting
immediately will result in a Fatal Exit Code in VNBT(Blue Screen).  Ordinary connections
are not affected.


Modem In Use Error and TCP/IP
-----------------------------

Intsalling with TCP/IP on a net card and TCP/IP on the DialUp Adapter
results in an erroneous error when you try to use DialUp Networking.
DialUp networking will return an error stating that the modem is in use by
another application, even though it isn't. To workaround this, add another
protocol, or add and remove the Dialup driver. Then reboot, and this should
clear up the problem.


MS Net Logon and TCP/IP
-----------------------

Installing the Dial-up Adapter with only one TCP/IP instance in the system will not
trigger a logon. To work around this, add a protocol such as IPX to the Dial-up driver.



MS Net Client and TCP/IP
------------------------

The Microsoft Network Client can not see TCP/IP servers over Dial-Up
Networking when NetBEUI and TCP/IP are bound to the Dial-up Adapter. Although
netbios servers maybe visible via a gateway.  To work around this, unbind NetBEUI
from the Dial-up adapter.


Static IP address on Dialup Adapter Binding
-------------------------------------------

Entering a static IP address on the instance of TCP/IP that is bound to the Dial-up Adapter
will cause the system to hang at boot if you attempt to logon to the network.  You can cancel
out of network logon to avoid the hang, but the preferred solution is to not enter a static IP
address and to let the Dial-up Adapter determine appropriate address through PPP negotiation.



TCP/IP
======

Please rename your HOSTS file to HOSTS.SAV when upgrading to M7 otherwise the Windows 95 setup
will overwrite your HOSTS file.



PLUG AND PLAY
=============

M6 Bugs fixed in this release:
------------------------------
- Using PCMCIA modems with Win16 applications.
- "New device found" and/or "Device conflict" messages should not appear
  any longer during setup/first boot on PCI or EISA machines. Please file
  detailed bugs if you still see these messages.
- "Safe detection" has been added to the setup process. If certain classes
  of devices such as network or CD-ROM are not detected via safe methods, you
  will be prompted to force hardware detection on these classes.
- Better "crash recovery" during detection. Please file detailed bugs if
  you still have multiple hangs during setup.

New features in this release:
-----------------------------
- PCMCIA support will be disabled by default. In order to enable Windows 95's
  32-bit Plug & Play PCMCIA support, do the following steps:
  a) Choose Settings->Control Panel from the Start menu, and start the
     PCMCIA control panel.
  b) Starting the control panel will automatically launch a wizard which
     will walk you through the process of disabling your real-mode
     PCMCIA support and enabling the Plug and Play drivers.
  c) The wizard automatically determines which drivers should be
     commented out of AUTOEXEC.BAT, CONFIG.SYS, and SYSTEM.INI. If makes
     erroneous suggestions or misses a driver which is related to PCMCIA,
     please file a detailed bug report as soon as possible, including the
     names of the drivers in question.

- The following portables have been tested with the Windows 95 PCMCIA
  drivers:
  AST Bravo, Austin, Compaq Aero, Compaq Elite, Compaq Concerto,  Compudyne,
  DEC 425SL, Gateway Handbook, Gateway Colorbook, IBM Thinkpad 750, 755, 355,
  360, 500, NEC Versa V, Versa E, UltraLite, Toshiba T4600, T4700, T4800,
  T3400, T3300, TI TravelMate, TI M-Series.
- The Compaq Aero PCMCIA floppy drive is now supported. However, the floppy
  PCMCIA card must be inserted at power on time for the drive to be
  recognized.
- Hot docking works on Plug and Play portables with docking stations.
  Docking support includes automatic switching of screen resolutions when
  docked and undocked.
- Device Manager now includes a hierarchical "tree view" of your system.
- Device Manager allows printing of system and device properties and
  resources.
- Serial port enumerator enables automatic installation of Plug and Play
  modems and mice.
- Video enumerator enables automatic installation of Plug and Play
  monitors.

Known issues in this release:
-----------------------------
- Enabling PCMCIA on some portables may cause the system to reboot. If this
  happens, remove all PCMCIA cards from your machine, restart and disable
  PCMCIA via the Device Manager tab in the System control panel. Please
  file a detailed bug report specifying the type of portable, amount of
  memory, cards installed, etc.
- There are some known repaint problems when going through the PCMCIA
  wizard. A fix will be included in the next beta release.
- The PCMCIA wizard erroneously marks the driver for the Trantor parallel
  port SCSI adapter (TSCLDR.SYS) as a PCMCIA driver, you should de-select
  the relevant line if you have this device.
- Installing  Windows 95 on a Compaq Elite over a Novell ODI net with a
  3Com 3C589 PCMCIA adapter will cause Win 95 to hang during first boot.
  The reason is the 3C589.COM ODI MLID driver is has problems. The work
  around is to boot windows with an F8 --> "Command Prompt", remove the
  following four lines from the autoexec.bat, and make a separate batch
  file (called from the end of the autoexec.bat) which executes them.
  The lines to remove from autoexec.bat are:
  C:\NWCLIENT\LSL.COM
  C:\NWCLIENT\3C589.COM
  C:\WINDOWS\ODIHELP.EXE
  SET NWLANGUAGE=ENGLISH

  The new batch file should look something like this:
  CD \NWCLIENT
  SET NWLANGUAGE=ENGLISH
  LSL.COM
  \WINDOWS\ODIHELP.EXE
  CD\
  WIN.COM


REGISTRY/REGEDIT:
=================
Known problems in this release:
-------------------------------
- If Windows determines that your registry is corrupt during boot, the
  automatic recovery feature will malfunction and you may see the following
  things:
  a) A dialog telling you that there was a problem backing up your registry,
     where the only option is OK.  This is not in fact the case.
  b) This is potentially followed by New Device Found dialogs with no device
     listed, and possibly by GP Faults if this dialog is dismissed.
  Rebooting may simply repeat the process.

  If you encounter this situation, restore your registry manually with the
  following steps:
  a) Boot to the command prompt by pressing F8 at the Starting Windows...
     prompt, followed by choosing Command-Line Start.
  b) At the C> prompt, change to your main windows directory.
  c) Use the "attrib -h -s -r *.da?" command to unhide the SYSTEM.DAT,
     USER.DAT files, as well as their backups, the SYSTEM.DA0 file and
     USER.DA0 file.
  d) Save the .DAT files in a safe location, and include them in a detailed
     bug report.
  e) Copy SYSTEM.DA0 to SYSTEM.DAT and USER.DA0 to USER.DAT. This will
     replace your corrupt registry with the intact backups.
  f) Use "attrib +h +s +r *.dat" to re-hide the registry files, and reboot
     as normal.

M6 Bugs fixed in this release:
------------------------------
- Several minor cleanups to the Regedit user interface.
- A bug was fixed which would cause Regedit to GP fault at startup on
  certain machines.  If you still encounter GP faults with the new version
  of Regedit, please be sure to file a WinBug through Compuserve.

New features in this release:
-----------------------------
- Regedit now remembers screen position and window size settings.
- Regedit now includes a search feature, which will allow you to search
  for text in Key names, Value names, or Value data.  Searching for binary
  data is not supported.
- Regedit can now be run in real-mode to import or export registry
  information to/from .REG text files. Run REGEDIT /? for syntax details.
  Note that running regedit from a Windows 95 MS-DOS box will still invoke
  the GUI editor, however.
- The real mode regedit functions additionally support complete replacement
  of the registry from a .REG file.  This is only recommended if the text
  file was a complete backup of the registry.

POWER MANAGEMENT:
==================

M6 Bugs fixed in this release:
------------------------------
- A bug causing intermittent hangs in suspend or resume operations has
  been resolved.
- Machines which used to crash at Windows 95 startup when APM features were
  enabled should work properly now.
- Several fixes to the battery meter to work around buggy status reported
  by various machines have been included.
- Screen saver power management features are no longer visible unless your
  machine has the required hardware and software interfaces.  These
  requirements are: a) An Energy Star (VESA DPMS) monitor, which is settable
  from the Monitor Settings property sheet, and b) a system BIOS supporting
  either APM 1.1 or VESA BIOS Power Management extensions.
- Some machines with appropriate APM support will now completely shut off
  when Windows is shut down. If you wish to restart instead of turning off
  power, use the Restart option instead.

Known issues:
-------------
- Some machines may not suspend "automatically" based on inactivity
  timers. However, user-initiated suspends should still work correctly. If
  you encounter this behavior please file a complete Winbug with the name
  and model of the machine, and the BIOS revision if available.
- Resuming from suspend while running in fail-safe mode may cause disk
  access problems on some machines, typically resulting in GP faults which
  require a system reboot.


FILE SYSTEM/MEMORY MANAGEMENT/DEVICE DRIVERS
============================================

Bugs fixed in this release
--------------------------
1. FLAG_NO_BUFFERING fixed to work as doc-ed in API.
2. IN-2000 adapter related bugs fixed.
3. Sony miniport bug fixes and performance tweaks.
4. Panasonic miniport bug fixes and performance tweaks.
5. Mitsumi miniport bug fixes and performance tweaks.
6. Old Future Domain BIOSes (which were buggy) now supported.
7. CD now uses sync data transfer (and async is a user option).

New features in this release
----------------------------
1. PCI IDE supported through protect mode IDE driver.
2. Protect mode driver for Mitsumi miniport.
3. Protect mode driver for Mediavision (with Trantor CD interface).
4. Protect mode driver for DblSpace and DrvSpace drives.
5. UI for manipulating adpater related settings and drive letters.
6. Virtual Memory applet updated.

Salient Issues
--------------
1. The NEC IDE CD-ROM (NEC 260GW), with firmware Rev 1.0, on
   the older Gateways needs a special switch to support data
   transfer.  Please set NonStandardATAPI=True in the [386enh]
   section of SYSTEM.INI.  Even with this switch set only data
   transfer is realiably supported.  Audio and Multisession
   support is still a problem.  NEC is writing a VSD and will
   distribute it when they are done.
2. The Panasonic miniport still has some performance problems
   that need to be addressed. An interim work around is to use
   the equivalent real mode driver.
3. If AMD SCSI is installed in your system, then you must comment
   out (i.e. put ; as the first character in) the following 5 lines
   of SYSTEM.INI to disable the AMD SCSI fastdisk driver.  Drives
   controlled by this adapter will not be accessible unless these
   lines are deleted:
   AMSIWF=TRUE
   device=AMSIWF.386
   device=AMSINT13.386
   AMSIAV=TRUE
   device=AMSIAV.386
   You can also disable FASTDISK/WIN ASPI by setting the AMSIWF and AMSIAV
   flags to FALSE.
4. The Mitsumi driver has timing related problems on very old drives and
   on P5-90 class machines.  In these cases you might get errors that are
   not valid.  A work around is to use the real mode driver.
5. The Always miniport has timing related problems on old Texel drives.
   A work around is to use the real mode drivers.  An update is expected
   to be posted on CIS.


DOUBLESPACE and DRIVESPACE
==========================
If you are already using DoubleSpace or DriveSpace, Setup will update your
D??SPACE.BIN and .SYS drivers with versions that support both DoubleSpace
and DriveSpace drives simultaneously.  If you are upgrading from MS-DOS 6.x
(not a previous version of Win95), Setup will also disable the D??SPACE.EXE,
SCANDISK.EXE and DEFRAG.EXE programs in your \DOS directory and install a
BAT file that tells you how to run the Windows 95 equivalent.  The old
versions of these files will be deleted if you upgrade a previous Windows
installation, but if you "clean install" Windows 95 to a different directory,
Setup will preserve the old versions of these files by renaming them.

Beta  2 contains the new DriveSpace disk compression application to
allow you to compress drives under Windows 95 and to manage existing
DoubleSpace and DriveSpace drives.  You'll find it on the Start menu
in the Programs\Accessories\System Tools folder.  Although DriveSpace
has already been tested extensively, we recommend that you backup your
system before using it.  Known limitations:
 - If you are already running Stacker or SuperStor, do not run the
   DriveSpace application.
 - If you are using Windows NT or OS/2, do not compress a drive containing
   any of the files required to boot them or that you need to access when
   running those OS's.
 - Compressing or uncompressing the drive containing your Windows files
   or your swap file is quite slow.  We recommend that you run these
   operations at the end of the day so that they can run overnight.
 - If your C: drive is already compressed, in order to perform certain
   compression operations (for example, resizing a drive), DriveSpace
   requires at least 1.5MB free on Drive C's host drive.  In order to create
   enough free space, delete the DRVSPACE.TMP folder from Drive C's host
   drive.  If there is no DRVSPACE.TMP, you may have to move your registry
   files to drive C from its host drive.  To do so, boot your system to
   real-mode (see the Setup section above for instructions) and copy
   SYSTEM.* (and USER.*, if present) from the \windows folder on your host
   drive to your Windows 95 folder on drive C. Then delete those files from
   your host drive and reboot.
 - If you are not already using compression, and you use DriveSpace to create
   a new, empty compressed drive, you need to manually copy the DRVSPACE.BIN
   file from your Win95\command subdirectory to the root directory of your
   boot drive.  Also, make a copy there called DBLSPACE.BIN.
 - Beta 2 is unable to mount compressed SyDos drives.  If you are using
   SyDos removable hard drives, do not attempt to compress them.
 - Automounting may not work on compressed removable drives other than A
   and B.  To mount them, run DriveSpace and choose Mount from the Advanced
   menu.
 - If you are upgrading over a previous version of Windows 95, you may see
   the message "DxxSPACE.SYS is not the same internal revision as DxxSPACE.BIN"
   if you use F4 to boot to MS-DOS 6.x.  This message is harmless, but you can
   eliminate it by replacing the D??SPACE.SYS file in your MS-DOS 6.x directory
   with the DRVSPACE.SYS file from your Win95\command directory.


DEFRAG and SCANDISK
===================
Beta 2 supports defragmenting drives compressed with DblSpace or DriveSpace,
as well as uncompressed drives.  However, in Beta 2, you cannot defragment
compressed drives that contain your Windows 95 swap file.  Known limitations:
 - The keyboard accelerators do not work.
 - The date of the last defrag shown on the Tools page of the property sheet
   for a drive is not updated when you run the Defragmenter.
 - The text that appears on some of Defrag's buttons may overflow the button.

The Windows disk checking/repair tool is now called ScanDisk. Windows ScanDisk
can now detect when a long name has been incorrectly associated with a file;
it repairs it by deleting the long name.  Windows ScanDisk can also detect
folders whose contents are partially or completely damaged.


Info Center & Microsoft Exchange
================================
The Info Center is now known as Microsoft Exchange....

What is Microsoft Exchange?
-----------------------------------------------------------------------
Microsoft Exchange enables you to send and receive Email, FAX, and documents.
It also provides rich viewing of the objects you store in it. Information
Services that can be used with this beta of Windows 95 include Microsoft Mail
version 3.0, version 3.2, Microsoft Mail postoffice, Compuserve, and
Internet. Note that a Microsoft Mail postoffice is included in this version
of Windows 95. See "Installing a Microsoft Mail postoffice" in EXCHANGE.DOC
(see "More Information" below).

Minimum Hardware Requirements
-----------------------------------------------------------
Microsoft Exchange will run on the minimum hardware platform for Windows 95.
However, it is recommended that you have at least 6 megabytes of RAM for good
performance in this Beta release.

In addition, computers using Microsoft Exchange in this release should have
at least 7 megabytes of hard disk space available for their swap file.

Using Microsoft Exchange with Schedule+ 1.0
-----------------------------------------------------------
Microsoft Exchange is designed to work with Schedule+ 2.0, which may not be
available yet. When Microsoft Exchange is installed, it will partially
disable Schedule+ 1.0. You will still be able to use Schedule+ 1.0 in
'standalone' mode, but you will not be able to send or receive meeting
requests. You will see an error message when starting Schedule+ 1.0 that
notifies you that the Mail Spooler can not be started. Click Yes to continue
here, and Schedule+ 1.0 will continue to load.

This disablement is accomplished by changing MAILSPL.EXE to MAILSPL.BAK
and by putting a StartupOffline=1 line into SCHDPLUS.INI.


Using Microsoft Exchange with Microsoft Mail
-------------------------------------------------------------
Microsoft Exchange is designed to replace the Microsoft Mail application that
comes with Windows for Workgroups and the Microsoft Mail version 3.x system.
After installing Microsoft Exchange you will no longer be able to run the
older Microsoft Mail application, but you will be able to use Microsoft
Exchange to do your Microsoft Mail messaging.


Upgrading from Windows95 M6 or later
------------------------------------------------------------
You can install Microsoft Exchange along with Windows 95 Beta 2 over an
existing Windows 95 setup. Microsoft Exchange replaces the Info Center
application of earlier versions of Windows 95. You will need to recreate your
settings for your information services, however. Additionally, there is only
partial compatibility with earlier versions of the Personal Information
Store, and NO compatibility at all with earlier versions of the Personal
Address Book. For information regarding the use of existing Personal Address
Books and Personal Information Stores, see "Creating or Using the Personal
Address Book" and "Creating or Using the Personal Information Store" in
EXCHANGE.DOC (see "More Information" below).

Using Microsoft Exchange with Windows 95 M6 Info Center
--------------------------------------------------------------
There is NO interoperability between M6 and the M7 releases.  You cannot
send/receive messages to/from mismatched Windows 95 releases.  Upgrade all M6
Info Center users simultaneously to use Microsoft Exchange.

Sending From Within Another Application Does Not Work
----------------------------------------------------------------
Simple MAPI for 16 bit applications  is not yet implemented.  You cannot use
Simple MAPI for 16 bit applications running on Windows 95 (e.g. File.Send in
MS Word running on Windows 95.)


Installation
-----------------------------------------------------------------------
Microsoft Exchange is not installed on Windows 95 by default.  To install
Microsoft Exchange , choose the Custom Setup option when running Setup.  When
you are prompted with a list of components, you must select Microsoft
Exchange from the components list. You may also wish to select and install
one of the following services that use Microsoft Exchange: Microsoft At Work
Fax, The Microsoft Network, Internet Mail Services.

You can also install Microsoft Exchange after you have complete installation
of Windows 95 by using the Add/Remove Programs control panel. Make sure that
you Shut Down and Restart Windows 95 after doing this.

When you use Microsoft Exchange for the first time, the Microsoft Exchange
Setup Wizard will be run. The Wizard is also run each time you Add a new
profile in the Microsoft Exchange Profiles control panel.

In this beta, if you configure the Microsoft Mail service using the new
profile wizard, then it will save your password in the profile, and
automatically enter it for you when you start mail.  This will mean that
someone else using your machine will be able to access your mail.  To
configure the Microsoft Mail service to ask for your password each time at
startup, go to the control panel, MS Exchange Settings, Microsoft mail, Logon
page, and uncheck the remember password check-box.

There is a known problem playing sounds when new mail arrives; this leads to
a Page Not Present fault in Kernel. Simple workaround is to open the Control
Panel and start the Sounds applet. Find the New Mail Notification sound
event, and set the sound to "(None)".


More Information
--------------------------
For more information, please see EXCHANGE.DOC in the DOCS directory of the
CD-ROM; also found in the DOCS directory of floppy disk #1.


THE MICROSOFT NETWORK
=====================

What is The Microsoft Network?
-------------------------------
The Microsoft Network is Microsoft's new online service.  We are Beta
testing this service in conjunction with the Microsoft Windows 95
operating system and welcome you to dial in and try us out.  If you
did not install The Microsoft Network when you first ran Windows 95
Setup, you can easily install it now by using the Windows Setup tab
in the Add/Remove Programs control panel.

During the Beta, all access to The Microsoft Network is free, and no
billing information will be collected.  However, you will still need
a member ID to sign in.  To get a member ID and password, choose Sign
Up For The Microsoft Network in the Accessories folder on the Start
menu.  If you have trouble viewing the legal agreement or other
documentation while running Signup, you can review this information
by opening LEGALAGR.TXT, PRODINFO.TXT, and NEWTIPS.TXT while running
WordPad.

When signing up for The Microsoft Network, you may get a message
telling you that no additional new members are being accepted.  Since
this is an early Beta release, we are adding members slowly.  If you
get this message, we strongly encourage you to try signing up again
in a week or so.  We're constantly improving the quality of our
software, and we want to have room for everyone who is participating
in this Beta test of the Windows 95 operating system and The
Microsoft Network!

The Microsoft Network is a big place, with lots of forums to visit
and bulletin boards to browse.  While some of them will have forum
managers and you'll see content (that is, BBS messages and chat
discussions) when you enter these areas, not every area will be
populated on the first day.  Please don't let this stop you from
posting a message or arranging to meet someone in a chat room.  If
you don't find something you want, keep checking; we're going to keep
growing and adding content with time.

We'd love to hear what you think of The Microsoft Network; when
you're online.  Stop by the General BBS in The Microsoft Network Beta
folder and post a message or two.  Let us know what you like, what
you want to see added, and what you couldn't live without in an
online service!

General Information
-------------------
Here's an overview of what you'll find on The Microsoft Network.
Your main window onto The Microsoft Network is Home Base.  This is
what you see when you double-click the Microsoft Network icon that
you get after signing up.  From here, you're just a mouse-click away
from Online Today, our live newsletter featuring the latest and
greatest to hit The Microsoft Network.  E-mail lets you send
confidential messages to other members.  Favorite Places is a
collection of what you like best.  Try right-clicking an icon and
choosing Add to Favorite Places.  For this Beta, Headlines (a service
that provides the latest news) is not available.

Also on Home Base you'll find hot spots that take you to Categories
and Member Services.  The Categories folder is the place from which
you can get anywhere on The Microsoft Network.  Just double-click an
icon to open a category folder, and away you go.  All the folders in
The Microsoft Network look and act like the Windows folders that hold
files.  Member Services is a folder with information about using an
online service and how to get help using The Microsoft Network.

You'll also find the following services available on The Microsoft
Network:

   Bulletin boards (BBSs).  A place to post and read messages, meet
   new people, and hold long-term discussions.

   Chat.  Talk live with people from all over the planet.  The moment
   you type something, other Beta testers will be able to read it...
   and talk back!

   Files to copy to your computer.  Copy software, pictures, and
   other files right from BBS messages to your hard disk.  If you
   have something to share, just attach it to a BBS message and post
   it for everyone to see and use!

   Kiosks.  These helpful rest stops describe what a forum is all
   about.

If you'd like to know more about using any of the specific services
on The Microsoft Network, complete documentation is available from
the Help menu in any of the above applications.

Beta Support
------------
Support for The Microsoft Network (as well as support for Online
Registration) is available right online!  You can get help from our
Beta support staff in several ways, all of which are centered around
the Beta Support folder, located in Member Services on Home Base.
Here you'll find the Known Bugs BBS, which has the updated list of
problems you should be aware of for this Beta.

Also, you can open the BBS that corresponds to the area of The
Microsoft Network in which you're interested, and you'll see a list
of the questions that the other members have already asked.  If you
don't see the answer you need, post a message by choosing New Message
from the Compose menu.  Our support staff should have an answer for
you within 24 hours.

If the answer you need is not available in one of these BBSs, try the
Live Support Chat room.  Frequently a support technician for The
Microsoft Network will be in the room.

If you're having trouble connecting to The Microsoft Network, or
cannot read or post on the support bulletin boards, we also offer a
phone number where you can leave a message.  When describing the
problem you are having, please be sure to describe the specific
symptoms, including any error messages and modem behavior,
information about your configuration (CPU, modem, phone line, and so
on) and your name and a number where you can be reached.

The Microsoft Network Support Voice Mail number is (206) 936-6947.

Having Trouble Connecting or Getting to Online Support?
-------------------------------------------------------
If you're able to dial in, but are not able to connect, you may be
trying to call in while we are performing periodic maintenance.
Although we will not take The Microsoft Network offline every day, we
will occasionally schedule work between 11:00 P.M. and 2:00 A.M., PST
(7:00 A.M.-10:00 A.M., GMT).  At other times, we may be experiencing
unanticipated outages of service.  Please be patient and try
connecting at a few different times before contacting our customer
support line directly.

When you try to log on to The Microsoft Network and you have "Large
Fonts" turned on, the Sign In window will open off screen, preventing
you from using it.  To work around this, open the Display control
panel, choose the Settings tab, and set Font size to Small Fonts.
If you have trouble connecting while trying to sign up for a Member
ID, you may be in an area which is not supported by our toll-free
Signup number.  If this happens, you will hear your modem try to
connect several times without success.  The Connection Settings
dialog will then appear. Choose the Access Numbers button, and then
use the Change button to choose both a primary and backup phone
number in your calling area.  Choosing Try Again in the Connection
Settings dialog after choosing local numbers should then work
properly.

When Signup picks phone numbers for you to dial, make sure it offers
both primary and backup access numbers.  If either is missing, choose
the appropriate Change button to fill in the blank phone number.

If you return to a BBS to read a message you have previously read,
you may discover the message no longer appears in the message list.
We are planning on adding a feature to let you choose which, if any,
messages are hidden after they have been read, but for now you must
manually restore the hidden messages. To do this, delete the file
MOSNEWS.INI from the Windows folder on your hard drive.  The next
time you enter any BBS, all messages should appear in the folder as
if never read.

Refer to the previous section for more information regarding Beta
support, including The Microsoft Network voice support line.

Reporting Bugs
--------------
If you think you've found a bug, use the WinBug® tool in the System
Tools folders in the Accessories folder to fill out a bug report.
When you complete the bug report, WinBug will summarize it in a file
in your Windows folder. WinBug will name the file after your Beta ID,
followed by a three digit number (for example, 123456.001).  You can
send your bug report to us in one of two ways:

1   Compose a mail message in Microsoft Exchange, attach the bug
    report file, and send it to the MOSBUGS member ID.

2   Post a message in the Bug Reports BBS located in the Member
    Services folder, with your bug report attached to the message.


APPLICATION COMPATIBILITY
===============================

Re-installing Applications
--------------------------
If you upgrade your existing Windows 3.1 or Windows for Workgroups
directory to Windows 95 then you do not need to re-install your applications.
If you install Windows 95 to a new directory then you must re-install all of
your Windows-based applications.  Hand copying files from your Windows 3.1
directory to Windows 95 is not sufficient.

Shortcuts created before this release may not have the working directory
set correctly.  You must manually set the current directory.  Any shortcut
created with M7 will have this field correctly updated.

Microsoft Office - Running Microsoft Office's Setup to uninstall any installed office components will likely make your shell unusable as a result of improper registry entries and will require you to reinstall Win95. Please refrain from using Office uninstall.

Microsoft Delta - you will need to copy the DELTABAT.PIF file to the
\WINDOWS\PIF directory in order to run MS Delta.

When running Peachtree 2.0, you may see a message saying you need to upgrade.
This message is incorrect. Peachtree 2.0 will function properly on Win95.
However Peachtree 3.0 does not function properly on Win95.

Wordscan Plus 3.0 may not work with this interum release of Windows 95.

SuperStor compression not fully supported in M7.  You may not be able to get
SuperStor compression working with Windows 95 M7.  If you do decide to
install over SuperStor please report any problems you encounter.

The DOS version of Colorado Backup may not function correctly with this
release.  The Windows version is working so please report any problems you
find.

F4 function with Stacker Ver2.0 & 3.0 and other compression software may not
function in this release.

If you compress your Windows 95 volume using the Stacker DOS compression
program your desktop shortcuts will need to be manually repaired.  You will
also need to move USER.DAT & USER.DA0 from the host volume to your compressed
volume.

AD-DOS.COM in AUTOEXEC.BAT prevents Windows from loading the After Dark TSR
because AD-DOS.COM stops Winboot.sys from calling Win.com.  User can
type Win at the command prompt to continue executing Windows 95.

Quick Time for Windows will cause a general protection fault that will hang
the system.  A work-around is to add the below settings to QTW.INI:
	[video]
	Optimize=BMP

Embedded TrueType Fonts Not Displayed - Microsoft PowerPoint can embed
TrueType fonts in a saved document so that your presentation can use those
fonts even if they are not installed on the another system.

PowerPoint 3.0 may GPF if you hit the F1 key without a presentation loaded.

Disk Utilities - Direct disk writes using Int 26h or Int 13h will fail under
Windows 95.  This is necessary in a multi-tasking environment to prevent disk
corruption from multiple utilities running simultaneously.  The LOCK and
UNLOCK commands can be used as work-arounds for DOS utilities.

Adobe Acrobat 1.0 - Acrobat 1.0 is not functioning correctly on this Windows 95
release.  The problems are understood.  You should upgrade to Acrobat 2.0.

Microsoft Visual C++ v1.5 - A duplicate VMCPD entry will be placed in your
SYSTEM.INI file during installation.  You should remove this entry to avoid
Windows 95 error messages while booting.

Symantec C++ version 6.1 Profession Will Not Run Under Windows 95.  Symantec
Norton Utilities version 8.0 Will Not Run Under Windows 95.  These applications
will refuse to run under any version of Windows except for 3.1.

Flight Simulator Unable to Lock XMS Memory with SoundBlaster - Microsoft
Flight Simulator 5.0 crashes with a message "Unable to lock XMS memory-fatal
error" if you try to run it with a SoundBlaster/SoundBlaster Pro card.  To
work-around, run the Flight Simulator setup program.  On the sound board
settings screen, choose 'NO' for the 'Use XMS Memory' option.

Flight Simulator Hangs when Changing Speaker Settings.  If you change your PC
speaker settings after you set up Microsoft Flight Simulator 5.0, you may
hang your system.  Select the PC speaker during Flight Simulator setup.

Microsoft Publisher - Currently, changes to text in a text frame
(font, font size, bold, etc) do not occur until the mouse is clicked.  As long
as the mouse click occurs in the 'document design area' (ie. not on a menu or
button bar), changes will take place.

cc:Mail installation (the MS-DOS  version) fails from inside a Windows 95 VM.  cc:Mail should be
setup using single MS-DOS application mode.

Virus utilities can detect but not clean viruses from Windows 95.  This is
because virus utilities use low-level writes to repair the disk.  MS-DOS
based utilities can still be run using the LOCK command.  Windows virus
utilities can only repair viruses if you setup Windows 95 in Windows 3.1
Compatibility mode (no long file names).

Do not install IronClad on the same machine as Windows 95.  The security
features of Ironclad create numerous problems for Windows 95.

Dashboard may cause Windows 95 to hang or spontaneously shut down.

Laplink Pro - Running Laplink Pro 4.0a with a communication rate of >9600
may not recognize other connected machines.  If you have this problem change
the communication rate to 9600.

Xtree Gold 2.51 for Dos - Xtree Gold cannot PRUNE directories.  The
work-around is to manually delete the directory structure using another VM.
Also, Xtree Gold may see long filename components while running in real mode.

Timeline 1.0 for Windows- The tutorial for Timeline is not functioning with
this release.

Visio 3.0 may fault in VISIOLIB.DLL - After extended use, Visio may fault
in the before mentioned DLL.  Please do not bug this issue.  This will be
fixed for subsequent releases.

Stacker 2/3.0  - Following the manual compress in place of a working
Windows '95 system (running the LFNBK utility in protected mode prior to
compressing the drive), the user may be required to hand copy the USER.DA?
files (SYSTEM, HIDDEN, READ-ONLY) from the mirrored <HOSTdrv:>\windows
directory to C:\windows (CVF) before Win95 can startup properly.

MS-DOS based gaming - There are many known issues with MS-DOS-based games
not restoring video properly following task-switching from working MS-DOS
boxes (ALT+TAB, ALT-ENTER, Tray switching to open programs, etc.). The work
around is to run such games full-screen.  Please do bug these issues,
however, so that we can keep track of such games.  Also, check with the
SYSOP to see if any video driver updates have been posted (instructions
will be posted as to how to load such drivers).

MSVC 1.5 and 1.51 - There is a known issue dealing w/using MSVC's debugger
and hanging.  We have a fix for this, and it will be posted on CIS by the
SYSOP.  Please keep up on the forum to catch this general posting.

Compaq Computer Setup for Windows - This application generates error
messages that it can not find MAIN.CPL.  You may ignore these errors.  You
will not have direct pushbutton access to the Control Panel from CSW.
Use the Start - Settings - Control Panel to access these settings under
Windows 95.

Compaq Multimedia Presarios (520CDS, 720CDS and 920CDS) Media Pilot
applications do all function 100%.  On startup, the system will generate an
error message that begins "CPQDMN2:" and says that MSMIXMGR can
not be found.  This will be fixed in the future.  Meanwhile, the native
Media Pilot "Mixer functions" will not be available (CD, Midi, etc.).  Use
the Microsoft Mixer and Sound Volume app to set the levels (Start, Programs,
Accessories, Multimedia).  The Microsoft and Media Pilot CD Players also may
interact improperly.  The Address Book in the Telepone does not work.  These
problems will be addressed in a future build.

MSDN Release 9 and the 32-bit viewer (MSIN32.EXE) - MSDN Release 9's 32-bit
viewer (MSIN32.EXE) will cause MSDN to page fault.  During the install
process, you must choose the 16-bit viewer during the install process for
this release to work.

Colorado backup for MS-DOS - Currently, Windows 95 does not have support for
the MS-DOS side of Colorado's MS-DOS backup tools.  To backup your data, you
must either use Colorado's Windows-based equivalents, or use the backup
utility that is included with Windows '95.

APPLETS
=======

WordPad
------
Wordpad may not read very complex WinWord 6 documents correctly.  If you
have such a document, please submit the document with your bug report so
we can get these problems identified and fixed.

Wordpad objects can not be inserted into a 16 bit OLE client.

If you try to open a WRI file directly off a CD using Wordpad, it may take
minutes to open.  This a known problem with the WRI converter.

MSPaint
-------
Problems with rotating, skewing, stretching images and selections.

Problems with the vertical scroll bar when in a zoomed out mode.

Repaint problems with the curve tool when placing a curve.

Drag and drop of a selection into an OLE client doesn't work.

Support for the PCX file format has been dropped.

Toolbar, Colorbar, and status bars may not work correctly when an
MSPaint object is activated inside an OLE client.

File Viewers
------------
Several new features have been added to file viewers.  Once a file
viewer window is open, you can drag and drop files from the cabinet
onto the viewer window.  The View/Reuse Window command
controls whether a new window is created.

Support for viewing PowerPoint 4, Excel 5 charts, 1-2-3 for
Windows charts, and Quattro Pro for MS-DOS ver 5 spreadsheets has been
added.  Note this version will not work when viewing PowerPoint 3 files.

Support for viewing technical information about EXE and DLL files has
been dropped because few users feel this information is useful.  The
file version information has been added to the property sheets for these
types of files.

There are still a few problems with viewing complex PowerPoint 4 files.
If you have such a file, please send it to us with your bug report so
we can reproduce the problem here.

Backup
-------
-- The Backup applet does not work with SCSI tape devices.  It only works
   with QIC-40/80/3010/3020 tape hardware connected to the primary floppy
   disk controller or the Colorado Memory Systems tape hardware which
   uses the parallel port.  The Backup applet will correctly format and
   make use of all the capacity on the new "QIC 350" tapes which just
   came out.

-- When a backup or restore or compare operation is completed, an error
   log is created in the LOG subdirectory under your Windows 95 directory.
   Please look in this location for the error log.  The error log is a
   simple text file.  Also, if you submit a bug, Please include this
   error.log file with your bug report.

-- This version of backup will NOT work with file sets or backup sets
   created by older versions of the Windows 95 Backup applet.  To re-use
   a tape, erase the tape.

-- Many people have submitted bugs for "out of memory" conditions.  We
   have a fix for these, but this fix did not make it into the Beta 2
   release.  If you encounter such a problem, please look on the Compu-
   Serve forum, in the Applets section, for information about a new
   version of the Win 95 Backup applet which corrects these memory issues.
   Please ensure the fixed version works for your particular hardware/
   software configuration if you encounter this problem with the Beta
   2 version of Backup.

-- The backup format is subject to change between now and when Windows 95
   is released so do not store any important information in backup sets
   as it may not be readable in a later version of Windows 95.

-- If you submit a bug related to using tape backup, be sure and note the
   type of tape drive you have in the bug report.  90% of our bug reports
   do not have this information, which really, really slows us down (!!)

-- The last action during a backup is to set the archive bit on all the
   files which were backed up.  If a file is open or otherwise write
   protected, an error in the errorlog.txt file will be generated.
   If you try to backup your Win95 directory, you will typically get
   between 40-60 errors because of this.  This is a bug and will be
   fixed in a later release.

-- Many tape format bugs have been fixed.  Those remaining seem to be
   related to not being able to format a tape which has been formatted
   with a Win 3.1 backup application such as Norton Backup for Windows.
   If you encounter a different kind of tape format problem, please
   submit a bug report.

-- If you backup an open file (no access type of open) and the file changes
   size between the time when the file is selected and when it is actually
   written out to tape/floppy/disk, it will not be possible to restore all
   the files written out after this one file.  Note that the error.log
   file will be open when you perform a backup/restore/compare operation.

-- The "Always Format" option for tapes is not working correctly.  It will
   only format the tape if the tape is unformatted.  This is a known bug.

-- To enable compression when doing a backup, look at settings/options/
   backup dialog.  Compression is only available when backing up to floppy
   or tape.  This will be corrected.

-- if you need more than one tape for a large backup and you insert an
   unformatted tape for the second or higher tape in the sequence, the
   backup applet will fail, even if always format tape option is enabled.


MS-DOS Editor
-------------
If the root directory of the current drive is read only, the editor will
fail with a message saying it can not create a temp file.


HyperTerminal
-------------------
Known problems: (all of these problems will be addressed in future releases)
+ If you do not have a modem installed HyperTerminal recognizes this and
  asks you if you would like to install a modem.  When you have completed
  the installation and return to HyperTerminal it does not recognize that
  you have installed a modem and will prompt you to do so again.   The
  work around is to either install a modem before running HyperTerminal,
  or if you install a modem when prompted to do so by HyperTerminal then
  you will need to close and restart HyperTerminal before the modem will
  be recognized.
+ Sometimes auto detection does not work properly.  If you run into this
  you can manually set your data bits, parity, and stop bits via Configure
  button on the Phone number tab under File/Properties for your connection.
+ The data bits, parity, and stop bits are not being saved, so if you customize 
     your connection you will need to do this each time.

WinPad
------
WINPAD DATA FILES: WINPAD.DAT and WINPAD.IDX are data files that store
information entered in any of the WinPad applications as well as WinPad's
online Help. Deleting these files removes all stored information as well
as online Help.

REINSTALLING Windows 95: If you need to reinstall this release of the Windows 95
beta, follow these steps after running Setup in order to restore the existing
data from your previous WINPAD.DAT file:
1. Copy WINPAD.BAK to WINPAD.DAT
2. Delete WINPAD.IDX

EXITING WINPAD AND Windows 95: Exiting Windows 95 before you exit WinPad can cause
your system to hang. Therefore, always close WinPad before closing Windows 95.
This problem will be fixed in later versions of the product.

Keyboard Support has been enhanced, but is not fully implemented in this
release.

Briefcase
----------
The Briefcase database has changed.  As a result, if you were using a
Briefcase in an earlier beta and you try and open it, you will get an error
message that your briefcase database is corrupt.  Choose OK in this dialog
and when the Briefcase opens, simply delete the corrupted database which now
shows up as a file in the Briefcase.


DISPLAY
=======

New Display Features since Beta 1
---------------------------------
Nearly all display types supported by Win95 have received
performance increases since beta 1.  In particular, there
is now a much faster Cirrus Logic driver for adapters which
use CL5429/5434 accelerators.

32 bit-per-pixel modes are now supported on several major
display types.  16 bit (5-6-5) modes are also new.

ATI mach64 adapters, incl. ISA versions, are now detected
by Setup and supported with an integrated driver.

Chips & Technologies accelerators are supported with a new driver.
If you have one of these adapters (e.g. "Wingine DGX"), make
sure your display type is set to "Chips & Tech. Accelerator"
in the Display control panel Settings page.

Win95 automatically changes display driver & settings for
docked and undocked configurations of laptop PCs.  In order
to dynamically change resolution when warm/hot docking, both
configurations must be set to the same color palette, 256 colors
or higher.

Display adapter changes are automatically detected.  When this
happens, Win95 will start in VGA mode, and either automatically
configure a new PCI adapter, or ask you to run the Hardware
Installation Wizard to detect & install a new ISA or VLB adapter.


General Display Notes
---------------------
"Safe mode":  If you encounter display problems, try adding
"SafeMode=1" (faster) or "SafeMode=2" (safer) to the [windows]
section of WIN.INI.  This disables some or all hardware
acceleration.  When submitting bug reports for display related
problems, pls indicate whether the problem is fixed by using
"safe mode".

Cursor problems:  A general workaround for cursor problems is
to enable Mouse Trails in the Mouse control panel.  Use the
shortest length if you don't like an actual trail to appear.

SYSTEM.INI, [boot] now always contains "display.drv=pnpdrvr.drv"
if a Windows 95 version display driver is installed.  The actual
driver (e.g. VGA.DRV, S3.DRV, etc.) is now loaded from the registry,
to support dockable PCs w/ different adapters in laptop vs. docking
station.

Refresh Rates:  Setting monitor type in the Win95 Display control
panel does not currently affect the refresh rate output by your
display adapter.  To change this, you must run a utility supplied
by your display adapter or PC manufacturer. Some must be run
in AUTOEXEC.BAT, while on some PCs, monitor type is set in
BIOS configuration programs.  Examples of utilities from
adapter manufacturers include:

	ATI                     INSTALL.EXE
	Cirrus Logic            MONTYPE.EXE, CLMODE.EXE
	Diamond Stealth         STLMODE.EXE
	Tseng Labs              VMODE.EXE
	Western Digital         VGAMODE.EXE

ATI mach8/32/64:  Your adapter must be configured correctly using
the ATI install.exe program for Win95 to be able to use high-res
modes properly.  Correct setting of your monitor type is especially
important.  Otherwise, high-resolution modes may not be available
for selection, or your computer may crash attempting to switch to
them.

Compaq QVision 2000:  These adapters use the Matrox MGA controller.
See the notes below for Matrox MGA.

Diamond Viper:  Setup will preserve Win3.1 drivers for this adapter
when run from Win3.1.  Running Setup from MS-DOS will cause the
Win95 VGA driver to be installed.  If this happens, use the the
Diamond Viper setup program to install Win3.1 drivers into
Win95.  Then, copy the latest Viper files from the \Drivers
directory on the Win95 CD, or download them from the WINBTU
library on Compuserve.

IBM ThinkPad:  Versions of these laptops which use Western Digital
display controllers require IBM's VESA driver to be loaded from
AUTOEXEC.BAT in order for 256 color and high-resolution modes to be
supported by the Win95 Western Digital display driver.

Matrox MGA:  These adapters are not currently supported with Win95
drivers.  The VGA driver will be installed by Setup.  However, early
versions of a Win95 MGA driver are available in the Win95 CD-ROM
\Drivers directory, or can be downloaded from the WINBTU library
on Compuserve.

* S3: The S3 driver has a number of switches that can be
used to work around the following problems.  These can be
added to the [display] section of SYSTEM.INI:

  Refresh_Rate=56       ; or 60, 72, 75

	Corrects monitor synchronization or refresh
	rate problems.

  SWCursor=1

	Corrects problems with the mouse pointer
	by disabling the hardware cursor.

  HighColor=15

	Corrects incorrect color problems at 16 bits
	per pixel by using a 555 format pixel mode.

  MMIO=0

	May correct misc. crashes on some adapters
	by disabling memory-mapped I/O.


Known Display Problems in Beta 2
--------------------------------
Please do not submit bug reports about the following known problems.
However, do watch for updates to the WINBTU forum for these.  If
an update does not correct the problem, pls do submit a report.

* ATI mach64:  A crash may occur changing resolution if animated
cursors are enabled in the Mouse control panel.  Also, the screen
may go blank if you change Background or Appearance settings
in the Display control panel when running with 256 colors.

* Cirrus Logic:  The mouse pointer is not visible on some color
laptops.  Workaround: Use a color cursor in the Mouse control panel,
or enable Mouse Trails.

* Compaq Advanced VGA (AVGA):  Screen corruption can occur switching
between Windows and full screen MS-DOS applications when in 256
color mode.

* Compaq QVision:  If Win95 is not centered on your monitor, or there
is a problem with the refresh rate, edit the COMPAQ.INI file in
your Windows directory to set your monitor type.  Change the
"Monitor=" line in the [Display] section to one of the section
names in the CPQMON.INI file, e.g. "Monitor=NEC-4FG".  This will
be done automatically in a future release.

* Compaq QVision: Running Windows applications from a full-screen
MS-DOS prompt may cause screen corruption.  Put the prompt into
a window first, then enter the command line for the Windows program.


MODEM COMMUNICATIONS
====================

New features since Beta 1
-------------------------
The status of modem connections made by Remote Access and
HyperTerminal is displayed by an icon in the notification
area of the Win95 Taskbar.  Double-click the icon to
see a dialog with information about the connection.

All AT command modems are now detected by the Modems control
panel.  In this release, some are detected as generic modems.
When this happens, you can use the "Change" button to search
for an exact match in the list of supported modems.

If your modem is detected as type "Generic", please find the
MODEMDET.TXT file in your Windows directory, and e-mail this
to modemdet@microsoft.com.  Edit the file to include the *precise*
make and model name for your modem as it appears on the modem's
packaging and user manual, and use this as the subject line of
the message.  This will help us improve Win95 modem detection.

Known Modem Problems
--------------------
A "Port Settings" button appears on in the properties dialog
for PCMCIA modems, but does nothing when clicked.  This
button is not supposed to appear, it will be removed in
future releases.  Please do not submit bug reports for this.

General Modem Notes
-------------------
When reporting a bug concerning a problem using a modem
with HyperTerminal, Dialer, or Remote Access, please include
the MODEMLOG.TXT file.  This is created in the Windows directory
when "Record a log file" is checked in the modem's properties
dialog, located in the Advanced dialog on the "Connection" page.

Telephony
=========

Significant changes for Beta 2:

- The "Dialing Properties" and "Phone Dialer" user interfaces
have been significantly updated.

- You can now specify Tone/Pulse dialing default and Cancel
Call Waiting commands on a per-location basis.

- The Telephony control panel is not copied onto your system
unless you are upgrading over a system that already had it,
since it is not needed for the Telephony service provider built
into Windows 95. If you need it to use with another Telephony
service provider, it is available in the Windows 95 SDK and on
CompuServe.


MOUSE
=======

New Mouse Features since Beta 1
-------------------------------
Color/animated cursors:  You can set your mouse pointer
to appear as a colorful waving Windows flag.  Enable
this in the Mouse control panel, Pointer page.  Watch
for additional .ANI cursor files in future releases.
System requirements:
- Win95 display driver at 256 colors or higher
  (not supported on ATI Ultra (mach8))
- 32-bit Paging (check System control panel, Performance)

Logitech MouseWare is supported.  Make sure your mouse
type is set to one of the Logitech types in the Mouse
control panel.


Known problems
--------------
Error messages about POINTER.EXE at startup occur if your
mouse is not correctly detected.  To fix this, use the
Mouse control panel to change to the appropriate Microsoft
mouse type, which will allow POINTER.EXE to load and support
its Mouse Manager features.

Microsoft IntelliPoint 1.0:  Some features may not work in
Win95 Beta 2, incl. Orientation, Appearance, and Special
Effects.

Printing
========

Printing with WinFax Pro 4.0 from Microsoft Word for Windows 6.0
----------------------------------------------------------------
You may have problems when printing to WinFax Pro 4.0 from
WinWord 6.0. This will be corrected in a future release. To
workaround this problem, start WinFax Pro 4.0 before attempting
to print from WinWord 6.0.

Printing with WordPerfect 6.0 for Windows
-----------------------------------------
You may have problems when printing from WordPerfect for Windows
when using a WordPerfect supplied printer driver. This will be
corrected in a future release. To workaround this problem, install
any Windows 95 printer driver to the same port the WordPerfect driver
is refering to.

Bi-directional Functionality with a HP LaserJet 4 or 4M
-------------------------------------------------------
In order for the bi-directional port monitor for the HP LaserJet 4
or LaserJet 4M (note this is for these specific models only, other
LaserJet 4 family printers are not effected), you must set the
"Friendly" name of the printer to "HP LaserJet 4". This will
be corrected & no longer required in a future release.

Windows Printing System & other "At Work" printers
--------------------------------------------------
When using the Windows Printing System or other "Microsoft
At Work" devices (such as the Lexmark WinWriter family),
you should not attempt to set the spooling option for
"Direct" printing in this release, and should also avoid
requesting the self test page upon intial installation.
After installation of an "At Work" printer driver, you
should also go to the printers folder to force initialization
to occur correctly. All of these problems will be corrected
in the next release.

Redirecting local ports to network connections
----------------------------------------------
Upgrading redirected printer port from Windows 3.1x is not functional
in this release. This will be corrected in the next release.

To establish redirection (Capturing) a local port to a network connection,
perform the following steps:
1) Select the printer from the printer folder
2) Choose "Map Printer Port" from the "File" menu
3) Select the redirection from the dialog that is displayed.

Printers not being shared after restarting Windows 95
-----------------------------------------------------
In cases where a printer is shared, and an attribute of the
printer is changed, the printer will not be shared upon the next
Windows 95 session until the printers folder is opened or a job
is printed from the local machine. Note this is only for the very
first session after the printer settings have been changed,
subsequent sessions will allow the printer to be shared as expected.
This problem will be corrected in the next release.

HP DeskJet Printer Driver for Color DeskJet Models
--------------------------------------------------
You may choose to install the Hewlett-Packard supplied printer driver
for the HP Color DeskJet models if desired. This may be done by 
follwing the normal instructions for installing a printer, and on the
screen which lists the manufacturers, choose "Have Disk", and refer
to the \drivers\printers\deskjetc directory from the M7 CD. If
you have the floppy disk version of M7, you may download these files
from the WINBTU forum on Compuserve.

NEW OLE LIBRARIES
=================
Beta 2 contains a new pagetuned version of the OLE2 libraries, which means
that the released versions of any 32-bit OLE2-enabled applications (such
as Microsoft Word or Microsoft Excel) will run faster that previous builds
in 8MB.


CONTROL PANEL
============
Regional Settings: Selecting a locale has no effect until you reboot
the system.

System Control Panel (or Properties of My Computer)
----------------------------------------------------------------
Problems with the Virtual Memory dialogue from the preformance tab:
It may incorrectly list volumes that are compressed through a real mode compression driver or may not list any volumes at all.  Windows 95 will not page virtual memory through real mode compression drivers.  So you should avoid selecting compressed volumes unless you are sure they are compressed through a protect-mode compression driver


PEN WINDOWS
===================
For this beta, we recommend that pen users run in 640 x 480 16-color mode.
After setup with certain configurations, you may need to use the Control
Panel to explicitly select that display mode. (Display problems may occur at
higher resolutions with some video cards.) Whenever you change display
resolution, you will need to reboot the machine in order for ink to be
displayed properly.

Simultaneous use of pen and mouse is not supported on the Compaq Concerto.

Pressure pens are not supported in this beta.

The Writing Window application and the standalone on-screen keyboard can not
be used to send characters to 32-bit applications in this release.

Users of previous pen DDK betas who do not do a clean install of this beta
may have old information in their registry for the following tablets or pen
machines:  Calcomp 33070, Calcomp 31120, Calcomp 31090, IBM Thinkpad, Wacom
PL-100V, Wacom UD-0608-R.  If this information is present, we recommend
removing it manually using the regedit application.  The current sample pen
driver supports Wacom HD-648A, Wacom SD-510C, Grid 2260, and Compaq Concerto.

The pendrv.reg file is included as an example of how pen driver developers
can place new pen driver registry information into the Windows 95 registry.
It contains default configuration information for the 4 supported pen
devices.  Any additional (or replacement) device information should be placed
here or modeled after this .reg file.  To manually enter this information
into the Windows 95 registry, use the Regedit command line "regedit /s
pendrv.reg" or use the Registry.Import Registry Info menu option of
regedit.exe.  PENDRV.INF provides a method to have Windows 95 place this
information in the registry at setup time.

Pen DDK users should consult pendrv.h for additional pen driver messages not
found in penwin.h.


ACCESSIBILITY
=============
The following notes describe features designed for users with disabilities.

New features in this release:
-----------------------------
1. Accessibility features can now be adjusted using the Accessibility icon
in Control Panel, which has on-line help.
2. A new accessibility status indicator can be displayed on the task bar or
as a floating window.
3. The SerialKeys feature now functions to allow alternative input devices
to simulate keyboard and mouse input.
4. The High Contrast feature now allows you to switch between normal and
high contrast color schemes.

New bugs since M6
-----------------
1. SerialKeys will cause a system errors when turned off, and you can't
change settings once the feature is turned on.
2. Appearance schemes chosen using the High Contrast feature are not always
reflected in the Display property sheet, the Accessibility Status
indicator, and the system Start button.

SDK/DDK
======
Installing on NT
-----------------
The SDK and DDK setup programs will not install on NT.  You can simply tree
copy the kit from the CD to your hard disk.  This will be fixed in the next
version of the SDK and DDK.

Win32 tools
-----------
The Release of MSVC++ 2.0 that shipped with Beta 1 cannot be used with this
build of Windows 95--you will have to install the shipping version of
MSVC++ 2.0.  One file will need to be replaced.  The file be found
on the CD in the \SDK\VCPATCH directory.

Note that when you upgrade to this version of Windows 95, the setup program
places the MFC dll's in the Windows\system directory and marks them  read
only.  This is to prevent these dll's being overwritten by the MSVC++ 2.0
setup program.  The dll's which come with this version of Win 95 are
binary compatible with the shipping version of MSVC++ 2.0.

Building and running 32bit apps
-------------------------------
Due to the number of changes in Win95 since Beta 1 it will be necessary to
rebuild any applications that you built with the Beta 1 kit.  If you use the
exception handling verbs "try" and "accept" your code will not compile
with this release.  You will need  to prepend double underscores to these
verbs.

What is new in this release
--------------------------
We are pleased to announce that this release of the Windows 95 SDK
includes a new setup toolkit.  The new toolkit, InstallSHIELD SDK Edition
was designed by the Microsoft Windows 95 SDK Group and Stirling
Technologies.  It has been licensed from Stirling Technologies.
InstallSHIELD has been proven for many years by thousands of ISVs worldwide.
The SDK Edition is a subset of the InstallSHIELD Professional Edition for
Windows 95. We are excited to be working with Stirling on InstallSHIELD.
To install InstallSHIELD SDK Edition you will need to run SETUP.EXE from the
\SDK\ISHIELD\DISK1 directory. Please be sure to read the README.TXT and
other documentation files installed on your system.


We have also included new common controls hosted on NT 3.5.  Please note that
these DLLS, like all beta software, are not redistributable.

Win16 tools
-----------
If you are using MSVC 1.5 for your 16bit development tools on Windows 95 you
will need to update the linker.  The linker can be found in the \SDK\VCPATCH
directory on this CD.

New Printer Commdlgs
--------------------
Note there are new printer commdlgs in the SDK.  These include an
updated File/Print and File/Page Setup dialogs.

Known Problems in the SDK
-------------------------
MultiMedia Samples
The MultiMedia samples BATCH and MIDIPLYR are not functional in this release.
We will update in the next release.

ICMTEST
The SDK sample APP ICMTEST includes 6 *.IBM bitmaps. The application will
fail to initialize these images because the bitmaps are in a format
unrecognized by GDI.  These warnings are harmless and use of the rest of
the bitmaps will be unaffected.

Windows Help Compiler
Choosing the Generate Full Text Search Index File option (under the FTS
tab in Options) without choosing the Maximum Compression or Customize
option (under the Compression tab in Options) will cause a GP fault. Turn on
Maximum Compression to avoid this problem.

Async 32bit playsound
There is a problem in Kernel32 that gets triggered by async 32-bit playsound
calls that will cause a page fault.

InstallShield SDK Edition
The PACKLST.EXE may if it encounters any zero lenght files.  If you run into
this problem pad zero lenght files additional comments to change the size of
the file. This problem will be corrected soon and a new version of PACKLST.EXE will be
posted on WinDEV when it is available.

Also This version of InstallShield will not run on NT.  We will fix this soon.

Wizard Sample
The Wizard sample has errors in the make file that make it impossible to build
from the command line.  The make file works fine from the VC++ 2.0 command line.
We will fix this in the next release of Win95.

DDK-Building VxDs
=================
 To build VxDs with this version of the DDK it is necessary to pick up
  some updates for your tools.  Here are the steps:

   1.  Copy linker files (link.*) from the \DDK\MSVC20 directory to your
   MSVC20\BIN directory. This is an updated version of the MSVC20 linker
   which  includes special enhancements for building VxDs. This linker will
   replace LINK386.EXE.

   2.  Copy the contents of the MASM611C directory into your MASM6x\BIN
   directory.  These fixes will be included in the next release of MASM 6.X.

If you have not built VxDs with the new linker yet, consult \DDK\BASE\VC2CONV.TXT
for instructions on modifying your make files.

Debugging VxDs with WinICE
==========================
VxDs built with MSVC 2.0 and LINK.EXE can be successfully debugged at source
level using the following tools and procedures.

MSVC 2.0 compiler
LINK.EXE version 2.50.4286 or greater (in M7 DDK)
ML version 6.11C (in M7 ddk)

   The assembly language modules must be built as coff objs and the MAP switch
   must be used for DEBUGTYPE when linking.  The following switches were used:

       ML -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 -DDEBLEVEL=1 -DDEBUG

       CL -Zdp -Gs -c -DIS_32 -Zl -DDEBLEVEL=1 -DDEBUG

       LINK -VXD -DEBUG -DEBUGTYPE:MAP

   You will need to pickup a new version of MSYM.EXE off the DDK CD.  It will
   not be installed automatically.

   
WinHelp
=======
Known Problems:
+ Bubble help popup does not come up in the correct location.
+ fails to display Windows 95 SDK help from Dolphin
+ ALink macro duplicates topic titles
+ If a .CNT file containing a :Base command is :included in another .CNT file,
  then none of the jumps that don't specify a filename will work
+ WinHelp: on-top status doesn't stick accross window jumps
+ Under a low memory condition, the 'What's this?' option comes up, but
  clicking on it just makes it go away instead of bringing up popup, a
  message indicating why the help can't come up needs to be added.
+ Context sensitive help for printer prop "Not Selected" is incomplete
+ Incorrect popup help message on printer property "printer memory tracking"
  item
+ Using context sensitive help in the 'Advanced Options' window of Control
  Panel 'System' applet will hang your system; do not use any context
  sensitive help in this window.
+ If you go to 'What's new' window through the Welcome screen, you should not
  attempt to use the index tab.


REGIONAL SETTINGS
=================

Known Problems:
+ If the regional setting of: English (South Africa), Afrikanns, Dutch
  (Belgian), or French (Belgian) are selected at setup time, your DOS country
  settings will be incorrect when you do a directory.  This bug has been
  fixed for the localized versions.


Keyboard Layouts
================

Known Problems:
+ Do not install the French (Belgian) keyboard layout.  This layout
  may not repsond or could cause a GP fault.  This bug has been fixed in the
  localized versions of Windows 95.

