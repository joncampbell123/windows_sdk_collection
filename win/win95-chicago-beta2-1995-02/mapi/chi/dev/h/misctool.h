/*
 -  M I S C T O O L . H
 -
 *  Purpose:
 *      Misc. functions to support the sample applications.
 */

#ifndef __misctool_h_
#define __misctool_h_

#ifdef __cplusplus
extern "C" {
#endif

int     EcStringFromFile(LPSTR *lppsz, LPSTR lpszFileName, LPVOID lpv);
int     EcBinaryFromFile(LPSBinary, LPSTR, LPVOID);
LPSTR   SzIIDToString(LPSTR, LPIID);
LPSTR   SzGetPropValue(LPSTR, LPSPropValue);
LPTSTR  SzGetEntryID(LPTSTR lpsz,LPENTRYID lp, ULONG cb);
LPSTR   SzTextizeProp(LPSTR lpszRet, LPSPropValue lpProp);
LPSTR   SzGetPropTag(LPSTR lpsz, ULONG ulPropTag);
void    CopyPropValue(LPSPropValue, LPSPropValue, LPVOID lpv);
void    CopyGuid(LPGUID, LPGUID);
BOOL    MakePropValue(LPSPropValue, ULONG, LPSTR, LPVOID lpv);
BOOL    FreeRowSet(LPSRowSet lpRows);
void    MakeGuid(LPGUID, ULONG, WORD, WORD, BYTE, BYTE,
                 BYTE, BYTE, BYTE, BYTE, BYTE, BYTE);


BOOL FSeekTag( FILE *pFile, LPTSTR lpszTag );

/* NOW IN MAPIW.LIB ?
#ifdef WIN16
BOOL    WINAPI FileTimeToSystemTime(FILETIME FAR *pft, SYSTEMTIME FAR *pst);

BOOL    WINAPI SystemTimeToFileTime(SYSTEMTIME FAR *pst, FILETIME FAR *pft);

void    WINAPI GetSystemTime(SYSTEMTIME FAR *pst);


//BOOL WINAPI GetFileTime(HFILE hFile, LPVOID lpftCreation, LPVOID lpftLastAccess,
//  LPVOID lpftLastWrite);


#endif


*/

BOOL    FIsEntryID(ULONG ulPropTag);

LPIID   GetLPIID( LPMAPIPROP  lpObj );

void    String8ToUnicode( LPSTR    szMultiByte,
                       LPWSTR   *lppWideChar,
                       void FAR *lpvParent);


BOOL    PASCAL AsciiToHex (ULONG len, char *pch, ULONG * lpulOut);

// PROPERTY READING/WRITING TO FILE

/* OLD
void WritePropValArray( LPSTR           lpszFileName,
                        LPSTR           lpszTag,
                        ULONG           cValues,
                        LPSPropValue    lpspva);

*/

// NEW

// used to indicate in WritePropValArray that you
// want to dump the actual binary hex values instead
// of the dummy binary stub to file.
#define DUMP_BINARY_DATA        0x0001
#define WRITE_NEW_DATA_ONLY     0x0010


void WritePropValArray( LPSTR           lpszFileName,
                        LPSTR           lpszTag,
                        ULONG           cValues,
                        LPSPropValue    lpspva,
                        ULONG           ulFlags);


void WritePropTagArray( LPSTR           lpszFileName,
                        LPSTR           lpszTag,
                        LPSPropTagArray lpspta);

void WriteProblemArray( LPSTR               lpszFileName,
                        LPSTR               lpszTag,
                        LPSPropProblemArray lpProblem);




// BRENTK PUT READ ROUTINES HERE





#ifdef __cplusplus
}
#endif

#endif //__misctool_h_
