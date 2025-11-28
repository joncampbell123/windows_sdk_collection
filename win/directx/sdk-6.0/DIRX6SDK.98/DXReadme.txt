DirectX6 SDK

This CD contains all SDK components.
__________________________________________________________________

Contents:

- Installation
- Updated SDK
- CD Layout
- Known Issues 
- Compiler Support

__________________________________________________________________


INSTALLATION:

To install the DirectX6 SDK and/or Runtime, execute setup.exe in the root 
directory of the CD.  If you are installing over the top of an existing 
MSDN Platform SDK, read the "UPDATED SDK" section of this readme.

You should uninstall previous releases of the DirectX SDK prior to 
installing DirectX6 (see UnInstall).  The directory structure and file 
names have changed significantly.  


UnInstall:

To uninstall, use "Add/Remove Programs" from the Control Panel.

__________________________________________________________________


UPDATED SDK

As you'll see the directory structure for the DXSDK has been changed 
significantly.  We did this for two important reasons.  First to allow
for more intuitive traversal and retrieval of the SDK information, and 
second, to more closely comply with the MSDN Platform SDK.

The most obvious updates to the structure will be:  the default install
folder (c:\mssdk); sub-directories that specify DirectX6 (docs, redist);
and the strangely out of place \multimedia directory.  All of these
are to enable merging with the MSDN Platform SDK.  

With the next release of the MSDN Platform SDK, or if you install this
SDK over an existing MSDN Platform SDK the reasons for the changes 
may become obvious. 

Other major improvements include more detailed and accurate
documentation and reworked D3DIM samples showing new functionality.
	
__________________________________________________________________


CD LAYOUT

The following is a brief description of the directories found on the CD.
Depending on options specified during installation, some of these 
directories can be installed on your hard drive.

\Bin
    High level DirectX applications & tools.
    All can be accessed from the Start menu if "Utilities" are installed.

\Debug
    Debug versions of the DirectX6 DLLs.

\Doc
    Contains documentation for the DirectX APIs.  The latest documentation
    for the DX6 is best viewed with HTMLHelp.  The HTMLHelp viewer requires
    installation Internet Explorer 4.0 (IE4) and one active control.  
    DirectX installation procedure will prompt you for this installation.
    You can also view the readme.txt file in the \extras\htmlhelp directory.
    If you chose not to install (IE4), you can still view the DX6 
    documentation by accessing the files in \docs\directx6\word.

\Extras
    DMusic (Direct Music)
       - This release is beta 1 of Direct Music.  The directory contains 
         the run-time dlls for DirectMusic.  The DLLs are installed with the
         SDK.  Documentation can be found in the DX6 Documentation.  Samples
         are included in the DX SDK.  We're eager for your feedback.
    DMProducer (Direct Music Producer)
       - Direct Music Producer is the authoring tool for Direct Music.  It
         allows composers to use the interactive and variable resources of 
         DirectMusic along with the consistent sound performance of DLS.
    DX6 Drivers
       - This directory contains an HTML document that contains links to IHV
         web sites where DX6 drivers can be located.
    DXMedia
       - Contains the DirectX Media 5.2b SDK (includes DirectAnimation and
         DirectShow).  To install this latest released version of the DirectX 
         Media SDK (version 5.2b), run setup.exe from 
         "extras\DXMedia\DXM 5.2 SDK\x86".
    DSound
       - Voice Manager.  The Voice Manager Property Set specification enables
         advanced voice management for DirectSound3D on supported sound cards.
         This provides easier support of hardware accelerated DirectSound3D.
    Internet Explorer 4.01
       - Contains the required files for installing IE4.  IE4 is required 
         in order to view the DX6 Documentation in the HTMLHelp format.
         Word files are also included.
       - Also contains the Active Control required for HTMLHelp it is only 
         installed if IE4 is installed.
    IPeak
       - Included with the DirectX 6.0 is an evaluation copy of the 
         Intel IPEAK Graphics Tool Kit.  You can find this in the extras 
         folder on the CD.  Many developers have found IPEAK helpful in 
         maximizing the performance of their Direct3D title.
    Win98
       - For those of you developing on Win98 that would like a complete set
         of DirectX debug DLLs, we have shipped debug DLLs for both 
         DSound and DInput.  The DX6 SDK setup will not update these 
         components.  See the \extras\Win98\Readme.txt for more information.
  
\Include
    contains include files for DirectDraw, Direct3D, DirectSound, 
    DirectInput, DirectPlay and DirectMusic

\Lib
    contains library files for DirectDraw, Direct3D, DirectSound, 
    DirectInput and DirectPlay.
    *.LIB : COFF libraries (used by Microsoft Visual C++ 2.0 or higher).
    There is also a sub-directory that contains Borland 11.0 versions 
    of the libraries.

\License
    Text versions of the DirectX SDK and End User License Agreements and 
    the Redistributable License Agreement.

\Redist
    Redistributable versions of the DirectX6 DLLs.

\Samples
    Contains all sample code and sample binaries.   See readme.txt file 
    in each sample's directory for more details.  Most samples can be
    accessed from the Start menu.

__________________________________________________________________


KNOWN ISSUES

DDraw/D3DIM

- When creating mipmaps, it is illegal to specify a 
DDSURFACEDESC.dwMipMapCount which is greater than the power-of-two
of the smallest of the width or height of the top-level surface. For 
example, a 4x2 mipmap can only have dwMipMapCount=2, not 3. The practical 
implication is that you cannot expect a 4x2 mipmap to generate levels 
which are 2x1, 1x1. DirectDraw will only allow the smallest dimension to 
drop to 1, and won't allow deeper mip levels.


DPlay

- The IDirectPlay4A::EnumConnections( ) API will always return single-byte 
character descriptions of the service provider available on the system.  
In order to obtain fully localized multibyte descriptions without having 
to convert your entire application to Unicode, the following procedure 
may be used:

     a) QueryInterface for an IDirectPlay4 interface (Unicode)
     b) call IDirectPlay4::EnumConnections - this will give you Unicode 
        strings
     c) convert each string to ANSI using the Windows WideCharToMultiByte( )
        function
     d) release the IDirectPlay4 interface
     e) continue to use the IDirectPlay4A interface for the rest of your 
        application

     Alternatively, you can QueryInterface for an IDirectPlay3A interface. 
This will return localized multibyte character strings and eliminate step c).

- The latest drivers for US Robotics modems can cause problems hosting 
a session. This is a bug in the drivers.

- Disconnecting the phone line while enumerating modem sessions can 
result in lock ups.


DMusic

- In this release, DirectMusic does not yet support reverberation.  Also, 
WDM support for DirectMusic is still under construction.

- We are still working on improving the latency of the synthesizer under 
DirectSound.  Currently, the latency is around 100ms on systems with 
DirectSound drivers. We are working on making the latency scale to the 
driver implementation, so it will always be optimal.

- We will be adding a property set command to IDirectMusicSynthSink to 
allow the applicationto explicitly set which DirectSound device it wants 
the software synthesizer to use.  The application can then create a 
DirectSound object and pass it to IDirectMusicSynthSink.


Samples

- The D3DIM samples can be run with the DX6 reference rasterizer, however the
required registry key is not set by default.  You can set the proper registry 
key by using the files in the \samples\Multimedia\D3DIM\bin directory.  
Without the software rasterizer registry key set you will get a message 
suggesting to enable it when first trying DX6 specific samples such as BumpMap.
Because this is a software rasterizer you should expect very slow frame rates.

- There are miscellaneous issues with some display hardware & drivers.
Please let us know of any problems you encounter and specify whether you
believe it is sample or driver related.

- Depending on the resolution of your desktop and the available amount of video 
memory in your system, there may not be enough video memory for the sample 
textures.  In this case, the D3DIM samples will still run but will not be able 
to display textures (surfaces will be white).  In order to see the textures you 
can lower your desktop resolution or use a display card with more memory.

- The Direct Music sample DMShell may not work correctly with Windows 95.  When 
you attempt to change sound schemes or ports, the program may crash.  Note that
Direct Music is currently in beta.  This issue will be resolved before the final
release of Direct Music.

- DInput sample JoyStiMM.exe returns the error message "Direct Input not 
initialized" if the stick attached is not 3 axis.  This will be fixed in a future
release.

- For further information on samples, reference the dxreadme.txt at
<drive:>\mssdk\samples\multimedia.


Debug Installation

- You may see files marked as retail in the debug installation (DDrawex.dll,
DSound.vxd).  This is not an error.  It is not necessary for these files to
have debug versions.  

__________________________________________________________________


COMPILER SUPPORT

DX6 samples will work with VC++ 4.2 or better, Watcom 11.0 and Borland C 
Builder 3.  We have tried to create a single environment that will work 
with these compilers.  We have included VC project files (.mdp for support 
of VC 4.2) and makefiles that will work with VC, Watcom and Borland.

For instructions on building the samples, open
CDRom:\samples\multimedia\DXReadme.txt.  In this readme you will find 
sections on How to Build, Notes for VC++ Users, Notes for Watcom Users and 
Notes for Borland Users.  The SDK will install this readme file to 
<installed name>\samples\multimedia.

Some samples will not compile with Borland and/or Watcom.  This was most
likely due to modifications that would have been required of components
or header files outside of the DirectX domain.  
