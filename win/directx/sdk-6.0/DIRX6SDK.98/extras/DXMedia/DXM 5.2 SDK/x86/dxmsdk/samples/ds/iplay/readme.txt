Iplay Sample Application

Version: 1,0,0,3	Date:  Dec 20, 1996

Table of Contents
-----------------
1.  Overview
2.  System Requirements
3.  Application Structure
4.  Build Instructions
5.  How To Run
6.  Known Limitations
7.  References


1. Overview
-----------
Iplay is a sample ActiveMovie video player.   The purpose of Iplay is to  
demonstrate the Indeo Video Interactive API in the ActiveMovie 
environment.  


2.  System Requirements
-----------------------
Windows 95 or Windows NT
ActiveMovie 2.0 Alpha SDK
MSVC++ 4.2
ir41_32.ax placed in the windows system directory and registered
  (   drive:\msdev\bin\regsvr32   drive:\windows\system\ir41_32.ax   )


3.  Application Structure
-------------------------
Iplay is written in C++ using the Microsoft Foundation Class (MFC) 
library.  It follows the standard structure of an MFC application:

	File			Class			Description
	------------------------------------------------------------------
	Iplay.cpp		CIPlayApp		The application.
	Iplay.h

	Mainfrm.cpp		CMainFrame		The main frame window.
	Mainfrm.h

	Indeo.cpp		Cindeo			The form view containing
	Indeo.h						the Indeo-specific controls.

	IPlayDoc.cpp		CIPlayDoc		The "document", which in this
	IPlayDoc.h					case is a filter graph.

There are also include files and resource files that make up Iplay.  The 
resource files are located in the .\res subdirectory of the Iplay source
directory.

	File				Description
	------------------------------------------------------------------
	Ax_spec.h			The Indeo Video Interactive interface
					definitions for ActiveMovie.

	Iplay.rc			The Iplay resource definitions.

	Bitmap1.bmp			The custom toolbar bitmap.

	Indeo.ico			The Indeo icon.

	Vfw_spec.h			Indeo-specific data structures.

All ActiveMovie applications must include the following ActiveMovie 
header files.

	File				Description
	------------------------------------------------------------------
	control.h			Interface to type library: QuartzTypeLib.

	evcode.h			Standard Quartz event codes.

	strmif.h			COM interface definitions.

	uuids.h				The GUIDs for the MediaType type and class
					ids for well-known components.


4.  Build Instructions
----------------------

    1. Run Microsoft Developer Studio.
    2. Open Workspace .\src\iplay.mak.
    3. Add the ActiveMovie include files directory (drive:\MSAMovDK\include)
       to the Developer Studio Include files directory list. To do this,
       select Options from the Tools menu, click on the Directories tab,
       select "Include files" from the "Show directories for" drop-down list, 
       and add the directory to the list.
    4. Add the ActiveMovie library files directory (drive:\MSAMovDK\lib)
       to the Developer Studio Library files directory list. To do this,
       select Options from the Tools menu, click on the Directories tab,
       select "Include files" from the "Show directories for" drop-down list, 
       and add the directory to the list.
    5. Update all dependencies for the Release and Debug versions. To do this,
       select Update All Dependencies from the Build menu.
    6. Rebuild all.


5.  How To Run
--------------

The Iplay application is a standard Windows 95/NT application.  It can 
be started by double-clicking its icon, or by using the Run command.  
Iplay also supports drag-and-drop; you can drag a movie file (.avi) onto 
the Iplay application to open the file in Iplay. If you are working in 
Microsoft Developer Studio, from the Build menu select Execute 
Iplay.exe.

Iplay can play any movie file, not just Indeo Video Interactive (IVI) 
format movies.  If a movie is not IVI format, the Indeo-specific 
controls will not be enabled.

When the application starts, the initial view is just the menu and the 
toolbar.  Click the Indeo icon on the toolbar to show/hide the extended 
view which contains the Indeo-specific controls.

Using The Indeo-Specific Controls

Indeo Video Interactive provides some special settings that enhance 
standard playback.  The settings are of two types: sequence options and 
frame options.  The sequence options affect a playback sequence and must 
be set prior to playing the movie; once the movie is playing the 
sequence options cannot be changed.  The frame options affect each frame 
of the movie and can be changed at any time while the movie is stopped 
or playing.

The sequence options consist of Scalability, Access Key, Alt-Line and 
Transparency Fill.  Turning Scalability on allows the Indeo codec to 
scale playback performance by dropping bands instead of dropping frames 
to maintain frame rate.  A movie must be encoded with Scalability in 
order for this option to have an effect on playback.  The Access Key is 
an encrypted integer that provides password protection to a movie.  The 
Access Key is set at encode time, and at playback the Access Key option 
must be turned on and the correct access key value provided in order to 
play the movie. If a movie was not encoded with an Access Key, this 
option has no effect.  Turning Alt-Line on affects playback when the 
movie is resized to exactly double its original size (zoom by two).  
When the movie is "zoomed" and Alt-Line is enabled, the codec stretches 
the image by filling alternate lines of the output buffer with black 
lines.  The black lines are written only once (if the window is not 
moved around on the screen), resulting in some performance savings 
because less information has to be written to the output buffer for each 
frame.  Turning Transparency Fill on or off only affects movies that 
were encoded with transparency.  When Transparency Fill is on, the codec 
writes the "transparent" pixels to the output buffer as a solid color.  
When Transparency Fill is off, the codec does not write the transparent 
pixels to the output buffer.  

The frame options consist of Brightness, Saturation, Contrast, Decode 
Time, Decode Rect and View Rect.  Brightness, Saturation, and Contrast 
control the appearance of the movie’s colors.  They can range in value 
from -128 to 128, with 0 being normal.  Decode Time is the time limit in 
milliseconds for the codec to decode each frame.  A setting of 0 means 
the codec should use the default time limit based on the encoded frame 
rate of the movie.  For example, a 15 frame-per-second (fps) movie has a 
default decode time limit of 1/15 x 100 = 66.7 ms.  The Decode Rect is 
the portion of the source image that is actually decoded.  The Decode 
Rect cannot exceed the boundaries of the source image.  A Decode Rect 
with with 0 width and 0 height located at 0,0 defaults to the entire 
source image.  The View Rect is the portion of the decoded image that is 
actually written to the output buffer.  The View Rect cannot exceed the 
boundaries of the Decode Rect.  A View Rect of 0 width and 0 height 
located at 0,0 defaults to the entire Decode Rect.


6.  References
--------------

Indeo Video Interactive Features and Capabilities
(http://www.intel.com/pc-supp/multimed/indeo)

