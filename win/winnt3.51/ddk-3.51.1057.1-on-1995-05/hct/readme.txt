                          Windows NT 3.51

                     Hardware Compatibility Test


Outline
 Introduction
 Installing the HCTs
 Running the HCT Manager
 Running Tests
 Known Test Problems
 Returning Results
 Specific Tests
 Documentation
 Changes/Enhancements


Introduction
------------                                   

The Windows NT HCT program is designed to test computer systems
for compatibility with the release of Windows NT 3.51.  All of your
machines that fully pass the system compatibility tests in this release 
of the HCTs and whose results are reported to us will be included in
the next update of the Hardware Compatibility List(HCL).  Also, by
returning results we can work together to solve any hardware 
incompatibilities that may are present in your hardware and 
this release of Windows NT.


This also includes some HCTs for devices that are intended for use 
as drivers are developed.  The NDIS tests are of certification quality.  
Future releases of these tests will be applicable to a certification 
program.    


Installing the HCTs
-------------------

There is one GUI setup interface regardless of the method of Windows NT
installation.
Change to the \HCT directory on the CD-ROM drive and type SETUP.  Only the
system HCTs are neccesary for system certification.

This NDIS tests are installed via a batch file in \hct\tests\ndis.

The FT tests (installed via seperate install.bat in \hct\tests\ft
directory) are installed to driveletter:\ft
Note: It is necessary to have NT Server to run these tests.


Running the HCT Manager
-----------------------

The HCT Manager is a 32-bit Windows app designed to help you run the
compatibility tests and provide a quick summary of test results.
This section describes its features and intended use.

Start the HCT Manager by running TESTMGR.EXE from the hct install directory.
The first time you run it, you'll be prompted for machine
identification information.  This ID will allow us to track a
machine's status for the duration of the program.  The ID for a given
test machine should remain the same between test runs.

The Install Information window is for feedback on the system setup
process.  We're interested in expanding our hardware coverage for
setup testing and would like to hear of any problems you may find.
Select one of the options in each of the four categories listed.
These categories are

    Boot Media                  - what media was booted to begin setup?

    Source for Setup Files      - where are the NT system files being
                                  installed from?

    New OS Configuration        - which flavor of NT are you trying
                                  to install?

    Existing OS Configuration   - what OS are you installing over?

The System Configuration window requires you to provide detailed
hardware information, down to the level of BIOS revision, for various
components of the test machine.  Please fill in everything completely
- this will assist us in debugging and understanding test failures
without having direct access to the actual machine.
Most of this information is gathered automatically at this point.
You should still verify this data and fill in/correct where necc.



Running Tests 
------------- 
The main HCT Manager window provides a list of tests available for
running.  Double click on a test to move it to the Selected list, or
highlight and use the Add button to move it over.  Use Set Parameters
to change the default parameters for the tests, if you need to, then
Start to run what you've selected.  Each test will be run and
summarized in the order you've chosen.  You can then use the Test
Results option to see a summary of the results.  You can abort any
running test (except System Stress) using the Abort selection.
Note: most tests now do automatic parameter selection/checking, so
altering the parameters will often not be necc.


Known Test Problems
-------------------
If you experience any failures when running any Tape I/O test under the
HCT Manager, you are encouraged to run it in standalone under 
a Command Prompt window as described in the printed documentation.


Returning Results
-----------------
In order to qualify your test machines, we need the test logs created
by the HCT Manager.  We have provided a convenient way to collect and
return these logs.  Select Return Test Results from the Tests menu,
insert a floppy in the drive and select the drive letter to copy the
logs to the disk.  If for some reason you are unable to do this,
manually copy the logs to the floppy in this format:

    \HCT\INSTALL.LOG --> A:\INSTALL.LOG
    \HCT\SUMMARY.LOG --> A:\SUMMARY.LOG
    \HCT\CONFIG.LOG  --> A:\CONFIG.LOG
    \HCT\LOGS\*.*    --> A:\LOGS\*.*

Please mail the floppy back to us at:

    Windows NT Hardware Compatibility Test
    Microsoft Corp.
    One Microsoft Way
    Redmond, WA  98052
    USA



Specific Tests 
--------------

Video(Windows Fonts/Text):
The Video(Windows Fonts/Text) will fail on several video adapters.  This
test is not required for system certificaion.  Video adapter drivers
should take note of these failures.

DisplayTest(automatic vesion):
The auto. version of the test is not making correct comparisons at the time
of this release.  Please run it using the manual instructions.  Note that
this test is not part of the system certification tests.

SerialPort Tests (COM1,COM2,and COMx):
If the serial port test fails on any COM port run Registry Editor
(REGEDT32.EXE) go to:
 \HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\Serial
Set the ForceFIFOEnable value from 1 to 0 and restart the system and rerun the 
COM port test.

Documentation
-------------

We have included online copies of the printed HCT documentation.  It
is located in the \HCT\DOC directory and is in Microsoft Word for 
Windows v. 2.0 format.


DDK_DBG.DOC Debugger Information from the Windows NT DDK

HCTG_FRT.DOC    Windows NT HCT Guide, Table of Contents
HCTG_C01.DOC    Windows NT HCT Guide, Chapter 1 : Overview
HCTG_C02.DOC    Windows NT HCT Guide, Chapter 2 : System HCTs
HCTG_C03.DOC    Windows NT HCT Guide, Chapter 3 : Storage
HCTG_C04.DOC    Windows NT HCT Guide, Chapter 4 : Video
HCTG_C05.DOC    Windows NT HCT Guide, Chapter 5 : NDIS MAC Network Driver Tester
HCTG_C06.DOC    Windows NT HCT Guide, Chapter 6 : MultiMedia
HCTG_C07.DOC    Windows NT HCT Guide, Chapter 7 : Printer
HCTG_C08.DOC    Windows NT HCT Guide, Chapter 8 : Miscellaneous Devices


Changes/Enhancements
--------------------
- When starting the TestManager, you will be queried about a customer type.
If you are an OEM or System Manufacturer, please choose the first option.
Corporate test sites should choose #2.
- There is a survey to be filled out when returning results.  Please fill out
a response in each of the four selection categories and add comments.  After
making the four selections it is possible to continue beyond this dialog and
return results.
If you are returning for several machines, please check the box stating you
have already returned results.
- Test Parameters Dialog now allows for running some tests debug, some in a
verbose logging format.  These can be useful for debugging problems when
testing.
- If the system crashes while testing, look at "latrun.log" in the HCT
directory.  It will contain a list of tests run to this point.  This will
help to pinpoint what is crashing the system.
-Diskload test now supports compress files on NTFS volumes.  The syntax is
"C<drive_letter>:threads, i.e. CD:20 for twenty threads running compressed
on drive D.
- Tape API test excercises variable block size operation and had better
logging, manual operation.
- New NDIS tester.  Setup script can be found in CD:\hct\tests\ndis.
A readme.txt that gets installed goes over the rudimentary operation.
- Diskload supports compressed drives/files.
