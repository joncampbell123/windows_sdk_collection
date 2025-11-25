OpenGL(R) 1.1 for Windows(R) 95 
-------------------------------

The OpenGL API is supported on a variety of graphics hardware; the
software in this release provides support for graphics hardware including
basic emulation on any video adapter that is supported with the operating
system, and accelerated graphics hardware that is supported by an OpenGL
mini-client driver (MCD) or an OpenGL installable client driver (ICD).

This software will help application developers port OpenGL-based 
applications from Windows NT to Windows 95; it can also be used in porting
OpenGL-based applications from other operating systems to Windows 95, 
although other API's and tools will be necessary to completely port those
applications.  And of course it can be used to develop new applications
that require OpenGL on Windows 95.

The OpenGL runtime libraries for Windows 95 are not bundled with the
Windows 95 operating system currently, but application developers may
freely redistribute this software along with their applications 
to other Windows 95 systems.  In addition, the Windows 95 libraries have been
bundled with the Windows 95 operating system in the OEM system release 2, so
OEM Windows 95 systems shipping later in 1996 will begin appearing with the
OpenGL runtime libraries included.

The DLL directory contains the runtime dynamic-link libraries for OpenGL
and GLU. We recommend either of two methods for redistributing these
libraries with your application on Windows 95 (for Windows NT, the libraries
are bundled with the operating system and should not be redistributed):
  1) In your setup program, install these libraries in the 
     application directory along with your application.  This gives you
     greater control over the version of OpenGL that your application
     will link to (an issue if other applications install other versions
     of the library), but also gives you greater responsibility for 
     updating your customers' libraries if and when that is required
     to address defects, add functionality, improve performance, etc.
  2) In your setup program, install these libraries in the windows
     system directory.  If you do this, you should use the Win32 setup
     API call VerInstallFile to help prevent installing an older version
     of the libraries over another application's installation of a more
     recent version of the libraries.

OpenGL is a registered trademark of Silicon Graphics, Inc.
Windows is a registered trademark of Microsoft Corporation.
