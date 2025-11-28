*
*Obvious note to localizers: anything that doesn't end with "_LOC" should be left alone
*

*Any given #def may be used in several places; the notes on the
*"location" where it's used is mostly for the developer's benefit

#DEFINE HELP_OPENDBC	489321236
#DEFINE HELP_CONFLICTS	489321237
#DEFINE HELP_SAMPLE		489321238


* Operating System codes
#DEFINE	OS_W32S				1
#DEFINE	OS_NT				2
#DEFINE	OS_WIN95			3
#DEFINE	OS_MAC				4
#DEFINE	OS_DOS				5
#DEFINE	OS_UNIX				6
#define ERRORTITLE_LOC		"Microsoft Visual FoxPro"
#DEFINE CS_TIMEOUT			5

#define ERRORMESSAGE_LOC ;
	"Error #" + alltrim(str(m.nError)) + " in " + m.cMethod + ;
	" (" + alltrim(str(m.nLine)) + "): " + m.cMessage
#define MB_ICONEXCLAMATION		48
#define MB_ABORTRETRYIGNORE		2
#define MB_OK					0
#define ALERTTITLE_LOC		"Microsoft Visual FoxPro"
#define THERMCOMPLETE_LOC	"Complete."
#DEFINE	C_DEBUG				.F.

* Client server application defs
#DEFINE CS_DATABASE						"cs.dbc"
#DEFINE CS_CONNECTION					"emp_connection"
#DEFINE CS_TABLE						"employee"
#DEFINE CS_INITIALTABLE					"emp_init"
#DEFINE CS_LOCAL_VIEW					"emp_view_local"
#DEFINE CS_REMOTE_VIEW					"emp_view_remote"
#DEFINE CS_CONFLICTS					"conflicts"
#DEFINE CS_BIZRULES						"bizrules"

#DEFINE CS_ROWCONFLICT_LOC				"Conflicts on row:  "

#DEFINE CS_STATUS_LOC 					"Status: "
#DEFINE CS_STATTBL_LOC 					"Table: "
#DEFINE CS_STATLV_LOC 					"Local View: "
#DEFINE CS_STATRV_LOC 					"Remote View: "
#DEFINE CS_STATOPENEX_LOC 				"OPEN  EXCLUSIVE"
#DEFINE CS_STATOPENSH_LOC 				"OPEN  SHARED"
#DEFINE CS_STATNOOPEN_LOC 				"NOT  OPEN"
#DEFINE CS_STATONLINE_LOC 				"ONLINE"
#DEFINE CS_STATOFFLINE_LOC 				"OFFLINE"

#DEFINE NO_CURSOR						0
#DEFINE TABLE_CURSOR					1
#DEFINE LOCAL_VIEW_CURSOR				2
#DEFINE REMOTE_VIEW_CURSOR				3

#DEFINE DBC_TYPE						"DBC"
#DEFINE GETFILE_LOC						"File Name:"
#DEFINE GETFILEBTN_LOC					"OK"

* Client server Info messages
#DEFINE CS_DATABASE_INFO_LOC		"CS.DBC is the database required by this sample. Make " +;
								"sure the full path to CS.DBC is displayed in the text box." + ;
								" If it isn't, choose the dialog button beside the text box " + ;
								"and locate CS.DBC."
#DEFINE CS_DATABASE_INFO1_LOC		"Choose Open to open the CS.DBC database. Make sure Exclusive " + ;
								"is selected if you want to be able to modify the database in the sample."
#DEFINE CS_DATABASE_INFO2_LOC		"In order to upsize and view remote data you need to set up a connection " + ;
								"to a back end database. You only need to setup the connection the first " + ;
								"time you run this sample."
#DEFINE CS_CURSOR_INFO_LOC			"Select a cursor type based on a Table, Local View or Remote View. ;
For Table cursors you can reload the initial data and upsize the table. For View cursors you can " + ;
"toggle between the online and offline modes. Then select the concurrency option and open the cursor."

#DEFINE CS_FUNCTIONS_INFO_LOC	"Information about updates functionality."
#DEFINE CS_CONFLICTS_INFO_LOC	"Information about conflict resolution."

#DEFINE CONNECT_FAILURE_LOC		"Upsizing cannot continue because a connection error occurred"
#DEFINE	CANT_OPEN_EXCLU_LOC		"Unable to open '|1' exclusively."
#DEFINE NO_SUCH_FILE_LOC		"File does not exist."
#DEFINE BIZRULEERROR_LOC		"Business Rule Violation Error"
 
#DEFINE	WAIT_UPDATEOK_LOC		"Update succeeded"
#DEFINE	WAIT_UPDATEFAIL1_LOC	"Update failed, the primary key has changed"
#DEFINE	WAIT_UPDATEFAIL2_LOC	"Update failed"
#DEFINE WAIT_REVERTFAIL_LOC		"Revert failed"
#DEFINE WAIT_REVERT1_LOC		"1 row reverted"
#DEFINE WAIT_REVERT2_LOC		" rows reverted"
#DEFINE WAIT_REQUERYOK_LOC		"Requery succeeded"
#DEFINE WAIT_REQUERYFAIL_LOC	"Requery failed"

#DEFINE ERR_NOOPENEMP_LOC		"Unable to open Employee table."
#DEFINE WAIT_UPSIZEOK_LOC		"Upsizing table succeeded."