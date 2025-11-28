* TRANSFRM.H - Header file for Transformer files.
*

*-- Titles
#DEFINE T_TRANSFORMER_LOC			"Transformer"

*-- Fonts
#DEFINE F_ARIAL_LOC					"Arial"
#DEFINE F_MS_SANS_SERIF_LOC			"MS Sans Serif"

*-- ASCII codes
#DEFINE	MARKER			CHR(1)
#DEFINE	TAB				CHR(9)
#DEFINE	LF				CHR(10)
#DEFINE	CR				CHR(13)
#DEFINE CR_LF			CR+LF

*-- Rule codes
#DEFINE	RULE_CODE_WIN		CHR(160)
#DEFINE	RULE_CODE_MAC		CHR(202)
#DEFINE RULE_EXCLUDE		RULE_CODE_WIN+'NOT'+RULE_CODE_WIN
#DEFINE RULE_INCLUDE		RULE_CODE_WIN+CHR(171)+RULE_CODE_WIN
#DEFINE RULE_SEPARATOR		RULE_CODE_WIN+RULE_CODE_WIN+'|'+RULE_CODE_WIN+RULE_CODE_WIN


#DEFINE FILENOTFOUND1_LOC "File '"
#DEFINE FILENOTFOUND2_LOC "' not found."
#DEFINE ALTERFILES_LOC	'Transformer may permanently alter files. Backup of files to be processed is recommended.  Continue?'
#DEFINE ABORT1_LOC		'Process aborted due to error.'
#DEFINE ABORT2_LOC		'Process aborted manually.'
#DEFINE NOFILES_LOC		'No files'
#DEFINE HAS1FILE_LOC	'1 File'
#DEFINE HASMANYFILE_LOC	' files'
#DEFINE SCANNED_LOC		'scanned'
#DEFINE PROCESSED_LOC	'processed'
#DEFINE SECONDS_LOC		' seconds.'
#DEFINE NOFONTRULE_LOC	'No font rules'
#DEFINE FONTRULE1_LOC	' font rule'
#DEFINE FONTRULE2_LOC	' added.'
#DEFINE FONTRULE3_LOC	'Transformer Add Font Rules'
#DEFINE SCANNING_LOC	'Scanning ...'
#DEFINE SCANNING2_LOC	[Scanning: ]
#DEFINE NOFILESTOPROCESS_LOC	'There are no files to process.'
#DEFINE LOGSTART_LOC	'Transformer Log Start'
#DEFINE LOGEND_LOC	'Transformer Log End'
#DEFINE NOPROCESS_LOC	'Unabled to process file:  '
#DEFINE PROJECT_LOC		'Project:  '
#DEFINE CLASSLIB_LOC	'Class Library:  '
#DEFINE FORM_LOC	'Form:  '
#DEFINE MENU_LOC	'Menu:  '
#DEFINE REPORT_LOC	'Report:  '
