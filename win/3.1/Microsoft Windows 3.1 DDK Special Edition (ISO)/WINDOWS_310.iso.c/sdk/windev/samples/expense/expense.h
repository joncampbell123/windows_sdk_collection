/************************************************************

   PROGRAM: EXPENSE.H

   PURPOSE:
   
      Header file for EXPENSE.C


************************************************************/

/******** Macros *********/

/* Unused: To prevent CS 5.1 warning message 
*/
#define Unused(x)    x      

/******** Constants *********/

#define cchDataStrMax   32

#define chNextField     1
#define chPrecField     2

#define EmployNumDigits 5
#define DeptNumDigits   5

#define FIELDEDIT       0
#define FIELDBEDIT      1
#define FIELDPIC        2

#define DictParam_None  (-1)

#define cReportWidth       241   /* In dialog units */ 
#define cReportHeight      200   /* In dialog units */ 

#define cTexts             13
#define cEditFields        24
#define MAXWORDLISTPATH    256
#define cERRSIZE           512

#define SIG_FIELD          22    /* Index of sig field in rgeditfield array */ 

/* Dictionary Types 
*/
#define DICT_NULL          0     /* Null Dictionary */ 
#define DICT_USER          1
#define DICT_CUSTOM        2

/* Word Lists (index into rgwordlist) 
*/
#define IWORDLIST_NONE     (-1)
#define IWORDLIST_NAME     0
#define IWORDLIST_DEPTNAME 1

#define SIZE_WORDLIST      (sizeof(rgwordlist)/sizeof(WORDLIST))


/******** Typedefs *********/

typedef struct
   {
   char  *szProfileString;
   char  *szDefault;
   int   iList;
   }
   WORDLIST, *PWORDLIST, FAR *LPWORDLIST;

typedef struct
   {
   char  szDataStr[cchDataStrMax];
   int   x;
   int   y;
   }
   TEXT, *PTEXT, FAR *LPTEXT;    /* static text */
   
typedef struct
   {
   int    x;
   int    y;
   int    cx;
   int    cy;
   DWORD  dwStyle;
   ALC    alc;
   LPSTR  lpCh;          /* Characters to recognize if ALC_USEBITMAP set */ 
   LONG   lRcOptions;
   int    iDictType;
   int    iWordList;
   WORD   wFieldType;
   HWND   hwnd;
   }
   EDITFIELD, *PEDITFIELD, FAR *LPEDITFIELD;    /* edit field */

/******** Prototypes *********/

LRESULT CALLBACK ExpenseWndProc (HWND hwnd,  UINT message,  WPARAM wParam,  LPARAM lParam);
BOOL CALLBACK AboutDlgProc (HWND hDlg,  UINT message,  WPARAM wParam,  LPARAM lParam);
int FAR PASCAL ExpenseDictionaryProc (int irq, LPVOID lpIn, LPVOID lpOut, int cbMax, DWORD lContext, DWORD lD);

BOOL FInitApp (HANDLE hInstance);
BOOL FInitInstance (HANDLE hInstance,  int cmdShow);
BOOL FCreateReport (HWND hwndParent);
BOOL ProcessFieldChange (HWND hwndFocusField,  WORD wParam);
int IFromHwnd (HWND hwnd);
VOID SetAlcBits (LPBYTE rgb, LPSTR lp);
VOID CloseCustomDictionary (VOID);
VOID CloseUserDictionary (VOID);


