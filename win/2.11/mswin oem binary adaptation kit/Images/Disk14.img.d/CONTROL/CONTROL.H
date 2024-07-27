/* menu mnemonics (for hack in paneldlgfilter()) */
/* these must be the mnemonics for the top level menus, in order, capitalized */
#define MENUMNEMONIC1   'I'
#define MENUMNEMONIC2   'S'
#define MENUMNEMONIC3   'P'

/* indicies into the winini string array rglpszWinIni[]. 17-Sep-1987. */
#define WININIWINDOWS     0
#define WININICOLORS      1
#define WININIDEVICES     2
#define WININIFONTS       3
#define WININIPORTS       4
#define WININIINTL        5
#define CWININIENTRIES    6
#define CBWININIENTRIES   48

/* panel constants */

#define  NOSELECT -1		    /* indices for int Selected; */
#define  HOUR	  0		    /* index into rDateTime, wDateTime, wRange */
#define  MINUTE   1
#define  SECOND   2
#define  MONTH	  3
#define  DAY	  4
#define  YEAR	  5
#define  UPTIME  6
#define  DOWNTIME 7
#define  UPDATE  8
#define  DOWNDATE 9

#define AM_PM  2		    /* index into rTimeSep */

#define  CLICKMIN    100      /* milliseconds */
#define  CLICKMAX    900
#define  CLICKSUM    (CLICKMIN+CLICKMAX)
#define  CLICKRANGE  (CLICKMAX-CLICKMIN)
#define  CURSORMIN   200
#define  CURSORMAX   1200
#define  CURSORSUM   (CURSORMIN+CURSORMAX)
#define  CURSORRANGE (CURSORMAX-CURSORMIN)

/* other useful constants */
#define MAX_PRINTERS 20       /* maximum number of installable printers */
#define MAX_FONTS    25       /* maximum number of installable fonts */
#define FILEMAX 13	      /* max length filename with extension */
#define PATHMAX 128	      /* path length max */
#define DESCMAX 129	      /* max description in newexe header */

#define OUT_OF_MEM   -1       /* current undocumented DlgBox() out of mem
				 return */
#define FOO 10		      /* for useless control ids */

/* TYPEDEFS */
typedef struct {
    char reserved[21];
    char attr;
    unsigned time;
    unsigned date;
    long length;
    char name[FILEMAX];
}  FINDTYPE;

typedef struct {
    char name[PATHMAX];
    char desc[DESCMAX];
    char ModName[12];
    int ModType;
    } BUFTYPE;
typedef BUFTYPE *PBUFTYPE;

/* macros */
#define XCOORD(x)	((short int)((x)&0xFFFF))
#define YCOORD(y)	((short int)(((y)>>16)&0xFFFF))


/* EXTERNS */
extern	LPSTR	    far PASCAL lstrcpy( LPSTR, LPSTR );
extern	LPSTR	    far PASCAL lstrcat( LPSTR, LPSTR );
extern	int	    far PASCAL lstrcmp( LPSTR, LPSTR );
extern	int	    far PASCAL lstrlen( LPSTR );

#ifdef INCLUDE_UTILITIES
extern	    FAR doscall();
extern int  FAR ffirst( PSTR, int, FINDTYPE * );
extern int  FAR fnext( FINDTYPE * );
extern int  FAR Remove( LPSTR );
extern int  FAR myread( int, LPSTR, int );
extern int  FAR mywrite( int, LPSTR, int );
extern int  FAR close( int );
extern int  FAR chdir( LPSTR );
extern int  FAR GetCurDir( PSTR, int );
extern int  FAR GetCurDrive();
extern int  FAR curdir( LPSTR, int );
extern LONG FAR GetFileTime( int );
extern	    FAR SetFileTime( int, LONG );

extern void FAR itoa( int, char * );
extern BOOL FAR FillWithPrinters( HWND );
extern BOOL FAR Exists( char * );
extern char * FAR strscan( char *, char * );
extern FAR StripBlanks( char * );
extern FAR TellTheWorld( );
extern FAR IsNewExe( PBUFTYPE );
extern BOOL FAR Copy( HWND, char *, char * );
extern BOOL FAR RootPath( LPSTR, LPSTR );
extern FAR GetFirstFloppy();
extern FAR BorderRect( HDC, LPRECT, HBRUSH );
extern FAR ErrMemDlg();
extern FAR ErrWinDlg();
extern int FAR CheckMemDlgBox( HANDLE, LPSTR, HWND, FARPROC );
extern int FAR AlertBox( HWND, LPSTR, LPSTR, LPSTR, unsigned );

#endif


/* stringtable defs */
#define INITS	     0
#define ADDPRT	     16
#define DELPRT	     32
#define ADDFONT      48
#define DELFONT      64

#define COPY	     80       /* "sharing" the same resource chunk */
#define CONNECT      88       /* "sharing" the same resource chunk */

#define SETUP	     96       /* "sharing" the same resource chunk */
#define COMM	     100      /* "sharing" the same resource chunk */
#define UTILS	     104      /* "sharing" the same resource chunk */

#define COLOR	     112
#define FIRST_COUNTRY   144   /* country strings */

#define COUNTRY      176      /* country messages */

/* dialog template defs */
#define PANELDLG	   1
#define ABOUT		   2
#define ADD10		   3
#define ADD20DEL10	   4
#define ADD30DEL20	   5
#define CONNECTDLG	   6
#define PRTDLG		   7
#define COMMDLG 	   8
#define COLORDLG	   9
#define MOUSEDLG	   10
#define COUNTRYDLG	   11
#define BORDERDLG          12
#define KEYBOARDDLG	   13

/* menu ID defs */
#define MENU_CONNECTIONS   1
#define MENU_ADDPRT	   2
#define MENU_DELPRT	   3
#define MENU_PRINTERS	   4
#define MENU_PORTS	   5
#define MENU_COLORS	   6
#define MENU_BEEP	   7
#define MENU_COUNTRY	   8
#define MENU_ADDFONT	   9
#define MENU_DELFONT	   10
#define MENU_MOUSE	   11
#define MENU_ABOUT	   12
#define MENU_EXIT          13
#define MENU_BORDER        14
#define MENU_KEYBOARD	   15

/* timer defs */
#define BLINK		1
#define SECOND		2
#define OPEN_TLEN 450

/* control ID defs */

#define  THE_LISTBOX	20	       /* general use */
#define  PUSH_OK	IDOK
#define  PUSH_RESET	21
#define  PUSH_CANCEL	IDCANCEL
#define  PUSH_SETUP	23
#define  EDIT_PATH	26
#define  EDIT_FILE	28

#define FILEBOX 	220	       /* add and delete dialogs */
#define AVAIL		230
#define DISK		240
#define FILETYPE	250
#define SRCDEST 	260


#define LBOX_PORT	300	       /* connections dialog */
#define LBOX_PRINTER	310

#define TIME		400	       /* panel dialog */
#define SCROLL_TIME	405
#define DATE		410
#define SCROLL_DATE	415
#define SCROLL_CURSOR	420
#define CURSOR		430
#define SCROLL_CLICK	440
#define CLICK		450

#define COLOR_HUE	700	       /* color dialog */
#define COLOR_COLOR	710
#define COLOR_BRIGHT	720
#define SAMPLE_STRING 705

#define  CHK_MOUSE	150	       /* mouse dialog */
#define  RB_NONE        151
#define  RB_MEDIUM      152
#define  RB_HIGH        153

#define  EDIT_DEV       160         /* default printer dialog */
#define  EDIT_TRANS     161

#define EDIT_WIDTH      170            /* border width dialog */

#define KSPEED_SCROLL	180		/* keyboard speed scroll bar */
#define KSPEED_EDIT	181		/* keyboard speed edit control */


/* The following items in the comm ports dlg must be a contiguous block of IDs
   from COMMSET_FIRST to COMMSET_LAST.
   Similarly for COMMPORT_FIRST to COMMPORT_LAST.
   The order within the "settings" range is expected to be
      parity
      word length
      stop bits
      handshaking
   The order within ports is unimportant.
*/
#define  PAR_EVEN	801		    /* Even parity */
#define  PAR_ODD	802		    /* Odd parity */
#define  PAR_NONE	803		    /* No parity */
#define  WDLEN4 	804
#define  WDLEN5 	805
#define  WDLEN6 	806
#define  WDLEN7 	807
#define  WDLEN8 	808
#define  STOPBITS1	809		    /* One stop bit */
#define  STOPBITS15	810		    /* 1.5 stop bits */
#define  STOPBITS2	811		    /* Two stop bits */
#define  SHAKE_HWARE	812
#define  SHAKE_NONE	813

#define  BAUD		820	       /* communications ports dialog */

#define  PORT_C1	830
#define  PORT_C2	831

#define  COMMSET_FIRST	PAR_EVEN
#define  COMMSET_LAST	SHAKE_NONE
#define  COMMPORT_FIRST PORT_C1
#define  COMMPORT_LAST	PORT_C2

/* verify following */
#define DEF_BAUD     "1200"		       /* DEFINE DEFAULT CHOICES */
#define DEF_WORD     WDLEN8
#define DEF_PARITY   PAR_NONE
#define DEF_STOP     STOPBITS1
#define DEF_PORT     PORT_C1



#define COUNTRY_CODE	900	       /* country dialog */
#define CURRENCY_SYMBOL 901
#define PREFIX		902
#define SUFFIX		903
#define DECIMAL_DIGITS	904
#define DECIMAL_SEP	905
#define LIST_SEP	906
#define MDY		907
#define DMY		908
#define YMD		909
#define DATE_SEP	910
#define PM_AM		911
#define TWENTY_FOUR_HR	912
#define TIME_SEP	913
#define THOUSANDS_SEP	914
#define STRING_AM	915
#define STRING_PM	916
#define LEADING_ZERO	917
#define COUNTRY_RESET	919
#define COUNTRY_SAMPLES 921
