#define LWORD(x)        ((short)((x)&0xFFFF))


/* spooler error code */
#define SP_ERROR            (-1)    /* general error - mostly used when spooler isn't loaded */
#define SP_APPABORT         (-2)    /* app aborted the job through the driver */
#define SP_USERABORT        (-3)    /* user aborted the job through spooler's front end */
#define SP_OUTOFDISK        (-4)    /* simply no disk to spool */
#define SP_OUTOFMEMORY      (-5)
#define SP_RETRY            (-6)    /* retry sending to the port again  */
#define SP_NOTREPORTED      0x4000  /* set if GDI did not report error */

#define SP_PRINTERNAME      20
#define SP_REGISTER         21
#define SP_CONNECTEDPORTCNT 25
#define SP_QUERYDISKUSAGE   26
#define SP_DISKFREED        27
#define SP_INIT             28
#define SP_LISTEDPORTCNT    29
#define SP_DISK_BUFFER      (20000) /* wait for about 20 K of disk space to free
                                       free up before attempting to write to disk */

#define SP_NEWJOB           0x1001
#define SP_DELETEJOB        0x1002
#define SP_DISKNEEDED       0x1003

/* in /windows/oem/printer.h */
#define SP_QUERYDISKAVAIL   0x1004

/* job status */
#define JB_ENDDOC           0x0001
#define JB_INVALIDDOC       0x0002
#define JB_DIRECT_SPOOL     0x8000  /* go directly to the printer without the spooler */
#define JB_FILE_PORT        0x4000  /* were given a file for a port name */
#define JB_VALID_SPOOL      0x2000  /* everything is cool, continue to spool normally */
#define JB_NOTIFIED_SPOOLER 0x1000  /* already notified the spooler of this job */
#define JB_WAITFORDISK      0x0800  /* out of disk condition has been detected previously */
#define JB_DEL_FILE         0x0400  /* no deletion of file after spool          */
#define JB_FILE_SPOOL       0x0200  /* spooling a file          */

/* allow 2 dialog box messages initially and increment 8 at a time */
#define SP_DLGINC       8
#define SP_DLGINIT      8

#define NAME_LEN        32
#define BUF_SIZE        128
#define MAX_PROFILE     80
#define JCBBUF_LEN      256

#define lower(c)        ((c > 'A' && c < 'Z') ? (c - 'A' + 'a') : c)

/* win.ini constants to determine whether the spooler should come in */
#define WININI_SECTION  "windows"
#define WININI_LHS      "spooler"
#define WININI_RHS      "no"

/* comm driver stuff */

#define IDS_LENGTH          40

#define COMM_INQUE          0x010                       /* wm091385 */
#define COMM_OUTQUE         0x030                       /* wm091385 */
#define COMM_ERR_BIT        0x8000
#define TXTIMEOUT           45000               /* milliseconds */
#define DNSTIMEOUT          15000               /* milliseconds */

#define BAUDRATE            0
#define PARITY              1
#define BYTESIZE            2
#define STOPBITS            3
#define REPEAT              4


typedef struct {
    short   type;           /* type of dialog. This will tell whether it is */
                            /* call back function or pure dialog etc        */
    short   size;           /* size of special function data                */
    short   adr;
}DIALOGMARK;


#define SP_TEXT         0   /* text type                                */
#define SP_NOTTEXT      1   /* not text type                            */
#define SP_DIALOG       2   /* dialog type data                         */
#define SP_CALLBACK     3   /* call back type function                  */



#define MAXPORTLIST 10  /* allow 10 ports to be listed under win.ini */
#define MAXPORT     8   /* only allow 8 ports to be connected */
#define MAXSPOOL    20
#define MAXMAP      18
#define PORTINDENT   2
#define JOBINDENT    3
#define MAXPAGE     7     /* allow 7 pages at first */
#define INC_PAGE    8     /* increase by 8 pages at a time */

typedef struct {
    short pnum;
    short printeratom;
}JCBQ;

typedef struct jcb {
    unsigned        type;
    short           pagecnt;
    short           maxpage;
    short           portnum;
    HANDLE          hDC;
    short           chBuf;
    char            buffer[JCBBUF_LEN];
    unsigned long   size;
    char            jobName[NAME_LEN];
    short           page[MAXPAGE];
}JCB;

typedef struct page {
    short    filenum;
    unsigned maxdlg;                    /* max number of dialog */
    unsigned dlgptr;                    /* number of dialogs */
    long     spoolsize;
    OFSTRUCT fileBuf;
    DIALOGMARK  dialog[SP_DLGINIT];
}PAGE;

#define SP_COMM_PORT    0
#define SP_FILE_PORT    1

typedef struct
{
        short type;
        short fn;
        long  retry;            /* system timer on first error  */
}   PORT;

#ifdef library

/*   _SPOOL library routines */

JCB FAR * NEAR PASCAL IsJobValid(HANDLE);
void  NEAR PASCAL NameSpoolFile(LPSTR, short, short);
short NEAR PASCAL WriteFile(JCB far *, PAGE far *, LPSTR, short);
void  NEAR PASCAL lstrncpy(LPSTR, LPSTR, short);
short NEAR PASCAL FreeAll(HANDLE, JCB far *);
short FAR  PASCAL OutOfDiskHandler(HANDLE, LPSTR, short, short);
short FAR  PASCAL FindAllPorts();

/* job queue control routines */

FAR removecolon(LPSTR, LPSTR);
FAR ValidPort(LPSTR);

short FAR FindAtom(LPSTR);
short FAR AddAtom(LPSTR);
short FAR GetAtomName(short, LPSTR, short);
short FAR PASCAL FindPort(short);
FAR FindPrinterNames(LPSTR);

#endif

/* exported routines */
LONG FAR PASCAL GetSpoolJob(short, long);
char FAR PASCAL GetSpoolTempDrive();

/* imported routines */
