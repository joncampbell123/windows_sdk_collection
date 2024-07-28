/*--------------  FileOpen Commmon Dialog Box Define ---------------*/

#define IDD_INFO            100

/*--------------  FileSave Commmon Dialog Box Define ---------------*/

#define IDD_FILETYPE        101
#define IDD_RGB             102
#define IDD_RLE4            103
#define IDD_RLE8            104
#define IDD_PM              105
#define IDD_1               106
#define IDD_4               107
#define IDD_8               108
#define IDD_24              109
#define IDD_FILETYPEGROUP   110
#define IDD_BPP             111

/*--------------  Dib Information Dialog Box Defines ---------------*/

#define IDD_NAME            200
#define IDD_FORMAT          201
#define IDD_WIDTH           202
#define IDD_HEIGHT          203
#define IDD_COLORS          204
#define IDD_COMPRESS        205
#define IDD_COMPHEAD        206


/*--------------  DIB header Marker Define -------------------------*/

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')


/*--------------  MyRead Function Define ---------------------------*/

// When we read in a DIB, we read it in in chunks.  We read half a segment
//  at a time.  This way we insure that we don't cross any segment
//  boundries in _lread() during a read.  We don't read in a full segment
//  at a time, since _lread takes some "int" type parms instead of
//  WORD type params (it'd work, but the compiler would give you warnings)...

#define BYTES_PER_READ  32767

/*--------------  Define for PM DIB  -------------------------------*/
// The constants for RGB, RLE4, RLE8 are already defined inside
// of Windows.h

#define BI_PM       3L


/*-------------- Magic numbers -------------------------------------*/
// Maximum length of a filename for DOS is 128 characters.

#define MAX_FILENAME 129


/*--------------  TypeDef Structures -------------------------------*/

typedef struct InfoStruct
  {
  char  szName[13];
  char  szType[15];
  DWORD cbWidth;
  DWORD cbHeight;
  DWORD cbColors;
  char  szCompress[5];
  }  INFOSTRUCT;

typedef WORD (CALLBACK* FARHOOK)(HWND,UINT,WPARAM,LPARAM);
/*--------------  Global Variables ---------------------------------*/

extern char szFileName[256];        // Filename of DIB (valid iff (hDIB))
char   szDirName[256];              // Directory name
WORD   wDibType;                    // Type of Dib
DWORD  biStyle;                     // Type of Dib - RGB, RLE4, RLE8, PM
WORD   biBits;                      // bits per pixel

/*--------------  Local Function Prototypes ------------------------*/

HANDLE   OpenDIBFile       (LPSTR szFileName);
BOOL     GetFileName       (LPSTR, WORD);
HANDLE   ReadDIBFile       (int);
BOOL     MyRead            (int, LPSTR, DWORD);
int      CheckIfFileExists (char *);
int      GetDibInfo        (char *, INFOSTRUCT *);
BOOL     SaveDIBFile       (void);
WORD     ExtractDibType    (INFOSTRUCT *);
WORD     ExtractDibBits    (INFOSTRUCT *);
HANDLE   WinDibFromBitmap  (HBITMAP, DWORD, WORD, HPALETTE);
HANDLE   PMDibFromBitmap   (HBITMAP, DWORD, WORD, HPALETTE);
BOOL     WriteDIB          (LPSTR, HANDLE);
VOID     ParseCommandLine  (LPSTR);
HANDLE   GetDIB            (void);
DWORD PASCAL lwrite        (int, VOID FAR *, DWORD);

/*--------------  Exported Function Prototypes ---------------------*/

BOOL FAR PASCAL FileOpenHookProc (HWND, WORD, WORD, LONG);
BOOL FAR PASCAL FileSaveHookProc (HWND, WORD, WORD, LONG);
BOOL FAR PASCAL InfoDlgProc      (HWND, WORD, WORD, LONG);
