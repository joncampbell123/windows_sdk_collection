/*
 * utils.h	defs of utility functions found in utils.c
 *
 */


int	FAR PASCAL Scale(int, int, int);	/* scale.asm */

long FAR PASCAL lmul(long, long);
long FAR PASCAL ldiv(long, long);
long FAR PASCAL lmod(long, long);

void FAR PASCAL lstrncat   (LPSTR, LPSTR, int);
void FAR PASCAL lmemcpy    (void FAR *, void FAR *, int);
void FAR PASCAL lmemset    (void FAR *, BYTE, int);
BOOL FAR PASCAL lmemIsEqual(void FAR *, void FAR *, int);

void FAR PASCAL ClipBox(LPDV, LPRECT);

void FAR PASCAL lsfpfmcopy(LPSTR, LPSTR);      	/*** rb BitStream ***/
void FAR PASCAL lsfloadpathcopy(LPSTR, LPSTR);	/*** rb BitStream ***/
void FAR PASCAL lbitmapext(LPSTR);		/*** rb BitStream ***/
void FAR PASCAL GetProfileStringFromResource(short, short, short, short, LPSTR, short);
BOOL FAR PASCAL isUSA(void);

LPSTR FAR PASCAL SetKey(LPSTR lpKey, LPSTR  lpDevName, LPSTR lpFile);



#define ATTR_READONLY   0x0001
#define ATTR_HIDDEN     0x0002
#define ATTR_SYSTEM     0x0004
#define ATTR_VOLUME     0x0008
#define ATTR_DIR        0x0010
#define ATTR_ARCHIVE    0x0020
#define ATTR_FILES      (ATTR_READONLY+ATTR_SYSTEM)
#define ATTR_ALL_FILES  (ATTR_READONLY+ATTR_SYSTEM+ATTR_HIDDEN)
#define ATTR_ALL        (ATTR_READONLY+ATTR_DIR+ATTR_HIDDEN+ATTR_SYSTEM)

typedef struct {
    char        Reserved[21];
    BYTE        Attr;
    WORD        Time;
    WORD        Date;
    DWORD       Length;
    char        szName[13];
}   FCB;

typedef FCB     * PFCB;
typedef FCB FAR * LPFCB;

int FAR PASCAL DosFindFirst(LPFCB lpfcb, LPSTR szFileSpec, WORD attr);
int FAR PASCAL DosFindNext(LPFCB lpfcb);
