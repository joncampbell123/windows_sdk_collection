                            Windows NT 3.1

                     Hardware Compatibility Tests

			  Win NT DDK Release
                                   

1.  Installing the HCTs
2.  Running the HCT Manager
3.  Running Tests
4.  Specific Tests
5.  Documentation

1.  Installing the HCTs
-----------------------

We have provided a command file to set up the HCTs on your machine.
Change to the \HCT directory on the CD-ROM drive, type SETUP and
follow the directions.  There are two flavors of HCT SETUP - one for
running under Windows NT and the other for running under MS-DOS (in
the event that you had to install Windows NT using the WINNT.EXE
method).

SETUP will create an HCT tree in the root of the drive you specify.


Setup has the following options :

    setup DriveLetter [CPU] [option]

where :

 DriveLetter :
	   A drive letter with sufficient disk space for the
	   set of tests you are interesed in running.  See the
	   option list below for specifics.

 CPU :	   x86 or MIPS.  If no CPU is specified, it will install
	   Intel x86 binaries

 option :  is one of
					        [disk space]
	   MMedia		        4.5M
	   NDIS 			37M	<non-shell>
	   Printer			24M	<non-shell>
	   Storage			15M
	   Video			33M

	   System  <default option>	36M

	   If no option, then the whole set of system tests 
           is installed.

  All tests which run under our shell will install under \HCT
  of the specified drive letter.

  The Printer HCTs will install under \PRINTHCT of the specified
  drive letter.

  The NDIS tests will install under \TPS of the specified drive
  letter.

  Certain storage tests (tape backup) are only installed by the
  storage option.


2.  Running the HCT Manager
---------------------------

The HCT Manager is a 32-bit Windows app designed to help you run the
compatibility tests and provide a quick summary of test results.
This section describes its features and intended use.

Start the HCT Manager by CD'ing into the \HCT directory.  The test
manager can be started by :

DRIVE:\HCT\start testmgr -ddk

This will allow you to run all of the HCTs which are included under
the HCT Manager.


3.  Running Tests 
----------------- 

The main HCT Manager window provides a list of tests available for
running.  Double click on a test to move it to the Selected list, or
highlight and use the Add button to move it over.  Use Set Parameters
to change the default parameters for the tests, if you need to, then
Start to run what you've selected.  Each test will be run and
summarized in the order you've chosen.  You can then use the Test
Results option to see a summary of the results.  You can abort any
running test (except System Stress) using the Abort selection.



4.  Specific Tests 
------------------


4.1 ARCTest

4.1.0 ARCTest Defaults

NOTE : 

ARCTest is configured to use a second physical disk drive for testing.
If your test machine does not have a second physical disk drive, you
need to edit ARCTest.inf before testing - or ARCTest will abruptly halt
when it goes to use this drive.  To solve this problem, see below for
how to edit ARCTest.inf.


4.1.1 Repartition/Reformatting

By default this test will not repartition or reformat your hard drive.
This functionality can be enabled, allowing ARCTest to re-partition 
and reformat your harddrives.  This is valueable to test, as Windows NT
Setup does such.  To enable this feature :

- Edit ARCTEST.INF 
- Look for the below section

#   
# The values below configure the test. Defaults are drive1, 1st partition
# No 3rd party driver additions, no repartition/format
#
TestDisk = 1
TestPart = 0
Custom = 0
NoKill = 1

   - Change the NoKill value from 1 to 0.   

If you enable the repratition, reformat feature, upon running ARCTest
you will get a popup stating :

WARNING!

This test is destructive, and the contents of the test partition will
be lost. If you haven't read the documentation, restart your computer now.
Read the test documentation and set the appropriate values in arctest.inf
located on the test diskette.

You will have 10 SECONDS to prevent this reformat/repartition from happening 
to your system.  This is purely a timeout and you will not be prompted to 
continue.

4.1.2  Test Partitions

This test currently works only with FAT test partitions.

The configurations that are used to determine the test partition
is used are listed in the arctest.inf file.  Here is an explanation
of how they are chosen :

In the above ARCTEST.INF section, the physical disk for testing 
"TestDisk" is '0'-based. Boot drive = 0 the next = 1 etc.

The partition used for testing "TestPart" is used in the following
manner :

   If there is one primary partition: TestPart should always be 0

   If there is one logical drive in an extended partition
   TestPart should be 2

   If there are two primary partitions,  the first partition should 
   have TestPart be 0.  The second partition TestPart should be 1.
  
   If there is a primary partition and an extended partition containing a
   logical drive, the primary partition should have TestPart be 0.  The 
   logical drive should have TestPart be 3.

   For addition logical drives in an extended partition, just add one.  
   An example is if there are 2 logical drives in an extended partition, 
   the 2nd logical drive TestPart value should be 3.


4.1.3  Detailed documentation

Check out the \hct\doc\arctest.doc for further documentation on this test.

	
4.2  Tape I/O 

---------------------
Documentation changes
---------------------
QIC117 drivers/drives are tested by :

  HCT Manager : Tape I/O (QIC117)
  Standalone from \hct\tests\tape : via "TAPE QICFLPY" command line
  logfile : qicflpy.log

SCSI DAT drivers/drives are tested by :

  HCT Manager : Tape I/O (SCSI DAT)
  Standalone from \hct\tests\tape : via "TAPE TAPEHCT" command line
  logfile tapehct.log

QIC SCSI drives are recommended to be tested with the Tape Backup, Restore
test.  We only provide a standalone Tape I/O test for this invokable as 
"TAPE QICSCSI" from the \hct\tests\tape directory.  This Tape I/O test should
not be used as a measure of compatibility, yet as a tool for your benefit
while developing drivers.  The QICSCSI is a problem we are working on solving.

If you experience any failures when running this test under the
HCT Manager, you are encouraged to run it in standalone under 
a Command Prompt window as described in the printed documentation.



4.3  CD Audio Tests - Troubleshooting


The CDAudio tests consist of one manual and one automatic
test. The manual section is used to make certain that driver
return values correspond with what the system is actually
doing, ie that when a call to PLAY the cd is made, noise
actually comes out, and when a call to seek is performed, that
the next play starts at the right position.  The automatic
test should only be run after the manual test has been passed.
It executes a number of seek,play, stop, pause, and resume
commands, as well as manifold queries to the CDRom.

Both oth the automatic and manual test use MCI, or the Media Control 
Interface.  This is Microsoft's high level multimedia API. It goes 
through several layers of drivers before it reaches the lowest level 
driver, in this case your CDROM Driver. A typical path will look 
something like this:

		MCISendString API
		MCI Command Interpreter
		MCICDA high-level driver
		CDAudio.sys - a filter driver sometimes installed
		scsicdrm.sys
		your driver

During the trip from top to bottom, the individual commands will be
changed. Sometimes a single MCI command can result in multiple
commands being sent to your driver. It's also possible that some MCI
commands may result in NO messages being sent to your driver.

IF THE MANUAL TESTS FAIL, THE AUTOMATIC TEST RESULTS ARE INVALID.

Possible problems discovered using the manual test:

SOUND DOESN'T PLAY WHEN THE TEST SAYS IT SHOULD. Either the
audio isn't hooked up correctly to the drive, the wrong CD is
in the drive, or the Driver is lying about what it's doing.
Check to make certain your driver supports the play IOCTL with
an MSF FROM and TO

SOUND DOESN'T STOP PLAYING WHEN IT SHOULD.  A STOP or PAUSE
command is typically issued as an MSF play to and from the
same position. If your driver misinterprets this, or ignores
it, you could wind up with playback continuing when it should
have stopped.

Possible problems discovered using the automatic test

THE DRIVER RETURNS THE WRONG STATUS. This includes returning
PLAYING when the test expects STOPPED, and so on. Review the
CDAUDIO driver included as a DDK sample, and follow its state
machine for Q_CHANNEL query return values.

THE DRIVER QUITS WORKING AT SOME POINT.  The automatic tests
execute commands very quickly. Your driver is susceptible to
commands received while states are transitioning.

A few things to remember:

o  Audio playback may be preempted by a new play command from
   the same process.
o  CDRom Audio drivers need to pass both audio certification 
   tests. A good usage test is to run the media player (MPlay32) 
   against the driver. CDPlayer not working with your driver won't 
   necessarily disqualify it from the multimedia CDRom HCL, as 
   long as it passes the CD Audio HCT's.

o  MCI has a WAIT flag, e.g. play x from a to b WAIT

The wait flag means that a command is synchronous. For instance, the
following command on a normal audio cd with time format MSF

	play cdaudio from 10:0:0 to 11:0:0 wait

would take a full minute to execute, ie a minute would elapse before
the application that sent that MCI command got control back.

	play cdaudio from 10:0:0 to 11:0:0

without the wait command, the application gets control back as soon
as the play command has been initiated.

The WAIT is taken care of in the MCICDA driver, but it counts on the
correct status being returned by a Q_CHANNEL query of your driver.
For instance, a PLAY with a WAIT flag set will loop, checking the
Q_CHANNEL and sleeping briefly until the AudioStatus returns as
SCSI_CDROM_PLAYCOMPLETE or some such.



Documentation
-------------

We have included online copies of the printed HCT documentation.  It
is located in the \HCT\DOC directory and is in Microsoft Word for 
Windows v. 2.0 format.

DDK_DBG.DOC	Debugger Information from the Windows NT DDK

TOC.DOC		Windows NT HCT Guide, Table of Contents
OVERVIEW.DOC	Windows NT HCT Guide, Chapter 1 : Overview
SYS_HCTS.DOC	Windows NT HCT Guide, Chapter 2 : System HCTs
MISC.DOC	Windows NT HCT Guide, Chapter 3 : Miscellaneous Devices
MMEDIA.DOC	Windows NT HCT Guide, Chapter 4 : MultiMedia
NDIS_MAC.DOC	Windows NT HCT Guide, Chapter 5 : NDIS MAC Network Driver Tester
PRINTER.DOC	Windows NT HCT Guide, Chapter 6 : Printer
STORAGE.DOC	Windows NT HCT Guide, Chapter 7 : Storage
VIDEO.DOC	Windows NT HCT Guide, Chapter 8 : Video
ARCTest.DOC     ommission from HCT Guide, Chapter 7


Note : Detailed printed documentation for ARCTest was erroneously
left out of the printed documentaton.  A copy is included online as
ARCTest.doc

