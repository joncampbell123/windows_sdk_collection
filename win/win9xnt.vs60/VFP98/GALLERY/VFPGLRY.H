#DEFINE CR		CHR(13)
#DEFINE LF		CHR(10)
#DEFINE CRLF	CHR(13)+CHR(10)
#DEFINE TAB		CHR(9)

* Messagebox constants
#DEFINE MSG_YESNO	36
#DEFINE MSG_YES		6
#DEFINE MSG_NO		7

*Shortcut menu prompts
#DEFINE MENU_VIEW_LOC				"\<View"
#DEFINE MENU_VIEW_LGICON_LOC		"Lar\<ge Icons"
#DEFINE MENU_VIEW_SMICON_LOC		"S\<mall Icons"
#DEFINE MENU_VIEW_LIST_LOC			"\<List"
#DEFINE MENU_VIEW_DETAILS_LOC		"\<Details"

#DEFINE MENU_CUT_LOC				"Cu\<t"
#DEFINE MENU_COPY_LOC				"\<Copy"
#DEFINE MENU_PASTE_LOC				"\<Paste"
#DEFINE MENU_PASTELINK_LOC			"Paste \<Link"
#DEFINE MENU_UNDO_LOC				"\<Undo"

#DEFINE MENU_CREATELINK_LOC			"Create \<Link"
#DEFINE MENU_REMOVE_LOC				"Remo\<ve"
#DEFINE MENU_RENAME_LOC				"Rena\<me"
#DEFINE MENU_NEW_LOC				"\<New"
#DEFINE MENU_NEW_FOLDER_LOC			"\<Folder"
#DEFINE MENU_NEW_ITEM_LOC			"\<Item"
#DEFINE MENU_NEW_OBJECT_LOC			"\<Object"
#DEFINE MENU_NEW_LINK_LOC			"\<Link"
#DEFINE MENU_PROPERTIES_LOC			"P\<roperties"

#DEFINE MENU_QVIEW_LOC				"\<Quick View"
#DEFINE MENU_SHOW_LOC				"S\<how"
#DEFINE MENU_OPENWITH_LOC			"Op\<en with..."
#DEFINE MENU_PRINT_LOC				"\<Print"

#DEFINE MENU_NEWFORM_LOC			"Ne\<w Form"
#DEFINE MENU_NEWREPO_LOC			"Ne\<w Report"
#DEFINE MENU_NEWAPP_LOC				"Ne\<w Application"
#DEFINE MENU_NEWDATA_LOC			"Ne\<w Database"
#DEFINE MENU_NEWTABLE_LOC			"Ne\<w Table"

#DEFINE MENU_ACTIVEX_LOC			"Refre\<sh Controls"
#DEFINE MENU_SYSTEM_LOC				"Install on my S\<ystem"
#DEFINE MENU_NEWCLSID_LOC			"Change Regi\<stered Class..."
#DEFINE MENU_ADDFORM_LOC			"Add to \<Form"
#DEFINE MENU_ADDPROJECT_LOC			"\<Add to Project"
#DEFINE MENU_MODIFY_LOC				"Mo\<dify..."
#DEFINE MENU_RUN_LOC				"Ru\<n"
#DEFINE MENU_TYPELIB_LOC			"Vie\<w Type Library..."
#DEFINE MENU_REGREMOTE_LOC			"Re\<gister Remote..."
#DEFINE MENU_VERSION_LOC			"Version \<Information..."

#DEFINE MENU_SAMPLE_LOC				"View \<Sample"
#DEFINE MENU_SAMPLEOPEN_LOC			"\<Open"
#DEFINE MENU_SAMPLERUN_LOC			"Ru\<n"
#DEFINE MENU_HELP_LOC				"\<Help..."
#DEFINE MENU_WEBHELP_LOC			"E\<xtended Web Help..."
#DEFINE MENU_PREVIEW_LOC			"Previe\<w"
#DEFINE MENU_OPEN_LOC				"\<Open"
#DEFINE MENU_ADDHYPER_LOC			"Add Hyperlink to \<Form..."
#DEFINE MENU_CLSBROWSER_LOC			"View in \<Browser"
#DEFINE MENU_CLASSFORM_LOC			"Create \<Form..."

#DEFINE MENU_SETWALL_LOC			"Set VFP Wallpaper"
#DEFINE MENU_CLEARWALL_LOC			"Clear VFP Wallpaper"
#DEFINE MENU_FORMWALL_LOC			"Set Form \<Wallpaper"

#DEFINE MENU_BROWSER_LOC			"\<Browse"
#DEFINE MENU_QUICKFORM_LOC			"\<Quick Form"
#DEFINE MENU_QUICKREPORT_LOC		"Quic\<k Report"

#DEFINE MENU_MAILMERGE_LOC			"\<Word Mail Merge"
#DEFINE MENU_PIVOT_LOC				"\<Excel Pivot Table"
#DEFINE MENU_OUTHTML_LOC			"\<Output to HTML"

#DEFINE MENU_OPENCATALOG_LOC		"\<Open Catalog"
#DEFINE MENU_ADDCATALOG_LOC			"\<Add Catalog"

#DEFINE MENU_BUILD_LOC				"\<Build"

* Shared Objects
#DEFINE OBJ_SHARED					"VFPGLRY!SHAREDOBJ"
#DEFINE OBJ_GLRYUTIL				"VFPGLRY!GLRYUTILOBJ"
#DEFINE OBJ_REGISTRY				"VFPGLRY!REGOBJ"


#DEFINE WIN_PJX_DESIGN_LOC			"PROJECT MANAGER -"
#DEFINE WIN_SCX_DESIGN_LOC			"FORM DESIGNER -"
#DEFINE WIN_VCX_DESIGN_LOC			"CLASS DESIGNER -"
#DEFINE WIN_FRX_DESIGN_LOC			"REPORT DESIGNER -"
#DEFINE WIN_MNX1_DESIGN_LOC			"MENU DESIGNER -"
#DEFINE WIN_MNX2_DESIGN_LOC			"SHORTCUT DESIGNER -"
#DEFINE WIN_DBC_DESIGN_LOC			"DATABASE DESIGNER -"
#DEFINE WIN_PJX_DESIGN_FLAG			1
#DEFINE WIN_SCX_DESIGN_FLAG			2
#DEFINE WIN_VCX_DESIGN_FLAG			4
#DEFINE WIN_FRX_DESIGN_FLAG			8
#DEFINE WIN_MNX1_DESIGN_FLAG		16
#DEFINE WIN_MNX2_DESIGN_FLAG		32
#DEFINE WIN_DBC_DESIGN_FLAG			64

* File Extensions
#DEFINE	LOCALSRV_EXTN		"EXE"
#DEFINE	INPROCSRV_EXTN		"DLL"
#DEFINE	TYPELIB_EXTN		"TLB"
#DEFINE	TYPELIB2_EXTN		"OLB"
#DEFINE	REMOTEREG_EXTN		"VBR"
#DEFINE	CONTROL_EXTN		"OCX"
#DEFINE	PROJECT_EXTN		"PJX"
#DEFINE	FORM_EXTN			"SCX"
#DEFINE	CLASS_EXTN			"VCX"
#DEFINE	REPORT_EXTN			"FRX"

#DEFINE	OLECONTROL_CLASS	"OLECONTROL"

* _ActiveXFolder
#DEFINE C_DEFACTX_PARENT	"ACTXSYSCTRLS"
#DEFINE REGPROMPT1_LOC	"No ActiveX Controls were found registered in Options dialog. "+;
	"Would you like search the entire Registry?"
#DEFINE MSG_ADDACTIVEX1_LOC "Reading Registry for ActiveX Components..."
#DEFINE MSG_ADDACTIVEX2_LOC	"Processed "
#DEFINE MSG_ADDACTIVEX3_LOC	"% complete"
#DEFINE MSG_ADDACTIVEX4_LOC	"Adding ActiveX Items to Catalog..."
#DEFINE MSG_ADDACTIVEX5_LOC "Searching Registry for available classes..."
#DEFINE TMP_ADDACTIVEXTMP_LOC	"Refreshing catalog..."
#DEFINE HKEY_CLASSES_ROOT		-2147483648  && BITSET(0,31)
#DEFINE HKEY_CURRENT_USER		-2147483647  && BITSET(0,31)+1


#DEFINE VFP_OPTIONS_KEY1		"Software\Microsoft\VisualFoxPro\"
#DEFINE VFP_OPTIONS_KEY2		"\Options\OLEList"
#DEFINE CLSID_KEY				"CLSID"
#DEFINE PROGID_KEY				"\ProgID"
#DEFINE CONTROL_KEY				"Control"
#DEFINE SERVER_KEY				"Programmable"
#DEFINE SHELL_KEY				"\Shell\"
#DEFINE INPROC_KEY				"InProcServer32"
#DEFINE LOCALSVR_KEY			"LocalServer32"

*_ACTIVEXITEM
#DEFINE DEFWIDTH	200
#DEFINE DEFHEIGHT	150
#DEFINE ACTXCONTROL	0
#DEFINE ACTXDLLSVR	1
#DEFINE ACTXEXESVR	2
#DEFINE	MSG_GETCONTROL_LOC	"Select directory for control:"
#DEFINE ERR_BADCOPY_LOC		"Could not copy ActiveX file to local machine. Check for access or write problems."
#DEFINE MSG_GOODINSTALL_LOC	"ActiveX component successfully installed."
#DEFINE MSG_NOVBRFILE_LOC 	"Could not locate associated VBR file with ActiveX Server. Make sure it is installed in same location as server."
#DEFINE	ERR_NOACTXFILE_LOC	"File name does not exist for this item."
#DEFINE WINSYS_NT	"\SYSTEM32\"
#DEFINE	WINSYS_W95	"\SYSTEM\"
#DEFINE	MSG_NOREGSVR32_LOC	"Could not locate REGSVR32.EXE in you Windows System folder. Make sure it is properly installed."
#DEFINE REGSVR_FILE			"REGSVR32.EXE"
#DEFINE CLIREG_FILE			"CLIREG32.EXE"
#DEFINE MSG_UPDATEFILE_LOC		"Would you like to update this item to point to the new local file?"
#DEFINE	MSG_REMOTEUPDATE_LOC	"Would you like to update this item from its remote location?"
#DEFINE MSG_NOCLIREG32_LOC 	"Could not locate CLIREG32.EXE in you FoxPro folder. Make sure it is properly installed."
#DEFINE	ITEMPICKER_CLASS	"itempicker"
#DEFINE	DATAPICKER_CLASS	"datapicker"
#DEFINE	PROJPICKER_CLASS	"projectpicker"
#DEFINE	VFPGLRY_VCX			"gallery\vfpglry.vcx"
#DEFINE	MSG_NOCLASSREG_LOC	"Component does not appear to be properly registered. Try selecting Install on my System menu option."
#DEFINE	MSG_NOREG_LOC		"** not registered **"
#DEFINE	MSG_ONECLASSREG_LOC	"Only one class is registerd for this component: "

#DEFINE	ERRCODE_GOODINSTALL	0
#DEFINE	ERRCODE_CLASSEXISTS	1
#DEFINE	ERRCODE_FAILINSTALL	2
	
*_CLASSITEM
#DEFINE MSG_NOSAMPLE_LOC	"No sample available."
#DEFINE STAT_LOADCLASS_LOC	"Loading Class Designer ..."
#DEFINE	VFP_SCREEN			"SCREEN"
#DEFINE	DEFCLASS			"DEFINE CLASS "
#DEFINE FORMNAMEVAR_LOC		"A public variable for form was created named: "
#DEFINE NOHELP_LOC			"No help available."
#DEFINE ERR_OBJNOTCREATED_LOC	"This class was not created -- possibly due to the INIT event returning a .F. value."

#DEFINE VFPHELPFILE		"FOXHELP.CHM"
#DEFINE MSDNHELPFILE	"MSDNVS98.COL"

*FILEITEM
#DEFINE SHELLPRINTTO	"printto"
#DEFINE SHELLDEFAULT	"open"
#DEFINE SHELLNEW		"new"
#DEFINE SHELLSHOW		"show"
#DEFINE SHELLPRINT		"print"
#DEFINE QVIEWPATH 		"QuickView\."
#DEFINE QVIEWEXEPATH	"QuickView\Shell\Open\Command"
#DEFINE BINARYTYPE		"*Binary*"			&& Binary data in any form.

*SHAREDITEM
#DEFINE MSG_NOFORM_LOC 		"No forms open..."
#DEFINE MSG_NOPROJECT_LOC 	"No projects open..."
#DEFINE MSG_FILEADDED_LOC 	"File successfully added to project."
#DEFINE MSG_FILEINPJX_LOC 	"File already in project." 
#DEFINE MSG_LOCATE_LOC 		"Locate:" 
#DEFINE ERR_NOGLRYDLOG_LOC 	"One of the gallery's components is in use. Operation could not be performed" 
#DEFINE C_SELECTFORM_LOC	"Select &form"

*TEMPLATEITEM
#DEFINE DEFWIZCLASSLIB 	"WIZBTNS.VCX"

*File Version Strings
#DEFINE FILEVER_COMMENT_LOC 		"Comments: "
#DEFINE FILEVER_COMPANY_LOC 		"Company Name: "
#DEFINE FILEVER_FILEDESC_LOC 		"File Description: "
#DEFINE FILEVER_FILEVER_LOC 		"File Version: "
#DEFINE FILEVER_INTERNAL_LOC		"Internal Name: "
#DEFINE FILEVER_COPYRIGHT_LOC	 	"Legal Copyright: "
#DEFINE FILEVER_TRADMARK_LOC		"Legal Trademarks: "
#DEFINE FILEVER_FILENAME_LOC	 	"Original Filename: "
#DEFINE FILEVER_PRIVATE_LOC 		"Private Build: "
#DEFINE FILEVER_PRODUCTNAME_LOC		"Product Name: "
#DEFINE FILEVER_PRODUCTVER_LOC	 	"Product Version: "
#DEFINE FILEVER_SPECIAL_LOC			"Special Build: "
#DEFINE FILEVER_LANGUAGE_LOC		"Language: "
#DEFINE FILEVER_NOVERSION_LOC		"No version information found."
#DEFINE MSG_FILEVERSION_LOC			"Version information for: "

**Item Type Icons**
#DEFINE ICONFOLDER		"graphics\"
#DEFINE ICO_ACTXEXE		"actxexe.ico"
#DEFINE ICO_ACTXDLL		"actxdll.ico"
#DEFINE ICO_ACTXOCX		"actxocx.ico"
#DEFINE ICO_APPSTYLE	"appstyle.ico"
#DEFINE ICO_AUDIO		"audio.ico"
#DEFINE ICO_CATALOG		"catalog.ico"
#DEFINE ICO_CLASS		"class.ico"
#DEFINE ICO_CLASSLIB	"classlib.ico"
#DEFINE ICO_FILE		"file.ico"
#DEFINE ICO_FORM		"form.ico"
#DEFINE ICO_FRMSTYLE	"frmstyle.ico"
#DEFINE ICO_GIF			"gif.ico"
#DEFINE ICO_HELP		"help.ico"
#DEFINE ICO_JPEG		"jpeg.ico"
#DEFINE ICO_LABEL		"label.ico"
#DEFINE ICO_MENU		"menu.ico"
#DEFINE ICO_MSEXCEL		"excel.ico"
#DEFINE ICO_MSWORD		"word.ico"
#DEFINE ICO_MSOUTLOOK	"outlook.ico"
#DEFINE ICO_MSPOWERPNT	"powerpoint.ico"
#DEFINE ICO_MUSIC		"music.ico"
#DEFINE ICO_PICTURE		"picture.ico"
#DEFINE ICO_PROGRAM		"program.ico"
#DEFINE ICO_PROJECT		"project.ico"
#DEFINE ICO_REPORT		"report.ico"
#DEFINE ICO_RPTSTYLE	"rptstyle.ico"
#DEFINE ICO_TABLE		"table.ico"
#DEFINE ICO_TEXT		"text.ico"
#DEFINE ICO_VIEW		"view.ico"
#DEFINE ICO_WEBDOC		"webdoc.ico"
#DEFINE ICO_WEBFILE		"webfile.ico"
#DEFINE ICO_WEBSITE		"website.ico"

* Operating System codes
#DEFINE	OS_W32S				1
#DEFINE	OS_NT				2
#DEFINE	OS_WIN95			3
#DEFINE	OS_MAC				4
#DEFINE	OS_DOS				5
#DEFINE	OS_UNIX				6

* DLL Paths for various operating systems
#DEFINE DLLPATH_NT			"\SYSTEM32\"
#DEFINE DLLPATH_WIN95		"\SYSTEM\"

* DLL files used to read INI files
#DEFINE	DLL_KERNEL_NT		"KERNEL32.DLL"
#DEFINE	DLL_KERNEL_WIN95	"KERNEL32.DLL"

* DLL files used to read registry
#DEFINE	DLL_ADVAPI_NT		"ADVAPI32.DLL"
#DEFINE	DLL_ADVAPI_WIN95	"ADVAPI32.DLL"

* DLL files used to read ODBC info
#DEFINE DLL_ODBC_NT			"ODBC32.DLL"
#DEFINE DLL_ODBC_WIN95		"ODBC32.DLL"

* Registry roots
#DEFINE HKEY_CLASSES_ROOT           -2147483648  && BITSET(0,31)
#DEFINE HKEY_CURRENT_USER           -2147483647  && BITSET(0,31)+1
#DEFINE HKEY_LOCAL_MACHINE          -2147483646  && BITSET(0,31)+2
#DEFINE HKEY_USERS                  -2147483645  && BITSET(0,31)+3

* Misc
#DEFINE APP_PATH_KEY		"\Shell\Open\Command"
#DEFINE OLE_PATH_KEY		"\Protocol\StdFileEditing\Server"
#DEFINE VFP_OPTIONS_KEY1	"Software\Microsoft\VisualFoxPro\"
#DEFINE VFP_OPTIONS_KEY2	"\Options"
#DEFINE CURVER_KEY			"\CurVer"

* Error Codes
#DEFINE ERROR_SUCCESS		0	&& OK
#DEFINE ERROR_EOF 			259 && no more entries in key

* Note these next error codes are specific to this Class, not DLL
#DEFINE ERROR_NOAPIFILE		-101	&& DLL file to check registry not found
#DEFINE ERROR_KEYNOREG		-102	&& key not registered
#DEFINE ERROR_BADPARM		-103	&& bad parameter passed
#DEFINE ERROR_NOENTRY		-104	&& entry not found
#DEFINE	ERROR_BADKEY		-105	&& bad key passed
#DEFINE	ERROR_NONSTR_DATA	-106	&& data type for value is not a data string
#DEFINE ERROR_BADPLAT		-107	&& platform not supported
#DEFINE ERROR_NOPLAT		-111	&& call not supported on this platform


* Data types for keys
#DEFINE REG_SZ 				1	&& Data string
#DEFINE REG_EXPAND_SZ 		2	&& Unicode string
#DEFINE REG_BINARY 			3	&& Binary data in any form.
#DEFINE REG_DWORD 			4	&& A 32-bit number.

* Data types labels
#DEFINE REG_BINARY_LOC		"*Binary*"			&& Binary data in any form.
#DEFINE REG_DWORD_LOC 		"*Dword*"			&& A 32-bit number.
#DEFINE REG_UNKNOWN_LOC		"*Unknown type*"	&& unknown type

#DEFINE APPHOOK_FILE	"APPHOOK.VCX"
#DEFINE APPHOOK_CLASS	"APPHOOK"
