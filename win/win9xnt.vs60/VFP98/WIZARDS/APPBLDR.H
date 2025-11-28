#DEFINE CRLF CHR(13)+CHR(10)
#DEFINE	MENU_PROMPT_LOC		"App\<lication Builder..."
#DEFINE	MENU_MESSAGE_LOC	"Runs Application Framework Builder"

#DEFINE LOG_NEWDATA_LOC	"Added data, form, or report files to project."
#DEFINE LOG_UPDATE_LOC	"Updated project with new Application Builder settings."

#DEFINE FRAMEWORK_HELPID	2299000008
#DEFINE OVERALL_HELPID		2299000000
#DEFINE GENERAL_HELPID		2299000007
#DEFINE CREDITS_HELPID		2299000001
#DEFINE DATA_HELPID			2299000002
#DEFINE FORMS_HELPID		2299000003
#DEFINE REPORTS_HELPID		2299000004
#DEFINE ADVANCED_HELPID		2299000005

#DEFINE C_DEFAULTSTYLE "Embossed"
#DEFINE C_NOSTYLETABLE_LOC 	"In order to use the Data page, please run the Form Wizard once to create the visual style table."
#DEFINE C_WIZSTYLETABLE1 "WIZARDS\FRMSTYLE.DBF"
#DEFINE C_WIZSTYLETABLE2 "WIZARDS\FRMSTYL2.DBF"
#DEFINE C_WIZSTYLETABLE3 "WIZARDS\RPTSTYLE.DBF"

#DEFINE C_RPTSTYLE1_LOC "Presentation"
#DEFINE C_RPTSTYLE2_LOC "Ledger"
#DEFINE C_RPTSTYLE3_LOC "Executive"
#DEFINE C_RPTSTYLE1_FILE "Style1H.FRX"
#DEFINE C_RPTSTYLE2_FILE "Style2H.FRX"
#DEFINE C_RPTSTYLE3_FILE "Style3H.FRX"

#DEFINE C_FORMDESCSUFFIX_LOC " Form"
#DEFINE C_RPTDESCSUFFIX_LOC " Report"

#DEFINE C_ADDFILE_LOC "Would you also like to add this file to the framework meta table?"

#DEFINE APPHOOK_FILE	"APPHOOK.VCX"
#DEFINE APPHOOK_CLASS	"APPHOOK"
#DEFINE C_SELECTHOOK_LOC	"Select Application Hook Class"

#DEFINE C_INSTALLFRAMEWORK_LOC "The Application Builder will allow you to store additional information "+;
		"for your forms and reports if you chose to install an application meta table. "+;
		"Would you like to install the meta table now?" +CRLF+CRLF+C_INSTALLFRAMEWORK5_LOC 
		
#DEFINE C_INSTALLFRAMEWORK1_LOC "It appears that you already have an application meta table for this project "+;
		"but no project hooks. Would you like to install the project hooks now?" +CRLF+CRLF+C_INSTALLFRAMEWORK5_LOC 

#DEFINE C_INSTALLFRAMEWORK2_LOC "You will have to close and reopen the project once for full changes to take effect."
#DEFINE C_INSTALLFRAMEWORK3_LOC "You have a custom project hook installed. Certain builder options may be disabled."
#DEFINE C_INSTALLFRAMEWORK4_LOC "It appears that you have an application meta table for this project, but it is not a valid structure for use by this builder. Certain builders options will be disabled."
#DEFINE C_INSTALLFRAMEWORK5_LOC "Note: certain tabs on the builder are only available if you create a project using the Application Wizard which has a complete application framework."
#DEFINE C_INSTALLFRAMEWORK6_LOC "The application meta table specified is not a valid meta table for this Builder. Certain builders options will be disabled."

#DEFINE C_NOHELP_LOC	"The FOXHELP.CHM file could not be loaded. This Help file contains valuable tips and ideas for developing your applications using the Application Builder and other tools."

#DEFINE C_DATAPAGEOPEN_LOC	"You have outstanding items selected in the Data page that have not yet been processed. Would you like to process them before exiting?"

#DEFINE C_FORMSFOLDER		"FORMS"
#DEFINE C_MENUSFOLDER		"MENUS"
#DEFINE C_REPORTSFOLDER		"REPORTS"
#DEFINE C_DATAFOLDER		"DATA"
#DEFINE C_GALLERYCLASS		"galleryutils"
#DEFINE C_GALLERYCLASSLIB	"GALLERY\VFPGLRY.VCX"
#DEFINE C_WEBCLASSLIB		"GALLERY\_WEBVIEW.VCX"
#DEFINE C_HTMHELP			"WIZARDS\_FRAMEWK.HTM"
#DEFINE C_WEBCLASS			"_webform"
#DEFINE	C_WEBCAPTION_LOC	"What's Next!"
#DEFINE C_NOWEBFILE_LOC		"One of the Application Builder help files could not be found. "+;
							"Check to make sure that both GALLERY\_WEBVIEW.VCX and WIZARDS\_FRAMEWK.HTM "+;
							"are installed properly in your Visual FoxPro root directory."

#DEFINE C_NOFRAMEWORK_LOC "Application does not appear to contain VFP framework classes. Certain tabs will be disabled."
#DEFINE C_FAILAPPOBJ_LOC	"Failed to create App Object"
#DEFINE	C_NOUPDATEGO_LOC "Could not update Go menu. Make sure file is not in use."

#DEFINE	C_NOUPDATE1_LOC "Could not update Application class. Make sure file "
#DEFINE	C_NOUPDATE2_LOC " is not in use."
#DEFINE C_NOFIND_LOC	"Could not find Application class."
#DEFINE C_NOSPLASH_LOC	"Could not find Splash class."

#DEFINE C_NOPROJ_LOC	"You must have a project open to use."
#DEFINE C_WIZFILEEXISTS1_LOC	"The file "
#DEFINE C_WIZFILEEXISTS2_LOC	" already exists. Would you like to have the Wizard generate a new one?"

#DEFINE C_METAEXCL1_LOC	"The meta table "
#DEFINE C_METAEXCL2_LOC	" could not be opened because it may be used exclusively. You need to close it or open it shared for use with Application Builder."

#DEFINE APP_BUILDER_TOPFORM 	"_topform"
#DEFINE APP_BUILDER_APP		 	"_application"
#DEFINE APP_BUILDER_SPLASH 		"_splash"

#DEFINE TOPMENU_OBJTYPE			5
#DEFINE NORMALMENU_OBJTYPE		1

#DEFINE APP_BUILDER_CLASS_PREFIX 	  "app"
#DEFINE	APP_BUILDER_FILE_SUFFIX		  "_app"

#DEFINE APP_BUILDER_MAINMENU_SUFFIX   "_main"
#DEFINE APP_BUILDER_TOPMENU_SUFFIX    "_top"		
#DEFINE APP_BUILDER_APPENDMENU_SUFFIX "_append"		
#DEFINE APP_BUILDER_GOMENU_SUFFIX     "_go"

#DEFINE STAT_LOADING_LOC	"Loading Application Builder...."
#DEFINE STAT_APPBLDR_LOC	"Application Builder"
#DEFINE STAT_UPDATE_LOC		"Please be patient while application is updated...."

#DEFINE FORM_TYPE_EXTENSIONS	"SCX;VCX;PRG"
#DEFINE REPORT_TYPE_EXTENSIONS	"FRX;LBX;PRG"
#DEFINE BADFILETYPE_LOC	"Invalid file type selected."
#DEFINE NOTINMETA_LOC	"File(s) were found in the meta table which no longer exist in the project. Would you like to remove these from the meta table?"
#DEFINE DELETEFROMMETA_LOC	"Are you sure you want to remove this document from the meta table?"
#DEFINE WRITEOUTPROJECT_LOC	"The Application Builder currently contains data from another project. Would you like to save these changes?"

#DEFINE CAPTION_LOC	"Application Builder"
#DEFINE UPDATINGAPP_LOC	"Updating application..."

#DEFINE CLEANUPMETA_LOC	"This routine will synchronize the active project to entries in the application meta table by adding/removing files from meta table to correspond with those in the active project. Would you like to continue?"

#DEFINE STAT_UPDATEMETA_LOC	"Removing files from meta table not found in active project..."
#DEFINE STAT_UPDATEPJX_LOC	"Adding new files to meta table found in active project..."
#DEFINE STAT_REFRESHDONE_LOC	"Finished updating."

#DEFINE	ADDAPPOBJ_LOC	"Would you like to add a mediator object to this form to fully enable it for use with the application framework?"

#DEFINE	MSG_ERROR_LOC	"Error number: "
#DEFINE	MSG_METHOD_LOC	"Method: "
#DEFINE	MSG_LINENUM_LOC	"Line number: "
#DEFINE	MSG_LINE_LOC	"Line: "
#DEFINE	MSG_MESSAGE_LOC "Message: "
#DEFINE	MSG_ERRORCONTINUE_LOC	"Would you like to continue running builder?"

#DEFINE APPOBJECT_PATH		"\libs\"
#DEFINE APPOBJECT_FILE		"_app.vcx"
#DEFINE APPOBJECT_NAME		"_application"

#DEFINE	FRMOBJECT_CLASS		"_formmediator"
#DEFINE	FRMOBJECT_FILE		"_framewk.vcx"
#DEFINE	FRMOBJECT_NAME		"app_mediator"

#DEFINE C_BASEAPPCLASS		"_application"
#DEFINE C_BASEAPPCLASSLIB	"WIZARDS\_FRAMEWK.VCX"

#DEFINE C_TABLEWIZ "WIZARDS\WZTABLE.APP"
#DEFINE C_DATAWIZ "WIZARDS\WZDBC.APP"

#DEFINE C_NOHOOKFILE_LOC	"The following file could not be located in your Wizards folder : "+APPHOOK_FILE +CRLF+CRLF+;
		"This file is required to use the Application Builder."
		
#DEFINE METAFIELD1	"DOC_TYPE"

#DEFINE LOCATEHELP_LOC	"Please locate the FOXHELP.CHM help file:"

#DEFINE MENU_ONTOP_LOC		"\<Always On Top"
#DEFINE MENU_CLEANUP_LOC	"\<Cleanup"
#DEFINE MENU_HELP_LOC		"A\<ssistance"
#DEFINE MENU_CANCEL_LOC		"Cancel"

#DEFINE C_PJXEXISTS_LOC	"This project name already exists, would you like to overwrite it?"