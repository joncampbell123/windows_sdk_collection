SAMPLE: BROWSE: OLE Automation controller

BROWSE is an automation  controller that controls the automation objects of the 
BROWSEH (Browse Helper) inproc server to browse a type library. BROWSE
is a developer-oriented type library browser. 

In addition to displaying type library information, this sample shows how
to control an automation object by accessing it's properties and methods. 

(BROWSEH is an Automation object that exposes the type information of a 
type library. BROWSEH is shipped as sample code with this release).

To compile:
-----------

Requires OLE 2.02 or later.
Use the external makefile called makefile to compile. 


To run:
-------

Run browse.exe and use the menu to select a type library to browse.

Files:
------

INVHELP.CPP, INVHELP.H contains two helper functions that are useful to create an automation
object (CreateObject()) and to invoke a method or property of that object (Invoke()).
These functions are general enough to be used by any Automation controller.

BROWSE.CPP uses the helper functions in invhelp.cpp to access the properties
and methods of the automation objects of BROWSEH.  

MAKEFILE makefile.  

================================================================================


