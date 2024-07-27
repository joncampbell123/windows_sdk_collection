#include "spoolids.h"

#define BEEPFREQ    10      /* beep on every 10th flash */
#define SPOOL_LEN   256
#define LOW         10
#define HIGH        11
#define PAUSE       20
#define RESUME      21
#define RESTART     22
#define TERMINATE   23
#define ABOUT       24
#define EXIT        25

#define CHAR_TRANS  8
#define INIT_TRANS  1000
#define MESSAGE_LEN 9

#define SP_VALID        0x01
#define SP_JCBLOCKED    0x02
#define BUFFERLENGTH    512

/* message box stuff */
#define MB_OK                   0x0000
#define MB_OKCANCEL             0x0001
#define MB_ABORTRETRYIGNORE     0x0002
#define MB_YESNOCANCEL          0x0003
#define MB_YESNO                0x0004
#define MB_RETRYCANCEL          0x0005

#define MB_ICONHAND             0x0010
#define MB_ICONQUESTION         0x0020
#define MB_ICONEXCLAMATION      0x0030
#define MB_ICONASTERISK         0x0040

#define MB_DEFBUTTON1           0x0000
#define MB_DEFBUTTON2           0x0100
#define MB_DEFBUTTON3           0x0200

#define MB_APPLMODAL            0x0000
#define MB_SYSTEMMODAL          0x1000
#define MB_NOFOCUS              0x8000
#define MB_MISCMASK             0xc000
#define MB_TYPEMASK             0x000f
#define MB_ICONMASK             0x00f0
#define MB_DEFMASK              0x0f00
#define MB_MODEMASK             0x3000

#define EOF     (-1)

#define MAXPORTLEN  80
#define DIALOG  0
#define TEXT    1

#define MAXTRY  8

#define SP_USERPAUSE   0x01
#define SP_CLEARUSER   (~SP_USERPAUSE)
#define SP_SYSTEMPAUSE 0x02
#define SP_CLEARSYSTEM (~SP_SYSTEMPAUSE)
#define SP_ATTENTIONPAUSE 0x04
#define SP_CLEARATTENTION (~SP_ATTENTIONPAUSE)

#if 0
/*  in /windows/oem/spool.h */

/* win.ini constants to determine whether the spooler should come in */
#define WININI_SECTION  "windows"
#define WININI_LHS      "spooler"
#define WININI_RHS      "no"

typedef struct
{
        short type;
        short fn;
        long  retry;        /*   system timer on first error */
}   PORT;

#endif

typedef struct  server
{
        short   pause;
        short   portfn;
        short   type;
        short   valid;      /* printer has been removed from this port */
        HANDLE  hJCB;
        short   pg;
        short   dlgptr;
        unsigned long    filestart;
        unsigned long    fileend;
        short   bufstart;
        short   bufend;
        HANDLE  queue[MAXSPOOL];
        short   jobcnt;
        char    portname[NAME_LEN];
        short   portlength;
        char    printername[NAME_LEN];
        short   printerlength;
	char	buffer[SPOOL_LEN + 1];
}       SERVER;

typedef JCB far *FJCB;

NEAR PASCAL FreeAll(HANDLE);
NEAR PASCAL UpdateScreen();
NEAR PASCAL AddSpoolJob(SERVER *, HANDLE, short);
short  NEAR PASCAL SpoolRead(PAGE far *, SERVER *, short);
BOOL   NEAR PASCAL Refill(SERVER *, FJCB);
short  Output(SERVER *);

/*  Spooler task routines */

/*  routines to output to the port */
short NEAR PASCAL line_out(short, char *, short, short);
short NEAR PASCAL PostDialog(JCB far *, DIALOGMARK far *);
long near PASCAL CalcDiskAvail();

short FAR PASCAL OpenPort(LPSTR);
short FAR PASCAL ResetPort(short);
short FAR PASCAL WritePort(short, LPSTR, short);
short FAR PASCAL ClosePort(short);

long FAR PASCAL SpoolerWndProc(HWND, unsigned, WORD, LONG);
FAR SpoolerKey(HWND, WORD);
FAR InvertLine(HDC, short, short);
FAR SpoolerVertScroll(HWND, short, short);
short FAR SpoolerInit(HANDLE, LPSTR);
FAR SpoolerCreate(HWND);
near SpoolerError(short, short, short, LPSTR);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

extern short maxline;
extern short curjob, tos;
extern short charheight, charwidth;
extern short maxport;
extern HANDLE screen[];
extern HWND   hWnd, hInst;
extern SERVER *servers[];
extern short ActiveJobs;
extern char SpoolerName[];
extern char OutOfMemString[];
