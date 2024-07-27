#ifdef WIN_INTERNAL
    #ifndef LSTRING
    #define NOLSTRING
    #endif
    #ifndef LFILEIO
    #define NOLFILEIO
    #endif
#endif

#ifndef NOLSTRING
int         far PASCAL lstrcmp( LPSTR, LPSTR );
LPSTR       far PASCAL lstrcpy( LPSTR, LPSTR );
LPSTR       far PASCAL lstrcat( LPSTR, LPSTR );
int         far PASCAL lstrlen( LPSTR );
LPSTR       far PASCAL lstrbscan( LPSTR, LPSTR );
LPSTR       far PASCAL lstrbskip( LPSTR, LPSTR );
#endif


#ifndef NOLFILEIO
int         far PASCAL OpenPathname( LPSTR, int );
int         far PASCAL DeletePathname( LPSTR );
int         far PASCAL _lopen( LPSTR, int );
void        far PASCAL _lclose( int );
int         far PASCAL _lcreat( LPSTR, int );
BOOL        far PASCAL _ldelete( LPSTR );
WORD        far PASCAL _ldup( int );
LONG        far PASCAL _llseek( int, long, int );
WORD        far PASCAL _lread( int, LPSTR, int );
WORD        far PASCAL _lwrite( int, LPSTR, int );

#define READ        0   /* Flags for _lopen */
#define WRITE       1
#define READ_WRITE  2
#endif

#ifndef NOATOM
/* atom manager internals */
#define ATOMSTRUC struct atomstruct
typedef ATOMSTRUC *PATOM;
typedef ATOMSTRUC {
    PATOM chain;
    WORD  usage;             /* Atoms are usage counted. */
    BYTE  len;               /* length of ASCIZ name string */
    BYTE  name;              /* beginning of ASCIZ name string */
} ATOMENTRY;

typedef struct {
    int     numEntries;
    PATOM   pAtom[ 1 ];
} ATOMTABLE;
ATOMTABLE * PASCAL pAtomTable;
#endif


/* for the control panel */
void FAR PASCAL SetCaretBlinkTime(WORD);
WORD FAR PASCAL GetCaretBlinkTime(void);

/* scheduler things that the world knows not */
BOOL        far PASCAL WaitEvent( HANDLE );
BOOL        far PASCAL PostEvent( HANDLE );
BOOL        far PASCAL KillTask( HANDLE );

/* print screen hooks */
BOOL        FAR PASCAL SetPrtScHook(FARPROC);
FARPROC     FAR PASCAL GetPrtScHook(void);


/* scroll bar messages */
#define SBM_SETPOS      WM_USER+0
#define SBM_GETPOS      WM_USER+1
#define SBM_SETRANGE    WM_USER+2
#define SBM_GETRANGE    WM_USER+3

/* module stuff */
HANDLE  FAR PASCAL LoadModule(LPSTR, LPSTR);
BOOL    FAR PASCAL FreeModule( HANDLE );
HANDLE  FAR PASCAL GetDSModule( WORD );
HANDLE  FAR PASCAL GetDSInstance( WORD );
