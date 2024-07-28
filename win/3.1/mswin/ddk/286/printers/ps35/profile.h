/*
 * profile.h
 *
 */

/* bit fields for booleans in advanced dialog */
#define ADVF_NEGIMAGE   1
#define ADVF_PERPAGE    2
#define ADVF_DSC        4
#define ADVF_TYPE1      8
#define ADVF_SUBFONTS   16
#define ADVF_MIRROR     32
#define ADVF_COLORTOBLACK   64
#define ADVF_COMPRESS   128
#define ADVF_TRUETYPE   256
#define ADVF_ERRHANDLER 512
#define ADVF_NODOWNLOAD 1024
#define ADVF_TYPE3      2048

// default to no compression for PS printers 2/14/92
#define ADVF_DEFAULTS	(ADVF_TYPE1 | ADVF_COMPRESS | ADVF_SUBFONTS | ADVF_ERRHANDLER)
#define ADVF_TIDEFAULTS (ADVF_TRUETYPE | ADVF_COMPRESS | ADVF_SUBFONTS | ADVF_ERRHANDLER)

short FAR PASCAL GetDefaultPaper(void);

BOOL FAR PASCAL MakeEnvironment(LPSTR, LPSTR, LPPSDEVMODE, LPSTR);
void FAR PASCAL SaveEnvironment(LPSTR, LPSTR, LPPSDEVMODE, LPPSDEVMODE, LPSTR, BOOL, BOOL);

int	FAR PASCAL GetExternPrinter(int i);
void	FAR PASCAL GetExternPrinterFilename(int i, LPSTR lpFilename);
int	FAR PASCAL MatchPrinter(LPSTR lpName);

// Substitution entry consists of pairs of character array with array
// length == LF_FACESIZE

typedef struct
    {
    char rgTTFont[LF_FACESIZE];
    char rgDevFont[LF_FACESIZE];
    } SUBENTRY, *PSUBENTRY, FAR *LPSUBENTRY;

// Substitution table consists of a word indicating how many substitution
// entries.
typedef struct
    {
    int     nSub;
    BOOL    bUser;
    int     MaxSub;
    SUBENTRY	SubEnt[1];
    } SUBTAB, * PSUBTAB, FAR *LPSUBTAB;

LPSUBTAB FAR PASCAL LockSubTable(LPDV, int);
void FAR PASCAL UnlockSubTable(LPDV);
void FAR PASCAL FreeSubTable(LPDV);
void FAR PASCAL EraseSubTable(LPDV);
void FAR PASCAL WriteSubTable(LPDV);
BOOL FAR PASCAL FindSubFont(LPDV lpdv, BOOL bUser, LPSTR lpszTTFont, LPSTR lpszDevFont);
BOOL   FAR PASCAL UpdateSubTable(LPDV lpdv, HWND hwndTT, HWND hwndDV);
