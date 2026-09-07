
                             OLE Samples
                             ===========

A short description of the directories found under this tree:

    INCLUDE  - Common include files used by all the OLE samples\

    COMMON   - Common code used by all the OLE samples
    BROWSE   - OLE Automation controller that controls the automation
                objects of the BROWSEH inproc server.
    BROWSEH  - OLE Automation server that browses a type library.
    BTTNCUR  - Genertates BTTNCUR.DLL and BTTNCUR.LIB
    GIZMOBAR - Generates GIZMOBAR.DLL and GIZMOBAR.LIB
    OLESTD   - Generates OLESTD.LIB
    WINHLPRS - library of Windows helper functions
                used by MFRACT and DFVIEW
    CMALLSPY - Sample IMallocSpy implementation
    DEFO2V   - source code for OLE2VIEW's default object viewer DLL
    DFVIEW   - docfile viewer
    DISPCALC - OLE Automation sample program - calculator
    DISPDEMO - Sample OLE Automation controller
    DSPCALC2 - OLE Automation sample program - calculator
    HELLO    - OLE automation server that implements a dual interface.
    INOLE2   - Updated versions of samples from the book: "Inside OLE2"
    LINES    - OLE Automation implementing collections and sub-objects.
    MFRACT   - custom interface sample - modular fractal generator
    OLEAPT   - OLE Apartment model threading sample
    OUTLINE  - The OUTLINE series - samples illustrating converting
                a basic Windows application to OLE
    SPOLY    - OLE Automation sample program - polygon drawing sample
    SPOLY2   - OLE Automation sample program - polygon drawing sample
    TIBROWSE - OLE Automation sample program - type library browser
    SIMPCNTR - simple OLE container sample
    SIMPDND  - simple Drag and Drop sample
    SIMPSVR  - simple OLE server sample

Additional directories created by these samples:
    LIB      - will contain library files generated as part of this build
    BIN      - will contain many of the target EXE's and DLL's

More details on each of these samples can be found in the readme.txt files
found in most of these directories.

To build all of these samples, type "nmake -a" from this directory.
