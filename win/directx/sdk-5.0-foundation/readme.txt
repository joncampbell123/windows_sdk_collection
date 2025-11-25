========================
= DirectX 5 SDK Readme =
========================

To install the DirectX SDK and/or DirectX, please run SETUP.EXE from the root 
directory.

Overview
--------
Welcome to the DirectX 5 SDK.   If you did a full installation,
you will find a number of directories installed on your hard disk:

DOCS     - Help and Readme files for each of the DirectX components
FOXBEAR  - Fox & Bear sample demo of DirectDraw/DirectSound
ROCKEM   - Direct3D sample demo
SDK      - DirectX SDK.  Contains sample code, libraries, include files,
           and debug versions of the DirectX components
DXBUG    - DirectX bug reporting tool.

You will also find the following directories useful.  They are located on
the CD, but are not installed onto your hard disk:

DEBUG    - Debug version of DirectX runtime components
EXTRAS   - Contains 3rd party drivers, debugging tools and other stuff.
LICENSE  - License agreement
REDIST   - DirectX redistributable components
SAMPGAME - A sample game. 
 

Please see the readme file for each component (DirectDraw, Direct3D,
DirectSound, DirectPlay, and DirectInput) for more information.  These
readme files can be found in the DOCS directory.


Release Notes
-------------
1) The EXTRAS directory on the CD that contains some goodies you might like.
  
2) This SDK will install on Windows NT version 4.0 and higher for x86
   processors.  However, it will not install any DirectX runtime components
   on NT, since DirectX is built in to NT.  Windows NT 4.0 contains DirectX 2
   compatible 2 versions of DirectDraw, DirectSound, and DirectPlay. A 
   DirectX 3-compatible version, including Direct3D, is available in
   Windows NT 4.0 SP3.

   Be advised that many of the sample applications will not run on NT until
   the components they require (mainly D3D) have been installed through a
   service pack.  Please be patient with us!

3) If you use the Watcom compiler, please get the latest version - it
   contains the latest Windows header files that you will need to compile
   DirectX samples. These files are not included with the DirectX SDK.
   Support for Watcom compilers is available on the web at www.sybase.com.

4) You should know that Direct3D and DirectSound3D were designed to run on
   Pentium-class processors.  Both use floating point math, and are
   therefore quite slow on machines which do not have math co-processors.
   If you use these components, make sure you try your application on a
   486 (especially a 486SX) to verify that it runs adequately.  If it doesn't,
   we recommend that you detect the processor at setup time and refuse to
   run on those machines.

5) Users of the ATI-TV tuner option will experience loss of TV tuner and
   MPEG viewing functionality after installing DirectX drivers.  If this
   happens, the user should uninstall the DirectX drivers by going to the
   control panel, selecting Add/Remove Programs, then selecting DirectX
   drivers and pressing the "Restore Display Drivers" button.

6) The DirectX setup program may be unable to restore ATI OEM drivers for Rage
   1 & 2 using the uninstall option. If this happens, the user may reinstall
   the original OEM drivers using the following steps: in the control panel,
   select "Display" to bring up the "Display Properties" window. Click on the
   "Settings" tab, and select the "Change Display Type" button setting tab.
   Now click on the Adapter Type "Change" button, and select the original OEM
   driver to be reinstalled.

7) Users of the NEC PC-9821Ct20 may experience loss of video adapter 
   functionality if the "no" option is selected rather than the "yes" option at 
   display driver installation dialog box during DirectX5 setup. If "no" is 
   selected, the user might see a screen when the system reboots saying, "The 
   display adapter is disabled.  Press 'OK' to start Device Manager and resolve 
   the problem". The user must then re-enable the display driver by selecting 
   'Display Driver' from the device manager list and selecting the box that says, 
   "Use this device in all hardware profiles". Then select "OK" from device 
   manager.  Next,  press the "Yes" button to restart the computer.  The display 
   driver should now be functional.

8) Users of the NEC PC-9821Cu13/Cu16/Ct20 may experience loss of the 98TV tuner
   functionality after they installing DirectX 5 display driver. If this happens,
   please update your 98TV software. To get those updated software,
   please consult the guide book "CanBe WO TSUKAI KONASOU"-"Software SAISHIN
   NO OSHIRASE(The latest software information)". This page contains descriptions
   about the NEC user support information.
