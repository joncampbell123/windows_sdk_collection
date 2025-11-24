                               B C T S H E L L
                               ---------------

Version 206 - October 1994

By: Adrian J. Oney
For: Windows 95 BCT tests

Release Notes:
1. Still making bug fixes, enhancements...

Design:
  BCTShell serves two purposes: One, automation of testing among different
types of drives and devices. Two, testing of windows's memory paging
functionality. 

Overview:
  The following short segment of BCTSHELL.INI will help show how things are
arranged:

[Suites]
Disk13=Disk13Write, Disk13Read

[Disk13Write]
Return=DOS
%7=LWriteDrive
%8=NWriteDrive
%9=ProgramDrive
cmdline=%9\disk13.exe %7: time write %9\dtw%8.log hct

[Disk13Read]
Return=HCT
%7=LReadDrive
%8=NReadDrive
%9=ProgramDrive
cmdline=%9\disk13.exe %7: time verify %9\dtv%8.log hct

  The suite Disk13 contains two variations, the Disk13Read variation and
the Disk13Write test. The keyname "Return" listed for each test variation
identifies the test as returning DOS or HCT error codes. This is neccessary
for interpretation of the results of the test. This setting (and all others)
can be changed from the GUI tool screens.
  Looking at the Disk13Read variation, the commandline references three
parameters, %7, %8, %9. The keyname %9 is tied to the ProgramDrive
identifier. At runtime, it will be filled in with something like
'C:\CT\BCT'. %7 and %8 mention the drive letter and two-digit drive-number
(respectively) for the drive-profile 'ReadDrive'. What is the ReadDrive
Profile. Here is the WriteDrive Profile:

[WriteDrive]
HardDrive=Yes
FloppyDrive=Yes
CDROM=Yes
RamDrive=Yes
TapeDrive=Yes
MagnetoOptical=Yes
Drive3.5=Yes
Drive5.25=Yes
Drive8=Yes
MediaPresent=Yes
SwapDrive=DontCare
Removable=DontCare
Remote=DontCare
CauseFaults=No
INT13VolumeLockRequested="=0"
IOCTLVolumeLockRequested="=0"

  Now you can see how BCTShell works. The command-line is parsed for drive
profiles. At runtime, that profile is matched to any drives on the system
meeting those requirements. The L in LReadDrive indicates %7 will be replaced
with the drive letter representing that drive. The N in NReadDrive indicates
the two-digit drive-number (00-25) will be placed in %8's position. Any
unmentioned parameters are taken from the global parameters under [Settings].
The GUI interface again is much more intuitive here.
  When started, all test suites (packages) will appear in the upper list box.
All valid combinations of drives for each variation will appear in the lower
list box (tests).
  Pressing the tool icon (pocketknife) will bring up the variation editing
screen. No variations can be added or deleted, only modified. Modification
will result in a rescan of the system. Under the variation-edit screen are
buttons for editing global-settings (global-parameters %1-%9 for example),
Profile-settings (What type of drive is requested) and Drive-settings
(How did BCT-Shell interpret what it saw?).
  Enabling and disabling of suites is done by double-clicking on that suite.
Individual tests cannot be disabled. However, the global-screen has a string
containing all ExemptedDrives (Those not valid in any test). So tests on a
particular drive can be disabled. When BCTShell is passed a suite-name as a
command-line argument (only one can be passed) BCTShell will automatically
launch that suite with its screens minimized (acceptable for disk I/O tests).
Upon completion, BCTShell will automatically exit, returning CT-Shell
error-codes. In this way, BCTShell can be including into the CT-Shell,
launching one suite at a time. When CT-Shell displays failure for that
suite, the user can manually run BCTShell to find out what individual test
faulted. The user can then run the test suite himself and watch the screen
for an error.

                                                - Adrian J. Oney -
EMail: A-ADRIAO
Extension: x14622
Office: 4/1012


