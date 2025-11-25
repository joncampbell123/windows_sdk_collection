The PRINTER sample does the following:

  - Shows how to print on NT, using both the CreateDC()
    and the PrinterDlg() methods for acquiring a printer
    HDC. The user is allowed to print different graphical
    objects, as well as a complete device font set. An
    "Abort" dialog is also implemented.

  - Provides complete device capabilities for all printers
    & the display.

  - Provides information (levels 1 and 2) returned
    by a call to EnumPrinters.

  - Shows how to enumerate fonts for a particular DC.

  - Illustrates differences between the various mappping
    modes.

  - Demonstrates GDI functionality.


The main application window contains a menu and a toolbar.
The various submenus allow for:

  - Options

      Print-               calls CreateDC to get a device context for the
                           selected printer in the toolbar combobox, and
                           then prints the current graphics options to this
                           DC.

      PrintDlg-            calls PrintDlg to retrieve a device context
                           for a printer, then prints out current graphics
                           options to this DC.

      GetDeviceCaps-       retrieves device capabilities for device
                           currently selected in toolbar combobox,
                           and displays them in a dialog box.

      EnumPrinters-        retrieves level 1 & 2 information returned
                           by EnumPrinters and displays this information
                           in a dialog box.

      GetPrinterDriver-    returns level 1 & 2 information returned by
                           GetPrinterDriver (for currently selected printer)
                           and displays this information in a dialog box.

      EnumPrinterDrivers-  returns level 1 & 2 information returned by
                           EnumPrinterDrivers and displays this information
                           in a dialog box.

      Refresh-             refreshes the contents for the toolbar combobox
                           (changes made in Print Manager will be relfected
                           by this).

      About-               application information dialog

  - Mapping Modes          user selects between different mapping modes

  - Graphics               user selects different primitives to display

  - Pen                    user can configure size, color, and style of
                           drawing pen

  - Brush                  user can configure size, color, and style of
                           drawing brush

  - Text color...          user can configure color used to draw fonts
