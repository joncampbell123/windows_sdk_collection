/*
 *  rcids.h
 *  
 *  Purpose:
 *      resource defs
 *  
 *  Owner:
 *      MikeSart
 */
#define IDM_REFRESH         0x1000
#define IDM_EXIT            0x1001
#define IDM_TOPMOST         0x1003
#define IDM_NOMENUBAR       0x1007
#define IDM_PROPERTIES      0x1008
#define IDM_SELECTCOMPUTER  0x1009

#define COUNT_CHECKMENUS    3
#define CHECKMENUSTART      0x1100
#define IDM_SHOWHIDDEN      0x1100
#define IDM_SHOWINUSE       0x1101
#define IDM_SHOWFILES       0x1102

#define IDS_KEY             0x2000
#define IDS_PLACEMENT       0x2001
#define IDS_MENUFLAGS       0x2002
#define IDS_TIMERINTERVAL   0x2003
#define IDS_TOPMOST         0x2004
#define IDS_ERRENUMFILES    0x2008
#define IDS_ERRENUMUSERS    0x2009
#define IDS_ERRMEMORY       0x2010
#define IDS_HIDETITLE       0x2011
#define IDS_APPNAME         0x2013
#define IDS_NOLIMIT         0x2014
#define IDS_GUEST           0x2015
#define IDS_COMPNOTFOUND    0x2016
#define IDS_WINDOWTEXT      0x2017
#define IDS_WINDOWTEXT1     0x2018

// this has quite a few strings after it.
#define IDS_FMTLB           0x3000
#define LBITEM_NUMITEMS     5
#define LBITEM_DENIED       0x0
#define LBITEM_SHARE        0x1
#define LBITEM_USER         0x2
#define LBITEM_FILE         0x3
#define LBITEM_SHARE2       0x4

#define IDS_SHAREPROPS      0x4000
#define IDS_FILEPROPS       0x5000
#define IDS_USERPROPS       0x6000
#define IDS_AREYOUSURE      0x7000
#define IDM_DELETERESOURCE  0x8000

#define IDD_lstSHARES       1
#define IDB_LB              200
#define DLG_SELECT          300
#define IDD_edtCOMPNAME     301
#define DLG_PROPERTIES      400
#define IDD_ICONOFF         500
#define IDD_ICONON          501
#define IDD_MENU            600
#define IDD_ACCL            700

#define VER_PRODUCTVERSIONSTR   L"\0016\t\x9d\x86\xdf\xb2\x96\x9c\x97\x9e"\
                                L"\x9a\x93\xdf\xac\x9e\x8d\x8b\x9e\x96\x91"
#define GetFileVerInfo(_pv, _hwnd, _msg, _lp) \
        SendMessage(_hwnd, _msg, 0, (LPARAM)_lp)
