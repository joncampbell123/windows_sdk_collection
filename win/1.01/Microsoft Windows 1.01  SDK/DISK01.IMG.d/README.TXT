Microsoft Windows Software Development Kit Version 1.01

The following computer equipment is required:
      o IBM XT, IBM AT or COMPAQ Computer
      o 512kb memory
      o Two Floppy disks or
          One Floppy disk and a Hard disk (recommended)
      o IBM Color graphics card, Hercules Graphics Card,
        or EGA graphics card with Monochrome, Color
             or Enhanced Color Display
      o External console connected to COM1: (debugging)

The following software is required:
      o MS-DOS 2.x or 3.x
      o Microsoft C 3.x, Microsoft Pascal 3.3x or Microsoft Macro Assembler

The following harddisk directory structure is recommended.
    c:\lib
         *.lib, *.inc, *.h files from the Libraries and Include Files disks
    c:\bin
         files in \bin on the Utilities disk
         files on C compiler interim update disk
    c:\windows
         files in \test on the Libraries (Pascal) disk
    c:\windows\pascal
         example Pascal application from Sample Sources disk
    c:\windows\c
         example C applications from Sample Sources disk
    c:\windows\pif
         *.pif files from the PIF disk
    c:\temp
         temp directory for compiler

The following environment variables should be set by autoexec.bat:
    set lib=c:\lib
    set include=c:\lib
    set tmp=c:\temp
    set temp=c:\temp
    set path=c:\windows;c:\bin;c:\lib

---------------------------------------------------------------------------
The Windows Runtime System disks provided contain Windows system modules
that do not contain debugging code.  In the \sym directory on the Libraries
(Large Model) disk are the symbol files for this "non-debugging" version.

In the \debug directory on the Libraries (Large Model) and (Pascal) disks
are the "debugging" versions of these system modules: GDI.EXE; USER.EXE;
and KERNEL.EXE along with the matching symbol files: GDI.SYM; USER.SYM and
KERNEL.SYM.

To build a debugging version of the Windows System, a copy of the Windows
System disks should be made and the KERNEL.EXE, GDI.EXE and USER.EXE files
replaced with the debugging versions before running the SETUP program.

In addition to copies of the applications documented in the Programming Guide,
source code for the Cardfile application has been included as an example of
a more full featured Windows application.  Note:  This application uses
command line features available only with Version 4.0 of the Microsoft
Macro Assembler

---------------------------------------------------------------------------
Please note the following changes and additions to the Windows Software
Development Kit documentation.
---------------------------------------------------------------------------

Programmer's Reference:


Additional notes on Windows Font files

The Microsoft Windows Utilities disk includes font files for a variety of
output devices.  The Setup program automatically determines which fonts to
install based on your choice of graphics adapter and printer(s).  These
fonts can also be added or deleted with the Control Panel.  The fonts are
divided into sets.  Two types of fonts are provided: raster fonts and
stroke (also known as vector) fonts.  Raster fonts are generally only
used for raster devices such as the screen and dot matrix printers.
They are available only in fixed sizes.  The advantages of raster fonts
are that they look better and may be faster than stroke fonts when drawn
on raster devices.  Stroke fonts are typically used if you are doing output
to a plotter.

Devices fonts may also be listed by an application.  These are the fonts
that are provided by the device.  For example, on print wheel printers, font
names equate to wheel names.  These may not be added or deleted with the
Control Panel, but may be listed when the output device is selected.  Since
there may not be a corresponding raster font for the screen, Windows will
usually substitute a screen font in the same class.

Note:  Because some applications handle fonts differently, applications may
not list all available fonts.

Font families have names which represent the characteristics of that font.
Included on your Windows disk are:

Helv - (Raster font) A proportional (characters have varying widths) font
without serifs ("sans serif").

Courier - (Raster font) A fixed pitch font (characters have uniform widths)
with serifs.

Tms Rmn - (Raster font) A proportional font with serifs.

System - (Raster font) A fixed pitch font designed for the screen.

Terminal - (Raster font) A fixed pitch font which is the same as the font
your computer displays from DOS.

Roman - (Stoke font) Classic design with serifs and strokes of varying
width.

Modern - (Stroke font) Composed of lines of uniform width, either without
serifs or with serifs of the same line quality.

Script - (Stroke font) Slanted characters formed from nearly continuous
curved lines.

The font files that can be added or deleted with the Control Panel are
classified into sets based upon the device characteristics.  If you add a
dot-matrix printer you may want to check this list to see if you want to
add a font for your printer.

Set #1  These are stroke fonts and can be used for screen, printer, or
plotter devices of any resolution.

Set #2  These are raster fonts are primarily designed for the screen
resolution of 640 x 200, specifically this includes the IBM Color Graphics
Adapter or compatible adapter card.

Set #3  These are raster fonts also designed for the screen 640 x 350
resolution, specifically the IBM Enhanced Graphics Adapter and the Hercules
Graphic Card.  These fonts are also used by the following printers: Epson
MX-80 and FX-80 printers in landscape mode, CITOH 8510 portrait mode, IBM
Graphics Printer and IBM Proprinter in landscape mode, Okidata 92, 93, 192,
193 in landscape mode, and Star Micronics SG-10 in landscape mode.

Set #4  These are raster fonts designed primarily for printers, Okidata 92,
93, 192, 193 (std. models) in portrait mode.

Set #5  These are raster fonts designed for printers and include the
following:  Epson MX-80, FX-80, in portrait mode, IBM Graphics and IBM
Proprinter in portrait mode, Okidata 92, 93, 192, 193 (IBM compatible
models) in portrait mode, Star Micronics in portrait mode.

---------------------------------------------------------------------------

page 53:  IsClipboardFormatAvailable

        A new function has been defined to make it easier to determine 
        whether or not data of a certain type exists in the clipboard:

        BOOL IsClipboardFormatAvailable(fmt)
        WORD fmt;

        fmt is the clipboard format you're interested in checking for.  
        Returns TRUE if that format is available, FALSE if not.  Unlike 
        GetClipboardData(), this function does NOT cause the data to be 
        rendered.

page 59:  SetTimer

        SetTimer( hWnd, nIDEvent, wElapse, lpTimerFunc ) : nIDNewEvent

        Parameters:

        hWnd is a handle to a window.

        If hWnd is NULL, nIDEvent is ignored and SetTimer returns a unique
        nIDNewEvent that can be used to kill the timer.

        If hWnd is non-NULL, nIDEvent is not ignored and must be used to 
        kill the timer. In this case, nIDNewEvent should be interpreted 
        as a boolean indicating whether SetTimer succeeded.

        nIDEvent is an integer value identifying the timer event.

page 124:  LineDDA

    Note:
        The callback routine must be declared as FAR PASCAL.  This is a 
        requirement for all Windows callback routines.

page 311:
        For the NAME directive, the "modulename" parameter is optional.
        If modulename is not given, the linker uses the name of the .EXE 
        file without the extension.  If neither a NAME nor LIBRARY directive
        is given, the linker behaves as if "NAME" were given without a 
        modulename.

page 312:
        For the LIBRARY directive, the "libraryname" parameter is optional.
        If libraryname is not given, the linker uses the name of the .EXE
        file without the extension.

page 318:
        For the STUB directive, if the stub file is not found in the current
        directory, the linker looks for it in the directories listed in the
        PATH environment variable.

---------------------------------------------------------------------------

Programmer's Utility Guide:


Page  5:
        (in "Important" section) IRQ5 is the best one for
        the AT.  IRQ3 will work, but then com2 won't work if
        the hardware is installed.

page 10:
        You should also copy debugging versions of GDI and USER to
        the SETUP and BUILD disks before running the Setup program.

page 37:
        /NODEFAULTLIBRARIES should read /NODEFAULTLIBRARYSEARCH.

        /ALIGNMENT takes an argument that is the number of bytes, not a 
                power of two.

        /HELP   Same as in LINK

        /NOIGNORECASE
                Same as in LINK

        /PAUSE  Same as in LINK

        /STACK  Alternative to using STACKSIZE in definitions file.

page 47:
        Additional SYMDEB option.  The /W3 switch can be used
        to see allocation messages.  (please refer to page 53)

page 83:
        Mentions that pictures can be printed from the MS-DOS Executive.
        Pictures can not be printed directly from the MS-DOS Executive.
