/* SUDATA.C -- messages module for Setup (must be localized)
 * whenever a message is added or removed, this change should be
 * done in sutest.c as well
**
**	Modification History
**
**  31 jan 89    wch     references to DisketteFinishedMsg removed
**   1 feb 89    wch     Text changed in screen txCompat[]
**   6 feb 89    wch     removed unused screens/menus
 */

#include "setup.h"

/* ** string displayed in menu if a file has a null description ** */

CHAR nullfile[] = "???";

/* name of the backup of the WIN.INI file
*/
CHAR WinIniBakFile[PATHMAX] = "WIN.OLD";

/* name of the main readme file
*/
CHAR Readme[] = "README.TXT";

/* search pattern for readmes
*/
CHAR ReadmeNames[] = "READ*.TXT";

/* name of Setup information file
*/
CHAR SetupInf[] = "SETUP.INF";

/* Buffer to convert integers. A pointer to it is in inserttext[].
*/
CHAR siNumber[6] = "0";

/* TRUE if font selection should be default (international), otherwise FALSE
*/
BOOL bFontSelection = FALSE;

/* ********* Fixed Menus for Selection of Systems and Drivers ********** */

/* insertion strings for display text -- inserted when a character is
** between 1 and 31 inclusive (001 and 037 octal). In a C string, the
** number should be inserted as a backslash followed by a 3-digit
** octal number.
** The pointers in entries 001 to 012 of this array are changed when a
** single-application setup is done.
*/

int ninserts = 021;
CHAR * inserttext[31] = {      /* To use, insert octal number below: */
/* 001 */ "",           /* Windows startup (.com) file name */
/* 002 */ "",           /* application program name */
/* 003 */ "",           /* default directory */
/* 004 */ "",           /* short program name */
/* 005 */ "",           /* long program name */
/* 006 */ "",           /* how to start program */
/* 007 */ "",           /* which program to start after the installation */
/* 010 */ "",
/* 011 */ "",
			/* the following are used by error & status displays */
/* 012 */ "",           /* for name of source disk */
/* 013 */ "",		/* for filenames */
/* 014 */ "",		/* for [section name] */
/* 015 */ "",		/* for destination path */
/* 016 */ siNumber,	/* for numbers */
/* 017 */ "ERROR: ",
/* 020 */ "Please check and restart Setup!"
	};

/* Strings for status messages */

CHAR smReading[] = "Setup is reading \013";
CHAR smLoading[] = "Setup is loading \013";
CHAR smWriting[] = "Setup is writing \013 to \015";
CHAR smCopying[] = "Setup is copying \013 to \015";
CHAR smDisplay[] =
"\013: use DIRECTION (\030,\031) keys to scroll by page, ESC to skip file";

/* Strings for FatalError() messages */

CHAR smInfErr[] = "\017 in SETUP.INF, [\014], line \016. \020";
CHAR smInfMissingErr[] = "Filename missing in SETUP.INF. \020";
CHAR smNoSection[] = "Section [\014] missing in SETUP.INF. \020";
CHAR smNoMemory[] = "Not enough memory (Setup needs at least 512K).";
CHAR smNoFile[] = "File \013 is missing on disk \016. \020";
CHAR smLoadErr[] = "\017loading \013 from disk \016. \020";
CHAR smWriteErr[] =
	"\017writing \013 to \015 (disk may be full).";
CHAR smOpenErr[] =
	"\017Can't open \013 (FILES= may be too small or disk full).";
CHAR smDirErr[] = "\017Can't create subdirectory \015";
CHAR smInfOpenErr[] =
   "Information file is not in current directory. \020";
CHAR smCurrentDir[] =
   "It's impossible to set up \004 in the current directory.";
CHAR smRAMDRVBackUpFail[] = 
	"Could not create a backup of RAMDRIVE.SYS under the name RAMDRIVE.OLD";
CHAR smCONFIGBackUpFail[] = 
	"Could not create a backup of CONFIG.SYS under the name CONFIG.BAK";
CHAR smConfigModfFail[] = 
	"Error while trying to modify CONFIG.SYS file.";
CHAR smAutoExecModfFail[] = 
	"Error while trying to modify AUTOEXEC.BAT file.";
CHAR smWinIniBackUpFail[] =
	"Error in creating a backup of WIN.INI under the name \013";
CHAR smCopyHimemFail[] =
	"ERROR in copying HIMEM.SYS from Windows directory to \013.";
CHAR smHimemExists[] =
	"Failure copying HIMEM.SYS to \013. Destination file already exists.";
CHAR smMissingID[] = 
	"\017in SETUP.INF,[\014],line \016.Missing ID information.PLEASE CHECK !";
CHAR smIllegalID[] =
	"\017in SETUP.INF, [\014], line \016. Illegal ID. PLEASE CHECK !";
CHAR smMissingNoMouseRec[] = 
	"\017in SETUP.INF, [\014], Missing NoMouse.drv record. PLEASE CHECK !";
CHAR smNoDisplayRec[] = 
	"\017in SETUP.INF,[\014],Display Record with Display ID = \016 absent.";
CHAR smIncorrectDOS20L1[] = 
	" ERROR : Incorrect DOS Version.";
CHAR smIncorrectDOS20L2[] = 
	"         To set up Windows, you need DOS Version 3.0 or higher.";
CHAR smIncorrectDOS386L1[] = 
	" ERROR : Incorrect DOS Version.";
CHAR smIncorrectDOS386L2[] = 
	"         To set up WIN/386, you need DOS Version 3.10 or higher.";

CHAR smNot386MachineL1[] = 
	" ERROR : You do not have an 80386 processor.";
CHAR smNot386MachineL2[] = 
	"         Win/386 can only run on a machine with an 80386 processor.";

CHAR smUserSMissing[] =
  "\017 Entry for USERS.EXE missing in section [\014] in SETUP.INF.";



/* String and position for "more.." message in scrollable selection menus */

CHAR smMore[] =
"             (To see more of the list, press the DOWN(\031) key.)";
int iMoreLine = 19;


/* String for inserting OEM (not distribution) disks. This string appears
** at the end of selection menus
*/
CHAR smSelectionInsertDisk[] =
   "Other (requires disk provided by a hardware manufacturer)";


/* strings for explaining the usage of keys for selection from a list
*/
CHAR sListKeys1[] =
"- Use the DIRECTION (\030,\031) keys to move the highlight to your selection.";

/* texts which can be used as helpers to create nice menus 
   graphic chars moved to graphics.c */
extern CHAR shBoxTop[];
extern CHAR shBoxVL[] ;
extern CHAR shBoxLi[] ;
extern CHAR shBoxBo[] ;

extern CHAR shBoxTop2[];
extern CHAR shBoxLi2[] ;
extern CHAR shBoxBo2[] ;

extern CHAR shDiskSymbol1[] ;
extern CHAR shDiskSymbol2[] ;
extern CHAR shDiskSymbol3[] ;
extern CHAR shDiskSymbol5[] ;


CHAR shBoxTo[] = "WHEN YOU'RE READY TO";
CHAR shBoxPr[] = "PRESS";
CHAR shBoxIf[] = "IF YOU WANT SETUP TO";
CHAR shBoxCtrlX[] = "Exit without completing Setup               CONTROL+X";
CHAR shBoxContinue[] = "Continue Setup";
CHAR shBoxEnter[] = "ENTER";
CHAR shBoxConfirm[] = "Confirm your choice";

/* Incorrect DOS version message */
textline IncorrectDos20[] = {
  {21, 1, smIncorrectDOS20L1},
  {22, 1, smIncorrectDOS20L2},
  {0,  0, NULL}
};

textline IncorrectDos386[] = {
  {21, 1, smIncorrectDOS386L1},
  {22, 1, smIncorrectDOS386L2},
  {0,  0, NULL}
};

/* Not a 80386 machine */
textline NotA386Machine[] = {
  {21, 1, smNot386MachineL1},
  {22, 1, smNot386MachineL2},
  { 0, 0, NULL}
};

textline HardDiskReqdMsg[] = {
	{18, 1,
	"ERROR :"},
	{19, 1,
	"       Setup could not find a hard disk drive on your system."},
	{20, 1,
	"       Windows can only be installed on a system with hard disk."},
	{21, 1,
	"       Please re-run Setup after you have installed a hard disk."},
	{0, 0, NULL}
};

textline DirErrMsg[] = {
	{5, 10, smDirErr},
	{7, 10,  "Press ENTER to return to the previous screen. "},
	{16, 1, " "},
	{0, 0, NULL}
};
	
/* message displayed at the bottom of the screen by entering a path
*/

textline ConfirmPathMsg[] = {
  {18, 10, shBoxTop},
  {19, 10, shBoxVL},
  {19, 12, shBoxTo},
  {19, 60, shBoxPr},
  {19, 66, shBoxVL},
  {20, 10, shBoxLi},
  {21, 10, shBoxVL},
  {21, 12, shBoxContinue},
  {21, 60, shBoxEnter},
  {21, 66, shBoxVL},
  {22, 10, shBoxVL},
  {22, 12, shBoxCtrlX},
  {22, 66, shBoxVL},
  {23, 10, shBoxBo},
  { 0, 0, NULL}
  };

/* message displayed at the bottom of the screen by selecting from a list
*/

textline ConfirmSelectionMsg[] = {
  {20, 10, shBoxTop},
  {21, 10, shBoxVL},
  {21, 12, shBoxTo},
  {21, 60, shBoxPr},
  {21, 66, shBoxVL},
  {22, 10, shBoxLi},
  {23, 10, shBoxVL},
  {23, 12, shBoxConfirm},
  {23, 60, shBoxEnter},
  {23, 66, shBoxVL},
  {24, 10, shBoxVL},
  {24, 12, shBoxCtrlX},
  {24, 66, shBoxVL},
  {25, 10, shBoxBo},
  { 0, 0, NULL}
  };

/* first screen for setup */

textline SetupBanner[] = {
  { 1,  1, 
"        SETUP PROGRAM for \005"},
  { 3,  1, 
"Setup prepares \004 to run on your computer. Setup detects the"},
  { 4,  1, 
"appropriate options for your display, pointing device, and additional memory,"},
  { 5,  1, 
"though it would be helpful if you knew the following:"},
  { 6,  3, 
"*  what kind of display you have"},
  { 7,  3, 
"*  what kind of pointing device (mouse) you have, if any"},
  { 8, 3, 
"*  what additional memory you have, if any"},
  { 9, 3, 
"*  what kind of printer(s) you have, if any"},
  { 10, 3, 
"*  which port your printer is connected to"},
  { 12, 1, 
"    If you do not know the above, simply press ENTER to accept the default"},
  { 13, 1,
"selections and continue Setup."},
  { 14, 1,
"    To set up \004 properly, you should complete the Setup program."},
  { 15, 1,
"You can leave the Setup process by pressing CONTROL+X (press and hold"},
  {16, 1, 
"the CONTROL key, and press the X key). Please keep in mind that if you leave"},
  {17, 1, 
"Setup, it will not be completed and you must run Setup again to install"},
  {18, 1, 
"\004 on your computer."},
  { 0, 0, NULL}
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

CHAR EnterDriveLetter1[] =
	"- If you want to use a different directory and/or drive, use the";
CHAR EnterDriveLetter2[] =
	"  BACKSPACE key to delete the name shown, then type the correct name.";

/* Display for input of path for the destination WINDOWS directory */

textline PathMenu[] = {
  {  1,  1, "\004 will now be set up on your hard disk in the directory shown"},
  {  2,  1, "below."},
  {  4,  3, EnterDriveLetter1},
  {  5,  3, EnterDriveLetter2},
  {  7,  2, ""},        /* to position the prompt line */
  {  0,  0, NULL}
  };

/* This screen will be displayed while copying files */

textline CopyMsg[] = {
  {  2,  1, "Setup is installing files ..."},
  {  0,  0, NULL}
  };

/* Screen for [machine] selection */
textline MachineMenu[] = {
  { 1, 1,
  "Below is a list of computers on which you may set up \004."},
  { 3, 3, sListKeys1},
  { 0, 0, NULL}
  };

/* Screen for listing selected DISPLAY, MOUSE, KEYBOARD drivers and
** SYSTEM and OEM fonts, and selecting which of these files are to be
** changed.
*/
textline DriversMenu[] = {
  { 1, 1, "Below is a list of the items you've selected."},
  { 3, 3, "- If these items are correct, press the ENTER key."},
  { 4, 3,
"- If you want to change an item, use the DIRECTION (\030,\031) keys to move"},
  { 5, 5,"the highlight to the item, then press the ENTER key."},
  { 0, 0, NULL}
  };

/* Screen for listing selected DISPLAY, MOUSE, KEYBOARD drivers and
** SYSTEM and OEM fonts, and selecting which of these files are to be
** changed.
*/
textline FinalMenu[] = {
  { 1, 1, 
"Shown below is the display adapter, keyboard and mouse or pointing device"},
  { 2, 1, 
"which Setup thinks you have. Review the list, then do one of the following:"},
  { 4, 4, 
"- If the list is correct, press the ENTER key to continue."},
  { 5, 4, 
"- If any items are incorrect, use the DIRECTION (\030,\031) keys to"},
  { 6, 4, 
"  move the highlight to the incorrect item, then press the ENTER key."},
  { 9, 1, 
" "},
  { 0, 0, NULL}
  };


CHAR MsgNoChange[] = "No Change";

/* Screens for Display, Keyboard, Mouse, Sysfonts, Oemfonts selection */

textline DisplayMenu[] = {
  { 1, 1, "Please select your display adapter from the following list."},
  { 3, 3, sListKeys1},
  { 0, 0, NULL}
  };

textline KeyboardMenu[] = {
  { 1, 1, "Please select your keyboard from the following list."},
  { 3, 3, sListKeys1},
  { 0, 0, NULL}
  };

textline MouseMenu[] = {
  { 1, 1, "Please select your pointing device from the following list."},
  { 3, 3, sListKeys1},
  { 0, 0, NULL}
  };

textline SysFontMenu[] = {
  { 1, 1,
  "Select the System font used by \004:" },
  { 2, 1,
  "This selection allows you to change the size of fonts appearing on your screen" },
  { 3, 1,
  "while running \004. Setup automatically chooses a standard setting," },
  { 4, 1,
  "depending on your display type. You can change the default setting by making" },
  { 5, 1,
  "a choice from of the following list." },
  { 7, 3, sListKeys1},
  { 0, 0, NULL}
  };

textline OemFontMenu[] = {
  {1, 1, "Select the Terminal (OEM) font used for MS-DOS applications:" },
  { 2, 1,
  "This selection allows you to change the size of fonts appearing on your" },
  { 3, 1,
  "screen while running standard MS-DOS applications. Setup automatically" },
  { 4, 1,
  "chooses a standard setting, depending on your display type. You can change" },
  { 5, 1,
  "the default setting by making a choice from of the following list." },
  { 7, 3, sListKeys1},
  { 0, 0, NULL}
  };

/* Menu for insertion of disks designated by disk number.
** The disk name (from SETUP.INF) is displayed in the position
** here indicated by \012.
**/
textline InsertMenu[] = {
  {  1, 30, shDiskSymbol1},
  {  2, 30, shDiskSymbol2},
  {  3, 30, shDiskSymbol3},
  {  4, 30, shDiskSymbol2},
  {  5, 30, shDiskSymbol5},
  {  7,  1, "Insert the \012 in the following drive:" },
  { 11,  3, EnterDriveLetter1},
  { 12,  3, EnterDriveLetter2},
  {  9,  2, ""},                /* position of input line */
  {  0,  0, NULL}
  };

/* Menu for insertion of OEM driver disks.
** The name of the kind of driver is displayed in the position
** here indicated by \014.
**/
textline InsertOemMenu[] = {
  {  1, 30, shDiskSymbol1},
  {  2, 30, shDiskSymbol2},
  {  3, 30, shDiskSymbol3},
  {  4, 30, shDiskSymbol2},
  {  5, 30, shDiskSymbol5},
  {  7, 1,
  "Insert disk for your \014 in the following drive:" },
  { 11,  3, EnterDriveLetter1},
  { 12,  3, EnterDriveLetter2},
  {  9, 2, ""},          /* position of input line */
  {  0, 0, NULL}
  };


textline SelectOemMenu[] = {
  { 1, 1, "Here are the \014 on your disk."},
  { 3, 3, sListKeys1},
  { 0, 0, NULL}
  };

/* The following strings are inserted in the above menu for OEM drivers. */

CHAR sInsertDisplay[] = "Display driver";
CHAR sInsertKB[] = "Keyboard driver";
CHAR sInsertMouse[] = "Mouse driver";
CHAR sInsertPrinter[] = "Printer or Plotter driver";
CHAR sInsertSysFont[] = "System Font";
CHAR sInsertOemFont[] = "Terminal Font";

/* Insertion string in smReading (if there is not yet a file name) */

CHAR sInsertDisk[] = "the Driver Disk";

/* These are used in a menu after certain OEM drivers are loaded */

CHAR sOemDisplayLoaded[] =
     "The selected display driver is loaded and can not be changed.";
CHAR sOemKBLoaded[] =
     "The selected keyboard driver is loaded and can not be changed.";
CHAR sOemMouseLoaded[] =
     "The selected mouse driver is loaded and can not be changed.";
CHAR sOemSysFontLoaded[] =
     "The selected System font is loaded and can not be changed.";
CHAR sOemOemFontLoaded[] =
     "The selected Terminal font is loaded and can not be changed.";


/* 
CHAR Warning[] = "Warning:";
*/

/* Ramdrive update Msg. */
/* all this was just put in the screen - no middleman
CHAR RamDriveL1[] =
"You currently have RAMDrive installed in your computer. In order for RAMDrive";
CHAR RamDriveL2[] = 
"to work correctly, you must use the latest RAMDrive, provided with this";
CHAR RamDriveL3[] = 
"version of \004. Setup can copy the latest RAMDrive to your computer";
CHAR RamDriveL4[] = 
"now, after creating a backup of your old one under the name RAMDRIVE.OLD.";
CHAR RamDriveL5[] =
" ";
CHAR RamDriveL6[] =
"For more information on installing RAMDRIVE.SYS see the readme file";
CHAR RamDriveL7[] = 
"RAMDRIVE.TXT after you have completed Setup.";
*/

/* Screen for updating RAMDRIVE.SYS if it exists. */

textline RamDriveMsg[] = {
  {  1,  1, 
"Warning:"},
  {  3,  1, 
"You currently have RAMDrive installed in your computer. In order for RAMDrive"},
  {  4,  1, 
"to work correctly, you must use the latest RAMDrive, provided with this"},
  {  5,  1, 
"version of \004. Setup can copy the latest RAMDrive to your computer"},
  {  6,  1, 
"now, after creating a backup of your old one under the name RAMDRIVE.OLD."},
  {  7,  1, 
" "},
  {  8,  1, 
"For more information on installing RAMDRIVE.SYS see the readme file"},
  {  9,  1, 
"RAMDRIVE.TXT after you have completed Setup."},
  { 13, 10, shBoxTop},
  { 14, 10, shBoxVL},
  { 14, 12, shBoxIf},
  { 14, 60, shBoxPr},
  { 14, 66, shBoxVL},
  { 15, 10, shBoxLi},
  { 16, 10, shBoxVL},
  { 16, 12, "Copy latest RAMDrive now"},
  { 16, 66, shBoxVL},
  { 17, 10, shBoxVL},
  { 17, 12, "Not Copy latest RAMDrive"},
  { 17, 66, shBoxVL},
  { 18, 10, shBoxLi},
  { 19, 10, shBoxVL},
  { 19, 12, shBoxCtrlX},
  { 19, 66, shBoxVL},
  { 20, 10, shBoxBo},
  {  0,  0, NULL}
};

textline RamDriveChoices[] = {
  { 16, 62, "C"},
  { 17, 62, "N"},
  { 0, 0, NULL}
  };

textline ConfigMsg[] = {
	{1, 1, 
"Setup needs to modify your CONFIG.SYS file to install the driver HIMEM.SYS."},
	{2, 1, 
"Setup can do this for you by modifying your CONFIG.SYS file to include the"},
	{3, 1, 
"the following line :"},
	{4, 1, 
"                    DEVICE = \HIMEM.SYS"},
	{6, 1, 
"Setup will create a backup of the old one under the name CONFIG.BAK."},
	{8, 1, 
"If your CONFIG.SYS is not modified to include the HIMEM.SYS line, \004"},
	{9, 1, 
"will not be able to take advantage of your extended memory. If you want to"},
	{10, 1,
"modify your CONFIG.SYS file manually, HIMEM.SYS will be available in the"},
{11, 1,
"\004 directory once Setup is complete."},
{12, 1,
"In any case, once your CONFIG.SYS file is modified and Setup is complete"},
{13, 1,
"you should reboot your machine so that your system recognizes the changes"},
{14, 1,
"made to your CONFIG.SYS file."},
   {16, 10, shBoxTop},
   {17, 10, shBoxVL},
   {17, 12, shBoxIf},
   {17, 60, shBoxPr},
   {17, 66, shBoxVL},
   {18, 10, shBoxLi},
   {19, 10, shBoxVL},
   {19, 12, "Update CONFIG.SYS"},
   {19, 66, shBoxVL},
   {20, 10, shBoxVL},
   {20, 12, "Not Update CONFIG.SYS"},
   {20, 66, shBoxVL},
   {21, 10, shBoxLi},
   {22, 10, shBoxVL},
   {22, 12, shBoxCtrlX},
   {22, 66, shBoxVL},
   {23, 10, shBoxBo},
	{0, 0 , NULL}
};

textline YesNoChoices[] = {
  { 19, 62, "Y"},
  { 20, 62, "N"},
  { 0, 0, NULL}
  };


textline BootDriveMsg[] = {
	{1, 1, "Setup needs to know where your CONFIG.SYS file exists on the hard disk."},
	{2, 1, " "},
	{3, 1, " "},
	{4, 1, "Please specify the boot Drive where Setup can find the CONFIG.SYS file."},
	{5, 1, "(If it doesn't exist Setup will create it.)"},
	{6, 1, " "},
	{7, 1, "- If it is a different drive, use the BACKSPACE key to delete"},
	{8, 1, "  the drive shown, then type the correct drive name and a colon."},
	{10, 2, ""},
	{0, 0, NULL}
};

textline BootDriveMsg1[] = {
	{1, 1, "Setup needs to know where your AUTOEXEC.BAT file exists on the hard disk."},
	{2, 1, " "},
	{3, 1, " "},
	{4, 1, "Please specify the boot Drive where Setup can find the AUTOEXEC.BAT file."},
	{5, 1, "(If it doesn't exist it will create it.)"},
	{6, 1, " "},
	{7, 1, "- If it is a different drive, use the BACKSPACE key to delete"},
	{8, 1, "  the drive shown, then type the correct drive name and a colon."},
	{10, 2, ""},
	{0, 0, NULL}
};

textline AutoexecMsg1[] = {
  {1, 1, 
"The PATH variable in your AUTOEXEC.BAT lists a directory containing"},
  {2, 1, 
"another version of \004. You can have Setup automatically modify the"},
  {3, 1, 
"PATH variable in your AUTOEXEC.BAT file to contain the directory for the"},
  {4, 1, 
"new version of \004. Setup will also create a backup of your current"},
  {5, 1, 
"AUTOEXEC.BAT file under the name AUTOEXEC.BAK."},
  {7, 1, 
"If Setup does not modify the PATH variable now, remember to do it yourself"},
  {8, 1, 
"after completing Setup."},
  {13, 10, shBoxTop},
  {14, 10, shBoxVL},
  {14, 12, "IF YOU WANT"},
  {14, 60, shBoxPr},
  {14, 66, shBoxVL},
  {15, 10, shBoxLi},
  {16, 10, shBoxVL},
  {16, 12, "Setup to modify your AUTOEXEC.BAT"},
  {16, 66, shBoxVL},
  {17, 10, shBoxVL},
  {17, 12, "No modification to AUTOEXEC.BAT"},
  {17, 66, shBoxVL},
  {18, 10, shBoxLi},
  {19, 10, shBoxVL},
  {19, 12, shBoxCtrlX},
  {19, 66, shBoxVL},
  {20, 10, shBoxBo},
  {0, 0 , NULL}
};

textline AutoexecMsg2[] = {
  {1, 1, 
"The PATH variable in the AUTOEXEC.BAT should be modified to include the"},
  {2, 1, 
"directory for the new version of \004."},
  {4, 1, 
"If you have an AUTOEXEC.BAT file, Setup can automatically modify it."},
  {5, 1, 
"Setup will create a backup of your current AUTOEXEC.BAT file under the name"},
  {6, 1, 
"AUTOEXEC.BAK. If Setup does not modify the PATH variable now, you will need"},
  {7, 1, 
"to do it yourself after completing Setup."},
  {9, 1, 
"If you do not have an AUTOEXEC.BAT file, Setup can automatically create"},
  {10, 1, 
"it now."},
  {13, 10, shBoxTop},
  {14, 10, shBoxVL},
  {14, 12, "IF YOU WANT"},
  {14, 60, shBoxPr},
  {14, 66, shBoxVL},
  {15, 10, shBoxLi},
  {16, 10, shBoxVL},
  {16, 12, "Setup to modify/create your AUTOEXEC.BAT"},
  {16, 66, shBoxVL},
  {17, 10, shBoxVL},
  {17, 12, "No modification to AUTOEXEC.BAT"},
  {17, 66, shBoxVL},
  {18, 10, shBoxLi},
  {19, 10, shBoxVL},
  {19, 12, shBoxCtrlX},
  {19, 66, shBoxVL},
  {20, 10, shBoxBo},
  {0, 0 , NULL}
};

textline AutoexecChoices[] = {
{16, 62, "Y"},
{17, 62, "N"},
{0, 0, NULL}
};


textline Incorrect386File[] = {
{1, 1, 
"WARNING:"},
{3, 1,
"The display driver specific files on the disk that you have inserted do"},
{4, 1, 
"not support the latest version of \004, version 2.11. If you install"},
{5, 1,
"this driver now, \004 version 2.03 will be loaded onto your"},
{6,1 ,
"computer system, and not version 2.11."},
{8, 1,
"Please contact your display manufacturer for information on when a driver"},
{9, 1, 
"for \004 version 2.11 will be available. Once you have the new driver"},
{10, 1, 
"files you can re-install \004."},
{15, 1, " "},
{0, 0, NULL}
};

textline MemsetMsg286[] = {
{1, 1, 
"    \004 can take advantage of extended/expanded memory and SMARTDrive"},
{2, 1, 
"to enhance its performance. Memset is a program designed to guide you"},
{3, 1, 
"through the setup procedure for extended/expanded memory and the SMARTDrive"},
{4, 1,
"disk-caching program."},
{0, 0, NULL}
};

textline MemsetMsg386[] = {
{1, 1, 
"    \004 can take advantage of extended memory and SMARTDrive"},
{2, 1, 
"to enhance its performance. Memset is a program designed to guide you"},
{3, 1, 
"through the setup procedure for extended memory and the SMARTDrive"},
{4, 1,
"disk-caching program."},
{0, 0, NULL}
};

textline MemsetMsg[] = {
{5,1, 
"    If a memory board is already installed correctly, run Memset now by"},
{6, 1, 
"pressing M."},
{7, 1, 
"    If you don't have a memory board installed now, you can still continue"},
{8, 1,
"with Setup by pressing C and Windows will be installed for your use,"},
{9,1, 
"though it won't be installed for optimum performance."},
{10, 1, 
"    To optimize your \004 performance, you can run Memset at a later"},
{11, 1,
"time once you have a memory board installed according to the instructions"},
{12, 1, 
"provided by the manufacturer and the guidelines in \"Setting Up Extended or"}, 
{13, 1, 
"Expanded Memory\" in the \"Microsoft Windows:  Questions and Answers\""},
{14, 1,
"booklet.  If you want to run Memset at a later time, type Memset at the"},
{15, 1,
"DOS prompt in your Windows directory."},
{18, 10, shBoxTop},
{19, 10, shBoxVL},
{19, 12, shBoxIf},
{19, 60, shBoxPr},
{19, 66, shBoxVL},
{20, 10, shBoxLi},
{21, 10, shBoxVL},
{21, 12, "Run Memset now"},
{21, 66, shBoxVL},
{22, 10, shBoxVL},
{22, 12, "Continue without running Memset"},
{22, 66, shBoxVL},
{23, 10, shBoxBo},
{  0,  0, NULL}
};

textline MemsetChoices[] = {
  { 21, 62, "M"},
  { 22, 62, "C"},
  { 0, 0, NULL}
};


textline MemsetFailMsg[] = {
	{1, 1, 
"Memset did not complete successfully."},
	{2, 1, 
"You will need to run Memset after you complete Setup. To do this, type"},
	{3, 1, 
"Memset at the prompt in the directory where \004 will be installed."},
	{5, 1 ,
"To continue with Setup press ENTER now."},
	{0, 0, NULL}
};


textline ExtMemMsg0[] = {
	{1, 1, "You have HIMEM.SYS installed on your system. Currently, this"},
	{2, 1, "high memory area is being used by some other program."},
	{3, 1, "\004 could make better use of this high memory to optimize its"},
	{4, 1, "performance. If you would like to re-configure your computer system to"},
	{5, 1, "enhance the performance of \004, press E to exit Setup, change your"},
	{6, 1, "CONFIG.SYS file to free the high memory area (first 64K of"},
	{7, 1, "extended memory), reboot your machine so that your system recognizes the"},
	{8, 1, "changes to your CONFIG.SYS file and then re-run Setup."},
	{10, 1, 
"We recommend that you reconfigure your system so that \004 is set up for"},
	{11, 1, 
"optimum performance. If you continue with Setup, \004 will still be"},
	{12, 1, 
"installed for your use, but it will not be set up for optimum performance."},
	{14, 1, " "},
   {16, 10, shBoxTop},
   {17, 10, shBoxVL},
   {17, 12, shBoxTo},
   {17, 60, shBoxPr},
   {17, 66, shBoxVL},
   {18, 10, shBoxLi},
   {19, 10, shBoxVL},
   {19, 12, "Exit Setup to modify your configuration"},
   {19, 66, shBoxVL},
   {20, 10, shBoxVL},
   {20, 12, "Continue (with no extended memory for \004)"},
   {20, 66, shBoxVL},
   {21, 10, shBoxLi},
   {22, 10, shBoxVL},
   {22, 12, shBoxCtrlX},
   {22, 66, shBoxVL},
   {23, 10, shBoxBo},
   { 0,  0, NULL}
};

textline ExtMemMsg1[] = {
	{1, 1, "Your computer system has more than 64K of extended memory. Currently, this"},
	{2, 1, "extended memory is being used by some other program."},
	{3, 1, "\004 could make better use of this extended memory to optimize its"},
	{4, 1, "performance. If you would like to re-configure your computer system to"},
	{5, 1, "enhance the performance of \004, press E to exit Setup, change your"},
	{6, 1, "CONFIG.SYS file to free at least 64K of extended memory,"},
	{7, 1, "reboot your machine so that your system recognizes the changes to your"},
	{8, 1, "CONFIG.SYS file and then re-run Setup."},
	{10, 1, "We recommend that you reconfigure your system so that \004 is set up for"},
	{11, 1, 
"optimum performance. If you continue with Setup, \004 will still be"},
	{12, 1, 
"installed for your use, but it will not be set up for optimum performance."},
	{14, 1, " "},
   {16, 10, shBoxTop},
   {17, 10, shBoxVL},
   {17, 12, shBoxTo},
   {17, 60, shBoxPr},
   {17, 66, shBoxVL},
   {18, 10, shBoxLi},
   {19, 10, shBoxVL},
   {19, 12, "Exit Setup to modify your configuration"},
   {19, 66, shBoxVL},
   {20, 10, shBoxVL},
   {20, 12, "Continue (with no extended memory for \004)"},
   {20, 66, shBoxVL},
   {21, 10, shBoxLi},
   {22, 10, shBoxVL},
   {22, 12, shBoxCtrlX},
   {22, 66, shBoxVL},
   {23, 10, shBoxBo},
   { 0,  0, NULL}
};

textline DefaultExitChoices[] = {
  { 19, 62, "E"},
  { 20, 62, "C"},
  { 0, 0, NULL}
};

textline ExtMemMsg2[] = {
   {1, 1, "Extended Memory Setting: "},
   {2, 1, 
"\004 can use extended memory to optimize its performance. Based on your"},
	{3, 1, 
"hardware configuration, Setup has determined the appropriate extended"},
	{4, 1, 
"memory setting."},
	{6, 1, 
"If you are unfamiliar with memory configurations, simply press ENTER and"},
   {7, 1, 
"Setup will automatically select the appropriate option."},
	{9, 1, 
"If you want to change this setting, please note the following points,"},
	{10, 1,
" - Choose E ONLY if you have more than 64K of extended memory available and"},
   {11, 1,
"   you want \004 to use it."},
   {12, 1,
" - Choose N if you do not have extended memory, OR if the available extended"},
   {13, 1,
"   memory is less than 64K, OR if you do not want \004 to use extended"},
	{14, 1,
"   memory."},
   {16, 10, shBoxTop},
   {17, 10, shBoxVL},
   {17, 12, shBoxIf},
   {17, 60, shBoxPr},
   {17, 66, shBoxVL},
   {18, 10, shBoxLi},
   {19, 10, shBoxVL},
   {19, 12, "Extended memory available"},
   {19, 66, shBoxVL},
   {20, 10, shBoxVL},
   {20, 12, "No extended memory"},
   {20, 66, shBoxVL},
   {21, 10, shBoxLi},
   {22, 10, shBoxVL},
   {22, 12, shBoxCtrlX},
   {22, 66, shBoxVL},
   {23, 10, shBoxBo},
   { 0,  0, NULL}
};

textline ExtMemChoices2[] = {
  { 19, 62, "E"},
  { 20, 62, "N"},
  { 0, 0, NULL}
  };

textline WinIniMsg[] = {
	{1, 1, "You have a WIN.INI file which you may want to save. Setup normally saves"},
	{2, 1, "this under the name WIN.OLD, but WIN.OLD already exists."},
	{3, 1, " "},
	{4, 1, "  - If it is OK to save your WIN.INI as WIN.OLD (i.e. overwrite your WIN.OLD),"},
	{5, 1, "    press ENTER."},
	{6, 1, " "},
	{7, 1, "  - If you want to save the existing WIN.INI under a different name"},
	{8, 1, "    use the BACKSPACE key to delete the name shown, then type the correct name"},
	{9, 1, "    and press ENTER. "},
	{11, 2, " "},
	{0, 0, NULL}
};


/* Menu selections for Printer selection in SUPRINTR.C */

CHAR sNullPort[] = "None";              /* this should be the same as the
					** NullPort string in WIN.INI */
/* String which is displayed as the first entry in a selection list if
** there is no default (OEM driver, printer)
*/
CHAR smNoSelection[] = "No selection";

 
textline MenuWantPrinter[] = {
  { 1, 1, "Setup will allow you to install a printer or plotter." },
  { 4, 10, shBoxTop},
  { 5, 10, shBoxVL},
  { 5, 12, shBoxTo},
  { 5, 60, shBoxPr},
  { 5, 66, shBoxVL},
  { 6, 10, shBoxLi},
  { 7, 10, shBoxVL},
  { 7, 12, "Install a printer or plotter"},
  { 7, 66, shBoxVL},
  { 8, 10, shBoxVL},
  { 8, 12, "Continue Setup"},
  { 8, 66, shBoxVL},
  { 9, 10, shBoxLi},
  {10, 10, shBoxVL},
  {10, 12, shBoxCtrlX},
  {10, 66, shBoxVL},
  {11, 10, shBoxBo},
  { 0, 0, NULL}
  };

textline MenuWantAnotherPrinter[] = {
  { 1, 1, "You can install another printer or plotter." },
  { 4, 10, shBoxTop},
  { 5, 10, shBoxVL},
  { 5, 12, shBoxTo},
  { 5, 60, shBoxPr},
  { 5, 66, shBoxVL},
  { 6, 10, shBoxLi},
  { 7, 10, shBoxVL},
  { 7, 12, "Install another printer or plotter"},
  { 7, 66, shBoxVL},
  { 8, 10, shBoxVL},
  { 8, 12, "Continue Setup"},
  { 8, 66, shBoxVL},
  { 9, 10, shBoxLi},
  {10, 10, shBoxVL},
  {10, 12, shBoxCtrlX},
  {10, 66, shBoxVL},
  {11, 10, shBoxBo},
  { 0, 0, NULL}
  };

textline PrinterYesNo[] = {
  { 7, 62, "I"},
  { 8, 62, "C"},
  { 0, 0, NULL}
  };

textline MenuInstallPrinter[] = {
  { 1, 1,
  "Select an output device (printer, plotter, etc.) from the following list." },
  { 3, 3, sListKeys1},
  { 0, 0, NULL}
  };

textline MenuSelectPort[] = {
  { 1, 1,
  "Select the port your printer/plotter is attached to from the following list." },
  { 3, 3, sListKeys1},
  { 0, 0, NULL}
  };


/* screen for [intl](WIN.INI)/[country](SETUP.INF) selection */

textline CountryMenu[] = {
  { 1, 1,
  "To create appropriate country settings (currency symbol, time format, etc.),"},
  { 2, 1,
  "please select a country from the following list."},
  { 0, 0, NULL}
  };

/* selection entry if country is not in SETUP.INF, but the information
** is read from DOS
*/
CHAR smDosCountry[] = "To install country information from DOS";


/* screen prompting for displaying readme files */
textline ReadmeMsg[] = {
{ 1, 1,
"There is advanced information on \004 available in the readme files" },
{ 2, 1,
"which are located on the \004 disks. You can review this information" },
{ 3, 1,
"later using the Windows application Notepad, which is documented in the"},
{4, 1, 
"Microsoft Windows User's Guide."},
{ 6, 1,
"Press any key to continue Setup ..." },
{ 0, 0, NULL}
};

textline ReadmeChoices[] = {
  { 12, 62, "V"},
  { 13, 62, "F"},
  { 0, 0, NULL}
  };


textline ReinstallFinishedMsg1[] = {
  { 3, 1, "SETUP HAS NOW BEEN COMPLETED SUCCESSFULLY."},
  { 5, 1, 
"PLEASE REMOVE THE DISK FROM DRIVE A: AND REBOOT YOUR SYSTEM NOW BY"},
{6, 1,
"PRESSING CONTROL+ALT+DEL."},
  { 8, 1, 
"Setup updated the version of Windows already installed on your hard disk."},
  { 9, 1, 
"A new version of the Windows configuration file (WIN.INI) was copied to your"},
  { 10,1, 
"hard disk. Your existing WIN.INI file has been renamed \013."},
  { 11,1, 
"See the README.TXT file for more information on earlier versions of Windows."},
  { 13,1, 
"If Setup did not update your CONFIG.SYS or AUTOEXEC.BAT file you must do it"},
  { 14,1, 
"yourself and then reboot your system before running \004."},
  { 16,1, 
"If you were unable to run Memset from within Setup, run Memset"},
  { 17,1, 
"by typing Memset at the prompt in the Windows directory."},
  { 19,1, 
"After rebooting, \004 can be started by typing \006 and pressing"},
  {20, 1,
"the ENTER key."},
  {22, 1, " "},

  { 0, 0, NULL}
  };

textline FinishedMsg1[] = {
  { 3, 1, "SETUP HAS NOW BEEN COMPLETED SUCCESSFULLY."},
  { 5, 1, 
"PLEASE REMOVE THE DISK FROM DRIVE A: AND REBOOT YOUR SYSTEM NOW BY"},
  {6, 1, 
"PRESSING CONTROL+ALT+DEL."},
  { 8, 1, "If Setup did not update your CONFIG.SYS or AUTOEXEC.BAT file you must do it"},
  { 9, 1, "yourself and then reboot your system before running \004."},
  { 11,1, "If you were unable to run Memset from within Setup, run Memset"},
  { 12,1, "by typing Memset at the prompt in the Windows directory."},
  { 14,1, 
"After rebooting, \004 can be started by typing \006 and pressing"},
  {15, 1, 
"the ENTER key."},
  {17, 1, " "},
    { 0, 0, NULL}
  };

textline ReinstallFinishedMsg2[] = {
{1, 1, "Setup will now run Memset."},
{2, 1, "Before Memset is run please note the following things :"},
{5, 1, "SETUP HAS NOW BEEN COMPLETED SUCCESSFULLY."},
{7, 1, "Setup updated the version of Windows already installed on your hard disk."},
{8, 1, "A new version of the Windows configuration file (WIN.INI) was copied to your"},
{9,1, "hard disk. Your existing WIN.INI file has been renamed \013."},
{10,1, "See the README.TXT file for more information on earlier versions of Windows."},
{12,1, "If Setup did not update your CONFIG.SYS or AUTOEXEC.BAT file you must do it"},
{13,1, "yourself and then reboot your system before running \004."},
{14,1, 
"After rebooting, \004 can be started by typing \006 and pressing"},
{15, 1,
"the ENTER key."},
{19, 1, "Press ENTER now to run Memset ..."},
{ 0, 0, NULL}
};

textline FinishedMsg2[] = {
{1, 1, "Setup will now run Memset."},
{2, 1, "Before Memset is run please note the following things :"},
{5, 1, "SETUP HAS NOW BEEN COMPLETED SUCCESSFULLY."},
{7, 1, "If Setup did not update your CONFIG.SYS or AUTOEXEC.BAT file you must do it"},
{8, 1, "yourself and then reboot your system before running \004."},
{9,1, 
"After rebooting, \004 can be started by typing \006 and pressing"},
{10, 1, 
"the ENTER key."},
{14, 1, "Press ENTER now to run Memset ..."},
{0, 0, NULL}
};


textline LeaveSetupMsg[] = {
  {  8, 10, "\004 is not properly installed."},
  { 10, 10, "To prepare \004 to run on your computer, run the Setup"},
  { 11, 10, "program again."},
  { 20,  1, ""},    /* to position the cursor */
  {  0,  0, NULL}
  };


/* Incorrect driver (e.g. VDISK.SYS) message */
/* BILLHU changed text */

textline txCompat[] = {
{ 1,  1,
"Setup has detected a device driver in your CONFIG.SYS file that may"},
{ 2,  1,
"interfere with Windows' use of extended memory.  You can either"},
{ 3,  1,
"reconfigure your system now, or continue with Setup.  We recommend that"},
{ 4,  1,
"you reconfigure your system so that Setup can install Windows' extended"},
{ 5,  1,
"memory driver."},

{ 7,  1,
"If you want to reconfigure your system now, you must exit Setup and"},
{ 8,  1,
"remove the following line(s) from your CONFIG.SYS file.  After removing the"},
{ 9,  1,
"lines, reboot your machine and then rerun Setup."},

	/* allow space for 10 CONFIG.SYS lines + lines above & below */
	/* config.sys lines start on line 11.  If you make a change here that
		affects that number, edit sucompat.c to fix it there too */

{21,  1, 
"Continue Setup without installing extended memory."},
{22,  1, 
"Exit Setup to modify your CONFIG.SYS file."},

{ 9,  1, ""}, /* position cursor to display CONFIG.SYS lines in space above */

{ 0,  0, NULL}
};

textline CompatChoices[] = {
  { 21, 62, "C"},
  { 22, 62, "E"},
  { 0, 0, NULL}
  };
