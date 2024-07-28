/**[f******************************************************************
* $sflib.h
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
typedef struct
{                       /* -------------------------------------------- */
    HANDLE hFiles;      /* Handle to memory containing all file names    */
    WORD length;        /* Length of memory                              */
    WORD NextEntry;     /* Next free space to store file name in memory  */
    int NumFiles;       /* Number of file names stored in block          */
} DIRECTORY, FAR * LPDIRECTORY;
  
  
  
typedef struct
{
    int   Type;
    WORD  Length;
    BOOL Selected;
    int  usage;
    TYPEINFO TypeInfo;
    DWORD ListboxEntry;
    WORD name_off;
    WORD OffsetName;
    WORD OffsetPath;
} SFI_FONTLIBENTRY, far *LPSFI_FONTLIBENTRY;
  
  
/* Fields taken out of the above structure
since we dont really use them
*/
//       LPSTR next;
//       LPSTR prev;
//       int   Listbox;
//       SCRNFNTINFO ScrnFntInfo;
//       char ScreenSizes[MAX_NUMBER_OF_SCREEN_FONTS+1];
//       int ScreenType;
  
  
#define MAX_NUMBER_OF_SCREEN_FONTS 50
  
typedef struct
{
    char Name[64];
    BOOL IsDJFont;
    char Creator[16];
    int DisplayRes;
    char SymbolSet[5];
    char ScreenSizes[MAX_NUMBER_OF_SCREEN_FONTS+1];
} SCRNFNTINFO, FAR * LPSCRNFNTINFO;
  
  
  
  
BOOL FAR PASCAL AddDirEntry(LPDIRECTORY,LPSTR,LPSTR,LPSTR,LPSTR,int,int,LPTYPEINFO, LPSCRNFNTINFO);
  
  
#ifdef DEBUG
VOID FAR PASCAL DBGLibEntry(LPSFI_FONTLIBENTRY, LPSTR);
#endif
