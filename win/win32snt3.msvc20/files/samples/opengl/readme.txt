Windows NT OpenGL(tm) Release Notes

1.  OpenGL Functionality Notes
2.  Compiler warnings and include files
3.  Notes for users of Media Vision OpenGL SDK
4.  Sample Applications


1.  Functionality Notes

This release of Windows NT OpenGL will run on all supported hardware
under Windows NT 3.5.  In particular, it will work on any display including
VGA and Super VGA 16 color mode.  This functionality works on all CPU plaforms.

A 256 color graphics mode is desirable to yield reasonable color for most OpenGL 
applications.  To achieve the best color for OpenGL applications, 256 colors 
or greater works best.


2.  Compiler warnings and include files

When developing or porting OpenGL source code for Windows NT, you may notice
certain compiler warning messages related to floating point type conversions.

Microsoft has recognized "acceptable" type-conversion warnings and a means to
ignore them in the provided OpenGL sample source files.  See the included "glos.h"
header file.  When using this file, be certain to include it at an analogous
location in your source file.  


3.  Notes for users of Media Vision OpenGL SDK for Windows NT

Users of Media Vision's OpenGL SDK for Windows NT will be able to bring applications
developed over to Windows NT OpenGL in a compatible fashion.  Microsoft has been cooperating
with Media Vision in order to address mutual customer's needs. MediaVision has 
agreed to provide an update to their SDK which reflects these changes at a 
later date.

The following changes were made to the Microsoft implementation of OpenGL and code written using the Media Vision SDK should be updated to reflect them :
a) Microsoft updated the type defininition name of HRC to HGLRC.
   
   Applications which use rendering contexts are required to do a simple update
   and change any variable declaration occurences using HRC to HGLRC.

   A simple update may be achieved by "typedef HGLRC HRC".  Note that this temporary
   update would be incompatibile with Windows for Pen Computing code.

b) Microsoft implementation requires application to set pixel format of a DC before 
   calling wglCreateContext.

   For further reference, see the wglCreateContext documentation. 

c) New windows style bits that are required for creating OpenGL windows.

   For further reference, see the SetPixelFormat() documentation.

d) Media Vision Extensions

   The Media Vision extensions are only provided in the Media Vision implementation of OpenGL.


4.  Sample Applications

a)  Samples included

    BOOK	Sample code from "OpenGL Programming Guide" 

    DEMOS	AUXDEMO  -  AUX Library demonstration.

		BACKTRCE -  BackTrace sample ported from Silicon Graphics Irix/OpenGL.

		GENGL	 -  A Generic Windows NT OpenGL sample application. 
			    This is a great starting point for new programmers learning
                            OpenGL.

		STONEHNG -  Stonehenge application ported from SGI workstation.

    SAMPLES	OpenGL Sample code from SGI.  See SAMPLES\ABOUT.TXT for further details. 


b)  Disclaimer for Sample Applications that contain SGI copyright notice


ALL RIGHTS RESERVED 
Permission to use, copy, modify, and distribute this software for 
any purpose and without fee is hereby granted, provided that the above
copyright notice appear in all copies and that both the copyright notice
and this permission notice appear in supporting documentation, and that 
the name of Silicon Graphics, Inc. not be used in advertising
or publicity pertaining to distribution of the software without specific,
written prior permission. 

THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.

US Government Users Restricted Rights 
Use, duplication, or disclosure by the Government is subject to
restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
(c)(1)(ii) of the Rights in Technical Data and Computer Software
clause at DFARS 252.227-7013 and/or in similar or successor
clauses in the FAR or the DOD or NASA FAR Supplement.
Unpublished-- rights reserved under the copyright laws of the
United States.  Contractor/manufacturer is Silicon Graphics,
Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.

OpenGL(TM) is a trademark of Silicon Graphics, Inc.
