/* ===== MEMDATA.C ====================================================

	Menus and strings for Memset program.

======================================================================= 


	    PLEASE PUT NOTES in HISTORY (below) WHEN YOU EDIT
	    THIS FILE.


======================================================================= 
21 jun 88	peterbe		The messages txSavedSetup and
				txRerunSavedSetup were eliminated.
				txSavedMemset renamed to txSaved.
16 jun 88	peterbe		txConfigSmart, InsuffExtOrExp286,
				InsuffExp286 modified to indicate free
				rather than total expanded memory.
===================================================================== */

#include "setup.h"

/* graphics for menus from "graphics.c".
** 7-bit (text) strings are define here, 8-bit (graphics) sstrings
** are externs.
*/

extern char shBoxTop[];
extern char shBoxVL[];

char shBoxTo[] = "WHEN YOU'RE READY TO";
char shBoxPr[] = "PRESS";

extern char shBoxLi[];
extern char shBoxBo[];

char shBoxIf[] = "IF YOU WANT MEMSET TO";
char shBoxCtrlX[] = "Exit without completing Memset               CONTROL+X";
char shBoxContinue[] = "Continue Memset";
char shBoxEnter[] = "ENTER";
char shBoxConfirm[] = "Confirm your choice";

extern char shDiskSymbol1[];
extern char shDiskSymbol2[];
extern char shDiskSymbol3[];
extern char shDiskSymbol5[];


/* strings for explaining the usage of keys for selection from a list
*/
char sListKeys1[] =
"- Use the DIRECTION (\030,\031) keys to move the highlight to your selection.";

/* insertion strings for display text -- inserted when a character is
** between 1 and 31 inclusive (001 and 037 octal). In a C string, the
** number should be inserted as a backslash followed by a 3-digit
** octal number.
** The pointers in entries 001 to 012 of this array are changed when a
** single-application setup is done.
*/

int nInserts = 021;
char * InsertText[31] = {      /* To use, insert octal number below: */
/* 001 */ "",	/* extended memory. */           
/* 002 */ "",  /* expanded memory. */         
/* 003 */ "",  /* file name */         
/* 004 */ "",  /* path name */         
/* 005 */ "",           
/* 006 */ "",           
/* 007 */ "",           
/* 010 */ "",
/* 011 */ "",
/* 012 */ "",           
/* 013 */ "",		
/* 014 */ "",		
/* 015 */ "",		
/* 016 */ "",	/* for numbers */
/* 017 */ "",
/* 020 */ ""
	};


/* message and choices for 'Continue'/'Quit' choice.  The first row of
** these displays is set to be 2 rows after the last line displayed
** by the program.  (ContinueOrQuit() function).
*/

textline ContQuitMsg[] = {
  { 4, 10, shBoxTop},
  { 5, 10, shBoxVL},
  { 5, 12, shBoxTo},
  { 5, 60, shBoxPr},
  { 5, 66, shBoxVL},
  { 6, 10, shBoxLi},
  { 7, 10, shBoxVL},
  { 7, 12, shBoxContinue},
  { 7, 60, shBoxEnter},
  { 7, 66, shBoxVL},
  { 8, 10, shBoxVL},
  { 8, 12, shBoxCtrlX},
  { 8, 66, shBoxVL},
  { 9, 10, shBoxBo},
  { 0, 0, NULL}
  };

textline IntroMsg[] = {
{1, 26,			"Memset program"},
{3, 1, 
"    Windows can take advantage of extended/expanded memory to enhance its"},
{4, 1,
"performance.  Memset is a program designed to guide you through setting"},
{5, 1,
"up your computer's extended and expanded memory for use with Windows."},
{6, 1, 
"    For a discussion of extended and expanded memory and how to run Memset,"},
{7, 1,
"see the section \"Taking Advantage of Memory\" in the \"Microsoft Windows:"},
{8, 1,
"Questions and Answers\" booklet.  It is ESSENTIAL that you read the sub-"},
{9, 1, 
"section, \"Setting Up Extended or Expanded Memory\" if you plan to use an"},
{10, 1,
"extended or expanded memory board correctly with Windows."},
{11, 1,
"    You should re-run Memset each time you add or remove an extended or"},
{12, 1,
"expanded memory board."},
{13, 1, 
"    You should run Memset now unless you are going to add an extended or"},
{14, 1,
"expanded memory board now.  If you are going to add a board now, then exit"},
{15, 1,
"Memset by pressing Control-X, install the memory board according to the"},
{16, 1, 
"instructions provided by the manufacturer, and then re-run Memset by"},
{17, 1,
"typing Memset at the DOS prompt in the Windows directory."},
{0, 0, NULL}
};

textline SmartDriveInMsg[] = {
{1, 1, 
"Memset detected that SMARTDrive is currently installed. Since SMARTDrive"},
{2, 1, 
"is installed we assume you have already configured the memory properly for your"},
{3,1 , 
"system."},
{5, 1, 
"If this assumption is not correct, remove SMARTDrive from your CONFIG.SYS"},
{6,1,
"file, restart your computer, and then run Memset by typing Memset at the"},
{7, 1, 
"prompt in the directory where Windows is installed."},
{13, 1, " "},
{0, 0, NULL}
};

textline IsWin386[] = {
    {6, 6, "Will you be running Microsoft Windows/386 ?"},
    {13, 10, shBoxTop},
    {14, 10, shBoxVL},
    {14, 12, "IF YOU"},
    {14, 60, shBoxPr},
    {14, 66, shBoxVL},
    {15, 10, shBoxLi},
    {16, 10, shBoxVL},
    {16, 12, "Will  be running WINDOWS/386"},
    {16, 66, shBoxVL},
    {17, 10, shBoxVL},
    {17, 12, "Won't be running WINDOWS/386"},
    {17, 66, shBoxVL},
    {18, 10, shBoxLi},
    {19, 10, shBoxVL},
    {19, 12, shBoxCtrlX},
    {19, 66, shBoxVL},
    {20, 10, shBoxBo},
	{0, 0, NULL}
};

textline YesNoChoices[] = {
  { 16, 62, "Y"},
  { 17, 62, "N"},
  { 0, 0, NULL}
};


textline NoExtOrExp386[] = {
    {1, 1,
"Memset did not find any extended or expanded memory."},

    {3, 1,
"If you have expanded memory, set it up as extended memory since Windows/386"},
    {4, 1,
"uses only extended memory.    To do this, re-install your memory board as"},
    {5, 1,
"extended memory and then re-run Memset. The documentation which came with"},
    {6, 1,
"your memory board describes how to switch from expanded to extended memory."},
    {0, 0, NULL}
};

textline NoExtOrExp286[] = {
{1, 1,
"Memset did not find any extended or expanded memory and hence Memset cannot"}, 
{2, 1,
"install SMARTDrive.  If you add an extended or expanded memory board to"},
{3, 1,
"your computer at some later time, you should re-run Memset."},
{0, 0, NULL}
};

/*[*] [Only displayed with Win386 and expanded memory found] */

textline ExpOnly386[] = {
    {1, 1,
"You have \002K of expanded memory."},

    {3, 1,
"Windows/386 uses only extended memory.  However, most expanded memory boards"},
    {4, 1,
"can be set up as either expanded or extended memory."},

    {6, 1,
"You should switch the memory on your board from expanded to extended and"},
    {7, 1,
"and then re-run Memset. The documentation that came with your memory board"},
    {8, 1,
"describes how to switch the memory from expanded to extended."},
	 {13, 1, " "},
   {0, 0, NULL}
};

textline ExpAndExt386[] = {
{1, 1,
"You have"}, 
{2, 15,
"         \002K of expanded memory."},
{3, 15,
"         \001K of extended memory."},
{5, 1, 
"Windows/386 uses only extended memory.  However, most expanded memory"},
{6, 1,
"boards can be set up as either expanded or extended memory."},
{7, 4,	/* indent */
   "If you continue with Memset, it will try to set up SMARTDrive to use"},
{8, 1,
"the available extended memory. However your expanded memory will not be used"},
{ 9, 1,
"at all."},
{10, 4,	/* indent */
  "Alternatively, you can change your expanded memory to extended memory so"},
{11, 1,
"that no memory is wasted. SMARTDrive and Windows/386 will both get more"
  },
{12, 1,
"memory to work with and hence will perform better."},
{13, 4,	/* indent */
   "To do this, exit Memset, change your memory board"},
{14, 1,
"from expanded to extended, and then re-run Memset. The documentation"},
{15, 1,
"that came with your memory board documentation describes how to switch the"},
{16, 1,
"memory from expanded to extended."},
{0, 0, NULL}
};


/* [*] [Only displayed with Win386 and not enough extended memory --
     i.e., total is less than 2Mb.]
*/

textline InsuffExtMem1[] = {
{1, 1,
"You have \001K of extended memory."},

{3, 1,
"Windows/386 will perform better if you have more than 1 Megabyte"},
{4, 1,
"of extended memory."},
{6, 1,
"You should add at least \002K bytes of additional extended memory to your"},
{7, 1,
"system. Once you have installed this additional memory, re-run Memset."},
{0, 0, NULL}
};

textline InsuffExtMem2[] = {
{1, 1,
"You have \001K of extended memory."},
{2, 1, 
"         \002K of expanded memory."},
{4, 1,
"Windows/386 will perform better if you have more than 1 Megabyte"},
{5, 1,
"of extended memory."},
{7, 1,
"You should make at least 1 Megabyte extended memory available for Window/386 by"},
{8, 1,
"doing one or more of the following:"},
{9, 1, 
" - re-install your expanded memory as extended memory,"},
{10, 1,
" - install more extended memory into your system,"},
{12, 1, 
"Once you have this additional extended memory, re-run Memset."},
{0, 0, NULL}
};


textline InsuffFreeExtMem1[] = {
{1, 1,
"You have \001K of extended memory"},
{2, 1, 
"         \003K of free extended memory."},
{4, 1, 
"Windows/386 will perform better if you have more than 1 Megabyte of"},
{5, 1,
"free extended memory available for it."},
{7, 1,
"You should make at least 1 MB of extended memory available for Windows/386 by"},
{8, 1, 
"doing one or more of the following:"},
{10, 1, 
" - install more extended memory into your system,"},
{11, 1, 
" - if some other program (driver or TSR) is using some of the extended"},
{12, 1, 
"   memory, change your CONFIG.SYS file to remove this program or modify its"},
{13, 1, 
"   parameters to make more of the extended memory available for Windows/386."},
{14, 1,
"Once you have this additional extended memory, re-run Memset."},
{0, 0, NULL}
};

textline InsuffFreeExtMem2[] = {
{1, 1,
"You have \001K of extended memory"},
{2, 1, 
"         \003K of free extended memory."},
{3, 1, 
"         \002K of expanded memory."},
{5, 1, 
"Windows/386 will perform better if you have more than 1 Megabyte of"},
{6, 1,
"free extended memory available for it."},
{8, 1,
"You should make at least 1 MB of extended memory available for Windows/386 by"},
{9, 1, 
"doing one or more of the following:"},
{11, 1, 
" - if you have expanded memory change it to extended memory,"},
{12, 1,
" - install more extended memory into your system,"},
{13, 1, 
" - if some other program (driver or TSR) is using some of the extended"},
{14, 1, 
"   memory, change your CONFIG.SYS file to remove this program or modify its"},
{15, 1, 
"   parameter to make more of the extended memory available for Windows/386."},
{16, 1, 
"Once you have this additional extended memory, re-run Memset."},
{0, 0, NULL}
};


textline InsuffExt286[] = {
{1, 1,
"You have \001K of extended memory,"},
{2, 1,
"         \003K of free extended memory,"},
{4, 1,
"You do not have sufficient extended memory to install SMARTDrive with Windows."},
{5, 1,
"If your extended memory is currently being used by some other program,"},
{6, 1,
"you should modify your CONFIG.SYS file to free up at least 192K of extended"},
{7, 1, 
"memory (128K for SMARTDrive and 64K for Windows), reboot your system and then"},
{8, 1, 
"re-run Memset and Windows Setup."},
{9, 1, 
"Alternatively you could install more extended memory and then re-run Memset"},
{10, 1, 
"and Windows Setup."},
{13, 1, " "},
{0, 0, NULL}
};

textline InsuffExp286[] = {
{1, 1,
"You have 0K of extended memory,"},
{2, 1,
"         \002K of free expanded memory."},
{4, 1, 
"You do not have sufficient memory to install SMARTDrive."},
{5, 1,
"You  should add more expanded memory to your system and then re-run Memset."},
{13, 1, " "},
{0, 0, NULL}
};

textline InsuffExtOrExp286[] = {
{1, 1,
"You have \001K of extended memory,"},
{2, 1,
"         \003K of free extended memory,"},
{3, 1,
"         \002K of free expanded memory."},
{5, 1, 
"You do not have sufficient memory to install SMARTDrive."},
{7, 1,
"If you have extended memory but it is currently being used by some other"},
{8, 1,
"program, you should modify your CONFIG.SYS file to free up at least 192K of"},
{9, 1, 
"extended memory (128K for SMARTDrive and 64K for Windows), reboot your system"},
{10, 1, 
"and then re-run Memset and Windows Setup."},
{12, 1, 
"If you do not have extended memory but you have expanded memory, you should"},
{13, 1, 
"install more expanded memory and then re-run Memset."},
{14, 1, " "},
{0, 0, NULL}
};


textline NY286[] = {
{1, 1,
"Memset did not find any expanded memory installed. If you do have expanded"}, 
{2, 1,
"memory, but you have not installed an expanded memory driver, Memset"},
{3, 1,
"can help you install it. If you do not have any expanded memory, choose"},
{4, 1, "NONE."},
{0, 0, NULL}
};

textline ExpMemBoardsMsg[] = {
{6, 1,
"Please select your Expanded Memory board from the following list."},

{8, 3, sListKeys1},
{0, 0, NULL}
};

char * BoardsList[] = {
    "None",
    "AST RAMpage",
    "IBM Personal Systems/2 80286 Expansion Memory Option",
    "Intel Above Board",
    "Other",
    "" };

textline UnknownExpMemBoard[] = {
{1, 1, "Windows provides memory managers for the following memory boards:"},

{3, 8, " - AST RAMpage!"},
{4, 8, " - IBM PS/2 Memory Expansion Option"},
{5, 8, " - Intel Above Board"},	

{7, 1,
"If your board is not listed, you must install the expanded memory manager"},
{8, 1,
"provided by your memory board's manufacturer."},

{10, 1,
"You should install the expanded memory manager for your memory board," },
{11, 1,
"and then re-run Memset. The instructions provided with your memory board"},
{12, 1,
"explain how to set up your memory board's expanded memory manager."},
{13, 1,
" "},
{0, 0, NULL}
};

/* Msg to confirm the entered path */

textline ConfirmPathMsg[] = {
  {17, 10, shBoxTop},
  {18, 10, shBoxVL},
  {18, 12, shBoxTo},
  {18, 60, shBoxPr},
  {18, 66, shBoxVL},
  {19, 10, shBoxLi},
  {20, 10, shBoxVL},
  {20, 12, shBoxContinue},
  {20, 60, shBoxEnter},
  {20, 66, shBoxVL},
  {21, 10, shBoxVL},
  {21, 12, shBoxCtrlX},
  {21, 66, shBoxVL},
  {22, 10, shBoxBo},
  { 0, 0, NULL}
  };

/* message displayed at the bottom of the screen by selecting from a list
*/
/* Msg to confirm a selection */

textline ConfirmSelectionMsg[] = {
  {17, 10, shBoxTop},
  {18, 10, shBoxVL},
  {18, 12, shBoxTo},
  {18, 60, shBoxPr},
  {18, 66, shBoxVL},
  {19, 10, shBoxLi},
  {20, 10, shBoxVL},
  {20, 12, shBoxConfirm},
  {20, 60, shBoxEnter},
  {20, 66, shBoxVL},
  {21, 10, shBoxVL},
  {21, 12, shBoxCtrlX},
  {21, 66, shBoxVL},
  {22, 10, shBoxBo},
  { 0, 0, NULL}
  };

/* ======== The remainder of the text is used in the MEMOUT module. ======== */


/* ===== Text for GetPath() ====== */

textline txBootDrive[] = {
{ 1, 1,
"Memset needs to know the boot drive for your machine on your hard disk." },
{ 3, 1,
" - If the drive shown is not correct, use the BACKSPACE key to delete"},
{4, 1, 
"   the name shown, then type the correct name followed by a colon,"},
{ 5, 1, 
"   and press ENTER."},
{7, 25, " "},
{0, 0, NULL}
};

/* textline txBootBad[] = {
** { 1, 7,
** "The boot drive you typed was incorrect." },
** {0, 0, NULL}
** };
*/

textline txDriverPath[] = {
{ 1, 1,
"Memset will now copy the driver \"\013\" on to your hard disk."},
{2, 1, 
"This should be copied to the root directory (e.g. C:\\), or some subdirectory"},
{3, 1, 
"where all the drivers exist (e.g. C:\\device)."},
{5, 1, 
"Enter the drive and directory where the driver \"\013\" should" },
{6, 1,
"be copied:" },
{8, 1,
"- If you want to use a different directory and/or drive, use the"},
{9, 1, 
"  BACKSPACE key to delete the name shown, then type the correct name."},
{12, 20, " "},
{0, 0, NULL}
};

textline txNotMade[] = {
{ 1, 7, "The drive and directory you have entered is not valid."},
{0, 0, NULL}
};


/* ===== Text for ReadConfig ===== */

/* CONFIG.SYS is too big to fit in buffer */
textline txTooLarge[] = {
{ 3, 15, "Error!" },
{ 5, 15, "\013 is too large."},
{0, 0, NULL}
};

/* ========== Text for FileCopy() ========== */


/* txNeedsFile or txCantFind is used together with txInsertDisk */
textline txNeedsFile[] = {
{ 1, 1, "Memset needs to copy \013 from your Utilities 2 diskette." },
{0, 0, NULL}
};

textline txCantFind[] = {
{ 1, 1, "Memset can not find \013 on the diskette in your A: drive." },
{0, 0, NULL}
};

/* combine with 1 of the above 2 messages */
textline txInsertDisk[] = {
{ 4, 13, "Insert the Utilities 2 diskette in your A: drive." },

{  7, 28, shDiskSymbol1 },
{  8, 28, shDiskSymbol2 },
{  9, 28, shDiskSymbol3 },
{ 10, 28, shDiskSymbol2 },
{ 11, 28, shDiskSymbol5 },

{0, 0, NULL}
};

textline txUnable[] = {
{ 3, 7, "Memset can not create \013."},
{0, 0, NULL}
};


/* ======= screens for CONFIG.SYS ======= */

/* ======= Screen for removing VDISK from CONFIG.SYS ====== */

textline txRmVdisk[] = {
  { 3, 1,
"The following command line for VDISK.SYS is in your CONFIG.SYS file:"},
  { 5, 20,        "\006"},
  { 7, 1,
"VDISK is incompatible with SMARTDrive and Windows."},
  { 8, 1,
"You need to delete this line from your CONFIG.SYS file."},

  {13, 10, shBoxTop},

  {14, 10, shBoxVL},
  {14, 12, "If you want Memset to"},
  {14, 60, shBoxPr},	/* press */
  {14, 66, shBoxVL},

  {15, 10, shBoxLi},

  {16, 10, shBoxVL},
  {16, 12, "Delete the VDISK.SYS line"},
  {16, 66, shBoxVL},

  {17, 10, shBoxVL},
  {17, 12, "Leave VDISK line in CONFIG.SYS"},
  {17, 66, shBoxVL},

  {18, 10, shBoxBo},

{ 0, 0, NULL}
};

/* ==== screens for displaying additions to config.sys and ===== */

/* These 2 screens display what needs to be done to config.sys.
** Either one is followed by txChangeConfig and txSaveChoices.
**/

/* the following may be added to the above.
** command line etc. for smartdrive.
** NOTE: there is code in MEMOUT.C which is specific to the number of
** lines in this message and the next one!
*/
textline txConfigSmart[] = {
{ 1, 1,
"You have \014Kb of conventional, \016Kb of free extended, and \017 KB of"},
{ 2, 1,
"free expanded memory.  In order to use SMARTDrive, which is described in"},
{ 3, 1,
"Appendix C, \"Speeding up Windows with SMARTDrive\", in your Windows"},
{ 4, 1,
"User's Guide, it is necessary to add the following line to your"},
{ 5, 1,
"CONFIG.SYS file."},

{ 7, 15, "\006" },	/* insert command line for SMARTDrive here */

{0, 0, NULL}
};

/* command line etc for EMM driver.
*/
textline txConfigEmm[] = {
{ 1, 1,
"In order to use your \"\007\" expanded memory driver, which is described" },
{ 2, 1,
"in the text file \"\010\" on the Utilities 2 diskette, it is necessary to" },
{ 3, 1,
"add the following line to your CONFIG.SYS file." },

{ 5, 15, "\006" },	/* the command line for xxxxx.SYS */

{0, 0, NULL}
};


/* select between changing CONFIG.SYS and backing it up, and
** just writing CONFIG.NEW
*/

textline txChangeConfig[] = {
{ 9, 1,
"Memset can do this for you, copying CONFIG.SYS to a backup file named"},
{10 , 1,
"CONFIG.BAK.  Or, if you prefer, it can save the information to a file named"},
{11, 1, 
"CONFIG.NEW, which you can use later to modify CONFIG.SYS."},

  {13, 10, shBoxTop},

  {14, 10, shBoxVL},
  {14, 12, "If you want Memset to"},
  {14, 60, shBoxPr},	/* press */
  {14, 66, shBoxVL},

  {15, 10, shBoxLi},

  {16, 10, shBoxVL},
  {16, 12, "Change your CONFIG.SYS file"},
  {16, 66, shBoxVL},

  {17, 10, shBoxVL},
  {17, 12, "Save the changes in CONFIG.NEW"},
  {17, 66, shBoxVL},

  {18, 10, shBoxVL},
  {18, 12, "Exit without changing any files"},
  {18, 52, "CONTROL-X"},
  {18, 66, shBoxVL},

  {19, 10, shBoxBo},

{ 0, 0, NULL}
};

textline txSaveChoices[] = {
    { 16, 60, "C"},
    { 17, 60, "S"},
    { 0, 0, NULL}
};

/* ==== strings used in the WriteNew() function for CONFIG.NEW ==== */

char sNewDelete[] = "Delete the following command from Config.sys:";
char sNewAdd[] = "Add the following string to Config.sys:";


/* ==== messages for exit from program ==== */

/* this line is used together with txNotSaved. */
textline txCouldNotSave[] = {
    { 3, 1, "Memset was unable to copy your CONFIG.SYS file to CONFIG.BAK"},
    { 0, 0, NULL}
};

/* this message is used alone or with txCouldNotSave */
textline txNotSaved[] = {
{ 1, 22,
           "MEMSET HAS NOW BEEN COMPLETED."},
    { 5, 1,
"Your CONFIG.SYS file has not been changed.  Information telling you how to"},
    { 6, 1,
"change CONFIG.SYS has been saved in the file CONFIG.NEW."},

    { 8, 1,
"You must change your CONFIG.SYS file according to the information saved in"},
    { 9, 1,
"the file CONFIG.NEW. Once you have changed your CONFIG.SYS file, be sure to" },
    {10, 1,
"restart your computer by pressing CONTROL+ALT+DELETE so that it will"},
    {11, 1,
"recognize the changes you have made."},
    { 0, 0, NULL}
};

/* CONFIG.SYS was saved, program was run standalone */

textline txSaved[] = {
{ 3, 15,
           "MEMSET HAS NOW BEEN COMPLETED SUCCESSFULLY."},

{ 5, 1,
"PLEASE REBOOT YOUR SYSTEM NOW.   TO DO THIS, REMOVE THE DISKETTE FROM"},
{ 6, 1,
"DRIVE A: AND PRESS CONTROL+ALT+DELETE."},
    { 0, 0, NULL}
};


/* this is displayed after an EMM manager is installed in CONFIG.SYS */

textline txRerunMemset[] = {
{ 1, 15,
           "MEMSET HAS NOW BEEN COMPLETED SUCCESSFULLY."},
{3, 1, 
"Memset has installed the expanded memory manager \"\013\" for the"},
{4, 1,
"expanded memory board in your computer."},

/* one of the next two messages appears here .. print it FIRST!! */

{10, 1, 
"After rebooting your machine, you must run Memset AGAIN to set up SMARTDrive."},
{11, 1,
"Memset can set up SMARTDrive only after your expanded memory manager is"},
{12, 1, 
"properly installed on your system. Hence it cannot set up SMARTDrive now."},
{0, 0, NULL}
};

/* One of the following two messages are combined with the above one. */

/* (1) Config.sys changed, so reboot */

textline txRerunSaved[] = {
{ 6, 1,
"PLEASE REBOOT YOUR SYSTEM NOW.   TO DO THIS, REMOVE THE DISKETTE FROM"},
{ 7, 1,
"DRIVE A: AND PRESS CONTROL+ALT+DELETE."},
{0, 0, NULL}
};

/* (2) Config.sys was NOT changed. */

textline txRerunNotSaved[] = {
{6, 1, 
"Memset did NOT modify your CONFIG.SYS file, so you must modify"},
{7,1, 
"CONFIG.SYS yourself, according to the instructions in the file CONFIG.NEW,"},
{8, 1, 
"and then reboot your machine by pressing CONTROL+ALT+DELETE."},
{0, 0, NULL}
};
