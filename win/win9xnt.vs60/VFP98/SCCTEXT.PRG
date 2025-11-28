*+--------------------------------------------------------------------------
*
*	File:		SCCTEXT.PRG
*
*	Copyright:	(c) 1995, Microsoft Corporation.
*				All Rights Reserved.
*
*	Contents:	Routines for creating text representations of .SCX, .VCX,
*				.MNX, .FRX, and .LBX files for the purpose of supporting
*				merge capabilities in source control systems.
*
*   Author:		Sherri Kennamer
*
*	Parameters:	cTableName	C	Fully-qualified name of the SCX/VCX/MNX/FRX/LBX
*				cType		C	Code indicating the file type
*								(See PRJTYPE_ constants, defined below)
*				cTextName	C	Fully-qualified name of the text file
*				lGenText	L	.T. Create a text file from the table
*								.F. Create a table from the text file
*
*	Returns:	0		File or table was successfully generated
*				-1		An error occurred
*
*	History:	17-Aug-95	sherrike	written
*				20-Nov-95	sherrike	use smart defaults for single filename
*				02-Dec-95	sherrike	return values for merge support
*
*---------------------------------------------------------------------------

#include "foxpro.h"

#define C_DEBUG .F.

* If merge support is 1 and C_WRITECHECKSUMS is .T., write a checksum (sys(2007)) instead of
* converting binary to ascii. This drastically improves performance because OLE controls can
* be large and time-consuming to convert.
#define C_WRITECHECKSUMS .T.

#define SCCTEXTVER_LOC "SCCTEXT Version 4.0.0.2"

#define ALERTTITLE_LOC "Microsoft Visual FoxPro"
#define ERRORTITLE_LOC "Program Error"
#define ERRORMESSAGE_LOC ;
	"Error #" + alltrim(str(m.nError)) + " in " + m.cMethod + ;
	" (" + alltrim(str(m.nLine)) + "): " + m.cMessage

#define ERR_FOXERROR_11_LOC "Function argument value, type, or count is invalid."
#define ERR_NOTABLE_LOC "A table name is required."
#define ERR_FILENOTFOUND_LOC "File not found: "
#define ERR_UNSUPPORTEDFILETYPE_LOC "File type not supported: "
#define ERR_BIN2TEXTNOTSUPPORTED_LOC "Text file generation not supported for type '&cType' files."
#define ERR_TEXT2BINNOTSUPPORTED_LOC "Binary file generation not supported for type '&cType' files."
#define ERR_UNSUPPORTEDFIELDTYPE_LOC "Field type not supported: "
#define ERR_INVALIDTEXTNAME_LOC "Invalid TEXTNAME parameter."
#define ERR_INVALIDREVERSE_LOC "Invalid REVERSE parameter."
#define ERR_NOTEXTFILE_LOC "Text file name is required to create a table."
#define ERR_FCREATE_LOC "FCREATE() error: "
#define ERR_FOPEN_LOC "FOPEN() error: "
#define ERR_FIELDLISTTOOLONG_LOC "Field list is too long."
#define ERR_BADVERSION_LOC "Bad SCCTEXT version."
#define ERR_LINENOACTION_LOC "No action was taken on line: "
#define ERR_ALERTCONTINUE_LOC "Continue?"
#define ERR_OVERWRITEREADONLY_LOC "File &cParameter1 is read-only. Overwrite it?"
#define ERR_MAXBINLEN_LOC "MAXBINLEN value must be a multiple of 8. Program aborted."

#define CRLF chr(13) + chr(10)
#define MAXBINLEN	96		&& this value must be a multiple of 8!!!

#define FILE_ATTRIBUTE_NORMAL	128

* Text file support for each file type
*	0 indicates no text file support
*	1 indicates one-way support (to text)
*	2 indicates two-way support (for merging)
#define SCC_FORM_SUPPORT	1
#define SCC_LABEL_SUPPORT	1
#define SCC_MENU_SUPPORT	1
#define SCC_REPORT_SUPPORT	1
#define SCC_VCX_SUPPORT		1
#define SCC_DBC_SUPPORT		0

* These are the extensions used for the text file
#define SCC_ASCII_FORM_EXT		"SCA"
#define SCC_ASCII_LABEL_EXT		"LBA"
#define SCC_ASCII_MENU_EXT		"MNA"
#define SCC_ASCII_REPORT_EXT	"FRA"
#define SCC_ASCII_VCX_EXT		"VCA"
#define SCC_ASCII_DBC_EXT		"DBA"

* These are the extensions used for the binary file
#define SCC_FORM_EXT		"SCX"
#define SCC_LABEL_EXT		"LBX"
#define SCC_MENU_EXT		"MNX"
#define SCC_REPORT_EXT		"FRX"
#define SCC_VCX_EXT			"VCX"
#define SCC_DBC_EXT			"DBC"

* These are the extensions used for the binary file
#define SCC_FORM_MEMO		"SCT"
#define SCC_LABEL_MEMO		"LBT"
#define SCC_MENU_MEMO		"MNT"
#define SCC_REPORT_MEMO		"FRT"
#define SCC_VCX_MEMO		"VCT"
#define SCC_DBC_MEMO		"DBT"

* These are the project type identifiers for the files
#define PRJTYPE_FORM		"K"
#define PRJTYPE_LABEL		"B"
#define PRJTYPE_MENU		"M"
#define PRJTYPE_REPORT		"R"
#define PRJTYPE_VCX			"V"
#define PRJTYPE_DBC			"d"

* These are the extensions used for table backups
#define SCC_FORM_TABLE_BAK		"SC1"
#define SCC_FORM_MEMO_BAK		"SC2"
#define SCC_LABEL_TABLE_BAK		"LB1"
#define SCC_LABEL_MEMO_BAK		"LB2"
#define SCC_MENU_TABLE_BAK		"MN1"
#define SCC_MENU_MEMO_BAK		"MN2"
#define SCC_REPORT_TABLE_BAK	"FR1"
#define SCC_REPORT_MEMO_BAK		"FR2"
#define SCC_VCX_TABLE_BAK		"VC1"
#define SCC_VCX_MEMO_BAK		"VC2"
#define SCC_DBC_TABLE_BAK		"DB1"
#define SCC_DBC_MEMO_BAK		"DB2"
#define SCC_DBC_INDEX_BAK		"DB3"

* These are the extensions used for text file backups
#define SCC_FORM_TEXT_BAK		"SCB"
#define SCC_LABEL_TEXT_BAK		"LBB"
#define SCC_MENU_TEXT_BAK		"MNB"
#define SCC_REPORT_TEXT_BAK		"FRB"
#define SCC_VCX_TEXT_BAK		"VCB"
#define SCC_DBC_TEXT_BAK		"DBB"

* These are used for building markers used to parse the text back into a table
#define MARKMEMOSTARTWORD	"[START "
#define MARKMEMOSTARTWORD2	"]"
#define MARKMEMOENDWORD		"[END "
#define MARKMEMOENDWORD2	"]"
#define MARKBINSTARTWORD	"[BINSTART "
#define MARKBINSTARTWORD2	"]"
#define MARKBINENDWORD		"[BINEND "
#define MARKBINENDWORD2		"]"
#define MARKFIELDSTART		"["
#define MARKFIELDEND		"] "
#define MARKEOF				"[EOF]"
#define MARKRECORDSTART		"["
#define MARKRECORDEND		" RECORD]"
#define MARKCHECKSUM		"CHECKSUM="

#define SKIPEMPTYFIELD		.T.

* These are used to override default behavior for specific fields
#define VCX_EXCLUDE_LIST		" OBJCODE TIMESTAMP "
#define VCX_MEMOASCHAR_LIST		" CLASS CLASSLOC BASECLASS OBJNAME PARENT "
#define VCX_MEMOASBIN_LIST		" OLE OLE2 "
#define VCX_CHARASBIN_LIST		""
#define VCX_MEMOVARIES_LIST		" RESERVED4 RESERVED5 "

#define FRX_EXCLUDE_LIST		" TIMESTAMP "
#define FRX_MEMOASCHAR_LIST		" NAME STYLE PICTURE ORDER FONTFACE "
#define FRX_MEMOASBIN_LIST		" TAG TAG2 "
#define FRX_CHARASBIN_LIST		""
#define FRX_MEMOVARIES_LIST		""

#define MNX_EXCLUDE_LIST		" TIMESTAMP "
#define MNX_MEMOASCHAR_LIST		" NAME PROMPT COMMAND MESSAGE KEYNAME KEYLABEL "
#define MNX_MEMOASBIN_LIST		""
#define MNX_CHARASBIN_LIST		" MARK "
#define MNX_MEMOVARIES_LIST		""

#define DBC_EXCLUDE_LIST		""
#define DBC_MEMOASCHAR_LIST		""
#define DBC_MEMOASBIN_LIST		""
#define DBC_CHARASBIN_LIST		""
#define DBC_MEMOVARIES_LIST		" PROPERTY CODE USER "

* Used by the thermometer
#define C_THERMLABEL_LOC		"Generating &cThermLabel"
#define C_THERMCOMPLETE_LOC		"Generate &cThermLabel complete!"
#DEFINE WIN32FONT				"MS Sans Serif"
#DEFINE WIN95FONT				"Arial"
#define C_BINARYCONVERSION_LOC	"Converting binary data: &cBinaryProgress.%"

parameters cTableName, cType, cTextName, lGenText
LOCAL iParmCount
iParmCount = parameters()

LOCAL  obj, iResult
m.iResult = -1
if m.iParmCount = 1 .and. type('m.cTableName') = 'C'
	* Check to see if we've been passed only a PRJTYPE value. If so, return a
	* value to indicate text support for the file type.
	*	0 indicates no text file support
	*	1 indicates one-way support (to text)
	*	2 indicates two-way support (for merging)
	*  -1 indicates m.cTableName is not a recognized file type
	m.iResult = TextSupport(m.cTableName)
endif
if m.iResult = -1 && .and. file(m.cTableName)
	m.obj = createobj("SccTextEngine", m.cTableName, m.cType, m.cTextName, m.lGenText, m.iParmCount)
	if type("m.obj") = "O" .and. .not. isnull(m.obj)
		obj.Process()
		if type("m.obj") = "O" .and. .not. isnull(m.obj)
			m.iResult = obj.iResult
		endif
	endif
	release m.obj
endif
return (m.iResult)

procedure TextSupport
	parameters cFileType
	do case
	* Check to see if we've been passed only a PRJTYPE value. If so, return a
	* value to indicate text support for the file type.
	*	0 indicates no text file support
	*	1 indicates one-way support (to text)
	*	2 indicates two-way support (for merging)
	case m.cFileType == PRJTYPE_FORM
		return SCC_FORM_SUPPORT
	case m.cFileType == PRJTYPE_LABEL
		return SCC_LABEL_SUPPORT
	case m.cFileType == PRJTYPE_MENU
		return SCC_MENU_SUPPORT
	case m.cFileType == PRJTYPE_REPORT
		return SCC_REPORT_SUPPORT
	case m.cFileType == PRJTYPE_VCX
		return SCC_VCX_SUPPORT
	case m.cFileType == PRJTYPE_DBC
		return SCC_DBC_SUPPORT
	otherwise
		return -1
	endcase
endproc

define class SccTextEngine as custom
	HadError = .f.
	iError = 0
	cMessage = ""
	SetErrorOff = .f.

	iResult = -1 && Fail
	cTableName = ""
	cMemoName = ""
	cIndexName = ""
	cTextName = ""
	
	lMadeBackup = .F.
	cTableBakName = ""
	cMemoBakName = ""
	cIndexBakName = ""
	cTextBakName = ""
	
	cVCXCursor = ""		&& If we're generating text for a .VCX, we create a temporary
						&& file with the classes sorted.
	
	cType = ""
	lGenText = .t.
	iHandle = -1
	dimension aEnvironment[1]
	
	oThermRef = ""
	
	procedure Init(cTableName, cType, cTextName, lGenText, iParmCount)
		local iAction
		
		if m.iParmCount = 1 .and. type('m.cTableName') = 'C'
			* Interpret the single parameter as a filename and be smart about defaults
			if this.IsBinary(m.cTableName)
				m.cType = this.GetPrjType(m.cTableName)
				m.cTextName = this.ForceExt(m.cTableName, this.GetAsciiExt(m.cType))
				m.lGenText = .t.
			else
				if this.IsAscii(m.cTableName)
					m.cType = this.GetPrjType(m.cTableName)
					m.cTextName = m.cTableName
					m.cTableName = this.ForceExt(m.cTextName, this.GetBinaryExt(m.cType))
					m.lGenText = .f.
				endif
			endif
		endif
		
		this.cTableName = m.cTableName
		this.cType = m.cType
		this.cTextName = m.cTextName
		this.lGenText = m.lGenText
		
		* Verify that we've got valid parameters
		if type('this.cTableName') <> 'C' .or. type('this.cType') <> 'C' ;
			.or. type('this.cTextName') <> 'C' .or. type('this.lGenText') <> 'L'
			this.Alert(ERR_FOXERROR_11_LOC)
			return .f.
		endif
		
		* REC00XYS Verify parameters before calling this.ForceExt
		this.cMemoName = this.ForceExt(this.cTableName, this.GetBinaryMemo(this.cType))

		* Verify that we support the requested action
		m.iAction = iif(m.lGenText, 1, 2)
		do case
		case m.cType == PRJTYPE_FORM .and. SCC_FORM_SUPPORT < m.iAction
			m.iAction = m.iAction * -1
		case m.cType == PRJTYPE_LABEL .and. SCC_LABEL_SUPPORT < m.iAction
			m.iAction = m.iAction * -1
		case m.cType == PRJTYPE_MENU .and. SCC_MENU_SUPPORT < m.iAction
			m.iAction = m.iAction * -1
		case m.cType == PRJTYPE_REPORT .and. SCC_REPORT_SUPPORT < m.iAction
			m.iAction = m.iAction * -1
		case m.cType == PRJTYPE_VCX .and. SCC_VCX_SUPPORT < m.iAction
			m.iAction = m.iAction * -1
		case m.cType == PRJTYPE_DBC .and. SCC_DBC_SUPPORT < m.iAction
			m.iAction = m.iAction * -1
		endcase

		if m.iAction = -1
			this.Alert(ERR_BIN2TEXTNOTSUPPORTED_LOC)
			return .f.
		endif
		if m.iAction = -2
			this.Alert(ERR_TEXT2BINNOTSUPPORTED_LOC)
			return .f.
		endif
			
		if .not. this.Setup()
			return .f.
		endif
		
		if (MAXBINLEN % 8 <> 0)
			this.Alert(ERR_MAXBINLEN_LOC)
			return .f.
		endif
	endproc

	procedure Erase
		parameters cFilename
		if !empty(m.cFilename) .and. file(m.cFilename)
			=SetFileAttributes(m.cFilename, FILE_ATTRIBUTE_NORMAL)
			erase (m.cFilename)
		endif
	endproc
	
	procedure MakeBackup
		* Fill in the names of the backup files
		do case
		case this.cType = PRJTYPE_FORM
			this.cTextBakName = this.ForceExt(this.cTextName, SCC_FORM_TEXT_BAK)
			this.cTableBakName = this.ForceExt(this.cTableName, SCC_FORM_TABLE_BAK)
			this.cMemoBakName = this.ForceExt(this.cMemoName, SCC_FORM_MEMO_BAK)
		case this.cType = PRJTYPE_REPORT
			this.cTextBakName = this.ForceExt(this.cTextName, SCC_REPORT_TEXT_BAK)
			this.cTableBakName = this.ForceExt(this.cTableName, SCC_REPORT_TABLE_BAK)
			this.cMemoBakName = this.ForceExt(this.cMemoName, SCC_REPORT_MEMO_BAK)
		case this.cType = PRJTYPE_VCX
			this.cTextBakName = this.ForceExt(this.cTextName, SCC_VCX_TEXT_BAK)
			this.cTableBakName = this.ForceExt(this.cTableName, SCC_VCX_TABLE_BAK)
			this.cMemoBakName = this.ForceExt(this.cMemoName, SCC_VCX_MEMO_BAK)
		case this.cType = PRJTYPE_MENU
			this.cTextBakName = this.ForceExt(this.cTextName, SCC_MENU_TEXT_BAK)
			this.cTableBakName = this.ForceExt(this.cTableName, SCC_MENU_TABLE_BAK)
			this.cMemoBakName = this.ForceExt(this.cMemoName, SCC_MENU_MEMO_BAK)
		case this.cType = PRJTYPE_LABEL
			this.cTextBakName = this.ForceExt(this.cTextName, SCC_LABEL_TEXT_BAK)
			this.cTableBakName = this.ForceExt(this.cTableName, SCC_LABEL_TABLE_BAK)
			this.cMemoBakName = this.ForceExt(this.cMemoName, SCC_LABEL_MEMO_BAK)
		case this.cType = PRJTYPE_DBC
			this.cTextBakName = this.ForceExt(this.cTextName, SCC_DBC_TEXT_BAK)
			this.cTableBakName = this.ForceExt(this.cTableName, SCC_DBC_TABLE_BAK)
			this.cMemoBakName = this.ForceExt(this.cMemoName, SCC_DBC_MEMO_BAK)
			this.cIndexBakName = this.ForceExt(this.cIndexName, SCC_DBC_INDEX_BAK)
		endcase
		
		* Delete any existing backup
		this.DeleteBackup()
		
		* Create new backup files
		if this.lGenText
			if file(this.cTextName)
				copy file (this.cTextName) to (this.cTextBakName)
			endif
		else
			if file(this.cTableName) .and. file(this.cMemoName)
				copy file (this.cTableName) to (this.cTableBakName)
				copy file (this.cMemoName) to (this.cMemoBakName)
				if !empty(this.cIndexName) .and. file(this.cIndexName)
					copy file (this.cIndexName) to (this.cIndexBakName)
				endif
			endif
		endif

		this.lMadeBackup = .T.
	endproc
	
	procedure RestoreBackup
		if this.lGenText
			this.Erase(this.cTextName)
		else
			this.Erase(this.cTableName)
			this.Erase(this.cMemoName)
			if .not. empty(this.cIndexName)
				this.Erase(this.cIndexName)
			endif
		endif
		
		if this.lGenText
			if file(this.cTextBakName)
				copy file (this.cTextBakName) to (this.cTextName)
			endif
		else
			if file(this.cTableBakName) .and. file(this.cMemoBakName)
				copy file (this.cTableBakName) to (this.cTableName)
				copy file (this.cMemoBakName) to (this.cMemoName)
				if !empty(this.cIndexBakName) .and. file(this.cIndexBakName)
					copy file (this.cIndexBakName) to (this.cIndexName)
				endif
			endif
		endif
	endproc
	
	procedure DeleteBackup
		if this.lGenText
			this.Erase(this.cTextBakName)
		else
			this.Erase(this.cTableBakName)
			this.Erase(this.cMemoBakName)
			if !empty(this.cIndexBakName)
				this.Erase(this.cIndexBakName)
			endif
		endif
	endproc
	
	procedure GetAsciiExt
		parameters cType
		do case
		case m.cType = PRJTYPE_FORM
			return SCC_ASCII_FORM_EXT
		case m.cType = PRJTYPE_REPORT
			return SCC_ASCII_REPORT_EXT
		case m.cType = PRJTYPE_VCX
			return SCC_ASCII_VCX_EXT
		case m.cType = PRJTYPE_MENU
			return SCC_ASCII_MENU_EXT
		case m.cType = PRJTYPE_LABEL
			return SCC_ASCII_LABEL_EXT
		case m.cType = PRJTYPE_DBC
			return SCC_ASCII_DBC_EXT
		endcase
	endproc
	
	procedure GetBinaryExt
		parameters cType
		do case
		case m.cType = PRJTYPE_FORM
			return SCC_FORM_EXT
		case m.cType = PRJTYPE_REPORT
			return SCC_REPORT_EXT
		case m.cType = PRJTYPE_VCX
			return SCC_VCX_EXT
		case m.cType = PRJTYPE_MENU
			return SCC_MENU_EXT
		case m.cType = PRJTYPE_LABEL
			return SCC_LABEL_EXT
		case m.cType = PRJTYPE_DBC
			return SCC_DBC_EXT
		endcase
	endproc
	
	procedure GetBinaryMemo
		parameters cType
		do case
		case m.cType = PRJTYPE_FORM
			return SCC_FORM_MEMO
		case m.cType = PRJTYPE_REPORT
			return SCC_REPORT_MEMO
		case m.cType = PRJTYPE_VCX
			return SCC_VCX_MEMO
		case m.cType = PRJTYPE_MENU
			return SCC_MENU_MEMO
		case m.cType = PRJTYPE_LABEL
			return SCC_LABEL_MEMO
		case m.cType = PRJTYPE_DBC
			return SCC_DBC_MEMO
		endcase
	endproc
	
	procedure GetPrjType
		parameters cFileName
		local m.cExt
		m.cExt = upper(this.JustExt(m.cFileName))
		do case
		case inlist(m.cExt, SCC_ASCII_FORM_EXT, SCC_FORM_EXT)
			return PRJTYPE_FORM
		case inlist(m.cExt, SCC_ASCII_REPORT_EXT, SCC_REPORT_EXT)
			return PRJTYPE_REPORT
		case inlist(m.cExt, SCC_ASCII_VCX_EXT, SCC_VCX_EXT)
			return PRJTYPE_VCX
		case inlist(m.cExt, SCC_ASCII_MENU_EXT, SCC_MENU_EXT)
			return PRJTYPE_MENU
		case inlist(m.cExt, SCC_ASCII_LABEL_EXT, SCC_LABEL_EXT)
			return PRJTYPE_LABEL
		case inlist(m.cExt, SCC_ASCII_DBC_EXT, SCC_DBC_EXT)
			return PRJTYPE_DBC
		otherwise
			return ''
		endcase
	endproc
	
	procedure IsAscii
		parameters cFileName
		local m.cExt
		m.cExt = upper(this.JustExt(m.cFileName))
		return inlist(m.cExt, SCC_ASCII_FORM_EXT, SCC_ASCII_REPORT_EXT, SCC_ASCII_VCX_EXT, ;
			SCC_ASCII_MENU_EXT, SCC_ASCII_LABEL_EXT, SCC_ASCII_DBC_EXT)
	endproc
	
	procedure IsBinary
		parameters cFileName
		local m.cExt
		m.cExt = upper(this.JustExt(m.cFileName))
		return inlist(m.cExt, SCC_FORM_EXT, SCC_REPORT_EXT, SCC_VCX_EXT, ;
			SCC_MENU_EXT, SCC_LABEL_EXT, SCC_DBC_EXT)
	endproc
	
	procedure Setup
		
		dimension this.aEnvironment[5]
		
		this.aEnvironment[1] = set("deleted")
		this.aEnvironment[2] = select()
		this.aEnvironment[3] = set("safety")
		this.aEnvironment[4] = set("talk")
		this.aEnvironment[5] = set("asserts")
		
		SET TALK OFF

		declare INTEGER SetFileAttributes in win32api ;
			STRING lpFileName, INTEGER dwFileAttributes
		declare INTEGER sprintf in msvcrt40.dll ;
			STRING @lpBuffer, string lpFormat, integer iChar1, integer iChar2, ;
			integer iChar3, integer iChar4, integer iChar5, integer iChar6, ;
			integer iChar7, integer iChar8

		set safety off
		set deleted off
		select 0
		if C_DEBUG
			set asserts on
		endif
		
	endproc
	
	procedure Cleanup
		local array aEnvironment[alen(this.aEnvironment)]
		=acopy(this.aEnvironment, aEnvironment)
		set deleted &aEnvironment[1]
		set safety &aEnvironment[3]
		use
		select (aEnvironment[2])
		if this.iHandle <> -1
			=fclose(this.iHandle) 
			this.iHandle = -1
		endif
		SET TALK &aEnvironment[4]		
		if used(this.cVCXCursor)
			use in (this.cVCXCursor)
			this.cVCXCursor = ""
		endif
		set asserts &aEnvironment[5]
	endproc
	
	procedure Destroy
		if type("this.oThermRef") = "O"
			this.oThermRef.Release()
		endif
	
		this.Cleanup
		
		if this.lMadeBackup
			if this.iResult <> 0
				this.RestoreBackup()
			endif
			this.DeleteBackup()
		endif
	endproc
	
	PROCEDURE Error
		Parameters nError, cMethod, nLine, oObject, cMessage

		local cAction
		
		THIS.HadError = .T.
		this.iError = m.nError
		this.cMessage = iif(empty(m.cMessage), message(), m.cMessage)
	
		if this.SetErrorOff
			RETURN
		endif
		
		m.cMessage = iif(empty(m.cMessage), message(), m.cMessage)
		if type("m.oObject") = "O" .and. .not. isnull(m.oObject) .and. at(".", m.cMethod) = 0
			m.cMethod = m.oObject.Name + "." + m.cMethod
		endif
				
		if C_DEBUG
			m.cAction = this.Alert(ERRORMESSAGE_LOC, MB_ICONEXCLAMATION + ;
				MB_ABORTRETRYIGNORE, ERRORTITLE_LOC)
			do case
			case m.cAction="RETRY"
				this.HadError = .f.
				clear typeahead
				set step on
				&cAction
			case m.cAction="IGNORE"
				this.HadError = .f.
				return
			endcase
		else
			if m.nError = 1098
				* User-defined error
				m.cAction = this.Alert(message(), MB_ICONEXCLAMATION + ;
					MB_OK, ERRORTITLE_LOC)
			else
				m.cAction = this.Alert(ERRORMESSAGE_LOC, MB_ICONEXCLAMATION + ;
					MB_OK, ERRORTITLE_LOC)
			endif
		endif
		this.Cancel

	ENDPROC
	
	procedure Cancel
		parameters cMessage
		if !empty(m.cMessage)
			m.cAction = this.Alert(m.cMessage)
		endif
		return to Process -1
	endproc
	
	PROCEDURE Alert
		parameters cMessage, cOptions, cTitle, cParameter1, cParameter2

		private cOptions, cResponse

		m.cOptions = iif(empty(m.cOptions), 0, m.cOptions)

		if parameters() > 3 && a parameter was passed
			m.cMessage = [&cMessage]
		endif
		
		clear typeahead
		if !empty(m.cTitle)
			m.cResponse = MessageBox(m.cMessage, m.cOptions, m.cTitle)
		else
			m.cResponse = MessageBox(m.cMessage, m.cOptions, ALERTTITLE_LOC)
		endif

		do case
		* The strings below are used internally and should not 
		* be localized
		case m.cResponse = 1
			m.cResponse = "OK"
		case m.cResponse = 6
			m.cResponse = "YES"
		case m.cResponse = 7
			m.cResponse = "NO"
		case m.cResponse = 2
			m.cResponse = "CANCEL"
		case m.cResponse = 3
			m.cResponse = "ABORT"
		case m.cResponse = 4
			m.cResponse = "RETRY"
		case m.cResponse = 5
			m.cResponse = "IGNORE"
		endcase
		return m.cResponse

	ENDPROC

	procedure Process
		local cThermLabel
		
		if this.FilesAreWritable()
			* Backup the file(s)

			this.MakeBackup()
			
			* Create and show the thermometer
			m.cThermLabel = iif(this.lGenText, this.cTextName, this.cTableName)
			this.oThermRef = createobject("thermometer", C_THERMLABEL_LOC)
			this.oThermRef.Show()
			
			if this.lGenText
				this.iResult = this.WriteTextFile()
			else
				this.iResult = this.WriteTableFile()
			endif
			
			if this.iResult = 0
				this.oThermRef.Complete(C_THERMCOMPLETE_LOC)
			endif
		endif
	endproc
	
	procedure FilesAreWritable
		private aText
		if this.lGenText
			* Verify we can write the text file
			if (adir(aText, this.cTextName) = 1 .and. 'R' $ aText[1, 5])
				if this.Alert(ERR_OVERWRITEREADONLY_LOC, MB_YESNO, '', this.cTextName) = "NO"
					return .f.
				endif
			endif
			=SetFileAttributes(this.cTextName, FILE_ATTRIBUTE_NORMAL)
		else
			* Verify we can write the table
			if (adir(aText, this.cTableName) = 1 .and. 'R' $ aText[1, 5])
				if this.Alert(ERR_OVERWRITEREADONLY_LOC, MB_YESNO, '', this.cTableName) = "NO"
					return .f.
				endif
			else
				if (adir(aText, this.cMemoName) = 1 .and. 'R' $ aText[1, 5])
					if this.Alert(ERR_OVERWRITEREADONLY_LOC, MB_YESNO, '', this.cMemoName) = "NO"
						return .f.
					endif
				endif
			endif
			=SetFileAttributes(this.cTableName, FILE_ATTRIBUTE_NORMAL)
			=SetFileAttributes(this.cMemoName, FILE_ATTRIBUTE_NORMAL)
		endif
		return .t.
	endproc
	
	procedure WriteTableFile
		this.iHandle = fopen(this.cTextName)
		if this.iHandle = -1
			this.Alert(ERR_FOPEN_LOC + this.cTextName)
			return -1
		endif

		this.oThermRef.iBasis = fseek(this.iHandle, 0, 2)
		fseek(this.iHandle, 0, 0)
		
		this.ValidVersion(fgets(this.iHandle, 65000))
		this.CreateTable(fgets(this.iHandle, 65000), val(fgets(this.iHandle, 65000)))
		do case
			case inlist(this.cType, PRJTYPE_FORM, PRJTYPE_VCX, PRJTYPE_MENU, ;
				PRJTYPE_REPORT, PRJTYPE_LABEL)
				this.WriteTable
			otherwise
				this.Cancel(ERR_UNSUPPORTEDFILETYPE_LOC + this.cType)
		endcase
		
		=fclose(this.iHandle)
		this.iHandle = -1
		if inlist(this.cType, PRJTYPE_FORM, PRJTYPE_VCX)
			if this.cType = PRJTYPE_VCX
				* Additional work may need to be performed on a VCX
				this.FixUpVCX
			endif
			
			use
			compile form (this.cTableName)
		endif
		use
		return 0 && Success
	endproc
	
	procedure FixUpVCX
		private aClassList, i
		select objname, recno() from dbf() where not deleted() and reserved1 == 'Class' ;
			into array aClassList
		if type('aClassList[1]') <> 'U'
			* If objects were added to or removed from a class during merge, 
			* the record count will be out of sync.
			for m.i = 1 to alen(aClassList, 1)
				go (aClassList[m.i, 2])
				if m.i = alen(aClassList, 1)
					replace reserved2 with ;
						alltrim(str(reccount() - aClassList[m.i, 2]))
				else
					replace reserved2 with ;
						alltrim(str(aClassList[m.i + 1, 2] - aClassList[m.i, 2] - 1))
				endif
			endfor
		endif
	endproc
	
	procedure CreateTable
		parameters cFieldlist, iCodePage
		private c1, c2, c3, c4, c5, c6, aStruct

		do case
			* BugBug: This is a workaround for the problem with CREATE TABLE and a long
			* field list
			case inlist(this.cType, PRJTYPE_REPORT, PRJTYPE_LABEL)
				dimension aStruct[75, 4]
				this.GetReportStructure(@aStruct)
				create table (this.cTableName) free from array aStruct
				release aStruct
				if .not. m.cFieldlist == this.Fieldlist()
					this.Cancel(ERR_FIELDLISTTOOLONG_LOC)
				endif
			case len(m.cFieldlist) < 251
				create table (this.cTableName) free (&cFieldList)
			case len(m.cFieldlist) < 501
				m.c1 = substr(m.cFieldlist, 1, 250)
				m.c2 = substr(m.cFieldlist, 251)
				create table (this.cTableName) free (&c1&c2)
			case len(m.cFieldlist) < 751
				m.c1 = substr(m.cFieldlist, 1, 250)
				m.c2 = substr(m.cFieldlist, 251, 250)
				m.c3 = substr(m.cFieldlist, 501)
				create table (this.cTableName) free (&c1&c2&c3)
			case len(m.cFieldlist) < 1001
				m.c1 = substr(m.cFieldlist, 1, 250)
				m.c2 = substr(m.cFieldlist, 251, 250)
				m.c3 = substr(m.cFieldlist, 501, 250)
				m.c4 = substr(m.cFieldlist, 751)
				create table (this.cTableName) free (&c1&c2&c3&c4)
			case .f. .and. len(m.cFieldlist) < 1251
				m.c1 = substr(m.cFieldlist, 1, 250)
				m.c2 = substr(m.cFieldlist, 251, 250)
				m.c3 = substr(m.cFieldlist, 501, 250)
				m.c4 = substr(m.cFieldlist, 751, 250)
				m.c5 = substr(m.cFieldlist, 1001)
				* BugBug: This causes an error
				create table (this.cTableName) free (&c1&c2&c3&c4&c5)
			case .f. .and. len(m.cFieldlist) < 1501
				m.c1 = substr(m.cFieldlist, 1, 250)
				m.c2 = substr(m.cFieldlist, 251, 250)
				m.c3 = substr(m.cFieldlist, 501, 250)
				m.c4 = substr(m.cFieldlist, 751, 250)
				m.c5 = substr(m.cFieldlist, 1001, 250)
				m.c6 = substr(m.cFieldlist, 1251)
				* BugBug: This causes an error
				create table (this.cTableName) free (&c1&c2&c3&c4&c5&c6)
			otherwise
				* Not supported
				this.Cancel(ERR_FIELDLISTTOOLONG_LOC)
		endcase
		if cpdbf() <> m.iCodePage
			use
			this.SetCodePage(this.cTableName, m.iCodePage)
		endif
		use (this.cTableName) exclusive
	endproc
	
	procedure ValidVersion
		parameters cVersion
		if .not. m.cVersion == SCCTEXTVER_LOC
			this.Cancel(ERR_BADVERSION_LOC)
		endif
	endproc
	
	procedure FieldList
		* Returns a CREATE TABLE compatible field list for the current workarea.
		local cStruct, i
		local array aStruct[1]
		
		=afields(aStruct)
		m.cStruct = ""
		for m.i = 1 to alen(aStruct, 1)
			if .not. empty(m.cStruct)
				m.cStruct = m.cStruct + ","
			endif
			m.cStruct = m.cStruct + aStruct[m.i, 1] + " " + aStruct[m.i, 2] + ;
				"(" + alltrim(str(aStruct[m.i, 3])) + "," + ;
				alltrim(str(aStruct[m.i, 4])) + ")"
		endfor
		
		return m.cStruct
	endproc
	
	procedure CreateVcxCursor
		private iSelect, aClasslist, i, j, iCount, aRec, aStruct
		
		this.cVCXCursor = "_" + sys(3)
		do while used(this.cVCXCursor)
			this.cVCXCursor = "_" + sys(3)
		enddo
		
		* Get an ordered list of the classes in the vcx
		select padr(uniqueid, fsize('uniqueid')), recno() from dbf() ;
			where .not. deleted() .and. reserved1 == "Class" ;
			into array aClasslist order by 1

		m.iSelect = select() && The original .VCX

		* Create the temporary cursor
		=afields(aStruct)
		create cursor (this.cVCXCursor) from array aStruct
		
		* Copy the header record
		select (m.iSelect)
		go top
		scatter memo to aRec
		insert into (this.cVCXCursor) from array aRec
		
		* Scan through the class list and copy the classes over
		if type('aClassList[1]') <> 'U'
			for m.i = 1 to alen(aClasslist, 1)
				go (aClasslist[m.i, 2])
				m.iCount = 1 + val(reserved2)
				for m.j = 1 to m.iCount
					scatter memo to aRec
					insert into (this.cVCXCursor) from array aRec
					skip
				endfor
			endfor
		endif
		
		* Close the original file and use the cursor we've created
		use in (m.iSelect)
		
		select (this.cVCXCursor)
	endproc
	
	procedure WriteTextFile
		private iCodePage, aText
		
		use (this.cTableName) exclusive
		
		this.oThermRef.iBasis = reccount()

		m.iCodePage = cpdbf()
		
		if this.cType = PRJTYPE_VCX
			this.CreateVcxCursor
		endif

		this.iHandle = fcreate(this.cTextName)
		if this.iHandle = -1
			this.Alert(ERR_FCREATE_LOC + this.cTextName)
			return -1
		endif
		
		* First line contains the SCCTEXT version string
		=fputs(this.iHandle, SCCTEXTVER_LOC)

		* Second line contains the CREATE TABLE compatible field list
		=fputs(this.iHandle, this.FieldList())
		* Third line contains the code page
		=fputs(this.iHandle, alltrim(str(m.iCodePage)))
		
		do case
		case inlist(this.cType, PRJTYPE_FORM, PRJTYPE_VCX, PRJTYPE_LABEL, ;
			PRJTYPE_REPORT, PRJTYPE_MENU, PRJTYPE_DBC)
			this.WriteText
		otherwise
			this.Cancel(ERR_UNSUPPORTEDFILETYPE_LOC + m.cType)
		endcase

		=fclose(this.iHandle)
		this.iHandle = -1
		use
		return 0 && Success
	endproc

	procedure WriteTable
		private cLine, bInMemo, cMemo, cEndMark, bBinary, cFieldname, cValue, iSeconds
		m.cLine = ""
		m.bInMemo = .f.
		m.cMemo = ""
		m.cEndMark = ""
		m.bBinary = .f.
		m.cFieldname = ""
		m.cValue = ""
		
		this.oThermRef.Update(fseek(this.iHandle, 0, 1))
		m.iSeconds = seconds()
		
		do while .not. feof(this.iHandle)
			if (seconds() - m.iSeconds > 1)
				this.oThermRef.Update(fseek(this.iHandle, 0, 1))
				m.iSeconds = seconds()
			endif
			
			m.cLine = fgets(this.iHandle, 65000)
			
			if m.bInMemo
				do case
				case m.cEndMark == m.cLine
				case rat(m.cEndMark, m.cLine) <> 0
					if m.bBinary
						m.cMemo = m.cMemo + ;
							this.HexStr2BinStr(left(m.cLine, rat(m.cEndMark, m.cLine) - 1))
					else
						m.cMemo = m.cMemo + left(m.cLine, rat(m.cEndMark, m.cLine) - 1)
					endif
				otherwise
					if m.bBinary
						m.cMemo = m.cMemo + this.HexStr2BinStr(m.cLine)
					else
						m.cMemo = m.cMemo + m.cLine + CRLF
					endif
					loop				
				endcase
				
				* Drop out of if/endif to write the memo field
			else
				do case
				case empty(m.cLine)
					loop
				case m.cLine == MARKEOF
					* Don't read anything past the [EOF] mark
					return
				case m.bInMemo .and. m.cEndMark == m.cLine
				case this.IsRecordMark(m.cLine)
					append blank
					loop
				case this.IsMemoStartMark(m.cLine, @cFieldname)
					m.bInMemo = .t.
					m.bBinary = .f.
					m.cEndMark = this.SectionMark(m.cFieldname, .f., .f.)
					loop
				case this.IsBinStartMark(m.cLine, @cFieldname)
					m.bInMemo = .t.
					m.bBinary = .t.
					m.cEndMark = this.SectionMark(m.cFieldname, .f., .t.)
					loop
				case this.IsFieldMark(m.cLine, @cFieldname, @cValue)
					do case
					case inlist(type(m.cFieldname), "C", "M")
						replace (m.cFieldname) with m.cValue
					case type(m.cFieldname) = "N"
						replace (m.cFieldname) with val(m.cValue)
					case type(m.cFieldname) = "L"
						replace (m.cFieldname) with &cValue
					otherwise
						this.Cancel(ERR_UNSUPPORTEDFIELDTYPE_LOC + type(m.cFieldname))
					endcase
					loop
				otherwise
					if this.Alert(ERR_LINENOACTION_LOC + chr(13) + chr(13) + m.cLine + chr(13) + chr(13) + ;
						ERR_ALERTCONTINUE_LOC, MB_YESNO) = IDNO
						this.Cancel
					endif
				endcase
			endif
			
			* Write the memo field
			replace (m.cFieldname) with m.cMemo
			m.bInMemo = .f.
			m.cFieldname = ""
			m.cMemo = ""
			m.cEndMark = ""
		enddo
	endproc
	
	procedure IsMemoStartMark
		parameters cLine, cFieldname
		private cStartMark, cStartMark2
		if at(MARKMEMOSTARTWORD, m.cLine) = 1
			m.cFieldname = strtran(m.cLine, MARKMEMOSTARTWORD, "", 1, 1)
			m.cFieldname = left(m.cFieldname, rat(MARKMEMOSTARTWORD2, m.cFieldname) - 1)
			return .t.
		endif
		return .f.
	endproc

	procedure IsBinStartMark
		parameters cLine, cFieldname
		private cStartMark, cStartMark2
		if at(MARKBINSTARTWORD, m.cLine) = 1
			m.cFieldname = strtran(m.cLine, MARKBINSTARTWORD, "", 1, 1)
			m.cFieldname = left(m.cFieldname, rat(MARKBINSTARTWORD2, m.cFieldname) - 1)
			return .t.
		endif
		return .f.
	endproc
	
	procedure IsFieldMark
		parameters cLine, cFieldname, cValue
		if at(MARKFIELDSTART, m.cLine) = 1
			m.cFieldname = strtran(m.cLine, MARKFIELDSTART, "", 1, 1)
			m.cFieldname = left(m.cFieldname, at(MARKFIELDEND, m.cFieldname) - 1)
			m.cValue = substr(m.cLine, at(MARKFIELDEND, m.cLine))
			m.cValue = strtran(m.cValue, MARKFIELDEND, "", 1, 1)
			return .t.
		endif
		return .f.
	endproc
	
	procedure RecordMark
		parameters cUniqueId
		=fputs(this.iHandle, "")
		=fputs(this.iHandle, MARKRECORDSTART + MARKRECORDEND)
	endproc
	
	procedure IsRecordMark
		parameters cLine
		if left(m.cLine, len(MARKRECORDSTART)) == MARKRECORDSTART .and. ;
			right(m.cLine, len(MARKRECORDEND)) == MARKRECORDEND
			return .t.
		else
			return .f.
		endif
	endproc
	
	procedure WriteText
		private cExcludeList, cMemoAsCharList, cMemoAsBinList, cCharAsBinList
		m.cExcludeList = ""
		m.cMemoAsCharList = ""
		m.cMemoAsBinList = ""
		m.cCharAsBinList = ""
		m.cMemoVariesList = ""

		do case
			case inlist(this.cType, PRJTYPE_FORM, PRJTYPE_VCX)
				m.cExcludeFields = VCX_EXCLUDE_LIST
				m.cMemoAsCharList = VCX_MEMOASCHAR_LIST
				m.cMemoAsBinList = VCX_MEMOASBIN_LIST
				m.cCharAsBinList = VCX_CHARASBIN_LIST
				m.cMemoVariesList = VCX_MEMOVARIES_LIST
			case inlist(this.cType, PRJTYPE_REPORT, PRJTYPE_LABEL)
				m.cExcludeFields = FRX_EXCLUDE_LIST
				m.cMemoAsCharList = FRX_MEMOASCHAR_LIST
				m.cMemoAsBinList = FRX_MEMOASBIN_LIST
				m.cCharAsBinList = FRX_CHARASBIN_LIST
				m.cMemoVariesList = FRX_MEMOVARIES_LIST
			case this.cType = PRJTYPE_MENU
				m.cExcludeFields = MNX_EXCLUDE_LIST
				m.cMemoAsCharList = MNX_MEMOASCHAR_LIST
				m.cMemoAsBinList = MNX_MEMOASBIN_LIST
				m.cCharAsBinList = MNX_CHARASBIN_LIST
				m.cMemoVariesList = MNX_MEMOVARIES_LIST
			case this.cType = PRJTYPE_DBC
				m.cExcludeFields = DBC_EXCLUDE_LIST
				m.cMemoAsCharList = DBC_MEMOASCHAR_LIST
				m.cMemoAsBinList = DBC_MEMOASBIN_LIST
				m.cCharAsBinList = DBC_CHARASBIN_LIST
				m.cMemoVariesList = DBC_MEMOVARIES_LIST
			otherwise
				this.Cancel(ERR_UNSUPPORTEDFILETYPE_LOC + this.cType)
		endcase

		scan
			this.oThermRef.Update(recno())
			if type("UNIQUEID") <> 'U'
				this.RecordMark(UNIQUEID)
			endif
			for i = 1 to fcount()
				if SKIPEMPTYFIELD and empty(evaluate(field(i)))
					loop
				endif
				do case
					case " " + field(i) + " " $ m.cExcludeFields
						&& skip this field
					case " " + field(i) + " " $ m.cMemoAsCharList
						&& memo fields treated as CHAR
						this.CharWrite(field(i))
					case type(field(i)) = "C"
						if " " + field(i) + " " $ m.cCharAsBinList
							this.MemoWrite(field(i), .t.)
						else
							this.CharWrite(field(i))
						endif
					case type(field(i)) = "M"
						if " " + field(i) + " " $ m.cMemoVariesList
							&& treat as text or binary based on contents of the memofield
							if this.MemoIsBinary(field(i))
								this.MemoWrite(field(i), .t.)
							else
								this.MemoWrite(field(i), .f.)
							endif
						else
							if " " + field(i) + " " $ m.cMemoAsBinList
								&& memo fields treated as BINARY
								this.MemoWrite(field(i), .t.)
							else
								this.MemoWrite(field(i), .f.)
							endif
						endif
					case type(field(i)) = "N"
						this.NumWrite(field(i))
					case type(field(i)) = "L"
						this.BoolWrite(field(i))
					otherwise
						this.Alert(ERR_UNSUPPORTEDFIELDTYPE_LOC + type(field(i)))
				endcase
			endfor
		endscan
		this.EOFMark
	endproc
	
	procedure MemoIsBinary
		* Scan the memo field to see if it contains binary characters
		parameters cFieldname
		private i, bIsBinary, cMemo
		m.cMemo = &cFieldname
		m.bIsBinary = .t.
		do case
			case chr(0) $ m.cMemo
			otherwise
				m.bIsBinary = .f.
				if len(m.cMemo) < 126
					for m.i = 1 to len(m.cMemo)
						if asc(substr(m.cMemo, m.i, 1)) > 126
							m.bIsBinary = .t.
							exit
						endif
					endfor
				else
					for m.i = 126 to 255
						if chr(m.i) $ m.cMemo
							m.bIsBinary = .t.
							exit
						endif
					endfor
				endif
		endcase
		return m.bIsBinary
	endproc
	
	procedure EOFMark
		=fputs(this.iHandle, MARKEOF)
	endproc
	
	procedure CharWrite
		parameters cFieldname
		private cTempfield
		m.cTempfield = &cFieldname
		=fputs(this.iHandle, MARKFIELDSTART + m.cFieldname + MARKFIELDEND + m.cTempfield)
	endproc
	
	procedure MemoWrite
		parameters cFieldname, bBinary
		private i, iLen, iStart, cBuf, cBinary, cBinaryProgress, iSeconds
		=fputs(this.iHandle, this.SectionMark(m.cFieldname, .t., m.bBinary))
		m.iLen = len(&cFieldname)
		if m.bBinary
			* If we don't support merging, simply write the checksum
			if C_WRITECHECKSUMS .and. TextSupport(this.cType) == 1
				=fputs(this.iHandle, MARKCHECKSUM + sys(2007, &cFieldname))
			else
				m.cBuf = repl(chr(0), 17)
				
				m.cBinaryProgress = "0"
				this.oThermRef.UpdateTaskMessage(C_BINARYCONVERSION_LOC)
				m.iSeconds = seconds()
				
				for m.i = 1 to int(m.iLen / MAXBINLEN) + iif(m.iLen % MAXBINLEN = 0, 0, 1)
					if seconds() - m.iSeconds > 1
						m.cBinaryProgress = alltrim(str(int(((m.i * MAXBINLEN) / m.iLen) * 100)))
						this.oThermRef.UpdateTaskMessage(C_BINARYCONVERSION_LOC)
						m.iSeconds = seconds()
					endif
					m.cBinary = substr(&cFieldname, ((m.i - 1) * MAXBINLEN) + 1, MAXBINLEN)
					for m.j = 1 to int(len(m.cBinary) / 8)
						sprintf(@cBuf, "%02X%02X%02X%02X%02X%02X%02X%02X", ;
							asc(substr(m.cBinary, ((m.j - 1) * 8) + 1, 1)), ;
							asc(substr(m.cBinary, ((m.j - 1) * 8) + 2, 1)), ;
							asc(substr(m.cBinary, ((m.j - 1) * 8) + 3, 1)), ;
							asc(substr(m.cBinary, ((m.j - 1) * 8) + 4, 1)), ;
							asc(substr(m.cBinary, ((m.j - 1) * 8) + 5, 1)), ;
							asc(substr(m.cBinary, ((m.j - 1) * 8) + 6, 1)), ;
							asc(substr(m.cBinary, ((m.j - 1) * 8) + 7, 1)), ;
							asc(substr(m.cBinary, ((m.j - 1) * 8) + 8, 1)))
						fwrite(this.iHandle, m.cBuf, 16)
					endfor
					if len(m.cBinary) % 8 = 0
						fputs(this.iHandle, "")
					endif
				endfor
				
				if len(m.cBinary) % 8 <> 0
					m.cBinary = right(m.cBinary, len(m.cBinary) % 8)
					sprintf(@cBuf, replicate("%02X", len(m.cBinary)), ;
						asc(substr(m.cBinary, 1, 1)), ;
						asc(substr(m.cBinary, 2, 1)), ;
						asc(substr(m.cBinary, 3, 1)), ;
						asc(substr(m.cBinary, 4, 1)), ;
						asc(substr(m.cBinary, 5, 1)), ;
						asc(substr(m.cBinary, 6, 1)), ;
						asc(substr(m.cBinary, 7, 1)), ;
						asc(substr(m.cBinary, 8, 1)))
					fwrite(this.iHandle, m.cBuf, len(m.cBinary) * 2)
					fputs(this.iHandle, "")
				endif
				
				this.oThermRef.UpdateTaskMessage("")
			endif
		else
			=fwrite(this.iHandle, &cFieldname)
		endif
		=fputs(this.iHandle, this.SectionMark(m.cFieldname, .f., m.bBinary))
	endproc

	procedure HexStr2BinStr
		parameters cHexStr
		private cBinStr, i
		m.cBinStr = ""

		m.cHexStr = strtran(m.cHexStr, 'A', chr(asc('9') + 1))
		m.cHexStr = strtran(m.cHexStr, 'B', chr(asc('9') + 2))
		m.cHexStr = strtran(m.cHexStr, 'C', chr(asc('9') + 3))
		m.cHexStr = strtran(m.cHexStr, 'D', chr(asc('9') + 4))
		m.cHexStr = strtran(m.cHexStr, 'E', chr(asc('9') + 5))
		m.cHexStr = strtran(m.cHexStr, 'F', chr(asc('9') + 6))
		
		for m.i = 1 to len(m.cHexStr) step 2
			m.cBinStr = m.cBinStr + ;
				chr((asc(substr(m.cHexStr, m.i, 1)) - 48) * 16 + asc(substr(m.cHexStr, m.i + 1, 1)) - 48)
		endfor

		return m.cBinStr
	endproc
	
	procedure NumWrite
		* This procedure supports the numerics found in forms, reports, etc. (basically, integers)
		parameters cFieldname
		=fputs(this.iHandle, MARKFIELDSTART + m.cFieldname + ;
			MARKFIELDEND + alltrim(str(&cFieldname, 20)))
	endproc
	
	procedure BoolWrite
		parameters cFieldname
		=fputs(this.iHandle, MARKFIELDSTART + m.cFieldname + ;
			MARKFIELDEND + iif(&cFieldname, ".T.", ".F."))
	endproc
	
	procedure SectionMark
		parameters cFieldname, lStart, bBinary
		if m.lStart
			if m.bBinary
				return MARKBINSTARTWORD + m.cFieldname + MARKBINSTARTWORD2
			else
				return MARKMEMOSTARTWORD + m.cFieldname + MARKMEMOSTARTWORD2
			endif
		else
			if m.bBinary
				return MARKBINENDWORD + m.cFieldname + MARKBINENDWORD2
			else
				return MARKMEMOENDWORD + m.cFieldname + MARKMEMOENDWORD2
			endif
		endif
	endproc

	FUNCTION JustPath
		* Returns just the pathname.
		LPARAMETERS m.filname
		m.filname = ALLTRIM(UPPER(m.filname))
		IF "\" $ m.filname
		   m.filname = SUBSTR(m.filname,1,RAT("\",m.filname))
		   IF RIGHT(m.filname,1) = "\" AND LEN(m.filname) > 1 ;
		            AND SUBSTR(m.filname,LEN(m.filname)-1,1) <> ":"
		         filname = SUBSTR(m.filname,1,LEN(m.filname)-1)
		   ENDIF
		   RETURN m.filname
		ELSE
		   RETURN ""
		ENDIF
	ENDFUNC
	
	FUNCTION ForceExt
		* Force filename to have a particular extension.
		LPARAMETERS m.filname,m.ext
		LOCAL m.ext
		IF SUBSTR(m.ext,1,1) = "."
		   m.ext = SUBSTR(m.ext,2,3)
		ENDIF

		m.pname = THIS.justpath(m.filname)
		m.filname = THIS.justfname(UPPER(ALLTRIM(m.filname)))
		IF AT(".",m.filname) > 0
		   m.filname = SUBSTR(m.filname,1,AT(".",m.filname)-1) + "." + m.ext
		ELSE
		   m.filname = m.filname + "." + m.ext
		ENDIF
		RETURN THIS.addbs(m.pname) + m.filname
	ENDFUNC
	
	FUNCTION JustFname
		* Return just the filename (i.e., no path) from "filname"
		LPARAMETERS m.filname
		IF RAT("\",m.filname) > 0
		   m.filname = SUBSTR(m.filname,RAT("\",m.filname)+1,255)
		ENDIF
		IF AT(":",m.filname) > 0
		   m.filname = SUBSTR(m.filname,AT(":",m.filname)+1,255)
		ENDIF
		RETURN ALLTRIM(UPPER(m.filname))
	ENDFUNC

	FUNCTION AddBS
		* Add a backslash unless there is one already there.
		LPARAMETER m.pathname
		LOCAL m.separator
		m.separator = IIF(_MAC,":","\")
		m.pathname = ALLTRIM(UPPER(m.pathname))
		IF !(RIGHT(m.pathname,1) $ "\:") AND !EMPTY(m.pathname)
		   m.pathname = m.pathname + m.separator
		ENDIF
		RETURN m.pathname
	ENDFUNC

	FUNCTION JustStem
		* Return just the stem name from "filname"
		LPARAMETERS m.filname
		IF RAT("\",m.filname) > 0
		   m.filname = SUBSTR(m.filname,RAT("\",m.filname)+1,255)
		ENDIF
		IF RAT(":",m.filname) > 0
		   m.filname = SUBSTR(m.filname,RAT(":",m.filname)+1,255)
		ENDIF
		IF AT(".",m.filname) > 0
		   m.filname = SUBSTR(m.filname,1,AT(".",m.filname)-1)
		ENDIF
		RETURN ALLTRIM(UPPER(m.filname))
	ENDFUNC

	FUNCTION justext
		* Return just the extension from "filname"
		PARAMETERS m.filname
		LOCAL m.ext
		m.filname = this.justfname(m.filname)   && prevents problems with ..\ paths
		m.ext = ""
		IF AT(".", m.filname) > 0
		   m.ext = SUBSTR(m.filname, AT(".", m.filname) + 1, 3)
		ENDIF
		RETURN UPPER(m.ext)
	ENDFUNC	

	procedure SetCodePage
		parameters m.fname, m.iCodePage
		private iHandle, cpbyte

		do case
			case m.iCodePage = 437
				m.cpbyte = 1
			case m.iCodePage = 850
				m.cpbyte = 2
			case m.iCodePage = 1252
				m.cpbyte = 3
			case m.iCodePage = 10000
				m.cpbyte = 4
			case m.iCodePage = 852
				m.cpbyte = 100
			case m.iCodePage = 866
				m.cpbyte = 101
			case m.iCodePage = 865
				m.cpbyte = 102
			case m.iCodePage = 861
				m.cpbyte = 103
			case m.iCodePage = 895
				m.cpbyte = 104
			case m.iCodePage = 620
				m.cpbyte = 105
			case m.iCodePage = 737
				m.cpbyte = 106
			case m.iCodePage = 857
				m.cpbyte = 107
			case m.iCodePage = 863
				m.cpbyte = 108
			case m.iCodePage = 10007
				m.cpbyte = 150
			case m.iCodePage = 10029
				m.cpbyte = 151
			case m.iCodePage = 10006
				m.cpbyte = 152
			case m.iCodePage = 1250
				m.cpbyte = 200
			case m.iCodePage = 1251
				m.cpbyte = 201
			case m.iCodePage = 1253
				m.cpbyte = 203
			case m.iCodePage = 1254
				m.cpbyte = 202
			case m.iCodePage = 1257
				m.cpbyte = 204
			otherwise
				* Handle the error
				return .f.
		endcase
		
		m.iHandle = fopen(m.fname, 2)
		if m.iHandle = -1
			return .f.
		else
			=fseek(m.iHandle, 29)
			=fwrite(m.iHandle, chr(m.cpbyte))
			=fclose(m.iHandle)
		endif
		return .t.
	endproc
	
	procedure GetReportStructure
		parameters aStruct
		aStruct[1, 1] = "PLATFORM"
		aStruct[1, 2] = "C"
		aStruct[1, 3] = 8
		aStruct[1, 4] = 0
		aStruct[2, 1] = "UNIQUEID"
		aStruct[2, 2] = "C"
		aStruct[2, 3] = 10
		aStruct[2, 4] = 0
		aStruct[3, 1] = "TIMESTAMP"
		aStruct[3, 2] = "N"
		aStruct[3, 3] = 10
		aStruct[3, 4] = 0
		aStruct[4, 1] = "OBJTYPE"
		aStruct[4, 2] = "N"
		aStruct[4, 3] = 2
		aStruct[4, 4] = 0
		aStruct[5, 1] = "OBJCODE"
		aStruct[5, 2] = "N"
		aStruct[5, 3] = 3
		aStruct[5, 4] = 0
		aStruct[6, 1] = "NAME"
		aStruct[6, 2] = "M"
		aStruct[6, 3] = 4
		aStruct[6, 4] = 0
		aStruct[7, 1] = "EXPR"
		aStruct[7, 2] = "M"
		aStruct[7, 3] = 4
		aStruct[7, 4] = 0
		aStruct[8, 1] = "VPOS"
		aStruct[8, 2] = "N"
		aStruct[8, 3] = 9
		aStruct[8, 4] = 3
		aStruct[9, 1] = "HPOS"
		aStruct[9, 2] = "N"
		aStruct[9, 3] = 9
		aStruct[9, 4] = 3
		aStruct[10, 1] = "HEIGHT"
		aStruct[10, 2] = "N"
		aStruct[10, 3] = 9
		aStruct[10, 4] = 3
		aStruct[11, 1] = "WIDTH"
		aStruct[11, 2] = "N"
		aStruct[11, 3] = 9
		aStruct[11, 4] = 3
		aStruct[12, 1] = "STYLE"
		aStruct[12, 2] = "M"
		aStruct[12, 3] = 4
		aStruct[12, 4] = 0
		aStruct[13, 1] = "PICTURE"
		aStruct[13, 2] = "M"
		aStruct[13, 3] = 4
		aStruct[13, 4] = 0
		aStruct[14, 1] = "ORDER"
		aStruct[14, 2] = "M"
		aStruct[14, 3] = 4
		aStruct[14, 4] = 0
		aStruct[15, 1] = "UNIQUE"
		aStruct[15, 2] = "L"
		aStruct[15, 3] = 1
		aStruct[15, 4] = 0
		aStruct[16, 1] = "COMMENT"
		aStruct[16, 2] = "M"
		aStruct[16, 3] = 4
		aStruct[16, 4] = 0
		aStruct[17, 1] = "ENVIRON"
		aStruct[17, 2] = "L"
		aStruct[17, 3] = 1
		aStruct[17, 4] = 0
		aStruct[18, 1] = "BOXCHAR"
		aStruct[18, 2] = "C"
		aStruct[18, 3] = 1
		aStruct[18, 4] = 0
		aStruct[19, 1] = "FILLCHAR"
		aStruct[19, 2] = "C"
		aStruct[19, 3] = 1
		aStruct[19, 4] = 0
		aStruct[20, 1] = "TAG"
		aStruct[20, 2] = "M"
		aStruct[20, 3] = 4
		aStruct[20, 4] = 0
		aStruct[21, 1] = "TAG2"
		aStruct[21, 2] = "M"
		aStruct[21, 3] = 4
		aStruct[21, 4] = 0
		aStruct[22, 1] = "PENRED"
		aStruct[22, 2] = "N"
		aStruct[22, 3] = 5
		aStruct[22, 4] = 0
		aStruct[23, 1] = "PENGREEN"
		aStruct[23, 2] = "N"
		aStruct[23, 3] = 5
		aStruct[23, 4] = 0
		aStruct[24, 1] = "PENBLUE"
		aStruct[24, 2] = "N"
		aStruct[24, 3] = 5
		aStruct[24, 4] = 0
		aStruct[25, 1] = "FILLRED"
		aStruct[25, 2] = "N"
		aStruct[25, 3] = 5
		aStruct[25, 4] = 0
		aStruct[26, 1] = "FILLGREEN"
		aStruct[26, 2] = "N"
		aStruct[26, 3] = 5
		aStruct[26, 4] = 0
		aStruct[27, 1] = "FILLBLUE"
		aStruct[27, 2] = "N"
		aStruct[27, 3] = 5
		aStruct[27, 4] = 0
		aStruct[28, 1] = "PENSIZE"
		aStruct[28, 2] = "N"
		aStruct[28, 3] = 5
		aStruct[28, 4] = 0
		aStruct[29, 1] = "PENPAT"
		aStruct[29, 2] = "N"
		aStruct[29, 3] = 5
		aStruct[29, 4] = 0
		aStruct[30, 1] = "FILLPAT"
		aStruct[30, 2] = "N"
		aStruct[30, 3] = 5
		aStruct[30, 4] = 0
		aStruct[31, 1] = "FONTFACE"
		aStruct[31, 2] = "M"
		aStruct[31, 3] = 4
		aStruct[31, 4] = 0
		aStruct[32, 1] = "FONTSTYLE"
		aStruct[32, 2] = "N"
		aStruct[32, 3] = 3
		aStruct[32, 4] = 0
		aStruct[33, 1] = "FONTSIZE"
		aStruct[33, 2] = "N"
		aStruct[33, 3] = 3
		aStruct[33, 4] = 0
		aStruct[34, 1] = "MODE"
		aStruct[34, 2] = "N"
		aStruct[34, 3] = 3
		aStruct[34, 4] = 0
		aStruct[35, 1] = "RULER"
		aStruct[35, 2] = "N"
		aStruct[35, 3] = 1
		aStruct[35, 4] = 0
		aStruct[36, 1] = "RULERLINES"
		aStruct[36, 2] = "N"
		aStruct[36, 3] = 1
		aStruct[36, 4] = 0
		aStruct[37, 1] = "GRID"
		aStruct[37, 2] = "L"
		aStruct[37, 3] = 1
		aStruct[37, 4] = 0
		aStruct[38, 1] = "GRIDV"
		aStruct[38, 2] = "N"
		aStruct[38, 3] = 2
		aStruct[38, 4] = 0
		aStruct[39, 1] = "GRIDH"
		aStruct[39, 2] = "N"
		aStruct[39, 3] = 2
		aStruct[39, 4] = 0
		aStruct[40, 1] = "FLOAT"
		aStruct[40, 2] = "L"
		aStruct[40, 3] = 1
		aStruct[40, 4] = 0
		aStruct[41, 1] = "STRETCH"
		aStruct[41, 2] = "L"
		aStruct[41, 3] = 1
		aStruct[41, 4] = 0
		aStruct[42, 1] = "STRETCHTOP"
		aStruct[42, 2] = "L"
		aStruct[42, 3] = 1
		aStruct[42, 4] = 0
		aStruct[43, 1] = "TOP"
		aStruct[43, 2] = "L"
		aStruct[43, 3] = 1
		aStruct[43, 4] = 0
		aStruct[44, 1] = "BOTTOM"
		aStruct[44, 2] = "L"
		aStruct[44, 3] = 1
		aStruct[44, 4] = 0
		aStruct[45, 1] = "SUPTYPE"
		aStruct[45, 2] = "N"
		aStruct[45, 3] = 1
		aStruct[45, 4] = 0
		aStruct[46, 1] = "SUPREST"
		aStruct[46, 2] = "N"
		aStruct[46, 3] = 1
		aStruct[46, 4] = 0
		aStruct[47, 1] = "NOREPEAT"
		aStruct[47, 2] = "L"
		aStruct[47, 3] = 1
		aStruct[47, 4] = 0
		aStruct[48, 1] = "RESETRPT"
		aStruct[48, 2] = "N"
		aStruct[48, 3] = 2
		aStruct[48, 4] = 0
		aStruct[49, 1] = "PAGEBREAK"
		aStruct[49, 2] = "L"
		aStruct[49, 3] = 1
		aStruct[49, 4] = 0
		aStruct[50, 1] = "COLBREAK"
		aStruct[50, 2] = "L"
		aStruct[50, 3] = 1
		aStruct[50, 4] = 0
		aStruct[51, 1] = "RESETPAGE"
		aStruct[51, 2] = "L"
		aStruct[51, 3] = 1
		aStruct[51, 4] = 0
		aStruct[52, 1] = "GENERAL"
		aStruct[52, 2] = "N"
		aStruct[52, 3] = 3
		aStruct[52, 4] = 0
		aStruct[53, 1] = "SPACING"
		aStruct[53, 2] = "N"
		aStruct[53, 3] = 3
		aStruct[53, 4] = 0
		aStruct[54, 1] = "DOUBLE"
		aStruct[54, 2] = "L"
		aStruct[54, 3] = 1
		aStruct[54, 4] = 0
		aStruct[55, 1] = "SWAPHEADER"
		aStruct[55, 2] = "L"
		aStruct[55, 3] = 1
		aStruct[55, 4] = 0
		aStruct[56, 1] = "SWAPFOOTER"
		aStruct[56, 2] = "L"
		aStruct[56, 3] = 1
		aStruct[56, 4] = 0
		aStruct[57, 1] = "EJECTBEFOR"
		aStruct[57, 2] = "L"
		aStruct[57, 3] = 1
		aStruct[57, 4] = 0
		aStruct[58, 1] = "EJECTAFTER"
		aStruct[58, 2] = "L"
		aStruct[58, 3] = 1
		aStruct[58, 4] = 0
		aStruct[59, 1] = "PLAIN"
		aStruct[59, 2] = "L"
		aStruct[59, 3] = 1
		aStruct[59, 4] = 0
		aStruct[60, 1] = "SUMMARY"
		aStruct[60, 2] = "L"
		aStruct[60, 3] = 1
		aStruct[60, 4] = 0
		aStruct[61, 1] = "ADDALIAS"
		aStruct[61, 2] = "L"
		aStruct[61, 3] = 1
		aStruct[61, 4] = 0
		aStruct[62, 1] = "OFFSET"
		aStruct[62, 2] = "N"
		aStruct[62, 3] = 3
		aStruct[62, 4] = 0
		aStruct[63, 1] = "TOPMARGIN"
		aStruct[63, 2] = "N"
		aStruct[63, 3] = 3
		aStruct[63, 4] = 0
		aStruct[64, 1] = "BOTMARGIN"
		aStruct[64, 2] = "N"
		aStruct[64, 3] = 3
		aStruct[64, 4] = 0
		aStruct[65, 1] = "TOTALTYPE"
		aStruct[65, 2] = "N"
		aStruct[65, 3] = 2
		aStruct[65, 4] = 0
		aStruct[66, 1] = "RESETTOTAL"
		aStruct[66, 2] = "N"
		aStruct[66, 3] = 2
		aStruct[66, 4] = 0
		aStruct[67, 1] = "RESOID"
		aStruct[67, 2] = "N"
		aStruct[67, 3] = 3
		aStruct[67, 4] = 0
		aStruct[68, 1] = "CURPOS"
		aStruct[68, 2] = "L"
		aStruct[68, 3] = 1
		aStruct[68, 4] = 0
		aStruct[69, 1] = "SUPALWAYS"
		aStruct[69, 2] = "L"
		aStruct[69, 3] = 1
		aStruct[69, 4] = 0
		aStruct[70, 1] = "SUPOVFLOW"
		aStruct[70, 2] = "L"
		aStruct[70, 3] = 1
		aStruct[70, 4] = 0
		aStruct[71, 1] = "SUPRPCOL"
		aStruct[71, 2] = "N"
		aStruct[71, 3] = 1
		aStruct[71, 4] = 0
		aStruct[72, 1] = "SUPGROUP"
		aStruct[72, 2] = "N"
		aStruct[72, 3] = 2
		aStruct[72, 4] = 0
		aStruct[73, 1] = "SUPVALCHNG"
		aStruct[73, 2] = "L"
		aStruct[73, 3] = 1
		aStruct[73, 4] = 0
		aStruct[74, 1] = "SUPEXPR"
		aStruct[74, 2] = "M"
		aStruct[74, 3] = 4
		aStruct[74, 4] = 0
		aStruct[75, 1] = "USER"
		aStruct[75, 2] = "M"
		aStruct[75, 3] = 4
		aStruct[75, 4] = 0
	endproc	
enddefine

DEFINE CLASS thermometer AS form

	Top = 196
	Left = 142
	Height = 88
	Width = 356
	AutoCenter = .T.
	BackColor = RGB(192,192,192)
	BorderStyle = 0
	Caption = ""
	Closable = .F.
	ControlBox = .F.
	MaxButton = .F.
	MinButton = .F.
	Movable = .F.
	AlwaysOnTop = .F.
	ipercentage = 0
	iBasis = 0
	ccurrenttask = ''
	shpthermbarmaxwidth = 322
	cthermref = ""
	Name = "thermometer"

	ADD OBJECT shape10 AS shape WITH ;
		BorderColor = RGB(128,128,128), ;
		Height = 81, ;
		Left = 3, ;
		Top = 3, ;
		Width = 1, ;
		Name = "Shape10"


	ADD OBJECT shape9 AS shape WITH ;
		BorderColor = RGB(128,128,128), ;
		Height = 1, ;
		Left = 3, ;
		Top = 3, ;
		Width = 349, ;
		Name = "Shape9"


	ADD OBJECT shape8 AS shape WITH ;
		BorderColor = RGB(255,255,255), ;
		Height = 82, ;
		Left = 352, ;
		Top = 3, ;
		Width = 1, ;
		Name = "Shape8"


	ADD OBJECT shape7 AS shape WITH ;
		BorderColor = RGB(255,255,255), ;
		Height = 1, ;
		Left = 3, ;
		Top = 84, ;
		Width = 350, ;
		Name = "Shape7"


	ADD OBJECT shape6 AS shape WITH ;
		BorderColor = RGB(128,128,128), ;
		Height = 86, ;
		Left = 354, ;
		Top = 1, ;
		Width = 1, ;
		Name = "Shape6"


	ADD OBJECT shape4 AS shape WITH ;
		BorderColor = RGB(128,128,128), ;
		Height = 1, ;
		Left = 1, ;
		Top = 86, ;
		Width = 354, ;
		Name = "Shape4"


	ADD OBJECT shape3 AS shape WITH ;
		BorderColor = RGB(255,255,255), ;
		Height = 85, ;
		Left = 1, ;
		Top = 1, ;
		Width = 1, ;
		Name = "Shape3"


	ADD OBJECT shape2 AS shape WITH ;
		BorderColor = RGB(255,255,255), ;
		Height = 1, ;
		Left = 1, ;
		Top = 1, ;
		Width = 353, ;
		Name = "Shape2"


	ADD OBJECT shape1 AS shape WITH ;
		BackStyle = 0, ;
		Height = 88, ;
		Left = 0, ;
		Top = 0, ;
		Width = 356, ;
		Name = "Shape1"


	ADD OBJECT shape5 AS shape WITH ;
		BorderStyle = 0, ;
		FillColor = RGB(192,192,192), ;
		FillStyle = 0, ;
		Height = 15, ;
		Left = 17, ;
		Top = 47, ;
		Width = 322, ;
		Name = "Shape5"


	ADD OBJECT lbltitle AS label WITH ;
		FontName = WIN32FONT, ;
		FontSize = 8, ;
		BackStyle = 0, ;
		BackColor = RGB(192,192,192), ;
		Caption = "", ;
		Height = 16, ;
		Left = 18, ;
		Top = 14, ;
		Width = 319, ;
		WordWrap = .F., ;
		Name = "lblTitle"


	ADD OBJECT lbltask AS label WITH ;
		FontName = WIN32FONT, ;
		FontSize = 8, ;
		BackStyle = 0, ;
		BackColor = RGB(192,192,192), ;
		Caption = "", ;
		Height = 16, ;
		Left = 18, ;
		Top = 27, ;
		Width = 319, ;
		WordWrap = .F., ;
		Name = "lblTask"


	ADD OBJECT shpthermbar AS shape WITH ;
		BorderStyle = 0, ;
		FillColor = RGB(128,128,128), ;
		FillStyle = 0, ;
		Height = 16, ;
		Left = 17, ;
		Top = 46, ;
		Width = 0, ;
		Name = "shpThermBar"


	ADD OBJECT lblpercentage AS label WITH ;
		FontName = WIN32FONT, ;
		FontSize = 8, ;
		BackStyle = 0, ;
		Caption = "0%", ;
		Height = 13, ;
		Left = 170, ;
		Top = 47, ;
		Width = 16, ;
		Name = "lblPercentage"


	ADD OBJECT lblpercentage2 AS label WITH ;
		FontName = WIN32FONT, ;
		FontSize = 8, ;
		BackColor = RGB(0,0,255), ;
		BackStyle = 0, ;
		Caption = "Label1", ;
		ForeColor = RGB(255,255,255), ;
		Height = 13, ;
		Left = 170, ;
		Top = 47, ;
		Width = 0, ;
		Name = "lblPercentage2"


	ADD OBJECT shape11 AS shape WITH ;
		BorderColor = RGB(128,128,128), ;
		Height = 1, ;
		Left = 16, ;
		Top = 45, ;
		Width = 322, ;
		Name = "Shape11"


	ADD OBJECT shape12 AS shape WITH ;
		BorderColor = RGB(255,255,255), ;
		Height = 1, ;
		Left = 16, ;
		Top = 61, ;
		Width = 323, ;
		Name = "Shape12"


	ADD OBJECT shape13 AS shape WITH ;
		BorderColor = RGB(128,128,128), ;
		Height = 16, ;
		Left = 16, ;
		Top = 45, ;
		Width = 1, ;
		Name = "Shape13"


	ADD OBJECT shape14 AS shape WITH ;
		BorderColor = RGB(255,255,255), ;
		Height = 17, ;
		Left = 338, ;
		Top = 45, ;
		Width = 1, ;
		Name = "Shape14"


	ADD OBJECT lblescapemessage AS label WITH ;
		FontBold = .F., ;
		FontName = WIN32FONT, ;
		FontSize = 8, ;
		Alignment = 2, ;
		BackStyle = 0, ;
		BackColor = RGB(192,192,192), ;
		Caption = "", ;
		Height = 14, ;
		Left = 17, ;
		Top = 68, ;
		Width = 322, ;
		WordWrap = .F., ;
		Name = "lblEscapeMessage"

	PROCEDURE complete
		* This is the default complete message
		parameters m.cTask
		private iSeconds
		if parameters() = 0
			m.cTask = THERMCOMPLETE_LOC
		endif
		this.Update(100,m.cTask)
	ENDPROC

	procedure UpdateTaskMessage
		* Update the task message only, used when converting binary data
		parameters cTask
		this.cCurrentTask = m.cTask
		this.lblTask.Caption = this.cCurrentTask
	endproc
	
	PROCEDURE update
		* m.iProgress is the percentage complete
		* m.cTask is displayed on the second line of the window

		parameters iProgress, cTask

		if parameters() >= 2 .and. type('m.cTask') = 'C'
			* If we're specifically passed a null string, clear the current task,
			* otherwise leave it alone
			this.cCurrentTask = m.cTask
		endif
		
		if ! this.lblTask.Caption == this.cCurrentTask
			this.lblTask.Caption = this.cCurrentTask
		endif

		if this.iBasis <> 0
			* interpret m.iProgress in terms of this.iBasis
			m.iPercentage = int((m.iProgress / this.iBasis) * 100)
		else
			m.iPercentage = m.iProgress
		endif
		
		m.iPercentage = min(100,max(0,m.iPercentage))
		
		if m.iPercentage = this.iPercentage
			RETURN
		endif
		
		if len(alltrim(str(m.iPercentage,3)))<>len(alltrim(str(this.iPercentage,3)))
			iAvgCharWidth=fontmetric(6,this.lblPercentage.FontName, ;
				this.lblPercentage.FontSize, ;
				iif(this.lblPercentage.FontBold,'B','')+ ;
				iif(this.lblPercentage.FontItalic,'I',''))
			this.lblPercentage.Width=txtwidth(alltrim(str(m.iPercentage,3)) + '%', ;
				this.lblPercentage.FontName,this.lblPercentage.FontSize, ;
				iif(this.lblPercentage.FontBold,'B','')+ ;
				iif(this.lblPercentage.FontItalic,'I','')) * iAvgCharWidth
			this.lblPercentage.Left=int((this.shpThermBarMaxWidth- ;
				this.lblPercentage.Width) / 2)+this.shpThermBar.Left-1
			this.lblPercentage2.Left=this.lblPercentage.Left
		endif
		this.shpThermBar.Width = int((this.shpThermBarMaxWidth)*m.iPercentage/100)
		this.lblPercentage.Caption = alltrim(str(m.iPercentage,3)) + '%'
		this.lblPercentage2.Caption = this.lblPercentage.Caption
		if this.shpThermBar.Left + this.shpThermBar.Width -1 >= ;
			this.lblPercentage2.Left
			if this.shpThermBar.Left + this.shpThermBar.Width - 1 >= ;
				this.lblPercentage2.Left + this.lblPercentage.Width - 1
				this.lblPercentage2.Width = this.lblPercentage.Width
			else
				this.lblPercentage2.Width = ;
					this.shpThermBar.Left + this.shpThermBar.Width - ;
					this.lblPercentage2.Left - 1
			endif
		else
			this.lblPercentage2.Width = 0
		endif
		this.iPercentage = m.iPercentage
	ENDPROC

	PROCEDURE Init
		* m.cTitle is displayed on the first line of the window
		* m.iInterval is the frequency used for updating the thermometer
		parameters cTitle, iInterval
		this.lblTitle.Caption = iif(empty(m.cTitle),'',m.cTitle)
		this.shpThermBar.FillColor = rgb(128,128,128)
		local cColor

		* Check to see if the fontmetrics for MS Sans Serif matches
		* those on the system developed. If not, switch to Arial. 
		* The RETURN value indicates whether the font was changed.
		if fontmetric(1, WIN32FONT, 8, '') <> 13 .or. ;
			fontmetric(4, WIN32FONT, 8, '') <> 2 .or. ;
			fontmetric(6, WIN32FONT, 8, '') <> 5 .or. ;
			fontmetric(7, WIN32FONT, 8, '') <> 11
			this.SetAll('FontName', WIN95FONT)
		endif

		m.cColor = rgbscheme(1, 2)
		m.cColor = 'rgb(' + substr(m.cColor, at(',', m.cColor, 3) + 1)
		this.BackColor = &cColor
		this.Shape5.FillColor = &cColor
	ENDPROC
ENDDEFINE
