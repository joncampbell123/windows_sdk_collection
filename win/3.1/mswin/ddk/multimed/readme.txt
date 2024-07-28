************************************************************************
*                 Multimedia Device Development Kit                    *
*                   Installation and Usage Notes                       *
************************************************************************


USING NOTEPAD TO VIEW ON-LINE DOCUMENTS
=======================================

  *  If you enlarge Notepad to its maximum size, the following 
     document will be easier to read. To do so, click the 
     Maximize button in the upper-right corner of the Notepad 
     window. Or open the Control menu in the upper-left corner
     of the Notepad window and choose Maximize.

  *  To move through the document, press PAGE UP and PAGE DOWN 
     or click the arrows at the top and bottom of the scroll bar 
     along the right side of the Notepad window.

  *  To print the document, choose Print from the Notepad File 
     menu.

  *  To read other on-line documents, choose Open from the File
     menu.

  *  For Help using Notepad, press F1.

  *  If you are currently setting up Windows, choose Exit from 
     the Notepad File menu to continue with Setup.


NOTES ON THE MULTIMEDIA DEVICE DEVELOPMENT KIT (MDDK)
=====================================================

This document contains important information that is not 
included in the printed documentation. These topics are covered:

  *  Contents of the MDDK distribution disks

  *  Installing the MDDK

  *  On-line references

  *  Corrections to documentation


CONTENTS OF THE MDDK
====================

Disk #1 contains the following sample source code:

    ADLIB     A sample MIDI synthesizer driver
    SNDBLST   A sample waveform audio and MIDI port driver
    VSBD      A sample VxD for audio device contention management
    IBMJOY    A sample joystick driver
    MCIPIONR  A sample MCI video disc driver
    MCIQA     An MCI driver template

Also included are necessary include files and test documentation.

Disk #2 contains test programs and files for use with the MDDK.

Except for the files in the root directory of the MDDK disks, all of
the files on these disks are compressed.  The utility file 
EXPAND.EXE on the MDDK disks decompresses these files during the install.  
You can use this utility to decompress files individually.


INSTALLATION
============

Make sure that you have at least 10 MB free on the destination 
disk for the MDDK.  Insert the distribution disks and run the 
MDDK install program for each disk.    

The install program usage is:

  INSTALL d:

  Where d: is the destination path for the MDDK.  The path must
  not be terminated with a backslash ('\').  In order for the
  install utility to operate properly, the current path must be set to
  the root of the MDDK directory.

  For example, if you are installing from A: to C:\ you would 
  insert MDDK disk #1 into drive A: and type:

           A:
           CD \
           INSTALL C:


SOURCE CODE
===========

When examining the source code, you will find that some comment 
blocks use special tags to identify the text. These tags are 
used by internal tools for documentation and they do not 
affect compiling the source code.

BUILD ENVIRONMENT FOR THE MDDK SAMPLE DRIVERS
=============================================

When setting the path for the MDDK files, make sure that
the directories containing the Windows SDK files precede the
directories containing the MDDK files.  Some dependencies 
in the MAKE files rely on finding the Windows SDK files before
the MDDK files. 

Use the CMacros from the Windows 3.1 SDK rather than the
CMacros from the Windows 3.1 DDK when building the sample
multimedia device drivers.  
                                     
TEST APPLICATIONS
=================

The test applications use the Multimedia functions available
in Windows 3.1 to test device drivers. 

ON-LINE REFERENCES
==================

The MDDK includes the following on-line reference files, which 
give complete, up-to-date descriptions of the functions,
messages, data types, and MCI commands in the MDDK:

     Help File         View Using
     ---------         ----------
    W31MDWH.HLP      Windows Help

    W31MDQH.HLP      Microsoft Advisor (in the Programmer's
                     Workbench) or QuickHelp utility

CORRECTIONS TO DOCUMENTATION
============================

The following section lists corrections to manuals provided
in the MDDK.


GENERAL CHANGES
---------------

This section lists functions and messages that have changed
or been added since the manuals were printed.

p. 3-41:  Add the following:

          mciGetCreatorTask 

          Syntax        HTASK WINAPI mciGetCreatorTask(wDeviceID)

                        This function retrieves a handle to the 
                        process responsible for opening a device.

          Parameters    UINT wDeviceID
                            Specifies the device ID whose creator task is 
                            to be returned.

          Return Value  Returns the handle to the creator task if 
                        successful. Otherwise, returns NULL.

p. 3-68:  In the list of MCI_REALIZE flags, change 
          MCI_ANIM_PAL_BKGD to MCI_ANIM_REALIZE_BKGD and change 
          MCI_ANIM_PAL_NORM to MCI_ANIM_REALIZE_NORM. 

p. 3-83:  In the animation extensions section of the MCI_STATUS 
          command, add the following constant to the MCI_STATUS_ITEM 
          flag:

              MCI_ANIM_STATUS_STRETCH
                  The dwReturn field is set to TRUE if 
                  stretching is enabled; otherwise it is set to 
                  FALSE.

p. 3-88:  In the video overlay extensions section of the MCI_STATUS 
          command, add the following constant to the MCI_STATUS_ITEM 
          flag:

              MCI_OVLY_STATUS_STRETCH
                  The dwReturn field is set to TRUE if 
                  stretching is enabled; otherwise it is set to 
                  FALSE.

p. 3-95:  In the animation extensions section of the MCI_WINDOW 
          command, add the following flags:

              MCI_ANIM_WINDOW_ENABLE_STRETCH
                  Enables stretching of the image.

              MCI_ANIM_WINDOW_DISABLE_STRETCH
                  Disables stretching of the image.

p. 3-96:  In the video overlay extensions section of the MCI_WINDOW 
          command, add the following flags:

              MCI_OVLY_WINDOW_ENABLE_STRETCH
                  Enables stretching of the image.

              MCI_OVLY_WINDOW_DISABLE_STRETCH
                  Disables stretching of the image.

p. 5-8:   Add the following note just before the section for 
          "Setting Up a Test Application":

          NOTE: Before running a test, you might want to disable or 
          exit any applications that are not needed. Some of the test 
          applications use extensive system resources and do not yield 
          to other applications. Also, some applications can interfere 
          with the display of status messages. For example, if a test 
          causes the computer to hang while a screen saver is displayed, 
          you cannot display the test application window to read 
          the status messages it has sent to the screen. 

p. 5-13:  Add the following function description after the 
          second paragraph in the "MINTST, MOUTTST, and MIDIIOT 
          Test Applications" section:

          MIDIIOT in the 1 Machine or 2 Machine mode is primarily a 
          port test. Its basic purpose is to transmit data to and 
          from the ports. In the 2 Machine mode, you can use MIDIIOT 
          as a source of data for an internal synthesizer. Configured 
          this way, MIDIIOT can stress test the systhesizer by rapidly 
          sending it MIDI data. 

          In the Pass Through mode, MIDIIOT passes the data it 
          receives from the input port to the output port. You can 
          use the through mode to direct the input to:

            *  an internal synthesizer 
            *  the midi mapper driver connected to any device
            *  to an output port  
