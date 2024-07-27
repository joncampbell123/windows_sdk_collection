/* SUTEST -- program to test screen messages (localizing aid)
**
**	Modifications history
**
** 31 jan 89    wch   removed references to DisketteFinishedMsg
**  1 feb 89    wch   updated txCompat to selection screen
**  6 feb 89    wch   removed references to unused screens/menus
*/


#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include "setup.h"

#define TYP_PATH 1         /* input path */
#define TYP_SEL  2         /* select with one character */
#define TYP_LIST 3         /* select from list */
#define TYP_CQ   4         /* continue / quit message */


/* externals from SUDATA.C */

extern textline CopyMsg[];
extern textline ReinstallFinishedMsg1[];
extern textline FinishedMsg1[];
extern textline ReinstallFinishedMsg2[];
extern textline FinishedMsg2[];
extern textline MenuWantPrinter[];
extern textline MenuWantAnotherPrinter[];
extern textline PrinterYesNo[];
extern textline MenuInstallPrinter[];
extern textline MenuSelectPort[];
extern textline CountryMenu[];
extern textline LeaveSetupMsg[];
extern textline RamDriveMsg[];
extern textline RamDriveChoices[];
extern textline ConfigMsg[];
extern textline YesNoChoices[];
extern textline BootDriveMsg[];
extern textline BootDriveMsg1[];
extern textline AutoexecMsg1[];
extern textline AutoexecChoices[];
extern textline AutoexecMsg2[];
extern textline Incorrect386File[];
extern textline MemsetMsg[];
extern textline MemsetChoices[];
extern textline MemsetFailMsg[];
extern textline DefaultExitChoices[];
extern textline ExtMemMsg0[];
extern textline ExtMemMsg1[];
extern textline ExtMemMsg2[];
extern textline ExtMemChoices2[];
extern textline WinIniMsg[];
extern textline ReadmeMsg[];
extern textline ReadmeChoices[];
extern textline IncorrectDOS20[];
extern textline IncorrectDOS386[];
extern textline NotA386Machine[];
extern textline txCompat[];
extern textline CompatChoices[];
extern textline MemsetMsg286[];
extern textline MemsetMsg386[];



extern CHAR smDisplay[];
extern CHAR smInfOpenErr[];
extern CHAR smNoSelection[];
extern CHAR sNullPort[];
extern CHAR smDosCountry[];
extern CHAR smInfMissingErr[];
extern CHAR smDirErr[];
extern CHAR smCurrentDir[];
extern CHAR smSelectionInsertDisk[];
extern CHAR sInsertDisk[];
extern CHAR smDosCountry[];
extern CHAR smBackUpFail[];
extern CHAR smMissingID[];
extern CHAR smIllegalID[];
extern CHAR smMissingNoMouseRec[];
extern CHAR smNoDisplayRec[];
extern CHAR smRAMDRVBackUpFail[]; 
extern CHAR smCONFIGBackUpFail[];
extern CHAR smConfigModfFail[];
extern CHAR smAutoExecModfFail[];
extern CHAR smWinIniBackUpFail[];
extern CHAR smCopyHimemFail[];
extern CHAR smHimemExists[];


typedef struct {                   /* struct for MS-DOS Find First/Next File */
	CHAR *descr;
	textline *msg;
	textline *choices;
	int typ;
} DataStruct;

static CHAR *list[] =
  { smNoSelection,
    sNullPort,
    smDosCountry,
    smSelectionInsertDisk,
    "{item 5}",
    "{item 6}",
    "{item 7}",
    "{item 8}",
    "{item 9}",
    "{item 10}",
    "{item 11}",
    "{item 12}",
    "{item 13}",
    "{item 14}",
    "{item 15}",
    "{item 16}",
    "{item 17}",
    "{item 18}",
    "{item 19}",
    "{item 20}",
    "{item 21}",
    "{item 22}",
    NULL
  };

static textline ErrorMsg1[] = {
  {  1, 1, "{smReading..}"},
  {  2, 1, smReading},
  {  3, 1, smLoading},
  {  4, 1, smWriting},
  {  5, 1, smCopying},
  {  6, 1, smDisplay},
  {  7, 1, smInfErr},
  {  8, 1, smInfMissingErr},
  {  9, 1, smNoSection},
  { 10, 1, smNoMemory},
  { 11, 1, smNoFile},
  { 12, 1, smLoadErr},
  { 13, 1, smWriteErr},
  { 14, 1, smOpenErr},
  { 15, 1, smDirErr},
  { 16, 1, smInfOpenErr},
  { 17, 1, smCurrentDir},
  { 18, 1, "{sInsertDisk}"},
  { 19, 1, sInsertDisk},
  {  0, 0, NULL}
};

static textline ErrorMsg2[] = {
  {  2, 1, smMissingID},
  {  3, 1, smIllegalID},
  {  4, 1, smMissingNoMouseRec},
  {  5, 1, smNoDisplayRec},
  {  6,1 , smRAMDRVBackUpFail},
  {  7, 1, smCONFIGBackUpFail},
  {8, 1, smConfigModfFail},
	{9, 1, smAutoExecModfFail},
	{10, 1, smWinIniBackUpFail},
	{11, 1,smCopyHimemFail},
	{12, 1,  smHimemExists},
  { 0, 0, NULL}
};

static textline DriverInstalledMsg[] = {
  {  1, 1, MsgNoChange},
  {  2, 1, sOemDisplayLoaded},
  {  3, 1, sOemKBLoaded},
  {  4, 1, sOemMouseLoaded},
  {  5, 1, sOemSysFontLoaded},
  {  6, 1, sOemOemFontLoaded},
  {  0, 0, NULL}
};

static textline DriverTypeMenu[] = {
  {  1, 1, sInsertDisplay},
  {  2, 1, sInsertKB},
  {  3, 1, sInsertMouse},
  {  4, 1, sInsertPrinter},
  {  5, 1, sInsertSysFont},
  {  6, 1, sInsertOemFont},
  {  0, 0, NULL}
};

static DataStruct Data[] =
{
 {"Error and status messages - Screen 1", ErrorMsg1, 0, 0},
 {"Error and status messages - Screen 2", ErrorMsg2, 0, 0},
 {"Incorrect DOS Version for Win 2.0", IncorrectDOS20, 0, 0 },
 {"Incorrect DOS Version for Win 386", IncorrectDOS386, 0, 0 },
 {"Not a 80386 processor", NotA386Machine, 0 , 0},
 {"SetupBanner", SetupBanner, 0, TYP_CQ},
 {"txCompat, CompatChoices", txCompat, CompatChoices, TYP_SEL},
 {"PathMenu", PathMenu, 0, TYP_PATH},
 {"CopyMsg", CopyMsg, 0, 0},
 {"FinalMenu", FinalMenu, 0, TYP_LIST },
 {"MachineMenu", MachineMenu, 0, TYP_LIST},
 {"DriversMenu", DriversMenu, 0, TYP_LIST},
 {"appropriate messages for drivers menu (MsgNoChange, sOem???Loaded)",
  DriverInstalledMsg, 0, 0},
 {"DisplayMenu", DisplayMenu, 0, TYP_LIST},
 {"KeyboardMenu", KeyboardMenu, 0, TYP_LIST},
 {"MouseMenu", MouseMenu, 0, TYP_LIST},
 {"SysFontMenu", SysFontMenu, 0, TYP_LIST},
 {"OemFontMenu", OemFontMenu, 0, TYP_LIST},
 {"InsertMenu", InsertMenu, 0, TYP_PATH},
 {"InsertOemMenu", InsertOemMenu, 0, TYP_PATH},
 {"SelectOemMenu", SelectOemMenu, 0, TYP_LIST},
 {"appropriate messages for last two menus (sInsert???)", DriverTypeMenu,
  0, 0},
 {"RamDriveMsg, RamDriveChoices", RamDriveMsg, RamDriveChoices, TYP_SEL},
 {"ConfigMsg, YesNoChoices", ConfigMsg, YesNoChoices, TYP_SEL},
 {"BootDriveMsg", BootDriveMsg, 0, TYP_PATH},
 {"BootDriveMsg1", BootDriveMsg1, 0, TYP_PATH},
 {"AutoexecMsg1", AutoexecMsg1, AutoexecChoices, TYP_SEL},
 {"AutoexecMsg2", AutoexecMsg2, AutoexecChoices, TYP_SEL},
 {"Incorrect386File", Incorrect386File, 0, TYP_CQ},
 {"MemsetMsg286", MemsetMsg286, 0, 0},
 {"MemsetMsg386", MemsetMsg386, 0, 0},
 {"MemsetMsg, MemsetChoices", MemsetMsg, MemsetChoices, TYP_SEL},
/* ZZZ
 {"CEMMMsg, DefaultExitChoices1", CEMMMsg, DefaultExitChoices1, TYP_SEL},
*/
 {"ExtMemMsg0, DefaultExitChoices", ExtMemMsg0, DefaultExitChoices, TYP_SEL},
 {"ExtMemMsg1, DefaultExitChoices", ExtMemMsg1, DefaultExitChoices, TYP_SEL},
 {"ExtMemMsg2, ExtMemChoices2", ExtMemMsg2, ExtMemChoices2, TYP_SEL},
 {"WinIniMsg", WinIniMsg, 0, TYP_PATH},
 {"MenuWantPrinter", MenuWantPrinter, PrinterYesNo, TYP_SEL},
 {"MenuWantAnotherPrinter", MenuWantAnotherPrinter, PrinterYesNo, TYP_SEL},
 {"MenuInstallPrinter", MenuInstallPrinter, 0, TYP_LIST},
 {"MenuSelectPort", MenuSelectPort, 0, TYP_LIST},
 {"CountryMenu", CountryMenu, 0, TYP_LIST},
 {"ReadmeMsg", ReadmeMsg, 0, 0},
 {"ReinstallFinishedMsg1", ReinstallFinishedMsg1, 0, 0},
 {"FinishedMsg1", FinishedMsg1, 0, 0},
 {"ReinstallFinishedMsg2", ReinstallFinishedMsg2, 0, 0},
 {"FinishedMsg2", FinishedMsg2, 0, 0},
 {"LeaveSetupMsg", LeaveSetupMsg, 0, 0},
 {0}
};


/* definitions which are needed for suscr.c, these definitions are part
 * of other modules normally
 */
union dfile * pheads[99];
BOOL IsQuick;


static void InitInserttext()
{
    inserttext[3] = "[program name]";
    inserttext[4] = "[long program name (version..)]";
    inserttext[5] = "[command]";
    inserttext[9] = "[disk name, e.g. installation disk ?]";
    inserttext[10]= "[file name]";
    inserttext[11]= "section/driver";
    inserttext[12]= "[destination directory]";
    inserttext[13]= "[number]";
}

main(argc, argv)
	int argc;
CHAR * argv[];
{
DataStruct *pd;

	InitInserttext();
	for (pd = Data; pd->descr; pd++) {
		ScrClear();
		ScrDisplay(pd->msg);

		switch (pd->typ) {
		case 0:
			getchw();
			break;
		case TYP_PATH:
			ScrInputPath("A:", "    ", 3);
			break;
		case TYP_SEL:
			ScrSelectChar(pd->choices, 1);
			break;
		case TYP_LIST:
			ScrSelectList(list);
			break;
		case TYP_CQ:
			ContinueOrQuit();
		}
		ScrClear();

		puts("last page:");
		puts(pd->descr);
		puts("");
		puts("next page:");
		puts((pd+1)->descr);

		getchw();
	}

	exit (0);
} /* main */

