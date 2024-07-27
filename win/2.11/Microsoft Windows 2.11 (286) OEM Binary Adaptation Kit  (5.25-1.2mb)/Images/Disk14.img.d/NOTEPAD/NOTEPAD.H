#define NOCOMM
#define NOSOUND
#include "windows.h"
#include "winexp.h"



#define ID_CURSOR	1
#define ID_ICON 	1
#define ID_MENUBAR	1

#define IDD_OPEN	1
#define IDD_SAVEAS	2
#define IDD_SAVECHANGES 3
#define IDD_CREATE	4
#define IDD_FILETOOBIG	5
#define IDD_DISKERR	6
#define IDD_READONLY	7
#define IDD_CHECKSAVE	8
#define IDD_SEARCH	9
#define IDD_ABOUT	10
#define IDD_SC		11
#define IDD_ABORTPRINT	12

/* Menu IDs */
#define M_OPEN	       10
#define M_SAVE		1
#define M_SAVEAS	2
#define M_CUT		WM_CUT
#define M_COPY		WM_COPY
#define M_PASTE 	WM_PASTE
#define M_CLEAR 	WM_CLEAR
#define M_FIND		3
#define M_DOSEARCH	4
#define M_HELP		5
#define M_SELECTALL	7
#define M_FINDNEXT	8
#define M_NEW		9
#define M_ABOUT 	11
#define M_DATETIME	12
#define M_PRINT 	14
#define M_UNDO		25
#define M_NOWW		26
#define M_WW		27
#define M_EXIT          28


#define ID_EDIT 	15
#define ID_LISTBOX	16
#define ID_DIRECTORY	17
#define ID_PATH 	18
#define ID_FILENAME	20

#define ID_SEARCH	19
#define ID_SRCHFWD	10
#define ID_SRCHBACK	11
#define ID_SRCHCASE	12
#define ID_SRCHNOCASE	13
#define ID_PFREE	14

#define CBBUFFER	2048

#define IDS_DISKERROR	1
#define IDS_PA		2
#define IDS_FNF 	3
#define IDS_CNF 	4
#define IDS_FAE 	5
#define IDS_OEF 	6
#define IDS_ROM 	7
#define IDS_YCNCTF	8
#define IDS_UE		9
#define IDS_SCBC	10
#define IDS_UNTITLED	11
#define IDS_NOTEPAD	12
#define IDS_LF		13
#define IDS_SF		14
#define IDS_RO		15
#define IDS_CFS 	16
#define IDS_ERRSPACE	17
#define IDS_FTL 	18
#define IDS_NN		19
#define IDS_ABOUT	20
#define IDS_PASTEERR	21
#define IDS_WRITEERR	22
#define IDS_INF 	23
#define IDS_EFD 	24
#define IDS_CSEF	25
#define IDS_CANTPRINT	26
#define IDS_NVF 	27
#define IDS_NVF2	28
#define IDS_NEDSTP	29
#define IDS_NEMTP	30
#define IDS_CREATEERR	31
#define IDS_NOWW	32
#define IDS_MERGE1	33
#define IDS_EXT         34

#define CSTRINGS        25  /* count of stringtable strings from .rc file */


#define CDIALOGS	6
#define IFNOPEN 	0
#define IFNSAVEAS	1
#define IFNSAVECHANGES	2
#define IFNCREATE	3
#define IFNFIND 	4
#define IFNABOUT	5

#define CCHKEYMAX       32      /* max characters in search string */
#define CCHLINEMAX      160     /* max chars on a line - NO LONGER CHECKED.01-Jul-1987 */
#define CCHNPMAX        16384   /* max number of bytes in a notepad file */
#define CCHFILENAMEMAX  128     /* max number of characters in filename */
#define CXMARGIN        8       /* left and right margin */
#define CYMARGIN        2       /* top and bottom margin */


void far InsertDateTime();
BOOL far CheckLineLength();
int far SetTitle();
int far AddExt();
int far AlertBox();
int far LoadFile();
BOOL far SaveFile();
BOOL far CheckSave();
void far New();
void far Search();
BOOL far NpReCreate();
int far InitCountry();
BOOL	far MergeStrings();
