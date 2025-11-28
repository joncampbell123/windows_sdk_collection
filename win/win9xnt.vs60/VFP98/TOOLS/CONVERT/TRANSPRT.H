*
* Define Global Constants
*
#DEFINE debugversion     .T.    && enables asserts.  Should usually be .T.

*- used in Windows VFP 3.0 conversion dialog
#DEFINE C_MSGBOXTITLE_LOC	"Converter"
#DEFINE C_ASK1_LOC			"Ask for each file;Don't ask for "		&& new radio to prevent showing dialog during conversion
#DEFINE C_ASK2_LOC			" files;Don't ask for any files"		&& new radio to prevent showing dialog during conversion

#DEFINE C_OVERWRITE1_LOC		"Transporting All Objects will overwrite all existing "
* Filetype constants for FoxPro 2.0 and FoxPro 2.5/2.6 formats
#DEFINE C_OVERWRITE2_LOC		" object definitions in the file." 

#DEFINE MB_OKCANCEL             1       && OK and Cancel buttons
#DEFINE IDCANCEL				2       && Cancel button pressed

* Filetype constants for FoxPro 2.0 and FoxPro 2.5/2.6 formats
#DEFINE c_20pjxtype        1
#DEFINE c_25scxtype       12
#DEFINE c_20scxtype        2
#DEFINE c_25frxtype       13
#DEFINE c_20frxtype        3
#DEFINE c_25lbxtype       14
#DEFINE c_20lbxtype        4

*- indexes into gAShowMe array, controls display of transporter dialog for various file types
#DEFINE N_MAXTRANFILETYPES	3				&& number of file types that transporter can handle (All,PJX, SCX, FRX)
#DEFINE N_TRANFILE_PJX	1
#DEFINE N_TRANFILE_SCX	2
#DEFINE N_TRANFILE_FRX	3

* FoxPro 1.02 and FoxBASE+ formats.  Note that the FoxBASE+ types are never
* actually passed in m.tp_filetype.  They are inferred in GetOldReportType and
* GetOldLabelTypefrom the ID byte in the report/label files.  The suffix tells
* us how the file was called, by REPORT FORM ... or by MODIFY REPORT ...
#DEFINE c_frx102repo      23
#DEFINE c_frx102modi      33
#DEFINE c_fbprptrepo      43
#DEFINE c_fbprptmodi      53
#DEFINE c_lbx102repo      24
#DEFINE c_lbx102modi      34
#DEFINE c_fbplblrepo      44
#DEFINE c_fbplblmodi      54
#DEFINE c_db4type		  70

* Definitions for Objtype fields in screens/reports/labels
#DEFINE c_otheader         1
#DEFINE c_otworkar         2
#DEFINE c_otindex          3
#DEFINE c_otrel            4
#DEFINE c_ottext           5
#DEFINE c_otline           6
#DEFINE c_otbox            7
#DEFINE c_otrepfld         8
#DEFINE c_otband           9
#DEFINE c_otgroup         10
#DEFINE c_otlist          11
#DEFINE c_ottxtbut        12
#DEFINE c_otradbut        13
#DEFINE c_otchkbox        14
#DEFINE c_otfield         15
#DEFINE c_otpopup         16
#DEFINE c_otpicture       17
#DEFINE c_otrepvar        18
#DEFINE c_ot20lbxobj      19
#DEFINE c_otinvbut        20
#DEFINE c_otpdset         21
#DEFINE c_otspinner       22
#DEFINE c_otfontdata      23

#DEFINE C_OBJTYPELIST c_otlist,c_ottxtbut,c_otbox,c_otradbut,c_otchkbox,c_otfield, c_otpopup,c_otinvbut,c_otspinner,c_otpicture,c_otline,c_otrepfld,c_otrepvar,c_ottext

* Window types
#DEFINE c_user             1
#DEFINE c_system           2
#DEFINE c_dialog           3
#DEFINE c_alert            4

* ObjCode definitions
#DEFINE c_sgsay            0
#DEFINE c_sgget            1
#DEFINE c_sgedit           2
#DEFINE c_sgfrom           3
#DEFINE c_sgbox            4
#DEFINE c_sgboxd           5
#DEFINE c_sgboxp           6
#DEFINE c_sgboxc           7

#DEFINE c_lnvertical       0
#DEFINE c_lnhorizontal     1

#DEFINE c_ocboxgrp         1

* Attempt to preserve colors of text, lines and boxes when transporting to DOS?
#DEFINE c_maptextcolor     .T.

* Field counts
#DEFINE c_20scxfld        57
#DEFINE c_scxfld          79
#DEFINE c_20frxfld        36
#DEFINE c_frxfld          74
#DEFINE c_frx30fld        75		&& field count for 3.0 FRX file (11/1/95 jd)
#DEFINE c_ot20label       30
#DEFINE c_20lbxfld        17
#DEFINE c_20pjxfld        33
#DEFINE c_pjxfld          31

* Strings for product names
#DEFINE c_foxwin_loc   "FoxPro for Windows"
#DEFINE c_foxmac_loc   "FoxPro for Macintosh"
#DEFINE c_foxdos_loc   "FoxPro for MS-DOS/UNIX"
#DEFINE c_winname  "WINDOWS"
#DEFINE c_macname  "MAC"
#DEFINE c_dosname  "DOS"
#DEFINE c_unixname "DOS"
#DEFINE c_dosnum    1
#DEFINE c_winnum    2
#DEFINE c_macnum    3
#DEFINE c_unixnum   4

* Metrics for various objects, report bands, etc.
#DEFINE c_radhght      1.308
#DEFINE c_chkhght      1.308
#DEFINE c_listht       1.000
#DEFINE c_adjfld       0.125
#DEFINE c_adjlist      0.125
#DEFINE c_adjtbtn      0.769
#DEFINE c_adjrbtn      0.308
#DEFINE c_vchkbox      0.154
#DEFINE c_vradbtn      0.154
#DEFINE c_vlist        0.500
#DEFINE c_hpopup       1.000
#DEFINE c_adjbox       0.500
#DEFINE c_chkpixel        12

#DEFINE c_charrptheight   66
#DEFINE c_charrptwidth    80
#DEFINE c_linesperinch    (66/11)
#DEFINE c_charsperinch    13.71

#DEFINE c_pathsep ":"   && path separator character

#DEFINE c_mapfonts 3    && number of specially mapped fonts

* Version codes, put into Objcode fields in the header record
#DEFINE c_25scx           63
#DEFINE c_25frx           53

* Major file types
#DEFINE c_report           0
#DEFINE c_screen           1
#DEFINE c_label            2
#DEFINE c_project          3

* Error codes
#DEFINE c_error1   "Minor"
#DEFINE c_error2   "Serious"
#DEFINE c_error3   "Fatal"

* Return values
#DEFINE c_yes              1
#DEFINE c_no               0
#DEFINE c_cancel          -1

* Codepage translation.
#DEFINE c_cptrans       .T.    && do special CP translation for FoxBASE+ and FoxPro 1.02?
* The following four contants may need to be localized.
#DEFINE c_doscp          437   && default DOS code page
#DEFINE c_wincp         1252   && default Windows code page
#DEFINE c_maccp        10000
#DEFINE c_unixcp           0

* bands[] array indexes
#DEFINE c_tobandvpos       1
#DEFINE c_tobandheight     2
#DEFINE c_fmbandvpos       3
#DEFINE c_fmbandheight     4

* Frequently used strings.  Make them #DEFINES to simplify localization.
#DEFINE c_converting   "Converting"
#DEFINE c_transporting "Transporting"

#DEFINE C_2DCONTROLS_LOC "2D Controls"

* Defines used in converting FoxBASE+ reports
#DEFINE maxliterals   55
#DEFINE litpoolsize   1452
#DEFINE maxrepflds   24
#DEFINE h_page   1
#DEFINE h_break 3
#DEFINE l_item   4
#DEFINE f_break 5
#DEFINE f_page   7
#DEFINE f_rpt   8

#define c_space 40
#DEFINE dos_code  1
#DEFINE win_code  2
#DEFINE mac_code  3
#DEFINE unix_code 4
#DEFINE c_2dmark '2'
#DEFINE c_3dmark '3'

** New (8/22/95) added for Localization
#DEFINE T_TITLE_LOC "Visual FoxPro Transporter"
#DEFINE T_CONVASIS_LOC "Convert As Is"
#DEFINE T_NOCONV_LOC "If the file is not transported, it will not be converted."
#DEFINE T_NOTRANSPORT_LOC "Don't Transport"
#DEFINE T_RECMOD_LOC " Objects More Recently Modified"
#DEFINE T_NEWMOD_LOC "The objects are new to Windows, or more recently modified than their Windows equivalents."
#DEFINE T_NEWMOD1_LOC "The objects are new to "
#DEFINE T_NEWMOD2_LOC ", or more "+CHR(13)+"recently modified than their "
#DEFINE T_NEWMOD3_LOC " equivalents."
#DEFINE T_BYTRANS1_LOC  "By transporting this file, you create"
#DEFINE T_BYTRANS2_LOC  " definitions for these objects. "
#DEFINE T_BYTRANS3_LOC  "By transporting this file, you add, update, or " + CHR(13) + "replace "
#DEFINE T_BYTRANS4_LOC  " definitions for objects in the file."
#DEFINE T_BYTRANS5_LOC  "By transporting this file, you add,"
#DEFINE T_BYTRANS6_LOC	"update, or replace MS-DOS definitions"
#DEFINE T_ADDUPDREPL_LOC  	"By transporting this file, you add, update, or replace Windows definitions for objects in the file."
#DEFINE T_UPDMSDOSDEF_LOC 	"By transporting this file, you add, update, or replace MS-DOS definitions"
#DEFINE T_MSDOSDEF_LOC 		"By transporting this file, you create MS-DOS definitions for these objects."
#DEFINE T_OTHERPLAT_LOC		"There are objects in this file defined "+CHR(13)+"for a platform other than "
#DEFINE T_OTHERPLAT2_LOC	"There are objects in this file defined"
#DEFINE T_OTHERPLAT3_LOC	"for a platform other than MS-DOS."
#DEFINE T_NODOSDEF_LOC 		"There are objects in this file defined for a platform other than MS-DOS."
#DEFINE T_OPENASIS_LOC "Open As Is"
#DEFINE T_NEWDOSOBJ_LOC "Transport Objects New to MS-DOS"
#DEFINE T_TRANSOBJ_LOC "Transport Objects From:"
#DEFINE T_REPLOBJ_LOC "All Objects -- Replace Existing Definitions."
#DEFINE T_REPORT_FILE_LOC "Report File:"
#DEFINE T_LABEL_FILE_LOC  "Label File:"
#DEFINE T_SCREEN_FILE_LOC "Screen File:"
#DEFINE T_SPIN_LOC	"Spinner"
#DEFINE T_SEXPR_LOC	"SAY Expression"
#DEFINE T_GFIELD_LOC	"GET Field"
#DEFINE T_FIELD_LOC	"Field"
#DEFINE T_POPUP_LOC	"Popup"
#DEFINE T_PICTURE_LOC	"Picture"
#DEFINE T_RPTVAR_LOC	"Rpt variable"
#DEFINE T_INVBTN_LOC	"Inv button"
#DEFINE T_PDRIVER_LOC "Printer driver"
#DEFINE T_FONTDATA_LOC "Font data"
#DEFINE T_UNKNOWNVERS_LOC "Unknown FoxPro version."
#DEFINE T_ENERGIZE_LOC "Energize"
#DEFINE	T_SELTRANS_LOC	"Select the file to transport"
#DEFINE T_TRANSPORT_LOC "Transport"
#DEFINE T_TRANSPERR_LOC "Transporter Error"
#DEFINE T_LINENO_LOC	"Line Number: "
#DEFINE T_CLEANUP_LOC	"Press any key to cleanup and exit..."
#DEFINE T_OBJNEWMOD_LOC "These objects are either new to the Windows platform or have been modified more recently"
#DEFINE T_OBJMOD_LOC "These objects have been modified more recently on "
#DEFINE T_OBJNEW1_LOC "These objects are new to "
#DEFINE T_OBJNEW2_LOC " platform or have been modified more recently on "
#DEFINE T_UNCHECK1_LOC "Uncheck any items you do"
#DEFINE T_UNCHECK2_LOC "not"
#DEFINE T_UNCHECK3_LOC "want to be transported."
#DEFINE T_EXPPROMPT_LOC "Expression/Prompt"
#DEFINE T_TYPE_LOC "Type"
#DEFINE T_VARIABLE_LOC "Variable"
#DEFINE T_STAT_LOC "Stat"
#DEFINE T_FONT_LOC "Font"
#DEFINE T_FONT1_LOC "Font..."
#DEFINE T_OK_LOC "OK"
#DEFINE T_CANCEL_LOC "Cancel"
#DEFINE T_TRANSOPEN_LOC "\!Transport & Open;Open As Is;\?Cancel"
#DEFINE T_TRANSOPEN1_LOC "\!Transport & Open;\?Cancel"
#DEFINE T_TRANSOPEN2_LOC "Transport & Open"

#DEFINE T_NOSTAND_LOC	"The Transporter cannot be run as a standalone program."
#DEFINE T_INVALIDSCR_LOC	"Invalid screen/report name."
#DEFINE T_WHEREIS_LOC	"Where is "
#DEFINE T_CONVFRX_LOC	"You must convert this file to an .FRX file before you"+CHR(13)+ ;
						"can use it. Use the Catalog Manager to open the file,"+CHR(13)+ ;
						"or choose Convert dBASE files from the Run menu."
#DEFINE T_UNKNOWNFRX_LOC	"Unknown report format"
#DEFINE T_TRANSNOTHING_LOC "The transporter has nothing to do." 
#DEFINE T_INVFILEFORMAT_LOC	"Unknown or invalid file format"
#DEFINE T_NORECS_LOC	"No records to transport"
#DEFINE T_COMVPRMPT1_LOC	"Convert 1.02 report file to 2.6 format?"
#DEFINE T_COMVPRMPT2_LOC	"Convert FoxBASE+/dBASE III report file to FoxPro 2.6 format?"
#DEFINE T_COMVPRMPT3_LOC	"Convert 1.02 label file to 2.6 format?"
#DEFINE T_COMVPRMPT4_LOC	"Convert FoxBASE+/dBASE III label file to FoxPro 2.6 format?"
#DEFINE T_COMVPRMPT5_LOC	"Convert 2.0 screen file to 2.6 format?"
#DEFINE T_COMVPRMPT6_LOC	"Convert 2.0 report file to 2.6 format?"
#DEFINE T_COMVPRMPT7_LOC	"Convert 2.0 project file to 2.6 format?"
#DEFINE T_UNKOPERATION_LOC	"Unknown doupdate operation"
#DEFINE T_UNKFOXVER_LOC		"Unknown Version of FoxPro."
#DEFINE T_SAMELINE_LOC		"To and from platforms are the same in line "
#DEFINE T_NOOPENREPT_LOC	"Could not open FoxBASE+ report form"
#DEFINE T_TOTAL1_LOC	"*** Total ***"
#DEFINE T_YESNO_LOC		"\<Yes;\!\?\<No"

#DEFINE T_THAN_LOC		"Than "
#DEFINE T_EQIVOBJS_LOC	" Equivalent Objects" 
#DEFINE	T_OBJSNEWTO_LOC	"Objects New to "
#DEFINE	T_OBJINFILE_LOC "for objects in the file."
#DEFINE T_THERMSCR_LOC	" screen: "
#DEFINE T_THERMRPT_LOC	" report: "
#DEFINE T_THERMLBL_LOC	" label: "
#DEFINE T_ASSERTFAIL_LOC	"Assertion failed: "
#DEFINE T_THANMAC_LOC	"Than Macinstosh Equivalent Objects"