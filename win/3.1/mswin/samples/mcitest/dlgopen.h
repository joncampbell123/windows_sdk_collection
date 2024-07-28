

int FAR PASCAL OpenFileDialog(
        HWND    hwndParent,     // parent window
        LPSTR   lszTitle,       // Optional dlgbox caption
        LPSTR   lszExtension,   // Optional File extensions
        DWORD   dwFlags,        // Flag values
        LPSTR   lszStatic,      // Optional static string
        LPSTR   lszPath,        // open file struct
        WORD    cbBufLen);      // length of buffer
/*
 *  flags:
 *     The LOWORD is the standard FileOpen() flags
 *     the HIWORD can be any of the following
 */
#define DLGOPEN_MUSTEXIST       0x00010000
#define DLGOPEN_NOSHOWSPEC      0x00020000
#define DLGOPEN_SAVE            0x00040000

/*
 *  Return values:
*/
#define DLGOPEN_CANCEL          -2
#define DLGOPEN_NOTFOUND        -1

/*
 *  Resource ID values for dlgopen box 
 */
#define DLGOPEN_STATIC          101
#define DLGOPEN_EDIT            102
#define DLGOPEN_FILE_LISTBOX    103
#define DLGOPEN_DIR_LISTBOX     104
#define DLGOPEN_PATH            105

//      Options particular to the "dump" dialog.

#define DLGOPEN_DUMP_OPTIONS    106
#define DLGOPEN_DUMP_FULL       107
#define DLGOPEN_DUMP_LEAVES     108

#ifdef RC_INVOKED
#include "dlgopen.dlg"
#endif

/*************************************
 *                                   *
 *  YOU MUST EXPORT THIS FUNCTION!!  *
 *                                   *
 *************************************/
BOOL FAR PASCAL  DlgfnOpen();
