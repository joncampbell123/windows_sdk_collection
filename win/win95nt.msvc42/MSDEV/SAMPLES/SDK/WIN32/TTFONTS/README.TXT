Sample: Demonstration of the Win32 Font API Functions

Summary:

The TTFONTS sample is an easy to use, powerful utility which
allows the user to explore the font system.  There is a
toolbar on top of the main frame window with buttons that
allow the following actions:

1.Enumerate all of the fonts installed for the display.
2.Get TEXTMETRIC & OUTLINETEXTMETRIC information.
3.Create a font based on an arbitrary LOGFONT structure.
4.Get "font data" by using the GetFontData() API.
5.Enumerate all of the fonts available to the default printer.

More Information:

The program is designed to provide the user an easy
interface to the API calls related to the font system.  It
will not protect against meaningless values, nor will it
hide system oddities.  Most of the buttons on the toolbar
are self explanatory and represent a single system API.

Pressing the EnumFonts button will show all of the face
names listed horizontally, and each of the fonts within that
face name listed vertically below it.  TrueType fonts will
be marked with a small colored "TT" bitmap.  Fonts that have
the DEVICE_FONTTYPE bit on will be marked with a small
bitmap image of a printer.  When the enumeration windows are
showing the user can click the left mouse button to copy the
information about a selected font into the LOGFONT and
TEXTMETRIC dialogs.  The user can dismiss this window
without changing the dialog boxes by clicking with the right
mouse button or typing any character.

The "Display" window is able to operate in any one of three
modes.  These are listed in the "Display" menu.  The first
just writes "Hello" in the middle of the screen, and it
grids the background.  This is useful when utilizing the
lfEscapement and lfOrientation fields of the LOGFONT structure.
The second mode writes all of the glyphs between the tmFirstChar
and tmLastChar values stored in the TEXTMETRIC structure.  The
final mode is used only for true type fonts.  It calls
GetFontData, finds the 'cmap' table, and displays glyphs from
the different ranges in this table.  Use the horizontal scroll
bar in the display window to step through the ranges.

Notice:

There is a font distributed on the Win32 SDK disk which covers
more than one thousand unicode characters.  In order to use this
application to its full potential, you will want to install that
font using the Control Panel, Fonts item.  The font is named
l_10646.TTF.
