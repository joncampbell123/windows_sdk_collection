*-- (c) Microsoft Corporation 1995

*-- Common include file
#INCLUDE "FOXPRO.H"
#INCLUDE "STRINGS.H"

#DEFINE DEBUGMODE	.T.
#DEFINE INIFILE		"TASTRADE.INI"
#DEFINE CRLF		CHR(13) + CHR(10)
#DEFINE CR			CHR(13)
#DEFINE TAB			CHR(9)

#DEFINE CURRENCY	"$"
#DEFINE AERRORARRAY	7

*-- These constants are used in tsbaseform to 
*-- indicate the status of the current alias
#DEFINE FILE_OK		0
#DEFINE FILE_BOF	1
#DEFINE FILE_EOF	2
#DEFINE FILE_CANCEL	3

*-- Constants to identify which trigger failed
*-- using element 5 of the array returned by 
*-- AERROR(), as well as to reference the appropriate
*-- array element in the error message array: aErrorMsg[]
#DEFINE INSERTTRIG  1
#DEFINE UPDATETRIG  2
#DEFINE DELETETRIG  3

*-- Constants used to read the system registry
#DEFINE HKEY_LOCAL_MACHINE			-2147483646  
#DEFINE KEY_SHARED_TOOLS_LOCATION	"Software\Microsoft\Shared Tools"
#DEFINE KEY_NTCURRENTVERSION		"Software\Microsoft\Windows NT\CurrentVersion"
#DEFINE KEY_WIN4CURRENTVERSION		"Software\Microsoft\Windows\CurrentVersion"
#DEFINE KEY_WIN4_MSINFO				"Software\Microsoft\Shared Tools\MSInfo"
#DEFINE KEY_QUERY_VALUE				1
#DEFINE ERROR_SUCCESS				0

#DEFINE ADMINBAR_LOC "Administration"
#DEFINE ALL_LOC "All"

#DEFINE USER_APPDEV_LOC "APPLICATIONS DEVELOPER"
#DEFINE USER_OPSMGR_LOC "OPERATIONS MANAGER"

#DEFINE DOLLAR_FORMAT1_LOC ": $"
#DEFINE DOLLAR_FORMAT2_LOC ""
#DEFINE DOLLAR_FORMAT3_LOC "$"
#DEFINE SEEKVALUE_LOC	"*Case Study"

#DEFINE SYS2011_EXCLUSIVE_LOC 	"EXCLUSIVE"
#DEFINE SYS2011_RECLOCK_LOC 	"RECORD LOCKED"
#DEFINE SYS2011_RECUNLOCK_LOC 	"RECORD UNLOCKED"

#DEFINE	I_SHPMIN	111			&& how far left can the Behind the Scenes splitter go?
#DEFINE I_SHPMAX	303			&& how far right can the Behind the Scenes splitter go?
