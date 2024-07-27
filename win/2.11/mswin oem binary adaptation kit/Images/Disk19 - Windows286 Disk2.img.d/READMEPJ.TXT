-------------------------------------------------------------------------
         INFORMATION ON THE HP PAINTJET COLOR GRAPHICS PRINTER
-------------------------------------------------------------------------

You have selected the Hewlett Packard PaintJet printer. Your printer driver is
not yet configured properly. This file contains instructions for configuring
your PaintJet driver after Windows is set up, and is used in conjunction with
the Microsoft Windows User's Guide. We recommend that you print this Readme
file for use during configuration. You may print this file using Windows Write
or another word processing package, or you may copy it directly to your printer
from your operating system. To do this, insert the Windows Utilities disc into
your floppy disc drive and enter the following at your DOS prompt:

              type a:readmepj.txt>LPT1

If your printer is not connected to LPT1, substitute the correct port
specification.

NOTE:  The HP PaintJet Soft Fonts for Microsoft Windows will be available
in early July of 1988. This package provides 16 high-resolution PaintJet soft
fonts for use with Microsoft Windows version 2.03 or higher. These fonts will
greatly improve text quality on your printer. To order this package, contact
your local HP sales office, your HP dealer, or call this toll-free number and
ask for part number 17301A:  1-800-538-8787.

CONFIGURING YOUR PRINTER

Connecting Your Printer

1. Follow the four steps on Connecting Printers in the "Configuring Your
System" section of Chapter 7 in the Microsoft Windows User's Guide. Select
PaintJet in the printer list box, then select the desired port (usually an LPT
port for Parallel interface, or a COM port for RS-232 interface) in the
Connections list box.

2. IF YOU SELECTED A COM PORT IN STEP 1:
   a) Pull down the Setup menu and select Communications Port.
   b) Establish the communications settings as:
           Baud rate=9600; Word Length=8; Parity=None; Stop Bits=1;
           Handshake=Hardware; Port=(your port)
   c) Click OK in the Communications Settings box.

Setting Up Your Printer

1. Follow ONLY the first three steps on Setting Up Printers in the
"Configuring Your System" section of Chapter 7 in the Microsoft Windows User's
Guide. Select "PaintJet on (port)" in the Default Printer list box.

The PaintJet Defaults dialog box is displayed.

     a. Print Box:  Select Color if you have an application that supports
     color printing, or Monochrome if your application is black and white.

     The Monochrome option allows you to print in color from a monochrome
     application by setting a foreground and a background color for your output
     If you select Color, the Background and Foreground options in the menu bar
     are unnecessary and therefore are unavailable.

     NOTE:  If you have a color application, selecting the Monochrome option
     and setting background/foreground colors will have no effect on printing.

     b. Orientation:  Select Portrait to use the shorter side of your paper as
     the X-axis, or Landscape to use the longer side as the X-axis.

     NOTE:  Printing is faster in Portrait mode, which is PaintJet's default
     orientation. The printer's internal fonts, Courier and Letter Gothic, can
     only be printed in Portrait mode.

     c. Media Type:  Select the appropriate type of media for your output.

     d. Media Size:  Choose A (8.5 x 11 inches) or A4 (210 x 297 mm) as media
     size, then indicate whether your media is single sheet or fanfold.

     If your media size does not match the size displayed, choose the size with
     the appropriate unit of measurement (inches or millimeters), then use the
     scroll bar on the Media Size text box to increase or decrease the length
     measurement.

     The Preloaded box is disabled for fanfold media, and is selected by
     default for single sheet media. The Preloaded selection turns off the
     "Insert Paper" prompt which appears after you start a print job, but
     before printing begins. If you use single sheet media and want the prompt
     to appear, click the Preloaded box to remove the "X".

     e. Background/Foreground:  If you chose Monochrome in the Print Box and
     you want to set a background color for your printout, choose Background
     from the PaintJet Defaults menu bar. Select your color; the check will
     appear beside your color the next time you pull down this menu. When you
     print a document, PaintJet will fill the background of your output with
     the specified color.

     Repeat this step to set a color for your text and/or graphics, this time
     selecting Foreground from the menu bar.

     f. Select the Options menu to establish which soft fonts are available to
     PaintJet, and to print a mirror image of your document.

          1. Mirror Image - Selecting this option causes PaintJet to print a
     reverse image of your document. A check appears when the option is in
     effect; it toggles on and off by repeated selection.

          2. Fonts - If you have soft fonts to use with PaintJet, you must
     use the Fonts option to make the fonts available on the printer. Complete
     printer configuration, then see "Installing Soft Fonts" below for font
     installation instructions. If you have the HP PaintJet Soft Fonts for
     Microsoft Windows, refer to the manual in that package for more detailed
     installation instructions.

     PaintJet soft fonts print at the same speed as the fonts supplied with
     Windows (Modern, Roman, Script), but produce much higher quality output.

     NOTE:  For the fastest possible printing on PaintJet, use the printer's
     internal fonts (Courier and Letter Gothic).

     g. Save Box:  The Save box is selected by default and MUST remain
     selected to complete printer configuration. If the Save box is checked
     when you choose the OK button, your current settings are saved in the
     WIN.INI file and become defaults for PaintJet.

     When you make subsequent changes to printer settings, leave the Save box
     selected if you want to save the changes as defaults for your future print
     jobs. If you unselect Save, the changes remain in effect only until you
     exit Windows.

Completing Configuration

After you make all necessary changes to the PaintJet Defaults dialog box,
complete configuration by selecting OK. Or you may choose Cancel or Reset if
you want to do the following:

     a. Cancel - ignores any changes made to the PaintJet Defaults dialog box
during the current session and returns control to the Default Printer box.
Your printer will NOT be configured if you select Cancel during initial
configuration.

     b. Reset - returns all options in the PaintJet Defaults dialog box to
their driver default values. You may then choose OK to complete configuration
and save the driver defaults as your defaults.


SOFT FONTS

Installing Soft Fonts

If you have soft font files for PaintJet and they are stored on your hard disc
in a subdirectory of your Windows directory, skip to step 1. Otherwise, follow
steps a-d below to create a fonts subdirectory and to copy the font files to
that directory. For details on these procedures, refer to Chapter 5, "Using
MS-DOS Executive" in the Windows User's Guide.

    a. From the MS-DOS Executive window, use the Special menu's Create
       Directory command to create a directory for soft fonts under your
       Windows directory (for example, C:\WINDOWS\PJFONTS).
    b. Use the Special menu's Change Directory command to move to the newly
       created fonts directory.
    c. Insert your fonts disc into the floppy disc drive.
    d. Use the File menu's Copy command to copy the files from your floppy
       disc to the current directory of your hard disc. Type A:*.* in the Copy
       box (if your floppy drive is not A:, substitute the correct letter).
       Leave the TO box empty and click OK.
    e. Use the Special menu's Change Directory command to return to your
       Windows directory.

Your soft fonts are now stored on your hard disc. To INSTALL the soft fonts so
that they are available on the printer from Windows applications:

1. Run Control Panel and choose Printer from the Setup menu.

2. Select PaintJet and choose OK to see the PaintJet Defaults dialog box.

3. Pull down the Options menu and select Fonts.

     a. In the Fonts dialog box, choose the button which matches your font
filename extension (PJF or FNT). (All soft fonts created for PaintJet have the
extension PJF.)

     b. In the Directories box, open the directory that contains your fonts by
double-clicking the drive/directory name. To move one subdirectory level above
your current location, double-click the bracketed dots ([..]). The current
directory name displays in the lower left corner of the Fonts box, and all font
files with the specified extension in that directory appear in the font scroll
box.

     c. In the font scroll box select the font(s) that you want to make
available on PaintJet. You may select more than one font by pressing the Shift
key while clicking each font filename. The selected fonts will REPLACE any
previously installed soft fonts when you choose the OK button, so be sure to
select all desired fonts. Unselected fonts will not be available on PaintJet,
but will still be stored in your fonts directory.

     d. Choose the Font Info button if you would like to see the typeface name
and point size of the selected soft fonts, then select OK to return to the
Fonts dialog box. If no fonts are selected, the Font Info button is disabled.

     e. Select OK in the Fonts dialog box to complete soft font installation.
Make any selection in the PaintJet Defaults dialog box; soft fonts will remain
installed regardless of your selection.

Now when you print from Windows applications which let you select fonts, the
soft fonts you have installed will display as options.


Removing Soft Fonts

You may remove soft fonts so they no longer appear as PaintJet font options in
your Windows applications. The font files will not be erased from your hard
disc; they simply will no longer appear on Windows applications font lists.

1. From Control Panel's Setup menu, select Printer and choose "PaintJet on
(port)."  Click OK to see the PaintJet Defaults dialog box.

2. Pull down the Options menu and select the Fonts option. Your currently
installed soft fonts are highlighted in the font scroll box.

3. Press the Shift key while you click the font(s) to be removed. The font
names become unhighlighted.

4. Choose the OK button in the Fonts dialog box.

5. Choose the OK button in the PaintJet Defaults dialog box. The fonts will be
removed regardless of the Save box setting.


UNDERSTANDING PAINTJET SETTINGS IN YOUR WIN.INI FILE

Your WIN.INI file contains settings for many Microsoft Windows features and
Windows applications. Refer to Appendix A, "Customizing Your WIN.INI File," of
the Microsoft Windows User's Guide for descriptions of these settings.

When you configure your Windows PaintJet driver, the following section is added
to the WIN.INI file (your values may differ according to the options you
selected during configuration):

[PaintJet,LPT1:]
softfonts=2
fontdir=C:\WINDOWS\PJF
FileMask=*.PJF
Font1=(your soft font names here)
Preloaded=1
MirrorImage=0
Orientation=0
PaperFeed=1
Size=0
Length=1100
PrintColor=0
MediaType=0
Background=7
Foreground=0

The first four keywords appear only if you install soft fonts for PaintJet.

All WIN.INI PaintJet settings may be changed through Control Panel, so it may
never be necessary for you to work directly in this file. And because some
settings are related, they should be changed ONLY in Control Panel where
incorrect options are unavailable for selection. The following is provided for
your reference.

Softfonts:   Shows the number of soft fonts available on PaintJet.

Fontdir:     Displays the directory path containing your soft fonts.

FileMask:    Lists your soft font filename extensions.

Font#:       Lists installed soft font filenames.

Preloaded:   0 = Paper is not preloaded
             1 = Paper is preloaded (default)

MirrorImage: 0 = Regular image (default)
             1 = Mirror image

Orientation: 0 = Portrait (default)
             1 = Landscape

PaperFeed:   0 = Single Sheet
             1 = Fanfold (default)

Size:        0 = "A" English/8.5 inches wide (default)
             1 = "A4" Metric/210 mm wide

Length:      1100  = Media is 11 inches long
             297   = Media is 297 mm long
             other = other length set using media length scroll box

PrintColor:  0 = Color application (default)
             1 = Monochrome application

MediaType:   0 = Paper (default)
             1 = Transparency

Background, Foreground:

  0=Black  1=Blue  2=Green   3=Cyan   4=Red   5=Magenta  6=Yellow  7=White


WIN.INI TROUBLESHOOTING

You may find that incorrect settings appear in your PaintJet dialog boxes if
your WIN.INI file becomes corrupt due to repeated installation of drivers or
direct editing of the file. If this occurs, delete the PaintJet section from
your WIN.INI file beginning with the [PaintJet,LPT1:] entry. Then reconfigure
your PaintJet driver using Control Panel, and the driver will insert its
default settings into your WIN.INI file. Refer to "Editing Your WIN.INI File"
in Appendix A of the Microsoft Windows User's Guide for instructions on editing
this file.

For faster printing on PaintJet, set the WIN.INI "Spooler=" setting to NO.

REMINDER:  Windows reads certain WIN.INI settings only when you start the
program. Therefore, any time you edit the WIN.INI file, you should save the
file, exit Windows, then restart Windows for your new settings to take
effect.
------------------------------------------------------------------------
                            END OF README FILE
------------------------------------------------------------------------
