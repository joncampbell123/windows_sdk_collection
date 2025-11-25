Using a Tab Control


The TABCTRL sample has a menu and a demo dialog box with a tab control in it.

This sample is based on the DIALOG sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
TabCtrl  - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
Demo     - Demos the tab control by bringing up the demo dialog box.
Page     - Implements the dialog procedure for the page dialogs
           that correspond to the tabs in the tab control.
