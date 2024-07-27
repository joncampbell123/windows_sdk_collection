/* Private data types */
#define PASCAL pascal
#define FAR    far
#define NULL 0
typedef int ( far PASCAL * FARPROC )();
typedef unsigned int  HANDLE;
typedef unsigned int  WORD;
typedef unsigned char BYTE;
typedef BYTE far *    LPSTR;

#define FALSE 0
#define TRUE (!FALSE)
#define CBSECTOR    512
#define NOCHANGE    0
#define CHANGE      1

#define NOERROR         0
#define NOMEMORY        1
#define INVALIDDSTDRIVE 2
#define INVALIDBOOTSEC  3
#define BADSYSTEMAREA   4
#define NOTSYSABLE      5
#define SRCDISKERROR    6
#define DSTDISKERROR    7
#define INTERNALERROR   8
#define FORMATCANCELLED 9
#define NOTSYSABLE1    10
#define NOTSYSABLE2    11

#define NOSRCFILEBIAS   100
#define CANTDELETEBIAS  110
#define COPYFILEBIAS    120

/* Private include files */
#include "disk.h"
#include "file.h"

/* Windows kernel procedures used */
HANDLE      far PASCAL GlobalAlloc( HANDLE, long );
char far *  far PASCAL GlobalLock( HANDLE );
int         far PASCAL GlobalUnlock( HANDLE );
HANDLE      far PASCAL GlobalFree( HANDLE );
#define GHND 0x0060     /* GMEM_ZEROINIT | GMEM_NODISCARD | GMEM_FIXED */
