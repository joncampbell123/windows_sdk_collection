; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSectionForm
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "enroll.h"
LastPage=0

ClassCount=8
Class1=CEnrollApp
Class2=CEnrollDoc
Class3=CSectionForm
Class4=CMainFrame
Class6=CSectionSet
Class7=CAboutDlg

ResourceCount=7
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource7=IDD_ENROLL_FORM

[CLS:CEnrollApp]
Type=0
HeaderFile=enroll.h
ImplementationFile=enroll.cpp
Filter=N

[CLS:CEnrollDoc]
Type=0
HeaderFile=enroldoc.h
ImplementationFile=enroldoc.cpp
Filter=N

[CLS:CSectionForm]
Type=0
HeaderFile=sectform.h
ImplementationFile=sectform.cpp
Filter=D
VirtualFilter=RVWC

[CLS:CSectionSet]
Type=0
HeaderFile=sectset.h
ImplementationFile=sectset.cpp
Filter=N

[DB:CSectionSet]
DB=1
ColumnCount=6
Column1=CourseID,12,8,1
Column2=SectionNo,12,4,1
Column3=InstructorID,12,8,1
Column4=RoomNo,12,10,1
Column5=Schedule,12,24,1
Column6=Capacity,5,2,1


[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
Filter=T



[CLS:CAboutDlg]
Type=0
HeaderFile=enroll.cpp
ImplementationFile=enroll.cpp
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
Command1=ID_FILE_PRINT
Command2=ID_FILE_PRINT_PREVIEW
Command3=ID_FILE_PRINT_SETUP
Command4=ID_APP_EXIT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_RECORD_FIRST
Command10=ID_RECORD_PREV
Command11=ID_RECORD_NEXT
Command12=ID_RECORD_LAST
Command13=ID_VIEW_TOOLBAR
Command14=ID_VIEW_STATUS_BAR
Command15=ID_APP_ABOUT
CommandCount=15

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_PRINT
Command2=ID_EDIT_UNDO
Command3=ID_EDIT_CUT
Command4=ID_EDIT_COPY
Command5=ID_EDIT_PASTE
Command6=ID_EDIT_UNDO
Command7=ID_EDIT_CUT
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_NEXT_PANE
Command11=ID_PREV_PANE
CommandCount=11

[DLG:IDD_ENROLL_FORM]
Type=1
Class=CSectionForm
ControlCount=12
Control1=IDC_STATIC,static,1342308352
Control2=IDC_COURSE,edit,1350633600
Control3=IDC_STATIC,static,1342308352
Control4=IDC_SECTION,edit,1350633600
Control5=IDC_STATIC,static,1342308352
Control6=IDC_INSTRUCTOR,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_ROOM,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_SCHEDULE,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_CAPACITY,edit,1350631552

