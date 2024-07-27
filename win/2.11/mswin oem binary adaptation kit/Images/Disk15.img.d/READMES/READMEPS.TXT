================================================================================
                   INFORMATION REGARDING POSTSCRIPT PRINTERS
================================================================================

You have selected one of the following printers:

Agfa/Compugraphic 400PS       Digital LPS PrintServer 40   TI OmniLaser 2115
Apple LaserWriter             IBM Personal Pageprinter     Varityper VT-600
Apple LaserWriter Plus        Linotronic 100/300/500       Wang LCS15
AST TurboLaser/PS - R4081     NEC Silentwriter LC-890      Wang LCS15 FontPlus
Dataproducts LZR 2665         QMS-PS 800                   Olivetti Cicero PG301
Digital LN03R ScriptPrinter   QMS-PS 800 Plus              Olivetti LP 5000
Apple LaserWriter II NT/NTX   TI OmniLaser 2108            

These printers are known as "PostScript  Printers."  Your printer has
been installed; however, it has not yet been configured properly.  To
properly configure your PostScript printer, use the Control Panel.
For information on how to do this, see "Configuring Your System" in
Chapter 7, "Using Control Panel", of the  Microsoft  Windows User's
Guide.  Be sure to properly set up your communications port.


Printing Faster With a Postscript Printer:

Each time you print from Windows to a PostScript printer, Windows
first prepares the printer by sending it a header file.  Since sending
this file to the printer takes about 30 seconds, and the printer only 
needs this information once each time it is turned on, you can save 
time by telling Windows not to send this header to the printer and 
then doing it yourself each time you turn your printer on.

NOTE:  Do NOT do this if you will be creating PostScript print
files to send to a service bureau for printing.

If you decide to do this, first use Control Panel to configure your
printer.  Bring up the PostScript device specific dialog, click
the options buttons, and select "Already downloaded."

Next, click the button that says "Header" and then click the
button that says "File".  Type in the filename "C:\WINDOWS\PSPREP.TXT"
or other directory and filename.

Now, each time you turn on your printer, you must copy this file,
to the communications port which connects your printer to your computer.
Assume you have created the file, PSPREP.TXT, in the C:\WINDOWS directory 
on your hard disk, and that  your printer is connected to the COM1:
communications port.  Then, AFTER you turn on your printer and
start Windows, you would need to use the File Copy command from
MS-DOS Executive to copy PSPREP.TXT from C:\WINDOWS to COM1:.
Likewise, before starting Windows, you could enter the following
command at the DOS prompt:

           MODE COM1:9600,n,8,1
           COPY C:\WINDOWS\PSPREP.TXT COM1

You could add the above DOS commands to your AUTOEXEC.BAT file
so that your computer automatically copies PSPREP.TXT to your
printer each time you turn it on. If you do this, BE SURE to turn
on your printer BEFORE you turn on your computer or your printer
will not be properly prepared for printing.  If you do forget to
turn on your printer first, simply issue the above command to
prepare your printer.


Using Soft Fonts with Postscript PDL Printers

You must purchase soft fonts separately; they do not come with
the Windows package.  Once you have these soft fonts, follow the
installation procedures provided with the soft font package.  If
you experience difficulties installing or using these fonts, consult
the documentation provided with the soft font package or contact the
maker of the soft fonts.


APPLE LASERWRITER OR APPLE LASERWRITER PLUS

If you use a Local Area Network Spooler rather than the Windows
Spooler, you may need to configure your Apple printer to use
hardware handshaking; this is typical when using a printer
connected to a Local Area Network.

To configure your Apple printer for hardware handshaking, use the
Control Panel to change your printer's communication port to
"Hardware" for the "Handshaking" option.  For more information
see "Setting Up a Communications Port" in Chapter 7, "Using
Control Panel," in the Microsoft Windows User's Guide.  Also, you
must select the Hardware option in the Handshaking dialog 
of the PostScript device dialogs options screen.  You need only 
do this once, since this information is permanently stored in the 
printer, even if you turn the printer off.

If you still have trouble printing, you may have a faulty serial
port, or you may have an older model of the Apple LaserWriter. In
order to resolve the latter problem, you may need to upgrade your
LaserWriter.  In either case, contact your printer dealer.

If you are only getting the first few pages of a printout, it may
be because your printer is connected to a device which does
hardware handshaking. Try setting up hardware handshaking per the
instructions above.

Printing ITC Zapf Dingbats:

With the LaserWriter Plus, you can select the ITC Zapf Dingbats
fonts, ZapfDingbats (i.e. a number of special symbols).  When
using these fonts, the screen will display normal characters:  no
font is available with the Windows package for viewing the Dingbats 
characters on the Windows screen.  However, when the document is 
printed, the Dingbats characters will be printed instead of normal 
characters.

Printing Complicated Print Jobs:

The Apple LaserWriter and LaserWriter Plus may spend a long time
"computing" the image of very complicated printouts.  When this
happens, the Spooler occasionally alerts you that the printer is
not responding with a dialog box asking if you would like to
cancel the printout.  If this happens, choose the "Retry" button
and your printer will continue "computing."  If you need to do
this often, you may want to adjust the "Transmission retry"
timeout with the Control Panel.  For more information on changing
the "Transmission retry", see "Setting Up Printers" in Chapter 7,
"Using the Control Panel," in the Microsoft Windows User's Guide.

LINOTRONIC IMAGESETTER:

When configuring your Linotronic printer, be sure to set the
"Graphics resolution:" option to the resolution of your model
(for example, 1270 dots per inch).   For  information  on
configuring your printer, see "Setting Up Printers" in Chapter 7,
"Using the Control Panel," in the Microsoft Windows User's Guide.


IBM PERSONAL PAGEPRINTER:

In order to use the IBM Personal Pageprinter, you must set up the
postscript printer driver to use the EPT: port.  If you did not
do this during Setup, then you may use the "Connections..."
command from the "Setup" menu of the control panel.  Special EPT:
port software is provided with your IBM printer.  For more
information, see your IBM Personal Pageprinter documentation.

TI OMNILASER:

When printing to a TI OmniLaser, you may experience problems such
as garbled output or messages such as "Can't write to printer."
These problems only occur when the printer is connected directly
to the computer via its parallel port. If you get garbled output,
then connect your printer to the computer via a serial port. If
you get messages such as "Can't write to printer," then either
connect your printer to the computer via a serial port, or simply
press "Retry" when Windows says this.  This problem has been 
fixed in version 1.4 and later of the OmniLaser ROM's.

USING A4 PAPER ON VERSION 23 OR EARLIER POSTSCRIPT ROM'S

Copy the file A4TRAY.TXT, which is located on your Utilities 2 Disk,
to the communications port that your PostScript printer is attached
to in order to get your printer to print properly on A4 paper.  You
will have to copy this file to your printer each time you turn it
on.  The following DOS commands could be used in your autoexec.bat:

           MODE COM1:9600,n,8,1
           COPY C:\WINDOWS\A4TRAY.TXT COM1

================================================================================
                              END OF READMEPS.TXT
================================================================================
