/*
 *   Windows Calendar
 *   Copyright (c) 1985 by Microsoft Corporation, all rights reserved.
 *   Written by Mark L. Chamberlin, consultant to Microsoft.
 *
*/

/*
 *****
 ***** cal.h
 *****
*/

/* Get rid of some stuff from windows.h. */
#define NOGDICAPMASKS
#define NOATOM
#define NOWH
#define NOSOUND
#define NOATOM
#define NOCOMM
#define NOMETAFILE
#define NOWINOFFSETS
#define NOREGION
#define NOKANJI

#include "windows.h"
#define LSTRING
#include "winexp.h"
#include "date.h"        /* Common date/time stuff */
#include "io.h" 	 /* Get I/O function declarations. */
#include "stdlib.h"	 /* Get declaration for errno. */
#include "errno.h"	 /* Get definitions of error codes. */


/* Note - I have avoided using frequently occuring sequences of letters
   for data types.  While the loss of mnemonic value is regrettable,
   it is less important than being able to easily search for occurences of
   the data type.  For example, an appointment record is QR instead of AR,
   because searching for "ar" doesn't work well (char, far, and all sorts
   of English words in comments).
*/

typedef unsigned DT;	      /*** DT - date packed into 2 bytes - the number
				   of days since January 1, 1980.
			      */
#define DTNIL 0xFFFF	      /* An impossible DT.  Note that by making this
				 the largest possible value, it can safely
				 be used in vftAlarmFirst.dt and
				 vftAlarmNext.dt and have comparisons work
				 properly.  For example, if vftAlarmNext.dt
				 == DTNIL, there is no next alarm, and
				 the current FT will never look greater or
				 equal, so the alarm can't go off.
			      */
#define DTFIRST 0	      /* The DT for January 1, 1980. */
#define DTLAST 0xAB35	      /* The DT for December 31, 2099. */

typedef WORD OBK;	      /*** Offset in Blocks within a file -
				   From 0 through 32765, so it
				   uses only 15 bits and does not include
				   DLNIL or DLNOCHANGE.
				*/

typedef WORD DL;	      /*** DL - Date Location.	Bit 15 is 0
				   if the date is in the original file,
				   1 if it's in the change file.  Bits
				   0 through 14 contain the OBK.
				   If the date is in neither file, the
				   DL is DLNIL.
			      */
#define DLNIL 0xFFFF	      /* An impossible DL - it means the date is
				 not in a file.
			      */
#define DLNOCHANGE 0xFFFE     /* An impossible DL that means don't change
				 the old DL.
			      */
#define DLSPECIALLOW 0xFFFE   /* The lowest special DL. */

#define DLFCHANGEFILEMASK 0x8000   /* Mask for getting at the change file
				      flag in a DL.
				   */
#define DLOBKMASK 0x7FFF	   /* Mask for getting at the OBK in a DL. */

typedef struct
	  {
	  DT	    dt; 		/* Date */
	  BOOL	    fMarked;		/* TRUE if the date is marked. */
	  int	    cAlarms;		/* Count of alarms set. */
	  DL	    dl; 		/* Where the date is stored. */
	  int	    idr;		/* The index into vrghlmDr if
					   the date is in memory.  If not
					   in memory, this is IDRNIL.
					*/
	  DL	    dlSave;		/* Used to hold the previous DL
					   during a Save operation.  This
					   is used for error recovery.
					*/
	  } DD; 	      /*** DD - Date Descriptor */


typedef BYTE FAR *LPBYTE;

typedef struct			   /*** D3 - date in three parts */
	   {
	   WORD     wMonth;	   /* 0 through 11 */
	   WORD     wDay;	   /* 0 through 30 */
	   WORD     wYear;	   /* 0 (1980) through 119 (2099) */
	   } D3;

#define MONTHJAN 0	      /* Representation of January in a D3.wMonth */
#define MONTHFEB 1	      /* Representation of February in a D3.wMonth */
#define MONTHDEC 11	      /* Representation of December in a D3.wMonth */
#define YEAR1980 0	      /* Representation of 1980 in a D3.wYear */
#define YEAR2099 119	      /* Representation of 2099 in a D3.wYear */

typedef int TM; 	      /*** - Time packed into an int - the number of
				     minutes since midnight.
			      */
#define TMLAST (24 * 60 - 1)
#define TMNILHIGH (TMLAST + 1)
#define TMNILLOW -1
#define TMNOON (12 * 60)

typedef struct
     {
     DT   dt;		      /* Date */
     TM   tm;		      /* Time */
     } FT;		      /*** FT - Full Time (date and time) */

#define OTQRNIL 0xFFFF

typedef struct
     {
     unsigned  cb : 8;	      /* Count of bytes in this QR. */
     unsigned  fAlarm : 1;    /* TRUE if alarm set. */
     unsigned  fSpecial : 1;  /* TRUE if special time. */
     unsigned  reserved : 6;  /* Reserved for future use - 0 filled. */
     TM        tm;	      /* Appointment Time. */
     char      qd [1];	      /*** -  QD - Appointment Description.  Up to
				      CCHQDMAX + 1 chars but declared as a
				      single char here since storage is
				      dynamically allocated.  Not AD since
				      we want to be able to search for this
				      data type.
			      */
     } QR;		      /*** QR - Appointment Record - not AR since
			       we want to be able to search for this data
			       type.
			      */

/* Count of bytes in a QR header (everything except the QD). */
#define CBQRHEAD (sizeof (QR) - sizeof (char))

/* !!! WARNING - If CCHQDMAX is changed, be sure to adjust the ACKALARM
   dialog in CALENDAR.RC (since the resource compiler can't multiply or
   divide, and it also seems that it doesn't support the use of symbolic
   constants for specifying the positions or sizes of controls.
*/
#define CCHQDMAX 40	      /* Maximum number of text characters in
				 an appointment description (does not
				 include the zero byte terminator).
			      */
/* !!! WARNING END */

#define CBNOTESMAX 200	      /* Maximum length of notes including unformatted
				 text, soft line breaks, and 0 terminator.
			      */
#define CLNNOTES 3	      /* The number of lines of notes. */

/* A soft line break takes 3 characters <CR,CR,LF>.  All but the last line
   of the notes area can end with a soft line break.  This is the count
   of bytes needed to accomodate that case.
*/
#define CBSOFTBREAKSMAX (3 * (CLNNOTES - 1))

/* The maximum length of the unformatted text of the notes (reserve
   space for 0 terminator and soft line breaks).
*/
#define CBNOTESTEXTMAX (CBNOTESMAX - 1 - CBSOFTBREAKSMAX)

typedef struct
     {
     TM        tm;	      /* Appointment time. */
     WORD      otqr;	      /* Offset into tqr. */
     } LD;		      /*** - Line Descriptor. */


#define CBBK 64     /*** BK - disk block.  The size of a disk block
			 (Calendar's fundamental unit of allocation).
		    */


/* !!! WARNING - Be sure to adjust CDRHEAD when when modifying the
   definition of a DR.	Also note that except for the dt member,
   all members can be initialized to 0 (dt must be set to DTNIL).
*/

/* The maximum size of a DR.  Must be a multiple of CBBK. */
#define CBKDRMAX 32
#define CBDRMAX (CBKDRMAX * CBBK)

/* The size of the header. */
#define CBDRHEAD (sizeof (WORD) + sizeof (DT) + sizeof (BOOL) \
 + sizeof (WORD) + sizeof (WORD))

/* The count of bytes allocatable for the notes and the tqr. */
#define CBDRDATAMAX (CBDRMAX - CBDRHEAD)

/* The count of bytes available for the tqr. */
#define CBTQRMAX (CBDRDATAMAX - CBNOTESMAX)

typedef struct
     {
     WORD wReserved;		   /* Reserved for future use - written out
				      as 0.
				   */
     DT   dt;			   /* The associated dt.  DTNIL means this
				      DR is not in use.
				   */
     BOOL fDirty;		   /* TRUE if modifications have been made. */
     WORD cbNotes;		   /* Length of notes. */
     WORD cbTqr;		   /* Length of tqr. */
     BYTE rgbData [CBDRDATAMAX];   /* Room for the notes and the tqr. */
     } DR;		 /*** DR - Date Record */

/* !!! WARNING END */

#define CDR 3		      /* The number of DRs in memory. */
#define IDRNIL 0xFFFF	      /* An inpossible idr. */

typedef OFSTRUCT REO;	      /*** REO - file reopen buffer. */
typedef OFSTRUCT NEAR *NPREO;
typedef OFSTRUCT FAR  *LPREO;

/* The maximum length of a file specification in DOS 3.0, including
   the device, directories, filename, and extension.
*/
#define CCHFILESPECMAX 128

/* Filename (8) plus period (1) plus extension (3). */
#define CCHFILENAMEANDEXTMAX 12

/* Enough room for "Calendar - filename.cal" plus the terminator (which
   is included in CCHSZCALENDARDASH).
*/
#define CCHSZWINDOWTEXTMAX (CCHSZCALENDARDASH + CCHFILENAMEANDEXTMAX)

/* The sizes of the items in the header BK. */
#define CBMAGIC 8
#define CBCDD sizeof (int)
#define CBMINEARLYRING sizeof (WORD)
#define CBFSOUND sizeof (BOOL)
#define CBMDINTERVAL sizeof (int)
#define CBMININTERVAL sizeof (int)
#define CBFHOUR24 sizeof (BOOL)
#define CBTMSTART sizeof (TM)

/* Offsets into the file header BK. */
#define OBMAGIC 0
#define OBCDD (OBMAGIC + CBMAGIC)
#define OBMINEARLYRING (OBCDD + CBCDD)
#define OBFSOUND (OBMINEARLYRING + CBMINEARLYRING)
#define OBMDINTERVAL (OBFSOUND + CBFSOUND)
#define OBMININTERVAL (OBMDINTERVAL + CBMDINTERVAL)
#define OBFHOUR24 (OBMININTERVAL + CBMININTERVAL)
#define OBTMSTART (OBFHOUR24 + CBFHOUR24)


#define CBMONTHARRAY 42       /* Number of bytes in the month array. */

#define GRAYOUT (DWORD)0x00FA0089  /* dest = dest OR pattern */

#define CCHMONTH 20	      /* The longest month name. */
#define CCHYEAR 4	      /* Chars in a year (e.g., 1985). */
#define CCHTIMESZ 12	      /* Absolute maximum number of chars in a zero
				 terminated time string, taking into account
				 international formats.  4 plus space plus
				 6 char AM/PM string plus 0 at end.
			      */

#define CCHDATEDISP 64	      /* The number of characters in a zero
				 terminated ASCII date string.
				 30 is large enough for US style
				 strings, so 64 ought to do it for all
				 else.
			      */

#define CCHDASHDATE 11	      /* Maximum length of zero terminated
				 dashed date string.  For example,
				 "10-28-1985" (10 + zero byte).
			      */

#define MARKED 128	      /* Bit indicating the date is marked. */
#define TODAY  64	      /* Bit indicating the date is today. */


#define SCROLLMONTHLAST 1439  /* Can scroll from 0 to SCROLLMONTHLAST while
				 in month mode.  (2099 - 1980 + 1) * 12 - 1.
			      */

#define MDINTERVAL15 0	      /* Interval is 15 minutes. */
#define MDINTERVAL30 1	      /* Interval is 30 minutes. */
#define MDINTERVAL60 2	      /* Interval is 60 minutes. */

#define LNNIL -1	      /* An invalid ln. */

#define ALARMBEEP 0	      /* Value sent to MessageBeep. */
#define CALARMBEEPS 4	      /* The total number of beeps in an alarm. */


/* Menu command ids. */

#define IDCM_NEW	      0
#define IDCM_OPEN	      1
#define IDCM_SAVE	      2
#define IDCM_SAVEAS	      3
#define IDCM_PRINT	      4
#define IDCM_REMOVE	      5

#define IDCM_CUT	      6
#define IDCM_COPY	      7
#define IDCM_PASTE	      8

#define IDCM_DAY	      9
#define IDCM_MONTH	      10

#define IDCM_TODAY	      11
#define IDCM_PREVIOUS	      12
#define IDCM_NEXT	      13
#define IDCM_DATE	      14

#define IDCM_SET	      15
#define IDCM_CONTROLS	      16

#define IDCM_MARK	      17
#define IDCM_SPECIALTIME      18
#define IDCM_DAYSETTINGS      19

#define IDCM_ABOUT	      20
#define IDCM_DEL	      21
#define IDCM_EXIT             22

/* 26-Mar-1987 */
#define IDCM_START            30
#define IDCM_START12          31


/* Dialog ids.
   !!! WARNING - The dialog ids are also used to index vrglpfnDialog
   so they must be consecutive integers starting at 1, and must match
   the entries in vrglpfnDialog.  Apparently a dialog id of 0 is unacceptable,
   so they begin with 1, and we subtract 1 to get the index into
   vrglpfnDialog.
*/
#define CIDD		      9    /* The number of dialog ids in
				      vrglpfnDialog.
				   */
#define IDD_SAVEAS	      1
#define IDD_PRINT	      2
#define IDD_REMOVE	      3

#define IDD_DATE	      4

#define IDD_CONTROLS	      5

#define IDD_SPECIALTIME       6
#define IDD_DAYSETTINGS       7
#define IDD_ABOUT	      8
#define IDD_ACKALARMS	      9

#define IDD_ABORTPRINT	      10   /* The abort print dialog is not included
				      in vrglpfnDialog.
				   */
#define IDD_OPEN	      11   /* The open dialog is not included in
				      vrglpfnDialog since it is taken
				      care of by dlgopen.c.
				   */


/* !!! WARNING END */

/* Control ids */

#define IDCN_IGNORE	      -1

/* IDOK is 1, and IDCANCEL is 2 from windows.h */

#define IDCN_EDIT	      3
#define IDCN_LISTBOX	      4
#define IDCN_PATH	      5
#define IDCN_FROMDATE	      6
#define IDCN_TODATE	      7
#define IDCN_EARLYRING	      8
#define IDCN_SOUND	      9
#define IDCN_INSERT	      10
#define IDCN_DELETE	      11
#define IDCN_MIN15	      12
#define IDCN_MIN30	      13
#define IDCN_MIN60	      14
#define IDCN_HOUR12	      15
#define IDCN_HOUR24	      16
#define IDCN_STARTINGTIME     17

#define IDECQD		      100	/* ID of appointment description
					   edit control.
					*/
#define IDECNOTES	      101	/* ID of notes area edit control. */


/* File ids.
   !!! WARNING - if the order of these is changed, the GIVEME string IDS must
       also be changed.
*/
#define IDFILEORIGINAL 0
#define IDFILECHANGE 1
#define IDFILENEW 2
#define CFILE 3 	      /* Number of files. */
/* !!! WARNING END */

/* String ids. */

#define IDS_UNTITLED		   0
#define IDS_CALENDAR		   1
#define IDS_BADDATE		   2
#define IDS_BADDATERANGE	   3
#define IDS_BADTIME		   4
#define IDS_NOCREATECHANGEFILE	   5
#define IDS_NOCHANGEFILE	   6
#define IDS_ERRORWRITINGCHANGES    7
#define IDS_ERRORREADINGDATE	   8
#define IDS_TIMETOSAVE		   9
#define IDS_OUTOFMEMORY 	   10
#define IDS_RENAMEFAILED	   11
#define IDS_SAVEFAILED		   12
#define IDS_DISKFULL		   13
#define IDS_FILEEXISTS		   14
#define IDS_SAVECHANGES 	   15
#define IDS_FILENOTFOUND	   16
#define IDS_NOTCALFILE		   17
#define IDS_CANNOTREADFILE	   18
#define IDS_BADEARLYRING	   19
#define IDS_NOTSPECIALTIME	   20
#define IDS_NOSUCHTIME		   21
#define IDS_TIMEALREADYINUSE	   22
#define IDS_CANNOTPRINT 	   23
#define IDS_CALENDARDASH	   24
#define CCHSZCALENDARDASH	   50	/* Length of string "Calendar - ",
					   including the termintor.  If the
					   string is changed in the .RC file,
					   this must be changed accordingly.
					*/
#define IDS_ABOUT		   25
#define IDS_TOOMANYDATES	   26
#define IDS_DATEISFULL		   27
#define IDS_TEXTTRUNCATED	   28


/* The GIVEME strings must be ordered according to the order of the
   IDFILEs (which are defined earlier in this file).
*/
#define IDS_GIVEMEFIRST 	   IDS_GIVEMEORIGINAL
#define IDS_GIVEMEORIGINAL	   29
#define IDS_GIVEMECHANGEFILE	   30
#define IDS_GIVEMENEW		   31

#define IDS_BADFILENAME 	   32

#define IDS_NEDSTP		   33
#define IDS_NEMTP		   34
#define IDS_ALARMSYNC		   35
#define IDS_NOTIMER		   36
#define IDS_DATERANGE		   37
#define IDS_DATESUBRANGE	   38
#define IDS_TIMESUBRANGE	   39
#define IDS_NOCREATE		   40
#define IDS_MERGE1		   41
#define IDS_FILEEXTENSION          42

#define CSTRINGS 43               /* The number of strings loaded from the
				      resource file.
				   */

#define CCHSTRINGSMAX 2048	   /* The total length of all loaded strings
				      must be less than or equal to this.
				      If this is exceeded, CalInit will fail,
				      preventing Calendar from running.  If
				      this should occur, CCHSTRINGSMAX should
				      be increased as necessary and the initial
				      heap size in CALENDAR.DEF should also
				      be increased accordingly.
				   */

/* Message posted to self upon receiving activate message */
#define CM_PROCALARMS		  WM_USER+100


#ifndef RESOURCE
/* Abbreviations for referencing the loaded strings. */
#define vszUntitled		   vrgsz [IDS_UNTITLED]
#define vszCalendar		   vrgsz [IDS_CALENDAR]
#define vszBadDate		   vrgsz [IDS_BADDATE]
#define vszBadDateRange 	   vrgsz [IDS_BADDATERANGE]
#define vszBadTime		   vrgsz [IDS_BADTIME]
#define vszNoCreateChangeFile	   vrgsz [IDS_NOCREATECHANGEFILE]
#define vszNoChangeFile 	   vrgsz [IDS_NOCHANGEFILE]
#define vszErrorWritingChanges	   vrgsz [IDS_ERRORWRITINGCHANGES]
#define vszErrorReadingDate	   vrgsz [IDS_ERRORREADINGDATE]
#define vszTimeToSave		   vrgsz [IDS_TIMETOSAVE]
#define vszOutOfMemory		   vrgsz [IDS_OUTOFMEMORY]
#define vszRenameFailed 	   vrgsz [IDS_RENAMEFAILED]
#define vszSaveFailed		   vrgsz [IDS_SAVEFAILED]
#define vszDiskFull		   vrgsz [IDS_DISKFULL]
#define vszFileExists		   vrgsz [IDS_FILEEXISTS]
#define vszSaveChanges		   vrgsz [IDS_SAVECHANGES]
#define vszFileNotFound 	   vrgsz [IDS_FILENOTFOUND]
#define vszNotCalFile		   vrgsz [IDS_NOTCALFILE]
#define vszCannotReadFile	   vrgsz [IDS_CANNOTREADFILE]
#define vszBadEarlyRing 	   vrgsz [IDS_BADEARLYRING]
#define vszNotSpecialTime	   vrgsz [IDS_NOTSPECIALTIME]
#define vszNoSuchTime		   vrgsz [IDS_NOSUCHTIME]
#define vszTimeAlreadyInUse	   vrgsz [IDS_TIMEALREADYINUSE]
#define vszCannotPrint		   vrgsz [IDS_CANNOTPRINT]
#define vszCalendarDash 	   vrgsz [IDS_CALENDARDASH]
#define vszAbout		   vrgsz [IDS_ABOUT]
#define vszTooManyDates 	   vrgsz [IDS_TOOMANYDATES]
#define vszDateIsFull		   vrgsz [IDS_DATEISFULL]
#define vszTextTruncated	   vrgsz [IDS_TEXTTRUNCATED]

#define vszBadFileName		   vrgsz [IDS_BADFILENAME]
#define vszAlarmSync		   vrgsz [IDS_ALARMSYNC]

/* This must go at the end since it may use typedefs from cal.h */
#include "declare.h"
#include "..\\common\\common.h"
#endif
