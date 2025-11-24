TOPIC: OLE 2.0 Outline Sample Code
FILE: readme.txt
DATE: 11/11/93


OUTLINE SERIES
--------------

This sample application set demonstrates taking a 'base' application
(in this case OUTLINE.EXE) and extending it into an OLE 2 server and
container (SVROUTL and CNTROUTL respectively) and into OLE 2 in-place 
(aka visual editing) container (ICNTROTL). 

These applications attempt to implement the complete
OLE 2 functionality and recommended user model. For example, these 
applications implement all of the OLE 2 User Interface dialogs. As
such this is not a trivial sample application set.

All OUTLINE applications support loading/saving files; they all use 
docfiles for storage.  


DIRECTORY STRUCTURE
-------------------

The OUTLINE sample uses the following directory structure:

			       OUTLINE         
			          |
       +--------------+------------+--------+--------+
       |              |            |        |        |
    OUTLINE        OLE2UI        WRAPUI  BTTNCUR    GIZMOBAR

    OUTLINE     -- source directory for OUTLINE sample app series
    OLE2UI      -- source directory for common OLE 2 UI library using
                   Unicode OLE.
    WRAPUI      -- source directory for common OLE 2 UI library using
                   OLE2ANSI wrappers for the OLE 2 calls.

All five variations of the OUTLINE applications are built from common
sources.  All of the common source code (*.C and *.H files) is in the
OUTLINE directory. Each variant of the application builds in the same
directory for this release. 

Files that begin with OUTL comprise the base version of the 
application. They are also used by OLE versions of the applicaton. Files 
that begin with CNTR are specific to the container version and files that
begin with SVR are specific to the server version.  Files that begin
with OLE are common to both the container and server versions. This
series of sample applications highlights the changes necessary to
implement OLE into an existing application.  The container version has
the same functionality as the base (OUTLINE) and also allows you to
embed objects. 

Some of the major OLE features are organized into special
files in order to group related code across the application variants:
    DRAGDROP.C  -- Drag/Drop implementation
    CLIPBRD.C   -- OLE 2 style copy/paste implementation (data transfer)
    LINKING.C   -- Linking and Moniker support implementation
    CNTRINPL.C  -- In-place container implementation
    SVRINPL.C   -- In-place server implementation
    CLASSFAC.C  -- IClassFactory implementation

Conditional compilation is used to control what code is used with which
version. The following defines are used to control the compilation:
    OLE_VERSION         -- code common to all OLE versions
    OLE_SERVER          -- code used by both SVROUTL and ISVROTL
    OLE_CNTR            -- code used by both CNTROUTL and ICNTROTL
    INPLACE_CNTR        -- code used by ICNTROTL only

The following defines are used to identify code specific for a particular
feature (NOTE: it is not really intended that these symbols should be 
turned of):
    USE_DRAGDROP        -- drag/drop code
    USE_FRAMETOOLS      -- formula bar and tool bar
    USE_HEADING         -- row/column headings code
    USE_STATUSBAR       -- status bar code
    USE_CTL3D           -- use CTL3D.DLL to have 3D effect for dialogs
    USE_MSGFILTER       -- IMessageFilter code
    _DEBUG              -- debug code

HOW TO BUILD
------------

To build, simple do an nmake -a -i from the root of the outline directory.

This will first build a version of the OLE2UI library, using it's own ANSI
to UNICODE translations. This library is a private version of the OLE2UI 
library to be used exclusively by the OUTLINE samples applications. 

NO OTHER APPLICATION MAY SHIP A DLL CALLED "OUTLUI.DLL". 

Every application that builds the OLE2UI library as a DLL
must build it with a unique name for that application. See the 
OLE2UI\README.TXT for instructions on how to build the OLE2UI library.


NOTES ON WRAPUI
---------------

WRAPUI builds a dll with the same functionality as OLE2UI, except that it
uses the OLE2ANSI wrappers provided in the ole2\ole2ansi directory instead
of doing its own ANSI to UNICODE translation.  In general we recommend that
ISVs actually provide their own translation since it will improve the
performance and reduce the size of your application.  However we realize
that for a large project this may not be a feasible solution, so we have
provided WRAPUI to illustrate how you may use the OLE2ANSI wrappers in your
own application.  Please see the ole2\ole2ansi\readme.txt for further info-
rmation regarding the wrappers.

