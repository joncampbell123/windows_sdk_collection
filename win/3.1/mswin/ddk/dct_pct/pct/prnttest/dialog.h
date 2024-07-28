/*---------------------------------------------------------------------------*\
|  DIALOG.H                                                                   |
|                                                                             |
|  Include file for dialog stuff only.  This is to make the new dialog        |
|  editor easier to live with.                                                |
|                                                                             |
|  08/27/1991  Pulled constants from prnttest.h                               |
|                                                                             |
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
| INTERFACE CONTROL ID'S                                                      |
\*---------------------------------------------------------------------------*/
#define IDD_INTRFACE_LIST       1000
#define IDD_INTRFACE_TEST       1001
#define IDD_INTRFACE_PROF       1002
#define IDD_INTRFACE_NAME       1003
#define IDD_INTRFACE_DRIV       1004
#define IDD_INTRFACE_PORT       1005
#define IDD_INTRFACE_MOD        1006
#define IDD_INTRFACE_ADD        1007
#define IDD_INTRFACE_REM        1008
#define IDD_INTRFACE_SET        1009
#define IDD_INTRFACE_TXT        1010

/*---------------------------------------------------------------------------*\
| DIALOG VALUES                                                               |
\*---------------------------------------------------------------------------*/
#define ABOUTDLG                  10
#define ABORTDLG                  11
#define RESETDCDLG                12
#define SETHEADER                 13
#define SETTESTS                  14
#define SETOBJECT                 15
#define INTERFACE                 16
#define SELECTENVIRONMENT         17
#define FONTOPTIONS               18

/*---------------------------------------------------------------------------*\
| TESTS DIALOG ID'S                                                           |
\*---------------------------------------------------------------------------*/
#define IDD_TEST_TEXT         0x8004
#define IDD_TEST_BITMAPS      0x8008
#define IDD_TEST_POLYGONS     0x8010
#define IDD_TEST_CHARWIDTH    0x8020
#define IDD_TEST_CURVES       0x8040
#define IDD_TEST_LINES        0x8080
#define IDD_TEST_RESETDC      0x8100
#define IDD_TEST_ABORTDOC     0x8200

/*---------------------------------------------------------------------------*\
| HEADER DIALOG ID's                                                          |
\*---------------------------------------------------------------------------*/
#define IDD_HEAD_TITLEPAGE    0x4004
#define IDD_HEAD_ENTRY        0x4008
#define IDD_HEAD_GRAY         0x4010
#define IDD_HEAD_BOUNDS       0x4020
#define IDD_HEAD_CAPS         0x4040
#define IDD_HEAD_BRSH         0x4080
#define IDD_HEAD_PEN          0x4100
#define IDD_HEAD_FONT         0x4200
#define IDD_HEAD_EXP          0x4400
#define IDD_HEAD_CON          0x4800

/*---------------------------------------------------------------------------*\
| STYLE DIALOG ID's                                                          |
\*---------------------------------------------------------------------------*/
#define IDD_STYLE_WIN30         1101
#define IDD_STYLE_WIN31         1102
#define IDD_STYLE_METAFILE      1103

/*---------------------------------------------------------------------------*\
| FONT OPTIONS DIALOG ID'S                                                   |
\*---------------------------------------------------------------------------*/
#define IDD_TT_FONTS            1200
#define IDD_DEV_FONTS           1201
#define IDD_NON_DEV_FONTS       1202

/*---------------------------------------------------------------------------*\
| OBJECT DIALOG ID's                                                          |
\*---------------------------------------------------------------------------*/
#define IDD_OBJT_PENLIST        1300
#define IDD_OBJT_BRSHLIST       1301
#define IDD_OBJT_FONTLIST       1302
#define IDD_OBJT_PENALL         1303
#define IDD_OBJT_BRSHALL        1304
#define IDD_OBJT_FONTALL        1305
#define IDD_OBJT_PENTEXT        1306
#define IDD_OBJT_BRUSHTXT       1307
#define IDD_OBJT_FONTTEXT       1308
#define IDD_OBJT_BANDING        1309

/*---------------------------------------------------------------------------*\
| RESET DIALOG ID's                                                           |
\*---------------------------------------------------------------------------*/
#define IDD_RESETDC_SETUP       1400
#define IDD_RESETDC_CONTINUE    1401
#define IDD_RESETDC_END         1402
#define IDD_RESETDC_ABORT       1403

