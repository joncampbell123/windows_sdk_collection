[Runtime Information]
    Printer Name=Printer Under Test
    Driver Name=winspool
    Output Destination=lpt1
    Run Options=8
    Test Name=Landscape BitBlt tests
    Landscape=1

[TestLog Information]
    File Name=lBitBlt.log
    Flags=210DBF

[Test DLLs]
    Count=2
    DLL #1=ITE_Head.DLL
    DLL #2=Win3ITE.DLL

[ITE_Head.DLL]
    Count=7
    Test #7=Show All Fonts
    Test #6=Show All Pens
    Test #5=Show All Brushes
    Test #4=List Device Capabilities
    Test #3=Show Printable Area
    Test #2=Generate Gray Scale Display
    Test #1=Generate Title Page

[Tests to Run]
    Count=2
    Test #1=Generate Title Page <ITE_Head.DLL>
    Test #2=StretchBlt Tests <Win3ITE.DLL>

[Win3ITE.DLL]
    Count=3
    Test #3=StretchBlt Tests
    Test #2=BitBlt Tests
    Test #1=Text Output/Clipping

