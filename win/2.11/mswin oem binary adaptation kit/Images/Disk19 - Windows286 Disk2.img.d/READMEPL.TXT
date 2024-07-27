=======================================================================
           INFORMATION REGARDING HEWLETT PACKARD PLOTTERS
=========================================================================

You have selected one of the following plotters:

     HP 7470A     HP 7580B      HP ColorPro
     HP 7475A     HP 7586B      HP ColorPro with GEC
     HP 7550A                   HP DraftPro
     HP 7580A                   HP DraftMaster I
     HP 7585A                   HP DraftMaster II


                         CONFIGURING YOUR PLOTTER

A.  SETTING UP THE COMMUNICATIONS PORT

 1.  From Control Panel, pull down the Setup menu and choose the
 Communications Port command.  A dialog box appears displaying the
 communications settings.

 2.  Specify the following settings, then choose the OK button in the
 Communications Settings box:

   Baud Rate:   9600
   Word Length: 8
   Parity:      None
   Stop Bits:   1
   Handshake:   Hardware (essential for RS-232 port)
   Port:        COM1 or COM2, depending on which port your
                plotter is connected to.


B.  SPECIFYING YOUR PLOTTER CONNECTIONS

 3.  Pull down the Setup menu and choose the Connections command.  A dialog
 box displays a list of printers/plotters and a list of ports.

 4.  Select "HP Plotter" in the printer list box.

 5.  Select the desired port from the Connections list box (the same port
 you specified in the Communications Settings box), and choose OK.  If you
 choose a port that is connected to another device, you must change the
 other device's port connection before the driver will accept your port
 choice.


C.  SETTING UP YOUR PLOTTER

IMPORTANT:  Be sure you have completed steps 1 through 5 before proceeding with
selecting your plotter.

 SELECTING YOUR PLOTTER AS DEFAULT PRINTER

 6. From Control Panel, pull down the Setup menu and choose Printer.  The
 Default Printer box appears, listing all available printers and their
 assigned port connections.

 7.  Select "HP plotter on COMx" in the Default Printer list box.

 8.  Use the Printer Timeouts box to adjust the amount of time Windows
 waits before sending you messages about plotter problems.

  a. "Device Not Selected."  We recommend that you leave this setting
  at the default of 15 seconds.  This setting controls how much time
  passes before Windows informs you that your plotter is off-line.

  b. "Transmission Retry."  We recommend initially increasing this
  setting to a value between 100 and 200 seconds.  Plots with many
  filled areas require a higher Transmission Retry setting.  You may
  need to experiment with various values to determine which works best
  with your plots.  See section C of "Advanced Features" below for
  important details on this setting.

 9.  Choose OK in the Default Printer box.  The Plotter dialog box appears,
 displaying default plotter settings. (You will override the defaults by
 selecting your plotter model in the next step.)

 10. Pull down the Device menu and select your plotter.  A check mark is
 placed next to the plotter you select, and the dialog box title changes to
 your plotter model.

 IMPORTANT:  If your 758xB plotter serial number prefix is less than 2402,
 select the corresponding 758xA plotter.

 SELECTING MEDIA SIZE

 11. From the Plotter dialog box, pull down the Size menu.  The media sizes
 which your plotter supports display in bold and a check mark appears next
 to the default media size for your plotter.

 12. If you use a size other than the default, select that size, and the
 check mark moves to your selection.  The current media size also appears
 in the lower left corner of the Plotter dialog box.

 SETTING PAPER FEED

 13. Choose one of the following options:

  a. "Manual" causes the plotter to stop between pages so you can
  manually load the next page when plotting multiple copies of a
  drawing.

  b. "Automatic" is available only for the following models, and
  allows continuous plotting on rollfeed or automatically-fed media:

      7550, 7586B, DraftMasterII

  c. "Preloaded" turns off the Insert Paper prompt which appears
  after you start a plot but before plotting actually begins.

 SETTING ORIENTATION

 14. Choose one of the following options:

  a.  "Portrait" uses the shorter side of your paper as the X-axis.

  b.  "Landscape" uses the longer side of your paper as the X-axis.

 Default orientation is Portrait; however, many standard plotter
 applications use Landscape orientation.

 SELECTING PEN COLORS AND TYPES

 The driver provides six carousels so you can customize pen combinations.
 For example, if you plot on both paper and transparency, you can set up
 one carousel with paper pens, and another with transparency pens.  Then
 you simply select the appropriate carousel for your plot, rather than
 repeatedly resetting the pen features of a single carousel.  The driver
 also allows you to use more colors than will fit in one carousel.  To do
 this you set more than one carousel as Active.  For instructions on using
 more than one carousel, refer to "Using Multiple Carousels" under
 "Advanced Features" below.

 Carousel 1 is the default Active carousel and contains default pen colors
 and types for your plotter. These defaults display in the right half of
 the plotter dialog box.  (The first time you set up carousels 2-6 there
 will be no previous settings displayed.) When you are ready to plot, you
 will place pens in your plotter's carousel according to the settings for
 the Active carousel.  You may accept the default settings in Carousel 1
 and skip to step 19, or you may customize individual pen features by using
 the Carousel menu, as follows:

 15. Pull down the Carousel menu.  A list of Current and Active carousels
 appears.

  a. Select under Current the carousel whose pen attributes you want
  to specify.  Only one carousel at a time may be Current.

  b. Choose as Active the carousel(s) to be used by your plotter for the
  next plot job.  You may select more than one Active carousel by repeatedly
  pulling down the Carousel menu and selecting additional carousels.  A
  check appears next to each Active carousel. This setting toggles on and
  off; select an Active carousel to make it inactive.

 16. Select from the Pen Color list the pen number(s) which you want to
 set.  You may select more than one pen by pressing the Shift key while
 clicking on each desired pen number.  All the menu bar items become
 available, enabling you to change pen color, type, and other options.

 17. Pull down the PenColor menu and choose your color from this list of HP
 colors.  Your new color replaces any previous color(s) for the selected
 pen(s).

 18. Pull down the Type menu and choose from the list of available pen
 types for the selected color.  The new type replaces the present type(s)
 in the carousel list.  Pen type abbreviations are:
    P = Paper     T = Transparency     V = Vellum     R = Rollerball

 NOTE:  When you select a pen type, the driver automatically assigns the
 correct speed, force, and acceleration.  If you use standard HP pen colors
 and types, you may never need to use the Options menu to adjust these
 features.

 SETTING DRAFT AND SAVE BOXES

 19. Draft:  An "X" in the Draft box signals your plotter to draw wide
 lines and borders quickly.  For final plotting, turn draft mode off by
 clicking the "X" in the Draft box.  Draft mode is recommended for the
 majority of your plots to reduce plotting time.

 20. Save:  If the Save box is selected when you choose OK in the Plotter
 dialog box, the current settings are saved in the WIN.INI file and they
 become your defaults for that plotter.  To execute your settings for the
 current session only, click on the Save box to remove the "X" before
 choosing OK.

 WHAT ABOUT RESET AND CANCEL?

 Reset returns to their driver default values all the settings in the
 Plotter dialog box except Draft and Save.  Cancel ignores any changes made
 in the Plotter dialog box during the current session and returns control
 to the Default Printer box.

 COMPLETING CONFIGURATION

 21. When you have made all necessary changes to the Plotter dialog box,
 choose the OK button.  Your plotter is now properly configured.


ADVANCED FEATURES

A. CUSTOMIZING PEN FEATURES BEYOND THE BASICS

The Options menu in the Plotter dialog box allows you to specify pen speed,
force, acceleration and priority.  (Force and Acceleration options are
unavailable on some plotters.)

 1.  Choose a Current carousel from the Carousel menu and set its pen
 colors and types, if necessary.

 2.  Select the pen number(s) you want to tailor.  Only select more than
 one pen if you want EACH selected pen to receive the SAME speed, force,
 acceleration and/or priority setting.

 3.  Pull down the Options menu and select the desired pen feature.  Your
 selected pen(s) for the Current carousel appear in a dialog box with their
 current settings.

 4.  Do one of the following:

  a.  To set pen speed, force, and acceleration:  Use the up/down arrow
  scroll box to change pen speed, force, or acceleration for the
  selected pen(s).  Choose OK to save the current setting, Reset to
  return to the driver default value, or Cancel to exit the feature box
  without changing the setting.

  b.  To set pen priority:  In the Priority box, choose the desired
  priority for the selected pen(s).  For example, you may specify all
  lines and text to be drawn with a P.3 pen (thin line), and all fill
  to be drawn with a P.7 pen (wide line) to speed your plotting. Choose
  OK to save the current setting, Reset to return to the driver
  default priority, or Cancel to exit the Priority box without changing
  the setting.

 5.  Choose OK in the Plotter dialog box when all values are set as
 desired.


B.  USING MULTIPLE CAROUSELS

 Your plotter driver allows you to set up multiple carousels for a single
 plot, e.g., in case you want to use all 10 colors on a single drawing.
 The plotter uses all the colors in the first Active carousel, then the
 Spooler prompts you to change carousels in the plotter, and then the
 plotter completes the drawing using the next Active carousel.  To receive
 the Change Carousel prompt, your application screen MUST be sized so that
 the Spooler icon is visible during plotting.  When the icon blinks, double
 click on the icon to display its message.

 IMPORTANT:  The driver minimizes bleeding of colors by using the colors in
 each carousel in the following sequence, regardless of how they are
 numbered in the carousel:

      1.  Yellow
      2.  Orange
      3.  Red
      4.  Green
      5.  Red-Violet
      6.  Aqua
      7.  Brown
      8.  Violet
      9.  Blue
     10.  Black

 Because all the pens in one carousel are used before the next carousel is
 introduced, bleeding is more likely if you place darker pens in Carousel 1
 and lighter pens in subsequent carousels.

 1.  To set a new carousel, pull down the Carousel menu, and select as
 Current the carousel you want to set up.  Select as Active each carousel
 you want used for your next plot.

 2.  Select the number of the first pen you want to set.  All the menu bar
 options become available, enabling you to set pen color, type, speed,
 force, acceleration and priority.

 3.  Set any pen options you want as described above under "Selecting Pen
 Colors and Types" and "Customizing Pen Features Beyond the Basics."

 4.  Repeat steps 1 through 3 for each carousel you want to set up, then
 choose OK in the Plotter dialog box to accept the current settings.

 Before plotting, place pens in the plotter's carousel according to the
 pens you specified for the first Active carousel.  When you are prompted
 to change carousels during plotting, replace the pens in the plotter's
 carousel with the pens you specified for the next Active carousel.


C.  MORE ON TRANSMISSION RETRY

When you start a plotting job, Windows sends data to your plotter at a constant
transmission rate.  Because plotters cannot plot at the same speed at which
data is transmitted, they have buffers that receive and store data while they
plot.  When the buffer fills, the plotter stops accepting data from Windows
until the buffer is able to accept more data.

The Transmission Retry setting tells Windows how long to wait after your
plotter stops accepting data before trying to send it more data.  If the
specified amount of time passes and your plotter still does not accept data,
Windows stops the plot and displays an error message.

The error message offers the option to Retry or Cancel the plot. In most cases,
Retry enables the plot to complete properly.  However, to avoid this disruption
of plotting, we recommend that you increase the Transmission Retry setting.
This is especially important for the following plotters which have large memory
buffers that may delay data acceptance for longer than the default of 45
seconds:

     ColorPro with GEC
     7475
     7550
     758XB
     DraftPro
     DraftMaster I/II


D.  UNDERSTANDING PLOTTER SETTINGS IN YOUR WIN.INI FILE

Your WIN.INI file contains settings for many Microsoft Windows features and
Windows applications.  Refer to Appendix A, "Customizing Your WIN.INI File," of
the Microsoft Windows User's Guide for descriptions of these settings.

Your plotter driver adds a [Plotter Model] section to the WIN.INI file.  All
plotter settings in this section of the file can be changed through Control
Panel, so it may never be necessary for you to work directly in this file.  And
because some of these settings are determined by your plotter model, they
should be changed ONLY in Control Panel where incorrect options for your
plotter are unavailable.  The following descriptions of the plotter settings
are provided for your reference.

Current Carousel

 Indicates which carousel displays as Current when you enter the Plotter
 dialog box.


PaperFeed

 0 = Single Sheet (Manual load)
 1 = Automatic

 This setting is 0 for all plotters except those with automatic paperfeed
 (7550, 7586B and DraftMasterII).


Preloaded

 0 = Paper is not preloaded (default)
 1 = Paper is preloaded


Orientation

 0 = Portrait Mode
 1 = Landscape Mode


Draft

 0 = Final mode
 1 = Draft mode (default)


Active Carousels

 0 = Inactive
 1 = Active

 Each of the six carousels is represented by the value in its respective
 position to the right of the "ActiveCarousels=" phrase.


Size

 The first value after "Size=" represents media size as follows:

      0-4 = A-E size media
      5-9 = A4-A0 size media

 The second value applies only to roll media and represents plot length:

      10  = 24 X 36-inch roll media
      11  = 36 X 48-inch roll media



C1..C6

 These settings display specific carousel information for Carousels 1 (C1)
 through 6 (C6).  Each line represents a carousel's eight pens, each with
 six characteristics listed in the following order:

      color, type, speed, force, acceleration, priority

 This order repeats across the line for each pen.

=======================================================================
                           END OF README FILE
=======================================================================
