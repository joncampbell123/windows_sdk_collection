/************************************************************

   Header file for JFORM.C

************************************************************/

// Strings -
// (defined for convenience; normally these would be in stringtable:

#ifdef JAPAN
// app
#define SZAPP           "Jform"
#define SZWND32         "Jform ｻﾝﾌﾟﾙ (32-bit)"
#define SZWND           "Jform ｻﾝﾌﾟﾙ"

#define SZFNAME         "氏名:"
#define SZFADR1         "住所1:"
#define SZFADR2         "住所2:"
#define SZFZIP          "〒:"
#define SZFNOTES        "備考:"
#define SZFTEL          "電話:"
#define SZFMAP          "地図:"

// dlg
#define SZOK            "OK"
#define SZCXL           "ｷｬﾝｾﾙ"
#define SZSAMPLE        "ｻﾝﾌﾟﾙ:"

// menu
#define SZMFILE         "ﾌｧｲﾙ(&F)"
#define SZMCLEAR        "すべてｸﾘｱ(&C)"
#define SZMDLG          "ﾀﾞｲｱﾛｸﾞ(&D)..."
#define SZMLOAD         "ﾛｰﾄﾞ(&L)"
#define SZMSAVE         "上書き保存(&S)"
#define SZMEXIT         "終了(&X)"

// warning
#define SZSNOPW \
"このｱﾌﾘｹｰｼｮﾝを起動するにはPen Services for Windows 95が必要です。"

// CorrectWritingEx captions
#define SZCAPZIP        "郵便番号の入力"
#define SZCAPTEL        "電話番号の入力"

#else // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// app
#define SZAPP           "Jform"
#define SZWND32         "Jform Sample (32-bit)"
#define SZWND           "Jform Sample"

#define SZFNAME         "Name:"
#define SZFADR1         "Address:"
#define SZFADR2         "City:"
#define SZFZIP          "Zip:"
#define SZFNOTES        "Notes:"
#define SZFTEL          "Tel #:"
#define SZFMAP          "Map:"

// dlg
#define SZOK            "OK"
#define SZCXL           "Cancel"
#define SZSAMPLE        "Sample:"

// menu
#define SZMFILE         "&File"
#define SZMCLEAR        "&Clear All"
#define SZMDLG          "&Dialog..."
#define SZMLOAD         "&Load"
#define SZMSAVE         "&Save"
#define SZMEXIT         "E&xit"

// warning
#define SZSNOPW \
"This application requires Pen Windows to be loaded as an installable \
driver for input with a pen."

// CorrectWritingEx captions
#define SZCAPZIP        "Enter Postal Code"
#define SZCAPTEL        "Enter Telephone Number"

#endif //JAPAN

/****************************************************************************/

#define IDM_MAIN     	200   // menu
#define IDM_EXIT     	201
#define IDM_CLEAR    	202
#define IDM_SAMPLEDLG	203
#define IDM_LOAD     	204
#define IDM_SAVE     	205

#define IDM_NEXT  		206   // menu accelerators for TAB & SHIFT-TAB
#define IDM_PREV  		207

#define ICONJF    		300

#define IDC_SAMPLE   	400
#define IDC_EDIT     	401

#ifdef RC_INVOKED
#define ID(id) id
#else
#define ID(id) MAKEINTRESOURCE(id)
#endif

// resource ID's
#define IDJFORM  ID(1)
