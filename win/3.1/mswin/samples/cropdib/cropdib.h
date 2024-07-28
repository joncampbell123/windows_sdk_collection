/* CropDIB.h
 */


/* dialog boxes */
#define ABOUTBOX                1
#define CROPDIBBOX              2


/* menu items */
#define IDM_ABOUT               11


/* CROPDIBBOX controls */
#define ID_INPUTFILEEDIT        101     // input file or pattern
#define ID_OUTPUTFILEEDIT       102     // output file or pattern
#define ID_XEDIT                103     // x-coord of crop box
#define ID_YEDIT                104     // y-coord of crop box
#define ID_WEDIT                105     // width of crop box
#define ID_HEDIT                106     // height of crop box
#define ID_BRIGHTEDIT           107     // brighten by
#define ID_STATUSTEXT           108     // status information


/* macros */
#define MB_ERROR        (MB_ICONEXCLAMATION | MB_OK)
#define Error(hwnd, idsFmt)                                     \
        ( ErrMsg(hwnd, (idsFmt)), FALSE )
#define Error1(hwnd, idsFmt, arg1)                              \
        ( ErrMsg(hwnd, (idsFmt), (arg1)), FALSE )
#define Error2(hwnd, idsFmt, arg1, arg2)                        \
        ( ErrMsg(hwnd, (idsFmt), (arg1), (arg2)), FALSE )


/* strings */
#define IDS_APPNAME             300     // CropDIB
#define IDS_CANTOPENFILE        301     // Can't open file '%s'
#define IDS_ERRORWRITING        302     // Error writing file '%s'
#define IDS_ERRORREADING        303     // Error reading file '%s'
#define IDS_OUTOFMEMORY         304     // Out of memory
#define IDS_INTERNALERROR       305     // Internal error %d
#define IDS_CANTLOADDIBDRV      306     // Cannot load DIB.DRV
#define IDS_FORMATNOTSUPPORTED 307 // Can't work with a 24-bit DIB
