DirectX Release Notes for the Win32 SDK

The DirectX samples in the Win32 SDK are taken from the DirectX SDK and are
distributed here for your convenience.  

DirectDraw, DirectSound, and DirectPlay are implemented in Windows NT 4.0, 
and are distributed with the operating system.  Direct3D is not yet 
implemented in this release of Windows NT but a beta is provided with
this Win32 SDK.  See the readme.txt in \Mstools\D3dForNT on this Win32 SDK 
CD for information on setting up the Direct 3D beta on NT 4.0.  

To use DirectX on Windows 95, you need to install the DirectX SDK for Windows
95.  The samples in the Win32 SDK will build on Windows 95 without the DirectX
SDK, but they will not run without installing DirectX.

DirectSound, in this release of Windows NT, uses the Hardware Emulation Layer
(HEL); it does not directly access the hardware through the Hardware
Abstraction Layer (HAL).  Although all DirectX applications will work and all
DirectSound functions will be performed, the latency will be the same as with
waveOut, i.e., approximately 100 to 150 milliseconds.

The following items are of particular interest to developers who are 
writing DirectDraw programs for Windows NT.

When running on a system with an unsupported video driver, an
application will be unable to lock the primary surface.  On Windows 95, 
drivers that are not DirectDraw-accelerated usually do have support
for DCI.  In this case, DirectDraw is able to allow a lock on the 
primary surface even though the driver is not DirectDraw-accelerated.
On Windows NT, DCI support is built on top of DirectDraw so there are no
drivers that support DCI but don't support DirectDraw.  If the
DDCAPS_NOHARDWARE bit is set then an application will be unable to lock
the primary surface. To get around this limitation, an application can
lock an offscreen surface and then blt to the primary surface.

DirectDraw does not currently support setting all 256 colors in a
DirectDraw palette.  Palette entry 0 and palette entry 255 are always
black and white, respectively.  DirectDraw running on Windows 95 reports
this capability by setting the DDPCAPS_ALLOW256 bit in the dwPalCaps 
field.  DirectDraw running on Windows NT does not report this capability
and this caps bit will be ignored when creating a palette.

When a video memory surface is locked, DirectDraw does not acquire the
Win16 Lock on Windows NT as it does on Windows 95.  This means that
code between Lock/Unlock pairs can be debugged using normal debugging 
techniques.

DirectDraw does not support Mode X modes on this release of Windows NT.
These modes are 320x240x8 and 320x200x8 and are supported across all 
drivers on Windows 95.  Mode X modes are not linear and so direct access
to the primary surface is not allowed in these modes.  Some drivers provide 
low-res linear modes which do allow direct access to video memory.  In this 
release of Windows NT, only the ET6000 driver supports low-res linear
modes.

The following video cards have drivers which contain DirectDraw support in 
this release.  Systems with video cards other than the ones on this list are
still able to run DirectDraw applications but no hardware acceleration is 
provided.
	x86
	ATI Mach64, Mach32 
	S3 Trio64V+, Trio64V2
	S3 868, 968
	Matrox Millennium 
	Tseng ET6000 

	Mips
	S3 868, 968, Trio64V+ (some Mips machines either don't support these 
                               cards, or won't be DirectDraw accelerated)

	PPC
	S3 868, 968, Trio64V+
	Matrox Millennium (the Millennium doesn't work in some IBM PPC machines)

	Alpha
	Matrox Millennium

Direct 3D
See the readme.txt in \Mstools\D3dForNT on this Win32 SDK for information 
on setting up the Direct 3D beta on NT 4.0.  The D3D for NT binaries will only 
work on final (non-beta) Windows NT version 4. The samples will build but 
will not run with out setting up the beta.

All of the D3d samples except rockem use common resources in 
DirectX\Media directory at runtime. These samples are egg, faces, fly, 
globe, hier1, hier2, quat, shadow, tex1, tex3, tex4, tex5, trans, uvis.  
Their makefiles copy the executables to the media directory so the programs 
can use the common media files. Rockem is not copied to the media directory
since all of its resources are in the rockem directory.

Note on MIPS, using RGB emulation or dithering causes painting problems and 
application errors in some cases. On PPC, the lighting is too dim in some 
samples which will make the objects hard to see.
