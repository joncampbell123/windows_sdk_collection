; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAllControlsSheet
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "cmnctrls.h"
LastPage=0

ClassCount=13
Class1=CAnimateCtrlPage
Class2=CCtrldemoApp
Class3=CListCtrlPage
Class4=CMyListCtrl
Class5=CMyTreeCtrl
Class6=CProgCtrlPage
Class7=CAllControlsSheet
Class8=CSliderCtrlPage
Class9=CSpinCtrlPage
Class10=CStandardBar
Class11=CPaletteBar
Class12=CToolBarCtrlPage
Class13=CTreeCtrlPage

ResourceCount=7
Resource1=IDD_LISTCTRL
Resource2=IDD_TREECTRL
Resource3=IDD_TOOLBARCTRL
Resource4=IDD_PROGCTRL
Resource5=IDD_SLIDERCTRL
Resource6=IDD_SPINCTRL
Resource7=IDD_ANIMCTRL

[CLS:CAnimateCtrlPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=animctrl.h
ImplementationFile=animctrl.cpp

[CLS:CCtrldemoApp]
Type=0
BaseClass=CWinApp
HeaderFile=ctrldemo.h
ImplementationFile=ctrldemo.cpp

[CLS:CListCtrlPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=listcpg.h
ImplementationFile=listcpg.cpp

[CLS:CMyListCtrl]
Type=0
BaseClass=CListCtrl
HeaderFile=mlistctl.h
ImplementationFile=mlistctl.cpp

[CLS:CMyTreeCtrl]
Type=0
BaseClass=CTreeCtrl
HeaderFile=mtreectl.h
ImplementationFile=mtreectl.cpp

[CLS:CProgCtrlPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=progctrl.h
ImplementationFile=progctrl.cpp

[CLS:CAllControlsSheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=propsht.h
ImplementationFile=propsht.cpp
LastObject=CAllControlsSheet

[CLS:CSliderCtrlPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=slidctrl.h
ImplementationFile=slidctrl.cpp

[CLS:CSpinCtrlPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=spinctrl.h
ImplementationFile=spinctrl.cpp

[CLS:CStandardBar]
Type=0
BaseClass=CToolBarCtrl
HeaderFile=toolbar1.h
ImplementationFile=toolbar1.cpp

[CLS:CPaletteBar]
Type=0
BaseClass=CToolBarCtrl
HeaderFile=toolbar2.h
ImplementationFile=toolbar2.cpp

[CLS:CToolBarCtrlPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=toolpage.h
ImplementationFile=toolpage.cpp

[CLS:CTreeCtrlPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=treecpg.h
ImplementationFile=treecpg.cpp

[DLG:IDD_ANIMCTRL]
Type=1
Class=CAnimateCtrlPage
ControlCount=12
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT1,edit,1350631552
Control3=IDC_BROWSE,button,1342242816
Control4=1032,button,1342308359
Control5=IDC_CENTER,button,1342373891
Control6=IDC_TRANSPARENT,button,1342242819
Control7=IDC_AUTOPLAY,button,1342242819
Control8=IDC_PLAY,button,1342242816
Control9=IDC_STOP,button,1342242816
Control10=IDC_ANIMFRAME,static,1342308352
Control11=IDC_ANIMSTATUS,static,1342308352
Control12=IDC_STATIC,button,1342177287

[DLG:IDD_LISTCTRL]
Type=1
Class=CListCtrlPage
ControlCount=14
Control1=IDC_LISTVIEW1,SysListView32,1350631489
Control2=IDC_AUTOARRANGE,button,1342242819
Control3=IDC_SINGLESEL,button,1342242819
Control4=IDC_NOLABELWRAP,button,1342242819
Control5=IDC_EDITLABELS,button,1342242819
Control6=IDC_NOCOLUMNHEADER,button,1342242819
Control7=IDC_NOSORTHEADER,button,1342242819
Control8=IDC_STATIC,static,1342308352
Control9=IDC_VIEWMODE,combobox,1344341251
Control10=IDC_STATIC,static,1342308352
Control11=IDC_ALIGN,combobox,1344340227
Control12=IDC_STATIC,static,1342308352
Control13=IDC_SORT,combobox,1342243075
Control14=IDC_LISTSTATUS,edit,1352730692

[DLG:IDD_PROGCTRL]
Type=1
Class=CProgCtrlPage
ControlCount=18
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_PROGRESS_FROM,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_PROGRESS_TO,edit,1350631552
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308352
Control8=IDC_PROGRESS_POS,edit,1350631552
Control9=IDC_SETPOS,button,1342242816
Control10=IDC_STATIC,static,1342308352
Control11=IDC_PROGRESS_DELTA,edit,1350631552
Control12=IDC_PROGRESS_SETDELTA,button,1342242816
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308352
Control15=IDC_PROGRESS_STEP,edit,1350631552
Control16=IDC_PROGRESS_STEPIT,button,1342242816
Control17=IDC_PROGRESS,msctls_progress32,1350565888
Control18=IDC_STATIC,button,1342177287

[DLG:IDD_SLIDERCTRL]
Type=1
Class=CSliderCtrlPage
ControlCount=25
Control1=IDC_STATIC,static,1342308352
Control2=IDC_SLIDER_ORIENTATION,combobox,1344340035
Control3=IDC_STATIC,static,1342308352
Control4=IDC_SLIDER_POINT,combobox,1344340035
Control5=IDC_SLIDER_NOTICKS,button,1342242819
Control6=IDC_SLIDER_AUTOTICKS,button,1342242819
Control7=IDC_STATIC,static,1342308352
Control8=IDC_SLIDER_TICKFREQ,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_SLIDER_RANGEFROM,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_SLIDER_RANGETO,edit,1350631552
Control13=IDC_SLIDER_ENABLESEL,button,1342242819
Control14=IDC_SLIDER_SELRANGEFROM,edit,1484849280
Control15=IDC_STATIC,static,1342308352
Control16=IDC_SLIDER_SELRANGETO,edit,1484849280
Control17=IDC_SLIDER_FIXED,button,1342242819
Control18=IDC_SLIDER_NOTHUMB,button,1342242819
Control19=IDC_STATIC,static,1342308352
Control20=IDC_SLIDER_PAGE,edit,1350631552
Control21=IDC_STATIC,static,1342308352
Control22=IDC_SLIDER_LINE,edit,1350631552
Control23=IDC_SLIDER_VERTPOS,static,1073741831
Control24=IDC_SLIDER_HORZPOS,static,1073741831
Control25=IDC_NOTIFICATIONS,static,1342308353

[DLG:IDD_SPINCTRL]
Type=1
Class=CSpinCtrlPage
ControlCount=17
Control1=IDC_STATIC,static,1342308352
Control2=IDC_SPIN_ORIENTATION,combobox,1344339971
Control3=IDC_STATIC,static,1342308352
Control4=IDC_SPIN_ALIGNMENT,combobox,1344339971
Control5=IDC_STATIC,static,1342308352
Control6=IDC_SPIN_FROM,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_SPIN_TO,edit,1350631552
Control9=IDC_SPIN_NOTHOUSANDS,button,1342242819
Control10=IDC_SPIN_WRAP,button,1342242819
Control11=IDC_SPIN_ARROWKEYS,button,1342242819
Control12=IDC_SPIN_AUTOBUDDY,button,1342242819
Control13=IDC_SPIN_SETBUDDYINT,button,1342242819
Control14=IDC_STATIC,button,1342177287
Control15=IDC_SPIN_POS,static,1073741831
Control16=IDC_SPIN_EDIT,edit,1350631552
Control17=IDC_STATIC,button,1342177287

[DLG:IDD_TOOLBARCTRL]
Type=1
Class=CToolBarCtrlPage
ControlCount=15
Control1=IDC_ALTDRAG,button,1342373891
Control2=IDC_STATIC,button,1342177287
Control3=ID_TOOLBARSTYLE,static,1342308352
Control4=ID_BUTTONSTYLE,static,1342308352
Control5=ID_TBSTYLEBUTTON,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=ID_TOOLBARSTYLE2,static,1342308352
Control8=ID_BUTTONSTYLE2,static,1342308352
Control9=ID_TBSTYLECHECKGROUP,static,1342308352
Control10=ID_CCSBOTTOM,static,1342308352
Control11=ID_CCSTOP,static,1342308352
Control12=ID_CCSADJUSTABLE,static,1342308352
Control13=ID_TBSTYLETOOLTIPS,static,1342308352
Control14=ID_TBSTYLEWRAPABLE,static,1342308352
Control15=ID_TBSTYLETOOLTIPS2,static,1342308352

[DLG:IDD_TREECTRL]
Type=1
Class=CTreeCtrlPage
ControlCount=8
Control1=IDC_TREEVIEW1,SysTreeView32,1342242816
Control2=IDC_TVEDITLABELS,button,1342242819
Control3=IDC_TVHASBUTTONS,button,1342242819
Control4=IDC_TVHASLINES,button,1342242819
Control5=IDC_TVLINESATROOT,button,1342242819
Control6=IDC_TVDISABLEDDRAGDROP,button,1342242819
Control7=IDC_TVSHOWSELALWAYS,button,1342242819
Control8=IDC_TREESTATUS,edit,1352730692

