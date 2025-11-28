//-----------------------------------------------------------------------------
// File: Readme.txt
//
// Desc: Readme file for the DirectX 6.0 SDK samples directory
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

This directory structure contains all the samples for the DirectX 6 SDK.


The samples are organized by component, where appropriate. For example, samples
specific to one component can be found in the following sub-directories:
        DDraw   - DirectDraw samples
        DInput  - DirectInput samples
        DMusic  - DirectMusic samples
        DPlay   - DirectPlay samples
        DSound  - DirectSound samples
        D3DIM   - Direct3D immediate-mode samples
                  For more info see "NOTES FOR D3DIM SAMPLES" below.
        D3DRM   - Direct3D retained-mode samples
        Misc    - Miscellaneous code and libraries (DSetup sample)
        DSprites - Sprite-based rendering samples

Some of the these directories are broken down even further, such as D3DIM and
Sprites, due to the large number of samples for those components. Please see
the readme.txt files in the individual directories for more information.


HOW TO BUILD
------------
All SDK samples are designed to be built in the directory they are in. Just
ensure that your Win32 development environment is set up, and you can go to
any sample directory and do a make (or nmake).

To set up your development environment to build the sample apps, modify the 
DXSETENV.BAT file, which can be found in your \mssdk\bin directory, to point
to your development tools, and execute that batch file.

NOTE: A "Too many parameters" error may happen if your path is over 128
characters as a result of running DXSETENV.BAT after VCVARS32.BAT.  A method
of resolving this is to reduce the amount of information on your path to the
minimum needed.

There is a main make file in each sample directory called "MAKEFILE" which is
for use with Microsoft VC++ 2.0 or higher (NMAKE), Watcom C\C++ 10.6 or higher
(NMAKE CLONE), or Borland CBuilder3 or compatible compiler (MAKE).

There are 2 ways to build each sample, debug or retail.  To build a sample as
retail, just type:

nmake nodebug=1

To build a sample as debug, just type:

nmake

To first remove all object files before doing a build, use:

nmake clean

Or:

nmake clean nodebug=1

There is also a master make file in the each sample category directory that will
compile all the samples at once called "DXALL.MAK" which is for use with Microsoft
VC++ 2.0 or higher (NMAKE) or Watcom C\C++ 10.6 or higher (NMAKE CLONE).

In order to use the build all makefiles, the following command line should be
used:

nmake /fdxall.mak /a                   (Builds all debug files)

Other examples:

nmake /fdxall.mak /a clean=1           (Removes all debug files)
nmake /fdxall.mak /a clean=1 nodebug=1 (Removes all release files)
nmake /fdxall.mak /a nodebug=1         (Release build)


NOTES FOR USERS OF VISUAL C++
-----------------------------
In each sample directory there is also a .MDP project file that is compatible
with Microsoft Visual C++ 4.2 or higher.

Visual C++ 4.2 or greater includes the DX2 header files and libraries.  Make
sure to set the DirectX include and library paths to be BEFORE the VC library
and include paths inside the IDE environment, which can be found under the
Tools menu, Options... dialog, Directories tab.


NOTES FOR USERS OF WATCOM C/C++
-------------------------------
Watcom C/C++ v10.6 is required to compile the DXSDK samples, v10.0 is not
sufficient.

The makefiles expect the WATCOM environment variable to be set, as it should
have been by the Watcom installation procedure.

In order to use any of the makefiles you must use the Watcom NMAKE clone, and
follow the same instructions for the command line as with VC++.

Watcom and Watcom C/C++ are trademarks of Powersoft, Watcom Division.


NOTES FOR USERS OF BORLAND CBUILDER3 OR COMPATIBLE
--------------------------------------------------
The makefiles expect the BORLAND environment variable to be set.

The master makefiles will NOT work with Borland.

CBuilder is a trademark of Borland International, Inc.

NOTES FOR D3DIM SAMPLES
-----------------------

Release Notes for Samples:

General:

	These samples are based on a common framework provided in the D3DFrame
directory.  This framework is intended to conceal most of the initialization
work so that the technique being demonstrated is clearly shown.  This
framework does not necessarily represent the optimal initialization code for
a major application.

	The samples do not currently operate in 8-bit screen depths.

	Some samples require features that may not be present on your
current hardware.  To see the effect of these features, the reference
rasterizer may be selected, *however it is not enabled by default.*
A registry key required to enable it and utility .reg files have
been provided in the bin directory for this purpose.


Commands:
	<f1>		About, this list of commands
	<f2>		Renderer Dialog
				Driver
				Device
				Display Mode (these are stored per device)
	<alt-enter>	Toggle full-screen/windowed operation
	<esc>		Exit


Sample Descriptions:

For detailed information on a particular sample see the readme.txt in each samples'
\src folder.

    Bend
        Demonstrates drawprimitive with a simple technique for smooth
        skinning of a joint between two segments.

    BillBoard
	Demonstrates colorkey for both objects and their shadows.

    Boids
	Demonstrates DrawPrim usage
	Known issue: earth texture applied to sphere backwards

    Bump Map
	Demonstrates the usage of the BUMPENVMAP bump mapping approach.
	Try using other images in earthenvmap.bmp to simulate different
	number/colors of light sources.  16x16 textures work fine.

    Filter
	Demonstrates quality differences in different texture filtering methods.
	Currently both objects are rendered using the current device.
	Known issue:  Sample never sets max anisotropy high enough for
			  anisotropic filtering to kick in.

    Fireworks
	Cool demo.

    Flare
	How to add lens flare to just about anything.

    Fog
	Demonstrates Direct3D geometry pipeline's vertex fog.
	Terrain is generated via Perlin-type noise function.

    Lights
	Demonstrates use of the various Direct3D light source types.

    MFCTex
        This tool is included on the CD (but not automatically installed).
        It is provided to allow rapid prototyping of multitexture shaders.
        The predefined settings menu on the upper right is not implemented.
	The number of texture stages currently supported is restricted to 2.
	Currently this tool does not work in full screen mode.
	Source code to this sample will be made available later.

    MipMap
	Demonstrates/tests mip mapping capabilities
	Image on left is not mipmapped, image on right is.

    MultiTexture
	Demonstrates a dual texture technique of applying a light map to a
	base texture.  This prefers to use hardware that supports single pass
	multitexture.  For hardware that does not, but does support
	SRCBLEND:DESTCOLOR alpha blending, it will emulate multitexture
	via multipass rendering.
        Other hardware will not appear in the selection dialog.
	Known issue:  Does not demonstrate proper use of ValidateDevice().

    PPlane
	Demonstrates drawPrimitive calls, wireframe.

    Shadow Volume
	Demonstrates use of stencil buffers to display real-time shadows cast
        on arbitrary surfaces.  This requires the software rasterizers or hardware
        supporting stencil buffers.  Other hardware will not appear in the dialog.

    Shadow Volume 2
        This sample demonstrates more sophisticated shadowing techniques that
	require more than one bit of stencil buffer, such as when object shadows
	are intersecting.  This source code does not demonstrate ideal usage of
	the API for applications beyond visual demonstrations.

    Vertex Buffer
	Demonstrates a usage scenario for vertex buffers.

    W-Buffer
	Demonstrates eye-relative z-buffering.  This requires either the
        Reference Software rasterizer or hardware that supports this technique.
        Other hardware (and the std software device) will not appear in the dialog.

