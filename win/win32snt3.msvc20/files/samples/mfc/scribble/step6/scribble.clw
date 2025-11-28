; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CScribView
LastTemplate=splitter
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "scribble.h"
LastPage=0

ClassCount=7
Class1=CScribbleApp
Class2=CScribDoc
Class3=CScribView
Class4=CMainFrame

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDR_SCRIBTYPE
Class5=CAboutDlg
Resource4=IDD_PEN_WIDTHS
Class6=CPenWidthsDlg
Class7=CScribFrame

[CLS:CScribbleApp]
Type=0
HeaderFile=scribble.h
ImplementationFile=scribble.cpp
Filter=N

[CLS:CScribDoc]
Type=0
HeaderFile=scribdoc.h
ImplementationFile=scribdoc.cpp
Filter=N
VirtualFilter=DC
LastObject=ID_PEN_WIDTHS

[CLS:CScribView]
Type=0
HeaderFile=scribvw.h
ImplementationFile=scribvw.cpp
Filter=C
LastObject=CScribView
VirtualFilter=VWC

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
Filter=T



[CLS:CAboutDlg]
Type=0
HeaderFile=scribble.cpp
ImplementationFile=scribble.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_HELP_INDEX
Command9=ID_HELP_USING
Command10=ID_APP_ABOUT
CommandCount=10

[MNU:IDR_SCRIBTYPE]
Type=1
Class=CScribView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_EDIT_CLEAR_ALL
Command16=ID_PEN_THICK_OR_THIN
Command17=ID_PEN_WIDTHS
Command18=ID_VIEW_TOOLBAR
Command19=ID_VIEW_STATUS_BAR
Command20=ID_WINDOW_NEW
Command21=ID_WINDOW_CASCADE
Command22=ID_WINDOW_TILE_HORZ
Command23=ID_WINDOW_ARRANGE
Command24=ID_HELP_INDEX
Command25=ID_HELP_USING
Command26=ID_APP_ABOUT
CommandCount=26

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW
Command3=ID_FILE_OPEN
Command4=ID_FILE_PRINT
Command5=ID_FILE_SAVE
Command6=ID_EDIT_PASTE
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_HELP
Command10=ID_CONTEXT_HELP
Command11=ID_NEXT_PANE
Command12=ID_PREV_PANE
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_EDIT_CUT
Command16=ID_EDIT_UNDO
CommandCount=16

[DLG:IDD_PEN_WIDTHS]
Type=1
Class=CPenWidthsDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_THIN_PEN_WIDTH,edit,1350631552
Control4=IDC_THICK_PEN_WIDTH,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_DEFAULT_PEN_WIDTHS,button,1342242816

[CLS:CPenWidthsDlg]
Type=0
HeaderFile=pendlg.h
ImplementationFile=pendlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_DEFAULT_PEN_WIDTHS

[CLS:CScribFrame]
Type=0
HeaderFile=scribfrm.h
ImplementationFile=scribfrm.cpp
Filter=T

