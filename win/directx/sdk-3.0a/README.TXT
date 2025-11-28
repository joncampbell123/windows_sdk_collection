========================
= DirectX 3 SDK Readme =
========================

To install the DirectX SDK and/or DirectX, please run SETUP.EXE from the root 
directory.

Overview
--------
Welcome to the DirectX 3 SDK.   If you did a full installation,
you will find a number of directories installed on your hard disk:

DOCS     - Help and Readme files for each of the DirectX components
FOXBEAR  - Fox & Bear sample demo of DirectDraw/DirectSound
IKLOWNS  - Immortal Klowns sample demo of DirectDraw/DirectSound
ROCKEM   - Direct3D sample demo
SDK      - DirectX SDK.  Contains sample code, libraries, include files,
           and debug versions of the DirectX components

You will also find the following directories useful.  They are located on
the CD, but are not installed onto your hard disk:

DEBUG    - Debug version of DirectX redistributable components
EXTRAS   - Contain drivers that have not completed the Microsoft QA process
LICENSE  - License agreement
REDIST   - DirectX redistributable components
SAMPGAME - A sample demo which uses DirectX redistributable components
WEBDIST  - Compressed DirectX components useful for web distribution

Please see the readme file for each component (DirectDraw, Direct3D,
DirectSound, DirectPlay, and DirectInput) for more information.  These
readme files can be found in the DOCS directory.


New for this Release
--------------------

1) DirectPlay 3, including an Internet Service Provider and redesigned
   easy-to-use interfaces.

2) DirectSound3D for positional audio.

3) DirectInput for easy control of mouse and keyboard.

4) Direct3D MMX acceleration.

5) New display, 3D, and sound drivers.


Release Notes
-------------

1) There is an EXTRAS directory that contains third-party display and sound
   drivers that have not completed the Microsoft QA process.  We include the
   drivers on the SDK as a convenience for you.  Please see the license.txt
   file in the EXTRAS directory for the license agreements pertaining to
   these components.
  
2) This SDK will install on Windows NT version 4.0 and higher for x86
   processors.  However, it will not install any DirectX runtime components
   on NT, since DirectX is built in to NT.  The 4.0 release contains
   the DirectX 2 release of DirectDraw, DirectSound, and DirectPlay.
   Direct3D and the DirectX 3 components (new DirectPlay, Direct3DSound,
   DirectInput) will be available in NT 4.0 service pack releases in the
   near future.

   Be advised that many of the sample applications will not run on NT until
   the components they require (mainly D3D) have been installed through a
   service pack.  Please be patient with us!

3) If you use the Watcom compiler, please get the latest version - it
   contains the latest Windows header files that you will need to compile
   DirectX samples.  We have deleted our sdk\samples\watinc directory (which
   used to contain those header files) because you should get the Watcom
   files to be completely up-to-date.

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

