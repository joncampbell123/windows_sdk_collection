Sample: Demonstration of the CreateDesktop, SwitchDesktop, ... APIs.

Summary:

The switcher sample allows management of multiple desktops.  Invoking
the app creates or opens a number of desktops and allows the user to
switch between them with the click of a button.

Usage:

switcher [-t numthreads]

  Numthreads is the number of desktops you wish to manage.
  The maximum is currently 15.

A small viewport to each desktop will appear in the app's client area.
To switch to a particular desktop, right click its viewport. To temporarily
enlarge the view of the desktop, left click it. A "preview" window will
appear on the left side of your screen until you release the button.

To invoke an application on a desktop, enter its name in the edit control
and hit Enter or click the Run Me button.  If you exit switcher then restart,
apps you invoked on other desktops should still be there.  To add another
desktop, click the "New Desktop" button.

Note that you can run progman or winfile on any desktop. Each instance of
these will in turn execute apps on the proper desktop.
