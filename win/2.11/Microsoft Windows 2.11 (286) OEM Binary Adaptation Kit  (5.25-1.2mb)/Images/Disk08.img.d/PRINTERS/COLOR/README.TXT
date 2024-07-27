                                                        01-August-1988

        This directory contains files which create a device independent
color printer library.  This relieves the driver writer from duplicating
code that is common to all Windows color printer drivers.  The .LIB
file created by the Makefile is used by the IBMCOLOR printer driver
which is provided as an example of a complete driver.  Following are
some notes provided by the author of this .LIB.

DMCOLOR.LIB contains the dm*() brutes specific to color printer drivers.
The library must be the first library to search for when linking to 
prevent gdi brutes being linked instead.

The brutes performs operations on memory bitmaps (color or b/w) only.
All PDevice must be faked to memory bitmaps before calling the rountines.

The color bitmaps are in RGB representation.  Each pixel is represented by
a bit in each of 3 planes.  The color planes are in RGB order (i.e. The
red plane are followed by green plane and then blue plane).  The b/w bitmap
has only one plane.

The conversion from RGB coordinate of screen devices to CMY coordinate of 
printers is a simple operation.  The formula is shown below:
			           _ _ _
			(C M Y) = (R G B)

As a result, printer drivers can transform the RGB planes to the OEM planes
easily.  Note that the brutes only support RGB planes and no other OEM 
planes.

The library contains the following brutes:
	dmrealizeobject
	dmenumobj
	dmcolorinfo
	dmpixel
	dmbitblt
	dmstrblt
	dmoutput
	dmscanlr

