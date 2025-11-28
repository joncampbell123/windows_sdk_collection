*:*****************************************************************************
*:
*: Procedure file: TRANSPRT.PRG
*:         System: FoxPro 2.6 Transporter
*:         Author: Microsoft Corp.
*:*****************************************************************************
*
* TRANSPORT - FoxPro screen, report and label conversion utility.
*
*:*****************************************************************************
* Copyright (c) 1993-95 Microsoft Corp.
* One Microsoft Way
* Redmond, WA 98052
*
* Notes:
* In this program, for clarity/readability reasons, we use variable
* names that are longer than 10 characters.  Note, however, that only
* the first 10 characters are significant.
*
*
* Revision History:
* First written by Matt Pohle, John Beaver and Walt Kennamer for FoxPro 2.5
*

#INCLUDE transprt.h

PROCEDURE transprt
PARAMETER m.g_scrndbf, m.tp_filetype, m.dummy, m.gAShowMe, m.gOTherm, m.cRealName, m.lPJX
*-
*- NOTE: gOTherm is a global object created in CONVERT.PRG. It must be present
*- for the transporter to work properly
*-
* "g_scrndbf" is the name of the file to transport.  It will usually be in some sort
* of database format (e.g., SCX/PJX/MNX) but might also be a FoxBASE+ or FoxPro 1.02
* report or label file, which is not a database.
*
* "tp_filetype" specifies what kind of file "g_scrndbf" is.  Allowable values are
* found in the #DEFINE constants immediately below.  Note that the Transporter usually
* does not use this value and instead figures out what kind of file it is being
* presented with by counting the fields in the database.  For FoxBASE+ and FoxPro 1.02 files,
* however, the Transporter does use this parameter to convert the report or label
* data into 2.0 database format before transporting to Windows.  Note that the FoxBASE+
* types are never actually passed in m.tp_filetype.  They are inferred in GetOldReportType
* and GetOldLabelTypefrom the ID byte in the report/label files.

* The "dummy" parameter is not used.  At one point in the developement of the Transporter,
* another parameter was passed.

* gAShowMe is an array of logical values that remember for which file types
* the transporter dialog should be shown.

* If gAShowMe[n,1] for a particular filetype is .F., the Transporter does not display its 
* dialogs and assumes default values.  
*
* Only the main transporter dialogs are suppressed, so this is not a general
* mechanism for skipping all the dialogs, especially those that are displayed for
* projects, FoxBASE+ and early versions of FoxPro files. Further, the thermometer
* is still displayed.
*
* gAShowMe[n,1] controls whether or not the transporter
* dialog is shown for that particular file type. This value can be switched by the user
* via radio buttons in the transporter dialog. gAShowMe[n,2] determines if the file
*- is to be transported (1), or used "as is". gAShowMe[n,3] remembers the chosen font (jd 3/13/95)

* added gOTherm. If object, will try, will call an update method for thermometer, instead
* of using transporter's thermometer (jd 2/2/95)

*- there is now a transprt.h file, which includes all of the #DEFINES
*- also, text strings are now localizable (11/1/95 jd)

*- 3.0 FRX files being moved from one platform to another (e.g., Mac -> Win) 
*- are now run through the converter, since they are essentially 2.6 files with an extra
*- field and some new objects (11/1/95 jd)

PRIVATE ALL EXCEPT gopjx

IF SET("TALK") = "ON"
   SET TALK OFF
   m.talkset = "ON"
ELSE
   m.talkset = "OFF"
ENDIF
m.pcount = PARAMETERS()

gError = .F.

IF TYPE("gAShowMe[1,1]") # "L"
	RELEASE gAShowMe
	DIMENSION gAShowMe[N_MAXTRANFILETYPES,9]
	*- don't ask for any file type
	LOCAL ictr
	FOR ictr = 1 TO ALEN(gAShowMe,1)
		gAShowMe[ictr,1] = .T.
		gAShowMe[ictr,2] = 1
		gAShowMe[ictr,3] = ""		&& font name
		gAShowMe[ictr,4] = 0		&& font size
		gAShowMe[ictr,5] = ""		&& font style
		gAShowMe[ictr,6] = ""		&& from platform
		gAShowMe[ictr,7] = .T.		&& convert new objects
		gAShowMe[ictr,8] = .T.		&& convert more recently modified objects
		gAShowMe[ictr,9] = .F.		&& replace all objects
	NEXT
ENDIF

DO CASE
CASE _MAC
   m.g_pophght      = 1.500    && popup height
   m.g_vpopup       = 0.750    && vpos adjustment going from DOS to Mac
CASE _WINDOWS
   m.g_pophght      = 1.538
   m.g_vpopup       = 0.906
OTHERWISE
   m.g_pophght      = 3.000
   m.g_vpopup       = 0.906
ENDCASE

IF _MAC
   m.g_pixelsize  = 72       && logical pixels per inch
   m.g_bandheight = ((14/m.g_pixelsize) * 10000)
   m.g_bandfudge  =  3262
ELSE
   m.g_pixelsize  = 96       && logical pixels per inch
   m.g_bandheight = ((19/m.g_pixelsize) * 10000)
   m.g_bandfudge  =  4350
ENDIF
* Used in bandinfo() to adjust band vpos's when transporting to MS-DOS.
* These calculations must match the ones immediately above.
m.g_macbandheight = ((14/72) * 10000)
m.g_winbandheight = ((19/96) * 10000)


* Check mark for selecting items to be transported
IF _MAC
   m.g_checkmark = "X"
ELSE
   m.g_checkmark = 'û'
ENDIF


PUSH KEY CLEAR

*
* Declare Environment Variables so that they are visible throughout the program
*
STORE "" TO m.cursor, m.consol, m.bell, m.exact, m.escape, m.onescape, m.safety, ;
   m.fixed, m.print, m.unqset, m.udfparms, m.exclusive, m.onerror, ;
   m.trbetween, m.comp, m.device, m.status, m.g_fromplatform, m.choice, ;
   m.g_fromobjonlyalias, m.g_boxeditemsalias, m.g_tempalias, m.mtopic, m.rbord, m.mcollate, ;
   m.mmacdesk, m.fields, mfieldsto
STORE 0 TO m.deci, m.memowidth, m.currarea
STORE .F. to m.g_char2grph, m.g_grph2char, m.g_grph2grph, m.g_char2char

*- index for gAShowMe
m.g_tpFileIndx = 1

DO setall

m.g_look2d           = .F.  && are buttons 2D or 3D?

m.g_filetype         =  0  && screen, report, label, etc.

* Set up these variables for scoping reasons here.  SetCtrl assigns them
* their real values.
m.g_ctrlfface        = ""
m.g_ctrlfsize        = 0
m.g_ctrlfstyle       = ""
m.g_windfface        = ""
m.g_windfsize        = 0
m.g_windfstyle       = ""
m.g_winbtnheight     = 0
m.g_macbtnheight     = 0
m.g_macbtnface       = ""
m.g_macbtnsize       = 0
m.g_macbtnstyle      = ""
m.g_winbtnface       = ""
m.g_winbtnsize       = 0
m.g_winbtnstyle      = ""
m.g_btnheight        = 0   && default btn height for the current platform

m.g_dfltfface        = ""
m.g_dfltfsize        = 0
m.g_dfltfstyle         = ""
m.g_thermface        = ""
m.g_thermsize        = 0
m.g_thermstyle         = ""

* These fonts are not necessarily used in the report, but their cxChar and
* cyChar are somewhat larger than the ones that are used.  This provides a
* "fudge factor" to make sure the fields are wide and tall enough.
IF _MAC
   m.g_rptfface            = "Courier"
   m.g_rptfsize            = 13
   m.g_rptfstyle           = 0
   m.g_rpttxtfontstyle     = ""
ELSE
   m.g_rptfface            = "Courier"
   m.g_rptfsize            = 10
   m.g_rptfstyle           = 0
   m.g_rpttxtfontstyle     = ""
ENDIF
DO CASE
CASE _WINDOWS
   m.g_rptlinesize      = (FONTMETRIC(1, m.g_rptfface, m.g_rptfsize, m.g_rpttxtfontstyle) / m.g_pixelsize) * 10000
   m.g_rptcharsize      = (FONTMETRIC(6, m.g_rptfface, m.g_rptfsize, m.g_rpttxtfontstyle) / m.g_pixelsize) * 10000
CASE _MAC
   * This factor is based on a cyChar of 13 for Geneva, 10 (Bold and regular)
   * No fudge factor needed for cyChar.
   m.g_rptlinesize      = (13/72) * 10000
   * This factor is based on a cxChar of 7 for Geneva, 10 Bold,
   * 72 pixels per inch for the Mac, and a 20% fudge factor.
   m.g_rptcharsize      = ((7/72)  * 10000) * 1.2
ENDCASE

DO setctrl   && set control/window measurement fonts, button height, etc.

* Font style for Transporter dialogs--not the converted screens, but the
* dialogs in the Transporter itself.
IF _MAC
   m.g_tdlgface   = "Geneva"
   m.g_tdlgsize   = 10.000
   m.g_tdlgstyle  = "BT"
   m.g_tdlgsty1   = "B"
   m.g_tdlgsty2   = ""
   m.g_tdlgbtn    = 1.500	    && button height

   m.g_smface     = "Geneva"   && small font
   m.g_smsize     = 10
   m.g_smstyle    = "T"
   m.g_smsty1     = ""
ELSE
   m.g_tdlgface   = "MS Sans Serif"
   m.g_tdlgsize   = 8.000
   m.g_tdlgstyle  = "BT"
   m.g_tdlgsty1   = "BO"
   m.g_tdlgsty2   = ""
   m.g_tdlgbtn    = 1.769

   m.g_smface   = "MS Sans Serif"
   m.g_smsize   = 8.000
   m.g_smstyle  = "BT"
   m.g_smsty1   = "BO"
ENDIF

m.g_fontset          = .F.      && default font changed?

* Font for object selection list
IF _MAC
   m.g_foxfont          = "Courier"
   m.g_foxfsize         = 10
ELSE
   m.g_foxfont          = "Foxfont"
   m.g_foxfsize         = 9
ENDIF
m.g_normstylenum        = 0
m.g_boldstylenum        = 1

m.g_fromplatform     = " "
m.g_toplatform       = " "
m.g_windheight       = 1
m.g_windwidth        = 1
m.g_thermwidth       = 0
m.g_mercury          = 0
m.g_20alias          = ""
m.g_status           = 0    && records error status
m.g_energize         = .F.  && does button say "Energize?"
m.g_norepeat         = .F.

m.g_allobjects       = .T.  && what objects are we transporting?
m.g_newobjects       = .T.
m.g_snippets         = .T.
m.g_newobjmode       = .F.

m.g_scrnalias        = ""
m.g_updenviron       = .F.  && have we transported the environment records?
m.g_tpselcnt         = 0    && number of entries in the tparray selection array

m.g_boxstrg = ['Ä','Ä','³','³','Ú','¿','À','Ù','Ä','Ä','³','³','Ú','¿','À','Ù']

m.g_returncode       = c_cancel

* Code pages we're translating to/from.
m.g_tocodepage       = 0
m.g_fromcodepage     = 0

*- index for gAShowMe
m.g_tpFileIndx = 1

* Dimension the array of records to be transported.  This is the picklist of new and
* updated objects.
DIMENSION tparray[1,2]

DIMENSION g_lastobjectline[2]
g_lastobjectline = 0
m.g_tempindex = "S" + SUBSTR(LOWER(SYS(3)),2,8) + ".cdx"

m.onerror = ON("ERROR")
ON ERROR DO errorhandler WITH MESSAGE(), LINENO(), c_error3

IF m.pcount < 2
   DO ErrorHandler WITH T_NOSTAND_LOC,LINENO(),c_error3
   RETURN
ENDIF

* Record fonts available on the current platform
DIMENSION g_fontavail[1]
=afont(g_fontavail)

DIMENSION g_fontmap[c_mapfonts,6]
DO initfontmap   && initialize font mapping array

*
* Make sure we have a file name we can deal with.  Prompt if the file cannot be found.
*
IF TYPE("m.g_scrndbf") != "C"
   m.g_scrndbf = ""
   DO assert WITH .T., T_INVALIDSCR_LOC
ENDIF
m.g_scrndbf = UPPER(ALLTRIM(m.g_scrndbf))
DO CASE
CASE SUBSTR(m.g_scrndbf, RAT(".", m.g_scrndbf)+1, 3) = "SCX"
   IF !FILE(m.g_scrndbf)
      m.g_scrndbf = GETFILE("SCX", T_WHEREIS_LOC+strippath(m.g_scrndbf))
   ENDIF
CASE SUBSTR(m.g_scrndbf, RAT(".", m.g_scrndbf)+1, 3) = "FRX"
   IF !FILE(m.g_scrndbf)
      m.g_scrndbf = GETFILE("FRX", T_WHEREIS_LOC+strippath(m.g_scrndbf))
   ENDIF
CASE SUBSTR(m.g_scrndbf, RAT(".", m.g_scrndbf)+1, 3) = "LBX"
   IF !FILE(m.g_scrndbf)
      m.g_scrndbf = GETFILE("LBX", T_WHEREIS_LOC+strippath(m.g_scrndbf))
   ENDIF
CASE SUBSTR(m.g_scrndbf, RAT(".", m.g_scrndbf)+1, 3) = "PJX"
   IF !FILE(m.g_scrndbf)
      m.g_scrndbf = GETFILE("PJX", T_WHEREIS_LOC+strippath(m.g_scrndbf))
   ENDIF
OTHERWISE
   IF !FILE(m.g_scrndbf)
      m.g_scrndbf = GETFILE("SCX|FRX|LBX|PJX", T_SELTRANS_LOC,T_TRANSPORT_LOC)
   ENDIF
ENDCASE

IF !FILE(m.g_scrndbf) OR EMPTY(m.g_scrndbf)
   DO cleanup
   RETURN .F.
ENDIF

DO putwinmsg WITH T_TITLE_LOC +": " + LOWER(strippath(m.cRealName))

DO setversion  WITH g_toplatform

m.g_tocodepage = settocp()  && based on runtime platform

*- set index for which filetype
DO CASE
	CASE INLIST(m.tp_filetype,c_25scxtype,c_20scxtype)
		m.g_tpFileIndx = N_TRANFILE_SCX
	CASE INLIST(m.tp_filetype,c_25frxtype,c_20frxtype,c_25lbxtype,c_20lbxtype,c_frx102modi,c_frx102repo,c_lbx102modi, c_lbx102repo)
		m.g_tpFileIndx = N_TRANFILE_FRX
	CASE INLIST(m.tp_filetype,c_20pjxtype)
		m.g_tpFileIndx = N_TRANFILE_PJX
	OTHERWISE
		m.g_tpFileIndx = N_TRANFILE_SCX
ENDCASE

*- added mac case (jd 11/13/95)
IF !EMPTY(gAShowMe[m.g_tpFileIndx,3])
   m.g_dfltfface = m.gAShowMe[m.g_tpFileIndx,3]
   m.g_dfltfsize = m.gAShowMe[m.g_tpFileIndx,4]
   m.g_dfltfstyle = m.gAShowMe[m.g_tpFileIndx,5]
ELSE
	DO CASE
		CASE _windows
			m.g_dfltfface  = "MS Sans Serif"
			m.g_dfltfsize  = 8
			m.g_dfltfstyle = "B"
		CASE _mac
			m.g_dfltfface  = "Geneva"
			m.g_dfltfsize  = 10
			m.g_dfltfstyle = "N"
	ENDCASE
ENDIF

* If we've been passed an old format report or label form, see if it is a FoxPro 1.02
* form, a FoxBASE+ form, or an unknown form.
* Convert FoxPro 1.02 or FoxBASE+ DOS reports into 2.5/2.6 DOS reports
IF INLIST(m.tp_filetype,c_frx102modi,c_frx102repo,c_lbx102modi, c_lbx102repo)

   IF INLIST(m.tp_filetype,c_frx102modi,c_frx102repo)
      m.tp_filetype = getoldreporttype()   && FoxPro 1.02 or FoxBASE+ report?
   ELSE
      m.tp_filetype = getoldlabeltype()    && FoxPro 1.02 or FoxBASE+ label?
   ENDIF

   m.g_fromcodepage = c_doscp

   IF doupdate()           && prompt to convert to 2.5 format; sets m.g_filetype
      DO CASE
      CASE INLIST(m.tp_filetype,c_frx102modi,c_frx102repo)
         * FoxPro 1.02 report
         m.g_scrndbf = cvrt102frx(m.g_scrndbf, m.tp_filetype)
      CASE INLIST(m.tp_filetype,c_fbprptmodi,c_fbprptrepo)
         * FoxBASE+ report
         m.g_scrndbf = cvrtfbprpt(m.g_scrndbf, m.tp_filetype)
      CASE INLIST(m.tp_filetype,c_lbx102modi,c_lbx102repo)
         * FoxPro 1.02 label
         m.g_scrndbf = cvrt102lbx(m.g_scrndbf, m.tp_filetype)
      CASE INLIST(m.tp_filetype,c_fbplblmodi,c_fbplblrepo)
         * FoxBASE+ label
         m.g_scrndbf = cvrtfbplbl(m.g_scrndbf, m.tp_filetype)
		CASE m.tp_filetype = c_db4type
			WAIT WINDOW T_CONVFRX_LOC NOWAIT
			DO cleanup WITH .T.
      OTHERWISE
         DO errorhandler WITH T_UNKNOWNFRX_LOC,LINENO(),c_error3
      ENDCASE
   ELSE
      DO cleanup
      RETURN c_cancel
   ENDIF
ENDIF

* Open the screen/report/label/project file
IF !opendbf(m.g_scrndbf)
   m.g_returncode = c_cancel
ENDIF

*
* We have three basic conversion cases.  These are transporting a 2.0 file to a
* graphical 2.5 platform (structure change and conversion), converting a 2.0 file
* to a character 2.5 platform (structure change) and transporting a 2.5 platform
* to another 2.5 platform (character/graphical conversion).  This case statement
* calls the appropriate dialog routines and makes sure we have done all the
* preparation (like creating the cursor we actually work with.)
*
* The 1.02 and FoxBASE+ reports/labels are handled in basically the same way.
* They get their own cases in this construct since we don't want to prompt the
* user twice for conversion.  Almost all of the actual conversion of these files
* has already taken place, in the "cvrt102frx" procedure (and related procedures)
* called above.
*
* Conversion of 2.0 project files is handled in its own case also.
*
DO CASE
CASE INLIST(m.tp_filetype,c_frx102repo,c_fbprptrepo,c_lbx102repo,c_fbplblrepo) ;
       AND (_WINDOWS OR _MAC)
   * FoxPro 1.02 or FoxBASE+ report/label opened via REPORT/LABEL FORM.  At this point,
   * we've already converted the old format form into FoxPro 2.5 DOS format.
   * Finish conversion, but don't transport it to Windows.
   m.g_fromplatform = c_dosname
   m.g_fromcodepage = setfromcp(m.g_fromplatform)
   m.g_returncode = c_yes
   DO starttherm WITH c_converting,g_filetype
   DO putwinmsg WITH c_converting + " " + LOWER(strippath(m.cRealName))
   DO converter

CASE INLIST(m.tp_filetype,c_frx102modi,c_fbprptmodi,c_lbx102modi,c_fbplblmodi) ;
       AND (_WINDOWS OR _MAC)
   * FoxPro 1.02 or FoxBASE+ report/label opened via MODIFY REPORT/LABEL. At this point,
   *  we've already converted the old format form into FoxPro 2.5 DOS format.
   * Finish conversion, and then transport it to Windows.
   m.g_fromplatform = c_dosname
   m.g_fromcodepage = setfromcp(m.g_fromplatform)
   m.g_returncode = c_yes
   DO putwinmsg WITH c_converting + " " + LOWER(strippath(m.cRealName))
   DO converter
   DO putwinmsg WITH c_transporting + " " + LOWER(strippath(m.cRealName))
   DO import
   DO synchtime WITH m.g_toplatform, m.g_fromplatform

CASE ((FCOUNT() = c_20scxfld OR FCOUNT() = c_20frxfld OR FCOUNT() = c_20lbxfld);
      AND (_DOS OR _UNIX))
   * Convert it to a DOS report, but don't transport it to Windows
   DO CASE
   CASE !doupdate()  && displays dialog and sets g_toPlatform
      m.g_returncode = c_cancel
   OTHERWISE
      m.g_fromplatform = c_dosname
      m.g_fromcodepage = setfromcp(m.g_fromplatform)
      m.g_returncode = c_yes
      DO starttherm WITH c_converting,g_filetype
      DO converter
   ENDCASE

CASE (FCOUNT() = c_20scxfld OR FCOUNT() = c_20frxfld ;
      OR FCOUNT() = c_20lbxfld) AND (_WINDOWS OR _MAC)

   * Convert it to DOS and then transport it to Windows
   m.choice = converttype(.T.)
   m.g_fromcodepage = setfromcp(m.g_fromplatform)

   *- added this (jd 2/2/95)
   m.g_fromplatform = c_dosname

   DO CASE
   CASE m.choice = c_yes
      m.g_returncode = c_yes
      DO converter
      DO import
      DO synchtime WITH m.g_toplatform, m.g_fromplatform
   CASE m.choice = c_no
      m.g_returncode = c_no

   OTHERWISE
      m.g_returncode = c_cancel
   ENDCASE

*- support 3.0 FRX file (11/1/95 jd)
CASE FCOUNT() = c_scxfld OR FCOUNT() = c_frxfld OR FCOUNT() = c_frx30fld
   m.choice = converttype(.F.)
   DO CASE
   CASE m.choice = c_yes
      m.g_returncode = c_yes
      DO makecursor
      DO import
      IF m.g_returncode <> c_cancel
         * This might happen if the user picked "Cancel" on the screen that lets
         * him/her uncheck specific items.
         SELECT (m.g_scrnalias)
         DO synchtime WITH m.g_toplatform, m.g_fromplatform
      ENDIF
   CASE m.choice = c_no
      m.g_returncode = c_no

   OTHERWISE
      m.g_returncode = c_cancel
   ENDCASE
CASE FCOUNT() = c_20pjxfld
   IF versnum() > "2.5"
      * Identify fields that contain binary data.  These should not be codepage-translated.
      * Note that files opened via low level routines (e.g., FoxPro 1.02 reports) will not
      * be codepage-translated automatically.  Strings in those files that require codepage
      * translation will be codepage translated explicitly below.
      SET NOCPTRANS TO arranged, object, symbols, devinfo
   ENDIF

   * Converting a 2.0 project to 2.5 format
   IF !doupdate()                 && displays dialog and sets g_toPlatform
      m.g_returncode = c_cancel
   ELSE
      m.g_fromplatform = c_dosname
      m.g_fromcodepage = setfromcp(m.g_fromplatform)
      m.g_returncode = c_yes
      DO putwinmsg WITH c_converting + " " + LOWER(strippath(m.cRealName))
      DO starttherm WITH c_converting,g_filetype
      DO converter
   ENDIF
CASE FCOUNT() = c_pjxfld
   * 2.5 project passed to us by mistake--shouldn't ever happen.
   WAIT WINDOW T_TRANSNOTHING_LOC NOWAIT
   m.g_returncode = c_cancel
OTHERWISE
   DO errorhandler WITH T_INVFILEFORMAT_LOC, LINENO(), c_error3
   m.g_returncode = c_cancel
ENDCASE

DO cleanup

RETURN m.g_returncode

*!*****************************************************************************
*!
*!       Function: OPENDBF
*!
*!      Called by: TRANSPRT.PRG
*!
*!*****************************************************************************
FUNCTION opendbf
PARAMETER fname
m.g_scrnalias = "S"+SUBSTR(LOWER(SYS(3)),2,8)
SELECT 0
USE (m.fname) AGAIN ALIAS (m.g_scrnalias)
IF RECCOUNT() = 0
   WAIT WINDOW T_NORECS_LOC NOWAIT
   RETURN .F.
ENDIF
RETURN .T.

*
* doupdate - Ask the user if a 2.0 screen/report/label should be updated to 2.5 format.
*
*!*****************************************************************************
*!
*!       Function: DOUPDATE
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: STRUCTDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION doupdate
PRIVATE m.result

DO CASE
CASE INLIST(m.tp_filetype,c_frx102modi, c_frx102repo)
   m.g_filetype = c_report
   m.result = structdialog(T_COMVPRMPT1_LOC) &&"Convert 1.02 report file to 2.6 format?"

CASE INLIST(m.tp_filetype,c_fbprptmodi, c_fbprptrepo)
   m.g_filetype = c_report
   m.result = structdialog(T_COMVPRMPT2_LOC) &&"Convert FoxBASE+/dBASE III report file to FoxPro 2.6 format?"

CASE INLIST(m.tp_filetype,c_lbx102modi, c_lbx102repo)
   m.g_filetype = c_label
   m.result = structdialog(T_COMVPRMPT3_LOC) &&"Convert 1.02 label file to 2.6 format?"

CASE INLIST(m.tp_filetype,c_fbplblmodi, c_fbplblrepo)
   m.g_filetype = c_label
   m.result = structdialog(T_COMVPRMPT4_LOC) &&"Convert FoxBASE+/dBASE III label file to FoxPro 2.6 format?"

CASE FCOUNT() = c_20scxfld
   m.g_filetype = c_screen
   m.result = structdialog(T_COMVPRMPT5_LOC) &&"Convert 2.0 screen file to 2.6 format?"

CASE FCOUNT() = c_20frxfld
   m.g_filetype = c_report
   m.result = structdialog(T_COMVPRMPT6_LOC) &&"Convert 2.0 report file to 2.6 format?"

CASE FCOUNT() = c_20lbxfld
   RETURN .F.

CASE FCOUNT() = c_20pjxfld
   m.g_filetype = c_project
   m.result = structdialog(T_COMVPRMPT7_LOC) &&"Convert 2.0 project file to 2.6 format?"
CASE m.tp_filetype = c_db4type
	m.result = .T.

OTHERWISE
   DO errorhandler WITH T_UNKOPERATION_LOC, LINENO(), c_error3
ENDCASE

RETURN m.result

*
* converttype - Display the dialog used when converting between 2.5 platforms
*
*!*****************************************************************************
*!
*!       Function: CONVERTTYPE
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: CLEANUP            (procedure in TRANSPRT.PRG)
*!               : SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
FUNCTION converttype
PARAMETER m.twooh
PRIVATE m.i, m.pcount, m.nplatforms

IF m.twooh  && If it's a 2.0 file, there is only one platform to convert from.
   DIMENSION platforms[1]
   platforms[1] = c_foxdos_loc

   DO CASE                           && Remember the type of file we are converting
   CASE INLIST(m.tp_filetype,c_frx102modi,c_frx102repo,c_fbprptmodi,c_fbprptrepo)
      m.g_filetype = c_report

   CASE FCOUNT() = c_20scxfld
      m.g_filetype = c_screen

   CASE FCOUNT() = c_20frxfld
      m.g_filetype = c_report

   CASE FCOUNT() = c_20lbxfld
      m.g_filetype = c_label

   CASE FCOUNT() = c_20pjxfld
      m.g_filetype = c_project
   ENDCASE
ELSE
   IF FCOUNT() = c_scxfld                && Remember the type of file we are converting
      m.g_filetype = c_screen
   ELSE
      IF UPPER(RIGHT(m.g_scrndbf, 4)) = ".LBX"
         LOCATE FOR objtype = c_ot20label OR ;
            ((platform = c_winname OR platform = c_macname) AND ;
            objtype = c_otheader AND BOTTOM)
         IF FOUND()
            m.g_filetype = c_label
         ELSE
            m.g_filetype = c_report
         ENDIF
      ELSE
         m.g_filetype = c_report
      ENDIF
   ENDIF

   * See if this file has the special warning the Mac writes to reports
	IF m.g_filetype = c_report
	   LOCATE FOR platform = "WINDOWS" AND iserrormsg(expr)
		IF FOUND()
			GOTO TOP
			LOCATE FOR platform = "WINDOWS"
			DELETE WHILE platform = "WINDOWS"
			PACK
		ENDIF
		GOTO TOP
	ENDIF

   * Get a list of the platforms in this file.
   SELECT DISTINCT platform ;
      FROM (m.g_scrnalias) ;
      WHERE !DELETED() ;
      INTO ARRAY availplatforms
   m.nplatforms = _TALLY
   m.pcount = 0

   IF m.nplatforms > 0
      m.g_fromplatform = availplatforms[1]

      FOR i = 1 TO m.nplatforms
         DO CASE
         CASE ATC('DOS',availplatforms[m.i]) > 0 AND !_DOS
            m.pcount = m.pcount + 1

         CASE ATC('WINDOWS',availplatforms[m.i]) > 0 AND !_WINDOWS
            m.pcount = m.pcount + 1

         CASE ATC('UNIX',availplatforms[m.i]) > 0 AND !_UNIX
            m.pcount = m.pcount + 1

         CASE ATC('MAC',availplatforms[m.i]) > 0 AND !_MAC
            m.pcount = m.pcount + 1
         ENDCASE
      ENDFOR
      RELEASE availplatforms
   ENDIF

   IF m.nplatforms = 0 OR m.pcount = 0     && There isn't anything to convert from.
      WAIT WINDOW T_TRANSNOTHING_LOC  NOWAIT
      DO cleanup
      RETURN c_cancel
   ENDIF
ENDIF

*   Call the dialog routine appropriate to this file type.
DO CASE                        && Ask the user what we should do.
CASE m.g_filetype = c_screen
   RETURN scxfrxdialog("SCX")
CASE m.g_filetype = c_report
   DO setrptfont
   RETURN scxfrxdialog("FRX")
CASE m.g_filetype = c_label
   DO setrptfont
   RETURN scxfrxdialog("LBX")
ENDCASE
RETURN c_cancel

*
* setversion - set global variable m.g_toPlatform with the name of the platform
*            we are running on.
*
*!*****************************************************************************
*!
*!      Procedure: SETVERSION
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: ERRORHANDLER       (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE setversion
PARAMETER m.to
DO CASE
CASE _WINDOWS
   m.to = c_winname
CASE _MAC
   m.to = c_macname
CASE _UNIX
   m.to = c_unixname
CASE _DOS
   m.to = c_dosname
OTHERWISE
   DO errorhandler WITH T_UNKFOXVER_LOC, LINENO(), c_error3
ENDCASE
*!*****************************************************************************
*!
*!      Procedure: settocp
*!
*!*****************************************************************************
PROCEDURE settocp
DO CASE
CASE _WINDOWS
   RETURN c_wincp
CASE _MAC
   RETURN c_maccp
CASE _UNIX
   RETURN c_unixcp
CASE _DOS
   RETURN c_doscp
OTHERWISE
   DO errorhandler WITH T_UNKFOXVER_LOC, LINENO(), c_error3
ENDCASE

*
* import - Do the import.
*
*!*****************************************************************************
*!
*!      Procedure: IMPORT
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: EMPTYPLATFORM()    (function  in TRANSPRT.PRG)
*!               : GETCHARSUPPRESS()  (function  in TRANSPRT.PRG)
*!               : CHARTOGRAPHIC      (procedure in TRANSPRT.PRG)
*!               : GRAPHICTOCHAR      (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
PROCEDURE import

IF m.g_fromplatform = m.g_toplatform
   * This shouldn't be possible
   DO assert WITH .T.,T_SAMELINE_LOC+TRIM(STR(LINENO()))
   RETURN
ELSE
   *   If we are converting everything, remove all records for the target
   *   platform.
   IF m.g_allobjects AND !emptyplatform(m.g_toplatform)
      * We need to copy the records we want to a temporary file, clear our cursor
      * and copy the records back since you can't pack a cursor and SELECT creates
      * a read only cursor.
		LOCAL cOldCPTrans

      m.g_tempalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
      SELECT * FROM (m.g_scrnalias) ;
         WHERE !DELETED() AND platform <> m.g_toplatform ;
         INTO TABLE (m.g_tempalias)

		cOldCPTrans = SET("NOCPTRANS")
		SET NOCPTRANS TO tag, tag2
		SELECT (m.g_scrnalias)
		ZAP

		APPEND FROM (m.g_tempalias)
		SET NOCPTRANS TO &cOldCPTrans
      SELECT (m.g_tempalias)
      USE
      DELETE FILE (m.g_tempalias+".dbf")
      DELETE FILE (m.g_tempalias+".fpt")
      SELECT (m.g_scrnalias)
   ENDIF

   IF !g_allobjects AND emptyplatform(m.g_toplatform)
	*- there are no records for the "to" platform, so force all objects (jd 5/20/95)
	m.g_allobjects = .T.
   ENDIF

   m.g_char2grph =  (m.g_toplatform = 'WINDOWS' OR m.g_toplatform = 'MAC') AND ;
      (m.g_fromplatform = 'DOS' OR m.g_fromplatform = 'UNIX')
   m.g_grph2grph =  (m.g_toplatform = 'WINDOWS' OR m.g_toplatform = 'MAC') AND ;
          (m.g_fromplatform = 'WINDOWS' OR m.g_fromplatform = 'MAC')
   m.g_grph2char =  (m.g_toplatform = 'DOS' OR m.g_toplatform = 'UNIX') AND ;
      (m.g_fromplatform = 'WINDOWS' OR m.g_fromplatform = 'MAC')
   m.g_char2char =  (m.g_toplatform = 'DOS' OR m.g_toplatform = 'UNIX') AND ;
      (m.g_fromplatform = 'DOS' OR m.g_fromplatform = 'UNIX')
ENDIF

IF g_filetype = c_report
   m.g_norepeat = getcharsuppress()
ENDIF

*  Pass control to the control routine appropriate for the direction we are converting.
DO CASE
CASE m.g_char2grph
   DO chartographic
CASE m.g_grph2char
   DO graphictochar
CASE m.g_grph2grph
   DO graphictographic
ENDCASE
RETURN

*
* GraphicToChar - Converts everything, new objects or changed snippets from a grpahical
*      platform to a character platform.
*
*!*****************************************************************************
*!
*!      Procedure: GRAPHICTOCHAR
*!
*!      Called by: IMPORT             (procedure in TRANSPRT.PRG)
*!
*!          Calls: ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : SELECTOBJ          (procedure in TRANSPRT.PRG)
*!               : STARTTHERM         (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : UPDATESCREEN       (procedure in TRANSPRT.PRG)
*!               : UPDATEREPORT       (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE graphictochar
IF m.g_allobjects
   *  Start the thermometer with the appropriate message.
   DO starttherm WITH c_transporting,m.g_filetype

   DO allgraphictochar
ELSE
   * Do a partial conversion, unless we're dealing with a label
   IF m.g_filetype = c_label      && We only do complete label conversion
      RETURN
   ENDIF

   DO selectobj   && figure out which ones to transport

   *  Start the thermometer with the appropriate message.
   DO starttherm WITH c_transporting,m.g_filetype

   m.g_mercury = 5
   DO updtherm WITH m.g_mercury

   DO putwinmsg WITH c_transporting + " " + LOWER(strippath(m.cRealName))

   SELECT (m.g_scrnalias)

   IF m.g_snippets
      IF m.g_filetype = c_screen
         DO updatescreen
      ELSE
         DO updatereport
      ENDIF
   ENDIF
   IF m.g_newobjects
      DO newgraphictochar
   ENDIF
ENDIF

*
* CharToGraphic - Converts everything, new objects or changed snippets from a character
*      platform to a graphical platform.
*
*!*****************************************************************************
*!
*!      Procedure: CHARTOGRAPHIC
*!
*!      Called by: IMPORT             (procedure in TRANSPRT.PRG)
*!
*!          Calls: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : SELECTOBJ          (procedure in TRANSPRT.PRG)
*!               : STARTTHERM         (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : UPDATESCREEN       (procedure in TRANSPRT.PRG)
*!               : UPDATEREPORT       (procedure in TRANSPRT.PRG)
*!               : NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE chartographic
IF m.g_allobjects
   *  Start the thermometer with the appropriate message.
   DO starttherm WITH c_transporting,m.g_filetype

   DO allchartographic
ELSE
   IF m.g_filetype = c_label      && We only do complete label convertsion
      RETURN
   ENDIF

   DO selectobj   && figure out which ones to transport

   *  Start the thermometer with the appropriate message.
   DO starttherm WITH c_transporting,m.g_filetype

   m.g_mercury = 5
   DO updtherm WITH m.g_mercury

   DO putwinmsg WITH c_transporting + " " + LOWER(strippath(m.cRealName))

   SELECT (m.g_scrnalias)

   IF m.g_snippets
      IF m.g_filetype = c_screen
         DO updatescreen
      ELSE
         DO updatereport
      ENDIF
   ENDIF
   IF m.g_newobjects
      DO newchartographic
   ENDIF
ENDIF
*
* GraphicToGraphic - Converts everything, new objects or changed snippets from a graphic
*      platform to another graphical platform.
*
*!*****************************************************************************
*!
*!      Procedure: GRAPHICOGRAPHIC
*!
*!      Called by: IMPORT             (procedure in TRANSPRT.PRG)
*!
*!          Calls: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : SELECTOBJ          (procedure in TRANSPRT.PRG)
*!               : STARTTHERM         (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : UPDATESCREEN       (procedure in TRANSPRT.PRG)
*!               : UPDATEREPORT       (procedure in TRANSPRT.PRG)
*!               : NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE graphictographic
IF m.g_allobjects
   *  Start the thermometer with the appropriate message.
   DO starttherm WITH c_transporting,m.g_filetype

   DO allgrphtogrph
ELSE
   IF m.g_filetype = c_label      && We only do complete label convertsion
      RETURN
   ENDIF

   DO selectobj   && figure out which ones to transport

   *  Start the thermometer with the appropriate message.
   DO starttherm WITH c_transporting,m.g_filetype

   m.g_mercury = 5
   DO updtherm WITH m.g_mercury

   DO putwinmsg WITH c_transporting + " " + LOWER(strippath(m.cRealName))

   SELECT (m.g_scrnalias)

   IF m.g_snippets
      IF m.g_filetype = c_screen
         DO updatescreen
      ELSE
         DO updatereport
      ENDIF
   ENDIF
   IF m.g_newobjects
      DO newgrphtogrph
   ENDIF
ENDIF

*
* UpdateScreen - Copy any non-platform specific
*
*!*****************************************************************************
*!
*!      Procedure: UPDATESCREEN
*!
*!      Called by: GRAPHICTOCHAR      (procedure in TRANSPRT.PRG)
*!               : CHARTOGRAPHIC      (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETSNIPFLAG()      (function  in TRANSPRT.PRG)
*!               : ISOBJECT()         (function  in TRANSPRT.PRG)
*!               : MAPBUTTON()        (function  in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!        Indexes: ID                     (tag)
*!
*!*****************************************************************************
PROCEDURE updatescreen
PRIVATE m.thermstep

COUNT TO m.thermstep FOR platform = m.g_toplatform
IF m.g_newobjects
   m.thermstep = 40/m.thermstep
ELSE
   m.thermstep = 80/m.thermstep
ENDIF

m.g_tempalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
SELECT * FROM (m.g_scrnalias) ;
   WHERE !DELETED() AND platform = m.g_fromplatform ;
   AND isselected(uniqueid,objtype,objcode) ;
   INTO CURSOR (m.g_tempalias)
INDEX ON uniqueid TAG id

SELECT (m.g_scrnalias)
SET RELATION TO uniqueid INTO (m.g_tempalias) ADDITIVE
LOCATE FOR .T.

SELECT (m.g_scrnalias)

* Check for flag to transport only code snippets
m.sniponly = .F.
LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
IF FOUND()
   m.sniponly = getsnipflag(setupcode)
ENDIF

IF !m.sniponly
   DO updenviron WITH .T.
ENDIF

* Update everything else
SCAN FOR platform = m.g_toplatform AND !DELETED() ;
      AND (INLIST(objtype,C_OBJTYPELIST) OR objtype = c_otheader)
   IF &g_tempalias..timestamp > timestamp
      IF !m.sniponly
         REPLACE name WITH &g_tempalias..name
         REPLACE expr WITH &g_tempalias..expr
         REPLACE STYLE WITH &g_tempalias..style
         IF INLIST(objtype,c_otradbut,c_ottxtbut)
            * Don't zap the whole set of buttons if there are just some new ones
            REPLACE PICTURE WITH mapbutton(&g_tempalias..picture,PICTURE)
         ELSE
            REPLACE PICTURE WITH &g_tempalias..picture
         ENDIF
         IF objtype <> c_otheader OR m.g_grph2char OR EMPTY(order)
            * Icon file name is stored in Windows header, "order" field
            REPLACE ORDER WITH &g_tempalias..order
         ENDIF
         REPLACE unique WITH &g_tempalias..unique
         *REPLACE Environ WITH &g_tempalias..Environ
         REPLACE boxchar WITH &g_tempalias..boxchar
         REPLACE fillchar WITH &g_tempalias..fillchar
         REPLACE TAG WITH &g_tempalias..tag
         REPLACE tag2 WITH &g_tempalias..tag2
         REPLACE ruler WITH &g_tempalias..ruler
         REPLACE rulerlines WITH &g_tempalias..rulerlines
         REPLACE grid WITH &g_tempalias..grid
         REPLACE gridv WITH &g_tempalias..gridv
         REPLACE gridh WITH &g_tempalias..gridh
         REPLACE FLOAT WITH &g_tempalias..float
         REPLACE CLOSE WITH &g_tempalias..close
         REPLACE MINIMIZE WITH &g_tempalias..minimize
         REPLACE BORDER WITH &g_tempalias..border
         REPLACE SHADOW WITH &g_tempalias..shadow
         REPLACE CENTER WITH &g_tempalias..center
         REPLACE REFRESH WITH &g_tempalias..refresh
         REPLACE disabled WITH &g_tempalias..disabled
         REPLACE scrollbar WITH &g_tempalias..scrollbar
         REPLACE addalias WITH &g_tempalias..addalias
         REPLACE TAB WITH &g_tempalias..tab
         REPLACE initialval WITH &g_tempalias..initialval
         REPLACE initialnum WITH &g_tempalias..initialnum
         REPLACE spacing WITH &g_tempalias..spacing
         * Update width if it looks like a text object got longer in Windows
         IF m.g_grph2char AND objtype = c_ottext
            REPLACE width WITH MAX(width,LEN(CHRTRANC(expr,'"'+chr(39),'')))
         ENDIF
      ENDIF
      IF objtype = c_otfield  && watch out for SAYs changing to GETs
         REPLACE objcode WITH &g_tempalias..objcode
      ENDIF
      REPLACE lotype WITH &g_tempalias..lotype
      REPLACE rangelo WITH &g_tempalias..rangelo
      REPLACE hitype WITH &g_tempalias..hitype
      REPLACE rangehi WITH &g_tempalias..rangehi
      REPLACE whentype WITH &g_tempalias..whentype
      REPLACE WHEN WITH &g_tempalias..when
      REPLACE validtype WITH &g_tempalias..validtype
      REPLACE VALID WITH &g_tempalias..valid
      REPLACE errortype WITH &g_tempalias..errortype
      REPLACE ERROR WITH &g_tempalias..error
      REPLACE messtype WITH &g_tempalias..messtype
      REPLACE MESSAGE WITH &g_tempalias..message
      REPLACE showtype WITH &g_tempalias..showtype
      REPLACE SHOW WITH &g_tempalias..show
      REPLACE activtype WITH &g_tempalias..activtype
      REPLACE ACTIVATE WITH &g_tempalias..activate
      REPLACE deacttype WITH &g_tempalias..deacttype
      REPLACE DEACTIVATE WITH &g_tempalias..deactivate
      REPLACE proctype WITH &g_tempalias..proctype
      REPLACE proccode WITH &g_tempalias..proccode
      REPLACE setuptype WITH &g_tempalias..setuptype
      REPLACE setupcode WITH &g_tempalias..setupcode

      REPLACE timestamp WITH &g_tempalias..timestamp
      REPLACE platform WITH m.g_toplatform
   ENDIF

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury

ENDSCAN

SELECT (m.g_tempalias)
USE
SELECT (m.g_scrnalias)

RETURN

*
* UpdateReport - Copy any "non-platform specific" information from one platform to another
*
*!*****************************************************************************
*!
*!      Procedure: UPDATEREPORT
*!
*!      Called by: GRAPHICTOCHAR      (procedure in TRANSPRT.PRG)
*!               : CHARTOGRAPHIC      (procedure in TRANSPRT.PRG)
*!
*!          Calls: ADJRPTSUPPRESS     (procedure in TRANSPRT.PRG)
*!               : ADJRPTFLOAT        (procedure in TRANSPRT.PRG)
*!               : ADJRPTRESET        (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!        Indexes: ID                     (tag)
*!
*!*****************************************************************************
PROCEDURE updatereport
PRIVATE m.thermstep

LOCAL cOldCPTrans
cOldCPTrans = SET("NOCPTRANS")

COUNT TO m.thermstep FOR platform = m.g_toplatform
IF m.g_newobjects
   m.thermstep = 40/m.thermstep
ELSE
   m.thermstep = 80/m.thermstep
ENDIF

m.g_tempalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
SELECT * FROM (m.g_scrnalias) ;
   WHERE platform = m.g_fromplatform AND !DELETED();
   AND isselected(uniqueid,objtype,objcode) ;
   INTO CURSOR (m.g_tempalias)
INDEX ON uniqueid TAG id

SELECT (m.g_scrnalias)
SET RELATION TO uniqueid INTO (m.g_tempalias) ADDITIVE
LOCATE FOR .T.

SELECT (m.g_scrnalias)
SET NOCPTRANS TO tag, tag2
DO updenviron WITH .T.


SCAN FOR platform = m.g_toplatform AND ;
      (objtype = c_otheader OR objtype = c_otfield OR objtype = c_otpicture OR ;
      objtype = c_otrepfld OR objtype = c_otband OR objtype = c_otrepvar OR ;
      objtype = c_ottext OR objtype = c_otline OR objtype = c_otbox) AND !DELETED()
   IF &g_tempalias..timestamp > timestamp
      REPLACE name WITH &g_tempalias..name
      IF objtype = c_otrepvar AND m.g_grph2char
         REPLACE name WITH UPPER(name)
      ENDIF
      REPLACE expr WITH &g_tempalias..expr
      REPLACE STYLE WITH &g_tempalias..style
      REPLACE PICTURE WITH &g_tempalias..picture
      REPLACE ORDER WITH &g_tempalias..order
      REPLACE unique WITH &g_tempalias..unique
      REPLACE ENVIRON WITH &g_tempalias..environ
      REPLACE boxchar WITH &g_tempalias..boxchar
      REPLACE fillchar WITH &g_tempalias..fillchar
      REPLACE TAG WITH &g_tempalias..tag
      REPLACE tag2 WITH &g_tempalias..tag2
      REPLACE mode WITH &g_tempalias..mode
      REPLACE ruler WITH &g_tempalias..ruler
      REPLACE rulerlines WITH &g_tempalias..rulerlines
      REPLACE grid WITH &g_tempalias..grid
      REPLACE gridv WITH &g_tempalias..gridv
      REPLACE gridh WITH &g_tempalias..gridh
      REPLACE FLOAT WITH &g_tempalias..float
      REPLACE STRETCH WITH &g_tempalias..stretch
      REPLACE stretchtop WITH &g_tempalias..stretchtop
      REPLACE TOP WITH &g_tempalias..top
      REPLACE BOTTOM WITH &g_tempalias..bottom
      REPLACE suptype WITH &g_tempalias..suptype
      REPLACE suprest WITH &g_tempalias..suprest
      REPLACE norepeat WITH &g_tempalias..norepeat
      REPLACE resetrpt WITH &g_tempalias..resetrpt
      REPLACE pagebreak WITH &g_tempalias..pagebreak
      REPLACE colbreak WITH &g_tempalias..colbreak
      REPLACE resetpage WITH &g_tempalias..resetpage
      REPLACE GENERAL WITH &g_tempalias..general
      REPLACE spacing WITH &g_tempalias..spacing
      REPLACE DOUBLE WITH &g_tempalias..double
      REPLACE swapheader WITH &g_tempalias..swapheader
      REPLACE swapfooter WITH &g_tempalias..swapfooter
      REPLACE ejectbefor WITH &g_tempalias..ejectbefor
      REPLACE ejectafter WITH &g_tempalias..ejectafter
      REPLACE PLAIN WITH &g_tempalias..plain
      REPLACE SUMMARY WITH &g_tempalias..summary
      REPLACE addalias WITH &g_tempalias..addalias
      REPLACE offset WITH &g_tempalias..offset
      REPLACE topmargin WITH &g_tempalias..topmargin
      REPLACE botmargin WITH &g_tempalias..botmargin
      REPLACE totaltype WITH &g_tempalias..totaltype
      REPLACE resettotal WITH &g_tempalias..resettotal
      REPLACE resoid WITH &g_tempalias..resoid
      REPLACE curpos WITH &g_tempalias..curpos
      REPLACE supalways WITH &g_tempalias..supalways
      REPLACE supovflow WITH &g_tempalias..supovflow
      REPLACE suprpcol WITH &g_tempalias..suprpcol
      REPLACE supgroup WITH &g_tempalias..supgroup
      REPLACE supvalchng WITH &g_tempalias..supvalchng
      REPLACE supexpr WITH &g_tempalias..supexpr

		*- if possibly transporting 3.0 files (11/14/95 jd)
		IF TYPE("user") == "M" AND TYPE(g_tempalias + ".user") == "M"
			REPLACE user WITH &g_tempalias..user
		ENDIF

      REPLACE timestamp WITH &g_tempalias..timestamp
      REPLACE platform WITH m.g_toplatform

      * Update width if it looks like a text object got longer in Windows
      IF m.g_grph2char AND objtype = c_ottext
         REPLACE width WITH MAX(width,LEN(CHRTRANC(expr,'"'+chr(39),'')))
      ENDIF

      DO adjrptsuppress
      DO adjrptfloat
      IF objtype = c_otrepvar OR (objtype = c_otrepfld AND totaltype > 0)
         DO adjrptreset
      ENDIF
   ENDIF

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDSCAN
SET NOCPTRANS TO &cOldCPTrans

SELECT (m.g_tempalias)
USE
SELECT (m.g_scrnalias)

RETURN


*!*****************************************************************************
*!
*!      Procedure: UPDENVIRON
*!
*!*****************************************************************************
PROCEDURE updenviron
PARAMETER m.mustexist
* Update environment records if the user selected environment records for transport
* and if any of them have been updated.
IF EnvSelect() AND IsNewerEnv(m.mustexist)
   * Drop the old environment and put the new one in
   DELETE FOR IsEnviron(objtype) and platform = m.g_toplatform
   SCAN FOR platform = m.g_fromplatform AND IsEnviron(Objtype)
      SCATTER MEMVAR MEMO
      APPEND BLANK
      GATHER MEMVAR MEMO
      REPLACE platform WITH m.g_toplatform
      IF m.g_grph2char
         * DOS requires the alias name to be in upper case, while Windows doesn't
         REPLACE TAG WITH UPPER(TAG)
         REPLACE tag2 WITH UPPER(tag2)
      ENDIF
   ENDSCAN
   m.g_updenviron = .T.
ENDIF

*
* CONVERTPROJECT - Convert project file from 2.0 to 2.5 format
*
*!*****************************************************************************
*!
*!      Procedure: CONVERTPROJECT
*!
*!      Called by: CONVERTER          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE convertproject
PRIVATE m.i

SELECT (m.g_scrnalias)
ZAP

SELECT (m.g_20alias)
SCAN FOR !DELETED()
   SCATTER MEMVAR MEMO
   m.wasarranged = arranged
   RELEASE m.arranged         && to avoid type mismatch at GATHER time

   SELECT (m.g_scrnalias)
   APPEND BLANK
   GATHER MEMVAR MEMO
   DO CASE
   CASE type == "H"
      IF !EMPTY(devinfo)
         * Adjust developer info to support wider state code
         REPLACE devinfo WITH STUFF(devinfo,162,0,CHR(0)+CHR(0)+CHR(0))
         REPLACE devinfo WITH STUFF(devinfo,176,0,REPLICATE(CHR(0),46))
      ENDIF

   CASE type == "s"   && must be lowercase S
      * Adjust for the new method of storing cross-platform arrangement info
      * (ScrnRow = -999 for centered screens)
      REPLACE arranged WITH ;
          PADR(c_dosname,8);
         +IIF(m.wasarranged,"T","F");
         +IIF(m.scrnrow=-999,"T","F");
         +PADL(LTRIM(STR(m.scrnrow,4)),8) ;
         +PADL(LTRIM(STR(m.scrncol,4)),8) ;
         +PADR(c_winname,8);
         +IIF(m.wasarranged,"T","F");
         +IIF(m.scrnrow=-999,"T","F");
         +PADL(LTRIM(STR(m.scrnrow,4)),8) ;
         +PADL(LTRIM(STR(m.scrncol,4)),8) ;
         +PADR(c_macname,8);
         +IIF(m.wasarranged,"T","F");
         +IIF(m.scrnrow=-999,"T","F");
         +PADL(LTRIM(STR(m.scrnrow,4)),8) ;
         +PADL(LTRIM(STR(m.scrncol,4)),8)
   ENDCASE

   * Adjust the symbol table
   IF !EMPTY(symbols)
      FOR i = 1 TO INT((LEN(symbols)-4)/14)
         * Format of a 2.0 symbol table is
         *   4 bytes of header information
         *   n occurrences of this structure:
         *      TEXT symName[11]
         *      TEXT symType
         *      TEXT flags[2]
         * Format of a 2.5 symbol table is the same, except symName is now 13 bytes long
         REPLACE symbols WITH STUFF(symbols,(m.i-1)*16+15,0,CHR(0)+CHR(0))
         REPLACE ckval WITH VAL(sys(2007,symbols))
      ENDFOR
   ENDIF

   * Blank out the timestamp
   REPLACE timestamp WITH 0
ENDSCAN

*
* NewCharToGraphic - Take any new objects from the character platform and copy them
*      to the graphical platform.
*
*!*****************************************************************************
*!
*!      Procedure: NEWCHARTOGRAPHIC
*!
*!      Called by: CHARTOGRAPHIC      (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETWINDFONT        (procedure in TRANSPRT.PRG)
*!               : NEWBANDS           (procedure in TRANSPRT.PRG)
*!               : BANDINFO()         (function  in TRANSPRT.PRG)
*!               : ISOBJECT()         (function  in TRANSPRT.PRG)
*!               : PLATFORMDEFAULTS   (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!               : RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : FINDLIKEVPOS       (procedure in TRANSPRT.PRG)
*!               : FINDLIKEHPOS       (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
PROCEDURE newchartographic
PRIVATE m.thermstep, m.bandcount

m.g_newobjmode = .T.
SELECT (m.g_scrnalias)
SET ORDER TO

* Get the default font for the window in the "to" platform
IF m.g_char2grph
   DO getwindfont
ENDIF

* Update the environment if it is new
DO updenviron WITH .F.

* Remember the window default font
SELECT (m.g_scrnalias)
LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
IF FOUND()
   m.wfontface  = fontface
   m.wfontsize  = fontsize
   m.wfontstyle = fontstyle
ELSE
   m.wfontface  = m.g_dfltfface
   m.wfontsize  = m.g_dfltfsize
   m.wfontstyle = m.g_dfltfstyle
ENDIF

m.g_tempalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
SELECT * FROM (m.g_scrnalias) ;
   WHERE !DELETED() AND platform = m.g_fromplatform AND ;
   isselected(uniqueid,objtype,objcode) AND ;
   uniqueid NOT IN (SELECT uniqueid FROM (m.g_scrnalias) ;
   WHERE platform = m.g_toplatform) ;
   INTO CURSOR (m.g_tempalias)

IF m.g_snippets
   m.thermstep = 35/_TALLY
ELSE
   m.thermstep = 70/_TALLY
ENDIF

IF m.g_filetype = c_report
   DO newbands

   * We need to know where bands start and where they end in
   * both platforms.
   SELECT (m.g_scrnalias)
   COUNT TO m.bandcount FOR platform = m.g_toplatform AND objtype = c_otband
   DIMENSION bands[m.bandCount,4]
   m.bandcount = bandinfo()
   SELECT (m.g_tempalias)
ENDIF

m.rightmost = 0
m.bottommost = 0

SCAN
   IF INLIST(objtype,C_OBJTYPELIST)
      SCATTER MEMVAR MEMO
      SELECT (m.g_scrnalias)
      APPEND BLANK
      GATHER MEMVAR MEMO

      REPLACE platform WITH m.g_toplatform

      DO platformdefaults WITH 0
      DO fillininfo

      DO CASE
      CASE INLIST(objtype,c_otbox, c_otline)
         DO adjbox WITH c_adjbox
      ENDCASE

      IF m.g_filetype = c_report
         DO rptobjconvert WITH m.bandcount
      ELSE
         REPLACE vpos WITH findlikevpos(vpos)
         REPLACE hpos WITH findlikehpos(hpos)

         m.rightmost = MAX(m.rightmost, hpos + width ;
          * FONTMETRIC(6,fontface,fontsize,num2style(fontstyle)) ;
          / FONTMETRIC(6,m.wfontface,m.wfontsize,num2style(m.wfontstyle)))
         m.bottommost = MAX(m.bottommost, vpos + height ;
          * FONTMETRIC(1,fontface,fontsize,num2style(fontstyle)) ;
          / FONTMETRIC(1,m.wfontface,m.wfontsize,num2style(m.wfontstyle)))
      ENDIF
   ENDIF

   SELECT (m.g_tempalias)

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDSCAN

SELECT (m.g_tempalias)
USE
SELECT (m.g_scrnalias)
* Update screen width/height if necessary to hold the new objects
IF m.g_filetype = c_screen
   LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
   IF FOUND()
      * If the screen/report isn't big enough to hold the widest/tallest object,
      * resize it.
      IF width < m.rightmost
         REPLACE width WITH m.rightmost + IIF(m.g_filetype = c_screen,2,2000)
      ENDIF
      IF height < m.bottommost AND m.g_filetype = c_screen
         REPLACE height WITH m.bottommost + IIF(m.g_filetype = c_screen,1,2000)
      ENDIF
   ENDIF
ENDIF
RETURN

*
* NewGraphicToChar - Take any new objects from the graphic platform and copy them
*      to the character platform.
*
*!*****************************************************************************
*!
*!      Procedure: NEWGRAPHICTOCHAR
*!
*!      Called by: GRAPHICTOCHAR      (procedure in TRANSPRT.PRG)
*!
*!          Calls: NEWBANDS           (procedure in TRANSPRT.PRG)
*!               : BANDINFO()         (function  in TRANSPRT.PRG)
*!               : ISOBJECT()         (function  in TRANSPRT.PRG)
*!               : PLATFORMDEFAULTS   (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!               : ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!               : RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : FINDLIKEVPOS       (procedure in TRANSPRT.PRG)
*!               : FINDLIKEHPOS       (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : MAKECHARFIT        (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
PROCEDURE newgraphictochar
PRIVATE m.thermstep, m.bandcount

m.g_newobjmode = .T.
SELECT (m.g_scrnalias)
SET ORDER TO

* Update the environment if it is new
DO updenviron WITH .F.

m.g_tempalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
*
* Get a cursor containing the records in the "to" platform that do not have
* counterparts in the "from" platform.  Exclude Windows report column headers
* and column footers (objtype = 9, objcode = 2 or 6) since they have no DOS analogs.
* Exclude boxes that are filled black.  They are probably used for shadow effects.
*
SELECT * FROM (m.g_scrnalias) ;
   WHERE !DELETED() AND platform = m.g_fromplatform AND ;
   !(objtype = c_otband AND INLIST(objcode,2,6)) AND ;
   isselected(uniqueid,objtype,objcode) AND ;
   !blackbox(objtype,fillred,fillblue,fillgreen,fillpat) AND ;
   uniqueid NOT IN (SELECT uniqueid FROM (m.g_scrnalias) ;
   WHERE platform = m.g_toplatform) ;
   INTO CURSOR (m.g_tempalias)

IF m.g_snippets
   m.thermstep = 35/_TALLY
ELSE
   m.thermstep = 70/_TALLY
ENDIF

IF m.g_filetype = c_report
   DO newbands

   * We need to know where bands start and where they end in
   * both platforms.
   SELECT (m.g_scrnalias)
   COUNT TO m.bandcount FOR platform = m.g_toplatform AND objtype = c_otband
   DIMENSION bands[m.bandCount,4]
   m.bandcount = bandinfo()
   SELECT (m.g_tempalias)
ENDIF

LOCATE FOR .T.
DO WHILE !EOF()
   IF INLIST(objtype,C_OBJTYPELIST) AND objtype <> c_otpicture
      SCATTER MEMVAR MEMO
      SELECT (m.g_scrnalias)
      APPEND BLANK
      GATHER MEMVAR MEMO

      REPLACE platform WITH m.g_toplatform

      DO platformdefaults WITH 0
      DO fillininfo

      IF m.g_filetype = c_screen
         DO adjheightandwidth
      ELSE
        DO rptobjconvert WITH m.bandcount
      ENDIF

      REPLACE vpos WITH findlikevpos(vpos)
      REPLACE hpos WITH findlikehpos(hpos)
   ENDIF

   SELECT (m.g_tempalias)
   SKIP

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDDO

SELECT (m.g_tempalias)
USE
SELECT (m.g_scrnalias)

DO makecharfit

RETURN

*
* NewGrphToGrph - Take any new objects from the graphic platform and copy them
*      to the other graphical platform.
*
*!*****************************************************************************
*!
*!      Procedure: NEWGRPHTOGRPH
*!
*!          Calls: NEWBANDS           (procedure in TRANSPRT.PRG)
*!               : BANDINFO()         (function  in TRANSPRT.PRG)
*!               : ISOBJECT()         (function  in TRANSPRT.PRG)
*!               : PLATFORMDEFAULTS   (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!               : ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!               : RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : FINDLIKEVPOS       (procedure in TRANSPRT.PRG)
*!               : FINDLIKEHPOS       (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : MAKECHARFIT        (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
PROCEDURE newgrphtogrph
PRIVATE m.thermstep, m.bandcount

m.g_newobjmode = .T.

m.g_bandfudge = 0

SELECT (m.g_scrnalias)
SET ORDER TO

* Update the environment if it is new
DO updenviron WITH .F.

m.g_tempalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
*
* Get a cursor containing the records in the "to" platform that do not have
* counterparts in the "from" platform.
*
SELECT * FROM (m.g_scrnalias) ;
   WHERE !DELETED() AND platform = m.g_fromplatform AND ;
   isselected(uniqueid,objtype,objcode) AND ;
   uniqueid NOT IN (SELECT uniqueid FROM (m.g_scrnalias) ;
   WHERE platform = m.g_toplatform) ;
   INTO CURSOR (m.g_tempalias)

IF m.g_snippets
   m.thermstep = 35/_TALLY
ELSE
   m.thermstep = 70/_TALLY
ENDIF

IF m.g_filetype = c_report

   DO newbands

   * We need to know where bands start and where they end in
   * both platforms.
   SELECT (m.g_scrnalias)
   COUNT TO m.bandcount FOR platform = m.g_toplatform AND objtype = c_otband
   DIMENSION bands[m.bandCount,4]
   m.bandcount = bandinfo()
   SELECT (m.g_tempalias)
ENDIF

LOCATE FOR .T.
DO WHILE !EOF()
   IF INLIST(objtype,C_OBJTYPELIST) AND objtype <> c_otpicture
      SCATTER MEMVAR MEMO
      SELECT (m.g_scrnalias)
      APPEND BLANK
      GATHER MEMVAR MEMO

      REPLACE platform WITH m.g_toplatform

      DO platformdefaults WITH 0
      DO fillininfo

      IF m.g_filetype = c_screen
         DO adjheightandwidth
      ELSE
        DO rptobjconvert WITH m.bandcount
      ENDIF

      REPLACE vpos WITH findlikevpos(vpos)
      REPLACE hpos WITH findlikehpos(hpos)
   ENDIF

   SELECT (m.g_tempalias)
   SKIP

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDDO

SELECT (m.g_tempalias)
USE
SELECT (m.g_scrnalias)

RETURN

*
* NewBands - Add any new band records.
*
*!*****************************************************************************
*!
*!      Procedure: NEWBANDS
*!
*!      Called by: NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!
*!          Calls: RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : BANDPOS()          (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE newbands
PRIVATE m.prevband, m.bandstart, m.bandheight
* We need to have the groups in order to do report objects, so we do them seperately.

SCAN FOR objtype = c_otband
   SCATTER MEMVAR MEMO
   SELECT (m.g_scrnalias)
   LOCATE FOR platform = m.g_fromplatform AND uniqueid = m.uniqueid
   SKIP -1
   m.prevband = uniqueid
   LOCATE FOR platform = m.g_toplatform AND uniqueid = m.prevband
   INSERT BLANK
   GATHER MEMVAR MEMO
   REPLACE platform WITH m.g_toplatform

   DO rptobjconvert WITH 0

   DO CASE
   CASE m.g_char2grph
      m.bandheight = height + m.g_bandheight + (m.g_bandfudge/m.g_pixelsize)
   CASE m.g_grph2char
      m.bandheight = 0
   CASE m.g_grph2grph
      m.bandheight = height + m.g_bandheight + (m.g_bandfudge/m.g_pixelsize)
		IF _MAC AND objcode >= 4
		   m.bandheight = m.bandheight + (1/m.g_pixelsize)*10000
		ENDIF
   ENDCASE
   m.bandstart = bandpos(m.uniqueid, m.g_toplatform)

	IF m.g_grph2grph
	   * Because of the bandfudge adjustment, we need to allow some leeway on
   	* the staring point of the band.  Allow 1/2 pixel.
	   m.bandstart = m.bandstart - ((1/2) / m.g_pixelsize) * 10000
	ENDIF

   * Move all the lower bands down by the size of the one we just inserted.
   REPLACE ALL vpos WITH vpos + m.bandheight ;
      FOR platform = m.g_toplatform AND ;
      (objtype = c_otline OR objtype = c_otbox OR ;
      objtype = c_ottext OR objtype = c_otrepfld) AND ;
      vpos >= m.bandstart
   SELECT (m.g_tempalias)
ENDSCAN

*
* AllGraphicToChar - Convert from a graphic platform to a character platform assuming
*      that no records exist for the target platform.
*
*!*****************************************************************************
*!
*!      Procedure: ALLGRAPHICTOCHAR
*!
*!      Called by: GRAPHICTOCHAR      (procedure in TRANSPRT.PRG)
*!
*!          Calls: ALLENVIRONS        (procedure in TRANSPRT.PRG)
*!               : ALLOTHERS          (procedure in TRANSPRT.PRG)
*!               : ALLGROUPS          (procedure in TRANSPRT.PRG)
*!               : RPTCONVERT         (procedure in TRANSPRT.PRG)
*!               : MERGELABELOBJECTS  (procedure in TRANSPRT.PRG)
*!               : LINESBETWEEN       (procedure in TRANSPRT.PRG)
*!               : MAKECHARFIT        (procedure in TRANSPRT.PRG)
*!               : SUPPRESSBLANKLINES (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
PROCEDURE allgraphictochar
PRIVATE m.objindex

DO allenvirons

*
* Create a cursor with all the objects we have left to add.
*
m.g_fromobjonlyalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
SELECT *, RECNO() AS recnum FROM (m.g_scrnalias) ;
   WHERE !DELETED() AND platform = m.g_fromplatform AND ;
   objtype <> c_otrel AND objtype <> c_otworkar AND objtype <> c_otindex AND ;
   objtype <> c_otheader AND objtype <> c_otgroup AND ;
   objtype <> c_otpicture AND ;
   !blackbox(objtype,fillred,fillblue,fillgreen,fillpat) AND ;
   !(m.g_filetype = c_label AND objtype = c_ot20label) AND ;
   !(objtype = c_ot20lbxobj AND EMPTY(expr)) AND;
   oktransport(comment) ;
   INTO CURSOR (m.g_fromobjonlyalias)
m.objindex = _TALLY

DO allothers WITH 80
DO allgroups WITH 10

DO CASE
CASE m.g_filetype = c_label
   ** Trim any records the character platforms won't deal with.
   DELETE FOR platform = m.g_toplatform AND ;
      ((objtype = c_otband AND objcode != 4) OR ;
      objtype = c_otrepvar OR objtype = c_otpicture OR ;
      objtype = c_otline OR objtype = c_otbox)
   DO rptconvert
   DO mergelabelobjects
   DO linesbetween

CASE m.g_filetype = c_report
   ** Trim any records the character platforms won't deal with.
   DELETE FOR platform = m.g_toplatform AND (objtype = c_otpicture)
   DO rptconvert
   DO makecharfit
   DO suppressblanklines

CASE m.g_filetype = c_screen
   DO makecharfit
ENDCASE

SELECT (m.g_fromobjonlyalias)
USE
SELECT (m.g_scrnalias)

RETURN

*
* AllCharToGraphic - Convert from a character platform to a graphic platform assuming
*      that no records exist for the target platform.
*
*!*****************************************************************************
*!
*!      Procedure: ALLCHARTOGRAPHIC
*!
*!      Called by: CHARTOGRAPHIC      (procedure in TRANSPRT.PRG)
*!
*!          Calls: ALLENVIRONS        (procedure in TRANSPRT.PRG)
*!               : ALLOTHERS          (procedure in TRANSPRT.PRG)
*!               : ALLGROUPS          (procedure in TRANSPRT.PRG)
*!               : CALCWINDOWDIMENSION(procedure in TRANSPRT.PRG)
*!               : ADJITEMSINBOXES    (procedure in TRANSPRT.PRG)
*!               : ADJINVBTNS         (procedure in TRANSPRT.PRG)
*!               : JOINLINES          (procedure in TRANSPRT.PRG)
*!               : RPTCONVERT         (procedure in TRANSPRT.PRG)
*!               : SUPPRESSBLANKLINES (procedure in TRANSPRT.PRG)
*!               : ADDGRAPHICALLABELGR(procedure in TRANSPRT.PRG)
*!               : LABELBANDS         (procedure in TRANSPRT.PRG)
*!               : LABELLINES         (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : num2style()        (function  in TRANSPRT.PRG)
*!               : STRETCHLINESTOBORDE(procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
PROCEDURE allchartographic
PRIVATE m.objindex

* Make equivalent screen/report records for the new platform.
DO allenvirons

m.g_fromobjonlyalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
SELECT *, RECNO() AS recnum FROM (m.g_scrnalias) ;
   WHERE !DELETED() AND platform = m.g_fromplatform AND objtype <> c_otrel AND ;
   objtype <> c_otworkar AND objtype <> c_otindex AND ;
   objtype <> c_otheader AND objtype <> c_otgroup AND ;
   !(m.g_filetype = c_label AND objtype = c_ot20label) AND ;
   !(objtype = c_ot20lbxobj AND EMPTY(expr)) AND;
   oktransport(comment) ;
   INTO CURSOR (m.g_fromobjonlyalias)

m.objindex = _TALLY
IF _TALLY = 0
   SELECT (m.g_fromobjonlyalias)
   USE
   SELECT (m.g_scrnalias)
   RETURN
ENDIF

DIMENSION objectpos[m.objindex, 9]

DO allothers WITH 25
DO allgroups WITH 5

* Attempt to adjust the position of objects to reflect the position
* in the previous platform.

DO CASE
CASE m.g_filetype = c_screen
   DO calcwindowdimensions
   DO adjitemsinboxes
   DO adjinvbtns
   *- set this relationship off, before SETting ORDER in the child table
   SELECT (m.g_fromobjonlyalias)
   SET RELATION OFF INTO (m.g_scrnalias)
   SELECT (m.g_scrnalias)
   SET ORDER TO

   DO joinlines

CASE m.g_filetype = c_report
   DO rptconvert
   DO joinlines
   DO suppressblanklines

CASE m.g_filetype = c_label
   DO addgraphicallabelgroups
   DO labelbands
   DO labellines
ENDCASE

m.g_mercury = MIN(m.g_mercury + 5, 95)
DO updtherm WITH m.g_mercury

IF m.g_filetype = c_screen
   IF m.g_allobjects
      LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader AND STYLE != 0
      IF FOUND()
         IF m.g_windheight - g_lastobjectline[1] - 3 = 0
            m.adjustment = .5
         ELSE
            m.adjustment = m.g_windheight - g_lastobjectline[1] - 3
         ENDIF

         IF m.adjustment < 0
            m.adjustment = m.adjustment + 1.5
         ENDIF

         IF m.adjustment > 0
            REPLACE height WITH g_lastobjectline[2] + ;
               m.adjustment * (FONTMETRIC(1) / ;
               FONTMETRIC(1,fontface, fontsize, num2style(fontstyle)))
         ELSE
            REPLACE height WITH g_lastobjectline[2] + 1
         ENDIF
      ENDIF
      DO stretchlinestoborders
   ENDIF
ENDIF

m.g_mercury = MIN(m.g_mercury + 5, 95)
DO updtherm WITH m.g_mercury

SELECT (m.g_fromobjonlyalias)
USE
SELECT (m.g_scrnalias)

*
* AllGrphToGrph - Convert from a graphic platform to another graphic platform assuming
*      that no records exist for the target platform.
*
*!*****************************************************************************
*!
*!      Procedure: ALLGRPHTOGRPH
*!
*!          Calls: ALLENVIRONS        (procedure in TRANSPRT.PRG)
*!               : ALLOTHERS          (procedure in TRANSPRT.PRG)
*!               : ALLGROUPS          (procedure in TRANSPRT.PRG)
*!               : RPTCONVERT         (procedure in TRANSPRT.PRG)
*!               : MERGELABELOBJECTS  (procedure in TRANSPRT.PRG)
*!               : LINESBETWEEN       (procedure in TRANSPRT.PRG)
*!               : MAKECHARFIT        (procedure in TRANSPRT.PRG)
*!               : SUPPRESSBLANKLINES (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
PROCEDURE allgrphtogrph
PRIVATE m.objindex

DO allenvirons

*
* Create a cursor with all the objects we have left to add.
*
m.g_fromobjonlyalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
SELECT *, RECNO() AS recnum FROM (m.g_scrnalias) ;
   WHERE !DELETED() AND platform = m.g_fromplatform AND ;
   objtype <> c_otrel AND objtype <> c_otworkar AND objtype <> c_otindex AND ;
   objtype <> c_otheader AND objtype <> c_otgroup AND ;
   !(m.g_filetype = c_label AND objtype = c_ot20label) AND ;
   !(objtype = c_ot20lbxobj AND EMPTY(expr)) AND;
   oktransport(comment) ;
   INTO CURSOR (m.g_fromobjonlyalias)
m.objindex = _TALLY

DO allothers WITH 80
DO allgroups WITH 10

DO CASE
CASE m.g_filetype = c_label
   DO rptconvert
   DO mergelabelobjects
   DO linesbetween

CASE m.g_filetype = c_report
   DO rptconvert

CASE m.g_filetype = c_screen
   *DO makecharfit
ENDCASE

SELECT (m.g_fromobjonlyalias)
USE
SELECT (m.g_scrnalias)

RETURN


*
* cvrt102FRX - Converts a DOS 1.02 report to DOS 2.5 format
*
*!*****************************************************************************
*!
*!       Function: CVRT102FRX
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: DOCREATE           (procedure in TRANSPRT.PRG)
*!               : FORCEEXT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION cvrt102frx
* Converts FoxPro 1.02 DOS report to FoxPro 2.5 DOS report
PARAMETER m.fname102, m.ftype
PRIVATE m.bakname, m.in_area

m.in_area = SELECT()
SELECT 0
* Create a database structure matching the tab delimited format
*  of a 1.02 report file.
CREATE CURSOR old ( ;
   objtype N(10,0), ;
   content N(10,0), ;
   fldcontent C(254), ;
   frmcontent C(254), ;
   vertpos N(10,0), ;
   horzpos N(10,0), ;
   height N(10,0), ;
   WIDTH N(10,0), ;
   FONT N(10,0), ;
   fontsize N(10,0), ;
   STYLE N(10,0), ;
   penred N(10,0), ;
   pengreen N(10,0), ;
   penblue N(10,0), ;
   fillred N(10,0), ;
   fillgreen N(10,0), ;
   fillblue N(10,0), ;
   PICTURE C(254), ;
   rangeup N(10,0), ;
   rangelow N(10,0), ;
   VALID N(10,0), ;
   initc N(10,0), ;
   calcexp N(10,0) ;
   )

* Replace quote marks with \" so that APPEND won't strip them out.  They are our only
* way of distinguishing quoted text from, say, field names.
m.fpin  = fopen(m.fname102,2)   && open for read access
m.outname = forceext(m.fname102,"TMP")
m.fpout = fcreate(m.outname)

IF m.fpin > 0 AND m.fpout > 0
   DO WHILE !FEOF(m.fpin)
      m.buf = fgets(m.fpin)
      m.buf = STRTRAN(m.buf,'"','\+')
      =fputs(m.fpout,m.buf)
   ENDDO
   =fclose(m.fpin)
   =fclose(m.fpout)

   APPEND FROM (m.outname) TYPE DELIMITED WITH TAB

   * Drop the temporary output file
   IF FILE(m.outname)
      DELETE FILE (m.outname)
   ENDIF

   * Replace quote markers with quotes in the character fields
   REPLACE ALL fldcontent WITH STRTRAN(fldcontent,'\+','"'), ;
               frmcontent WITH STRTRAN(frmcontent,'\+','"'), ;
               picture    WITH STRTRAN(picture,   '\+','"')  ;
      FOR objtype = 17
   * Strip quotes from other object types, such as quoted strings.
   REPLACE ALL fldcontent WITH STRTRAN(fldcontent,'\+',''), ;
               frmcontent WITH STRTRAN(frmcontent,'\+',''), ;
               picture    WITH STRTRAN(picture,   '\+','')  ;
      FOR objtype <> 17

ELSE
   APPEND FROM (m.fname102) TYPE DELIMITED WITH TAB
ENDIF

* Create an empty 2.5 report file
DO docreate WITH "new", c_report

SELECT old
SCAN
   DO CASE
   CASE objtype = 1  && report record
      SELECT new
      APPEND BLANK
      SELECT old
      REPLACE new.platform WITH c_dosname
      REPLACE new.objtype WITH 1
      REPLACE new.objcode WITH c_25frx
      REPLACE new.topmargin WITH old.vertpos
      REPLACE new.botmargin WITH old.horzpos
      REPLACE new.height WITH old.height
      REPLACE new.width WITH old.width
      REPLACE new.offset WITH old.fontsize
      IF (old.initc > 0)
         REPLACE new.environ WITH .T.
      ENDIF
      IF (old.calcexp = 1 OR old.calcexp = 3)
         REPLACE new.ejectbefor WITH .T.
      ENDIF
      IF (old.calcexp = 2 OR old.calcexp = 3)
         REPLACE new.ejectafter WITH .T.
      ENDIF

   CASE objtype = 5  && text record
      SELECT new
      APPEND BLANK
      SELECT old
      REPLACE new.platform WITH c_dosname
      REPLACE new.objtype WITH 5
      REPLACE new.vpos WITH old.vertpos
      REPLACE new.hpos WITH old.horzpos
      REPLACE new.height WITH 1
      REPLACE new.width WITH old.width
      IF (old.rangelow > 0)
         REPLACE new.float WITH .T.
      ENDIF
      REPLACE new.expr WITH '"' + CPTRANS(m.g_tocodepage,m.g_fromcodepage,ALLTRIM(old.fldcontent)) + '"'

   CASE objtype = 7 && box record
      SELECT new
      APPEND BLANK
      SELECT old
      REPLACE new.platform WITH c_dosname
      REPLACE new.objtype WITH 7
      REPLACE new.vpos WITH old.vertpos
      REPLACE new.hpos WITH old.horzpos
      REPLACE new.height WITH old.height
      REPLACE new.width WITH old.width
      REPLACE new.objcode WITH old.content + 4
      IF (old.rangelow > 0)
         REPLACE new.float WITH .T.
      ENDIF
      IF (old.fontsize > 0)
         REPLACE new.boxchar WITH CHR(old.fontsize / 256)
      ENDIF

   CASE objtype = 17 && field record
      SELECT new
      APPEND BLANK
      SELECT old
      REPLACE new.platform WITH c_dosname
      REPLACE new.objtype WITH 8
      REPLACE new.vpos WITH old.vertpos
      REPLACE new.hpos WITH old.horzpos
      REPLACE new.height WITH 1
      REPLACE new.width WITH old.width
      REPLACE new.expr WITH CPTRANS(m.g_tocodepage,m.g_fromcodepage,TRIM(old.fldcontent))
      IF !EMPTY(old.picture)
         REPLACE new.picture WITH '"' + CPTRANS(m.g_tocodepage,m.g_fromcodepage,ALLTRIM(old.picture)) + '"'
      ENDIF
      REPLACE new.totaltype WITH old.valid
      REPLACE new.resettotal WITH old.initc
      IF (old.rangeup > 0)
         REPLACE new.norepeat WITH .T.
      ENDIF

      IF (old.rangelow > 1)
         WRAP = MAX(old.rangelow - 3, 0)
      ELSE
         WRAP = old.rangelow
      ENDIF

      IF (WRAP > 0)
         REPLACE new.stretch WITH .T.
      ENDIF

      IF (old.rangelow = 3 OR old.rangelow = 4)
         REPLACE new.float WITH .T.
      ENDIF

      REPLACE new.fillchar WITH ALLTRIM(old.frmcontent)

   CASE objtype = 18 && band record
      SELECT new
      APPEND BLANK
      SELECT old
      REPLACE new.platform WITH c_dosname
      REPLACE new.objtype WITH 9
      REPLACE new.objcode WITH old.content
      REPLACE new.expr WITH CPTRANS(m.g_tocodepage,m.g_fromcodepage,old.fldcontent)
      REPLACE new.height WITH old.height
      IF (old.vertpos > 0)
         REPLACE new.pagebreak WITH .T.
      ENDIF
      IF (old.fontsize > 0)
         REPLACE new.swapheader WITH .T.
      ENDIF
      IF (old.style > 0)
         REPLACE new.swapfooter WITH .T.
      ENDIF
   ENDCASE
ENDSCAN

* Discard the temporary cursor
SELECT old
USE

IF m.ftype = c_frx102repo
   * Back up the original report and copy the new information to the original file name
   m.bakname = forceext(m.fname102,"TBK")
   RENAME (m.fname102) TO (m.bakname)
ENDIF

* Write the new information on top of the original 1.02 report
SELECT new
COPY TO (m.fname102)
USE
SELECT (m.in_area)
RETURN m.fname102

*!*****************************************************************************
*!
*!      Procedure: CVRTFBPRPT
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: ERRORHANDLER       (procedure in TRANSPRT.PRG)
*!               : CVTSHORT()         (function  in TRANSPRT.PRG)
*!               : CVTBYTE()          (function  in TRANSPRT.PRG)
*!               : DOCREATE           (procedure in TRANSPRT.PRG)
*!               : EVALIMPORTEXPR     (procedure in TRANSPRT.PRG)
*!               : INITBANDS          (procedure in TRANSPRT.PRG)
*!               : BLDBREAKS          (procedure in TRANSPRT.PRG)
*!               : BLDDETAIL          (procedure in TRANSPRT.PRG)
*!               : FORCEEXT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE cvrtfbprpt
* Convert a FoxBASE+ report to FoxPro 2.5 DOS format
PARAMETER m.fnamefbp, m.ftype
PRIVATE m.bakname, m.in_area, m.i, m.idbyte, m.objname, m.obj, m.rp_pool, ;
   m.rp_ltadr, m.rp_ltlen, m.rp_ssexno, m.rp_sbexno, m.rp_doublesp, ;
   m.rp_flds_width, m.rp_flds_exprno, m.rp_width, m.rp_flds_headno, ;
   m.rp_plain, m.band_rows, m.current_row, m.group_num, m.head_row

m.in_area = SELECT()
SELECT 0

m.objname       = ""
m.obj           = 0
m.rp_pool       = 0
m.rp_ltadr      = 0
m.rp_ltlen      = 0
m.rp_ssexno     = 0
m.rp_sbexno     = 0
m.rp_doublesp   = 0
m.rp_flds_width = 0
m.rp_flds_exprno= 0
m.rp_width      = 0
m.rp_flds_headno= 0
m.rp_plain      = 0
m.band_rows     = 0
m.current_row   = 0
m.group_num     = 0
m.head_row      = 0

* Create a set of parallel arrays to contain the report information we need to bring
* across to FoxPro 2.5 DOS.
DIMENSION rp_ltlen(maxliterals)
DIMENSION rp_ltadr(maxliterals)
DIMENSION rp_flds_width(maxrepflds)
DIMENSION rp_flds_type(maxrepflds)
DIMENSION rp_flds_totals(maxrepflds)
DIMENSION rp_flds_dp(maxrepflds)
DIMENSION rp_flds_exprno(maxrepflds)
DIMENSION rp_flds_headno(maxrepflds)
DIMENSION band_rows(10)
band_rows = 0

m.obj = FOPEN(m.g_scrndbf)
IF (m.obj < 1)
   DO errorhandler WITH T_NOOPENREPT_LOC,LINENO(),c_error3
ENDIF

m.idbyte = cvtshort(FREAD(m.obj,2),0)

poolsize = cvtshort(FREAD(m.obj,2),0)
FOR i = 1 TO maxliterals
   rp_ltlen(i) = cvtshort(FREAD(m.obj,2),0)
ENDFOR
FOR i = 1 TO maxliterals
   rp_ltadr(i) = cvtshort(FREAD(m.obj,2),0)
ENDFOR
rp_pool = FREAD(m.obj,litpoolsize)
FOR i = 1 TO maxrepflds
   rp_flds_width(i) = cvtshort(FREAD(m.obj,2),0)
   =FREAD(m.obj,2)
   rp_flds_type(i) = FREAD(m.obj,1)
   rp_flds_totals(i) = FREAD(m.obj,1)
   rp_flds_dp(i) = cvtshort(FREAD(m.obj,2),0)
   rp_flds_exprno(i) = cvtshort(FREAD(m.obj,2),0)
   rp_flds_headno(i) = cvtshort(FREAD(m.obj,2),0)
ENDFOR
rp_pghdno = cvtshort(FREAD(m.obj,2),0)
rp_sbexno = cvtshort(FREAD(m.obj,2),0)
rp_ssexno = cvtshort(FREAD(m.obj,2),0)
rp_sbhdno = cvtshort(FREAD(m.obj,2),0)
rp_sshdno = cvtshort(FREAD(m.obj,2),0)
rp_width = cvtshort(FREAD(m.obj,2),0)
rp_length = cvtshort(FREAD(m.obj,2),0)
rp_lmarg = cvtshort(FREAD(m.obj,2),0)
rp_rmarg = cvtshort(FREAD(m.obj,2),0)
rp_fldcnt = cvtshort(FREAD(m.obj,2),0)
rp_doublesp = FREAD(m.obj,1)
rp_summary = FREAD(m.obj, 1)
rp_subeject = FREAD(m.obj,1)
rp_other = cvtbyte(FREAD(m.obj,1),0)
rp_pageno = cvtshort(FREAD(m.obj,2),0)
=FCLOSE(m.obj)
IF (rp_pageno != 2)
   =FCLOSE(m.obj)
ENDIF

* Create an empty 2.5 report file
DO docreate WITH "new", c_report

* Fill it in
DO evalimportexpr
DO initbands
DO bldbreaks
IF rp_fldcnt > 0
   DO blddetail
ENDIF

* Add the header data
SELECT new
GOTO TOP
REPLACE objtype WITH 1, objcode WITH c_25frx

IF m.ftype = c_fbprptrepo
   * Back up the original report and copy the new information to the original file name
   m.bakname = forceext(m.fnamefbp,"TBK")
   RENAME (m.fnamefbp) TO (m.bakname)
ENDIF

* Write the new information to a file with an FRX extension but the
* same base name as the original FoxBASE+ report
SELECT new
COPY TO (m.fnamefbp)
USE
SELECT (m.in_area)
RETURN m.fnamefbp


*!********************************************************************
*!
*!		Convert FoxPro 1.0 label to 2.0 format
*!
*!********************************************************************

PROCEDURE cvrt102lbx
PARAMETERS m.fname102, m.ftype
PRIVATE m.i, m.short, m.contlen, m.obj, m.remarks, m.height, m.lmargin, m.width, ;
   m.numacross, m.spacesbet, m.linesbet, m.bakname, m.in_area

m.in_area = SELECT()

m.lblname = m.fname102

m.obj = FOPEN(m.lblname)
=FREAD(m.obj,1)				&& Skip revision
m.remarks = FREAD(m.obj,60)
m.height = cvtshort(FREAD(m.obj,2),0)
m.lmargin = cvtshort(FREAD(m.obj,2),0)
m.width = cvtshort(FREAD(m.obj,2),0)
m.numacross = cvtshort(FREAD(m.obj,2),0)
m.spacesbet = cvtshort(FREAD(m.obj,2),0)
m.linesbet = cvtshort(FREAD(m.obj,2),0)

* Read in label contents -- each line ends in a CR

m.contlen = cvtshort(FREAD(m.obj,2),0)
m.work = FREAD(m.obj, m.contlen)
=FCLOSE(m.obj)

DIMENSION lbllines[m.height]
m.start = 1
m.i = 1
FOR m.curlen = 1 TO m.contlen
   IF (SUBSTR(m.work, m.curlen, 1) = CHR(13))
      lbllines[m.i] = SUBSTR(m.work, m.start, m.curlen-m.start)
      m.start = m.curlen+1
      m.i = m.i + 1
   ENDIF
ENDFOR

DO WHILE (m.i <= m.height)
   lbllines[m.i] = ''
   m.i = m.i + 1
ENDDO

* Create an empty 2.0 label
CREATE CURSOR new (objtype N(2), objcode N(2), ;
   name m, expr m, STYLE m, HEIGHT N(3), WIDTH N(3), lmargin N(3), ;
   numacross N(3), spacesbet N(3), linesbet N(3), ENVIRON l, ;
   ORDER m, "unique" l, TAG m, tag2 m, addalias l)

* Add the header data
SELECT new
APPEND BLANK
REPLACE new.objtype WITH 30
REPLACE new.name WITH CPTRANS(m.g_tocodepage,m.g_fromcodepage,m.remarks)

REPLACE new.height WITH m.height
REPLACE new.width WITH m.width
REPLACE new.lmargin WITH m.lmargin
REPLACE new.numacross WITH m.numacross
REPLACE new.spacesbet WITH m.spacesbet
REPLACE new.linesbet WITH m.linesbet

* Add the label contents

FOR m.i = 1 TO m.height
   APPEND BLANK
   REPLACE new.objtype WITH 19
   REPLACE new.expr WITH CPTRANS(m.g_tocodepage,m.g_fromcodepage,lbllines[m.i])
ENDFOR

IF m.ftype = c_lbx102repo
   * Back up the original label and copy the new information to the original file name
   m.bakname = forceext(m.fname102,"TBK")
   RENAME (m.fname102) TO (m.bakname)
ENDIF

* Write the new information on top of the original 1.02 label
SELECT new
COPY TO (m.fname102)
USE
SELECT (m.in_area)
RETURN m.fname102


RETURN

*!********************************************************************
*!
*!		Convert FoxBase+ label to 2.0 format
*!
*!********************************************************************

PROCEDURE cvrtfbplbl
PARAMETERS m.fnamefbp, m.ftype

PRIVATE m.width, m.height, m.lmargin, m.spacesbet, m.linesbet, m.numacross, m.obj, ;
   m.i, m.lblname, m.in_area, m.dummy

m.in_area = SELECT()

m.lblname = m.fnamefbp

m.width = 0
m.height = 0
m.lmargin = 0
m.spacesbet = 0
m.linesbet = 0
m.numacross = 0

m.obj = FOPEN(m.lblname)
=FREAD(m.obj,1)				&& Skip revision
m.remarks = FREAD(m.obj,60)
m.height = cvtshort(FREAD(m.obj,2),0)
m.width = cvtshort(FREAD(m.obj,2),0)
m.lmargin = cvtshort(FREAD(m.obj,2),0)
m.linesbet = cvtshort(FREAD(m.obj,2),0)
m.spacesbet = cvtshort(FREAD(m.obj,2),0)
m.numacross = cvtshort(FREAD(m.obj,2),0)

*******************************************************
* Read the label contents -- strip spaces and add a CR
*******************************************************

DIMENSION lbllines[m.height]
lbllines = '""'
m.lastline = 0
FOR m.i = 1 TO m.height
   m.olen = 60
   m.work = FREAD(m.obj,m.olen)
   DO WHILE ((m.olen > 0) AND (SUBSTR(m.work, m.olen, 1) = ' '))
      m.olen = m.olen - 1
   ENDDO
   =STUFF(m.work, m.olen, 1, '\n')
   lbllines[m.i] = SUBSTR(m.work, 1, m.olen+1)
   IF EMPTY(lbllines[m.i])
      lbllines[m.i] = '""'
   ELSE
      m.lastline = m.i
   ENDIF
ENDFOR

=FCLOSE(m.obj)

CREATE CURSOR new (objtype N(2), objcode N(2), ;
   name m, expr m, STYLE m, HEIGHT N(3), WIDTH N(3), lmargin N(3), ;
   numacross N(3), spacesbet N(3), linesbet N(3), ENVIRON l, ;
  ORDER m, "unique" l, TAG m, tag2 m, addalias l)

* Add the header data
SELECT new
APPEND BLANK
REPLACE new.objtype WITH 30
REPLACE new.name WITH CPTRANS(m.g_tocodepage,m.g_fromcodepage,m.remarks)

REPLACE new.height WITH m.height
REPLACE new.width WITH m.width
REPLACE new.lmargin WITH m.lmargin
REPLACE new.numacross WITH m.numacross
REPLACE new.spacesbet WITH m.spacesbet
REPLACE new.linesbet WITH m.linesbet

FOR m.i = 1 TO m.lastline
   APPEND BLANK
   REPLACE new.objtype WITH 19
   REPLACE new.expr WITH CPTRANS(m.g_tocodepage,m.g_fromcodepage,lbllines[m.i])
ENDFOR

IF m.ftype = c_fbprptrepo
   * Back up the original report and copy the new information to the original file name
   m.bakname = forceext(m.fnamefbp,"TBK")
   RENAME (m.fnamefbp) TO (m.bakname)
ENDIF

* Write the new information to a file with an LBX extension but the
* same base name as the original FoxBASE+ label.
SELECT new
COPY TO (m.fnamefbp)
USE
SELECT (m.in_area)
RETURN m.fnamefbp

*!*****************************************************************************
*!
*!      Procedure: INITBANDS
*!
*!      Called by: cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLITEXPR()       (function  in TRANSPRT.PRG)
*!               : LINESFORHEADING()  (function  in TRANSPRT.PRG)
*!               : FLD_HEAD_EXIST()   (function  in TRANSPRT.PRG)
*!               : HOWMANYHEADINGS()  (function  in TRANSPRT.PRG)
*!               : MAKEBAND           (procedure in TRANSPRT.PRG)
*!               : TOTALS_EXIST()     (function  in TRANSPRT.PRG)
*!               : MAKETEXT           (procedure in TRANSPRT.PRG)
*!               : MAKEFIELD          (procedure in TRANSPRT.PRG)
*!               : GETHEADING()       (function  in TRANSPRT.PRG)
*!               : CENTER_COL()       (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE initbands

APPEND BLANK
REPLACE new->platform WITH c_dosname
REPLACE new->WIDTH WITH m.rp_width
REPLACE new->HEIGHT WITH m.rp_length
REPLACE new->offset WITH m.rp_lmarg
REPLACE new->ejectbefor WITH .T.
m.rp_plain = 0
m.group_num = 0
IF ("Y" = m.rp_summary)
   REPLACE new->SUMMARY WITH .T.
ENDIF
IF (INLIST(m.rp_other,1,3,5,7))
   REPLACE new->ejectbefor WITH .F.
ENDIF
IF (INLIST(m.rp_other,3,6,7))
   REPLACE new->ejectafter WITH .T.
ENDIF
IF (INLIST(m.rp_other,4,5,6,7))
   REPLACE new->PLAIN WITH .T.
   m.rp_plain = 1
ENDIF
m.rp_totals = 0
m.current_row = 0

* header band

m.bandsize = 1
IF (m.rp_plain = 0)
   m.bandsize = m.bandsize + 2
ENDIF

m.string = ""
IF (getlitexpr(m.rp_pghdno, @m.string) <> 0)
   m.size = linesforheading(m.string)
   m.bandsize = m.bandsize + m.size
ENDIF

IF (fld_head_exist() = 1)
   m.size = howmanyheadings()
   m.bandsize = m.bandsize + m.size + 3
ELSE
   m.bandsize = m.bandsize + 3
ENDIF

DO makeband WITH h_page, m.bandsize, "", .F.

* group bands
m.bandstring = ""
IF (getlitexpr(m.rp_sbexno, @m.bandstring) <> 0)
   IF ("Y" = m.rp_subeject)
      m.newpage = .T.
   ELSE
      m.newpage = .F.
   ENDIF
   DO makeband WITH h_break, 2, m.bandstring, m.newpage
   m.rp_totals = m.rp_totals + 1
   IF (getlitexpr(m.rp_ssexno, @m.bandstring) <> 0)
      DO makeband WITH h_break, 2, m.bandstring, .F.
      m.rp_totals = m.rp_totals + 1
   ENDIF
ENDIF

group_num = rp_totals
m.numlines = 1
IF ("Y" = m.rp_doublesp)
   m.numlines = 2
ENDIF

* detail band
DO makeband WITH l_item, m.numlines, "", .F.

* break footer bands
IF (totals_exist() = 1)
   m.bandsize = 2
ELSE
   m.bandsize = 1
ENDIF

m.groupnum = m.rp_totals

FOR i = 1 TO m.rp_totals
   DO makeband WITH f_break, m.bandsize, "", .F.
ENDFOR

* page footer band
DO makeband WITH f_page, 1, "", .F.

* report footer band
DO makeband WITH f_rpt, m.bandsize, "", .F.

IF (rp_plain = 0)
   DO maketext WITH 9, 1, "PAGE NO. ", band_rows(h_page)+1, 0
   DO makefield WITH 5, 1, "_PAGENO", band_rows(h_page)+1, 9, "C", .F., .F., 0, 0
   DO makefield WITH 8, 1, "DATE()", band_rows(h_page)+2, 0, "D", .F., .F., 0, 0
   m.head_row = 3
ELSE
   m.head_row = 0
ENDIF

IF (getlitexpr(m.rp_pghdno,@m.string) <> 0)
   m.string = m.string + ";"
   m.heading = ""
   DO WHILE .T.
      IF (getheading(@m.heading, @m.string) > 0)
         DO maketext WITH LEN(m.heading), 1, m.heading, m.head_row, center_col(LEN(m.heading))
         m.head_row = m.head_row + 1
      ELSE
         EXIT
      ENDIF
   ENDDO
ENDIF

m.head_row = m.head_row + 1

RETURN

*!*****************************************************************************
*!
*!      Procedure: BLDBREAKEXP
*!
*!      Called by: BLDBREAKS          (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLITEXPR()       (function  in TRANSPRT.PRG)
*!               : MAKETEXT           (procedure in TRANSPRT.PRG)
*!               : MAKEFIELD          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE bldbreakexp
PARAMETER m.exprno, m.headno, m.row, m.stars

PRIVATE m.string
m.string = ""
=getlitexpr(m.headno, @m.string)
m.string = m.stars + m.string
strlen = LEN(m.string)
DO maketext WITH m.strlen, 1, m.string, m.row, 0
=getlitexpr(m.exprno, @m.string)
DO makefield WITH rp_ltlen(m.exprno+1), 1, m.string, m.row, m.strlen + 1, "C", .F., .F., 0, 0
RETURN

*!*****************************************************************************
*!
*!      Procedure: BLDBREAKS
*!
*!      Called by: cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!
*!          Calls: LITEXIST()         (function  in TRANSPRT.PRG)
*!               : BLDBREAKEXP        (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE bldbreaks
IF (litexist(rp_sbexno) = 1)
   DO bldbreakexp WITH rp_sbexno, rp_sbhdno, band_rows(h_break) + 1, "** "
   IF (litexist(rp_ssexno) = 1)
      DO bldbreakexp WITH rp_ssexno, rp_sshdno, band_rows(h_break) + 3, "*"
   ENDIF
ENDIF
RETURN

*!*****************************************************************************
*!
*!      Procedure: BLDDETAIL
*!
*!      Called by: cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLITEXPR()       (function  in TRANSPRT.PRG)
*!               : MAKEFIELD          (procedure in TRANSPRT.PRG)
*!               : ADDTOTAL           (procedure in TRANSPRT.PRG)
*!               : GETHEADING()       (function  in TRANSPRT.PRG)
*!               : MAKETEXT           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE blddetail
PRIVATE m.i, m.pg_row, m.istotal, m.fcol, m.row, m.string, m.col, m.heading

m.pg_row = 0
m.istotal = 0
m.fcol = 0
m.row = band_rows(l_item)
m.string = ""
FOR m.i = 1 TO rp_fldcnt
   IF (getlitexpr(rp_flds_exprno(m.i), @m.string) <> 0)
      m.row = band_rows(l_item)
      IF (m.fcol + rp_flds_width(m.i) > m.rp_width - 1)
         rp_flds_width(m.i) = rp_flds_width(m.i) - (m.fcol + rp_flds_width(m.i) - m.rp_width)
         IF (rp_flds_width(m.i) < 0)
            EXIT
         ENDIF
      ENDIF
      DO makefield WITH rp_flds_width(m.i), 1, m.string, m.row, m.fcol, rp_flds_type(m.i), .T., .T., 0, 0
      IF ("Y" = rp_flds_totals(m.i))
         DO makefield WITH rp_flds_width(m.i), 1, m.string, band_rows(f_rpt) + 1, m.fcol, "N", .F., .F., 2, 0
         IF (m.group_num > 0)
            IF (m.group_num > 1)
               DO addtotal WITH m.istotal, band_rows(f_break), m.fcol, rp_flds_width(m.i), m.string, "* Subsubtotal *", 4
               DO addtotal WITH m.istotal, band_rows(f_break) + 2, m.fcol, rp_flds_width(m.i), m.string, "** Subtotal **", 3
            ELSE
               DO addtotal WITH m.istotal, band_rows(f_break), m.fcol, rp_flds_width(m.i), m.string, "** Subtotal **", 3
            ENDIF
         ENDIF
         m.istotal = 1
      ENDIF
   ENDIF

   IF (getlitexpr(rp_flds_headno(m.i), @m.string) <> 0)
      m.string = m.string + ";"
      m.heading = ""
      m.hrow = m.head_row
      DO WHILE .T.
         IF (getheading(@m.heading, @m.string) > 0)
            IF (rp_flds_type(m.i) = "N")
               m.col = (m.fcol + rp_flds_width(m.i)) - LEN(m.heading)
            ELSE
               m.col = m.fcol
            ENDIF
            DO maketext WITH LEN(m.heading), 1, m.heading, m.hrow, m.col
            m.hrow = m.hrow + 1
         ELSE
            EXIT
         ENDIF
      ENDDO
   ENDIF
   m.fcol = m.fcol + rp_flds_width(m.i) + 1
ENDFOR

IF (m.istotal = 1)
   DO maketext WITH 13, 1, T_TOTAL1_LOC, band_rows(f_rpt), 0
ENDIF

RETURN

*!*****************************************************************************
*!
*!      Procedure: ADDTOTAL
*!
*!      Called by: BLDDETAIL          (procedure in TRANSPRT.PRG)
*!
*!          Calls: MAKETEXT           (procedure in TRANSPRT.PRG)
*!               : MAKEFIELD          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE addtotal
PARAMETER m.isfirst, m.row, m.col, m.wt, m.workstr, m.totalstr, m.reset
IF (m.isfirst = 0)
   DO maketext WITH LEN(m.totalstr), 1, m.totalstr, m.row, 0
ENDIF
DO makefield WITH m.wt, 1, m.workstr, m.row+1, m.col, "N", .F., .F., 2, m.reset
RETURN


*!*****************************************************************************
*!
*!       Function: LITEXIST
*!
*!      Called by: BLDBREAKS          (procedure in TRANSPRT.PRG)
*!               : GETLITEXPR()       (function  in TRANSPRT.PRG)
*!               : FLD_HEAD_EXIST()   (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION litexist
PARAMETER m.idx
PRIVATE m.flag
m.flag = 0
IF m.idx != 65535
   IF "" <> SUBSTR(rp_pool, rp_ltadr(m.idx+1)+1, 1)
      m.flag = 1
   ENDIF
ENDIF
RETURN m.flag

*!*****************************************************************************
*!
*!       Function: GETLITEXPR
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!               : BLDBREAKEXP        (procedure in TRANSPRT.PRG)
*!               : BLDDETAIL          (procedure in TRANSPRT.PRG)
*!               : HOWMANYHEADINGS()  (function  in TRANSPRT.PRG)
*!               : EVALIMPORTEXPR     (procedure in TRANSPRT.PRG)
*!
*!          Calls: LITEXIST()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getlitexpr
PARAMETER m.idx, m.string
m.flag = 0
IF (litexist(m.idx) = 1)
   m.string = SUBSTR(m.rp_pool, rp_ltadr(m.idx+1)+1, rp_ltlen(m.idx+1) - 1)
   m.flag = 1
ELSE
   m.string = ""
ENDIF
RETURN m.flag

*!*****************************************************************************
*!
*!      Procedure: MAKEBAND
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE makeband
PARAMETER m.type, m.size, m.string, m.newpage
APPEND BLANK
REPLACE new->platform WITH c_dosname
REPLACE new->objtype WITH 9
REPLACE new->objcode WITH m.type
REPLACE new->expr WITH CPTRANS(m.g_tocodepage,m.g_fromcodepage,m.string)
REPLACE new->HEIGHT WITH m.size
REPLACE new->pagebreak WITH m.newpage
IF (band_rows(m.type) = 0)
   band_rows(m.type) = m.current_row
ENDIF
m.current_row = m.current_row + m.size
RETURN

*!*****************************************************************************
*!
*!      Procedure: MAKETEXT
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!               : BLDBREAKEXP        (procedure in TRANSPRT.PRG)
*!               : BLDDETAIL          (procedure in TRANSPRT.PRG)
*!               : ADDTOTAL           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE maketext
PARAMETER  wt, ht, string, ROW, COL
IF m.wt > 0
   APPEND BLANK
   REPLACE new->platform WITH c_dosname
   REPLACE new->expr WITH '"' + CPTRANS(m.g_tocodepage,m.g_fromcodepage,m.string) + '"'
   REPLACE new->objtype WITH 5
   REPLACE new->height WITH ht
   REPLACE new->WIDTH WITH wt
   REPLACE new->vpos WITH ROW
   REPLACE new->hpos WITH COL
ENDIF
RETURN

*!*****************************************************************************
*!
*!      Procedure: MAKEFIELD
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!               : BLDBREAKEXP        (procedure in TRANSPRT.PRG)
*!               : BLDDETAIL          (procedure in TRANSPRT.PRG)
*!               : ADDTOTAL           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE makefield
PARAMETER m.wt, m.ht, m.string, m.row, m.col, m.fldchar, m.strch, m.flt, m.total, m.reset

APPEND BLANK
REPLACE new->platform WITH c_dosname
REPLACE new->objtype WITH 8
REPLACE new->expr WITH CPTRANS(m.g_tocodepage,m.g_fromcodepage,m.string)
REPLACE new->height WITH m.ht
REPLACE new->WIDTH WITH m.wt
REPLACE new->vpos WITH m.row
REPLACE new->hpos WITH m.col
REPLACE new->fillchar WITH m.fldchar
REPLACE new->STRETCH WITH m.strch
REPLACE new->FLOAT WITH m.flt
REPLACE new->totaltype WITH m.total
REPLACE new->resettotal WITH m.reset
RETURN

*!*****************************************************************************
*!
*!       Function: GETHEADING
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!               : BLDDETAIL          (procedure in TRANSPRT.PRG)
*!               : LINESFORHEADING()  (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getheading
PARAMETER m.heading, m.string
PRIVATE m.flag, m.x, m.heading
m.flag = 0
m.x = AT(';',m.string)
m.heading = SUBSTR(m.string, 1, m.x-1)
m.string = SUBSTR(m.string, m.x+1)
IF (LEN(m.string) > 0)   && more left
   m.flag = 1
ENDIF
IF (LEN(m.heading) > 0)
   m.flag = 1
ENDIF
RETURN m.flag

*!*****************************************************************************
*!
*!       Function: LINESFORHEADING
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!               : HOWMANYHEADINGS()  (function  in TRANSPRT.PRG)
*!
*!          Calls: GETHEADING()       (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION linesforheading
PARAMETER m.string
PRIVATE m.retval, m.string2, m.heading
m.string2 = m.string + ";"
m.heading = ""
m.retval = 0
DO WHILE .T.
   IF (getheading(@m.heading, @m.string2) > 0)
      m.retval = m.retval + 1
   ELSE
      EXIT
   ENDIF
ENDDO
RETURN m.retval

*!*****************************************************************************
*!
*!       Function: HOWMANYHEADINGS
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLITEXPR()       (function  in TRANSPRT.PRG)
*!               : LINESFORHEADING()  (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION howmanyheadings
PRIVATE m.retval, m.i, m.newval
m.retval = 0
FOR m.i = 1 TO m.rp_fldcnt
   IF (getlitexpr(rp_flds_headno, @m.string) <> 0)
      m.newval = linesforheading(m.string)
      m.retval = MAX(m.newval, m.retval)
   ENDIF
ENDFOR
RETURN m.retval

*!*****************************************************************************
*!
*!       Function: FLD_HEAD_EXIST
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!
*!          Calls: LITEXIST()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION fld_head_exist
PRIVATE m.flag, m.i
m.flag = 0
FOR m.i = 1 TO m.rp_fldcnt
   IF (litexist(rp_flds_headno(m.i)) = 1)
      m.flag = 1
      EXIT
   ENDIF
ENDFOR
RETURN m.flag

*!*****************************************************************************
*!
*!       Function: TOTALS_EXIST
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION totals_exist
PRIVATE m.flag, m.i
m.flag = 0
FOR m.i = 1 TO m.rp_fldcnt
   IF ("Y" = rp_flds_totals(m.i))
      m.flag = 1
      EXIT
   ENDIF
ENDFOR
RETURN m.flag

*!*****************************************************************************
*!
*!       Function: CENTER_COL
*!
*!      Called by: INITBANDS          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION center_col
PARAMETER m.length
RETURN (MAX(0, ((m.rp_width - m.rp_lmarg - m.rp_rmarg) - m.length)/2))

*!*****************************************************************************
*!
*!      Procedure: EVALIMPORTEXPR
*!
*!      Called by: cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLITEXPR()       (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE evalimportexpr
PRIVATE string
m.string = ""
FOR i = 1 TO rp_fldcnt
   IF (getlitexpr(rp_flds_exprno(i), @string) <> 0)
      rp_flds_type(i) = TYPE(m.string)
      IF ("U" = rp_flds_type(i))
         rp_flds_type = "C"
      ENDIF
   ENDIF
ENDFOR
RETURN

*!*****************************************************************************
*!
*!       Function: GETOLDREPORTTYPE
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: CVTSHORT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getoldreporttype
* Open the main file and see what kind of file it is.  At this point, all we know
* is that it is either a FoxPro 1.02 report or a FoxBASE+ report, or possibly
* a report from some other product.

PRIVATE m.fp, m.reptotals, m.retcode, m.tag
m.retcode = m.tp_filetype

m.fp = FOPEN(m.g_scrndbf)
IF fp > 0
   m.reptotals = cvtshort(FREAD(m.fp,2),0)
   DO CASE
   CASE (m.reptotals == 2)   && FoxBASE+ report
      DO CASE
      CASE m.tp_filetype = c_frx102modi
         m.retcode= c_fbprptmodi
      CASE m.tp_filetype = c_frx102repo
         m.retcode = c_fbprptrepo
      OTHERWISE
         m.retcode = c_fbprptrepo
      ENDCASE
   OTHERWISE
		* Check for alien report
		=FSEEK(m.fp,0)
		m.tag = FREAD(m.fp,8)
		IF UPPER(m.tag) == "DBASE IV"
			m.retcode = c_db4type
		ELSE
	      m.retcode = m.tp_filetype
		ENDIF
   ENDCASE
   =FCLOSE(m.fp)
ENDIF
RETURN m.retcode

*!*****************************************************************************
*!
*!       Function: GETOLDLABELTYPE
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: CVTSHORT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getoldlabeltype
* Open the main file and see what kind of file it is.  At this point, all we know
* is that it is either a FoxPro 1.02 report or a FoxBASE+ label.

PRIVATE m.fp, m.reptotals, m.retcode
m.retcode = m.tp_filetype

m.fp = FOPEN(m.g_scrndbf)
IF fp > 0
   m.reptotals = cvtbyte(FREAD(m.fp,1),0)
   m.dummy     = FREAD(m.fp,1)   && skip this one
   DO CASE
   CASE (m.reptotals == 2)   && FoxBASE+ label
      DO CASE
      CASE m.tp_filetype = c_lbx102modi
         m.retcode= c_fbplblmodi
      CASE m.tp_filetype = c_lbx102repo
         m.retcode = c_fbplblrepo
      OTHERWISE
         m.retcode = c_fbplblrepo
      ENDCASE
   OTHERWISE
		* Check for alien report
		=FSEEK(m.fp,0)
		m.tag = FREAD(m.fp,8)
		IF UPPER(m.tag) == "DBASE IV"
			m.retcode = c_db4type
		ELSE
	      m.retcode = m.tp_filetype
		ENDIF
   ENDCASE
   =FCLOSE(m.fp)
ENDIF
RETURN m.retcode

*
* MAPBUTTON - Compare two sets of buttons
*
*!*****************************************************************************
*!
*!       Function: MAPBUTTON
*!
*!      Called by: UPDATESCREEN       (procedure in TRANSPRT.PRG)
*!
*!          Calls: SCATTERBUTTONS     (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION mapbutton
PARAMETER frombtn, tobtn
PRIVATE m.endpos, m.outstrg, m.topos, m.i, m.pictclau
m.pictclau = LEFT(m.tobtn,AT(' ',m.tobtn)-1)
DO CASE
CASE m.g_grph2char
   * Strip out the BMP extensions, if present
   m.frombtn = STRTRAN(m.frombtn,".BMP","")
   m.frombtn = STRTRAN(m.frombtn,".bmp","")

CASE ".BMP" $ UPPER(m.tobtn)
   * Add back in the bitmap extensions, if the to platform already has some.  The
   * strategy is to mark all existing bitmap extensions, then add one to each of the
   * atoms in the picture clause.
   DO CASE
   CASE RIGHT(m.tobtn,1) = '"' OR RIGHT(m.tobtn,1) = "'"
      m.tobtn = STUFF(m.tobtn,LEN(m.tobtn),0,';')
   OTHERWISE
      m.tobtn = m.tobtn + ';'
   ENDCASE

   * 'brlfq' is just a marker for where a semicolon needs to go.  Mark all the existing
   * BMP extensions.
   m.tobtn = STRTRAN(m.tobtn,".BMP;",".BMPbrlfq")
   m.tobtn = STRTRAN(m.tobtn,".bmp;",".BMPbrlfq")

   * Add a new BMP extension where there wasn't one before.
   m.tobtn = STRTRAN(m.tobtn,";",".BMPbrlfq")

   * Put the semicolons back
   m.tobtn = STRTRAN(m.tobtn,"brlfq",";")

   * Remove trailing semicolons
   DO WHILE RIGHT(m.tobtn,2) = ';"' OR RIGHT(m.tobtn,2) = ";'"
      m.tobtn = STUFF(m.tobtn,LEN(m.tobtn)-1,1,"")
   ENDDO

   * Now make sure there is a 'B' in the picture clause
   IF !("B" $ m.pictclau) AND ("@" $ m.pictclau)
      m.tobtn = STUFF(m.tobtn,AT("@",m.tobtn)+2,0,"B")
      m.pictclau = m.pictclau + "B"
   ENDIF
ENDCASE

DO CASE
CASE m.frombtn == m.tobtn
   RETURN m.frombtn
CASE OCCURS(';',m.frombtn) = OCCURS(';',m.tobtn)
   IF m.g_char2grph AND ("B" $ m.pictclau)
      * Return the newly modified "to" string in this case.
      RETURN m.tobtn
   ELSE
      RETURN m.frombtn
   ENDIF
CASE OCCURS(';',m.frombtn) > OCCURS(';',m.tobtn)
   * Are these bitmap buttons?
   IF ("B" $ m.pictclau)
      * Just add a blank one to the end
      m.endpos = RAT('"',m.tobtn)
      IF endpos > 1
         RETURN STUFF(m.tobtn,m.endpos,0,';NEW.BMP')
      ELSE
         RETURN m.tobtn + ';'
      ENDIF
   ELSE
      * Not bitmaps.
      RETURN m.frombtn
   ENDIF
OTHERWISE
   RETURN m.frombtn

   * An alternative strategy is to try to preserve as many as possible of the
   * destination buttons, especially since they might contain bitmaps, etc.

   * Populate two arrays with the button prompts.  Then scan through the
   * 'from' array seeing if we can match it up against something in the 'to'
   * array.  If so, emit the 'to' array picture.  Otherwise, emit the 'from'
   * one.
   DIMENSION fromarray[1], toarray[1]
   DO scatterbuttons WITH m.frombtn, fromarray
   DO scatterbuttons WITH m.tobtn, toarray
   outstrg = ""
   FOR m.i = 1 TO ALEN(fromarray)
      m.topos = ASCAN(toarray,fromarray[i])
      IF m.topos > 0
         m.outstrg = m.outstrg + IIF(EMPTY(m.outstrg),'',';') + toarray[m.topos]
      ELSE
         m.outstrg = m.outstrg + IIF(EMPTY(m.outstrg),'',';') + fromarray[m.i]
      ENDIF
   ENDFOR
   m.outstrg = LEFT(m.frombtn,AT(' ',m.frombtn)) + m.outstrg + '"'
   RETURN m.outstrg
ENDCASE

*!*****************************************************************************
*!
*!      Procedure: SCATTERBUTTONS
*!
*!      Called by: MAPBUTTON()        (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE scatterbuttons
PARAMETERS btnlist, destarray
PRIVATE m.i, m.fromstrg, m.num, m.theword
m.fromstrg = SUBSTR(m.btnlist,AT(' ',m.btnlist)+1)
m.fromstrg = CHRTRANC(m.fromstrg,CHR(34)+CHR(39),"")
m.num = OCCURS(';',m.fromstrg)
DIMENSION destarray[m.num+1]
FOR m.i = 1 TO m.num + 1
   DO CASE
   CASE m.i = 1    && first button
      m.theword = LEFT(m.fromstrg,AT(';',m.fromstrg)-1)
   CASE m.i = m.num + 1   && last button
      m.theword = SUBSTR(m.fromstrg,AT(';',m.fromstrg,m.num)+1)
   OTHERWISE
      m.theword = SUBSTR(m.fromstrg,AT(';',m.fromstrg,m.i-1)+1, ;
         AT(';',m.fromstrg,m.i) - AT(';',m.fromstrg,m.i-1))
   ENDCASE
   destarray[m.i] = UPPER(ALLTRIM(m.theword))
ENDFOR
RETURN

*
* FindLikeVpos - Tries to find an object in the from platform with a vpos that matches the vpos
*      of a new object we are adding.  If it finds one, we return that objects Vpos in the to
*      platform.  This gives us a reasonable chance of coming close to where the user will want
*      an object that is being added to a pre-converted screen.
*
*!*****************************************************************************
*!
*!      Procedure: FINDLIKEVPOS
*!
*!      Called by: NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!
*!          Calls: ISOBJECT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE findlikevpos
PARAMETER m.oldvpos
PRIVATE m.objid, m.saverec, m.retval
m.saverec = RECNO()
m.retval = m.oldvpos

LOCATE FOR platform = m.g_fromplatform AND vpos = m.oldvpos AND INLIST(objtype,C_OBJTYPELIST)
IF FOUND()
   m.objid = uniqueid
   LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
   IF FOUND()
      m.retval = vpos
   ENDIF
ENDIF

GOTO RECORD (m.saverec)
RETURN m.retval

*
* FindLikeHpos - Tries to find an object in the from platform with an hpos that matches the hpos
*      of a new object we are adding.  If it finds one, we return that objects Hpos in the to
*      platform.  This gives us a reasonable chance of coming close to where the user will want
*      an object that is being added to a pre-converted screen.
*
*!*****************************************************************************
*!
*!      Procedure: FINDLIKEHPOS
*!
*!      Called by: NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!
*!          Calls: ISOBJECT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE findlikehpos
PARAMETER m.oldhpos
PRIVATE m.objid, m.saverec, m.retval
m.saverec = RECNO()
m.retval = m.oldhpos

LOCATE FOR platform = m.g_fromplatform AND hpos = m.oldhpos AND INLIST(objtype,C_OBJTYPELIST)
IF FOUND()
   m.objid = uniqueid
   LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
   IF FOUND()
      m.retval = hpos
   ENDIF
ENDIF

GOTO RECORD (m.saverec)
RETURN m.retval

*
* MakeCharFit - Makes sure that a report or screen is large enough to hold all of its objects.
*
*!*****************************************************************************
*!
*!      Procedure: MAKECHARFIT
*!
*!      Called by: NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETRIGHTMOST       (procedure in TRANSPRT.PRG)
*!               : GETLOWEST          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE makecharfit
PRIVATE m.right, m.bottom

m.right = CEILING(getrightmost(m.g_toplatform))+2
m.bottom = CEILING(getlowest(m.g_toplatform))+2

LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
IF FOUND()
   IF WIDTH < m.right
      REPLACE WIDTH WITH m.right
   ENDIF

   IF height < m.bottom AND m.g_filetype = c_screen
      REPLACE height WITH m.bottom
   ENDIF
ENDIF
RETURN

*
* allenvirons - Process all the screen and environment records first.
*
*!*****************************************************************************
*!
*!      Procedure: ALLENVIRONS
*!
*!      Called by: ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: ADJCOLOR           (procedure in TRANSPRT.PRG)
*!               : ADJOBJCODE         (procedure in TRANSPRT.PRG)
*!               : ADJFONT            (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE allenvirons
PRIVATE m.recno

SCAN FOR platform = m.g_fromplatform AND !DELETED() AND ;
      (objtype = c_otheader OR objtype = c_otrel OR objtype = c_otworkar OR objtype = c_otindex OR ;
      (m.g_filetype = c_label AND objtype = c_ot20label))
   m.recno = RECNO()

   DO fixpen

   SCATTER MEMVAR MEMO
   APPEND BLANK
   GATHER MEMVAR MEMO

   REPLACE platform WITH m.g_toplatform
   IF IsEnviron(objtype) AND m.g_grph2char
      * DOS requires the alias name to be in upper case, while Windows doesn't
      REPLACE TAG WITH UPPER(TAG)
      REPLACE tag2 WITH UPPER(tag2)
   ENDIF

   IF objtype = c_otheader OR (m.g_filetype = c_label AND objtype = c_ot20label)
      m.g_windheight = HEIGHT
      m.g_windwidth = WIDTH

      DO CASE
      CASE m.g_filetype = c_screen
         DO adjcolor

      CASE m.g_filetype = c_report
         DO CASE
         CASE m.g_char2grph
            REPLACE vpos WITH 1,;
             WIDTH WITH -1.0,;
             ruler WITH 1,;
             rulerlines WITH 1,;
             gridv WITH 9,;
             gridh WITH 9,;
             penred   WITH 60,;
             pengreen WITH 80,;
             penblue    WITH 0
         CASE m.g_grph2char
            REPLACE height WITH c_charrptheight
            REPLACE WIDTH WITH c_charrptwidth
         ENDCASE

      CASE m.g_filetype = c_label
         DO CASE
         CASE m.g_char2grph
            REPLACE objtype WITH c_otheader,;
             ruler WITH 1,;
             rulerlines WITH 1,;
             grid WITH .T.,;
             gridv WITH 12,;
             gridh WITH 12,;
             penred   WITH -1,;
             pengreen WITH 65535,;
             stretchtop WITH .F.,;
             TOP WITH .F.,;
             BOTTOM WITH .T.,;
             curpos WITH .F.
         CASE m.g_grph2char
            REPLACE objtype WITH c_ot20label
            REPLACE hpos WITH (hpos * c_charsperinch)/10000
            REPLACE height WITH (height * c_linesperinch)/10000
            REPLACE WIDTH WITH (WIDTH * c_charsperinch)/10000
            IF WIDTH < 0
               REPLACE WIDTH WITH c_charrptwidth
            ENDIF
         ENDCASE
      ENDCASE

      DO adjobjcode
      DO adjfont
   ENDIF

   GOTO RECORD m.recno
ENDSCAN
m.g_mercury = MIN(m.g_mercury + 5, 95)
DO updtherm WITH m.g_mercury
RETURN

*
* allothers - Process all other records.
*
*!*****************************************************************************
*!
*!      Procedure: ALLOTHERS
*!
*!      Called by: ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: CALCPOSITIONS      (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE allothers
PARAMETER m.thermpart
PRIVATE m.recno, m.numothers, m.thermstep, m.i

m.thermstep = m.thermpart / m.objindex

SELECT (m.g_fromobjonlyalias)
SET RELATION TO recnum INTO m.g_scrnalias ADDITIVE
LOCATE FOR .T.
m.i = 1

SCAN FOR !DELETED()

   m.recno = RECNO()

   DO fixpen

   SCATTER MEMVAR MEMO

   IF m.g_char2grph
      DO calcpositions WITH m.i    && determine relative positions of objects
      m.i = m.i + 1
   ENDIF

   SELECT (m.g_scrnalias)
   APPEND BLANK
   GATHER MEMVAR MEMO

   IF gError
     *- seems to be necessary (jd 3/24/96)
     RETURN TO MASTER
   ENDIF

   REPLACE platform WITH m.g_toplatform

   DO fillininfo

   SELECT (m.g_fromobjonlyalias)
   GOTO RECORD m.recno

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury

ENDSCAN
RETURN

*
* FillInInfo - Fill in information for the fields in SCX/FRX database.
*
*!*****************************************************************************
*!
*!      Procedure: FILLININFO
*!
*!      Called by: NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : ALLOTHERS          (procedure in TRANSPRT.PRG)
*!
*!          Calls: ADJRPTSUPPRESS     (procedure in TRANSPRT.PRG)
*!               : ADJRPTFLOAT        (procedure in TRANSPRT.PRG)
*!               : ADJRPTRESET        (procedure in TRANSPRT.PRG)
*!               : OBJ2BASEFONT()     (function  in TRANSPRT.PRG)
*!               : num2style()        (function  in TRANSPRT.PRG)
*!               : ADJPEN             (procedure in TRANSPRT.PRG)
*!               : ADJCOLOR           (procedure in TRANSPRT.PRG)
*!               : ADJFONT            (procedure in TRANSPRT.PRG)
*!               : ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE fillininfo
IF m.g_filetype = c_report
   DO adjrptsuppress
   DO adjrptfloat
ENDIF

DO CASE
CASE m.g_char2grph
   * Handle 2D or 3D decision
   IF _MAC ;
         AND (INLIST(objtype, c_ottxtbut, c_otradbut, c_otchkbox, ;
             c_otspinner, c_otlist, c_otpopup) ;
          OR (objtype = c_otfield AND INLIST(objcode,c_sgget,c_sgedit)))
      * Applies to most objects and GET/EDIT fields (but not SAY fields)
      IF m.g_look2d
         * Add '2' to the control string
         REPLACE picture WITH addquote(make2d(picture))
      ELSE
         REPLACE picture WITH addquote(make3d(picture))
      ENDIF
   ENDIF

   DO CASE
   CASE objtype = c_otpopup
      * Popups are a special case since the arrow control counts against the width
      * under Windows.
      REPLACE WIDTH WITH WIDTH + 2
   CASE INLIST(objtype,c_otrepvar,c_otrepfld)
      DO adjrptreset
      IF fillchar = "N"
         REPLACE offset WITH 1      && Change alignment for numerics.
      ENDIF
   ENDCASE
CASE m.g_grph2char
   DO CASE
   CASE objtype = c_ottext
      REPLACE height WITH MAX(height,1), width WITH MAX(width,1)
   CASE objtype = c_otspinner
      * Map spinners to regular fields
      REPLACE objtype   WITH c_otfield, ;
         height    WITH 1, ;
         fillchar  WITH "N"
   CASE objtype = c_otline
      * Map Windows lines to DOS boxes
      REPLACE objtype WITH c_otbox
      REPLACE height  WITH MAX(height,1), WIDTH WITH MAX(WIDTH,1)
      IF pensize >= 6
         REPLACE boxchar WITH "Û"
      ENDIF
   CASE INLIST(objtype,c_otradbut,c_ottxtbut)
      * Remove the BMP extension from bitmap buttons
      REPLACE PICTURE WITH STRTRAN(PICTURE,".BMP","")
      REPLACE PICTURE WITH STRTRAN(PICTURE,".bmp","")
   CASE objtype = c_otfield AND ;
         (objcode = c_sgedit  OR (INLIST(objcode,c_sgsay,c_sgget) AND WIDTH > 25))
      * Adjust widths of edit fields and very long GET/SAY fields to account
      * for font differences between the object and the base font.
      REPLACE WIDTH WITH MAX(obj2basefont(WIDTH,g_dfltfface,g_dfltfsize,g_dfltfstyle,;
         fontface,fontsize,num2style(fontstyle)),1)
   CASE objtype = c_otbox AND (objcode = 4)
      IF pensize >= 6
         REPLACE boxchar WITH "Û"
      ENDIF
   CASE INLIST(objtype,c_otrepvar,c_otrepfld)
      DO adjrptreset
      IF objtype = c_otrepvar
         * DOS report variable names have to be in upper case
         REPLACE name WITH UPPER(name)
      ENDIF
   ENDCASE
CASE m.g_grph2grph
   * Handle 2D or 3D decision
   IF _MAC ;
         AND (INLIST(objtype, c_ottxtbut, c_otradbut, c_otchkbox, ;
             c_otspinner, c_otlist, c_otpopup) ;
          OR (objtype = c_otfield AND INLIST(objcode,c_sgget,c_sgedit)))
      * Applies to most objects and GET/EDIT fields (but not SAY fields)
      IF m.g_look2d
         * Add '2' to the control string
         REPLACE picture WITH addquote(make2d(picture))
      ELSE
         REPLACE picture WITH addquote(make3d(picture))
      ENDIF
   ENDIF

   DO CASE
   CASE objtype = c_ottxtbut
      * Preserve default button height across transportation sessions
      DO CASE
      CASE  _MAC AND height = m.g_winbtnheight
         REPLACE height WITH m.g_macbtnheight
      CASE  _WINDOWS AND INLIST(height,1.500,1.125,m.g_macbtnheight)
         * The Mac button might have been either 2D or 3D
         REPLACE height WITH m.g_winbtnheight
      ENDCASE
   CASE objtype = c_otpopup
      REPLACE height WITH m.g_pophght
   ENDCASE

	* Map Mac 3D lines/boxes back to Windows single line lines/boxes
	IF _WINDOWS AND INLIST(objtype,c_otbox,c_otline)
	   IF pensize = 2 AND penpat = 100
		   REPLACE pensize WITH 1, penpat WITH 8
		ENDIF
	ENDIF

ENDCASE

IF objtype <> c_otbox AND objtype <> c_otline
   DO adjpen
ENDIF

DO adjcolor
DO adjfont
IF m.g_filetype = c_screen
   DO adjheightandwidth
ENDIF
RETURN

*
* adjrptfloat - Convert float/stretch/relative postion types between
*      character and graphical positions
*
*!*****************************************************************************
*!
*!      Procedure: ADJRPTFLOAT
*!
*!      Called by: UPDATEREPORT       (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjrptfloat
DO CASE
CASE m.g_char2grph
   DO CASE
   CASE FLOAT AND (objtype = c_otbox AND HEIGHT > 1)
      * Box or a vertical line--float as band stretches translates to Top--stretch w/ band.
      * Use the height > 1 test because DOS boxes haven't been translated into Windows
      * lines yet.
      REPLACE stretchtop WITH .T.
      REPLACE TOP WITH .F.
      REPLACE BOTTOM WITH .F.
   CASE FLOAT AND STRETCH
      REPLACE stretchtop WITH .T.
      REPLACE TOP WITH .F.
      REPLACE BOTTOM WITH .F.
   CASE FLOAT
      REPLACE BOTTOM WITH .T.
      REPLACE TOP WITH .F.
      REPLACE stretchtop WITH .F.
   ENDCASE
CASE m.g_grph2char
   DO CASE
   CASE objtype = c_otrepfld AND (stretchtop OR STRETCH)
      REPLACE FLOAT WITH .T.
      REPLACE STRETCH WITH .T.
   CASE BOTTOM
      REPLACE FLOAT WITH .T.
      REPLACE STRETCH WITH .F.
   CASE TOP
      REPLACE FLOAT WITH .F.
      REPLACE STRETCH WITH .F.
   CASE stretchtop OR STRETCH
      REPLACE FLOAT WITH .T.
      REPLACE STRETCH WITH .F.
   ENDCASE
ENDCASE
RETURN

*
* adjrptSuppress - Convert Suppression types between 2.5 platforms.
*
*!*****************************************************************************
*!
*!      Procedure: ADJRPTSUPPRESS
*!
*!      Called by: UPDATEREPORT       (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjrptsuppress
* Handle suppression of repeated values.
*
* In DOS 2.0, the value of the detail record "norepeat" determines whether repeated values
* are suppressed, if this is a field object, or whether group headings are repeated,
* if this is a group header.  The main screen header record "norepeat" field determines
* whether blank lines are suppressed in the detail band.
*
* In 2.5, the norepeat field is used just for suppression of blank lines.
* We are positioned on a detail record now.
*
DO CASE
CASE m.g_char2grph
   IF objtype = c_otband
      * The meaning for DOS is reversed from Windows
      REPLACE norepeat WITH !norepeat
   ELSE
      IF norepeat            && suppress repeated values
         REPLACE supvalchng WITH .T.
         REPLACE supovflow WITH .F.
         DO CASE
         CASE resetrpt = 0
            REPLACE suprpcol WITH 0
            REPLACE supgroup WITH 0
         CASE resetrpt = 1
            REPLACE suprpcol WITH 3
            REPLACE supgroup WITH 0
         OTHERWISE
            REPLACE suprpcol WITH 0
            REPLACE supgroup WITH resetrpt+3
         ENDCASE
      ELSE                   && no suppression of repeated values
         REPLACE supalways WITH .T.
         REPLACE supvalchng WITH .F.
         REPLACE supovflow WITH .F.
         REPLACE suprpcol WITH 3
         REPLACE supgroup WITH 0
      ENDIF
   ENDIF
CASE m.g_grph2char
   IF supvalchng AND !supalways
      REPLACE norepeat WITH .T.
      IF supgroup > 0
         REPLACE resetrpt WITH supgroup - 3
      ELSE
         IF suprpcol = 3
            REPLACE resetrpt WITH 1
         ELSE
            REPLACE resetrpt WITH 0
         ENDIF
      ENDIF
   ELSE
      REPLACE norepeat WITH .F.
   ENDIF
ENDCASE
RETURN

*
* adjrptreset - Convert the reset values between 2.0 and 2.5.
*
*!*****************************************************************************
*!
*!      Procedure: ADJRPTRESET
*!
*!      Called by: UPDATEREPORT       (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjrptreset
DO CASE
CASE m.g_char2grph
   DO CASE
   CASE resettotal = 0
      REPLACE resettotal WITH 1
   CASE resettotal = 1
      REPLACE resettotal WITH 2
   OTHERWISE
      REPLACE resettotal WITH resettotal+3
   ENDCASE
CASE m.g_grph2char
   DO CASE
   CASE resettotal = 1
      REPLACE resettotal WITH 0
   CASE resettotal = 2 OR resettotal = 3
      REPLACE resettotal WITH 1
   OTHERWISE
      REPLACE resettotal WITH resettotal-3
   ENDCASE
ENDCASE
RETURN

*
* GetCharSuppress - Gets the global setting of blank line Suppression for a report. (This is
*      only valid for character mode reports).
*
*!*****************************************************************************
*!
*!       Function: GETCHARSUPPRESS
*!
*!      Called by: IMPORT             (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getcharsuppress
LOCATE FOR platform = m.g_fromplatform AND objtype = c_otheader
IF FOUND()
   RETURN norepeat
ELSE
   RETURN .F.
ENDIF

*
* SuppressBlankLines - Looks through the from platform to see if any
*      object is marked to Suppress blank lines.  If one is, we
*      make the entire "to" report (which is assumed to be character)
*      Suppress blank lines.
*
*!*****************************************************************************
*!
*!      Procedure: SUPPRESSBLANKLINES
*!
*!      Called by: ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETBANDCODE()      (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE suppressblanklines
PRIVATE m.supcount
DO CASE
CASE m.g_grph2char
   COUNT TO m.supcount FOR platform = m.g_fromplatform AND objtype = c_otrepfld
   IF m.supcount > 0
      LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
      IF FOUND()
         REPLACE norepeat WITH .T.
      ENDIF
   ENDIF
CASE m.g_char2grph
   * DOS suppression of blank lines only applies to detail lines.  Only mark graphical
   * objects in the detail band as suppressed.
   SCAN FOR platform = m.g_toplatform AND objtype <> c_otband AND objtype <> c_otheader
      myexpr = expr
      IF objtype = 8
         WAIT CLEAR
      ENDIF
      bcode  = getbandcode(vpos)
      IF bcode = 4     && detail band
         REPLACE norepeat WITH m.g_norepeat
      ELSE
         REPLACE norepeat WITH .F.
      ENDIF
   ENDSCAN
ENDCASE

*
* allGroups - Process all Group records.
*
*!*****************************************************************************
*!
*!      Procedure: ALLGROUPS
*!
*!      Called by: ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE allgroups
PARAMETER m.thermpart
PRIVATE m.recno, m.numothers, m.thermstep

m.thermstep = m.thermpart / m.objindex
SELECT (m.g_scrnalias)

SCAN FOR platform = m.g_fromplatform AND objtype = c_otgroup
   m.recno = RECNO()

   SCATTER MEMVAR MEMO
   APPEND BLANK
   GATHER MEMVAR MEMO

   REPLACE platform WITH m.g_toplatform

   GOTO RECORD m.recno

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDSCAN

*
* RptConvert - Converts entire reports between platforms.
*
*!*****************************************************************************
*!
*!      Procedure: RPTCONVERT
*!
*!      Called by: ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: ISREPTOBJECT()     (function  in TRANSPRT.PRG)
*!               : RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : BANDINFO()         (function  in TRANSPRT.PRG)
*!               : CLONEBAND          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE rptconvert
PRIVATE m.thermstep

COUNT TO m.thermstep FOR platform = m.g_toplatform AND ;
   (isreptobject(objtype) OR objtype = c_otband)

IF m.g_grph2char
   m.thermstep = 25 / m.thermstep
ELSE
   m.thermstep = 50 / m.thermstep
ENDIF

* We need to do bands before any other object.
SCAN FOR platform = m.g_toplatform AND objtype = c_otband
   DO rptobjconvert WITH 0
   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDSCAN

* We need to know where bands start and where they end in
* both platforms.
COUNT TO m.bandcount FOR platform = m.g_toplatform AND objtype = c_otband
GOTO TOP

DIMENSION bands[m.bandCount,4]
m.bandcount = bandinfo()

* Make sure that the band headers and footers match on Windows
IF m.g_char2grph
   DO cloneband
ENDIF

SCAN FOR platform = m.g_toplatform ;
   AND INLIST(objtype, c_otrepfld, c_ottext,c_otbox, c_otline, c_otpicture)

   IF m.g_grph2grph OR objtype <> c_otpicture
	   DO rptobjconvert WITH m.bandcount
   ENDIF

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDSCAN

*
* RptObjConvert - Converts the size and postion of a given record in a report/label
*
*!*****************************************************************************
*!
*!      Procedure: RPTOBJCONVERT
*!
*!      Called by: NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : NEWBANDS           (procedure in TRANSPRT.PRG)
*!               : RPTCONVERT         (procedure in TRANSPRT.PRG)
*!
*!          Calls: EMPTYBAND()        (function  in TRANSPRT.PRG)
*!               : CVTREPORTVERTICAL()(function  in TRANSPRT.PRG)
*!               : ADJBOX             (procedure in TRANSPRT.PRG)
*!               : ADJCOLOR           (procedure in TRANSPRT.PRG)
*!               : ADJFONT            (procedure in TRANSPRT.PRG)
*!               : GETBANDINDEX       (procedure in TRANSPRT.PRG)
*!               : CVTREPORTHORIZONTAL(function  in TRANSPRT.PRG)
*!               : CVTRPTLINES()      (function  in TRANSPRT.PRG)
*!               : ADJTEXT            (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE rptobjconvert
PARAMETER m.bandcount
PRIVATE m.bandindex, m.endindex, m.posinband, m.saverec, m.objid, m.origvpos, m.lineheight

IF m.g_grph2grph
   DO grphrptcvt    && map Mac and Windows coordinates
	IF _MAC AND !m.g_newobjmode
	   * We've already lined up all the Mac objects.
	   RETURN
   ENDIF
ENDIF

DO CASE
CASE objtype = c_otband
   * Map height and width of band to proper values

   DO CASE
   CASE m.g_char2grph AND emptyband(uniqueid)
      REPLACE height WITH 0
   CASE m.g_grph2grph
      * No conversion necessary.
   OTHERWISE
      m.lineheight = cvtreportvertical(HEIGHT)
      IF m.g_grph2char AND BETWEEN(m.lineheight,1.00,1.10) AND objcode = 4
         * This is a heuristic rule to make quick reports and other reports with
         * a single-line detail band transport to DOS correctly.  Sometimes the bands
         * will be just a little larger than one line in Windows.
         REPLACE height WITH 1
      ELSE
         REPLACE height WITH CEILING(m.lineheight)
      ENDIF
   ENDCASE

   DO CASE
   CASE m.g_char2grph
      * Map DOS offset field to Windows "if lines less than".  These fields control
      * when the data grouping decides to start a new page.  This data is stored in "width".
      REPLACE WIDTH WITH 10000 * offset / c_linesperinch
   CASE m.g_grph2char
      REPLACE height WITH MAX(1, height)
      REPLACE offset WITH ROUND(WIDTH/10000, 0) * c_linesperinch
   ENDCASE
OTHERWISE
   * Converting a regular object such as a field or line.
   m.origvpos   = vpos
   m.origheight = height

   IF m.g_char2grph AND objtype = c_otbox
      DO adjbox WITH 0
      DO adjcolor
      DO adjfont
   ENDIF

   * Find which band in the "from" platform this object came from
   * Use a vpos expressed in "from" units for this function.
   m.bandindex = getbandindex(m.origvpos, m.bandcount)

   * Since keeping objects in the proper bands is our highest
   * priority, we calculate the new Vpos by determining how many
   * lines into its band an object lies and adding this
   * value (converted) to that band's Vpos in the from platform.
   m.posinband = MAX(cvtreportvertical((vpos - bands[m.bandIndex, c_fmbandvpos])),0)
   REPLACE vpos WITH bands[m.bandIndex, c_tobandvpos] + m.posinband

   * Since vertical lines and boxes can stretch across bands, we need to
   * watch their ending positions.
   IF (objtype = c_otbox AND cvtreportvertical(height) > 1) ;
         OR (objtype = c_otline AND WIDTH < height)
      m.endindex = getbandindex(IIF(m.g_char2grph,m.origvpos+m.origheight-1,;
         m.origvpos + m.origheight), m.bandcount)
      IF m.endindex <> m.bandindex
         *m.endinband = IIF(m.g_char2grph, m.origvpos+m.origheight-.25, m.origvpos+m.origheight) ;
         *   - bands[m.endIndex, c_fmbandvpos]
         m.endinband = m.origvpos+m.origheight - bands[m.endIndex, c_fmbandvpos]
         IF m.g_char2grph
            * Allow for the fact that box characters in DOS appear in the middle of
            * the line, but always stick out into the "end" band a little bit.
            m.endinband = MAX(m.endinband - 0.5,0.25)
         ENDIF
         m.endinband = cvtreportvertical(m.endinband)
         REPLACE height WITH bands[m.endIndex, c_tobandvpos] + m.endinband - vpos
      ELSE
         REPLACE height WITH cvtreportvertical(HEIGHT)
      ENDIF
   ELSE
      REPLACE height WITH cvtreportvertical(height)
   ENDIF

   REPLACE hpos WITH cvtreporthorizontal(hpos)
   REPLACE WIDTH WITH cvtreporthorizontal(WIDTH)
   DO CASE
   CASE m.g_char2grph
      IF objtype = c_otline AND WIDTH > height
         * Handle horizontal lines separately.  They are very sensitive to line
         * height.
         REPLACE height WITH cvtrptlines(height)
      ENDIF
   CASE m.g_grph2char
      IF objtype = c_otbox AND ROUND(height,0) <> 1
         DO adjbox WITH 0
      ENDIF

      REPLACE vpos WITH ROUND(vpos,0)
      REPLACE hpos WITH ROUND(hpos,0)
      REPLACE height WITH ROUND(height,0)
      REPLACE WIDTH WITH ROUND(WIDTH,0)

      * Make sure that this object will not extend past the end of the last
      * band, which leads to "invalid report" errors on DOS.
      IF m.bandindex = m.bandcount AND ;
            (vpos + height ;
            > bands[m.bandIndex,c_tobandvpos] ;
            + bands[m.bandIndex,c_tobandheight])
         * Can we move the object up so that it fits?
         IF height <= bands[m.bandIndex, c_tobandheight]
            * It will fit if we scootch it up a little.
            REPLACE vpos WITH vpos -;
               (bands[m.bandIndex,c_tobandheight] - height)
         ELSE
            * No room for it at all.  Crop the height.  Make as much fit as possible.
            REPLACE vpos   WITH bands[m.bandIndex,c_tobandvpos]
            REPLACE height WITH bands[m.bandIndex,c_tobandheight]
         ENDIF
      ENDIF

      DO CASE
      CASE objtype = c_ottext
         REPLACE height WITH 1
         DO adjtext WITH WIDTH
         REPLACE WIDTH WITH LEN(expr)-2

      CASE objtype = c_otrepfld AND height < 1
         REPLACE height WITH 1

      ENDCASE
      IF ROUND(hpos,0) = -1
         REPLACE hpos WITH 0
      ENDIF
   ENDCASE

   * Guarantee that we are in the right band.
   IF vpos > bands[m.bandIndex,c_tobandvpos] ;
         + bands[m.bandIndex,c_tobandheight] - 1
      REPLACE vpos WITH bands[m.bandIndex,c_tobandvpos] ;
         + bands[m.bandIndex,c_tobandheight] - 1
   ENDIF

   IF vpos < 0
      REPLACE vpos WITH 0
   ENDIF
ENDCASE

IF height <= 0
   REPLACE height WITH 1
ENDIF

RETURN

*
* GetBandIndex - Given a Vpos (from platform), this function returns the
*      index in the Band array of the band which this Vpos lies in.
*
*!*****************************************************************************
*!
*!      Procedure: GETBANDINDEX
*!
*!      Called by: RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE getbandindex
PARAMETER m.vpos, m.bandcount
PRIVATE m.loop
FOR m.loop = 1 TO m.bandcount
   IF m.vpos >= bands[m.loop,c_fmbandvpos] ;
         AND m.vpos < bands[m.loop,c_fmbandvpos]+bands[m.loop,c_fmbandheight]
      RETURN m.loop
   ENDIF
ENDFOR
RETURN m.bandcount    && drop them into the bottom band as a default

*
* BandInfo - Fills a predefined array named Band as follows.
*   bands[1] = Start Position in To platform.
*   bands[2] = Height in To platform.
*   bands[3] = Start Position in From platform.
*   bands[4] = Height in From platform.
*
*!*****************************************************************************
*!
*!       Function: BANDINFO
*!
*!      Called by: NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : RPTCONVERT         (procedure in TRANSPRT.PRG)
*!
*!          Calls: RESIZEBAND         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION bandinfo
PRIVATE m.saverec, m.bandcount, m.loop, ;
   m.pagefooter, m.pageheader, m.colheader, m.colfooter, ;
   m.toposition, m.fromposition, m.objcode, m.expr

m.toposition   = 0
m.fromposition = 0
m.bandcount    = 0
m.colheader    = 0
m.colfooter    = 0
m.pageheader   = 0
m.pagefooter   = 0

SCAN FOR platform = m.g_toplatform AND objtype = c_otband
   m.bandcount = m.bandcount + 1

   DO CASE
   CASE objcode = 1
      m.pageheader = m.bandcount
   CASE objcode = 2
      m.colheader  = m.bandcount
   CASE objcode = 6
      m.colfooter  = m.bandcount
   CASE objcode = 7
      m.pagefooter = m.bandcount
   ENDCASE

   * The To fields are already converted at this point
   bands[m.bandCount,c_tobandvpos] = m.toposition
   DO CASE
   CASE m.g_char2grph
      bands[m.bandCount,c_tobandheight] ;
         = HEIGHT + m.g_bandheight + (m.g_bandfudge/m.g_pixelsize)
   CASE m.g_grph2char
      bands[m.bandCount,c_tobandheight] = height
   CASE m.g_grph2grph
      bands[m.bandCount,c_tobandheight] = height + ;
         m.g_bandheight + (m.g_bandfudge/m.g_pixelsize)
   ENDCASE

   m.objcode = objcode
   m.expr    = expr
   m.saverec = RECNO()

   IF !EMPTY(expr)
      LOCATE FOR platform = m.g_fromplatform AND ;
         objtype = c_otband AND objcode = m.objcode AND expr = m.expr
   ELSE
      * The expression is empty, which means this is probably a group footer.  There could
      * be many of them, all empty.  We have to find the right one.
      GOTO TOP
      * Figure out which occurrence this one is.
      COUNT TO m.seq FOR platform = m.g_toplatform AND ;
         objtype = c_otband AND objcode = m.objcode AND EMPTY(expr) ;
         AND RECNO() <= m.saverec
      GOTO TOP
      * Now find the corresponding band in the "from" platform
      LOCATE FOR platform = m.g_fromplatform AND ;
         objtype = c_otband AND objcode = m.objcode AND EMPTY(expr)
      m.i = 1
      DO WHILE FOUND() AND m.i < m.seq
         m.i = m.i + 1
         CONTINUE
      ENDDO
   ENDIF
   IF FOUND()
      bands[m.bandCount,c_fmbandvpos] = m.fromposition
      DO CASE
      CASE m.g_char2grph
         bands[m.bandCount,c_fmbandheight] = height
      CASE m.g_grph2char
         bands[m.bandCount,c_fmbandheight] = height ;
         	+ IIF(m.g_fromplatform = c_macname,m.g_macbandheight, m.g_winbandheight)
      CASE m.g_grph2grph
         bands[m.bandCount,c_fmbandheight] = height + m.g_bandheight
      ENDCASE

      m.fromposition = m.fromposition + bands[m.bandCount,c_fmbandheight]

      IF m.g_grph2char
         * Resize 'to' band if necessary to account for boxes that narrowly
         * surround text on a graphic platform.  Sometimes the box can be
         * tightly against the text such that the graphical band appears to
         * be only two rows high.  We need three rows to display the box in
         * a character platform
         bands[m.bandCount,c_tobandheight] = ;
            resizeband(bands[m.bandCount,c_tobandheight], ;
            bands[m.bandCount,c_fmbandvpos  ], ;
            bands[m.bandCount,c_fmbandheight])
      ENDIF
   ELSE
      bands[m.bandCount,c_fmbandvpos] = 9999999
      bands[m.bandCount,c_fmbandheight] = 9999999
   ENDIF


   m.toposition = m.toposition + bands[m.bandCount,c_tobandheight]

   GOTO RECORD (m.saverec)

   IF m.g_grph2char
      * Stuff the newly recomputed height into the DOS record
      REPLACE height WITH bands[m.bandCount,c_tobandheight]
   ENDIF

ENDSCAN


IF !m.g_grph2grph
   * We don't want to have any column headers/footers in the character
   * products so we need to combine them with the page headers/footers.
   IF m.colfooter > 0 AND m.pagefooter > 0
      bands[m.pageFooter,c_tobandvpos] = bands[m.colFooter,c_tobandvpos]
      bands[m.pageFooter,c_tobandheight];
         = bands[m.pageFooter,c_tobandheight] ;
         + bands[m.colFooter,c_tobandheight]
      bands[m.pageFooter,c_fmbandvpos] = bands[m.colFooter,c_fmbandvpos]
      bands[m.pageFooter,c_fmbandheight] ;
         = bands[m.pageFooter,c_fmbandheight] ;
         + bands[m.colFooter,c_fmbandheight]

      LOCATE FOR platform = m.g_toplatform ;
         AND objtype = c_otband AND objcode = 6
      IF FOUND()
         DELETE
      ENDIF

      LOCATE FOR platform = m.g_toplatform ;
         AND objtype = c_otband AND objcode = 7
      IF FOUND()
         REPLACE height WITH height + bands[m.colFooter,c_tobandheight]
      ENDIF

      =ADEL(bands,m.colfooter)
      m.bandcount = m.bandcount - 1
   ENDIF

   IF m.colheader > 0 AND m.pageheader > 0
      bands[m.pageHeader,c_tobandheight];
         = bands[m.pageHeader,c_tobandheight] ;
         + bands[m.colHeader,c_tobandheight]
      bands[m.pageHeader,c_fmbandheight] ;
         = bands[m.pageHeader,c_fmbandheight] ;
         + bands[m.colHeader,c_fmbandheight]

      LOCATE FOR platform = m.g_toplatform AND objtype = c_otband AND objcode = 2
      IF FOUND()
         DELETE
      ENDIF

      LOCATE FOR platform = m.g_toplatform AND objtype = c_otband AND objcode = 1
      IF FOUND()
         REPLACE height WITH height + bands[m.colHeader,c_tobandheight]
      ENDIF

      =ADEL(bands,m.colheader)
      m.bandcount = m.bandcount - 1
   ENDIF
ENDIF
RETURN m.bandcount


*!*****************************************************************************
*!
*!      Procedure: CLONEBAND
*!
*!      Called by: RPTCONVERT         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE cloneband
* Copy the band header record data into the respective footer bands.  Data in band header
* and footer records must match on Windows.  The main data that needs to match is the
* group expression and things like how many spaces to require after a heading
* before doing a page break.
PRIVATE m.in_area, m.in_rec, m.pivot, m.ouniqid, m.ovpos, m.ohpos, m.owidth, m.oheight,;
   m.oobjcode, m.headband
IF m.g_char2grph
   m.in_area = SELECT()
   m.in_rec = RECNO()
   * First find the detail band.  It acts as a pivot.
   GOTO TOP
   LOCATE FOR platform = m.g_toplatform ;
      AND objtype = c_otband ;
      AND objcode = 4     && detail band has code = 4
   IF !FOUND()
      * Return and make the best of it
      RETURN
   ENDIF
   m.pivot = RECNO()

   * Scan for each of the header bands
   SCAN FOR platform = m.g_toplatform ;
         AND objtype = c_otband ;
         AND objcode < 4 AND objcode > 0
      SCATTER MEMVAR MEMO

      m.headband = RECNO()

      * Go to the matching footer band record
      GOTO (m.pivot + (m.pivot - RECNO()))

      * Store the values we don't want to copy from the header
      m.ouniqid  = uniqueid
      m.ovpos    = vpos
      m.ohpos    = hpos
      m.oheight  = height
      m.oobjcode = objcode

      * Stuff header data into this footer band
      GATHER MEMVAR MEMO

      * Restore the data we didn't want to copy from the header
      REPLACE vpos WITH m.ovpos, hpos WITH m.ohpos, ;
         height WITH m.oheight, objcode WITH m.oobjcode, ;
         uniqueid WITH m.ouniqid

      GOTO (m.headband)

   ENDSCAN
   SELECT (m.in_area)
   GOTO (MIN(m.in_rec,RECCOUNT()))
ENDIF

RETURN

*
* RESIZEBAND - Resize the character mode report band to accommodate
* boxes, etc.
*
*!*****************************************************************************
*!
*!      Procedure: RESIZEBAND
*!
*!      Called by: BANDINFO()         (function  in TRANSPRT.PRG)
*!
*!          Calls: CVTREPORTVERTICAL()(function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE resizeband
PARAMETER tobandheight, fmbandvpos, fmbandheight

PRIVATE in_rec, minbandheight
m.in_rec = RECNO()
m.minbandheight = m.tobandheight
IF m.g_grph2char
   * Search for boxes that lie entirely within this band.
   SCAN FOR platform = m.g_fromplatform ;
         AND objtype = c_otbox AND vpos >= m.fmbandvpos ;
         AND vpos + height <= m.fmbandvpos + m.fmbandheight
      * The box needs to be expanded
      m.minbandheight = MAX(m.minbandheight,cvtreportvertical(height)+1)
      * If there is a box in the band, always make it at least three rows
      m.minbandheight = MAX(m.minbandheight,3)
   ENDSCAN
ENDIF
GOTO RECORD (m.in_rec)
RETURN CEILING(m.minbandheight)

*
* BandHeight - Given a band ID and platform, this function reurns the band's
*      starting position in that platform.
*
*!*****************************************************************************
*!
*!       Function: BANDPOS
*!
*!      Called by: NEWBANDS           (procedure in TRANSPRT.PRG)
*!               : EMPTYBAND()        (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION bandpos
PARAMETER m.objid, m.platform
PRIVATE m.saverec, m.bandstart
m.saverec = RECNO()
m.bandstart = 0

SCAN FOR platform = m.platform AND objtype = c_otband
   IF uniqueid <> m.objid
      IF m.platform = c_dosname OR m.platform = c_unixname
         m.bandstart = m.bandstart + height
      ELSE
         m.bandstart = m.bandstart + height + m.g_bandheight + (m.g_bandfudge/m.g_pixelsize)
      ENDIF
   ELSE
      LOCATE FOR .F.
   ENDIF
ENDSCAN

GOTO RECORD (m.saverec)
RETURN m.bandstart

*
* EmptyBand - Given a band ID, this funtion determines if the band is empty.
*
*!*****************************************************************************
*!
*!       Function: EMPTYBAND
*!
*!      Called by: RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!
*!          Calls: BANDPOS()          (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION emptyband
PARAMETER m.id
PRIVATE m.saverec, m.bandstart, m.bandheight, m.retval
IF m.g_toplatform = c_dosname OR m.g_toplatform = c_unixname
   RETURN .F.
ENDIF

m.saverec = RECNO()
m.retval = .F.

LOCATE FOR platform = m.g_fromplatform AND uniqueid = m.id
IF FOUND()
   m.bandheight = height
   m.bandstart = bandpos(m.id, m.g_fromplatform)
   * Look for objects in this band
   LOCATE FOR platform = m.g_fromplatform AND ;
      (objtype = c_otline OR objtype = c_otbox OR ;
      objtype = c_ottext OR objtype = c_otrepfld) AND ;
      vpos >= m.bandstart AND vpos < m.bandstart + m.bandheight
   IF !FOUND() AND m.g_char2grph
      * Look for a DOS box or line that ends in the band
      GOTO TOP
      LOCATE FOR platform = m.g_fromplatform AND ;
         INLIST(objtype,c_otbox, c_otline) AND ;
         vpos + height - 1 >= m.bandstart AND vpos + height - 1 < m.bandstart + m.bandheight
   ENDIF
   m.retval = !FOUND()
ENDIF

GOTO RECORD (m.saverec)
RETURN m.retval

*
* GETBANDCODE - returns band objcode given a vpos
*
*!*****************************************************************************
*!
*!       Function: GETBANDCODE
*!
*!      Called by: SUPPRESSBLANKLINES (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getbandcode
PARAMETER m.thisvpos
PRIVATE m.in_num, m.retcode
retcode = -1
m.in_num = RECNO()
m.startvpos = 0

IF INLIST(objtype,c_otheader, c_otband, c_otrel, c_otworkar, c_otindex)
   RETURN -1
ENDIF

SET FILTER TO platform = m.g_toplatform AND (objtype = c_otband)
GOTO TOP
DO WHILE m.startvpos <= m.thisvpos AND !EOF()
   IF m.startvpos + height +m.g_bandheight > m.thisvpos
      retcode = objcode
      EXIT
   ELSE
      m.startvpos = m.startvpos + height + m.g_bandheight
      SKIP
   ENDIF
ENDDO
SET FILTER TO
GOTO m.in_num
RETURN retcode


*!*****************************************************************************
*!
*!       Function: GRPHRPTCVT
*!
*!*****************************************************************************
PROCEDURE grphrptcvt
PRIVATE m.bandnum
* Convert single report object from one graphical platform to another
* The vpos adjustment reflects the fact that Windows report bands are
* 20 pixels high while Mac ones are 15 pixels high.
IF m.g_filetype = c_report    && labels don't require this conversion
	DO CASE
		CASE _WINDOWS
		   	IF objtype = c_ottext
		      	* Compute text object width exactly
		      	REPLACE width  WITH gettextwidth(expr)
		   	ENDIF
		CASE _MAC
			DO CASE
			   	CASE objtype = c_ottext
			      	* Compute text object width exactly
			      	REPLACE width  WITH gettextwidth(expr)
			   	CASE objtype = c_otpicture
			      	REPLACE width WITH width * 96 / 72
		   	ENDCASE
			IF !m.g_newobjmode OR objtype = c_otband
				m.bandnum = getbandnum(vpos,"WINDOWS")
				IF objtype <> c_otline OR height > width
					*- REPLACE height WITH height * 96 / 72		&&commented this out -- gives bad results (jd 3/24/96)
				ENDIF
				REPLACE vpos    WITH (vpos - ((m.bandnum-1) * (5/96) * 10000)) &&  * 96 / 72
			ENDIF
   ENDCASE
ENDIF

*!*****************************************************************************
*!
*!       Function: GETBANDNUM
*!
*!*****************************************************************************
FUNCTION getbandnum
PARAMETER m.theVpos, m.thePlat
PRIVATE m.bandno, m.past, m.cumvpos, m.therec
* Returns the band number that an object falls into.
m.bandno = 0
m.past = .F.
m.cumvpos = 0
m.therec = RECNO()
SCAN FOR platform = m.thePlat AND objtype = c_otband AND !m.past
   m.cumvpos = m.cumvpos + height
   IF m.bandno > 0
      m.cumvpos = m.cumvpos + m.g_bandheight + (m.g_bandfudge/m.g_pixelsize)
   ENDIF
   IF m.cumvpos >= m.theVpos
      m.past = .T.
   ENDIF
   m.bandno = m.bandno + 1
ENDSCAN
GOTO m.therec
IF m.past
   RETURN m.bandno
ELSE
   RETURN -1    && couldn't find the band
ENDIF

*!*****************************************************************************
*!
*!       Function: GETTEXTWIDTH
*!
*!*****************************************************************************
FUNCTION gettextwidth
PARAMETER m.strg
* Figure out how many 10000-ths of an inch a text object requires

* Don't count the quotation marks
m.strg = ALLTRIM(CHRTRANC(expr,CHR(0),""))
IF LEFT(m.strg,1) = '"'
   m.strg = SUBSTR(m.strg,2)
ENDIF
IF RIGHT(m.strg,1) = '"'
   m.strg = SUBSTR(m.strg,1,LEN(m.strg)-1)
ENDIF

RETURN TXTWIDTH(m.strg,fontface,fontsize,num2style(fontstyle)) * ;
      FONTMETRIC(6,fontface,fontsize,num2style(fontstyle)) * 10000 / m.g_pixelsize

*
* CvtReportVertical - Convert report vertical dimensions between 10000ths of an inch and characters
*      depending on the to platform.  (This function is for vertical dimensions only).
*
*!*****************************************************************************
*!
*!       Function: CVTREPORTVERTICAL
*!
*!      Called by: RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : RESIZEBAND         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION cvtreportvertical
PARAMETER m.units
DO CASE
CASE m.g_grph2char
   RETURN m.units/10000 * c_linesperinch
CASE m.g_char2grph
   RETURN (m.units * m.g_rptlinesize) + (5000/m.g_pixelsize)
OTHERWISE
   RETURN m.units
ENDCASE

*
* CvtReportWidth - Convert report horizontal dimensions between 10000ths of an inch
*      and chanracters depending on the to platform.
*
*!*****************************************************************************
*!
*!       Function: CVTREPORTHORIZONTAL
*!
*!      Called by: RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION cvtreporthorizontal
PARAMETER m.units
DO CASE
CASE m.g_grph2char
   RETURN m.units/10000 * c_charsperinch
CASE m.g_char2grph
   RETURN m.units * m.g_rptcharsize
OTHERWISE
   RETURN m.units
ENDCASE
*!*****************************************************************************
*!
*!       Function: CVTRPTLINES
*!
*!      Called by: RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION cvtrptlines
* Adjust the height of horizontal lines
PARAMETER m.height
IF _MAC
   * Adjust for 72 to 96 conversion
   m.height = m.height * 72 / 96
ENDIF
DO CASE
CASE g_char2grph
   DO CASE
   CASE BETWEEN(m.height,0,200)
      RETURN 104
   CASE BETWEEN(m.height,200,600)
      RETURN 520
   CASE BETWEEN(m.height,600,850)
      RETURN 850
   OTHERWISE
      RETURN m.height
   ENDCASE
OTHERWISE
   RETURN m.height
ENDCASE

*
* MergeLabelObjects - Combines report objects which lie on the same line
*      when going from a graphical platform to a character platform.
*
*!*****************************************************************************
*!
*!      Procedure: MERGELABELOBJECTS
*!
*!      Called by: ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!
*!          Calls: LABELOBJMERGE      (procedure in TRANSPRT.PRG)
*!
*!        Indexes: TEMP                   (tag)
*!
*!*****************************************************************************
PROCEDURE mergelabelobjects

IF !m.g_grph2grph
	INDEX ON platform+STR(vpos,3)+STR(hpos,3) TAG temp

	SCAN FOR platform = m.g_toplatform AND !DELETED() AND ;
      	(objtype = c_otrepfld OR objtype = c_ottext OR objtype = c_otbox OR objtype = c_otline)
   	DO labelobjmerge WITH RECNO()
	ENDSCAN

	DELETE TAG temp
ENDIF
RETURN

*
* LabelObjMerge - Given a record which is a report object, this function tries to find a label
*      object on the same line and combine them.  If no label object exists on the line, the
*      record is turned into one.
*
*!*****************************************************************************
*!
*!      Procedure: LABELOBJMERGE
*!
*!      Called by: MERGELABELOBJECTS  (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE labelobjmerge
PARAMETER m.recno
PRIVATE m.saverec, m.vpos, m.hpos, m.width, m.height, m.expr, m.type, m.picture

m.saverec = RECNO()
GOTO RECORD (m.recno)

m.vpos = vpos
m.width = WIDTH
m.expr = expr
m.type = fillchar
m.picture = PICTURE
DELETE

LOCATE FOR platform = m.g_toplatform AND !DELETED() AND ;
   objtype = c_ot20lbxobj AND vpos = m.vpos
IF FOUND()
   REPLACE expr WITH expr + "," + m.expr
ELSE
   GOTO RECORD (m.recno)
   RECALL
   REPLACE objtype WITH c_ot20lbxobj
ENDIF

GOTO RECORD (m.saverec)

*
* AddLabelBlanks - Adds sufficient blank lines to make the converted lines
*
*!*****************************************************************************
*!
*!      Procedure: ADDLABELBLANKS
*!
*!           Uses: M.G_SCRNALIAS
*!
*!*****************************************************************************
PROCEDURE addlabelblanks
PRIVATE m.linecount, m.last, m.scanloop
SELECT vpos FROM m.g_scrnalias ;
   WHERE !DELETED() AND platform = m.g_toplatform AND objtype = c_ot20lbxobj ;
   ORDER BY vpos ;
   INTO ARRAY lines

m.linecount = _TALLY
m.last = 0
FOR m.scanloop = 1 TO lines[m.linecount]
   IF ASCAN(lines, m.scanloop) = 0
      APPEND BLANK
      REPLACE platform WITH m.g_toplatform
      REPLACE objtype WITH c_ot20lbxobj
      REPLACE vpos WITH m.lines
   ENDIF
ENDFOR
RETURN

*
* LinesBetween - Removes all the whitespace from the bottom of the detail
*      band and puts it in lines between.
*
*!*****************************************************************************
*!
*!      Procedure: LINESBETWEEN
*!
*!      Called by: ALLGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE linesbetween
PRIVATE m.linecount, m.blanklines

IF !m.g_grph2grph
	COUNT TO m.linecount FOR platform = m.g_toplatform AND objtype = c_ot20lbxobj

	LOCATE FOR platform = m.g_toplatform AND objtype = c_otband AND objcode = 4
	IF FOUND() AND m.linecount < height
   	m.blanklines = height - m.linecount
   	REPLACE height WITH m.linecount
   	LOCATE FOR platform = m.g_toplatform AND objtype = c_ot20label
   	IF FOUND()
      	REPLACE penblue WITH m.blanklines
   	ENDIF
	ENDIF
ENDIF

*
* labelBands - Adds the group records needed by a graphical label
*
*!*****************************************************************************
*!
*!      Procedure: LABELBANDS
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE labelbands
PRIVATE m.lbxheight, m.lbxwidth, m.lbxlinesbet

LOCATE FOR platform = m.g_fromplatform AND objtype = c_otband AND objcode = 4
IF FOUND()
   m.lbxheight = height
ENDIF

LOCATE FOR platform = m.g_fromplatform AND objtype = c_ot20label
IF FOUND()
   DO CASE
   CASE name = '3 1/2" x 15/16" x 1' AND penblue = 1 AND ;
         WIDTH = 35 AND m.lbxheight = 5 AND vpos = 1 AND hpos = 0 AND height = 0
      m.lbxheight = (15/16) * 10000
      m.lbxwidth = -1
      m.lbxlinesbet = m.lbxheight / 5

   CASE name = '3 1/2" x 15/16" x 2' AND penblue = 1 AND ;
         WIDTH = 35 AND m.lbxheight = 5 AND vpos = 2 AND hpos = 0 AND height = 2
      m.lbxheight = (15/16) * 10000
      m.lbxwidth = (3 + (1/2)) * 10000
      m.lbxlinesbet = m.lbxheight / 5

   CASE name = '3 1/2" x 15/16" x 3' AND penblue = 1 AND ;
         WIDTH = 35 AND m.lbxheight = 5 AND vpos = 3 AND hpos = 0 AND height = 2
      m.lbxheight = (15/16) * 10000
      m.lbxwidth = (3 + (1/2)) * 10000
      m.lbxlinesbet = m.lbxheight / 5

   CASE name = '3 2/10" x 11/12" x 3 (Cheshire)' AND penblue = 1 AND ;
         WIDTH = 32 AND m.lbxheight = 5 AND vpos = 3 AND hpos = 0 AND height = 2
      m.lbxheight = (11/12) * 10000
      m.lbxwidth = (3 + (2/10)) * 10000
      m.lbxlinesbet = m.lbxheight / 5

   CASE name = '3" x 5 Rolodex' AND penblue = 4 AND ;
         WIDTH = 50 AND m.lbxheight = 14 AND vpos = 1 AND hpos = 0 AND height = 0
      m.lbxheight = 5 * 10000
      m.lbxwidth = -1
      m.lbxlinesbet = 4 * (m.lbxheight / 14)

   CASE name = '4" x 1 7/16" x 1' AND penblue = 1 AND ;
         WIDTH = 40 AND m.lbxheight = 8 AND vpos = 1 AND hpos = 0 AND height = 0
      m.lbxheight = (1 + (7/16)) * 10000
      m.lbxwidth = -1
      m.lbxlinesbet = m.lbxheight / 8

   CASE name = '4" x 2 1/4 Rolodex' AND penblue = 1 AND ;
         WIDTH = 40 AND m.lbxheight = 10 AND vpos = 1 AND hpos = 0 AND height = 0
      m.lbxheight = (2 + (1/4)) * 10000
      m.lbxwidth = -1
      m.lbxlinesbet = m.lbxheight / 10

   CASE name = '6 1/2" x 3 5/8 Envelope' AND penblue = 8 AND ;
         WIDTH = 65 AND m.lbxheight = 14 AND vpos = 1 AND hpos = 0 AND height = 0
      m.lbxheight = (3 + (5/8)) * 10000
      m.lbxwidth = -1
      m.lbxlinesbet = 8 * (m.lbxheight / 14)

   CASE name = '9 7/8" x 7 1/8 Envelope' AND penblue = 8 AND ;
         WIDTH = 78 AND m.lbxheight = 17 AND vpos = 1 AND hpos = 0 AND height = 0
      m.lbxheight = (7 + (1/8)) * 10000
      m.lbxwidth = -1
      m.lbxlinesbet = 8 * (m.lbxheight / 17)

   OTHERWISE
      m.lbxheight = m.lbxheight * m.g_rptlinesize
      m.lbxwidth = IIF(vpos > 1, WIDTH * m.g_rptcharsize, -1)
      m.lbxlinesbet = penblue * m.g_rptlinesize
   ENDCASE
ELSE
   RETURN
ENDIF

LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
IF FOUND()
   REPLACE vpos WITH IIF(vpos > 1, vpos * m.g_rptlinesize, 1)
   REPLACE WIDTH WITH m.lbxwidth
   REPLACE hpos WITH hpos * m.g_rptcharsize      && Left margin
   REPLACE height WITH height * m.g_rptcharsize   && Spaces Between Columns
ENDIF

LOCATE FOR platform = m.g_toplatform AND objtype = c_otband AND objcode = 4
IF FOUND()
   REPLACE height WITH m.lbxheight + m.lbxlinesbet
ENDIF

*
* labelLines - Converts the character style label objects to graphical report objects
*
*!*****************************************************************************
*!
*!      Procedure: LABELLINES
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: ADJFONT            (procedure in TRANSPRT.PRG)
*!               : ADJCOLOR           (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE labellines
PRIVATE m.bandstart, m.linecount, m.thermstep, m.lbxwidth, ;
   m.saverec, m.nextexpr, m.loop

COUNT TO m.thermstep FOR platform = m.g_toplatform AND objtype = c_ot20lbxobj
m.thermstep = 45 / m.thermstep
m.bandstart = 4166.667

LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
IF WIDTH != -1
   m.lbxwidth = WIDTH
ELSE
   LOCATE FOR platform = m.g_fromplatform AND objtype = c_ot20label
   m.lbxwidth = WIDTH * m.g_rptcharsize
ENDIF

m.linecount = 0

SCAN FOR platform = m.g_toplatform AND objtype = c_ot20lbxobj AND !DELETED()
   REPLACE expr WITH ALLTRIM(expr)
   REPLACE objtype WITH c_otrepfld
   REPLACE objcode WITH 0
   REPLACE vpos WITH m.bandstart + (m.linecount * m.g_rptlinesize)
   REPLACE hpos WITH 0
   REPLACE height WITH m.g_rptlinesize
   REPLACE WIDTH WITH m.lbxwidth
   REPLACE fillchar WITH "C"
   REPLACE FLOAT WITH .F.
   REPLACE STRETCH WITH .F.
   REPLACE spacing WITH 12
   REPLACE offset WITH 0
   REPLACE totaltype WITH 0
   REPLACE TOP WITH .T.
   REPLACE resettotal WITH 1
   REPLACE supalways WITH .T.
   REPLACE supovflow WITH .F.
   REPLACE suprpcol WITH 3
   REPLACE supgroup WITH 0
   REPLACE supvalchng WITH .F.

   DO adjfont
   DO adjcolor

   m.loop = (RIGHT(expr,1) = ";")
   DO WHILE m.loop
      m.saverec = RECNO()
      SKIP
      DO WHILE platform = m.g_toplatform AND objtype = c_ot20lbxobj AND DELETED()
         SKIP
      ENDDO
      IF platform = m.g_toplatform AND objtype = c_ot20lbxobj
         DELETE
         m.nextexpr = expr
         GOTO RECORD (m.saverec)
         REPLACE expr WITH expr + m.nextexpr
         REPLACE height WITH height + m.g_rptlinesize
         m.loop = (RIGHT(expr,1) = ";")
      ELSE
         GOTO RECORD (m.saverec)
         m.loop = .F.
      ENDIF
   ENDDO

   m.linecount = m.linecount + 1

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDSCAN

*
* calcpositions - Calculate each objects position as a percentage across
*            and down the window.
*
*!*****************************************************************************
*!
*!      Procedure: CALCPOSITIONS
*!
*!      Called by: ALLOTHERS          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE calcpositions
PARAMETER m.index
PRIVATE m.record, m.vert, m.horiz, m.width, m.numothers, m.thermstep, m.i
*
* Search for the original platform records and establish the horizontal
* and vertical positioning percentages.
*

objectpos[m.index, 1] = hpos / m.g_windwidth
objectpos[m.index, 2] = vpos / m.g_windheight
objectpos[m.index, 3] = uniqueid
objectpos[m.index, 4] = objtype
objectpos[m.index, 5] = .F.                && right aligned with object above or below?
objectpos[m.index, 6] = hpos
objectpos[m.index, 7] = WIDTH
objectpos[m.index, 8] = spacing
objectpos[m.index, 9] = PICTURE

IF objtype = c_ottext
   m.record = RECNO()
   m.vert1 = vpos
   m.horiz = hpos
   m.endpos = hpos + WIDTH

   LOCATE FOR objtype = c_ottext AND hpos != m.horiz AND ;
      m.vert1 - 1 = vpos AND hpos + WIDTH = m.endpos
   IF FOUND()
      objectpos[m.index,5] = .T.
      DO WHILE FOUND()
         IF objectpos[m.index, 7] < WIDTH
            objectpos[m.index, 7] = WIDTH
         ENDIF
         m.vert = vpos
         LOCATE FOR objtype = c_ottext AND hpos != m.horiz AND ;
            m.vert - 1 = vpos AND hpos + WIDTH = m.endpos
      ENDDO
   ENDIF
   LOCATE FOR objtype = c_ottext AND hpos != m.horiz AND ;
      m.vert1 + 1 = vpos AND hpos + WIDTH = m.endpos

   IF FOUND()
      objectpos[m.index,5] = .T.
      DO WHILE FOUND()
         IF objectpos[m.index, 7] < WIDTH
            objectpos[m.index, 7] = WIDTH
         ENDIF
         m.vert = vpos
         LOCATE FOR objtype = c_ottext AND hpos != m.horiz AND ;
            m.vert + 1 = vpos AND hpos + WIDTH = m.endpos
      ENDDO
   ENDIF

   GOTO RECORD m.record
   IF objectpos[m.index, 5]
      objectpos[m.index, 6] = hpos + WIDTH - 1
      objectpos[m.index, 1] = (hpos + WIDTH) / m.g_windwidth
   ENDIF

ENDIF

*
* calcwindowdimensions - Calculate the needed Height and Width for the new window
*
*!*****************************************************************************
*!
*!      Procedure: CALCWINDOWDIMENSIONS
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: FINDWIDEROBJECTS   (procedure in TRANSPRT.PRG)
*!               : HORIZBUTTON()      (function  in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : REPOOBJECTS        (procedure in TRANSPRT.PRG)
*!
*!        Indexes: UNIQUEID               (tag)
*!
*!*****************************************************************************
PROCEDURE calcwindowdimensions
PRIVATE m.i, m.curline, m.largestobj, m.lineheight, m.adjwindowwidth, m.thermstep

*- set relation off before indexing and creating the new relation
SELECT (m.g_fromobjonlyalias)
SET RELATION OFF INTO (m.g_scrnalias)
SELECT (m.g_scrnalias)
INDEX ON uniqueid + platform TAG uniqueid OF (m.g_tempindex) ADDITIVE
SELECT (m.g_fromobjonlyalias)
SET RELATION TO uniqueid+m.g_toplatform INTO (m.g_scrnalias) ADDITIVE
SELECT (m.g_scrnalias)

m.adjwindwidth = 0
DO findwiderobjects WITH m.adjwindwidth

=ASORT(objectpos,2)
STORE 0 TO m.curline, m.largestobj, m.lineheight, m.adjheight
m.thermstep = 10 / m.objindex

FOR m.i = 1 TO m.objindex

   IF objectpos[m.i,2] != m.curline
      m.adjheight = m.adjheight + m.lineheight
      STORE 0 TO m.lineheight, m.largestobj
      m.curline = objectpos[m.i,2]
   ENDIF

   IF m.largestobj != 3
      DO CASE
      CASE objectpos[m.i, 4] = c_ottxtbut AND m.largestobj < 3
         IF !horizbutton(objectpos[m.i, 9])
            m.numitems = OCCURS(';',objectpos[m.i, 9]) + 1
            m.lineheight = c_adjtbtn * m.numitems
         ELSE
            m.lineheight = c_adjtbtn
         ENDIF
         m.largestobj = 3

      CASE (objectpos[m.i, 4] = c_otradbut AND m.largestobj < 2) ;
            OR (objectpos[m.i, 4] = c_otchkbox AND m.largestobj < 2)
         IF objectpos[m.i, 4] = c_otradbut AND !horizbutton(objectpos[m.i, 9])
            m.numitems = OCCURS(';',objectpos[m.i, 9]) + 1
            m.lineheight = c_adjrbtn * m.numitems
         ELSE
            m.lineheight = c_adjrbtn
         ENDIF
         m.largestobj = 2

      CASE (objectpos[m.i, 4] = c_otlist AND m.largestobj < 1) ;
            OR (objectpos[m.i, 4] = c_otfield AND m.largestobj < 1)
         m.lineheight = c_adjlist
         m.largestobj = 1

      ENDCASE
   ENDIF
   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury

ENDFOR
m.adjheight = m.adjheight + m.lineheight
LOCATE FOR platform = m.g_toplatform AND objtype = 1
IF FOUND()
   REPLACE WIDTH WITH WIDTH + m.adjwindwidth
   DO repoobjects WITH HEIGHT + m.adjheight
ENDIF

RETURN

*
* findWiderObjects - Find objects which have changed in size
*
*!*****************************************************************************
*!
*!      Procedure: FINDWIDEROBJECTS
*!
*!      Called by: CALCWINDOWDIMENSION(procedure in TRANSPRT.PRG)
*!
*!          Calls: HORIZBUTTON()      (function  in TRANSPRT.PRG)
*!               : SGN()              (function  in TRANSPRT.PRG)
*!               : ADJHPOS            (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE findwiderobjects
PARAMETER m.adjwindowwidth
PRIVATE m.curcol, m.adjcol, m.i, m.rightalignflag, m.numitems, ;
   m.olduniqueid, m.oldwidth, m.buttonflag, m.newwidth, m.adjust, m.thermstep

m.thermstep = 10 / m.objindex

=ASORT(objectpos,6)   && sort on hpos
STORE 0 TO m.curcol, m.adjcol
m.rightalignflag = .F.

FOR m.i = 1 TO m.objindex
   * Start at the leftmost object
   IF objectpos[m.i,6] != m.curcol
      m.adjcol = 0
      m.rightalignflag = .F.
      m.curcol = objectpos[m.i,6]
   ENDIF

   DO CASE
   CASE objectpos[m.i, 4] = c_ottxtbut OR objectpos[m.i, 4] = c_otradbut
      * Count the objects in push buttons and radio buttons
      m.numitems = OCCURS(';',objectpos[m.i, 9]) + 1
      m.olduniqueid = objectpos[m.i, 3]

      IF horizbutton(objectpos[m.i, 9])
         m.oldwidth = (objectpos[m.i, 7] * m.numitems) + ;
            (objectpos[m.i, 8] * (m.numitems - 1))
         m.buttonflag = .T.
      ELSE
         m.buttonflag = .F.
         m.oldwidth = objectpos[m.i, 7]
      ENDIF

   OTHERWISE
      m.buttonflag = .F.
      m.oldwidth = objectpos[m.i, 7]
      m.olduniqueid = objectpos[m.i, 3]

   ENDCASE

   LOCATE FOR uniqueid = m.olduniqueid AND platform = m.g_toplatform
   IF FOUND()
      IF m.buttonflag
         m.newwidth = (WIDTH * m.numitems) + ;
            (spacing * (m.numitems - 1))
      ELSE
         m.newwidth = WIDTH
      ENDIF
      IF m.oldwidth != m.newwidth AND ;
            !(objtype = c_ottext ;
            AND ASC(SUBSTR(expr,2,1))>=179 ;
            AND ASC(SUBSTR(expr,2,1))<=218)
         m.adjust = m.newwidth - m.oldwidth
         IF ABS(m.adjust) > ABS(m.adjcol) OR sgn(m.adjust) <> sgn(m.adjcol)
            IF (!objectpos[m.i,5] OR !m.rightalignflag) AND m.adjust > 0
               * Move everything over
               DO adjhpos WITH m.adjust - m.adjcol, ;
                  IIF(objectpos[m.i,5], objectpos[m.i, 6], ;
                  objectpos[m.i, 6] + objectpos[m.i, 7] - 1)

               * Expand the window
               m.adjwindowwidth = m.adjwindowwidth + m.adjust - m.adjcol

               * AdjCol contains the cumulative adjustment
               m.adjcol = m.adjust

               IF objectpos[m.i, 5]
                  m.rightalignflag = .T.
                  REPLACE hpos WITH hpos + m.adjust - m.adjcol
               ENDIF
            ENDIF
         ENDIF
      ENDIF
   ENDIF
   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDFOR
RETURN

*
* adjHpos - Adjust the horizontal position of objects across as other objects
*       become bigger or smaller.
*
*!*****************************************************************************
*!
*!      Procedure: ADJHPOS
*!
*!      Called by: FINDWIDEROBJECTS   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjhpos
PARAMETER m.adjustment, m.position

SELECT (m.g_fromobjonlyalias)
SCAN FOR platform = m.g_fromplatform AND hpos >= m.position
   REPLACE &g_scrnalias..hpos WITH &g_scrnalias..hpos + m.adjustment
ENDSCAN

* Stretch lines that begin before the wider object and end after it starts.
SCAN FOR platform = m.g_fromplatform AND objtype = c_otbox AND height = 1 AND ;
      hpos < m.position AND hpos + WIDTH - 1 >= m.position
   REPLACE &g_scrnalias..width WITH &g_scrnalias..width + m.adjustment
ENDSCAN
SELECT (m.g_scrnalias)

*!*****************************************************************************
*!
*!       Function: SGN
*!
*!      Called by: FINDWIDEROBJECTS   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION sgn
PARAMETER num
DO CASE
CASE num = 0
   RETURN 0
CASE num > 0
   RETURN 1
CASE num < 0
   RETURN -1
ENDCASE


*
* repoObjects - Reposition objects to the relative positions on the new window.
*      This procedure assumes that the array objectpos is sorted on rows ([m.i, 2]).
*
*!*****************************************************************************
*!
*!      Procedure: REPOOBJECTS
*!
*!      Called by: CALCWINDOWDIMENSION(procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLASTOBJECTLINE()(function  in TRANSPRT.PRG)
*!               : HORIZBUTTON()      (function  in TRANSPRT.PRG)
*!               : ADJBOX             (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE repoobjects
PARAMETER m.windheight
PRIVATE m.windwidth, m.thermstep, m.rightalign, m.saverec, ;
   m.adjust, m.buttonadjust, m.numrb

m.saverec = RECNO()
m.windwidth = WIDTH
m.thermstep = 10 / m.objindex
STORE 0 TO m.adjust, m.buttonadjust, m.numrb

FOR m.i = 1 TO m.objindex

   IF objectpos[m.i,2] != m.curline
      IF m.numrb > 0
         m.adjust = m.adjust + c_vradbtn
         m.numrb = m.numrb - 1
      ENDIF
      m.adjust = m.adjust + m.buttonadjust
      STORE 0 TO m.buttonadjust
      m.curline = objectpos[m.i,2]
   ENDIF

   LOCATE FOR platform = m.g_toplatform AND uniqueid = objectpos[m.i,3]
   IF FOUND()

      g_lastobjectline[1] = getlastobjectline(g_lastobjectline[1], ;
         m.windheight * objectpos[m.i, 2] + m.adjust)

      REPLACE vpos WITH m.windheight * objectpos[m.i, 2] + m.adjust

      IF objectpos[m.i,5]
         m.rightalign = (m.windwidth * objectpos[m.i,1]) - WIDTH
         REPLACE hpos WITH IIF(m.rightalign < 0, 0, m.rightalign)
      ENDIF

      DO CASE
      CASE objectpos[m.i,4] = c_otfield
         REPLACE hpos WITH hpos + c_adjfld

      CASE objectpos[m.i,4] = c_otlist
         REPLACE vpos WITH vpos + c_vlist
         REPLACE height WITH height - c_listht

      CASE objectpos[m.i,4] = c_ottxtbut
         IF horizbutton(objectpos[m.i, 9])
            m.buttonadjust = c_adjtbtn
         ENDIF

      CASE objectpos[m.i,4] = c_otradbut
         IF m.buttonadjust < c_adjrbtn
            m.buttonadjust = c_adjrbtn
         ENDIF
         REPLACE vpos WITH vpos - c_vradbtn

      CASE objectpos[m.i,4] = c_otchkbox
         REPLACE vpos WITH vpos - c_vchkbox

      CASE objectpos[m.i,4] = c_otpopup
         REPLACE vpos WITH MAX(vpos + m.g_vpopup,0)
         REPLACE hpos WITH MAX(hpos + c_hpopup,0)

      CASE objectpos[m.i,4] = c_otbox
         DO adjbox WITH m.adjust
      ENDCASE
   ENDIF
   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDFOR
GOTO RECORD m.saverec

*
* adjItemsInBoxes - Adjust the location of objects within boxes
*
*!*****************************************************************************
*!
*!      Procedure: ADJITEMSINBOXES
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: ITEMSINBOXES       (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjitemsinboxes
PRIVATE m.subflag, m.emptybox, m.newlastline

DIMENSION boxdimension[4,2]
&& 1 - Topmost
&& 2 - Leftmost
&& 3 - Bottommost
&& 4 - Rightmost

SELECT (m.g_fromobjonlyalias)

SCAN FOR objtype = c_otbox AND HEIGHT != 1 AND WIDTH != 1
   STORE 999 TO boxdimension[1,1], boxdimension[2,1]
   STORE 0 TO boxdimension[3,1], boxdimension[4,1], boxdimension[4,2]
   STORE .F. TO m.subflag, m.emptybox, m.shrinkbox

   DO itemsinboxes WITH vpos, hpos, ;
      vpos + HEIGHT -1, hpos + WIDTH -1, m.emptybox, m.shrinkbox

   IF vpos + HEIGHT - 1 >= g_lastobjectline[1]
      m.newlastline = vpos + HEIGHT -1
      m.flag = .T.
      m.shrinkbox = .F.
   ELSE
      m.flag = .F.
   ENDIF

   boxdimension[1,1] = boxdimension[1,1] - vpos -.5
   boxdimension[2,1] = boxdimension[2,1] - hpos -.5
   boxdimension[3,1] = vpos + HEIGHT - 1 - boxdimension[3,1] - ;
      IIF(m.shrinkbox, .5 + m.g_vpopup, .5)
   boxdimension[4,1] = hpos + WIDTH - boxdimension[4,1] - 1.5

   SELECT (m.g_scrnalias)
   m.thisid = uniqueid
   LOCATE FOR uniqueid = m.thisid AND platform = m.g_toplatform
   IF FOUND() AND NOT m.emptybox
      REPLACE vpos WITH boxdimension[1,2] - boxdimension[1,1]
      REPLACE hpos WITH boxdimension[2,2] - boxdimension[2,1]
      REPLACE height WITH boxdimension[3,2] - vpos + boxdimension[3,1]
      REPLACE WIDTH WITH boxdimension[4,2] - hpos + boxdimension[4,1]
      IF m.flag AND vpos + HEIGHT >= g_lastobjectline[2]
         g_lastobjectline[1] = m.newlastline
         g_lastobjectline[2] = vpos + HEIGHT
      ENDIF
   ENDIF

   SELECT (m.g_fromobjonlyalias)

ENDSCAN
SELECT (m.g_scrnalias)

*
* itemsInBoxes - Adjust objects which are within a box
*
*!*****************************************************************************
*!
*!      Procedure: ITEMSINBOXES
*!
*!      Called by: ADJITEMSINBOXES    (procedure in TRANSPRT.PRG)
*!
*!          Calls: FINDOTHERSONLINE() (function  in TRANSPRT.PRG)
*!               : num2style()        (function  in TRANSPRT.PRG)
*!               : HORIZBUTTON()      (function  in TRANSPRT.PRG)
*!               : GETOBJWIDTH()      (function  in TRANSPRT.PRG)
*!
*!           Uses: M.G_FROMOBJONLYALIA
*!
*!*****************************************************************************
PROCEDURE itemsinboxes
PARAMETER m.top, m.left, m.bottom, m.right, m.emptybox, m.shrinkbox
PRIVATE m.rec, m.wasapopup, m.oldbottom, m.newbottom, m.twidth

m.rec = RECNO()
m.g_boxeditemsalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)

SELECT vpos, hpos, HEIGHT, WIDTH, uniqueid, spacing, objtype, PICTURE, platform ;
   FROM (m.g_fromobjonlyalias) ;
   WHERE (vpos > m.top AND vpos < m.bottom) ;
   AND (hpos > m.left AND hpos < m.right) AND ;
   objtype <> c_otbox AND !(LEN(expr)=3 ;
   AND ASC(SUBSTR(CPTCOND(c_doscp,c_wincp,expr),2,1)) >= 179 ;
    AND ASC(SUBSTR(CPTCOND(c_doscp,c_wincp,expr),2,1)) <= 218);
   INTO CURSOR (m.g_boxeditemsalias)

STORE 0 TO m.oldbottom, m.newbottom
IF _TALLY > 0
   SET RELATION TO uniqueid+m.g_toplatform INTO (m.g_scrnalias) ADDITIVE
   LOCATE FOR .T.
   m.wasapopup = .F.

   DO WHILE NOT EOF()
      IF vpos < boxdimension[1,1] OR (m.wasapopup AND vpos = boxdimension[1,1])
         boxdimension[1,1] = vpos
         boxdimension[1,2] = &g_scrnalias..vpos
         IF objtype = c_otpopup
            m.wasapopup = .T.
         ELSE
            m.wasapopup = .F.
         ENDIF
      ENDIF

      IF hpos < boxdimension[2,1]
         boxdimension[2,1]= hpos
         boxdimension[2,2] = &g_scrnalias..hpos
      ENDIF

      DO CASE
      CASE objtype = c_ottext OR objtype = c_otchkbox ;
            OR (objtype = c_otfield AND height = 1)
         IF vpos > m.oldbottom
            m.shrinkbox = .F.
            IF !findothersonline(vpos, @m.newbottom, @m.oldbottom, objtype)
               m.oldbottom = vpos + HEIGHT
               m.newbottom = &g_scrnalias..vpos + &g_scrnalias..height
            ENDIF
         ENDIF

         * Check TXTWIDTH for text strings
         IF m.g_char2grph AND objtype = c_ottext
            m.twidth = TXTWIDTH(&g_scrnalias..expr,g_dfltfface,g_dfltfsize,num2style(g_boldstylenum))
         ELSE
            m.twidth = &g_scrnalias..width
         ENDIF

         IF &g_scrnalias..hpos + m.twidth > boxdimension[4,2]
            boxdimension[4,1] = hpos + WIDTH - 1
            boxdimension[4,2] = &g_scrnalias..hpos + m.twidth
         ENDIF

      CASE objtype = c_otradbut OR objtype = c_ottxtbut OR objtype = c_otinvbut
         m.numitems = OCCURS(';',PICTURE) + 1

         IF horizbutton(PICTURE)

            IF vpos > m.oldbottom
               m.shrinkbox = .F.
               IF findothersonline(vpos, @m.newbottom, @m.oldbottom, ;
                     objtype)
                  IF objtype = c_ottxtbut
                     REPLACE &g_scrnalias..vpos WITH &g_scrnalias..vpos - 0.312
                  ENDIF
               ENDIF
               m.oldbottom = vpos + HEIGHT - 1
               m.newbottom = &g_scrnalias..vpos + &g_scrnalias..height
            ENDIF

            IF (hpos -1 + (WIDTH +spacing) * m.numitems - spacing) >= ;
                  boxdimension[4,1]
               boxdimension[4,1] = hpos - 1 + ;
                  getobjwidth(objtype, ;
                  PICTURE, ;
                  WIDTH, ;
                  spacing, ;
                  m.g_toplatform)
               boxdimension[4,2] = &g_scrnalias..hpos + ;
                  getobjwidth(&g_scrnalias..objtype, ;
                  &g_scrnalias..picture, ;
                  &g_scrnalias..width, ;
                  &g_scrnalias..spacing, ;
                  m.g_toplatform)
            ENDIF

         ELSE
            m.shrinkbox = .F.
            IF (vpos -1 + m.numitems + (spacing * (m.numitems -1))) >= ;
                  m.oldbottom
               m.oldbottom = vpos -1 + m.numitems + ;
                  (spacing * (m.numitems -1)) - 1
               m.newbottom = &g_scrnalias..vpos  + m.numitems + ;
                  (&g_scrnalias..spacing * (m.numitems -1))
            ENDIF

            IF hpos -1 + WIDTH >= boxdimension[4,1]
               boxdimension[4,1] = hpos -1 + WIDTH
               boxdimension[4,2] = &g_scrnalias..hpos  + ;
                  &g_scrnalias..width
            ENDIF
         ENDIF

      CASE objtype = c_otpopup
         IF vpos + HEIGHT - 2 > m.oldbottom
            IF !findothersonline(vpos + 1, @m.newbottom, @m.oldbottom, objtype)
               m.oldbottom = vpos + HEIGHT - 2
               m.newbottom = &g_scrnalias..vpos + &g_scrnalias..height
            ENDIF
            m.shrinkbox = IIF(m.bottom -1 = vpos + HEIGHT -1, .T., .F.)
         ENDIF

         IF hpos + WIDTH - 1 > boxdimension[4,1]
            boxdimension[4,1] = hpos + WIDTH - 1
            boxdimension[4,2] = &g_scrnalias..hpos + &g_scrnalias..width
         ENDIF

      CASE objtype = c_otfield OR ;
            objtype = c_otlist OR objtype = c_otbox

         IF vpos + HEIGHT - 1 > m.oldbottom
            m.shrinkbox = .F.
            IF !findothersonline(vpos, @m.newbottom, @m.oldbottom, objtype)
               m.oldbottom = vpos + HEIGHT - 1
               m.newbottom = &g_scrnalias..vpos + &g_scrnalias..height
            ENDIF
         ENDIF

         IF hpos + WIDTH - 1 > boxdimension[4,1]
            boxdimension[4,1] = hpos + WIDTH - 1
            boxdimension[4,2] = &g_scrnalias..hpos + &g_scrnalias..width
         ENDIF

      ENDCASE
      SKIP
   ENDDO

   m.emptybox = .F.
   boxdimension[3,1] = m.oldbottom
   boxdimension[3,2] = m.newbottom
ELSE
   m.emptybox = .T.
ENDIF

USE
SELECT (m.g_fromobjonlyalias)
GOTO RECORD m.rec
RETURN

*
* findOthersOnLine - Find any other objects in the box and on the line with a text button
*
*!*****************************************************************************
*!
*!       Function: FINDOTHERSONLINE
*!
*!      Called by: ITEMSINBOXES       (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION findothersonline
PARAMETER m.lineno, m.newbottom, m.oldbottom, m.curtype
PRIVATE m.saverec, m.prevtype, m.flag

m.prevtype = 0
m.flag = .F.
m.saverec = RECNO()
LOCATE FOR (objtype != c_otpopup AND vpos = m.lineno) OR ;
   (m.curtype != c_otpopup AND objtype = c_otpopup AND m.lineno = vpos + 1)

IF !FOUND()
   GOTO RECORD (m.saverec)
   RETURN m.flag
ENDIF

DO WHILE FOUND()
   DO CASE
   CASE objtype = c_ottxtbut
      IF m.curtype != objtype
         m.flag = .T.
         m.oldbottom = vpos + HEIGHT -1
         m.newbottom = &g_scrnalias..vpos + &g_scrnalias..height
         GOTO RECORD (m.saverec)
         RETURN m.flag
      ENDIF

   CASE objtype = c_otpopup
      m.flag = .T.
      m.oldbottom = vpos + HEIGHT - 2
      m.newbottom = &g_scrnalias..vpos + &g_scrnalias..height
      m.prevtype = c_otpopup

   CASE (objtype = c_otfield OR objtype = c_otlist OR objtype = c_otline) AND ;
         (m.prevtype != c_otpopup)
      m.flag = .T.
      m.oldbottom = vpos + HEIGHT - 1
      m.newbottom = &g_scrnalias..vpos + &g_scrnalias..height
      m.prevtype = objtype

   OTHERWISE
      m.flag = .T.
      m.oldbottom = vpos
      m.newbottom = &g_scrnalias..vpos + &g_scrnalias..height

   ENDCASE

   CONTINUE
ENDDO
GOTO RECORD (m.saverec)
RETURN m.flag

*
* StretchLinesToBorders - This procedure makes sure that any lines which stretched to the
*      edge of the from platform window will stretch to the edge of the to platform window.
*
*!*****************************************************************************
*!
*!      Procedure: ADJINVBTNS
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: HORIZBUTTON()      (function  in TRANSPRT.PRG)
*!               : ADJPOSTINV         (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjinvbtns
PRIVATE m.saverec, m.loop, m.horizontal, m.btnid, m.objid, m.flag, m.thermstep, m.leftmost, ;
   m.label, m.btnvpos, m.btnhpos, m.btnwidth, m.btnheight, m.btnspacing, m.btncount, ;
   m.ybtn, m.vbtn, m.xbtn, m.hbtn, m.defwidth, m.defwidthindex, m.defheight, m.defheightindex, ;
   m.topmargin, m.bottommargin, m.leftmargin, m.rightmargin, m.adjustment, m.totadjust, m.newhpos

m.saverec = RECNO()
m.totadjust = 0
m.leftmost = 0

COUNT TO m.thermstep FOR platform = m.g_fromplatform AND objtype = c_otinvbut
m.thermstep = 5/m.thermstep

SCAN FOR platform = m.g_fromplatform AND objtype = c_otinvbut
   m.horizontal = horizbutton(PICTURE)
   m.btnvpos = vpos
   m.btnhpos = hpos
   m.btnheight = HEIGHT
   m.btnwidth = WIDTH
   m.btnspacing = spacing
   m.btncount = OCCURS(";", PICTURE) + 1
   m.btnid = uniqueid

   STORE 0 TO m.defwidth, m.defwidthindex, m.defheight, m.defheightindex

   * This array is used to keep track of the rectangle which bounds the objects which
   * lie on top of each invisible button in the set.
   *
   *   sizes[x,1] = Minimum row on the FROM platform.
   *   sizes[x,2] = Minimum colum on the FROM platform.
   *   sizes[x,3] = Maximum row on the FROM platform.
   *   sizes[x,4] = Maximum colum on the FROM platform.
   *   sizes[x,5] = Minimum row on the TO platform.
   *   sizes[x,6] = Minimum colum on the TO platform.
   *   sizes[x,7] = Maximum row on the TO platform.
   *   sizes[x,8] = Maximum colum on the TO platform.
   *   sizes[x,9] = Comma delimeted list of uniqueid's for objects positioned on
   *               the button face.
   DIMENSION sizes[m.btnCount,9]

   FOR m.loop = 1 TO m.btncount
      m.ybtn = IIF(m.horizontal, m.btnvpos, m.btnvpos + ((m.loop-1) * m.btnheight) + ((m.loop-1) * m.btnspacing))
      m.vbtn = m.ybtn + m.btnheight
      m.xbtn = IIF(m.horizontal, m.btnhpos + ((m.loop-1) * m.btnwidth) + ((m.loop-1) * m.btnspacing), m.btnhpos)
      m.hbtn = m.xbtn + m.btnwidth

      STORE 0 TO sizes[m.loop,3], sizes[m.loop,4], sizes[m.loop,7], sizes[m.loop,8]
      STORE 99999999 TO sizes[m.loop,1], sizes[m.loop,2], sizes[m.loop,5], sizes[m.loop,6]

      sizes[m.loop,9] = ""

      SCAN FOR platform = m.g_fromplatform AND (objtype = c_ottext OR objtype = c_otfield  OR ;
            objtype = c_otbox OR objtype = c_otline) AND ;
            vpos >= m.ybtn AND vpos+HEIGHT <= m.vbtn AND hpos >= m.xbtn AND hpos+WIDTH <= m.hbtn
         m.objid = uniqueid
         sizes[m.loop,1] = MIN(sizes[m.loop,1], vpos)
         sizes[m.loop,2] = MIN(sizes[m.loop,2], hpos)
         sizes[m.loop,3] = MAX(sizes[m.loop,3], vpos+HEIGHT)
         sizes[m.loop,4] = MAX(sizes[m.loop,4], hpos+WIDTH)
         sizes[m.loop,9] = sizes[m.loop,9] + ;
            IIF(LEN(sizes[m.loop,9]) = 0, uniqueid, ","+uniqueid)

         LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
         IF FOUND()
            sizes[m.loop,5] = MIN(sizes[m.loop,5], IIF(objtype = c_otbox OR objtype = c_otline, ;
               vpos-c_adjbox, vpos))
            sizes[m.loop,6] = MIN(sizes[m.loop,6], IIF(objtype = c_otbox OR objtype = c_otline, ;
               hpos-c_adjbox, hpos))
            sizes[m.loop,7] = MAX(sizes[m.loop,7], IIF(objtype = c_otbox OR objtype = c_otline, ;
               vpos+HEIGHT+c_adjbox, vpos+HEIGHT))
            sizes[m.loop,8] = MAX(sizes[m.loop,8], IIF(objtype = c_otbox OR objtype = c_otline, ;
               hpos+WIDTH+c_adjbox, hpos+WIDTH))
         ENDIF

         LOCATE FOR platform = m.g_fromplatform AND uniqueid = m.objid
      ENDSCAN

      * The tallest button region will define where the button set gets
      * placed so we want to remember which region that was.
      IF (sizes[m.loop,7] - sizes[m.loop,5]) > m.defheight
         m.defheight      = sizes[m.loop,7] - sizes[m.loop,5]
         m.defheightindex = m.loop
         m.topmargin      = sizes[m.loop,1] - m.ybtn
         m.bottommargin   = m.vbtn - sizes[m.loop,3]
      ENDIF

      * The widest button region will define where the button set gets
      * placed so we want to remember which region that was.
      IF (sizes[m.loop,8] - sizes[m.loop,6]) > m.defwidth
         m.defwidth      = sizes[m.loop,8] - sizes[m.loop,6]
         m.defwidthindex = m.loop
         m.leftmargin    = sizes[m.loop,2] - m.xbtn
         m.rightmargin   = m.hbtn - sizes[m.loop,4]
      ENDIF
   ENDFOR

   IF m.defheightindex != 0 AND m.defwidthindex != 0
      LOCATE FOR platform = m.g_toplatform AND uniqueid = m.btnid
      IF FOUND()
         IF m.horizontal
            REPLACE vpos WITH sizes[m.defHeightIndex,5] - m.topmargin
         ELSE
            REPLACE hpos WITH sizes[m.defWidthIndex,6] - m.leftmargin
         ENDIF

         REPLACE height WITH (sizes[m.defHeightIndex,7] - sizes[m.defHeightIndex,5]) + m.topmargin + m.bottommargin
         REPLACE WIDTH WITH (sizes[m.defWidthIndex,8] - sizes[m.defWidthIndex,6]) + m.leftmargin + m.rightmargin
      ENDIF

      IF m.horizontal AND WIDTH > m.btnwidth
         m.adjustment = WIDTH - m.btnwidth
         IF spacing > 1
            IF m.adjustment <= spacing-1
               REPLACE spacing WITH spacing - m.adjustment
            ELSE
               m.adjustment = m.adjustment - (spacing-1)
               REPLACE spacing WITH 1
               m.leftmost = MAX(m.leftmost, hpos + (m.btncount*WIDTH) + ((m.btncount-1)*spacing))

               m.totadjust = MAX(m.totadjust, m.btncount * m.adjustment)

               DO adjpostinv WITH vpos, vpos+HEIGHT, ;
                  m.btnhpos + (m.btncount*m.btnwidth) + ((m.btncount-1)*m.btnspacing), ;
                  m.btncount * m.adjustment

               FOR m.loop = 2 TO m.btncount
                  DO WHILE LEN(sizes[m.loop,9]) > 0
                     IF AT(",", sizes[m.loop,9]) != 0
                        m.label = LEFT(sizes[m.loop,9], AT(",", sizes[m.loop,9])-1)
                        sizes[m.loop,9] = SUBSTR(sizes[m.loop,9], AT(",", sizes[m.loop,9])+1)
                     ELSE
                        m.label = sizes[m.loop,9]
                        sizes[m.loop,9] = ""
                     ENDIF

                     LOCATE FOR platform = m.g_fromplatform AND uniqueid = m.label
                     IF FOUND()
                        m.newhpos = hpos + (m.adjustment * (m.loop-1))
                        LOCATE FOR platform = m.g_toplatform AND uniqueid = m.label
                        IF FOUND()
                           REPLACE hpos WITH IIF(objtype = c_otbox OR objtype = c_otline, ;
                              m.newhpos+c_adjbox, m.newhpos)
                        ENDIF
                     ENDIF
                  ENDDO
               ENDFOR
            ENDIF
         ENDIF
      ENDIF
   ENDIF

   LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
   IF FOUND()
      IF m.totadjust > 0
         REPLACE WIDTH WITH WIDTH + m.totadjust
      ENDIF

      IF WIDTH < m.leftmost
         REPLACE WIDTH WITH m.leftmost + 1
      ENDIF
   ENDIF


   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury

   LOCATE FOR platform = m.g_fromplatform AND uniqueid = m.btnid
ENDSCAN

IF m.saverec <= RECCOUNT()
   GOTO RECORD (m.saverec)
ELSE
   LOCATE FOR .F.
ENDIF

*
* adjPostInv - This procedure moves objects which lie to the right of a set of horizontal
*      invisible buttons so that they won't overlap.
*
*!*****************************************************************************
*!
*!      Procedure: ADJPOSTINV
*!
*!      Called by: ADJINVBTNS         (procedure in TRANSPRT.PRG)
*!
*!          Calls: FINDALIGNEND()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjpostinv
PARAMETER m.ystart, m.yend, m.xstart, m.adjustment
PRIVATE m.saverec, m.saveid

m.saverec = RECNO()

m.ystart = findalignend(m.ystart, m.xstart, -1)
m.yend = findalignend(m.yend, m.xstart, 1)

SCAN FOR platform = m.g_fromplatform AND hpos >= m.xstart AND vpos >= m.ystart AND vpos <= m.yend AND ;
      (objtype = c_ottext   OR objtype = c_otline   OR objtype = c_otbox   OR objtype = c_list OR ;
      objtype = c_otradbut OR objtype = c_otchkbox OR objtype = c_otfield OR objtype = c_popup OR ;
      objtype = c_otinvbut)
   m.saveid = uniqueid
   LOCATE FOR platform = m.g_toplatform AND uniqueid = m.saveid
   IF FOUND()
      REPLACE hpos WITH hpos + m.adjustment
   ENDIF

   LOCATE FOR platform = m.g_fromplatform AND uniqueid = m.saveid
ENDSCAN

IF m.saverec <= RECCOUNT()
   GOTO RECORD m.saverec
ELSE
   LOCATE FOR .F.
ENDIF

*
* FindAlignEnd - Given a position to start with and a direction, this routine looks for the
*      last line where right aligned objects extend to from the starting position.
*
*!*****************************************************************************
*!
*!       Function: FINDALIGNEND
*!
*!      Called by: ADJPOSTINV         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION findalignend
PARAMETER m.ystart, m.xstart, m.increment
PRIVATE m.saverec, m.ytemp, m.xtemp, m.result

m.result = m.ystart

SCAN FOR platform = m.g_fromplatform AND hpos >= m.xstart AND vpos = m.ystart
   m.saverec = RECNO()

   m.ytemp = vpos + m.increment
   m.xtemp = hpos
   LOCATE FOR platform = m.g_fromplatform AND vpos = m.ytemp AND hpos = m.xtemp AND ;
      (objtype = c_ottext   OR objtype = c_otline   OR objtype = c_otbox   OR objtype = c_list OR ;
      objtype = c_otradbut OR objtype = c_otchkbox OR objtype = c_otfield OR objtype = c_popup OR ;
      objtype = c_otinvbut)
   DO WHILE FOUND()
      m.result = IIF(m.increment < 0, MIN(m.result, m.ytemp), MAX(m.result, m.ytemp))
      m.ytemp = m.ytemp + m.increment
      LOCATE FOR platform = m.g_fromplatform AND vpos = m.ytemp AND hpos = m.xtemp AND ;
         (objtype = c_ottext   OR objtype = c_otline   OR objtype = c_otbox   OR objtype = c_list OR ;
         objtype = c_otradbut OR objtype = c_otchkbox OR objtype = c_otfield OR objtype = c_popup OR ;
         objtype = c_otinvbut)
   ENDDO
   GOTO RECORD m.saverec
ENDSCAN

RETURN m.result

*
* StretchLinesToBorders - This procedure makes sure that any lines which stretched to the
*      edge of the from platform window will stretch to the edge of the to platform window.
*
*!*****************************************************************************
*!
*!      Procedure: STRETCHLINESTOBORDERS
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE stretchlinestoborders
PRIVATE m.saverec, m.objid, m.objrec, m.objwidth, m.fromheight, m.fromwidth

IF m.g_filetype = c_report OR m.g_filetype = c_label
   RETURN
ENDIF

m.saverec = RECNO()

LOCATE FOR platform = m.g_fromplatform AND objtype = c_otheader
IF FOUND()
   IF BORDER = 0 OR STYLE = 0
      m.fromheight = HEIGHT
      m.fromwidth = WIDTH
   ELSE
      m.fromheight = HEIGHT - 2
      m.fromwidth = WIDTH - 2
   ENDIF

   SCAN FOR platform = m.g_fromplatform AND objtype = c_otbox AND ;
         ((WIDTH = 1 AND vpos+HEIGHT = m.fromheight) OR (HEIGHT = 1 AND hpos+WIDTH = m.fromwidth))

      m.objrec = RECNO()
      m.objid = uniqueid
      m.objwidth = WIDTH
      LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
      IF FOUND()
         m.toheight = HEIGHT
         m.towidth = WIDTH

         LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
         IF FOUND()
            IF m.objwidth = 1
               REPLACE height WITH m.toheight-vpos
            ELSE
               REPLACE WIDTH WITH m.towidth-hpos
            ENDIF
         ENDIF
      ENDIF

      GOTO RECORD m.objrec
   ENDSCAN
ENDIF

IF m.saverec > RECCOUNT()
   LOCATE FOR .F.
ELSE
   GOTO RECORD m.saverec
ENDIF
RETURN

*
* JoinLines -This procedure examines each line to see where it meets other lines in the
*      from platform and constructs an array of these positons.  This array can then
*      be used to make the lines/boxes meet in the from platform.
*
*!*****************************************************************************
*!
*!      Procedure: JOINLINES
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: UPDTHERM           (procedure in TRANSPRT.PRG)
*!               : JOINHORIZONTAL     (procedure in TRANSPRT.PRG)
*!               : JOINVERTICAL       (procedure in TRANSPRT.PRG)
*!               : MEETBOXCHAR        (procedure in TRANSPRT.PRG)
*!               : ZAPBOXCHAR         (procedure in TRANSPRT.PRG)
*!               : REJOINBOXES        (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE joinlines
PRIVATE m.saverec, m.joincount, m.linerec, m.lineid, m.i, m.thermstep, ;
   m.objvpos, m.objhpos, m.objright, m.objbottom, m.objid, m.objrec, m.objcode, ;
   m.fromvpos, m.fromhpos, m.fromheight, m.fromwidth, m.fromend, m.fromcode, ;
   m.tovpos, m.tohpos, m.toheight, m.towidth, ;
   m.joinvpos, m.joinhpos, m.vlevel, m.hlevel

DIMENSION joins[1,5]
&& Joins[X,2] - toVpos
&& Joins[X,3] - toHpos
&& Joins[X,4] - Vpos match level
&& Joins[X,5] - Hpos match level
m.joincount = 0
m.saverec = RECNO()

COUNT TO m.thermstep FOR platform = m.g_fromplatform AND objtype = c_otbox AND (WIDTH=1 OR HEIGHT=1)
IF m.thermstep <> 0
   m.thermstep = 10 / m.thermstep
ELSE
   m.g_mercury = MIN(m.g_mercury + 10, 95)
   DO updtherm WITH m.g_mercury
ENDIF

SCAN FOR platform = m.g_fromplatform AND objtype = c_otbox AND (WIDTH=1 OR HEIGHT=1)
   m.fromvpos = vpos
   m.fromhpos = hpos
   m.fromheight = HEIGHT
   m.fromwidth = WIDTH
   m.fromcode = objcode
   m.lineid = uniqueid
   m.linerec = RECNO()

   LOCATE FOR platform = m.g_toplatform AND uniqueid = m.lineid
   IF FOUND()
      m.tovpos = vpos
      m.tohpos = hpos
      m.toheight = HEIGHT
      m.towidth = WIDTH

      SCAN FOR platform = m.g_fromplatform AND objtype = c_otbox AND uniqueid <> m.lineid
         IF m.fromheight = 1 AND HEIGHT <> 1 AND (m.fromvpos >= vpos AND m.fromvpos <= vpos+HEIGHT-1)
            m.fromend = m.fromhpos + m.fromwidth - 1

            ** Horizontal line which starts on a vertical line/box side.
            IF m.fromhpos = hpos OR m.fromhpos = hpos+WIDTH-1
               DO joinhorizontal WITH m.fromvpos, m.fromhpos, m.fromhpos, m.tovpos, m.toheight, m.fromcode
            ENDIF

            ** Horizontal line which ends on a vertical line/box side.
            IF m.fromend = hpos OR m.fromend = hpos+WIDTH-1
               DO joinhorizontal WITH m.fromvpos, m.fromend, m.fromend, m.tovpos, m.toheight, m.fromcode
            ENDIF

            ** Horizontal line which starts one to the right of a vertical line/box side
            IF m.fromhpos-1 = hpos OR m.fromhpos = hpos+WIDTH
               DO joinhorizontal WITH m.fromvpos, m.fromhpos-1, m.fromhpos, m.tovpos, m.toheight, m.fromcode
            ENDIF

            ** Horizontal line which ends one left of a vertical line/box side
            IF m.fromend+1 = hpos OR  m.fromend = hpos+WIDTH-2
               DO joinhorizontal WITH m.fromvpos, m.fromend+1, m.fromend, m.tovpos, m.toheight, m.fromcode
            ENDIF
         ENDIF

         IF m.fromwidth = 1 AND WIDTH <> 1 AND (m.fromhpos >= hpos AND m.fromhpos <= hpos+WIDTH-1)
            m.fromend = m.fromvpos + m.fromheight - 1

            ** Vertical line which starts on a horizontical line/box side.
            IF m.fromvpos = vpos OR m.fromvpos = vpos+HEIGHT-1
               DO joinvertical WITH m.fromvpos, m.fromvpos, m.fromhpos, m.tohpos, m.fromcode
            ENDIF

            ** Vertical line which ends on a horizontical line/box side.
            IF m.fromend = vpos OR m.fromend = vpos+HEIGHT-1
               DO joinvertical WITH m.fromend, m.fromend, m.fromhpos, m.tohpos, m.fromcode
            ENDIF

            ** Vertical line which starts one below a horizontal line/box side
            IF m.fromvpos-1 = vpos OR m.fromvpos = vpos+HEIGHT
               DO joinvertical WITH m.fromvpos-1, m.fromvpos, m.fromhpos, m.tohpos, m.fromcode
            ENDIF

            ** Vertical line which ends one above a horizontal line/box side
            IF m.fromend+1 = vpos OR m.fromend = vpos+HEIGHT-2
               DO joinvertical WITH m.fromend+1, m.fromend, m.fromhpos, m.tohpos, m.fromcode
            ENDIF
         ENDIF
      ENDSCAN
   ENDIF

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury

   GOTO RECORD m.linerec
ENDSCAN

DO meetboxchar
DO zapboxchar

m.thermstep = 10/m.joincount
FOR m.i = 1 TO m.joincount
   DO rejoinboxes WITH VAL(LEFT(joins[m.i, 1], 3)), VAL(RIGHT(joins[m.i, 1], 3)), joins[m.i, 2], joins[m.i, 3]

   m.g_mercury = MIN(m.g_mercury + m.thermstep, 95)
   DO updtherm WITH m.g_mercury
ENDFOR

IF m.saverec > RECCOUNT()
   LOCATE FOR .F.
ELSE
   GOTO RECORD m.saverec
ENDIF
RETURN

*
* joinHorizontal - This procedure adds a join for a horizontal line which has been determined to
*               intersect something vertical.
*
*!*****************************************************************************
*!
*!      Procedure: JOINHORIZONTAL
*!
*!      Called by: JOINLINES          (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLINEWIDTH()     (function  in TRANSPRT.PRG)
*!               : ADDJOIN            (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE joinhorizontal
PARAMETER m.fromvpos, m.oldhpos1, m.oldhpos2, m.tovpos, m.tothickness, m.fromcode
PRIVATE m.objvpos, m.objhpos, m.objright, m.objbottom, m.objcode, m.objid, m.objrec

m.objvpos = vpos
m.objhpos = hpos
m.objright = hpos + WIDTH - 1
m.objbottom = vpos + HEIGHT - 1
m.objcode = objcode
m.objid = uniqueid
m.objrec = RECNO()

LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
IF FOUND()
   DO CASE
   CASE m.fromvpos = m.objvpos OR m.fromvpos = m.objbottom
      IF objtype = c_otline
         m.joinvpos = m.tovpos - c_adjbox + (m.tothickness/2)
         STORE 2 TO m.vlevel, m.hlevel
      ELSE
         IF m.fromvpos = m.objvpos
            m.joinvpos = vpos - c_adjbox + (getlinewidth(m.objcode, .T.)/2)
         ELSE
            m.joinvpos = vpos+HEIGHT - c_adjbox - (getlinewidth(m.objcode, .T.)/2)
         ENDIF
         STORE 4 TO m.vlevel, m.hlevel
      ENDIF

   OTHERWISE
      m.joinvpos = m.tovpos - c_adjbox + (getlinewidth(m.fromcode, .T.)/2)
      m.vlevel = 0
      m.hlevel = IIF(objtype = c_otline, 1, 3)
   ENDCASE

   IF m.oldhpos1 = m.objhpos OR objtype = c_otline
      m.joinhpos = hpos - c_adjbox + (getlinewidth(m.objcode, .F.)/2)
   ELSE
      m.joinhpos = hpos+WIDTH - c_adjbox - (getlinewidth(m.objcode, .F.)/2)
   ENDIF

   DO addjoin WITH m.fromvpos, m.oldhpos1, m.joinvpos, m.joinhpos, m.vlevel, m.hlevel
   IF m.oldhpos1 <> m.oldhpos2
      DO addjoin WITH m.fromvpos, m.oldhpos2, m.joinvpos, m.joinhpos, m.vlevel, m.hlevel
   ENDIF
ENDIF

GOTO RECORD m.objrec
RETURN

*
* joinVertical - This procedure adds a join for a vertical line which has been determined to
*               intersect something horizontal.
*
*!*****************************************************************************
*!
*!      Procedure: JOINVERTICAL
*!
*!      Called by: JOINLINES          (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLINEWIDTH()     (function  in TRANSPRT.PRG)
*!               : ADDJOIN            (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE joinvertical
PARAMETER m.oldvpos1, m.oldvpos2, m.fromhpos, m.tohpos, m.fromcode
PRIVATE m.objvpos, m.objhpos, m.objright, m.objbottom, m.objcode, m.objid, m.objrec

m.objvpos = vpos
m.objhpos = hpos
m.objright = hpos + WIDTH - 1
m.objbottom = vpos + HEIGHT - 1
m.objcode = objcode
m.objid = uniqueid
m.objrec = RECNO()

LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
IF FOUND()
   DO CASE
   CASE m.fromhpos = m.objhpos OR m.fromhpos = m.objright
      IF objtype = c_otline
         m.joinhpos = IIF(m.fromhpos = m.objhpos, hpos, hpos+WIDTH-1)
         STORE 2 TO m.vlevel, m.hlevel
      ELSE
         IF m.fromhpos = m.objhpos
            m.joinhpos = hpos - c_adjbox + (getlinewidth(m.objcode, .F.)/2)
         ELSE
            m.joinhpos = hpos+WIDTH - c_adjbox - (getlinewidth(m.objcode, .F.)/2)
         ENDIF
         STORE 4 TO m.vlevel, m.hlevel
      ENDIF

   OTHERWISE
      m.joinhpos = m.tohpos - c_adjbox + (getlinewidth(m.fromcode, .F.)/2)
      m.vlevel = IIF(objtype = c_otline, 1, 3)
      m.hlevel = 0
   ENDCASE

   IF m.oldvpos1 = m.objvpos OR objtype = c_otline
      m.joinvpos = vpos - c_adjbox + (getlinewidth(m.objcode, .T.)/2)
   ELSE
      m.joinvpos = vpos+HEIGHT - c_adjbox - (getlinewidth(m.objcode, .T.)/2)
   ENDIF

   DO addjoin WITH m.oldvpos1, m.fromhpos, m.joinvpos, m.joinhpos, m.vlevel, m.hlevel
   IF m.oldvpos1 <> m.oldvpos2
      DO addjoin WITH m.oldvpos2, m.fromhpos, m.joinvpos, m.joinhpos, m.vlevel, m.hlevel
   ENDIF
ENDIF
GOTO RECORD m.objrec

*
* MeetBoxChar - This procedure looks at suspected box join characters and adds a join position for each
*            line which ends one short of it.
*
*!*****************************************************************************
*!
*!      Procedure: MEETBOXCHAR
*!
*!      Called by: JOINLINES          (procedure in TRANSPRT.PRG)
*!
*!          Calls: ADDJOIN            (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE meetboxchar
PRIVATE m.saverec, m.fromvpos, m.fromhpos, m.tovpos, m.tohpos, m.joinrec, m.joinid
m.saverec = RECNO()

SCAN FOR platform = m.g_fromplatform AND objtype = c_ottext AND LEN(expr)=3 AND ;
      ASC(SUBSTR(CPTCOND(c_doscp,c_wincp,expr),2,1)) >= 179 ;
      AND ASC(SUBSTR(CPTCOND(c_doscp,c_wincp,expr),2,1)) <= 218
   m.fromvpos = vpos
   m.fromhpos = hpos
   m.joinid = uniqueid
   m.joinrec = RECNO()

   LOCATE FOR platform = m.g_toplatform AND uniqueid = m.joinid
   IF FOUND()
      m.tovpos = vpos
      m.tohpos = hpos

      SCAN FOR platform = m.g_fromplatform AND objtype = c_otbox AND (WIDTH = 1 OR height = 1)
         IF WIDTH = 1 AND hpos = m.fromhpos
            DO CASE
            CASE vpos = m.fromvpos + 1
               DO addjoin WITH vpos, hpos, m.tovpos, m.tohpos, 2, 2

            CASE vpos+HEIGHT = m.fromvpos
               DO addjoin WITH vpos+HEIGHT-1, hpos, m.tovpos, m.tohpos, 2, 2
            ENDCASE
         ENDIF

         IF height = 1 AND vpos = m.fromvpos
            DO CASE
            CASE hpos = m.fromhpos + 1
               DO addjoin WITH vpos, hpos, m.tovpos, m.tohpos, 2, 2

            CASE hpos+WIDTH = m.fromhpos
               DO addjoin WITH vpos, hpos+WIDTH-1, m.tovpos, m.tohpos, 2, 2
            ENDCASE
         ENDIF
      ENDSCAN
   ENDIF

   GOTO RECORD m.joinrec
ENDSCAN

IF m.saverec > RECCOUNT()
   LOCATE FOR .F.
ELSE
   GOTO RECORD m.saverec
ENDIF
RETURN

*
* zapBoxChar - This procedure looks for any text record which is probably a box join
*            character and replaces it with a transparent space.
*
*!*****************************************************************************
*!
*!      Procedure: ZAPBOXCHAR
*!
*!      Called by: JOINLINES          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE zapboxchar
PRIVATE m.recno, m.fromvpos, m.fromhpos
m.recno = RECNO()

* See if we can find any single text box/line joining characters in a group.
SCAN FOR platform = m.g_toplatform AND objtype = c_ottext ;
      AND boxjoin(objtype,recno(),platform)
   REPLACE expr WITH '" "'
   REPLACE mode WITH 1
ENDSCAN

IF m.recno > RECCOUNT()
   GOTO RECCOUNT()
   SKIP
ELSE
   GOTO RECORD m.recno
ENDIF
RETURN

*
* AddJoin - This routine adds the position for a join character, or modifies a previous join
*      at the same from position if it has a lower priority.
*
*!*****************************************************************************
*!
*!      Procedure: ADDJOIN
*!
*!      Called by: JOINHORIZONTAL     (procedure in TRANSPRT.PRG)
*!               : JOINVERTICAL       (procedure in TRANSPRT.PRG)
*!               : MEETBOXCHAR        (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE addjoin
PARAMETER m.fromvpos, m.fromhpos, m.tovpos, m.tohpos, m.vmatch, m.hmatch
PRIVATE m.row, m.key
m.key = STR(m.fromvpos, 3)+STR(m.fromhpos, 3)
m.row = ASCAN(joins, m.key)
IF m.row = 0
   m.joincount = m.joincount + 1
   DIMENSION joins[m.joinCount, 5]
   joins[m.joinCount, 1] = m.key
   joins[m.joinCount, 2] = m.tovpos
   joins[m.JoinCount, 3] = m.tohpos
   joins[m.JoinCount, 4] = m.vmatch
   joins[m.JoinCount, 5] = m.hmatch
ELSE
   m.row = ASUBSCRIPT(joins, m.row, 1)

   IF m.vmatch > joins[m.row, 4]
      joins[m.row, 2] = m.tovpos
      joins[m.row, 4] = m.vmatch
   ENDIF

   IF m.hmatch > joins[m.JoinCount, 5]
      joins[m.row, 3] = m.tohpos
      joins[m.row, 5] = m.hmatch
   ENDIF
ENDIF

RETURN

*
* RejoinBoxes - This routine stretches lines so that they meet the join characters
*      they did in the from platform.
*
*!*****************************************************************************
*!
*!      Procedure: REJOINBOXES
*!
*!      Called by: JOINLINES          (procedure in TRANSPRT.PRG)
*!
*!          Calls: JOINLINEWIDTH()    (function  in TRANSPRT.PRG)
*!               : GETLINEWIDTH()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE rejoinboxes
PARAMETER m.fromvpos, m.fromhpos, m.tovpos, m.tohpos
PRIVATE m.objectcode, m.objend, m.saverecno, m.objid, m.joinwidth, m.objrec

m.saverecno = RECNO()

SCAN FOR platform = m.g_fromplatform AND objtype = c_otbox
   IF WIDTH = 1 OR height = 1
      m.objid = uniqueid
      m.objectcode = objcode
      m.objrec = RECNO()

      DO CASE
         ** A Vertical line which starts at a join character
      CASE m.fromvpos = vpos AND m.fromhpos = hpos AND WIDTH = 1
         LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
         IF FOUND()
            m.objend = vpos + HEIGHT
            m.joinwidth = joinlinewidth(m.fromvpos, m.fromhpos, .T., m.objid)
            REPLACE vpos WITH m.tovpos + c_adjbox - (m.joinwidth/2)
            REPLACE height WITH m.objend - vpos
            REPLACE hpos WITH m.tohpos + c_adjbox - (getlinewidth(m.objectcode, .F.)/2)
         ENDIF

         ** A Horizontal line which starts at a join character
      CASE m.fromvpos = vpos AND m.fromhpos = hpos AND height = 1
         LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
         IF FOUND()
            m.objend = hpos + WIDTH
            m.joinwidth = joinlinewidth(m.fromvpos, m.fromhpos, .F., m.objid)
            REPLACE hpos WITH m.tohpos + c_adjbox - (m.joinwidth/2)
            REPLACE WIDTH WITH m.objend - hpos
            REPLACE vpos WITH m.tovpos + c_adjbox - (getlinewidth(m.objectcode, .T.)/2)
         ENDIF

         ** A Vertical line which ends at a join character
      CASE m.fromvpos = (vpos+HEIGHT-1) AND m.fromhpos = hpos AND WIDTH = 1
         LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
         IF FOUND()
            m.joinwidth = joinlinewidth(m.fromvpos, m.fromhpos, .T., m.objid)
            REPLACE height WITH (m.tovpos + c_adjbox + (m.joinwidth/2)) - vpos
            REPLACE hpos WITH m.tohpos + c_adjbox - (getlinewidth(m.objectcode, .F.)/2)
         ENDIF

         ** A Horizontal line which ends at a join character
      CASE m.fromhpos = (hpos+WIDTH-1) AND m.fromvpos = vpos AND height = 1
         LOCATE FOR platform = m.g_toplatform AND uniqueid = m.objid
         IF FOUND()
            m.joinwidth = joinlinewidth(m.fromvpos, m.fromhpos, .F., m.objid)
            REPLACE WIDTH WITH (m.tohpos + c_adjbox + (m.joinwidth/2)) - hpos
            REPLACE vpos WITH m.tovpos + c_adjbox - (getlinewidth(m.objectcode, .T.)/2)
         ENDIF
      ENDCASE

      GOTO RECORD m.objrec
   ENDIF
ENDSCAN

IF m.saverecno > RECCOUNT()
   LOCATE FOR .F.
ELSE
   GOTO RECORD m.saverecno
ENDIF

RETURN

*
* JoinLineWidth - Looks for the thickest line or box which goes through a given point and
*      Returns either its horizontal or vertical Width.
*
*!*****************************************************************************
*!
*!       Function: JOINLINEWIDTH
*!
*!      Called by: REJOINBOXES        (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLINEWIDTH()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION joinlinewidth
PARAMETERS m.joinvpos, m.joinhpos, m.horizontal, m.skipid
PRIVATE m.i, m.saverecno, m.thickness
m.saverecno = RECNO()
m.thickness = 0

SCAN FOR platform = m.g_fromplatform AND objtype = c_otbox AND uniqueid <> m.skipid
   DO CASE
   CASE m.horizontal AND WIDTH <> 1 AND ;
         (ABS(m.joinvpos - vpos) <= 1 OR ABS(m.joinvpos - (vpos+HEIGHT-1)) <= 1) AND ;
         (m.joinhpos >= hpos AND m.joinhpos <= (hpos+WIDTH-1))
      m.thickness = MAX(getlinewidth(objcode, .T.), m.thickness)

   CASE !m.horizontal AND HEIGHT <> 1 AND ;
         (ABS(m.joinhpos - hpos) <= 1 OR ABS(m.joinhpos - (hpos+WIDTH-1)) <= 1) AND ;
         (m.joinvpos >= vpos AND m.joinvpos <= (vpos+WIDTH-1))
      m.thickness = MAX(getlinewidth(objcode, .F.), m.thickness)
   ENDCASE
ENDSCAN

IF m.thickness = 0
   SCAN FOR platform = m.g_fromplatform AND objtype = c_otbox AND uniqueid <> m.skipid
      IF (HEIGHT = 1 OR WIDTH = 1) AND ;
            (ABS(m.joinvpos - vpos) <= 1 OR ABS(m.joinvpos - (vpos+HEIGHT-1)) <= 1) AND ;
            (ABS(m.joinhpos - hpos) <= 1 OR ABS(m.joinhpos - (hpos+WIDTH-1)) <= 1)
         m.thickness = MAX(getlinewidth(objcode, m.horizontal), m.thickness)
      ENDIF
   ENDSCAN
ENDIF

GOTO RECORD m.saverecno
RETURN m.thickness

*
* getLastObjectLine - Determine if this object is the lowest object.
*
*!*****************************************************************************
*!
*!       Function: GETLASTOBJECTLINE
*!
*!      Called by: REPOOBJECTS        (procedure in TRANSPRT.PRG)
*!
*!          Calls: HORIZBUTTON()      (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getlastobjectline
PARAMETER m.currentlastline, m.newposition
PRIVATE m.numitems, m.max

DO CASE
CASE objtype = c_ottext OR objtype = c_otchkbox
   IF vpos > m.currentlastline
      g_lastobjectline[2] = m.newposition + HEIGHT
      RETURN vpos + HEIGHT
   ELSE
      RETURN m.currentlastline
   ENDIF

CASE objtype = c_otradbut OR objtype = c_ottxtbut OR objtype = c_otinvbut
   IF horizbutton(PICTURE)
      IF vpos + HEIGHT >= m.currentlastline
         g_lastobjectline[2] = m.newposition + HEIGHT
         RETURN vpos
      ELSE
         RETURN m.currentlastline
      ENDIF
   ELSE
      m.numitems = OCCURS(';',PICTURE)
      m.max = vpos + m.numitems + (m.numitems * spacing)
      IF m.max >= m.currentlastline AND (objtype = c_ottxtbut OR objtype = c_otinvbut) OR ;
            m.max > m.currentlastline AND objtype = c_otradbut
         g_lastobjectline[2] = m.newposition + (HEIGHT * (m.numitems + 1)) + ;
            (spacing * m.numitems)
         RETURN m.max + 1
      ELSE
         RETURN m.currentlastline
      ENDIF
   ENDIF

CASE objtype = c_otpopup
   IF vpos + 2 > m.currentlastline
      g_lastobjectline[2] = m.newposition + 2
      RETURN vpos +1
   ELSE
      RETURN m.currentlastline
   ENDIF

CASE objtype = c_otfield
   IF vpos + HEIGHT -1 > m.currentlastline
      g_lastobjectline[2] = m.newposition + HEIGHT
      RETURN vpos + HEIGHT -1
   ELSE
      RETURN m.currentlastline
   ENDIF

CASE objtype = c_otlist OR ;
      objtype = c_otbox OR objtype = c_otline
   IF vpos + HEIGHT - 1 > m.currentlastline
      g_lastobjectline[2] = m.newposition + HEIGHT
      RETURN vpos + HEIGHT - 1
   ELSE
      RETURN m.currentlastline
   ENDIF

OTHERWISE
   RETURN m.currentlastline

ENDCASE

*
* adjobjcode - Adjust object code field for Objtype = 1.
*
*!*****************************************************************************
*!
*!      Procedure: ADJOBJCODE
*!
*!      Called by: ALLENVIRONS        (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjobjcode
* Stuff the right version code into the object code field for the header record
DO CASE
CASE objtype = c_otheader OR (m.g_filetype=c_label AND objtype = c_ot20label)
   REPLACE objcode WITH IIF(m.g_filetype=c_screen,c_25scx,c_25frx)
CASE objtype = c_otgroup
   REPLACE objcode WITH 0
ENDCASE

*!*****************************************************************************
*!
*!      Procedure: GETWINDFONT
*!
*!      Called by: NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!
*!          Calls: num2style()        (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE getwindfont
* Get the default font for this window, if one has been defined
IF m.g_char2grph
   * Get font information from header
   GOTO TOP
   LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
   IF FOUND() AND !EMPTY(fontface)
      m.g_dfltfface  = fontface
      m.g_dfltfsize  = fontsize
      m.g_dfltfstyle = num2style(fontstyle)
   ENDIF
ENDIF
RETURN

*
* adjHeightAndWidth - Adjust the Height and width of objects.
*
*!*****************************************************************************
*!
*!      Procedure: ADJHEIGHTANDWIDTH
*!
*!      Called by: NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!
*!          Calls: num2style()        (function  in TRANSPRT.PRG)
*!               : DOSSIZE()          (function  in TRANSPRT.PRG)
*!               : COLUMNAR()         (function  in TRANSPRT.PRG)
*!               : ADJTEXT            (procedure in TRANSPRT.PRG)
*!               : ADJBITMAPCTRL      (procedure in TRANSPRT.PRG)
*!               : MAXBTNWIDTH()      (function  in TRANSPRT.PRG)
*!               : ADJBOX             (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjheightandwidth
PRIVATE m.txtwidthratio, m.boldtxtratio, m.chkboxwidth, m.saverec, ;
   m.oldwidth, m.newheight, m.newwidth, ;
   m.wndface, m.wndsize, m.wndstyle, m.alignment
* Only Screen objects come through this routine.

DO CASE
CASE m.g_char2grph
   m.saverec = RECNO()
   * Get font information from header
   LOCATE FOR platform = m.g_toplatform AND objtype = c_otheader
   IF FOUND()
      m.wndface  = fontface
      m.wndsize  = fontsize
      m.wndstyle = fontstyle
   ELSE
      m.wndface  = m.g_dfltfface
      m.wndsize  = m.g_dfltfsize
      m.wndstyle = m.g_dfltfstyle
   ENDIF
   GOTO m.saverec

   * This is the ratio of character size for the window font to that for the current object font
   m.txtwidthratio = FONTMETRIC(6, m.wndface, m.wndsize, num2style(m.wndstyle)) / ;
      FONTMETRIC(6,fontface,fontsize,num2style(fontstyle))
   m.boldtxtratio = FONTMETRIC(6, m.wndface, m.wndsize, num2style(m.wndstyle)) / ;
      FONTMETRIC(6,m.g_dfltfface,m.g_dfltfsize,num2style(m.g_boldstylenum))
   m.chkboxwidth = c_chkpixel / FONTMETRIC(6,m.g_dfltfface,m.g_dfltfsize,num2style(m.g_boldstylenum))
   m.chkboxwidth = m.chkboxwidth + (m.chkboxwidth / 2)
CASE m.g_grph2char
   m.saverec = RECNO()
   LOCATE FOR platform = m.g_fromplatform AND objtype = c_otheader
   IF FOUND()
      m.wndface = fontface
      m.wndsize = fontsize
      m.wndstyle = fontstyle
   ELSE
      m.wndface  = m.g_ctrlfface    && MS Sans Serif for Windows
      m.wndsize  = m.g_ctrlfsize
      m.wndstyle = m.g_ctrlfstyle
   ENDIF
   GOTO m.saverec
ENDCASE

DO CASE
CASE objtype = c_ottext
   DO CASE
   CASE m.g_char2grph
      m.oldwidth = WIDTH
      REPLACE WIDTH WITH TXTWIDTH(SUBSTR(expr, 2,LEN(expr)-2), fontface, ;
         fontsize, num2style(fontstyle)) && * m.txtwidthratio
   CASE m.g_grph2char
      m.oldwidth = ROUND(dossize(WIDTH, fontsize, m.wndsize), 0)
      m.newheight = 1
      m.newwidth = LEN(expr)-2

      m.alignment = columnar(vpos, hpos, WIDTH, objtype)
      DO CASE
      CASE m.alignment = 2
         REPLACE hpos WITH hpos + WIDTH - m.newwidth

      CASE m.alignment = 0
         REPLACE vpos WITH vpos + ((HEIGHT - m.newheight) / 2)
         REPLACE hpos WITH hpos + ((WIDTH - m.newwidth) / 2)
      ENDCASE

      REPLACE height WITH MAX(m.newheight,1)
      REPLACE WIDTH WITH MAX(m.newwidth,1)

      DO adjtext WITH m.oldwidth
   ENDCASE

CASE objtype = c_otchkbox
   DO CASE
   CASE m.g_char2grph
      m.oldwidth = WIDTH
      REPLACE WIDTH WITH (TXTWIDTH(SUBSTR(PICTURE, 6,LEN(PICTURE)-6) + SPACE(1), fontface, ;
         fontsize, num2style(fontstyle)) * m.boldtxtratio) + m.chkboxwidth
      REPLACE height WITH c_chkhght
   CASE m.g_grph2char
      DO adjbitmapctrl

      REPLACE height WITH 1
      REPLACE WIDTH WITH maxbtnwidth(PICTURE, "", "", "")+4
   ENDCASE

CASE objtype = c_otradbut
   DO CASE
   CASE m.g_char2grph
      m.oldwidth = WIDTH
      DO adjbitmapctrl
      REPLACE height WITH c_radhght
   CASE m.g_grph2char
      REPLACE height WITH 1
      REPLACE spacing WITH ROUND(dossize(spacing, fontsize, m.wndsize), 0)
      REPLACE WIDTH WITH MAX(maxbtnwidth(PICTURE, "", "", "")+4, dossize(WIDTH, fontsize, m.wndsize))
   ENDCASE

CASE objtype = c_otpopup
   DO CASE
   CASE m.g_char2grph
      * Force all popups to default height
      REPLACE height WITH m.g_pophght
   CASE m.g_grph2char
      m.newheight = 3
      REPLACE vpos WITH MAX(vpos + ((HEIGHT - m.newheight) / 2),0)
      REPLACE height WITH m.newheight
      REPLACE WIDTH WITH dossize(WIDTH, fontsize, m.wndsize)
   CASE m.g_grph2grph
      * Force all popups to default height
      REPLACE height WITH m.g_pophght
   ENDCASE

CASE objtype = c_ottxtbut
   DO CASE
   CASE m.g_char2grph
   	* Force all push buttons to default height when coming from DOS
      REPLACE height WITH m.g_btnheight
   CASE m.g_grph2char
      DO adjbitmapctrl

      REPLACE height WITH 1
      REPLACE spacing WITH ROUND(dossize(spacing, fontsize, m.wndsize), 0)
      REPLACE WIDTH WITH MAX(maxbtnwidth(PICTURE, "", "", "")+2, dossize(WIDTH, fontsize, m.wndsize))
	CASE m.g_grph2grph
		* This case is handled in fillininfo
   ENDCASE

CASE objtype = c_otfield
   DO CASE
   CASE m.g_char2grph
      REPLACE height WITH height + c_adjfld
   CASE m.g_grph2char
      IF INLIST(objcode,c_sgsay, c_sgget)
         REPLACE height WITH 1
      ELSE
         REPLACE height WITH MAX(dossize(HEIGHT, fontsize, m.wndsize),1)
      ENDIF
      REPLACE WIDTH WITH MAX(dossize(WIDTH, fontsize, m.wndsize),1)
   ENDCASE

CASE objtype = c_otline OR objtype = c_otbox
   IF m.g_grph2char
      DO adjbox WITH 0
   ENDIF
ENDCASE

IF m.g_grph2char OR m.g_char2grph AND INLIST(objtype,C_OBJTYPELIST)
  	REPLACE hpos WITH MAX(hpos,0)
	REPLACE vpos WITH MAX(vpos,0)
ENDIF

RETURN

*
* Columnar - This function takes and object and checks to see if it
*      is right or left aligned with other objects in a column.
*      Return values are:
*         0 - Not aligned
*         1 - Left aligned
*         2 - Right aligned
*
*!*****************************************************************************
*!
*!       Function: COLUMNAR
*!
*!      Called by: ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION columnar
PARAMETER m.vpos, m.hpos, m.type, m.otype
PRIVATE m.saverec

m.saverec = RECNO()

LOCATE FOR platform = m.g_fromplatform AND objtype = m.type AND ;
   hpos = m.hpos AND ABS(vpos - m.vpos) < m.vpos * 2
IF FOUND()
   GOTO RECORD (m.saverec)
   RETURN 1
ENDIF

LOCATE FOR platform = m.g_fromplatform AND objtype = m.type AND ;
   hpos + WIDTH = m.hpos + m.width  AND ;
   ABS(vpos - m.vpos) < m.vpos * 2
IF FOUND()
   GOTO RECORD (m.saverec)
   RETURN 2
ENDIF

GOTO RECORD (m.saverec)
RETURN 0

*
* DOSSize - This function attempts to normalize a dimension of an object to the font used for the
*      window it lies in.  Unfortunately, we can't use FONTMETRIC since this needs to run on a character
*      platform.  We use the ratio of point sizes.
*
*!*****************************************************************************
*!
*!       Function: DOSSIZE
*!
*!      Called by: ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION dossize
PARAMETER m.size, m.objsize, m.scrnsize
RETURN m.size * (m.objsize / m.scrnsize)

*
* AdjBitmapCtrl - Take the Picture clause for a control, see if it is a bitmap and
*      turn it into something that a character platform can handle.
*
*!*****************************************************************************
*!
*!      Procedure: ADJBITMAPCTRL
*!
*!      Called by: ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!
*!          Calls: STRIPPATH()        (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjbitmapctrl
PRIVATE m.function, m.oldpicture, m.newpicture, m.temp

m.function = ALLTRIM(SUBSTR(PICTURE, 1, AT(" ", PICTURE)))

IF AT("B", m.function) <> 0
   m.function = CHRTRANC(m.function, "B", "")
   m.oldpicture = ALLTRIM(SUBSTR(PICTURE, AT(" ", PICTURE)))
   m.newpicture = ""

   DO WHILE LEN(m.oldpicture) > 0
      IF AT(";", m.oldpicture) = 0
         m.temp = LEFT(m.oldpicture, LEN(m.oldpicture)-1)
         m.oldpicture = ""
      ELSE
         m.temp = LEFT(m.oldpicture, AT(";", m.oldpicture)-1)
         m.oldpicture = SUBSTR(m.oldpicture, AT(";", m.oldpicture)+1)
      ENDIF

      IF LEN(m.newpicture) = 0
         m.newpicture = ALLTRIM(strippath(m.temp))
      ELSE
         m.newpicture = m.newpicture + ";" + ALLTRIM(strippath(m.temp))
      ENDIF
   ENDDO

   REPLACE PICTURE WITH m.function + " " + m.newpicture + '"'
ENDIF

RETURN
*
* AdjColor - Adjust color fields in the database.
*
*!*****************************************************************************
*!
*!      Procedure: ADJCOLOR
*!
*!      Called by: ALLENVIRONS        (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!               : RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : LABELLINES         (procedure in TRANSPRT.PRG)
*!
*!          Calls: CONVERTCOLORPAIR   (procedure in TRANSPRT.PRG)
*!               : RGBTOX()           (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjcolor
DO CASE
CASE m.g_char2grph
   IF m.g_filetype = c_report OR m.g_filetype = c_label OR EMPTY(colorpair)
      IF m.g_filetype = c_screen
         REPLACE colorpair WITH ""
         REPLACE penred    WITH -1
         REPLACE pengreen  WITH -1
         REPLACE penblue   WITH -1
         REPLACE fillred   WITH -1
         REPLACE fillgreen WITH -1
         REPLACE fillblue  WITH -1
      ELSE
         REPLACE penred    WITH 0
         REPLACE pengreen  WITH 0
         REPLACE penblue   WITH 0
         IF objtype = c_otline
            REPLACE fillred   WITH 0
            REPLACE fillgreen WITH 0
            REPLACE fillblue  WITH 0
         ELSE
            REPLACE fillred   WITH 255
            REPLACE fillgreen WITH 255
            REPLACE fillblue  WITH 255
         ENDIF
      ENDIF
   ELSE
      DO convertcolorpair
   ENDIF
CASE m.g_grph2char
   IF m.g_filetype = c_screen
      DO CASE
      CASE objtype = c_otheader
         DO CASE
         CASE STYLE = c_user
            IF SCHEME + scheme2 = 0
               REPLACE SCHEME WITH 1
               REPLACE scheme2 WITH 2
            ENDIF

         CASE STYLE = c_system
            REPLACE SCHEME WITH 8
            REPLACE scheme2 WITH 9

         CASE STYLE = c_dialog
            REPLACE SCHEME WITH 5
            REPLACE scheme2 WITH 6

         CASE STYLE = c_alert
            REPLACE SCHEME WITH 7
            REPLACE SCHEME WITH 12
         ENDCASE

      CASE c_maptextcolor AND INLIST(objtype,c_otbox, c_otline,c_ottext)
         IF penred <> -1 OR fillred <> -1
            REPLACE colorpair WITH rgbtox(penred, penblue, pengreen) + "/" + ;
               rgbtox(fillred, fillblue, fillgreen)
            * Don't let it map to black on black
            IF colorpair = "N/N" OR TRIM(colorpair) == "/"
               REPLACE colorpair WITH ""
            ENDIF
         ENDIF
      OTHERWISE
          REPLACE scheme WITH 0   && default color scheme for everything else
      ENDCASE
   ENDIF
ENDCASE
RETURN

*
* RGBToX - Convert an RGB triplet to a traditional xBase color letter
*
*!*****************************************************************************
*!
*!       Function: RGBTOX
*!
*!      Called by: ADJCOLOR           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION rgbtox
PARAMETERS m.red, m.blue, m.green
PRIVATE m.color

*
* If it is automatic, we skip it.
*
IF m.red < 0 OR m.blue < 0 OR m.green < 0
   RETURN ""
ENDIF

*
* We use a special triplet for Light Gray which makes it a special case.
*
IF m.red = 192 AND m.blue = 192 AND m.green = 192
   RETURN "W"
ENDIF
IF _MAC AND m.red = 192 AND m.blue = 192 AND m.green = 192
   RETURN "W"
ENDIF

*
* This division makes sure that we give a letter for any possible triplet
*
m.red   = ROUND(m.red / 127, 0)
m.blue = ROUND(m.blue / 127, 0)
m.green = ROUND(m.green / 127, 0)

*
* Save some time by getting a number we can make a single comparison against
*
m.color = (m.red * 100) + (m.blue * 10) + m.green

DO CASE
CASE m.color = 222      && White
   RETURN "W+"
CASE m.color = 0        && Black
   RETURN "N"
CASE m.color = 111      && Dark Gray
   RETURN "N+"
CASE m.color = 200      && Light Red
   RETURN "R+"
CASE m.color = 100      && Dark Red
   RETURN "R"
CASE m.color = 220      && Yellow
   RETURN "GR+"
CASE m.color = 110      && Brown
   RETURN "GR"
CASE m.color = 2        && Light green
   RETURN "G+"
CASE m.color = 1        && Dark Green
   RETURN "G"
CASE m.color = 22       && Light Magenta
   RETURN "BG+"
CASE m.color = 11       && Dark Magenta
   RETURN "BG"
CASE m.color = 20       && Light Blue
   RETURN "B+"
CASE m.color = 10       && Dark Blue
   RETURN "B"
CASE m.color = 202      && Light Purple
   RETURN "RB+"
CASE m.color = 101      && Dark Purple
   RETURN "RB"
ENDCASE

RETURN ""      && It shouldn't be possible to reach this point.

*
* \ - Adjust pen attributes.
*
*!*****************************************************************************
*!
*!      Procedure: ADJPEN
*!
*!      Called by: FILLININFO         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjpen
IF m.g_char2grph
   DO CASE
   CASE objtype = c_ottext
      REPLACE pensize WITH 1
      REPLACE penpat  WITH 0
      REPLACE fillpat WITH 0

   OTHERWISE
      REPLACE pensize WITH 0
      REPLACE penpat  WITH 0
      REPLACE fillpat WITH 0
   ENDCASE
ENDIF
RETURN
*
* adjfont - Adjust font fields in the SCX or FRX database.
*
*!*****************************************************************************
*!
*!      Procedure: ADJFONT
*!
*!      Called by: ALLENVIRONS        (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!               : RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : LABELLINES         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjfont
PRIVATE m.i, m.outface, m.outsize, m.outstyle
m.outface  = fontface
m.outsize  = fontsize
m.outstyle = num2style(fontstyle)
DO CASE
CASE m.g_char2grph OR m.g_grph2grph
   DO CASE
   CASE objtype = c_otheader
		DO CASE
		CASE m.g_fontset
			* User chose a font with the "font" push button.  Use it for the
			* measurement font regardless of what used to be there.
    		REPLACE fontface  WITH m.g_dfltfface
     		REPLACE fontsize  WITH m.g_dfltfsize
     		REPLACE fontstyle WITH style2num(m.g_dfltfstyle)
		CASE commonfont(fontface)
			* Original measurement font was Arial, Courier, etc.  Leave it
			* alone.
		OTHERWISE
			* Use the defaults
    		REPLACE fontface  WITH m.g_windfface
     		REPLACE fontsize  WITH m.g_windfsize
     		REPLACE fontstyle WITH style2num(m.g_windfstyle)
		ENDCASE

   CASE INLIST(objtype,c_ottxtbut,c_otradbut,c_otchkbox,c_otinvbut,c_otspinner)
		IF !commonfont(fontface)
      	REPLACE fontface  WITH m.g_ctrlfface
      	REPLACE fontsize  WITH m.g_ctrlfsize
      	REPLACE fontstyle WITH style2num(m.g_ctrlfstyle)
		ENDIF

   CASE INLIST(objtype, c_otbox, c_otline)
		IF !commonfont(fontface)
     		REPLACE fontface  WITH m.g_ctrlfface
     		REPLACE fontsize  WITH m.g_ctrlfsize
			IF objtype = c_otbox AND m.g_filetype = c_screen AND style <> 0
				* Special case of rounded rectangles
				REPLACE fontstyle WITH 0
			ELSE
	     		REPLACE fontstyle WITH style2num(m.g_ctrlfstyle)
			ENDIF
		ENDIF

   CASE objtype = c_otpopup
		IF !commonfont(fontface)
      	REPLACE fontface  WITH m.g_ctrlfface
      	REPLACE fontsize  WITH m.g_ctrlfsize
      	REPLACE fontstyle WITH m.g_normstylenum
      ENDIF

   CASE objtype = c_ottext
      DO CASE
		CASE m.g_char2grph OR (m.g_grph2grph AND m.g_fontset)
         REPLACE fontface  WITH m.g_dfltfface
         REPLACE fontsize  WITH m.g_dfltfsize
         REPLACE fontstyle WITH m.g_boldstylenum
		CASE !commonfont(fontface)
        	DO mapfont WITH fontface, fontsize, num2style(fontstyle), m.outface, m.outsize, m.outstyle, _MAC
        	REPLACE fontface  WITH m.outface
        	REPLACE fontsize  WITH m.outsize
        	REPLACE fontstyle WITH style2num(m.outstyle)
      ENDCASE

   CASE objtype = c_otfield
		DO CASE
      CASE m.g_char2grph OR (m.g_grph2grph AND m.g_fontset)
         REPLACE fontface  WITH m.g_dfltfface
         REPLACE fontsize  WITH m.g_dfltfsize
         REPLACE fontstyle WITH m.g_normstylenum
      CASE !commonfont(fontface)
        	DO mapfont WITH fontface, fontsize, num2style(fontstyle), m.outface, m.outsize, m.outstyle, _MAC
        	REPLACE fontface  WITH m.outface
        	REPLACE fontsize  WITH m.outsize
        	REPLACE fontstyle WITH style2num(m.outstyle)
      ENDCASE

   OTHERWISE
		DO CASE
      CASE m.g_char2grph OR (m.g_grph2grph AND m.g_fontset)
         REPLACE fontface  WITH m.g_dfltfface
         REPLACE fontsize  WITH m.g_dfltfsize
         REPLACE fontstyle WITH m.g_normstylenum
      CASE !commonfont(fontface)
        	DO mapfont WITH fontface, fontsize, num2style(fontstyle), m.outface, m.outsize, m.outstyle, _MAC
        	REPLACE fontface  WITH m.outface
        	REPLACE fontsize  WITH m.outsize
        	REPLACE fontstyle WITH style2num(m.outstyle)
		ENDCASE
   ENDCASE
ENDCASE
RETURN

*!*****************************************************************************
*!
*!      Function: COMMONFONT
*!
*!*****************************************************************************
FUNCTION commonfont
* Is the font one that is in common for Mac and Windows?
PARAMETER m.thefont
m.thefont = UPPER(ALLTRIM(m.thefont))
RETURN INLIST(m.thefont, "ARIAL", "COURIER NEW", "TIMES NEW ROMAN")

*
* convertColorPair - Convert the color pair to appropriate RGB pen
*               and fill values.
*
*!*****************************************************************************
*!
*!      Procedure: CONVERTCOLORPAIR
*!
*!      Called by: ADJCOLOR           (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETCOLOR()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE convertcolorpair
PRIVATE m.oldscheme, m.rgbvalue, m.comma, m.frg, m.bkg

* Translate foreground colors
m.frg = UPPER(CHRTRANC(LEFT(colorpair,AT('/',colorpair)-1),'-*/, ',''))
REPLACE penred    WITH -1
REPLACE pengreen  WITH -1
REPLACE penblue   WITH -1
IF "W" $ m.frg
   REPLACE penred    WITH IIF('+' $ m.frg,255,128)
   REPLACE pengreen  WITH IIF('+' $ m.frg,255,128)
   REPLACE penblue   WITH IIF('+' $ m.frg,255,128)
ENDIF
IF "N" $ m.frg
   REPLACE penred    WITH 0
   REPLACE pengreen  WITH 0
   REPLACE penblue   WITH 0
ENDIF
IF "R" $ m.frg    && red
   REPLACE penred    WITH IIF('+' $ m.frg,255,128)
ENDIF
IF "G" $ m.frg    && green
   REPLACE pengreen  WITH IIF('+' $ m.frg,255,128)
ENDIF
IF "B" $ m.frg    && blue
   REPLACE penblue   WITH IIF('+' $ m.frg,255,128)
ENDIF
REPLACE penred   WITH IIF(penred < 0,0,penred)
REPLACE pengreen WITH IIF(pengreen < 0,0,pengreen)
REPLACE penblue  WITH IIF(penblue < 0,0,penblue)

m.bkg = UPPER(CHRTRANC(SUBSTR(colorpair,AT('/',colorpair)+1,3),'-*/, ',''))
REPLACE fillred    WITH -1
REPLACE fillgreen  WITH -1
REPLACE fillblue   WITH -1
DO CASE
CASE m.bkg = "W" OR m.bkg = "W+"    && white
   REPLACE fillred    WITH IIF('+' $ m.bkg,255,128)
   REPLACE fillgreen  WITH IIF('+' $ m.bkg,255,128)
   REPLACE fillblue   WITH IIF('+' $ m.bkg,255,128)
CASE m.bkg = "N" OR m.bkg = "N+"    && black
   REPLACE fillred    WITH 0
   REPLACE fillgreen  WITH 0
   REPLACE fillblue   WITH 0
CASE "R" $ m.bkg OR "G" $ m.bkg OR "B" $ m.bkg
   IF "R" $ m.bkg    && red
      REPLACE fillred    WITH IIF('+' $ m.bkg,255,128)
   ENDIF
   IF "G" $ m.bkg    && green
      REPLACE fillgreen  WITH IIF('+' $ m.bkg,255,128)
   ENDIF
   IF "B" $ m.bkg    && blue
      REPLACE fillblue   WITH IIF('+' $ m.bkg,255,128)
   ENDIF
   REPLACE fillred   WITH IIF(fillred < 0,0,fillred)
   REPLACE fillgreen WITH IIF(fillgreen < 0,0,fillgreen)
   REPLACE fillblue  WITH IIF(fillblue < 0,0,fillblue)
ENDCASE
RETURN

* getColor - Return the color value for a specified RGB value.
*
*!*****************************************************************************
*!
*!       Function: GETCOLOR
*!
*!      Called by: CONVERTCOLORPAIR   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getcolor
PARAMETER m.rgbstring, m.occurence
PRIVATE m.comma, m.value
m.comma = ATC(',', m.rgbstring, m.occurence)
m.value = SUBSTR(m.rgbstring, m.comma +1, ;
   ATC(',', m.rgbstring, m.occurence + 1)-m.comma -1)
RETURN m.value

*
*num2style - Return the style string which corresponds to the style
*         stored in screen database.
*
*!*****************************************************************************
*!
*!       Function: num2style
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : FILLININFO         (procedure in TRANSPRT.PRG)
*!               : ITEMSINBOXES       (procedure in TRANSPRT.PRG)
*!               : GETWINDFONT        (procedure in TRANSPRT.PRG)
*!               : ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION num2style
PARAMETER m.stylenum
PRIVATE m.i, m.strg, m.pow, m.stylechars, m.outstrg

DO CASE
CASE TYPE("m.stylenum") = "C"
   * already a character.  Do nothing.
   RETURN m.stylenum
CASE !EMPTY(m.stylenum)
	m.strg = ""
	* These are the style characters.  Their position in the string matches the bit
	* position in the num byte.
	m.stylechars = "BIUOSCE-"

	* Look at each of the bits in the stylenum byte
	FOR m.i = 8 TO 1 STEP -1
   	m.pow = ROUND(2^(i-1),0)
		IF m.stylenum >= m.pow
	   	m.strg = m.strg + SUBSTR(stylechars,m.i,1)
		ENDIF
		IF m.pow <> 0
		   m.stylenum = m.stylenum % m.pow
      ENDIF
	ENDFOR

	* Now reverse the string so that style codes appear in the traditional order
	m.outstrg = ""
	FOR m.i = 1 TO LEN(m.strg)
   	m.outstrg = m.outstrg + SUBSTR(m.strg,LEN(m.strg)+1-m.i,1)
	ENDFOR
	RETURN m.outstrg
OTHERWISE
   RETURN ""
ENDCASE
*!*****************************************************************************
*!
*!       Function: style2num
*!
*!*****************************************************************************
FUNCTION style2num
* Map style code (e.g., "B") to screen/report numeric style code (e.g., 1)
PARAMETER m.strg
PRIVATE m.num, m.i
m.strg= UPPER(ALLTRIM(m.strg))
DO CASE
CASE TYPE("m.strg") $ "NF"
   * already a number. Do nothing.
   RETURN m.strg
CASE !EMPTY(strg)
	m.num = 0
	FOR m.i = 1 TO LEN(m.strg)
   	DO CASE
   	CASE SUBSTR(m.strg,i,1) = "B"      && bold
      	m.num = m.num + 1
   	CASE SUBSTR(m.strg,i,1) = "I"	     && italic
      	m.num = m.num + 2
   	CASE SUBSTR(m.strg,i,1) = "U"      && underlined
      	m.num = m.num + 4
   	CASE SUBSTR(m.strg,i,1) = "O"      && outline
      	m.num = m.num + 8
   	CASE SUBSTR(m.strg,i,1) = "S"      && shadow
      	m.num = m.num + 16
   	CASE SUBSTR(m.strg,i,1) = "C"	     && condensed
      	m.num = m.num + 32
   	CASE SUBSTR(m.strg,i,1) = "E"      && extended
      	m.num = m.num + 64
   	CASE SUBSTR(m.strg,i,1) = "-"      && strikeout
      	m.num = m.num + 128
   	ENDCASE
	ENDFOR
	RETURN m.num
OTHERWISE
   RETURN 0
ENDCASE

*
* AdjText - Takes the current record and, if it is a multi-line text object, converts it into
*      multiple single line text objects.
*
*!*****************************************************************************
*!
*!      Procedure: ADJTEXT
*!
*!      Called by: RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjtext
PARAMETER m.oldwidth

PRIVATE m.saverec

IF objtype <> c_ottext OR AT(CHR(13), expr) = 0 OR !m.g_grph2char
   RETURN
ENDIF

m.saverec = RECNO()
SCATTER MEMVAR MEMO

* Update the original records
m.expr = SUBSTR(m.expr, 2, LEN(m.expr)-2)
m.pos = AT(CHR(13), m.expr)
REPLACE expr WITH '"' + LEFT(m.expr, m.pos-1) + '"'
REPLACE WIDTH WITH LEN(expr)-2
DO CASE
CASE m.picture = '"@J"'                        && Right aligned
   REPLACE hpos WITH hpos + m.oldwidth - WIDTH
CASE m.picture = '"@I"'                        && Centered
   REPLACE hpos WITH hpos + (m.oldwidth - WIDTH)/2
ENDCASE
m.expr = SUBSTR(m.expr, m.pos+1)
m.pos = AT(CHR(13), m.expr)
REPLACE hpos WITH MAX(0,hpos)

* Write all records but the last
DO WHILE m.pos > 0
   m.vpos = m.vpos + IIF(spacing = 1, m.height * 2, m.height)
   APPEND BLANK
   GATHER MEMVAR MEMO
   REPLACE platform WITH LOWER(platform)
   REPLACE uniqueid WITH SYS(2015)
   REPLACE expr WITH '"' + LEFT(m.expr, m.pos-1) + '"'
   REPLACE WIDTH WITH LEN(expr)-2
   DO CASE
   CASE m.picture = '"@J"'                     && Right aligned
      REPLACE hpos WITH hpos + m.oldwidth - WIDTH
   CASE m.picture = '"@I"'                     && Centered
      REPLACE hpos WITH hpos + (m.oldwidth - WIDTH)/2
   ENDCASE

   m.expr = SUBSTR(m.expr, m.pos+1)
   m.pos = AT(CHR(13), m.expr)
   REPLACE hpos WITH MAX(0,hpos)
ENDDO

* Write the last record.
IF LEN(ALLTRIM(m.expr)) <> 0
   m.vpos = m.vpos + IIF(spacing = 1, m.height * 2, m.height)
   APPEND BLANK
   GATHER MEMVAR MEMO
   REPLACE platform WITH LOWER(platform)
   REPLACE uniqueid WITH SYS(2015)
   REPLACE expr WITH '"' + m.expr + '"'
   REPLACE WIDTH WITH LEN(expr)-2
   DO CASE
   CASE m.picture = '"@J"'                     && Right aligned
      REPLACE hpos WITH hpos + m.oldwidth - WIDTH
   CASE m.picture = '"@I"'                     && Centered
      REPLACE hpos WITH hpos + (m.oldwidth - WIDTH)/2
   ENDCASE
   REPLACE hpos WITH MAX(0,hpos)
ENDIF

GOTO m.saverec
RETURN

*
*
* AdjBox - Converts a box/line record from character to graphic or graphic to character
*
*!*****************************************************************************
*!
*!      Procedure: ADJBOX
*!
*!      Called by: RPTOBJCONVERT      (procedure in TRANSPRT.PRG)
*!               : REPOOBJECTS        (procedure in TRANSPRT.PRG)
*!               : ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETLINEWIDTH()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE adjbox
PARAMETER m.adjust
DO CASE
CASE m.g_char2grph
   DO CASE
   CASE objcode = c_sgboxd
      REPLACE pensize WITH 4
   CASE objcode = c_sgboxp
      REPLACE pensize WITH 6
   OTHERWISE
      REPLACE pensize WITH 1
   ENDCASE

   DO CASE
   CASE height = 1
      REPLACE height WITH getlinewidth(objcode, .T.)
      REPLACE vpos WITH vpos + c_adjbox - (HEIGHT/2)
      IF m.g_filetype = c_screen
         REPLACE STYLE WITH c_lnhorizontal
      ENDIF

      REPLACE penpat  WITH 8
      REPLACE fillpat WITH 0
      REPLACE objtype WITH c_otline
      REPLACE objcode WITH 0

   CASE WIDTH = 1
      REPLACE WIDTH WITH getlinewidth(objcode, .F.)
      REPLACE hpos WITH hpos + c_adjbox - (WIDTH/2)
      IF m.g_filetype = c_screen
         REPLACE STYLE WITH c_lnvertical
      ENDIF

      REPLACE penpat  WITH 8
      REPLACE fillpat WITH 0
      REPLACE objtype WITH c_otline
      REPLACE objcode WITH 0

   OTHERWISE
      REPLACE vpos WITH vpos + c_adjbox - (getlinewidth(objcode, .T.)/2) + m.adjust
      REPLACE hpos WITH hpos + c_adjbox - (getlinewidth(objcode, .F.)/2) + m.adjust
      REPLACE height WITH height + getlinewidth(objcode, .T.) - 1
      REPLACE WIDTH WITH WIDTH + getlinewidth(objcode, .F.) - 1

      REPLACE penpat  WITH 8
      REPLACE fillpat WITH 0
      REPLACE objcode WITH 4
   ENDCASE

   IF m.g_filetype = c_screen
      IF BORDER > 4
         REPLACE BORDER WITH 1
      ELSE
         REPLACE BORDER WITH 0
      ENDIF
   ENDIF
CASE m.g_grph2char
   ******************* Start Graphic to Character Conversion ******************
   IF fillpat = 0
      REPLACE fillchar WITH CHR(0)
   ELSE
      REPLACE fillchar WITH " "
   ENDIF

   DO CASE
   CASE pensize = 4
      REPLACE objcode WITH c_sgboxd
   CASE pensize = 6
      REPLACE objcode WITH c_sgboxp
   OTHERWISE
      REPLACE objcode WITH c_sgbox
   ENDCASE

   DO CASE
   CASE (m.g_filetype = c_screen AND objtype = c_otline and style = c_lnhorizontal) ;
        OR (objtype = c_otbox and height <=1)
      REPLACE vpos WITH vpos - c_adjbox
      REPLACE height WITH 1
   CASE (m.g_filetype = c_screen AND objtype = c_otline and style = c_lnvertical) ;
        OR (objtype = c_otbox and width <=1)
      REPLACE hpos WITH hpos-c_adjbox
      REPLACE width WITH 1
   OTHERWISE
      REPLACE vpos WITH vpos-c_adjbox
      REPLACE hpos WITH hpos-c_adjbox
      REPLACE height WITH height+(c_adjbox*2)
      REPLACE WIDTH WITH WIDTH+(c_adjbox*2)
   ENDCASE
ENDCASE
RETURN

*
* GetLineWidth - Given an object code for a box or line and a flag indicating
*      if we want the thickness of a horizontal or vertical size, we return
*      the thickness of the side.
*
*!*****************************************************************************
*!
*!       Function: GETLINEWIDTH
*!
*!      Called by: JOINHORIZONTAL     (procedure in TRANSPRT.PRG)
*!               : JOINVERTICAL       (procedure in TRANSPRT.PRG)
*!               : REJOINBOXES        (procedure in TRANSPRT.PRG)
*!               : JOINLINEWIDTH()    (function  in TRANSPRT.PRG)
*!               : ADJBOX             (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getlinewidth
PARAMETERS m.objcode, m.horizontal

IF _WINDOWS OR _MAC
   DO CASE
   CASE m.objcode = c_sgboxd
      IF m.g_filetype = c_report
         RETURN 4 / FONTMETRIC(IIF(m.horizontal, 1, 6), m.g_rptfface, m.g_rptfsize, m.g_rpttxtfontstyle)
      ELSE
         RETURN 4 / FONTMETRIC(IIF(m.horizontal, 1, 6), m.g_dfltfface, m.g_dfltfsize, "B")
      ENDIF

   CASE m.objcode = c_sgboxp
      IF m.g_filetype = c_report
         RETURN 6 / FONTMETRIC(IIF(m.horizontal, 1, 6), m.g_rptfface, m.g_rptfsize, m.g_rpttxtfontstyle)
      ELSE
         RETURN 6 / FONTMETRIC(IIF(m.horizontal, 1, 6), m.g_dfltfface, m.g_dfltfsize, "B")
      ENDIF

   OTHERWISE
      IF m.g_filetype = c_report
         RETURN 1 / FONTMETRIC(IIF(m.horizontal, 1, 6), m.g_rptfface, m.g_rptfsize, m.g_rpttxtfontstyle)
      ELSE
         RETURN 1 / FONTMETRIC(IIF(m.horizontal, 1, 6), m.g_dfltfface, m.g_dfltfsize, "B")
      ENDIF
   ENDCASE
ELSE
   RETURN 1
ENDIF

*
* HorizButton - Will return a .T. if the ojbect passed in is a series of
*            horizontal buttons.  If they are vertical buttons, it
*            returns .F.
*
*!*****************************************************************************
*!
*!       Function: HORIZBUTTON
*!
*!      Called by: CALCWINDOWDIMENSION(procedure in TRANSPRT.PRG)
*!               : FINDWIDEROBJECTS   (procedure in TRANSPRT.PRG)
*!               : REPOOBJECTS        (procedure in TRANSPRT.PRG)
*!               : ITEMSINBOXES       (procedure in TRANSPRT.PRG)
*!               : ADJINVBTNS         (procedure in TRANSPRT.PRG)
*!               : GETLASTOBJECTLINE()(function  in TRANSPRT.PRG)
*!               : GETOBJWIDTH()      (function  in TRANSPRT.PRG)
*!               : GETOBJHEIGHT()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION horizbutton
PARAMETER m.pictclause

IF OCCURS(';', m.pictclause) = 0 OR ;
      AT("H", LEFT(m.pictclause, AT(" ", m.pictclause))) != 0
   RETURN .T.
ELSE
   RETURN .F.
ENDIF

*
* MaxBtnWidth - Given the Picture clause for a set of buttons (text or
*      radio) along with its font information and returns the Width in
*      foxels of the widest label.
*
*!*****************************************************************************
*!
*!       Function: MAXBTNWIDTH
*!
*!      Called by: ADJHEIGHTANDWIDTH  (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION maxbtnwidth
PARAMETERS m.picture, m.face, m.size, m.style
PRIVATE m.max, m.label

m.max = 0
m.picture = SUBSTR(m.picture, AT(" ", m.picture))

m.picture = STRTRAN(m.picture, "\\", "")
m.picture = STRTRAN(m.picture, "\<", "")
m.picture = STRTRAN(m.picture, "\!", "")
m.picture = STRTRAN(m.picture, "\?", "")

DO WHILE LEN(m.picture) != 0
   IF AT(";", m.picture) != 0
      m.label = ALLTRIM(LEFT(m.picture, AT(";", m.picture)-1))
      m.picture = SUBSTR(m.picture, AT(";", m.picture)+1)
   ELSE
      m.label = ALLTRIM(LEFT(m.picture, LEN(m.picture)-1))
      m.picture = ""
   ENDIF

   DO CASE
   CASE m.g_char2grph OR m.g_grph2grph
      m.max = MAX(m.max, TXTWIDTH(m.label, m.face, m.size, m.style))
   CASE m.g_grph2char
      m.max = MAX(m.max, LEN(m.label))
   ENDCASE
ENDDO

RETURN m.max

*
* GetObjWidth - Given a screen object, this function returns its Width.
*
*!*****************************************************************************
*!
*!       Function: GETOBJWIDTH
*!
*!      Called by: ITEMSINBOXES       (procedure in TRANSPRT.PRG)
*!               : GETRIGHTMOST       (procedure in TRANSPRT.PRG)
*!
*!          Calls: HORIZBUTTON()      (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getobjwidth
PARAMETERS m.objtype, m.picture, m.width, m.spacing, m.platform
PRIVATE m.numitems

DO CASE
CASE m.objtype = c_ottext OR m.objtype = c_otfield OR ;
      m.objtype = c_otline OR m.objtype = c_otbox OR ;
      m.objtype = c_otlist OR m.objtype = c_otchkbox OR ;
      m.objtype = c_otpopup OR m.objtype = c_otpicture OR ;
      m.objtype = c_otspinner OR m.objtype = c_otrepfld
   RETURN m.width

CASE m.objtype = c_ottxtbut OR m.objtype = c_otradbut OR m.objtype = c_otinvbut
   m.numitems = OCCURS(";", m.picture) + 1
   IF !horizbutton(m.picture) OR m.numitems = 1
      RETURN m.width
   ELSE
      RETURN (m.width * m.numitems) + (m.spacing * (m.numitems - 1))
   ENDIF

CASE (m.objtype = c_otbox OR m.objtype = c_otline) AND ;
      (m.platform = c_macname OR m.platform = c_winname)
   RETURN m.width

CASE (m.objtype = c_otbox OR m.objtype = c_otline) AND ;
      (m.platform = c_dosname OR m.platform = c_unixname)
   RETURN m.width-1

OTHERWISE
   RETURN m.width
ENDCASE

*
* GetObjHeight - Given a screen object, this function returns its Height.
*
*!*****************************************************************************
*!
*!       Function: GETOBJHEIGHT
*!
*!      Called by: GETLOWEST          (procedure in TRANSPRT.PRG)
*!
*!          Calls: HORIZBUTTON()      (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getobjheight
PARAMETERS m.objtype, m.picture, m.height, m.spacing, m.platform
PRIVATE m.numitems

DO CASE
CASE m.objtype = c_ottext OR m.objtype = c_otfield OR ;
      m.objtype = c_otline OR m.objtype = c_otbox OR ;
      m.objtype = c_otlist OR m.objtype = c_otchkbox OR ;
      m.objtype = c_otpopup OR m.objtype = c_otpicture OR ;
      m.objtype = c_otspinner OR m.objtype = c_otrepfld
   RETURN m.height

CASE m.objtype = c_ottxtbut OR m.objtype = c_otradbut OR ;
      m.objtype = c_otinvbut
   m.numitems = OCCURS(";", m.picture) + 1

   IF horizbutton(m.picture) OR m.numitems = 1
      RETURN m.height
   ELSE
      RETURN (m.height * m.numitems) + (m.spacing * (m.numitems - 1))
   ENDIF

CASE (m.objtype = c_otbox OR m.objtype = c_otline) AND ;
      (m.platform = c_macname OR m.platform = c_winname)
   RETURN m.height

CASE (m.objtype = c_otbox OR m.objtype = c_otline) AND ;
      (m.platform = c_dosname OR m.platform = c_unixname)
   RETURN m.height-1

OTHERWISE
   RETURN m.height
ENDCASE

*
* GetRightmost - Takes a platform and returns the rightmost position occupied by an object
*      in that platform
*!*****************************************************************************
*!
*!      Procedure: GETRIGHTMOST
*!
*!      Called by: MAKECHARFIT        (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETOBJWIDTH()      (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE getrightmost
PARAMETER m.platform
PRIVATE m.right

m.right = 0

SCAN FOR platform = m.platform AND !DELETED() AND ;
      (objtype = c_ottext OR objtype = c_otline OR ;
      objtype = c_otbox OR objtype = c_otrepfld OR ;
      objtype = c_otlist OR objtype = c_ottxtbut OR ;
      objtype = c_otradbut OR objtype = c_otchkbox OR ;
      objtype = c_otfield OR objtype = c_otpopup OR ;
      objtype = c_otpicture OR objtype = c_otinvbut OR ;
      objtype = c_otspinner)
   m.right = MAX(m.right, hpos + getobjwidth(objtype, PICTURE, WIDTH, spacing, m.g_toplatform))
ENDSCAN

RETURN m.right

*
* GetLowest - Takes a platform and returns the lowest position occupied by an object
*      in that platform
*!*****************************************************************************
*!
*!      Procedure: GETLOWEST
*!
*!      Called by: MAKECHARFIT        (procedure in TRANSPRT.PRG)
*!
*!          Calls: GETOBJHEIGHT()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE getlowest
PARAMETER m.platform
PRIVATE m.bottom

m.bottom = 0

SCAN FOR platform = m.platform AND !DELETED() AND ;
      (objtype = c_ottext OR objtype = c_otline OR ;
      objtype = c_otbox OR objtype = c_otrepfld OR ;
      objtype = c_otlist OR objtype = c_ottxtbut OR ;
      objtype = c_otradbut OR objtype = c_otchkbox OR ;
      objtype = c_otfield OR objtype = c_otpopup OR ;
      objtype = c_otpicture OR objtype = c_otinvbut OR ;
      objtype = c_otspinner)
   m.bottom = MAX(m.bottom, vpos + getobjheight(objtype, PICTURE, HEIGHT, spacing, m.g_toplatform))
ENDSCAN

RETURN m.bottom

*
* DoCreate - Creates an empty cursor with either a report or screen structure and a given name.
*
*!*****************************************************************************
*!
*!      Procedure: DOCREATE
*!
*!      Called by: cvrt102FRX()    (function  in TRANSPRT.PRG)
*!               : cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!               : MAKECURSOR         (procedure in TRANSPRT.PRG)
*!               : WRITERESULT        (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE docreate
PARAMETER m.name, m.type
DO CASE
CASE m.type = c_screen
   CREATE CURSOR (m.name) (platform C(8), uniqueid C(10), timestamp N(10), objtype N(2), objcode N(3), ;
      name m, expr m, vpos N(7,3), hpos N(7,3), HEIGHT N(7,3), WIDTH N(7,3), ;
      STYLE N(2), PICTURE m, ORDER m, "unique" l, comment m, ENVIRON l, ;
      boxchar C(1), fillchar C(1), TAG m, tag2 m, penred N(5), pengreen N(5), ;
      penblue N(5), fillred N(5), fillgreen N(5), fillblue N(5), pensize N(5), ;
      penpat N(5), fillpat N(5), fontface m, fontstyle N(3), fontsize N(3), ;
      mode N(3), ruler N(1), rulerlines N(1), grid l, gridv N(2), gridh N(2), ;
      SCHEME N(2), scheme2 N(2), colorpair C(8), lotype N(1), rangelo m, ;
      hitype N(1), rangehi m, whentype N(1), WHEN m, validtype N(1), VALID m, ;
      errortype N(1), ERROR m, messtype N(1), MESSAGE m, showtype N(1), SHOW m, ;
      activtype N(1), ACTIVATE m, deacttype N(1), DEACTIVATE m, proctype N(1), ;
      proccode m, setuptype N(1), setupcode m, FLOAT l, CLOSE l, MINIMIZE l, ;
      BORDER N(1), SHADOW l, CENTER l, REFRESH l, disabled l, scrollbar l, ;
      addalias l, TAB l, initialval m, initialnum N(3), spacing N(6,3), curpos l)

CASE m.type = c_report OR m.type = c_label
	*- added user field for 3.0 reports (11/14/95 jd)
   CREATE CURSOR (m.name) (platform C(8), uniqueid C(10), timestamp N(10), objtype N(2), objcode N(3), ;
      name m, expr m, vpos N(9,3), hpos N(9,3), HEIGHT N(9,3), WIDTH N(9,3), ;
      STYLE m, PICTURE m, ORDER m, "unique" l, comment m, ENVIRON l, ;
      boxchar C(1), fillchar C(1), TAG m, tag2 m, penred N(5), pengreen N(5), ;
      penblue N(5), fillred N(5), fillgreen N(5), fillblue N(5), pensize N(5), ;
      penpat N(5), fillpat N(5), fontface m, fontstyle N(3), fontsize N(3), ;
      mode N(3), ruler N(1), rulerlines N(1), grid l, gridv N(2), gridh N(2), ;
      FLOAT l, STRETCH l, stretchtop l, TOP l, BOTTOM l, suptype N(1), suprest N(1), ;
      norepeat l, resetrpt N(2), pagebreak l, colbreak l, resetpage l, GENERAL N(3), ;
      spacing N(3), DOUBLE l, swapheader l, swapfooter l, ejectbefor l, ejectafter l, ;
      PLAIN l, SUMMARY l, addalias l, offset N(3), topmargin N(3), botmargin N(3), ;
      totaltype N(2), resettotal N(2), resoid N(3), curpos l, supalways l, supovflow l, ;
      suprpcol N(1), supgroup N(2), supvalchng l, supexpr m, user m)
CASE m.type = c_project
   CREATE CURSOR (m.name) ;
      (name m, ;
      TYPE C(1), ;
      timestamp N(10), ;
      outfile m, ;
      homedir m, ;
      setid N(4), ;
      exclude l, ;
      mainprog l, ;
      arranged m, ;
      savecode l, ;
      defname l, ;
      openfiles l, ;
      closefiles l, ;
      defwinds l, ;
      relwinds l, ;
      readcycle l, ;
      multreads l, ;
      NOLOCK l, ;
      MODAL l, ;
      assocwinds m, ;
      DEBUG l, ;
      ENCRYPT l, ;
      nologo l, ;
      scrnorder N(3), ;
      cmntstyle N(1), ;
      objrev N(5), ;
      commands m, ;
      devinfo m, ;
      symbols m, ;
      OBJECT m, ;
      ckval N(6) ;
      )
ENDCASE
RETURN

*
* makecursor - Create a cursor with the structure we need for this file on the 2.5 platform.
*
*!*****************************************************************************
*!
*!      Procedure: MAKECURSOR
*!
*!      Called by: TRANSPRT.PRG
*!               : CONVERTER          (procedure in TRANSPRT.PRG)
*!
*!          Calls: DOCREATE           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE makecursor
PRIVATE m.temp20alias, m.in_del

LOCAL cOldCPTrans, m.cTag2
cOldCPTrans = SET("NOCPTRANS")

m.temp20alias = "S"+SUBSTR(LOWER(SYS(3)),2,8)
DO docreate WITH m.temp20alias, m.g_filetype
m.in_del = SET("DELETED")
SET DELETED ON
IF TYPE("tag") == 'C' AND TYPE("tag2") == 'C'	&& make sure the fields are there (RED00VZ1 jd 06/20/96)
	SET NOCPTRANS TO tag, tag2
ENDIF
APPEND FROM (m.g_scrndbf)
IF TYPE("tag") == 'C' AND TYPE("tag2") == 'C'	&& make sure the fields are there (RED00VZ1 jd 06/20/96)
	*- codepage translation workaround
	GO TOP
	IF !EMPTY(tag2)
		SELECT (g_scrnalias)
		GO TOP
		m.cTag2 = tag2
		SELECT (m.temp20alias)
		GO TOP
		REPLACE tag2 WITH m.cTag2
	ENDIF
ENDIF
SET NOCPTRANS TO &cOldCPTrans
SET DELETED &in_del

m.g_20alias = m.g_scrnalias
m.g_scrnalias = m.temp20alias


*
* AddGraphicalLabelGroups - Add page and column header records for a label.
*
*!*****************************************************************************
*!
*!      Procedure: ADDGRAPHICALLABELGROUPS
*!
*!      Called by: ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : UPDATELABELDATA    (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE addgraphicallabelgroups

IF m.g_char2grph
   * First make sure that we don't already have these headers.  Check for a page header.
   LOCATE FOR objtype = c_otband AND objcode = 1
   IF FOUND()
      * We already have a page header.  We don't want two.  Reports, like people, function
      * best with only a single head.
      RETURN
   ENDIF

   APPEND BLANK
   REPLACE objtype WITH c_otband
   REPLACE objcode WITH 1
   REPLACE height WITH 0
   REPLACE pagebreak WITH .F.
   REPLACE colbreak WITH .F.
   REPLACE resetpage WITH .F.
   REPLACE platform WITH m.g_toplatform
   REPLACE uniqueid WITH SYS(2015)

   APPEND BLANK
   REPLACE objtype WITH c_otband
   REPLACE objcode WITH 2
   REPLACE height WITH 0
   REPLACE pagebreak WITH .F.
   REPLACE colbreak WITH .F.
   REPLACE resetpage WITH .F.
   REPLACE platform WITH m.g_toplatform
   REPLACE uniqueid WITH SYS(2015)

   APPEND BLANK
   REPLACE objtype WITH c_otband
   REPLACE objcode WITH 6
   REPLACE height WITH 0
   REPLACE pagebreak WITH .F.
   REPLACE colbreak WITH .F.
   REPLACE resetpage WITH .F.
   REPLACE platform WITH m.g_toplatform
   REPLACE uniqueid WITH SYS(2015)

   APPEND BLANK
   REPLACE objtype WITH c_otband
   REPLACE objcode WITH 7
   REPLACE height WITH 0
   REPLACE pagebreak WITH .F.
   REPLACE colbreak WITH .F.
   REPLACE resetpage WITH .F.
   REPLACE platform WITH m.g_toplatform
   REPLACE uniqueid WITH SYS(2015)
ENDIF

*
* UpdateLabelData - Labels live in report dataases now and we need to add at least one band
*            record if we are coming from a 2.0 label.
*
*!*****************************************************************************
*!
*!      Procedure: UPDATELABELDATA
*!
*!      Called by: CONVERTER          (procedure in TRANSPRT.PRG)
*!
*!          Calls: ADDGRAPHICALLABELGR(procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE updatelabeldata
PARAMETER m.lbxnumacross, m.lbxlmargin, m.lbxspacesbet, m.lbxlinesbet, m.lbxheight

DO addgraphicallabelgroups

* We need a detail band for any platform.
APPEND BLANK
REPLACE objtype WITH c_otband
REPLACE objcode WITH 4
REPLACE height WITH m.lbxheight
REPLACE pagebreak WITH .F.
REPLACE colbreak WITH .F.
REPLACE resetpage WITH .F.

LOCATE FOR objtype = c_ot20label
IF FOUND()
   REPLACE vpos WITH m.lbxnumacross
   REPLACE hpos WITH m.lbxlmargin
   REPLACE height WITH m.lbxspacesbet
   REPLACE penblue WITH m.lbxlinesbet
ENDIF

*
* PlatformDefaults - Writes information to a record that would not exist on the source platform and
*         we don't add elsewhere.
*
*!*****************************************************************************
*!
*!      Procedure: PLATFORMDEFAULTS
*!
*!      Called by: CONVERTER          (procedure in TRANSPRT.PRG)
*!               : NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE platformdefaults
PARAMETER m.timestamp

IF m.timestamp > 0
   REPLACE uniqueid WITH SYS(2015)
   REPLACE timestamp WITH m.timestamp
   REPLACE platform WITH m.g_fromplatform
ENDIF

IF m.g_char2grph
   REPLACE ruler WITH 1             && inches
   REPLACE rulerlines WITH 1
   REPLACE grid WITH .T.
   REPLACE gridv WITH 9
   REPLACE gridh WITH 9
ENDIF

*
* converter - Convert a 2.0 screen or report to 2.5 format and fill in the
*            appropriate fields.
*
*!*****************************************************************************
*!
*!      Procedure: CONVERTER
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: MAKECURSOR         (procedure in TRANSPRT.PRG)
*!               : UPDATELABELDATA    (procedure in TRANSPRT.PRG)
*!               : CONVERTPROJECT     (procedure in TRANSPRT.PRG)
*!               : STAMPVAL()         (function  in TRANSPRT.PRG)
*!               : PLATFORMDEFAULTS   (procedure in TRANSPRT.PRG)
*!               : UPDATEVERSION      (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE converter
PRIVATE m.lbxnumacross, m.lbxlmargin, m.lbxspacesbet, m.lbxlinesbet, m.lbxheight, m.timestamp

DO CASE
CASE m.g_filetype = c_label
   LOCATE FOR objtype = c_ot20label
   IF FOUND()
      m.lbxnumacross   = numacross
      m.lbxlmargin     = lmargin
      m.lbxspacesbet   = spacesbet
      m.lbxlinesbet    = linesbet
      m.lbxheight      = HEIGHT
   ENDIF
ENDCASE

DO makecursor

DO CASE
CASE m.g_filetype = c_label
   DO updatelabeldata WITH m.lbxnumacross, m.lbxlmargin, m.lbxspacesbet, m.lbxlinesbet, m.lbxheight
CASE m.g_filetype = c_project
   DO convertproject
   RETURN
ENDCASE

m.timestamp = stampval()
SCAN
   DO platformdefaults WITH m.timestamp
ENDSCAN

DO updateversion

*
* UpdateVersion - Places the correct version number in the m.g_fromPlatfrom
*      records.
*!*****************************************************************************
*!
*!      Procedure: UPDATEVERSION
*!
*!      Called by: CONVERTER          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE updateversion
LOCATE FOR platform = c_dosname AND objtype = c_otheader
IF FOUND()
   DO CASE
   CASE m.g_filetype = c_screen
      REPLACE objcode WITH c_25scx
   OTHERWISE
      REPLACE objcode WITH c_25frx
   ENDCASE
ENDIF

*
* SynchTime - Takes the names of two platforms and makes the timestamp of the header (objectype = 1)
*      record for the first platfrom match the timestamp of the header record of the second.
*
*!*****************************************************************************
*!
*!      Procedure: SYNCHTIME
*!
*!      Called by: TRANSPRT.PRG
*!
*!*****************************************************************************
PROCEDURE synchtime
PARAMETER m.convertedplatform, m.matchplatform
PRIVATE m.timestamp
LOCATE FOR platform = m.matchplatform AND objtype = c_otheader
IF FOUND()
   m.timestamp = timestamp
   LOCATE FOR platform = m.convertedplatform AND objtype = c_otheader
   IF FOUND()
      REPLACE timestamp WITH m.timestamp
   ENDIF
ENDIF

*
* Get a timestamp value based on the current date and time.
*
*!*****************************************************************************
*!
*!       Function: STAMPVAL
*!
*!      Called by: CONVERTER          (procedure in TRANSPRT.PRG)
*!
*!          Calls: SHIFTL()           (function  in TRANSPRT.PRG)
*!               : SHIFTR()           (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION stampval
PRIVATE m.dateval, m.timeval

m.dateval = DAY(DATE()) + ;
   shiftl(MONTH(DATE()), 5) + ;
   shiftl(YEAR(DATE())-1980, 9)

m.timeval = shiftr(VAL(RIGHT(TIME(),2)),1) + ;
   shiftl(VAL(SUBSTR(TIME(),4,2)),5) + ;
   shiftl(VAL(LEFT(TIME(),2)),11)

RETURN shiftl(m.dateval,16)+m.timeval

*
* Shift a value x times to the left.  (This isn't a true match for
* a shift since we keep extending the value without truncating it,
* but it works for us.)
*
*!*****************************************************************************
*!
*!       Function: SHIFTL
*!
*!      Called by: STAMPVAL()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION shiftl
PARAMETER m.value, m.times
PRIVATE m.loop

FOR m.loop = 1 TO m.times
   m.value = m.value * 2
ENDFOR
RETURN m.value

*
* Shift a value x times to the right.  (This isn't a true match for
* a shift since we keep extending the value without truncating it,
* but it works for us.)
*
*!*****************************************************************************
*!
*!       Function: SHIFTR
*!
*!      Called by: STAMPVAL()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION shiftr
PARAMETER m.value, m.times
PRIVATE m.loop

FOR m.loop = 1 TO m.times
   m.value = INT(m.value / 2)
ENDFOR
RETURN m.value

*
* EmptyPlatform - Takes a platform ID and returns .T. if no records for that platform
*       are in the file or .F. if some are present.
*
*!*****************************************************************************
*!
*!       Function: EMPTYPLATFORM
*!
*!      Called by: IMPORT             (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION emptyplatform
PARAMETER m.platform
PRIVATE m.count
SELECT (m.g_scrnalias)

IF (FCOUNT() = c_20scxfld OR FCOUNT() = c_20frxfld OR FCOUNT() = c_20lbxfld)
   RETURN .T.
ENDIF

COUNT TO m.count FOR platform = m.platform
IF m.count > 0
   RETURN .F.
ELSE
   RETURN .T.
ENDIF

**
** Code Associated With Displaying the 2.0 to 2.5 conversion dialog.
**
*!*****************************************************************************
*!
*!       Function: STRUCTDIALOG
*!
*!      Called by: DOUPDATE()         (function  in TRANSPRT.PRG)
*!
*!          Calls: ERRORHANDLER       (procedure in TRANSPRT.PRG)
*!               : CURPOS()           (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION structdialog
PARAMETER m.textline
PRIVATE m.choice, m.ftype, m.dlgwidth, m.fnt_ratio

DO CASE
CASE m.g_filetype = c_screen
   m.ftype = "screen "
CASE m.g_filetype = c_report
   m.ftype = "report "
CASE m.g_filetype = c_label
   m.ftype = "label "
CASE m.g_filetype = c_project
   m.ftype = "project "
OTHERWISE
   m.ftype = ""
ENDCASE

m.dlgwidth = 60    && default
DO CASE
CASE _WINDOWS
	*- no dialog if Windows (conversion dialog should be enough) (jd 2/6/95)
	=UpdTherm(10)
	RETURN .T.	&& RETURN (MESSAGEBOX(m.textline,4,C_MSGBOXTITLE_LOC) == 6)
CASE _mac
	*- no dialog if Mac (conversion dialog should be enough) (jd 03/24/96)
	=UpdTherm(10)
	RETURN .T.
CASE _WINDOWS OR _MAC
   IF NOT WEXIST("tstructd")
		IF _MAC
			m.dlgwidth = 40
      	DEFINE WINDOW tstructd ;
         	AT 0,0 ;
         	SIZE 5.076,m.dlgwidth ;
         	TITLE "Converter" ;
         	FONT m.g_tdlgface, m.g_tdlgsize ;
         	STYLE m.g_tdlgstyle ;
         	FLOAT ;
         	NOCLOSE ;
         	MINIMIZE ;
         	SYSTEM  ;
	         COLOR RGB(0, 0, 0, 192, 192, 192)
		ELSE
			m.dlgwidth = 58.333
		*- added color to Windows screen (jd 11/15/94)
      	DEFINE WINDOW tstructd ;
         	AT 0,0 ;
         	SIZE 5.076,m.dlgwidth ;
         	TITLE "Converter" ;
         	FONT m.g_tdlgface, m.g_tdlgsize ;
         	STYLE m.g_tdlgstyle ;
         	FLOAT ;
         	CLOSE ;
         	MINIMIZE ;
         	SYSTEM  ;
	        COLOR RGB(0, 0, 0, 192, 192, 192)
		ENDIF
      MOVE WINDOW tstructd CENTER
   ENDIF

   IF WVISIBLE("tstructd")
      ACTIVATE WINDOW tstructd SAME
   ELSE
      ACTIVATE WINDOW tstructd NOSHOW
   ENDIF

	* Adjust for differences between dialog window font and text font
	m.fnt_ratio = 	FONTMETRIC(6,m.g_tdlgface, m.g_tdlgsize, m.g_tdlgsty2) ;
                  / FONTMETRIC(6,m.g_tdlgface, m.g_tdlgsize, m.g_tdlgstyle)

   @ 1.000, (m.dlgwidth - TXTWIDTH(m.textline, m.g_tdlgface, m.g_tdlgsize, m.g_tdlgstyle) * m.fnt_ratio) / 2 ;
      SAY m.textline ;
      SIZE 1.154,TXTWIDTH(m.textline, m.g_tdlgface, m.g_tdlgsize, m.g_tdlgstyle) ;
      FONT m.g_tdlgface, m.g_tdlgsize ;
      STYLE m.g_tdlgsty2

   @ 2.750, m.dlgwidth/2 - (13.5*2+4.308)/2 GET m.choice ;
      PICTURE "@*HT3 \!\<Yes;\?\<Cancel" ;
      SIZE m.g_tdlgbtn,13.500,4.308 ;
      DEFAULT 1 ;
      FONT m.g_tdlgface, m.g_tdlgsize ;
      STYLE m.g_tdlgstyle

CASE _DOS OR _UNIX
   IF NOT WEXIST("tstructd")
      DEFINE WINDOW tstructd ;
         FROM INT((SROW()-7)/2),INT((SCOL()-47)/2) ;
         TO INT((SROW()-7)/2)+7,INT((SCOL()-47)/2)+46 ;
         FLOAT ;
         NOCLOSE ;
         SHADOW ;
         DOUBLE ;
         COLOR SCHEME 7
   ENDIF

   IF WVISIBLE("tstructd")
      ACTIVATE WINDOW tstructd SAME
   ELSE
      ACTIVATE WINDOW tstructd NOSHOW
   ENDIF

   * Format the file name for display
   m.msg = "File: "+m.g_scrndbf
   IF LEN(m.msg) > 44
      m.msg = m.g_scrndbf
      IF LEN(m.msg) > 44
         m.msg = justfname(m.g_scrndbf)
      ENDIF
   ENDIF

   @ 1,(WCOLS()-LEN(m.msg))/2 SAY m.msg
   @ 2,(WCOLS()-LEN(m.textline))/2 SAY m.textline
   @ 4,2 GET m.choice ;
      PICTURE "@*HT "+T_YESNO_LOC ;
      SIZE 1,12,18 ;
      DEFAULT 1

OTHERWISE
   DO errorhandler WITH "Unknown Version.", LINENO(), c_error3
   RETURN .F.
ENDCASE

IF NOT WVISIBLE("tstructd")
   ACTIVATE WINDOW tstructd
ENDIF

READ CYCLE MODAL WHEN curpos()

RELEASE WINDOW tstructd

IF m.choice = 1
   RETURN .T.
ELSE
   RETURN .F.
ENDIF
RETURN

*!*****************************************************************************
*!
*!       Function: CURPOS
*!
*!      Called by: STRUCTDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION curpos
IF _DOS OR _UNIX
   _CUROBJ = 2
ENDIF
RETURN .T.

**
** Code Associated With Displaying the Screen Convert Dialog Box
**
*!*****************************************************************************
*!
*!       Function: SCXFRXDIALOG
*!
*!      Called by: CONVERTTYPE()      (function  in TRANSPRT.PRG)
*!
*!          Calls: HASRECORDS()       (function  in TRANSPRT.PRG)
*!               : STRIPPATH()        (function  in TRANSPRT.PRG)
*!               : SCRNCTRL()         (function  in TRANSPRT.PRG)
*!               : TRANSPRMPT()       (function  in TRANSPRT.PRG)
*!               : PVALID()           (function  in TRANSPRT.PRG)
*!               : ASKFONT()          (function  in TRANSPRT.PRG)
*!               : ERRORHANDLER       (procedure in TRANSPRT.PRG)
*!               : RDVALID()          (function  in TRANSPRT.PRG)
*!               : DEACCLAU()         (function  in TRANSPRT.PRG)
*!               : SHOWCLAU()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION scxfrxdialog
PARAMETER ftype
PRIVATE m.choice, m.fromplatform, m.dlgnum
m.choice = 0
m.gNShowMe = 1

DO CASE
CASE _WINDOWS
   IF m.ftype <> "LBX" AND hasrecords(c_winname)
      * No partial transport of labels

      m.fromplatform = dfltplat()
      m.dlgnum = 1
      m.g_allobjects = .F.

      * already contains some records for Windows
      DEFINE WINDOW transdlg ;
         AT  0.000, 0.000  ;
         SIZE 22.385,76.167 ;
         TITLE T_TITLE_LOC  ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1;
         FLOAT ;
         NOCLOSE ;
         NOMINIMIZE ;
         DOUBLE ;
         COLOR RGB(0,0,0,192,192,192)
      MOVE WINDOW transdlg CENTER

      IF WVISIBLE("transdlg")
         ACTIVATE WINDOW transdlg SAME
      ELSE
         ACTIVATE WINDOW transdlg NOSHOW
      ENDIF

      @ 14.077,1.667 TO 21.385,50.167 ;
         PEN 1, 8 ;
         STYLE "T"
      @ 13.615,2.667 SAY T_TRANSPORT_LOC  ;
         SIZE 1.000, 9.167, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 1.000,2.667 SAY IIF(m.ftype = "SCX",T_SCREEN_FILE_LOC,T_REPORT_FILE_LOC) ;
         SIZE 1.000,13.500, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle
      @ 1.000,16.667 SAY LOWER(strippath(m.cRealName)) ;
         SIZE 1.000,21.833 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 3.077,2.667 SAY T_OTHERPLAT_LOC+versioncap(m.g_toplatform)+"." ;
         SIZE 2.000,35.000, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 8.077,2.667 SAY T_BYTRANS3_LOC+versioncap(m.g_toplatform)+T_BYTRANS4_LOC ;
         SIZE 2.000,48.167, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 11.385,2.667 SAY T_TRANSOBJ_LOC +" " ;
         SIZE 1.000,23.500 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 5.615,2.667 SAY  T_NEWMOD1_LOC+versioncap(m.g_toplatform)+T_NEWMOD2_LOC+versioncap(m.g_toplatform)+T_NEWMOD3_LOC ;
         SIZE 2.000,47.833 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 17.846,7.500 SAY T_THAN_LOC	+versioncap(m.g_toplatform)+T_EQIVOBJS_LOC ;
         SIZE 1.000,32.667 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      m.thepict = "@^ "+makepict(c_dosnum,c_macnum,c_unixnum, @m.fromplatform)
      @ 11.231,25.833 GET m.fromplatform ;
         PICTURE m.thepict ;
         SIZE 1.538,24.333 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 14.923,4.500 GET m.g_newobjects ;
         PICTURE "@*C "+T_OBJSNEWTO_LOC+versioncap(m.g_toplatform) ;
         SIZE 1.308,28.167 ;
         DEFAULT .T. ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID scrnctrl() ;
		 COLOR ,,,,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192),,,RGB(0,0,0,192,192,192),RGB(128,128,128,192,192,192)
      @ 16.538,4.500 GET m.g_snippets ;
         PICTURE "@*C"+T_RECMOD_LOC ;
         SIZE 1.308,34.667 ;
         DEFAULT .T. ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID scrnctrl() ;
		 COLOR ,,,,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192),,,RGB(0,0,0,192,192,192),RGB(128,128,128,192,192,192)
      @ 19.385,4.500 GET m.g_allobjects ;
         PICTURE "@*C "+T_REPLOBJ_LOC ;
         SIZE 1.308,43.833 ;
         DEFAULT .F. ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID scrnctrl() ;
		 COLOR ,,,,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192),,,RGB(0,0,0,192,192,192),RGB(128,128,128,192,192,192)
      @ 0.615,51.667 GET m.choice ;
         PICTURE "@*VNT "+transprmpt()+";"+T_CONVASIS_LOC ;
         SIZE 1.769,23.000,0.308 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID pvalid()
      @ 13.077,51.667 GET m.g_askfont ;
         PICTURE "@*VN "+T_FONT1_LOC ;
         SIZE 1.769,23.000,0.308 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID askfont()

	   *- stop from showing this dialog? (2/28/95 jd)
	   IF m.lPJX
		   @ 16.385,51.667 GET m.gNShowMe ;
				PICTURE "@*RV " + C_ASK1_LOC + SUBS("PJXSCXFRX",((m.g_tpFileIndx - 1) * 3) + 1,3) + C_ASK2_LOC;
				DEFAULT 1 ;
				FONT m.g_tdlgface, m.g_tdlgsize ;
				STYLE m.g_tdlgsty1 ;
				COLOR ,,,,,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192),,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192)
		ENDIF

   ELSE    && no existing WINDOWS records
      m.fromplatform = dfltplat()
      m.dlgnum = 2
      DEFINE WINDOW transdlg ;
         AT 0.000, 0.000 ;
         SIZE 15.077,66.167 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         TITLE T_TITLE_LOC ;
         FLOAT ;
         NOCLOSE ;
         NOMINIMIZE ;
         DOUBLE ;
         COLOR RGB(0,0,0,192,192,192)
      MOVE WINDOW transdlg CENTER

      IF WVISIBLE("transdlg")
         ACTIVATE WINDOW transdlg SAME
      ELSE
         ACTIVATE WINDOW transdlg NOSHOW
      ENDIF

      @ 1.000,2.667 SAY IIF(m.ftype = "SCX",T_SCREEN_FILE_LOC ,;
         IIF(m.ftype = "FRX",T_REPORT_FILE_LOC,T_LABEL_FILE_LOC)) ;
         SIZE 1.000,11.500, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle
      @ 1.000,14.667 SAY LOWER(strippath(m.cRealName)) ;
         SIZE 1.000,21.833 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 3.077,2.667 SAY T_OTHERPLAT_LOC+versioncap(m.g_toplatform)+"." ;
         SIZE 2.000,35.000, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle
      @ 5.923,2.667 SAY T_BYTRANS1_LOC + CHR(13) + ;
         versioncap(m.g_toplatform)+T_BYTRANS2_LOC  + ;
         T_NOCONV_LOC  ;
         SIZE 4.000,36.833, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle
      @ 10.923,2.667 SAY T_TRANSOBJ_LOC +" " ;
         SIZE 1.000,23.500, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      m.thepict = "@^ "+makepict(c_dosnum,c_macnum,c_unixnum, @m.fromplatform)
      @ 12.154,2.667 GET m.fromplatform ;
         PICTURE m.thepict ;
         SIZE 1.538,24.333 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 6.846,40.833 GET m.g_askfont ;
         PICTURE "@*VN "+T_FONT1_LOC  ;
         SIZE 1.769,23.000,0.308 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID askfont()
      @ 0.615,40.833 GET m.choice ;
         PICTURE "@*VNT "+transprmpt()+";"+T_NOTRANSPORT_LOC ;
         SIZE 1.769,23.000,0.308 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID pvalid()

		IF m.lPJX
		   *- stop from showing this dialog?
		   @ 11.154,40.833 GET m.gNShowMe ;
				PICTURE "@*RV " + C_ASK1_LOC + SUBS("PJXSCXFRX",((m.g_tpFileIndx - 1) * 3) + 1,3) + C_ASK2_LOC;
				DEFAULT 1 ;
				FONT m.g_tdlgface, m.g_tdlgsize ;
				STYLE m.g_tdlgsty1 ;
				COLOR ,,,,,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192),,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192)
		ENDIF

   ENDIF

CASE  _MAC
   LOCAL iFormWidth

   iFormWidth = IIF(m.lPJX,68.500, 58)
   iFormHeight = IIF(m.lPJX,21.600, 13.077)

   IF m.ftype <> "LBX" AND hasrecords(c_macname)
      * No partial transport of labels

      m.fromplatform = dfltplat()
      m.dlgnum = 1
      m.g_allobjects = .F.

      * already contains some Mac records
      DEFINE WINDOW transdlg ;
         AT  0.000, 0.000  ;
         SIZE iFormHeight,iFormWidth ;
         TITLE T_TITLE_LOC  ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1;
         FLOAT ;
         CLOSE ;
         NOMINIMIZE ;
         DOUBLE ;
         COLOR RGB(0, 0, 0,192,192,192)
      MOVE WINDOW transdlg CENTER

      IF WVISIBLE("transdlg")
         ACTIVATE WINDOW transdlg SAME
      ELSE
         ACTIVATE WINDOW transdlg NOSHOW
      ENDIF

      @ 12.077,1.667 TO 19.385,46.0 ;
         PEN 1, 8 ;
         STYLE "T"
      @ 1.000,2.667 SAY IIF(m.ftype = "SCX",T_SCREEN_FILE_LOC ,T_REPORT_FILE_LOC) ;
         SIZE 1.000,13.500, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle
      @ 1.000,16.667 SAY LOWER(strippath(m.cRealName)) ;
         SIZE 1.000,21.833 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 0.615,49.000 GET m.choice ;
         PICTURE "@*VNTM "+transprmpt()+";"+T_OPENASIS_LOC ;
         SIZE m.g_tdlgbtn,12.000,0.500 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID pvalid()
      @ 12.077,49 GET m.g_askfont ;
         PICTURE "@*VNM "+T_FONT1_LOC  ;
         SIZE m.g_tdlgbtn,12.000,0.308 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID askfont()
		IF m.ftype = "SCX"
			@ 15.000, 48 GET m.g_look2d ;
		   	PICTURE "@*C3 " + C_2DCONTROLS_LOC ;
				DEFAULT 0 ;
         	FONT m.g_tdlgface, m.g_tdlgsize ;
         	STYLE m.g_tdlgstyle ;
				VALID setctrl()
	   ENDIF
      @ 3.077,2.667 SAY T_OTHERPLAT_LOC+versioncap(m.g_toplatform)+"." ;
         SIZE 2.000,50.000, 0.000 ;
         FONT m.g_smface, m.g_smsize ;
         STYLE m.g_smsty1
      @ 5.615,2.667 SAY T_NEWMOD1_LOC+versioncap(m.g_toplatform)+T_NEWMOD2_LOC+versioncap(m.g_toplatform)+T_NEWMOD3_LOC ;
         SIZE 2.000,60.000 ;
         FONT m.g_smface, m.g_smsize ;
         STYLE m.g_smsty1
      @ 8.077,2.667 SAY T_BYTRANS3_LOC +versioncap(m.g_toplatform)+T_BYTRANS4_LOC ;
         SIZE 2.000,60.000, 0.000 ;
         FONT m.g_smface, m.g_smsize ;
         STYLE m.g_smsty1
      @ 10.385,2.667 SAY T_TRANSOBJ_LOC +" " ;
         SIZE 1.000,28.000 ;
         FONT m.g_smface, m.g_smsize ;
         STYLE m.g_smsty1
      m.thepict = "@^3 "+makepict(c_winnum, c_dosnum, c_unixnum, @m.fromplatform)
      @ 10.231,21.833 GET m.fromplatform ;
         PICTURE m.thepict ;
         SIZE 1.538,24.333 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 11.615,2.667 SAY T_TRANSPORT_LOC  ;
         SIZE 1.000, 9.167, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 12.923,4.500 GET m.g_newobjects ;
         PICTURE "@*C3 "+T_OBJSNEWTO_LOC+versioncap(m.g_toplatform) ;
         SIZE 1.308,28.167 ;
         DEFAULT .T. ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle ;
         VALID scrnctrl()
      @ 14.538,4.500 GET m.g_snippets ;
         PICTURE "@*C3"+T_RECMOD_LOC ;
         SIZE 1.308,34.667 ;
         DEFAULT .T. ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle ;
         VALID scrnctrl()
      @ 15.846,7.500 SAY T_THAN_LOC+versioncap(m.g_toplatform)+T_EQIVOBJS_LOC ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         SIZE 1.000,30.000 ;
         STYLE m.g_tdlgstyle  && 

      @ 17.385,4.500 GET m.g_allobjects ;
         PICTURE "@*C3 "+T_REPLOBJ_LOC ;
         SIZE 1.308,43.833 ;
         DEFAULT .F. ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle ;
         VALID scrnctrl()

	   *- stop from showing this dialog? (11/1/95 jd)
	   IF m.lPJX
		   @ 16.385,47 GET m.gNShowMe ;
				PICTURE "@*RV " + C_ASK1_LOC + SUBS("PJXSCXFRX",((m.g_tpFileIndx - 1) * 3) + 1,3) + C_ASK2_LOC;
				DEFAULT 1 ;
				FONT m.g_tdlgface, m.g_tdlgsize ;
				STYLE m.g_tdlgsty1 ;
				COLOR ,,,,,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192),,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192)
		ENDIF

   ELSE    && no existing MAC records

      m.fromplatform = dfltplat()
      m.dlgnum = 2
      DEFINE WINDOW transdlg ;
         AT 0.000, 0.000 ;
         SIZE iFormHeight,iFormWidth ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         TITLE T_TITLE_LOC  ;
         FLOAT ;
         CLOSE ;
         NOMINIMIZE ;
         DOUBLE ;
         COLOR RGB(0, 0, 0, 192, 192, 192)
      MOVE WINDOW transdlg CENTER

      IF WVISIBLE("transdlg")
         ACTIVATE WINDOW transdlg SAME
      ELSE
         ACTIVATE WINDOW transdlg NOSHOW
      ENDIF

      @ 1.000,2.667 SAY IIF(m.ftype = "SCX",T_SCREEN_FILE_LOC ,;
         IIF(m.ftype = "FRX",T_REPORT_FILE_LOC,T_LABEL_FILE_LOC)) ;
         SIZE 1.000,11.500, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle
      @ 1.000,14.667 SAY LOWER(strippath(m.cRealName)) ;
         SIZE 1.000,22.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
      @ 3.077,2.667 SAY T_OTHERPLAT_LOC+versioncap(m.g_toplatform)+"." ;
         SIZE 2,45,0 ;
         FONT m.g_smface, m.g_smsize ;
         STYLE m.g_smstyle
      @ 5.923,2.667 SAY T_BYTRANS1_LOC + CHR(13)  ;
         + versioncap(m.g_toplatform)+T_BYTRANS2_LOC ;
         SIZE 2,45,0 ;
         FONT m.g_smface, m.g_smsize ;
         STYLE m.g_smstyle
      @ 8.923,2.667 SAY T_TRANSOBJ_LOC  ;
         SIZE 1.000, 28.000, 0.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty2
      @ 0.615,42.833 GET m.choice ;
         PICTURE "@*VNTM "+transprmpt()+";\?"+T_CANCEL_LOC;
         SIZE m.g_tdlgbtn,12.000,1.000 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID pvalid()
      @ 6.846,42.833 GET m.g_askfont ;
         PICTURE "@*VNM "+T_FONT1_LOC  ;
         SIZE m.g_tdlgbtn,12.000,0.308 ;
         DEFAULT 1 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1 ;
         VALID askfont()
		IF m.ftype = "SCX"
			@ 8.800, 40.833 GET m.g_look2d ;
		   	PICTURE "@*C3 " + C_2DCONTROLS_LOC;
				DEFAULT 0 ;
         	FONT m.g_tdlgface, m.g_tdlgsize ;
         	STYLE m.g_tdlgstyle ;
				VALID setctrl()
		ENDIF
      m.thepict = "@^3 "+makepict(c_winnum, c_dosnum, c_unixnum, @m.fromplatform)
      @ 10.154,2.667 GET m.fromplatform ;
         PICTURE m.thepict ;
         SIZE 1.538,24.333 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1

		IF m.lPJX
		   *- stop from showing this dialog?
		   @ 11.154,40.833 GET m.gNShowMe ;
				PICTURE "@*RV " + C_ASK1_LOC + SUBS("PJXSCXFRX",((m.g_tpFileIndx - 1) * 3) + 1,3) + C_ASK2_LOC;
				DEFAULT 1 ;
				FONT m.g_tdlgface, m.g_tdlgsize ;
				STYLE m.g_tdlgsty1 ;
				COLOR ,,,,,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192),,RGB(0,0,0,192,192,192),RGB(0,0,0,192,192,192)
		ENDIF
   ENDIF
CASE _DOS OR _UNIX
   m.fromplatform = c_foxwin_loc
   IF m.ftype <> "LBX" AND (hasrecords(c_dosname) OR hasrecords(c_unixname))
      m.dlgnum = 1
      m.g_allobjects = .F.

      DEFINE WINDOW transdlg ;
         FROM INT((SROW()-21)/2),INT((SCOL()-67)/2) ;
         TO INT((SROW()-21)/2)+20,INT((SCOL()-67)/2)+66 ;
         FLOAT ;
         CLOSE ;
         SHADOW ;
         NOMINIMIZE ;
         DOUBLE ;
         COLOR SCHEME 5

      IF WVISIBLE("transdlg")
         ACTIVATE WINDOW transdlg SAME
      ELSE
         ACTIVATE WINDOW transdlg NOSHOW
      ENDIF

      @ 11,2 TO 16,52
      @ 1,2 SAY IIF(m.g_filetype = c_screen,T_SCREEN_FILE_LOC ,T_REPORT_FILE_LOC) ;
         SIZE 1,12, 0
      @ 1,15 SAY UPPER(strippath(m.cRealName)) ;
         SIZE 1,19
      @ 3,2 SAY T_OTHERPLAT2_LOC ;
         SIZE 1,38, 0
      @ 4,2 SAY T_OTHERPLAT3_LOC ;
         SIZE 1,33, 0
      @ 9,4 SAY T_TRANSOBJ_LOC  ;
         SIZE 1,23, 0
      m.thepict = "@^ "+makepict(c_winnum, c_macnum, c_unixnum, @m.fromplatform)
      @ 8,29 GET m.fromplatform ;
         PICTURE m.thepict ;
         SIZE 3,24 ;
         COLOR SCHEME 5, 6
      @ 1,45 GET m.choice ;
         PICTURE "@*VNT "+T_TRANSOPEN_LOC  ;
         SIZE 1,20,1 ;
         DEFAULT 1 ;
         VALID pvalid()
      @ 11,4 SAY T_TRANSPORT_LOC  ;
         SIZE 1,9, 0
      @ 12,4 GET m.g_newobjects ;
         PICTURE "@*C "+T_OBJSNEWTO_LOC+versioncap(m.g_toplatform) ;
         SIZE 1,25 ;
         DEFAULT .T. ;
         VALID scrnctrl()
      @ 13,4 GET m.g_snippets ;
         PICTURE "@*C"+T_RECMOD_LOC ;
         SIZE 1,34 ;
         DEFAULT .T. ;
         VALID scrnctrl()
      @ 14,8 SAY T_THAN_LOC + versioncap(m.g_toplatform) + T_EQIVOBJS_LOC;
         SIZE 1,30, 0
      @ 15,4 GET m.g_allobjects ;
         PICTURE "@*C "+T_REPLOBJ_LOC ;
         SIZE 1,47 ;
         DEFAULT .F. ;
         VALID scrnctrl()
      @ 7,2 SAY T_OBJINFILE_LOC  ;
         SIZE 1,24, 0
      @ 5,2 SAY T_BYTRANS5_LOC   ;
         SIZE 1,35, 0
      @ 6,2 SAY T_BYTRANS6_LOC ;
         SIZE 1,37, 0

      IF NOT WVISIBLE("transdlg")
         ACTIVATE WINDOW transdlg
      ENDIF
   ELSE
      m.dlgnum = 2

      DEFINE WINDOW transdlg ;
         FROM INT((SROW()-15)/2),INT((SCOL()-68)/2) ;
         TO INT((SROW()-15)/2)+14,INT((SCOL()-68)/2)+67 ;
         FLOAT ;
         NOCLOSE ;
         SHADOW ;
         NOMINIMIZE ;
         DOUBLE ;
         COLOR SCHEME 5

      IF WVISIBLE("transdlg")
         ACTIVATE WINDOW transdlg SAME
      ELSE
         ACTIVATE WINDOW transdlg NOSHOW
      ENDIF

      @ 1,2 SAY IIF(m.g_filetype = c_screen,T_SCREEN_FILE_LOC ,T_REPORT_FILE_LOC) ;
         SIZE 1,12, 0
      @ 1,15 SAY UPPER(strippath(m.cRealName)) ;
         SIZE 1,19
      @ 3,2 SAY T_OTHERPLAT2_LOC ;
         SIZE 1,38, 0
      @ 4,2 SAY T_OTHERPLAT3_LOC ;
         SIZE 1,33, 0
      @ 8,4 SAY T_TRANSOBJ_LOC  ;
         SIZE 1,23, 0
      m.thepict = "@^ "+makepict(c_winnum, c_macnum, c_unixnum, @m.fromplatform)
      @ 9,4 GET m.fromplatform ;
         PICTURE m.thepict ;
         SIZE 3,24 ;
         COLOR SCHEME 5, 6
      @ 1,45 GET m.choice ;
         PICTURE "@*VNT "+T_TRANSOPEN1_LOC ;
         SIZE 1,20,1 ;
         DEFAULT 1 ;
         VALID pvalid()
      @ 5,2 SAY T_BYTRANS1_LOC ;
         SIZE 1,37, 0
      @ 6,2 SAY "MS-DOS" + T_BYTRANS2_LOC ;
         SIZE 1,37, 0

      IF NOT WVISIBLE("transdlg")
         ACTIVATE WINDOW transdlg
      ENDIF
   ENDIF
OTHERWISE
   DO errorhandler WITH T_UNKNOWNVERS_LOC, LINENO(), c_error3
   RETURN .F.
ENDCASE

* The effect of this code is to skip the read entirely if gAShowMe[filetype,1] is
* FALSE. All of the variables in this dialog are set to their default
* values, the dialog isn't displayed, the warning about overwriting
* existing records isn't displayed, and processing continues.
IF m.gAShowMe[m.g_tpFileIndx,1]
   IF NOT WVISIBLE("transdlg")
      ACTIVATE WINDOW transdlg
   ENDIF
	*- this do loop is here to work around bug in VFP Mac
	DO WHILE m.choice == 0
	   READ CYCLE MODAL ;
	      VALID rdvalid(m.dlgnum) ;
	      DEACTIVATE deacclau() ;
	      SHOW showclau() ;
	      SAVE						&& BUGBUG remove SAVE option after bug is fixed!
	ENDDO
ELSE
   CLEAR GETS
   m.choice = m.gAShowMe[m.g_tpFileIndx,2]    && pretend user said whatever they said before
   IF !EMPTY(m.gAShowMe[m.g_tpFileIndx,3])
       *- a font has been specified
	   m.g_fontset = .T.
	   m.g_dfltfface   =  m.gAShowMe[m.g_tpFileIndx,3]
	   m.g_dfltfsize   =  m.gAShowMe[m.g_tpFileIndx,4]
	   m.g_dfltfstyle  =  m.gAShowMe[m.g_tpFileIndx,5]
   ENDIF
   IF !EMPTY(m.gAShowMe[m.g_tpFileIndx,6])
	   m.fromplatform = m.gAShowMe[m.g_tpFileIndx,6]
   ENDIF
	m.g_newobjects = gAShowMe[m.g_tpFileIndx,7]								&& convert new objects
	m.g_snippets = gAShowMe[m.g_tpFileIndx,8]								&& convert more recently modified objects
	m.g_allobjects  = gAShowMe[m.g_tpFileIndx,9]							&& replace all objects -- changed from [IIF(!g_allobjects,.F.,gAShowMe[m.g_tpFileIndx,9])] bug? (jd 04/16/96)
	=pvalid()																&& make sure this gets executed
ENDIF

RELEASE WINDOW transdlg

*
* We could simply return m.choice, but this way we can mess with the dialog without changing
* the defines.
*
IF gAShowMe[m.g_tpFileIndx,1]
	DO CASE
	CASE m.choice = 1
	   *- handle radio button choice
	   DO CASE
		CASE m.gNShowMe = 1
			*- continue to ask
			gAShowMe[m.g_tpFileIndx,1] = .T.
			gAShowMe[m.g_tpFileIndx,2] = m.choice
		CASE m.gNShowMe = 2
			*- don't ask for this file type
			gAShowMe[m.g_tpFileIndx,1] = .F.
			gAShowMe[m.g_tpFileIndx,2] = m.choice
			gAShowMe[m.g_tpFileIndx,6] = m.fromplatform
			gAShowMe[m.g_tpFileIndx,7] = m.g_newobjects					&& convert new objects
			gAShowMe[m.g_tpFileIndx,8] = m.g_snippets					&& convert more recently modified objects
			gAShowMe[m.g_tpFileIndx,9] = m.g_allobjects					&& replace all objects
		CASE m.gNShowMe = 3
			*- don't ask for any file type
			LOCAL ictr
			FOR ictr = 1 TO ALEN(gAShowMe,1)
				gAShowMe[ictr,1] = .F.
				gAShowMe[ictr,2] = m.choice
				gAShowMe[ictr,6] = m.fromplatform				&& changed from [gAShowMe[m.g_tpFileIndx,6] = m.fromplatform] (looked like bug 04/16/96 jd)
				gAShowMe[ictr,7] = m.g_newobjects				&& convert new objects
				gAShowMe[ictr,8] = m.g_snippets					&& convert more recently modified objects
				gAShowMe[ictr,9] = m.g_allobjects				&& replace all objects
			NEXT
	   ENDCASE
	   RETURN c_yes
	CASE m.choice = 2 AND m.dlgnum = 1
	   DO CASE
		CASE m.gNShowMe = 1
			*- continue to ask
			gAShowMe[m.g_tpFileIndx,1] = .T.
			gAShowMe[m.g_tpFileIndx,2] = m.choice
		CASE m.gNShowMe = 2
			*- don't ask for this file type
			gAShowMe[m.g_tpFileIndx,1] = .F.
			gAShowMe[m.g_tpFileIndx,2] = m.choice
			gAShowMe[m.g_tpFileIndx,6] = m.fromplatform
			gAShowMe[m.g_tpFileIndx,7] = m.g_newobjects					&& convert new objects
			gAShowMe[m.g_tpFileIndx,8] = m.g_snippets					&& convert more recently modified objects
			gAShowMe[m.g_tpFileIndx,9] = m.g_allobjects					&& replace all objects
		CASE m.gNShowMe = 3
			*- don't ask for any file type
			LOCAL ictr
			FOR ictr = 1 TO ALEN(gAShowMe,1)
				gAShowMe[ictr,1] = .F.
				gAShowMe[ictr,2] = m.choice
				gAShowMe[ictr,6] = m.fromplatform
				gAShowMe[ictr,7] = m.g_newobjects					&& convert new objects
				gAShowMe[ictr,8] = m.g_snippets						&& convert more recently modified objects
				gAShowMe[ictr,9] = m.g_allobjects					&& replace all objects
			NEXT
	   ENDCASE
	   RETURN c_no
	OTHERWISE
	   DO CASE
		CASE m.gNShowMe = 1
			*- continue to ask
			gAShowMe[m.g_tpFileIndx,1] = .T.
			gAShowMe[m.g_tpFileIndx,2] = 2
		CASE m.gNShowMe = 2
			*- don't ask for this file type
			gAShowMe[m.g_tpFileIndx,1] = .F.
			gAShowMe[m.g_tpFileIndx,2] = 2
			gAShowMe[m.g_tpFileIndx,7] = m.g_newobjects					&& convert new objects
			gAShowMe[m.g_tpFileIndx,8] = m.g_snippets					&& convert more recently modified objects
			gAShowMe[m.g_tpFileIndx,9] = m.g_allobjects					&& replace all objects
		CASE m.gNShowMe = 3
			*- don't ask for any file type
			LOCAL ictr
			FOR ictr = 1 TO ALEN(gAShowMe,1)
				gAShowMe[ictr,1] = .F.
				gAShowMe[ictr,2] = 2
				gAShowMe[ictr,7] = m.g_newobjects					&& convert new objects
				gAShowMe[ictr,8] = m.g_snippets					&& convert more recently modified objects
				gAShowMe[ictr,9] = m.g_allobjects					&& replace all objects
			NEXT
	   ENDCASE
	   RETURN c_cancel
	ENDCASE
ELSE
	DO CASE
		CASE gAShowMe[g_tpFileIndx,2] = 1
			RETURN c_yes
		CASE gAShowMe[g_tpFileIndx,2] = 2 AND m.dlgnum = 1
			RETURN c_no
		OTHERWISE
			RETURN c_cancel
	ENDCASE
ENDIF
RETURN

*!*****************************************************************************
*!
*!       Function: dfltplat
*!
*!*****************************************************************************
FUNCTION dfltplat
* Return the default platform to transport from
PRIVATE m.plat
DO CASE
CASE hasrecords(c_winname) AND !_WINDOWS
   m.plat =   c_foxwin_loc
CASE hasrecords(c_macname) AND !_MAC
   m.plat =   c_foxmac_loc
CASE hasrecords(c_dosname) AND !_DOS
   m.plat =   c_foxdos_loc
OTHERWISE
   m.plat =   c_foxwin_loc
ENDCASE
RETURN m.plat

*!*****************************************************************************
*!
*!       Function: MAKEPICT
*!
*!*****************************************************************************
FUNCTION makepict
* Assemble picture clause for "from" platform popup.  This routine creates
* the popup entries and enables or disables them based on whether the
* candidate platform has any records in the screen/report file.
PARAMETER a,b,c, dfltitem
PRIVATE m.i, m.pictstrg
DECLARE a_plats[3]
a_plats[1] = m.a
a_plats[2] = m.b
a_plats[3] = m.c
m.pictstrg = ""

m.looptop = 3
m.found_dflt = .F.

FOR m.i = 1 TO m.looptop
   DO CASE
   CASE a_plats[m.i] = c_dosnum
		DO CASE
		CASE !hasrecords(c_dosname)
      	m.pictstrg = m.pictstrg + "\"
		CASE !m.found_dflt
			m.dfltitem = c_foxdos_loc
			m.found_dflt = .T.
		ENDCASE
     	m.pictstrg = m.pictstrg + c_foxdos_loc
   CASE a_plats[m.i] = c_winnum
		DO CASE
		CASE !hasrecords(c_winname)
      	m.pictstrg = m.pictstrg + "\"
		CASE !m.found_dflt
			m.dfltitem = c_foxwin_loc
			m.found_dflt = .T.
		ENDCASE
     	m.pictstrg = m.pictstrg + c_foxwin_loc

   CASE a_plats[m.i] = c_macnum
		DO CASE
		CASE !hasrecords(c_macname)
      	m.pictstrg = m.pictstrg + "\"
		CASE !m.found_dflt
			m.dfltitem = c_foxmac_loc
			m.found_dflt = .T.
		ENDCASE
     	m.pictstrg = m.pictstrg + c_foxmac_loc
   ENDCASE
   m.pictstrg = m.pictstrg + iif(m.i < m.looptop,";","")
ENDFOR
RETURN m.pictstrg

*
* TRANSPRMPT - Determine the prompt for the transport button
*
*!*****************************************************************************
*!
*!       Function: TRANSPRMPT
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION transprmpt
HOUR = LEFT(TIME(),2)
DO CASE
CASE _MAC
   RETURN "\!"+T_TRANSPORT_LOC 
CASE (DOW(DATE()) = 7 AND HOUR >= "23" AND HOUR < "24") OR ATC(T_ENERGIZE_LOC,GETENV("TRANSPRT")) > 0
   * Debts must be paid
   g_energize = .T.
   RETURN T_ENERGIZE_LOC       && Beam me up
OTHERWISE
   RETURN "\!"+T_TRANSPORT_LOC
ENDCASE

*
* RDVALID() - Prompts for overwriting all objects if g_allobjects is true
*
*!*****************************************************************************
*!
*!       Function: RDVALID
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!          Calls: VERSIONCAP()       (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION rdvalid
PARAMETER dlgnum
IF m.gAShowMe[m.g_tpFileIndx,1] AND m.g_allobjects AND m.dlgnum = 1 AND m.choice = 1
	IF MESSAGEBOX(C_OVERWRITE1_LOC + versioncap(m.g_toplatform) + C_OVERWRITE2_LOC,MB_OKCANCEL) = IDCANCEL
		RETURN .F.
	ELSE
		RETURN .T.
	ENDIF
ENDIF

*
* DEACCLAU - Deactivate clause code.  Clear current read if window closes.
*
*!*****************************************************************************
*!
*!       Function: DEACCLAU
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION deacclau
CLEAR READ
RETURN .T.

*
* SHOWCLAU - Refresh GETS
*
*!*****************************************************************************
*!
*!       Function: SHOWCLAU
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION showclau
IF m.dlgnum = 2
   RETURN
ENDIF

IF g_snippets=.T. OR g_newobjects = .T.
   SHOW GET g_allobjects DISABLE
ELSE
   SHOW GET g_allobjects ENABLE
ENDIF

m.thestring = T_THAN_LOC+versioncap(m.g_toplatform)+T_EQIVOBJS_LOC
IF g_allobjects
   SHOW GET g_snippets   DISABLE
   SHOW GET g_newobjects DISABLE
   DO CASE
*   CASE _WINDOWS AND RGBSCHEME(1,10) <> "RGB(128,128,128,192,192,192)"
*      @ 17.846,7.500 SAY m.thestring ;
*         COLOR (RGBSCHEME(1,10))
   CASE _WINDOWS && AND RGBSCHEME(1,10) == "RGB(0,0,0,255,255,255)"
      @ 17.846,7.500 SAY m.thestring ;
         COLOR RGB(128,128,128,192,192,192)
   CASE  _MAC AND RGBSCHEME(1,10) <> "RGB(0,0,0,255,255,255)"
      @ 15.846,7.500 SAY m.thestring ;
         SIZE 1.000,30.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle ;
		   COLOR (RGBSCHEME(1,10))
   CASE  _MAC AND RGBSCHEME(1,10) == "RGB(0,0,0,255,255,255)"
      @ 15.846,7.500 SAY m.thestring ;
         SIZE 1.000,30.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle	;
         COLOR RGB(192,192,192,255,255,255)
   OTHERWISE
      @ 14,8 SAY m.thestring ;
         COLOR (SCHEME(5,10))
   ENDCASE
ELSE
   SHOW GET g_snippets   ENABLE
   SHOW GET g_newobjects ENABLE
   DO CASE
   CASE _WINDOWS
      @ 17.846,7.500 SAY m.thestring
   CASE _MAC
      @ 15.846,7.500 SAY m.thestring ;
         SIZE 1.000,33.000 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgsty1
   OTHERWISE
      @ 14,8 SAY m.thestring
   ENDCASE
ENDIF

IF !g_allobjects AND g_snippets = .F. AND g_newobjects = .F.
   SHOW GET m.choice,1 DISABLE
ELSE
   SHOW GET m.choice,1 ENABLE
ENDIF

*
* SCRNCTRL - Called for check box validation from the first dialog
*
*!*****************************************************************************
*!
*!       Function: SCRNCTRL
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION scrnctrl
SHOW GETS OFF
RETURN .T.

*
* Makes sure the proper options are enabled based on the setting of m.g_allobjects
*
*!*****************************************************************************
*!
*!       Function: ENABLEPROC
*!
*!*****************************************************************************
FUNCTION enableproc
IF m.g_allobjects
   SHOW GET m.g_newobjects DISABLE
   SHOW GET m.g_snippets DISABLE
ELSE
   SHOW GET m.g_newobjects ENABLE
   SHOW GET m.g_snippets ENABLE
ENDIF

*
* Fills the m.g_fromplatform global variable when the user leaves the dialog.
*
*!*****************************************************************************
*!
*!       Function: PVALID
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION pvalid
DO CASE
CASE ATC('DOS',m.fromplatform) > 0
   m.g_fromplatform = 'DOS'
CASE ATC('WINDOWS',m.fromplatform) > 0
   m.g_fromplatform = 'WINDOWS'
CASE ATC('MAC',m.fromplatform) > 0
   m.g_fromplatform = 'MAC'
CASE ATC('UNIX',m.fromplatform) > 0
   m.g_fromplatform = 'UNIX'
ENDCASE

**
** Code Associated With Displaying of the Thermometer
**

*!*****************************************************************************
*!
*!      Procedure: STARTTHERM
*!
*!      Called by: TRANSPRT.PRG
*!               : GRAPHICTOCHAR      (procedure in TRANSPRT.PRG)
*!               : CHARTOGRAPHIC      (procedure in TRANSPRT.PRG)
*!
*!          Calls: ACTTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE starttherm
PARAMETER VERB,filetype
*  Start the thermometer with the appropriate message.
DO CASE
CASE m.filetype = c_screen
   DO acttherm WITH VERB+T_THERMSCR_LOC
CASE m.filetype = c_report
   DO acttherm WITH VERB+T_THERMRPT_LOC
CASE m.filetype  = c_label
   DO acttherm WITH VERB+T_THERMLBL_LOC
ENDCASE


*!*****************************************************************************
*!
*!      Procedure: THERMFNAME
*!
*!*****************************************************************************
FUNCTION thermfname
PARAMETER m.fname
PRIVATE m.addelipse, m.g_pathsep, m.g_thermfface, m.g_thermfsize, m.g_thermfstyle

IF _MAC
	m.g_thermfface = "Geneva"
	m.g_thermfsize = 10
	m.g_thermfstyle = ""
ELSE
	m.g_thermfface = "MS Sans Serif"
	m.g_thermfsize = 8
	m.g_thermfstyle = "B"
ENDIF

* Translate the filename into Mac native format
IF _MAC
	m.g_pathsep = ":"
	m.fname = SYS(2027, m.fname)
ELSE
    m.g_pathsep = "\"
ENDIF

IF TXTWIDTH(m.fname,m.g_thermfface,m.g_thermfsize,m.g_thermfstyle) > c_space
	* Make it fit in c_space
	m.fname = partialfname(m.fname, c_space - 1)

	m.addelipse = .F.
	DO WHILE TXTWIDTH(m.fname+'...',m.g_thermfface,m.g_thermfsize,m.g_thermfstyle) > c_space
		m.fname = LEFT(m.fname, LEN(m.fname) - 1)
		m.addelipse = .T.
	ENDDO
	IF m.addelipse
		m.fname = m.fname + "..."
   ENDIF
ENDIF
RETURN m.fname



*!*****************************************************************************
*!
*!      Procedure: PARTIALFNAME
*!
*!*****************************************************************************
FUNCTION partialfname
PARAMETER m.filname, m.fillen
* Return a filname no longer than m.fillen characters.  Take some chars
* out of the middle if necessary.  No matter what m.fillen is, this function
* always returns at least the file stem and extension.
PRIVATE m.bname, m.elipse, m.remain
m.elipse = "..." + m.g_pathsep
IF _MAC
    m.bname = SUBSTR(m.filname, RAT(":",m.filname)+1)
ELSE
	m.bname = justfname(m.filname)
ENDIF
DO CASE
CASE LEN(m.filname) <= m.fillen
   m.retstr = m.filname
CASE LEN(m.bname) + LEN(m.elipse) >= m.fillen
   m.retstr = m.bname
OTHERWISE
   m.remain = MAX(m.fillen - LEN(m.bname) - LEN(m.elipse), 0)
   IF _MAC
	   m.retstr = LEFT(SUBSTR(m.filname,1,RAT(":",m.filname)-1),m.remain) ;
		    +m.elipse+m.bname
   ELSE
  	   m.retstr = LEFT(justpath(m.filname),m.remain)+m.elipse+m.bname
   ENDIF
ENDCASE
RETURN m.retstr


*
* ACTTHERM(<text>) - Activate thermometer.
*
* Activates thermometer.  Update the thermometer with UPDTHERM().
* Thermometer window is named "thermometer."  Be sure to RELEASE
* this window when done with thermometer.  Creates the global
* m.g_thermwidth.
*
*!*****************************************************************************
*!
*!      Procedure: ACTTHERM
*!
*!      Called by: STARTTHERM         (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE acttherm
PARAMETER m.text
PRIVATE m.prompt

*- for converter, hide separate therm
RETURN

DO CASE
CASE _WINDOWS
   m.prompt = LOWER(m.g_scrndbf)
	m.prompt = thermfname(m.prompt)
   IF !WEXIST("thermomete")
      DEFINE WINDOW thermomete ;
         AT 0,0 ;
         SIZE 5.615,63.833 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle ;
         NOFLOAT ;
         NOCLOSE ;
         NONE ;
         COLOR RGB(0, 0, 0, 192, 192, 192)
   ENDIF
   MOVE WINDOW thermomete CENTER
   ACTIVATE WINDOW thermomete NOSHOW

   @ 0.5,3 SAY m.text FONT m.g_tdlgface, m.g_tdlgsize STYLE m.g_tdlgstyle
   @ 1.5,3 SAY m.prompt FONT m.g_tdlgface, m.g_tdlgsize STYLE m.g_tdlgstyle
   @ 0.000,0.000 TO 0.000,63.833 ;
      COLOR RGB(255, 255, 255, 255, 255, 255)
   @ 0.000,0.000 TO 5.615,0.000 ;
      COLOR RGB(255, 255, 255, 255, 255, 255)
   @ 0.385,0.667 TO 5.231,0.667 ;
      COLOR RGB(128, 128, 128, 128, 128, 128)
   @ 0.308,0.667 TO 0.308,63.167 ;
      COLOR RGB(128, 128, 128, 128, 128, 128)
   @ 0.385,63.000 TO 5.308,63.000 ;
      COLOR RGB(255, 255, 255, 255, 255, 255)
   @ 5.231,0.667 TO 5.231,63.167 ;
      COLOR RGB(255, 255, 255, 255, 255, 255)
   @ 5.538,0.000 TO 5.538,63.833 ;
      COLOR RGB(128, 128, 128, 128, 128, 128)
   @ 0.000,63.667 TO 5.615,63.667 ;
      COLOR RGB(128, 128, 128, 128, 128, 128)
   @ 3.000,3.333 TO 4.231,3.333 ;
      COLOR RGB(128, 128, 128, 128, 128, 128)
   @ 3.000,60.333 TO 4.308,60.333 ;
      COLOR RGB(255, 255, 255, 255, 255, 255)
   @ 3.000,3.333 TO 3.000,60.333 ;
      COLOR RGB(128, 128, 128, 128, 128, 128)
   @ 4.231,3.333 TO 4.231,60.333 ;
      COLOR RGB(255, 255, 255, 255, 255, 255)
   m.g_thermwidth = 56.269

CASE _MAC
   m.prompt = LOWER(m.g_scrndbf)
  	m.prompt = thermfname(m.prompt)
   IF !WEXIST("thermomete")
      DEFINE WINDOW thermomete ;
         AT  INT((SROW() - (( 5.62 * ;
         FONTMETRIC(1, m.g_thermface, m.g_thermsize, m.g_thermstyle )) / ;
         FONTMETRIC(1, WFONT(1,""), WFONT( 2,""), WFONT(3,"")))) / 2), ;
         INT((SCOL() - (( 63.83 * ;
         FONTMETRIC(6, m.g_thermface, m.g_thermsize, m.g_thermstyle )) / ;
         FONTMETRIC(6, WFONT(1,""), WFONT( 2,""), WFONT(3,"")))) / 2) ;
         SIZE 5.62,63.83 ;
         FONT m.g_tdlgface, m.g_tdlgsize ;
         STYLE m.g_tdlgstyle ;
         NOFLOAT ;
         NOCLOSE ;
			NONE ;
         COLOR RGB(0, 0, 0, 192, 192, 192)
   ENDIF
   MOVE WINDOW thermomete CENTER
   ACTIVATE WINDOW thermomete NOSHOW

   IF ISCOLOR()
      @ 0.000,0.000 TO 5.62,63.83 PATTERN 1;
         COLOR RGB(192, 192, 192, 192, 192, 192)
   	@ 0.000,0.000 TO 0.000,63.83 ;
      	COLOR RGB(255, 255, 255, 255, 255, 255)
   	@ 0.000,0.000 TO 5.62,0.000 ;
      	COLOR RGB(255, 255, 255, 255, 255, 255)
   	@ 0.385,0.67 TO 5.23,0.67 ;
      	COLOR RGB(128, 128, 128, 128, 128, 128)
   	@ 0.31,0.67 TO 0.31,63.17 ;
      	COLOR RGB(128, 128, 128, 128, 128, 128)
   	@ 0.385,63.000 TO 5.31,63.000 ;
      	COLOR RGB(255, 255, 255, 255, 255, 255)
   	@ 5.23,0.67 TO 5.23,63.17 ;
      	COLOR RGB(255, 255, 255, 255, 255, 255)
   	@ 5.54,0.000 TO 5.54,63.83 ;
      	COLOR RGB(128, 128, 128, 128, 128, 128)
   	@ 0.000,63.67 TO 5.62,63.67 ;
      	COLOR RGB(128, 128, 128, 128, 128, 128)
   	@ 3.000,3.33 TO 4.23,3.33 ;
      	COLOR RGB(128, 128, 128, 128, 128, 128)
   	@ 3.000,60.33 TO 4.31,60.33 ;
      	COLOR RGB(255, 255, 255, 255, 255, 255)
   	@ 3.000,3.33 TO 3.000,60.33 ;
      	COLOR RGB(128, 128, 128, 128, 128, 128)
   	@ 4.23,3.33 TO 4.23,60.33 ;
      	COLOR RGB(255, 255, 255, 255, 255, 255)
   ELSE
      @ 0.000, 0.000 TO 5.62, 63.830  PEN 2
      @ 0.230, 0.430 TO 5.39, 63.400  PEN 1
   ENDIF
   @ 0.5,3 SAY m.text FONT m.g_thermface, m.g_thermsize STYLE m.g_thermstyle ;
      COLOR RGB(0,0,0,192,192,192)
   @ 1.5,3 SAY m.prompt FONT m.g_thermface, m.g_thermsize STYLE m.g_thermstyle ;
      COLOR RGB(0,0,0,192,192,192)

   m.g_thermwidth = 57.17
	IF !ISCOLOR()
   	@ 3.000,3.33 TO 4.23,m.g_thermwidth + 3.33
	ENDIF

   SHOW WINDOW thermomete TOP
CASE _DOS OR _UNIX
   m.prompt = SUBSTR(SYS(2014,m.g_scrndbf),1,48)+;
      IIF(LEN(m.g_scrndbf)>48,"...","")
   IF !WEXIST("thermomete")
      DEFINE WINDOW thermomete;
         FROM INT((SROW()-7)/2), INT((SCOL()-57)/2) ;
         TO INT((SROW()-7)/2) + 6, INT((SCOL()-57)/2) + 57;
         DOUBLE COLOR SCHEME 5
   ENDIF
   ACTIVATE WINDOW thermomete NOSHOW

   m.g_thermwidth = 50
   @ 0,3 SAY m.text
   @ 1,3 SAY UPPER(m.prompt)
   @ 2,1 TO 4,m.g_thermwidth+4 &g_boxstrg

   SHOW WINDOW thermomete TOP
ENDCASE
RETURN

*
* UPDTHERM(<percent>) - Update thermometer.
*
*!*****************************************************************************
*!
*!      Procedure: UPDTHERM
*!
*!      Called by: TRANSPRT.PRG
*!               : GRAPHICTOCHAR      (procedure in TRANSPRT.PRG)
*!               : CHARTOGRAPHIC      (procedure in TRANSPRT.PRG)
*!               : UPDATESCREEN       (procedure in TRANSPRT.PRG)
*!               : UPDATEREPORT       (procedure in TRANSPRT.PRG)
*!               : NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : ALLCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : ALLENVIRONS        (procedure in TRANSPRT.PRG)
*!               : ALLOTHERS          (procedure in TRANSPRT.PRG)
*!               : ALLGROUPS          (procedure in TRANSPRT.PRG)
*!               : RPTCONVERT         (procedure in TRANSPRT.PRG)
*!               : LABELLINES         (procedure in TRANSPRT.PRG)
*!               : CALCWINDOWDIMENSION(procedure in TRANSPRT.PRG)
*!               : FINDWIDEROBJECTS   (procedure in TRANSPRT.PRG)
*!               : REPOOBJECTS        (procedure in TRANSPRT.PRG)
*!               : ADJINVBTNS         (procedure in TRANSPRT.PRG)
*!               : JOINLINES          (procedure in TRANSPRT.PRG)
*!               : WRITERESULT        (procedure in TRANSPRT.PRG)
*!
*!          Calls: ACTTHERM           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE updtherm
PARAMETER m.percent
PRIVATE m.nblocks, m.percent

*- for converter, use gOTherm
IF TYPE("gOTherm") == "O"
	gOTherm.Update(MIN(MAX(m.percent,0),100))
ENDIF
RETURN

IF m.percent > 100
   m.percent = 100
ENDIF
IF m.percent < 0
   m.percent = 0
ENDIF

IF !WEXIST("thermomete")
   DO acttherm WITH ""
ENDIF
ACTIVATE WINDOW thermomete

m.nblocks = (m.percent/100) * (m.g_thermwidth)
DO CASE
CASE _WINDOWS
   @ 3.000,3.333 TO 4.231,m.nblocks + 3.333 ;
      PATTERN 1 COLOR RGB(128, 128, 128, 128, 128, 128)
CASE _MAC
   @ 3.000,3.33 TO 4.23,m.nblocks + 3.33 ;
      PATTERN 1 COLOR RGB(0, 0, 128, 0, 0, 128)
OTHERWISE
   @ 3,3 SAY REPLICATE("Û",m.nblocks)
ENDCASE
RETURN

*
* deactTherm - Deactivate and Release thermometer window.
*
*!*****************************************************************************
*!
*!      Procedure: DEACTTHERM
*!
*!      Called by: CLEANUP            (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE deacttherm
IF WEXIST("thermomete")
   RELEASE WINDOW thermomete
ENDIF
RETURN

*
* ERRORHANDLER - Error Processing Center.
*
*!*****************************************************************************
*!
*!      Procedure: ERRORHANDLER
*!
*!      Called by: TRANSPRT.PRG
*!               : SETVERSION         (procedure in TRANSPRT.PRG)
*!               : cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!               : STRUCTDIALOG()     (function  in TRANSPRT.PRG)
*!               : SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!          Calls: CLEANUP            (procedure in TRANSPRT.PRG)
*!               : ERRSHOW            (procedure in TRANSPRT.PRG)
*!               : CLEANWIND          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE errorhandler
PARAMETERS m.msg, m.linenum, errcode
IF ERROR() = 22
   ON ERROR &onerror
   m.g_status = 1
   DO cleanup
   CANCEL
ENDIF
SET MESSAGE TO
DO CASE
CASE errcode == c_error1
   m.g_status = 1
CASE errcode == c_error2
   DO errshow WITH m.msg, m.linenum
   m.g_status = 2
   ON ERROR &onerror
CASE errcode == c_error3
   ON ERROR &onerror
   DO errshow WITH m.msg, m.linenum
   DO cleanwind
   m.g_status = 3
   m.g_returncode = c_cancel
   DO cleanup WITH .T.
ENDCASE

*
* CLEANWIND - Release windows that might still be open
*
*!*****************************************************************************
*!
*!      Procedure: CLEANWIND
*!
*!      Called by: ERRORHANDLER       (procedure in TRANSPRT.PRG)
*!               : ESCHANDLER         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE cleanwind
IF WEXIST("transdlg") AND WVISIBLE("transdlg")
   RELEASE WINDOW transdlg
ENDIF
IF WEXIST("lblwind") AND WVISIBLE("lblwind")
   RELEASE WINDOW lblwind
ENDIF
IF WEXIST("msgscrn") AND WVISIBLE("msgscrn")
   RELEASE WINDOW msgscrn
ENDIF
IF WEXIST("Thermomete") AND WVISIBLE("Thermomete")
   RELEASE WINDOW thermomete
ENDIF
IF WEXIST("tpselect") AND WVISIBLE("tpselect")
   RELEASE WINDOW tpselect
ENDIF

*
* ESCHANDLER - Escape handler.
*
*!*****************************************************************************
*!
*!      Procedure: ESCHANDLER
*!
*!      Called by: SETALL             (procedure in TRANSPRT.PRG)
*!
*!          Calls: CLEANWIND          (procedure in TRANSPRT.PRG)
*!               : CLEANUP            (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
*PROCEDURE eschandler
*ON ERROR &onerror
*m.g_status = 1
*DO cleanwind
*DO cleanup
*CANCEL

*
* ERRSHOW - Show error in an alert box on the screen.
*
*!*****************************************************************************
*!
*!      Procedure: ERRSHOW
*!
*!      Called by: ERRORHANDLER       (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE errshow
PARAMETER m.msg, m.lineno
PRIVATE m.curcursor

DO CASE
CASE _WINDOWS
   DEFINE WINDOW ALERT ;
      AT 0,0 ;
      SIZE 5.615,63.833 ;
      FONT m.g_tdlgface, m.g_tdlgsize ;
      STYLE m.g_tdlgstyle ;
      NOCLOSE ;
      DOUBLE ;
      TITLE T_TRANSPERR_LOC 
   MOVE WINDOW ALERT CENTER
   ACTIVATE WINDOW ALERT NOSHOW

   m.msg = SUBSTR(m.msg,1,44)+IIF(LEN(m.msg)>44,"...","")
   @ 1,(WCOLS()-TXTWIDTH( m.msg ))/2 SAY m.msg

   m.msg = T_LINENO_LOC+LTRIM(STR(m.lineno,5))
   @ 2,(WCOLS()-TXTWIDTH( m.msg ))/2 SAY m.msg

   m.msg = T_CLEANUP_LOC
   @ 3,(WCOLS()-TXTWIDTH( m.msg ))/2 SAY m.msg
CASE _MAC
   DEFINE WINDOW ALERT ;
      AT 0,0 ;
      SIZE 5.615,63.833 ;
      FONT m.g_tdlgface, m.g_tdlgsize ;
      STYLE m.g_tdlgstyle ;
      NOCLOSE ;
      DOUBLE ;
      TITLE T_TRANSPERR_LOC 
   MOVE WINDOW ALERT CENTER
   ACTIVATE WINDOW ALERT NOSHOW

   m.msg = SUBSTR(m.msg,1,44)+IIF(LEN(m.msg)>44,"...","")
   @ 1,(WCOLS()-TXTWIDTH( m.msg ))/2 SAY m.msg

   m.msg = T_LINENO_LOC+LTRIM(STR(m.lineno,5))
   @ 2,(WCOLS()-TXTWIDTH( m.msg ))/2 SAY m.msg

   m.msg = T_CLEANUP_LOC
   @ 3,(WCOLS()-TXTWIDTH( m.msg ))/2 SAY m.msg
OTHERWISE
   DEFINE WINDOW ALERT;
      FROM INT((SROW()-6)/2), INT((SCOL()-50)/2) ;
      TO INT((SROW()-6)/2) + 6, INT((SCOL()-50)/2) + 50;
      FLOAT NOGROW NOCLOSE NOZOOM   SHADOW DOUBLE;
      COLOR SCHEME 7

   ACTIVATE WINDOW ALERT NOSHOW

   m.msg = SUBSTR(m.msg,1,44)+IIF(LEN(m.msg)>44,"...","")
   @ 1,(WCOLS()-LEN(m.msg))/2 SAY m.msg

   m.msg = T_LINENO_LOC+STR(m.lineno, 5)
   @ 2,(WCOLS()-LEN(m.msg))/2 SAY m.msg

   m.msg = T_CLEANUP_LOC
   @ 3,(WCOLS()-LEN(m.msg))/2 SAY m.msg
ENDCASE

m.curcursor = SET( "CURSOR" )
SET CURSOR OFF
SHOW WINDOW ALERT

=INKEY(0, "M")

RELEASE WINDOW ALERT
SET CURSOR &curcursor

*
* JUSTSTEM - Returns just the stem name of the file
*
*!*****************************************************************************
*!
*!       Function: JUSTSTEM
*!
*!*****************************************************************************
FUNCTION juststem
* Return just the stem name from "filname"
PARAMETERS m.filname
PRIVATE ALL
IF RAT('\',m.filname) > 0
   m.filname = SUBSTR(m.filname,RAT('\',m.filname)+1,255)
ENDIF
IF AT(':',m.filname) > 0
   m.filname = SUBSTR(m.filname,AT(':',m.filname)+1,255)
ENDIF
IF AT('.',m.filname) > 0
   m.filname = SUBSTR(m.filname,1,AT('.',m.filname)-1)
ENDIF
RETURN ALLTRIM(UPPER(m.filname))

*
* STRIPPATH - Strip the path from a file name.
*
* Description:
* Find positions of backslash in the name of the file.  If there is one
* take everything to the right of its position and make it the new file
* name.  If there is no slash look for colon.  Again if found, take
* everything to the right of it as the new name.  If neither slash
* nor colon are found then return the name unchanged.
*
* Parameters:
* filename - character string representing a file name
*
* Return value:
* The string "filename" with any path removed
*
*!*****************************************************************************
*!
*!       Function: STRIPPATH
*!
*!      Called by: TRANSPRT.PRG
*!               : ADJBITMAPCTRL      (procedure in TRANSPRT.PRG)
*!               : SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION strippath
PARAMETER m.filename
PRIVATE m.slashpos, m.namelen, m.colonpos
m.slashpos = RAT("\", m.filename)
IF m.slashpos > 0
   m.namelen  = LEN(m.filename) - m.slashpos
   m.filename = RIGHT(m.filename, m.namelen)
ELSE
   m.colonpos = RAT(":", m.filename)
   IF m.colonpos > 0
      m.namelen  = LEN(m.filename) - m.colonpos
      m.filename = RIGHT(m.filename, m.namelen)
   ENDIF
ENDIF
RETURN m.filename

*
* ISOBJECT - Is otype a screen or report object?
*
*!*****************************************************************************
*!
*!       Function: ISOBJECT
*!
*!      Called by: UPDATESCREEN       (procedure in TRANSPRT.PRG)
*!               : NEWCHARTOGRAPHIC   (procedure in TRANSPRT.PRG)
*!               : NEWGRAPHICTOCHAR   (procedure in TRANSPRT.PRG)
*!               : FINDLIKEVPOS       (procedure in TRANSPRT.PRG)
*!               : FINDLIKEHPOS       (procedure in TRANSPRT.PRG)
*!               : SELECTOBJ          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION isobject
PARAMETER m.otype
RETURN INLIST(m.otype,c_otlist,c_ottxtbut,c_otbox,c_otradbut,c_otchkbox,c_otfield, ;
   c_otpopup,c_otinvbut,c_otspinner,c_otpicture,c_otline,c_otrepfld,c_otrepvar,c_ottext)


*
* ISREPTOBJECT - Is otype a report object?
*
*!*****************************************************************************
*!
*!       Function: ISREPTOBJECT
*!
*!      Called by: RPTCONVERT         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION isreptobject
PARAMETER m.otype
RETURN INLIST(m.otype,c_otrepfld,c_ottext,c_otbox,c_otline)

*
* ISGRAPHOBJ - Is otype an object that is present in graphics screens/reports but not
*              in character screens?
*
*!*****************************************************************************
*!
*!       Function: ISGRAPHOBJ
*!
*!*****************************************************************************
FUNCTION isgraphobj
PARAMETER m.otype
RETURN INLIST(m.otype,c_otpicture,c_otspinner)

*!*****************************************************************************
*!
*!       Function: ISENVIRON
*!
*!*****************************************************************************
FUNCTION isenviron
PARAMETER m.otype
RETURN INLIST(m.otype,c_otworkar,c_otindex,c_otrel)

*!*****************************************************************************
*!
*!       Function: IsNewerEnv
*!
*!*****************************************************************************
FUNCTION IsNewerEnv
PARAMETER m.mustexist    && does the "to" environment have to exist?
PRIVATE m.maxfromts, m.maxtots
* Is the "from" platform environment newer than the "to" platform environment
m.maxfromts = -1
SCAN FOR platform = m.g_fromplatform and IsEnviron(objtype)
   m.maxfromts = MAX(timestamp, m.maxfromts)
ENDSCAN
m.maxtots = -1
SCAN FOR platform = m.g_toplatform and IsEnviron(objtype)
   m.maxtots = MAX(timestamp, m.maxtots)
ENDSCAN
IF m.mustexist
   * The to platform had an environment, but it was out of date
   RETURN IIF(m.maxfromts > m.maxtots AND m.maxtots >= 0 , .T. , .F.)
ELSE
   * The to platform had no environment and the from platform does
   RETURN IIF(m.maxfromts >= 0 AND m.maxtots < 0  , .T. , .F.)
ENDIF

*
* HASRECORD - Does filname contain platform records for target?
*
*!*****************************************************************************
*!
*!       Function: HASRECORDS
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION hasrecords
PARAMETER m.target
PRIVATE m.inrec, m.retval
m.inrec = RECNO()
DO CASE
CASE TYPE("PLATFORM") <> "U"
   LOCATE FOR UPPER(ALLTRIM(platform)) == UPPER(ALLTRIM(m.target))
   m.retval = FOUND()
CASE UPPER(ALLTRIM(m.target)) == "DOS"
   m.retval = .T.   && assume DOS if no platform field
OTHERWISE
   m.retval = .F.
ENDCASE
GOTO m.inrec
RETURN m.retval


*!*****************************************************************************
*!
*!       Function: setctrl
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION setctrl
* This function is called during Transporter setup to initialize some of
* the font selections.  It is also called as the valid() routine when
* the 2D controls checkbox is checked.
DO CASE
CASE _MAC
	* Set fonts based on 2D/3D choice--Mac only
	IF m.g_look2d
	   * Push button and controls font.  Font button does not override this.
   	m.g_ctrlfface        = "Chicago"
   	m.g_ctrlfsize        = 12
   	m.g_ctrlfstyle       = ""

		* Window measurement font
   	m.g_windfface        = "Chicago"
   	m.g_windfsize        = 12
   	m.g_windfstyle       = ""

		* Set default font for SCX/FRX objects (e.g., text).
		* The Font button may override this.
   	m.g_dfltfface         = "Geneva"
   	m.g_dfltfsize         = 10
   	m.g_dfltfstyle        = ""

		m.g_macbtnheight = 1.125
		m.g_macbtnface   = "Chicago"
		m.g_macbtnsize   = 12
		m.g_macbtnstyle  = ""
	ELSE
   	m.g_ctrlfface        = "Geneva"
   	m.g_ctrlfsize        = 9
   	m.g_ctrlfstyle       = "B"

		* The cxChar for Geneva, 10 nonbold is 6 pixels, just like
 		* MS Sans Serif,8 bold.  This is a good mapping for screens coming
		* over from Windows.
   	m.g_windfface        = "Geneva"
   	m.g_windfsize        = 10
   	m.g_windfstyle       = ""

		* Set default font for SCX objects.  The Font button may
 		* override this.
   	m.g_dfltfface         = "Geneva"
   	m.g_dfltfsize         = 10
   	m.g_dfltfstyle        = ""

		m.g_macbtnheight = 1.500
		m.g_macbtnface   = "Geneva"
		m.g_macbtnsize   = 10
		m.g_macbtnstyle  = "B"
	ENDIF
   m.g_winbtnheight = 1.769
	m.g_winbtnface   = "MS Sans Serif"
	m.g_winbtnsize   = 8
	m.g_winbtnstyle  = "B"

	m.g_thermface    = "Geneva"
	m.g_thermsize    = 10
	m.g_thermstyle   = "T"
	m.g_btnheight    = m.g_macbtnheight
OTHERWISE
   * Font for push buttons
   m.g_ctrlfface        = "MS Sans Serif"
   m.g_ctrlfsize        = 8
   m.g_ctrlfstyle       = "B"

	* Window measurement font
   m.g_windfface        = "MS Sans Serif"
   m.g_windfsize        = 8
   m.g_windfstyle       = "B"

   * Font selections for fields/text in the SCX/FRX itself.  May be overridden by user.
   *- use remembered settings
   IF EMPTY(gAShowMe[m.g_tpFileIndx,3])
      m.g_dfltfface         = "MS Sans Serif"
      m.g_dfltfsize         = 8
      m.g_dfltfstyle        = "B"
   ELSE
	   m.g_dfltfface = m.gAShowMe[m.g_tpFileIndx,3]
	   m.g_dfltfsize = m.gAShowMe[m.g_tpFileIndx,4]
	   m.g_dfltfstyle = m.gAShowMe[m.g_tpFileIndx,5]
   ENDIF

   m.g_winbtnheight = 1.769
	m.g_macbtnheight = 1.500      && figure that most screens will be 3D
	m.g_macbtnface   = "Geneva"
	m.g_macbtnsize   = 10
	m.g_macbtnstyle  = "B"
	m.g_winbtnface   = "MS Sans Serif"
	m.g_winbtnsize   = 8
	m.g_winbtnstyle  = "B"
	m.g_btnheight    = m.g_winbtnheight

ENDCASE

*!*****************************************************************************
*!
*!       Function: SETRPTFONT
*!
*!*****************************************************************************
PROCEDURE setrptfont
* Set the default report font for a report coming to the Mac
* Disabled by WJK
IF .F. && _MAC AND INLIST(m.g_filetype,c_report,c_label)
	m.g_windfface        = m.g_rptfface
	m.g_windfsize        = m.g_rptfsize
	m.g_windfstyle       = num2style(m.g_rptfstyle)

	* Set default font for FRX objects.  The Font button may
	* override this.
   *- use remembered settings
   IF EMPTY(gAShowMe[m.g_tpFileIndx,3])
		m.g_dfltfface         = m.g_rptfface
		m.g_dfltfsize         = m.g_rptfsize
		m.g_dfltfstyle        = num2style(m.g_rptfstyle)
   ELSE
	   m.g_dfltfface = m.gAShowMe[m.g_tpFileIndx,3]
	   m.g_dfltfsize = m.gAShowMe[m.g_tpFileIndx,4]
	   m.g_dfltfstyle = m.gAShowMe[m.g_tpFileIndx,5]
   ENDIF

ENDIF

*
* ASKFONT - Prompt for a font
*
*!*****************************************************************************
*!
*!       Function: ASKFONT
*!
*!      Called by: SCXFRXDIALOG()     (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION askfont
PRIVATE m.fontstrg

*- temp fix here
IF .F.
	fontstrg = "Geneva,10,N"

	   m.g_dfltfface   =  LEFT(m.fontstrg,AT(',',m.fontstrg)-1)
	   m.g_dfltfsize   =  VAL(SUBSTR(m.fontstrg,AT(',',m.fontstrg)+1,RAT(',',m.fontstrg)-AT(',',m.fontstrg)-1))
	   m.g_dfltfstyle  =  SUBSTR(m.fontstrg,RAT(',',m.fontstrg)+1)
	   IF _MAC OR _WINDOWS
	      m.g_rptlinesize      = (FONTMETRIC(1, m.g_dfltfface, m.g_dfltfsize, m.g_rpttxtfontstyle) / m.g_pixelsize) * 10000
	      m.g_rptcharsize      = (FONTMETRIC(6, m.g_dfltfface, m.g_dfltfsize, m.g_rpttxtfontstyle) / m.g_pixelsize) * 10000
	   ENDIF
	   m.g_fontset = .T.
	   m.gAShowMe[m.g_tpFileIndx,3] = m.g_dfltfface
	   m.gAShowMe[m.g_tpFileIndx,4] = m.g_dfltfsize
	   m.gAShowMe[m.g_tpFileIndx,5] = m.g_dfltfstyle

	RETURN
ENDIF
*- end temp fix

* Set up a default font for reports
IF m.g_filetype = c_report AND (_WINDOWS OR _MAC)
   DEFINE WINDOW transtemp FROM 1,1 TO 2,2 FONT "&g_rptfface", m.g_rptfsize
   ACTIVATE WINDOW transtemp NOSHOW
ENDIF

m.fontstrg = GETFONT()

IF !EMPTY(m.fontstrg)
   m.g_dfltfface   =  LEFT(m.fontstrg,AT(',',m.fontstrg)-1)
   m.g_dfltfsize   =  VAL(SUBSTR(m.fontstrg,AT(',',m.fontstrg)+1,RAT(',',m.fontstrg)-AT(',',m.fontstrg)-1))
   m.g_dfltfstyle  =  SUBSTR(m.fontstrg,RAT(',',m.fontstrg)+1)
   IF _MAC OR _WINDOWS
      m.g_rptlinesize      = (FONTMETRIC(1, m.g_dfltfface, m.g_dfltfsize, m.g_rpttxtfontstyle) / m.g_pixelsize) * 10000
      m.g_rptcharsize      = (FONTMETRIC(6, m.g_dfltfface, m.g_dfltfsize, m.g_rpttxtfontstyle) / m.g_pixelsize) * 10000
   ENDIF
   m.g_fontset = .T.
   m.gAShowMe[m.g_tpFileIndx,3] = m.g_dfltfface
   m.gAShowMe[m.g_tpFileIndx,4] = m.g_dfltfsize
   m.gAShowMe[m.g_tpFileIndx,5] = m.g_dfltfstyle
ENDIF

IF m.g_filetype = c_report AND (_WINDOWS OR _MAC)
   RELEASE WINDOW transtemp
ENDIF

RETURN

*
* IS20SCX - Is the current database a 2.0 screen?
*
*!*****************************************************************************
*!
*!       Function: IS20SCX
*!
*!*****************************************************************************
FUNCTION is20scx
RETURN (FCOUNT() = c_20scxfld)
*
* IS20FRX - Is the current database a 2.0 report?
*
*!*****************************************************************************
*!
*!       Function: IS20FRX
*!
*!*****************************************************************************
FUNCTION is20frx
RETURN (FCOUNT() = c_20frxfld)
*
* IS20LBX - Is the current database a 2.0 screen?
*
*!*****************************************************************************
*!
*!       Function: IS20LBX
*!
*!*****************************************************************************
FUNCTION is20lbx
RETURN (FCOUNT() = c_20lbxfld)
IF WEXIST("lblwind")   AND WVISIBLE("lblwind")
   RELEASE WINDOW lblwind
ENDIF

*
* GETSNIPFLAG - See if we are just updating snippets
*
*!*****************************************************************************
*!
*!       Function: GETSNIPFLAG
*!
*!      Called by: UPDATESCREEN       (procedure in TRANSPRT.PRG)
*!
*!          Calls: WORDNUM()          (function  in TRANSPRT.PRG)
*!               : MATCH()            (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION getsnipflag
PARAMETER snippet
PRIVATE m.oldmline, m.retcode
* Format for directive is "#TRAN SNIPPET ONLY" in setup snippet
m.oldmline = _MLINE
m.retcode = .F.
IF AT('#',snippet) > 0
   _MLINE = 0
   m.sniplen = LEN(snippet)
   DO WHILE _MLINE < m.sniplen
      m.line = MLINE(snippet,1,_MLINE)
      m.upline = UPPER(LTRIM(m.line))
      IF '#TRAN' $ m.upline
         IF LEFT(wordnum(m.upline,1),5) = '#TRAN' ;
               AND match(wordnum(m.upline,2),'SNIPPETS') ;
               AND match(wordnum(m.upline,3),'ONLY')
            m.retcode = .T.
         ENDIF
      ENDIF
   ENDDO
   _MLINE = m.oldmline
ENDIF
RETURN m.retcode


*
* MATCH - Returns TRUE if candidate is a valid 4-or-more-character abbreviation of keyword
*
*!*****************************************************************************
*!
*!       Function: MATCH
*!
*!      Called by: GETSNIPFLAG()      (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION match
PARAMETER candidate, keyword
PRIVATE m.in_exact, m.retval

m.in_exact = SET("EXACT")
SET EXACT OFF
DO CASE
CASE EMPTY(m.candidate)
   m.retval = EMPTY(m.keyword)
CASE LEN(m.candidate) < 4
   m.retval = IIF(m.candidate == m.keyword,.T.,.F.)
OTHERWISE
   m.retval = IIF(m.keyword = m.candidate,.T.,.F.)
ENDCASE
IF m.in_exact != "OFF"
   SET EXACT ON
ENDIF
RETURN m.retval


*
* WORDNUM - Returns w_num-th word from string strg
*
*!*****************************************************************************
*!
*!       Function: WORDNUM
*!
*!      Called by: GETSNIPFLAG()      (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION wordnum
PARAMETERS strg,w_num
PRIVATE strg,s1,w_num,ret_str

m.s1 = ALLTRIM(m.strg)

* Replace tabs with spaces
m.s1 = CHRTRANC(m.s1,CHR(9)," ")

* Reduce multiple spaces to a single space
DO WHILE AT('  ',m.s1) > 0
   m.s1 = STRTRAN(m.s1,'  ',' ')
ENDDO

ret_str = ""
DO CASE
CASE m.w_num > 1
   DO CASE
   CASE AT(" ",m.s1,m.w_num-1) = 0   && No word w_num.  Past end of string.
      m.ret_str = ""
   CASE AT(" ",m.s1,m.w_num) = 0     && Word w_num is last word in string.
      m.ret_str = SUBSTR(m.s1,AT(" ",m.s1,m.w_num-1)+1,255)
   OTHERWISE                         && Word w_num is in the middle.
      m.strt_pos = AT(" ",m.s1,m.w_num-1)
      m.ret_str  = SUBSTR(m.s1,strt_pos,AT(" ",m.s1,m.w_num)+1 - strt_pos)
   ENDCASE
CASE m.w_num = 1
   IF AT(" ",m.s1) > 0               && Get first word.
      m.ret_str = SUBSTR(m.s1,1,AT(" ",m.s1)-1)
   ELSE                              && There is only one word.  Get it.
      m.ret_str = m.s1
   ENDIF
ENDCASE
RETURN ALLTRIM(m.ret_str)

*
* ADDBS - Add a backslash unless there is one already there.
*
*!*****************************************************************************
*!
*!       Function: ADDBS
*!
*!      Called by: FORCEEXT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION addbs
* Add a backslash to a path name, if there isn't already one there
PARAMETER m.pathname
PRIVATE ALL
m.pathname = ALLTRIM(UPPER(m.pathname))
IF !(RIGHT(m.pathname,1) $ '\:') AND !EMPTY(m.pathname)
   m.pathname = m.pathname + '\'
ENDIF
RETURN m.pathname

*!*****************************************************************************
*!       Function: JUSTFNAME
*!*****************************************************************************
FUNCTION justfname
*)
*) JUSTFNAME - Return just the filename (i.e., no path) from "filname"
*)
PARAMETERS m.filname

*- use platform specific path (10/28/95 jd)
LOCAL clocalfname, cdirsep
clocalfname = SYS(2027,m.filname)
cdirsep = IIF(_mac,':','\')
IF RAT(m.cdirsep ,m.clocalfname) > 0
   m.clocalfname = SUBSTR(m.clocalfname,RAT(m.cdirsep,m.clocalfname)+1,255)
ENDIF
IF AT(':',m.clocalfname) > 0
   m.clocalfname = SUBSTR(m.clocalfname,AT(':',m.clocalfname)+1,255)
ENDIF
RETURN ALLTRIM(m.clocalfname)


*!*****************************************************************************
*!       Function: JUSTPATH
*!*****************************************************************************
FUNCTION justpath
*)
*) JUSTPATH - Returns just the pathname.
*)
PARAMETERS m.filname
m.filname = ALLTRIM(UPPER(m.filname))
*- use platform specific path (10/28/95 jd)
LOCAL clocalfname, cdirsep
clocalfname = SYS(2027,m.filname)
cdirsep = IIF(_mac,':','\')
IF m.cdirsep $ m.clocalfname 
   m.clocalfname = SUBSTR(m.clocalfname,1,RAT(m.cdirsep,m.clocalfname ))
   IF RIGHT(m.filname,1) = m.cdirsep AND LEN(m.filname) > 1 ;
            AND SUBSTR(m.clocalfname,LEN(m.clocalfname)-1,1) <> ':'
         clocalfname= SUBSTR(m.clocalfname,1,LEN(m.clocalfname)-1)
   ENDIF
   RETURN m.clocalfname
ELSE
   RETURN ''
ENDIF

*
* FORCEEXT - Force filename to have a paricular extension.
*
*!*****************************************************************************
*!
*!       Function: FORCEEXT
*!
*!      Called by: cvrt102FRX()       (function  in TRANSPRT.PRG)
*!               : cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!
*!          Calls: JUSTPATH()         (function  in TRANSPRT.PRG)
*!               : JUSTFNAME()        (function  in TRANSPRT.PRG)
*!               : ADDBS()            (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION forceext
* Force the extension of "filname" to be whatever ext is.
PARAMETERS m.filname,m.ext
PRIVATE ALL
IF SUBSTR(m.ext,1,1) = "."
   m.ext = SUBSTR(m.ext,2,3)
ENDIF

m.pname = justpath(m.filname)
m.filname = justfname(UPPER(ALLTRIM(m.filname)))
IF AT('.',m.filname) > 0
   m.filname = SUBSTR(m.filname,1,AT('.',m.filname)-1) + '.' + m.ext
ELSE
   m.filname = m.filname + '.' + m.ext
ENDIF
RETURN addbs(m.pname) + m.filname

*!*****************************************************************************
*!
*!       Function: CVTLONG
*!
*!          Calls: CVTSHORT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION cvtlong
PARAMETER m.itext, m.ioff
RETURN cvtshort(m.itext,m.ioff) + (65536 * cvtshort(m.itext,m.ioff+2))

*!*****************************************************************************
*!
*!       Function: CVTSHORT
*!
*!      Called by: GETOLDREPORTTYPE() (function  in TRANSPRT.PRG)
*!               : cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!               : CVTLONG()          (function  in TRANSPRT.PRG)
*!
*!          Calls: CVTBYTE()          (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION cvtshort
PARAMETER m.itext, m.ioff
RETURN cvtbyte(m.itext,m.ioff) + (256 * cvtbyte(m.itext,m.ioff+1))

*!*****************************************************************************
*!
*!       Function: CVTBYTE
*!
*!      Called by: cvrtfbpRPT      (procedure in TRANSPRT.PRG)
*!               : CVTSHORT()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION cvtbyte
PARAMETER m.itext, m.ioff
RETURN ASC(SUBSTR(m.itext,m.ioff+1,1))

*!*****************************************************************************
*!
*!       Function: OBJ2BASEFONT
*!
*!      Called by: FILLININFO         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION obj2basefont
PARAMETER m.mwidth, m.bfontface, m.bfontsize, m.bfontstyle, m.ofontface, ;
   m.ofontsize, m.ofontstyle
* Map a width from one font to another one
DO CASE
CASE m.g_char2grph
   RETURN m.mwidth * FONTMETRIC(6,m.ofontface,m.ofontsize,m.ofontstyle) ;
      / FONTMETRIC(6,m.bfontface,m.bfontsize,m.bfontstyle)
CASE m.g_grph2char AND UPPER(m.ofontface) == "MS SANS SERIF" AND ;
      UPPER(m.bfontface) == "MS SANS SERIF" AND ;
      m.ofontsize = m.bfontsize AND ;
      !("B" $ m.ofontstyle) AND ;
      "B" $ m.bfontstyle
   * We can't use FONTMETRIC on DOS, so we use heuristics instead.  Most
   * of the time we will be converting between MS Sans Serif 8 Bold and
   * MS Sans Serif Regular.  If that is the case here, use the 5/6 conversion
   * factor that is the relative widths of the chars in these two font styles.
   RETURN m.mwidth * 5/6
OTHERWISE
   RETURN m.mwidth
ENDCASE


*!*****************************************************************************
*!
*!       Function: VERSIONCAP
*!
*!      Called by: RDVALID()          (function  in TRANSPRT.PRG)
*!               : SELECTOBJ          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION versioncap
* Map a platform name ("DOS") to its descriptive equivalent ("MS-DOS")
PARAMETER m.strg
DO CASE
CASE strg = c_dosname
   RETURN "MS-DOS"
CASE strg = c_winname
   RETURN "Windows"
CASE strg = c_macname
   RETURN "Macintosh"
CASE strg = c_unixname
   RETURN c_unixname
OTHERWISE
   RETURN strg
ENDCASE


*!*****************************************************************************
*!
*!       Function: BLACKBOX
*!
*!*****************************************************************************
FUNCTION blackbox
PARAMETER otype , mred, mblue, mgreen, mpattern
* Is this a black box?
IF m.g_grph2char AND m.otype = c_otbox AND ;
      m.mred = 0 AND m.mblue = 0 AND m.mgreen = 0 ;
      AND m.mpattern = 0
   RETURN .T.
ELSE
   RETURN .F.
ENDIF

*!*****************************************************************************
*!
*!      Procedure: SELECTOBJ
*!
*!      Called by: GRAPHICTOCHAR      (procedure in TRANSPRT.PRG)
*!               : CHARTOGRAPHIC      (procedure in TRANSPRT.PRG)
*!
*!          Calls: INITSEL            (procedure in TRANSPRT.PRG)
*!               : ISOBJECT()         (function  in TRANSPRT.PRG)
*!               : ADDSEL             (procedure in TRANSPRT.PRG)
*!               : VERSIONCAP()       (function  in TRANSPRT.PRG)
*!               : TPSELECT           (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!        Indexes: ID                     (tag)
*!
*!*****************************************************************************
PROCEDURE selectobj
* Figure out what to transport
DO initsel

IF m.g_snippets
   m.g_tempalias = "S" + SUBSTR(LOWER(SYS(3)),2,8)
   SELECT * FROM (m.g_scrnalias) ;
      WHERE !DELETED() AND platform = m.g_fromplatform ;
         AND oktransport(comment) ;
      INTO CURSOR (m.g_tempalias)
   IF _TALLY > 0
      INDEX ON uniqueid TAG id

      SELECT (m.g_scrnalias)
      SET RELATION TO uniqueid INTO (m.g_tempalias) ADDITIVE
      LOCATE FOR .T.
      DO CASE
      CASE m.g_filetype = c_screen
         SCAN FOR platform = m.g_toplatform ;
               AND (INLIST(objtype,C_OBJTYPELIST) OR objtype = c_otheader OR objtype = c_otworkar) ;
               AND &g_tempalias..timestamp > timestamp
            DO addsel WITH "Upd"
         ENDSCAN
      CASE m.g_filetype = c_report
         SCAN FOR platform = m.g_toplatform AND ;
               INLIST(objtype,c_otheader,c_otfield,c_otpicture, ;
                 c_otrepfld,c_otband,c_otrepvar,c_ottext,c_otline,c_otbox,c_otworkar) ;
               AND &g_tempalias..timestamp > timestamp
            DO addsel WITH "Upd"
         ENDSCAN
      ENDCASE
      SELECT (m.g_tempalias)
      USE
   ENDIF
   SELECT (m.g_scrnalias)
ENDIF

IF m.g_newobjects
   m.junk = "S" + SUBSTR(LOWER(SYS(3)),2,8)
   DO CASE
   CASE m.g_char2grph
      SELECT * FROM (m.g_scrnalias) ;
         WHERE !DELETED() AND platform = m.g_fromplatform AND ;
         !(objtype = c_otfontdata) AND ;
         uniqueid NOT IN (SELECT uniqueid FROM (m.g_scrnalias) ;
         WHERE platform = m.g_toplatform) ;
            AND oktransport(comment) ;
         ORDER BY objtype ;
         INTO CURSOR (m.junk)
   CASE m.g_grph2char
      SELECT * FROM (m.g_scrnalias) ;
         WHERE !DELETED() AND platform = m.g_fromplatform AND ;
         !(objtype = c_otband AND INLIST(objcode,2,6)) AND ;
         !(objtype = c_otpicture) AND ;
         !(objtype = c_otfontdata) AND ;
         !blackbox(objtype,fillred,fillblue,fillgreen,fillpat) AND ;
         uniqueid NOT IN (SELECT uniqueid FROM (m.g_scrnalias) ;
         WHERE platform = m.g_toplatform) ;
            AND oktransport(comment) ;
         INTO CURSOR (m.junk)
   CASE m.g_grph2grph
      SELECT * FROM (m.g_scrnalias) ;
         WHERE !DELETED() AND platform = m.g_fromplatform AND ;
         uniqueid NOT IN (SELECT uniqueid FROM (m.g_scrnalias) ;
         WHERE platform = m.g_toplatform) ;
            AND oktransport(comment) ;
         ORDER BY objtype ;
         INTO CURSOR (m.junk)
   ENDCASE
   IF _TALLY > 0
      SCAN
         DO addsel WITH "New"
      ENDSCAN
      USE  && discard the cursor
   ENDIF
ENDIF

IF m.g_tpselcnt > 0   && This variable is incremented in addsel()
   m.tpcancel = 1

   IF m.gAShowMe[m.g_tpFileIndx,1]
      * Prompt user to designate at any items he does not want transported
      DO tpselect WITH tparray, m.tpcancel,versioncap(m.g_fromplatform),versioncap(m.g_toplatform)
   ELSE
      m.tpcancel = 1   && pretend like the OK button was pressed
   ENDIF

   DO CASE
   CASE m.tpcancel = 1   && user pressed OK, so let's get to it.
   CASE m.tpcancel = 2   && user pressed "cancel" on the selection dialog.
      m.g_status = 3
      m.g_returncode = c_cancel
      RETURN TO transprt
   CASE m.tpcancel > 2
      * There aren't any objects that qualify for transporting.  User deselected all of them.
      * Pretend like we're done.
      m.g_status = 3
      m.g_returncode = c_yes
      RETURN TO transprt
   ENDCASE
ELSE
   * There aren't any objects that qualify for transporting.
   * Pretend like we're done.
   m.g_status = 3
   m.g_returncode = c_yes
   RETURN TO transprt
ENDIF

RETURN

*!*****************************************************************************
*!
*!      Procedure: INITSEL
*!
*!      Called by: SELECTOBJ          (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE initsel
* Initialize the tparray selection array
m.g_tpselcnt = 0
RETURN

*!*****************************************************************************
*!
*!      Procedure: ADDSEL
*!
*!      Called by: SELECTOBJ          (procedure in TRANSPRT.PRG)
*!
*!          Calls: ASSEMBLE()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE addsel
PARAMETER STATUS
* Don't use RECCOUNT() here since the open "database" will often be a cursor.
IF _WINDOWS OR _MAC
   m.g_tpselcnt = m.g_tpselcnt + 1
   DIMENSION tparray[m.g_tpselcnt,3]
   tparray[m.g_tpselcnt,1] = m.g_checkmark+' '+assemble(STATUS)
   tparray[m.g_tpselcnt,2] = uniqueid
   tparray[m.g_tpselcnt,3] = objtype

ELSE
   m.g_tpselcnt = m.g_tpselcnt + 1
   DIMENSION tparray[m.g_tpselcnt,3]
   tparray[m.g_tpselcnt,1] = m.g_checkmark+' '+assemble(STATUS)
   tparray[m.g_tpselcnt,2] = uniqueid
   tparray[m.g_tpselcnt,3] = objtype
ENDIF
RETURN

*!*****************************************************************************
*!
*!       Function: ISSELECTED
*!
*!*****************************************************************************
FUNCTION isselected
* Returns .T. if this uniqueid passed in idnum corresponds to an item
* marked on the tparray list.
PARAMETER idnum,mobjtype, mobjcode
DO CASE
CASE m.mobjtype = c_otfontdata
   RETURN .T.
OTHERWISE
   m.pos = ASCAN(tparray,m.idnum)
   IF m.pos > 0
      * Check pos-1 since this is a two dimensional array.  ASCAN returns an element number
      * but we are really interested in the column before the one that the match took place in.
      RETURN IIF(LEFT(tparray[m.pos-1],1) <> ' ',.T.,.F.)
   ELSE
      RETURN .F.
   ENDIF
ENDCASE

*!*****************************************************************************
*!
*!       Function: ASSEMBLE
*!
*!      Called by: ADDSEL             (procedure in TRANSPRT.PRG)
*!
*!          Calls: TYPE2NAME()        (function  in TRANSPRT.PRG)
*!               : CLEANPICT()        (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION assemble
* Form the string used for user selection of objects to transport
PARAMETER statstrg
PRIVATE m.strg
DO CASE
CASE INLIST(objtype,c_ottxtbut,c_otradbut,c_otchkbox)
   m.strg = PADR(statstrg,5);
      + PADR(type2name(objtype),15) ;
      + PADR(name,15) ;
      + PADR(cleanpict(PICTURE),30)
CASE objtype = c_otfield AND EMPTY(name)    && it's a SAY expression
   m.strg = PADR(statstrg,5);
      + PADR(type2name(objtype),15) ;
      + PADR(expr,45)
CASE INLIST(objtype,c_otbox,c_otline)
   DO CASE
   CASE m.g_char2grph OR m.g_grph2grph
      m.strg = PADR(statstrg,5);
         + PADR(type2name(objtype),15) ;
         + PADR("",15) ;
         + PADR("From "+ALLTRIM(STR(vpos,3))+","+ALLTRIM(STR(hpos,3))+" to " ;
         + ALLTRIM(STR(vpos+HEIGHT,3))+","+ALLTRIM(STR(hpos+WIDTH,3)),45)
   CASE m.g_grph2char
      m.strg = PADR(statstrg,5);
         + PADR(type2name(objtype),15) ;
         + PADR("",15) ;
         + PADR("At: " ;
         + ALLTRIM(STR(ROUND(cvtreportvertical(vpos),0),3));
         + ",";
         + ALLTRIM(STR(ROUND(cvtreportvertical(hpos),0),3));
         + ", Height: ";
         + ALLTRIM(STR(ROUND(cvtreportvertical(height),0),3));
         + ", Width: " ;
         + ALLTRIM(STR(ROUND(cvtreportvertical(width),0),3)),45)
   ENDCASE
OTHERWISE
   m.strg = PADR(statstrg,5);
      + PADR(type2name(objtype),15) ;
      + PADR(name,15) ;
      + PADR(expr,30)
ENDCASE

IF _WINDOWS OR _MAC
   RETURN LEFT(m.strg,5) + ansitooem(RIGHT(m.strg,LEN(m.strg)-5))
ELSE
   RETURN m.strg
ENDIF
*!*****************************************************************************
*!
*!       Function: TYPE2NAME
*!
*!      Called by: ASSEMBLE()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION type2name
PARAMETER N
PRIVATE strg
DO CASE
CASE m.n = c_otheader
   m.strg = "Header"
CASE INLIST(m.n,c_otworkar,c_otindex,c_otrel)
   m.strg = "Environment"
CASE m.n = c_ottext
   m.strg = "Text"
CASE m.n = c_otline
   m.strg = "Line"
CASE m.n = c_otbox
   m.strg = "Box"
CASE m.n = c_otrepfld
   m.strg = "Report field"
CASE m.n = c_otband
   m.strg = "Band"
CASE m.n = c_otgroup
   m.strg = "Group"
CASE m.n = c_otlist
   m.strg = "List"
CASE m.n = c_ottxtbut
   m.strg = "Push button"
CASE m.n = c_otradbut
   m.strg = "Radio button"
CASE m.n = c_otchkbox
   m.strg = "Check box"
CASE m.n = c_otfield
   DO CASE
   CASE EMPTY(name)
      IF !EMPTY(expr)
         m.strg = T_SEXPR_LOC
      ELSE
         m.strg = T_FIELD_LOC
      ENDIF
   CASE EMPTY(expr)
      m.strg = T_GFIELD_LOC
   OTHERWISE
      m.strg = T_FIELD_LOC
   ENDCASE
CASE m.n = c_otpopup
   m.strg = T_POPUP_LOC
CASE m.n = c_otpicture
   m.strg = "Picture"
CASE m.n = c_otrepvar
   m.strg = T_RPTVAR_LOC
CASE m.n = c_otinvbut
   m.strg = T_INVBTN_LOC
CASE m.n = c_otspinner
   m.strg = T_SPIN_LOC
CASE m.n = c_otpdset
   m.strg = T_PDRIVER_LOC 
CASE m.n = c_otfontdata
   m.strg = T_FONTDATA_LOC 
OTHERWISE
   m.strg = STR(objtype,4)
ENDCASE

RETURN m.strg


*!*****************************************************************************
*!
*!       Function: CLEANPICT
*!
*!      Called by: ASSEMBLE()         (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION cleanpict
PARAMETER m.strg
PRIVATE m.atsign

* Drop quotation marks
IF AT(LEFT(m.strg,1),CHR(34)+CHR(39)) > 0
   m.strg = SUBSTR(m.strg,2)
ENDIF
IF AT(RIGHT(m.strg,1),CHR(34)+CHR(39)) > 0
   m.strg = SUBSTR(m.strg,1,LEN(m.strg)-1)
ENDIF

m.atsign = AT("@",m.strg)
IF m.atsign > 0
   m.strg = LTRIM(SUBSTR(m.strg,m.atsign+AT(' ',SUBSTR(m.strg,m.atsign))))
ENDIF

IF LEN(m.strg) > 30
   m.strg = LEFT(m.strg,27) + '...'
ENDIF
RETURN m.strg


*!*****************************************************************************
*!
*!      Procedure: TPSELECT
*!
*!      Called by: SELECTOBJ          (procedure in TRANSPRT.PRG)
*!
*!          Calls: TOGGLE()           (function  in TRANSPRT.PRG)
*!               : OKVALID()          (function  in TRANSPRT.PRG)
*!               : WREADDEAC()        (function  in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE tpselect
PARAMETERS tparray, tpcancel, fromplat,toplat
DO CASE
CASE m.g_snippets AND m.g_newobjects
   ptext = T_OBJNEW1_LOC + m.toplat + T_OBJNEW2_LOC + m.fromplat+"."
CASE m.g_newobjects
   ptext = T_OBJNEW1_LOC + m.toplat + "."
CASE m.g_snippets 
   ptext = T_OBJMOD_LOC + m.fromplat + "."
ENDCASE

DO CASE
CASE _WINDOWS
   IF NOT WEXIST("tpselect")
      DEFINE WINDOW tpselect ;
         AT  0.000, 0.000  ;
         SIZE 25.538,116.000 ;
         TITLE T_TITLE_LOC  ;
         FONT m.g_smface, m.g_smsize ;
         FLOAT ;
         CLOSE ;
         NOMINIMIZE ;
         DOUBLE ;
         COLOR RGB(0,0,0,192,192,192)
      MOVE WINDOW tpselect CENTER
   ENDIF
   IF WVISIBLE("tpselect")
      ACTIVATE WINDOW tpselect SAME
   ELSE
      ACTIVATE WINDOW tpselect NOSHOW
   ENDIF
   @ 6.769,2.400 TO 8.154,113.000 ;
      PATTERN 1 ;
      PEN 1, 8 ;
      COLOR RGB(,,,192,192,192)
   @ 8.154,2.600 GET xsel ;
      PICTURE "@&N" ;
      FROM tparray ;
      SIZE 17.500,68.875 ;
      DEFAULT 1 ;
      FONT m.g_foxfont, m.g_foxfsize ;
      VALID toggle()
   @ 1.462,3.000 SAY ptext ;
      SIZE 4.000,33.833 ;
      FONT m.g_smface, m.g_smsize ;
      STYLE "B"
   @ 1.462,50.400 SAY T_UNCHECK1_LOC  ;
      SIZE 1.000,28.000, 0.000 ;
      FONT m.g_smface, m.g_smsize ;
      STYLE "BT"
   @ 2.385,50.200 SAY T_UNCHECK2_LOC ;
      SIZE 1.000,4.167, 0.000 ;
      FONT m.g_smface, m.g_smsize ;
      STYLE "BIT"
   @ 2.385,55.000 SAY T_UNCHECK3_LOC  ;
      SIZE 1.000,27.000, 0.000 ;
      FONT m.g_smface, m.g_smsize ;
      STYLE "BT"
   @ 0.923,93.600 GET tpcancel ;
      PICTURE "@*VT \!\<"+T_OK_LOC+";\?\<"+T_CANCEL_LOC ;
      SIZE 1.846,16.333,0.308 ;
      DEFAULT 1 ;
      FONT m.g_tdlgface, m.g_tdlgsize ;
      STYLE m.g_tdlgstyle ;
      VALID okvalid()
   @ 6.923,5.800 SAY T_STAT_LOC  ;
      SIZE 1.000,5.000, 0.000 ;
      FONT m.g_smface, m.g_smsize ;
      STYLE "BT"
   @ 6.923,14.000 SAY T_TYPE_LOC  ;
      SIZE 1.000,6.000, 0.000 ;
      FONT m.g_smface, m.g_smsize ;
      STYLE "BT"
   @ 6.923,38.200 SAY T_VARIABLE_LOC  ;
      SIZE 1.000,10.000, 0.000 ;
      FONT m.g_smface, m.g_smsize ;
      STYLE "BT"
   @ 6.923,62.000 SAY T_EXPPROMPT_LOC  ;
      SIZE 1.000,25.000, 0.000 ;
      FONT m.g_smface, m.g_smsize ;
      STYLE "BT"

   IF NOT WVISIBLE("tpselect")
      ACTIVATE WINDOW tpselect
   ENDIF

   READ CYCLE;
      MODAL;
      DEACTIVATE wreaddeac()

   RELEASE WINDOW tpselect
CASE _MAC
   IF NOT WEXIST("tpselect")
      DEFINE WINDOW tpselect ;
         AT  0.000, 0.000  ;
         SIZE 25.538,100.000 ;
         TITLE T_TITLE_LOC  ;
         FONT "Geneva",9 ;
			STYLE "" ;
         FLOAT ;
         CLOSE ;
         NOMINIMIZE ;
         DOUBLE
      MOVE WINDOW tpselect CENTER
   ENDIF
   IF WVISIBLE("tpselect")
      ACTIVATE WINDOW tpselect SAME
   ELSE
      ACTIVATE WINDOW tpselect NOSHOW
   ENDIF
   *@ 6.769,2.400 TO 8.154,97.800 ;
   *   PATTERN 1 ;
   *   PEN 1, 8 ;
   *   COLOR RGB(,,,192,192,192)
   @ 8.154,2.600 GET xsel ;
      PICTURE "@&N" ;
      FROM tparray ;
      SIZE 16.000,78.875 ;
      DEFAULT 1 ;
      FONT m.g_foxfont, m.g_foxfsize ;
      VALID toggle()
   @ 1.462,3.000 SAY ptext ;
      SIZE 4.000,33.833 ;
      FONT "Geneva", 9 ;
      STYLE m.g_smsty1
   @ 1.462,50.400 SAY T_UNCHECK1_LOC  ;
      SIZE 1.000,28.000, 0.000 ;
      FONT "Geneva", 9 ;
      STYLE ""
   @ 2.385,50.200 SAY T_UNCHECK2_LOC  ;
      SIZE 1.000,4.167, 0.000 ;
      FONT "Geneva", 9 ;
      STYLE ""+"I"
   @ 2.385,54.000 SAY T_UNCHECK3_LOC  ;
      SIZE 1.000,27.000, 0.000 ;
      FONT "Geneva", 9 ;
      STYLE ""
   @ 0.923,83.600 GET tpcancel ;
      PICTURE "@*VT \!\<"+T_OK_LOC+";\?\<"+T_CANCEL_LOC ;
      SIZE m.g_tdlgbtn,10.000,0.500 ;
      DEFAULT 1 ;
      FONT m.g_tdlgface, m.g_tdlgsize ;
      STYLE m.g_tdlgstyle ;
      VALID okvalid()
   @ 6.923,5.550 SAY T_STAT_LOC  ;
      SIZE 1.000,5.000, 0.000 ;
      FONT "Geneva", 9 ;
      STYLE "TB"
   @ 6.923,11.500 SAY T_TYPE_LOC  ;
      SIZE 1.000,5.500, 0.000 ;
      FONT "Geneva", 9 ;
      STYLE "TB"
   @ 6.923,29.200 SAY T_VARIABLE_LOC  ;
      SIZE 1.000,10.000, 0.000 ;
      FONT "Geneva", 9 ;
      STYLE "TB"
   @ 6.923,47.500 SAY T_EXPPROMPT_LOC  ;
      SIZE 1.000,25.000, 0.000 ;
      FONT "Geneva", 9 ;
      STYLE "TB"

   IF NOT WVISIBLE("tpselect")
      ACTIVATE WINDOW tpselect
   ENDIF

   READ CYCLE;
      MODAL;
      DEACTIVATE wreaddeac()

   RELEASE WINDOW tpselect
CASE _DOS
   IF NOT WEXIST("tpselect")
      DEFINE WINDOW tpselect ;
         FROM INT((SROW()-23)/2),INT((SCOL()-77)/2) ;
         TO INT((SROW()-23)/2)+22,INT((SCOL()-77)/2)+76 ;
         TITLE T_TITLE_LOC  ;
         FLOAT ;
         CLOSE ;
         NOMINIMIZE ;
         DOUBLE ;
         COLOR SCHEME 5
   ENDIF
   IF WVISIBLE("tpselect")
      ACTIVATE WINDOW tpselect SAME
   ELSE
      ACTIVATE WINDOW tpselect NOSHOW
   ENDIF
   @ 0,0 CLEAR
   @ 8,1 GET xsel ;
      PICTURE "@&N" ;
      FROM tparray ;
      SIZE 13,72 ;
      DEFAULT 1 ;
      VALID toggle() ;
      COLOR SCHEME 6
   @ 1,30 SAY T_UNCHECK1_LOC  ;
      SIZE 1,24, 0
   @ 2,30 SAY T_UNCHECK2_LOC  ;
      SIZE 1,3, 0
   @ 2,34 SAY T_UNCHECK3_LOC  ;
      SIZE 1,23, 0
   @ 1,62 GET tpcancel ;
      PICTURE "@*VT \!\<"+T_OK_LOC+";\?\<"+T_CANCEL_LOC ;
      SIZE 1,10,0 ;
      DEFAULT 1 ;
      VALID okvalid()
   @ 7,10 SAY T_TYPE_LOC  ;
      SIZE 1,4, 0
   @ 7,40 SAY T_EXPPROMPT_LOC  ;
      SIZE 1,17, 0
   @ 7,25 SAY T_VARIABLE_LOC  ;
      SIZE 1,8, 0
   @ 7,5 SAY T_STAT_LOC  ;
      SIZE 1,4, 0
   @ 1,2 SAY ptext ;
      SIZE 5,26

   IF NOT WVISIBLE("tpselect")
      ACTIVATE WINDOW tpselect
   ENDIF

   READ CYCLE ;
      MODAL ;
      DEACTIVATE wreaddeac()

   RELEASE WINDOW tpselect
ENDCASE

*!*****************************************************************************
*!
*!       Function: TOGGLE
*!
*!      Called by: TPSELECT           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION toggle
* Toggle mark
IF LEFT(tparray[xsel,1],1) <> ' '
   tparray[xsel,1] = STUFF(tparray[xsel,1],1,1,' ')
ELSE
   tparray[xsel,1] = STUFF(tparray[xsel,1],1,1,m.g_checkmark)
ENDIF
SHOW GETS
RETURN .F.

*!*****************************************************************************
*!
*!       Function: OKVALID
*!
*!      Called by: TPSELECT           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION okvalid
* Simulate a cancel if no objects were selected.
IF tpcancel = 1
   PRIVATE m.i
   m.cnt = 0
   FOR m.i = 1 TO m.g_tpselcnt
      IF LEFT(tparray[m.i,1],1) <> ' '
         m.cnt = m.cnt + 1
      ENDIF
   ENDFOR
   IF m.cnt = 0
      m.tpcancel = 3   && code that means, "just open as is."
   ENDIF
ENDIF

*!*****************************************************************************
*!
*!       Function: WREADDEAC
*!
*!      Called by: TPSELECT           (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
FUNCTION wreaddeac
*
* Deactivate Code from screen: TP
*
CLEAR READ

*!*****************************************************************************
*!
*!       Function: EnvSelect
*!
*!*****************************************************************************
FUNCTION EnvSelect
PRIVATE m.i
* Was an environment record selected for transport?
FOR m.i = 1 TO m.g_tpselcnt
   IF IsEnviron(tparray[m.i,3]) AND LEFT(tparray[m.i,1],1) <> " "
      RETURN .T.
   ENDIF
ENDFOR
RETURN .F.

*!*****************************************************************************
*!
*!       Function: OutputOrd
*!
*!*****************************************************************************
FUNCTION outputord
PARAMETER m.otype, m.rno
* Function to sort screen and report files.  We want the header and environment
* records to be at the "top" of the platform, and other records to be in their
* original order.
IF objtype <= 4
   RETURN STR(m.otype,3)+STR(m.rno,3)
ELSE
   RETURN STR(m.rno,3)+STR(m.otype,3)
ENDIF

*!*****************************************************************************
*!
*!       Procedure: PUTWINMSG
*!
*!*****************************************************************************
PROCEDURE putwinmsg
PARAMETER m.msg
IF _WINDOWS OR _MAC
   SET MESSAGE TO m.msg
ENDIF

*
* SETALL - Create program's environment.
*
* Description:
* Save the user's environment that is being modified by the GENSCRN,
* then issue various SET commands.
*
*!*****************************************************************************
*!
*!      Procedure: SETALL
*!
*!      Called by: TRANSPRT.PRG
*!
*!          Calls: ESCHANDLER         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE setall

*-CLEAR PROGRAM
CLEAR GETS

m.escape = SET("ESCAPE")
*SET ESCAPE ON

*m.onescape = ON("ESCAPE")
*ON ESCAPE DO eschandler

*SET ESCAPE OFF
m.trbetween = SET("TRBET")
SET TRBET OFF
m.comp = SET("COMPATIBLE")
SET COMPATIBLE FOXPLUS
m.device = SET("DEVICE")
SET DEVICE TO SCREEN

m.rbord = SET("READBORDER")
SET READBORDER ON

m.status = SET("STATUS")
*SET STATUS OFF

m.currarea = SELECT()

m.udfparms = SET('UDFPARMS')
SET UDFPARMS TO VALUE

m.mtopic = SET("TOPIC")
IF SET("HELP") = "ON"
   DO CASE
   CASE ATC(".DBF",SET("HELP",1)) > 0
      SET TOPIC TO CHR(254)+" Transporter"
      ON KEY LABEL F1 HELP þ Transporter
   CASE ATC(".HLP",SET("HELP",1)) > 0
      SET TOPIC TO Transporter Dialog
      ON KEY LABEL F1 HELP Transporter Dialog
   ENDCASE
ENDIF

m.mfieldsto = SET("FIELDS",1)
m.fields = SET("FIELDS")
SET FIELDS TO
SET FIELDS OFF

m.memowidth = SET("MEMOWIDTH")
SET MEMOWIDTH TO 256

m.cursor = SET("CURSOR")
SET CURSOR OFF

m.consol = SET("CONSOLE")
SET CONSOLE OFF

m.bell = SET("BELL")
SET BELL OFF

m.exact = SET("EXACT")
SET EXACT ON

m.deci = SET("DECIMALS")
SET DECIMALS TO 10

m.fixed = SET("FIXED")
SET FIXED ON

m.print = SET("PRINT")
SET PRINT OFF

m.unqset = SET("UNIQUE")
SET UNIQUE OFF

m.safety = SET("SAFETY")
SET SAFETY OFF

m.exclusive = SET("EXCLUSIVE")
SET EXCLUSIVE ON

IF versnum() > "2.5"
   m.mcollate = SET("COLLATE")
   SET COLLATE TO "machine"
ENDIF

#if "MAC" $ UPPER(VERSION(1))
   IF _MAC
      m.mmacdesk = SET("MACDESKTOP")
      SET MACDESKTOP ON
   ENDIF
#endif

*
* CLEANUP - Restore environment to pre-execution state.
*
* Description:
* Put SET command settings back the way we found them.
*
*!*****************************************************************************
*!
*!      Procedure: CLEANUP
*!
*!      Called by: TRANSPRT.PRG
*!               : ERRORHANDLER       (procedure in TRANSPRT.PRG)
*!               : CONVERTTYPE()      (function  in TRANSPRT.PRG)
*!               : ESCHANDLER         (procedure in TRANSPRT.PRG)
*!
*!          Calls: WRITERESULT        (procedure in TRANSPRT.PRG)
*!               : DEACTTHERM         (procedure in TRANSPRT.PRG)
*!
*!*****************************************************************************
PROCEDURE cleanup

PARAMETER m.cancafter
IF PARAMETERS() = 0
   m.cancafter = .F.
ENDIF
IF NOT EMPTY(m.g_20alias)
   IF m.g_status != 0
      IF USED(m.g_tempalias)
         SELECT (m.g_tempalias)
         USE
      ENDIF
      IF USED(m.g_fromobjonlyalias)
         SELECT (m.g_fromobjonlyalias)
         USE
      ENDIF
      IF USED(m.g_boxeditemsalias)
         SELECT (m.g_boxeditemsalias)
         USE
      ENDIF
      SELECT (m.g_20alias)
      USE
      SELECT (m.g_scrnalias)
   ELSE
      DO writeresult   && updates thermometer too
   ENDIF
ENDIF

ON ERROR &onerror
*ON ESCAPE &onescape

IF m.consol = "ON"
   SET CONSOLE ON
ELSE
   SET CONSOLE OFF
ENDIF

IF m.escape = "ON"
   SET ESCAPE ON
ELSE
   SET ESCAPE OFF
ENDIF

IF m.bell = "ON"
   SET BELL ON
ELSE
   SET BELL OFF
ENDIF

SET FIELDS TO &mfieldsto
IF m.fields = "ON"
   	SET FIELDS ON
ELSE
   	SET FIELDS OFF
ENDIF

IF m.exact = "ON"
   SET EXACT ON
ELSE
   SET EXACT OFF
ENDIF

IF m.comp = "ON"
   SET COMPATIBLE ON
ENDIF

IF m.print = "ON"
   SET PRINT ON
ENDIF

IF m.fixed = "OFF"
   SET FIXED OFF
ENDIF

IF m.trbetween = "ON"
   SET TRBET ON
ENDIF

IF m.unqset = "ON"
   SET UNIQUE ON
ENDIF

IF m.rbord = "OFF"
   SET READBORDER OFF
ENDIF

IF m.status = "ON"
   SET STATUS ON
ENDIF

SET DECIMALS TO m.deci
SET MEMOWIDTH TO m.memowidth
SET DEVICE TO &device
SET UDFPARMS TO &udfparms
SET TOPIC TO &mtopic

IF versnum() > "2.5"
   SET COLLATE TO "&mcollate"
ENDIF

#if "MAC" $ UPPER(VERSION(1))
   IF _MAC
      SET MACDESKTOP &mmacdesk
	ENDIF
#endif

ON KEY LABEL F1
POP KEY

USE
DELETE FILE (m.g_tempindex)
SET MESSAGE TO

SELECT (m.currarea)

DO deacttherm

IF m.cursor = "ON"
   SET CURSOR ON
ELSE
   SET CURSOR OFF
ENDIF

IF m.safety = "ON"
   SET SAFETY ON
ENDIF

IF m.talkset = "ON"
   SET TALK ON
ENDIF

IF m.exclusive = "ON"
   SET EXCLUSIVE ON
ELSE
   SET EXCLUSIVE OFF
ENDIF
IF m.talkset = "ON"
   SET TALK ON
ENDIF

IF m.cancafter
   *- CANCEL
ENDIF

*
* WRITERESULT - Writes the converted cursor to the SCX/FRX/LBX/whatever.  The point of this is that we
*      need to write the records in their original order so we don't mees up any groups.  We also need
*      to keep records for a given platform contiguous.
*
*!*****************************************************************************
*!
*!      Procedure: WRITERESULT
*!
*!      Called by: CLEANUP            (procedure in TRANSPRT.PRG)
*!
*!          Calls: DOCREATE           (procedure in TRANSPRT.PRG)
*!               : UPDTHERM           (procedure in TRANSPRT.PRG)
*!
*!           Uses: M.G_SCRNALIAS
*!
*!        Indexes: TEMP                   (tag)
*!
*!*****************************************************************************
PROCEDURE writeresult
PRIVATE m.platforms, m.loop, m.thermstep

IF g_filetype = c_project
   SELECT (m.g_20alias)        && Close the database so we can replace it.
   USE

   SELECT (m.g_scrnalias)      && Copy the temporary cursor to the database and
   COPY TO (m.g_scrndbf)       &&      get rid of the cursor
   USE
   DO updtherm WITH 100
ELSE
   REPLACE ALL platform WITH UPPER(platform)

   * Get a list of the platforms we need to write.
   SELECT DISTINCT platform ;
      FROM (m.g_scrnalias) ;
      WHERE !DELETED() ;
      INTO ARRAY plist
   m.platforms = _TALLY

   * The following select creates a new cursor with the desired structure.  We write
   * into this and then dump the cursor to disk.  It's a bit cumbersome, but reduces
   * the chances of frying the original file.
   m.g_tempalias = "S"+SUBSTR(LOWER(SYS(3)),2,8)
   DO docreate WITH m.g_tempalias, m.g_filetype

   * We need to write DOS/UNIX label records in the order we want the objects to appear.
   * So, we create this index and set order to it when we want to write those records.
   IF m.g_filetype = c_label
      SELECT (m.g_scrnalias)
      INDEX ON platform + ;
         IIF(objtype = c_ot20label,CHR(1)+CHR(1), STR(objtype,2)) + ;
         STR(objcode,2) + ;
         STR(vpos,3) TAG temp
   ENDIF

   IF m.g_updenviron
      SELECT (m.g_scrnalias)
      INDEX ON outputord(objtype,recno()) TAG temp1
   ENDIF

   SELECT (m.g_scrnalias)
   IF RECCOUNT() > 0
      m.thermstep = (100 - m.g_mercury)/RECCOUNT()
   ELSE
      m.thermstep = 0
   ENDIF

   * Write the records for each platform.
   FOR m.loop = 1 TO m.platforms
      SELECT (m.g_scrnalias)

      DO CASE
      CASE m.g_filetype = c_label
         SET ORDER TO TAG temp
      CASE m.g_updenviron
         SET ORDER TO TAG temp1
      OTHERWISE
         SET ORDER TO
      ENDCASE

      SCAN FOR platform = plist[m.loop] AND !DELETED()
         SCATTER MEMVAR MEMO
         SELECT (m.g_tempalias)
         APPEND BLANK
         GATHER MEMVAR MEMO
         SELECT (m.g_scrnalias)

         m.g_mercury = MIN(m.g_mercury + m.thermstep, 100)
         DO updtherm WITH m.g_mercury
      ENDSCAN
   ENDFOR

   SELECT (m.g_20alias)        && Close the database so we can replace it.
   USE

   SELECT (m.g_tempalias)      && Copy the temporary cursor to the database and
   COPY TO (m.g_scrndbf)       &&      get rid of the cursor
   USE

   SELECT (m.g_scrnalias)      && Get rid of the master cursor
   USE

   DO updtherm WITH 100
ENDIF
*!*****************************************************************************
*!
*!      Function: VERSNUM
*!
*!*****************************************************************************
FUNCTION versnum
* Return string corresponding to FoxPro version number
RETURN wordnum(vers(),2)

*!*****************************************************************************
*!
*!      Function: CPTRANS
*!
*!*****************************************************************************
FUNCTION cptrans
* Translate from one codepage to another, if translation is in effect.  Note that
* this function takes parameters in a different order than CPCONVERT.
PARAMETER m.tocp, m.fromcp, m.strg
IF c_cptrans AND versnum() > "2.5"
   RETURN CPCONVERT(m.fromcp, m.tocp, m.strg)
ELSE
   RETURN m.strg
ENDIF
*!*****************************************************************************
*!
*!      Function: CPTCOND
*!
*!*****************************************************************************
FUNCTION cptcond
* Conditionally translate from one codepage to another, if translation is in effect.
* Note that this function takes parameters in a different order than CPCONVERT.
* Only translate if the current database isn't already the tocp.
PARAMETER m.tocp, m.fromcp, m.strg
IF c_cptrans AND cpdbf() <> m.tocp AND versnum() > "2.5"
   RETURN CPCONVERT(m.fromcp, m.tocp, m.strg)
ELSE
   RETURN m.strg
ENDIF

*!*****************************************************************************
*!
*!      Function: setfromcp
*!
*!*****************************************************************************
FUNCTION setfromcp
PARAMETER m.plat
DO CASE
CASE m.plat = c_dosname
   RETURN c_doscp
CASE m.plat = c_winname
   RETURN c_wincp
CASE m.plat = c_macname
   RETURN c_maccp
CASE m.plat = c_unixname
   RETURN c_unixcp
OTHERWISE
   RETURN c_doscp
ENDCASE

*!*****************************************************************************
*!
*!      Function: oktransport
*!
*!*****************************************************************************
FUNCTION oktransport
PARAMETER strg
DIMENSION plat_arry[4]
plat_arry = 0
IF ATC("#DOSOBJ",m.strg) > 0
   plat_arry[dos_code] = 1
ENDIF
IF ATC("#WINOBJ",m.strg) > 0
   plat_arry[win_code] = 1
ENDIF
IF ATC("#MACOBJ",m.strg) > 0
   plat_arry[mac_code] = 1
ENDIF
IF ATC("#UNIXOBJ",m.strg) > 0
   plat_arry[unix_code] = 1
ENDIF

* If no platform-specific designations found, transport anywhere
IF plat_arry[1] + plat_arry[2] + plat_arry[3] + plat_arry[4] = 0
   plat_arry = 1
ENDIF

DO CASE
CASE m.g_toplatform = c_dosname
   RETURN IIF(plat_arry[dos_code] = 1, .T.,.F.)
CASE m.g_toplatform = c_winname
   RETURN IIF(plat_arry[win_code] = 1, .T.,.F.)
CASE m.g_toplatform = c_macname
   RETURN IIF(plat_arry[mac_code] = 1, .T.,.F.)
CASE m.g_toplatform = c_unixname
   RETURN IIF(plat_arry[unix_code] = 1, .T.,.F.)
ENDCASE

*!*****************************************************************************
*!
*!      Function: iserrormsg
*!
*!*****************************************************************************
FUNCTION iserrormsg
PARAMETER m.strg
* Was this an error message that the Mac RW added to a report file that
* didn't have any Windows records?  If so, don't transport it.
RETURN IIF(ATC("** ERROR", UPPER(m.strg)) > 0, .T., .F.)

*!*****************************************************************************
*!
*!      Function: boxjoin
*!
*!*****************************************************************************
FUNCTION boxjoin
PARAMETERS m.otype, m.rnum, m.pform
* Is this text object in a box group and thus boxjoin?
PRIVATE m.in_rec, m.retval, m.objpos
m.retval = .F.
IF m.otype = c_ottext
   m.in_rec = RECNO()

   * Get object position (position in linked list of objects) of current record
   m.objpos = GetObjPos(m.rnum, m.pform)
   IF m.objpos > 0
      * Look at all the box groups
      GOTO TOP
      SCAN FOR m.pform == platform AND objtype = c_otgroup AND objcode = 1 AND !m.retval
         * hpos has the starting object number for this group, vpos has the number of
         * objects the group includes.
         IF m.objpos >= hpos AND m.objpos <= hpos + vpos - 1
            m.retval = .T.
         ENDIF
      ENDSCAN
   ENDIF
   GOTO m.in_rec
ENDIF
RETURN m.retval

*!*****************************************************************************
*!
*!      Function: GetObjPos
*!
*!*****************************************************************************
FUNCTION getobjpos
PARAMETERS m.rnum, m.pform
PRIVATE m.objcount, m.retval

* Get ordinal number of this object
m.objcount = 0
m.retval = 0
SCAN FOR m.pform == platform AND INLIST(objtype,C_OBJTYPELIST)
   m.objcount = m.objcount + 1
   IF RECNO() = m.rnum
      m.retval = m.objcount
   ENDIF
ENDSCAN
RETURN m.retval

*!*****************************************************************************
*!
*!      Procedure: InitFontMap
*!
*!*****************************************************************************
PROCEDURE initfontmap
* Initialize font mapping array.  Windows font characteristics are in the
* first three columns, Mac in the next three.  These functions are used
* mainly to map text fields and static text.
PRIVATE m.i

*****************************************************************************
* Font characteristic table for some common fonts (from FontMetric()):
*
*                     8     8B     9     9B     10     10B     12
*                ---------------------------------------------------
* Geneva	         	4x11   5x11  5x12   6x12	6x13	 7x13    7x16
* Chicago         	4x11   5x11  5x12   6x12	6x13	 7x13    7x16
* MS Sans Serif      5x13   6x13	 5x13   6x13   7x16   8x16    8x20
* Arial              5x14   5x14  5x15   6x15   6x16   6x16    8x19
* FoxFont            7x9    8x9   8x12   9x12   8x12   9x12    8x12
* Courier New        7x14   7x14  7x15   7x16   8x16   8x16    10x18
*****************************************************************************

g_fontmap[1,1] = "MS Sans Serif"
g_fontmap[1,2] = 8
g_fontmap[1,3] = "B"
g_fontmap[1,4] = "Geneva"
g_fontmap[1,5] = 10
g_fontmap[1,6] = ""

g_fontmap[2,1] = "MS Sans Serif"
g_fontmap[2,2] = 8
g_fontmap[2,3] = ""
g_fontmap[2,4] = "Geneva"
g_fontmap[2,5] = 9
g_fontmap[2,6] = ""

g_fontmap[3,1] = "Courier New"
g_fontmap[3,2] = 0    && wildcard
g_fontmap[3,3] = "*"  && wildcard
g_fontmap[3,4] = "Courier"
g_fontmap[3,5] = 0
g_fontmap[3,6] = "*"

FOR m.i = 1 TO ALEN(g_fontmap,1)
   g_fontmap[m.i,1] = UPPER(ALLTRIM(g_fontmap[m.i,1]))
   g_fontmap[m.i,3] = UPPER(ALLTRIM(g_fontmap[m.i,3]))
   g_fontmap[m.i,4] = UPPER(ALLTRIM(g_fontmap[m.i,4]))
   g_fontmap[m.i,6] = UPPER(ALLTRIM(g_fontmap[m.i,6]))
ENDFOR
*!*****************************************************************************
*!
*!      Procedure: MapFont
*!
*!*****************************************************************************
PROCEDURE mapfont
PARAMETER m.inface, m.insize, m.instyle, m.outface, m.outsize, m.outstyle, m.win2mac
PRIVATE m.i, m.asterisk, m.aoff   && array offset

m.asterisk = "*"
m.aoff = IIF(m.win2mac,0,3)
FOR m.i = 1 TO ALEN(g_fontmap,1)
   IF g_fontmap[m.i,1+m.aoff] == UPPER(ALLTRIM(m.inface)) ;
         AND INLIST(g_fontmap[m.i,2+m.aoff],m.insize,0) ;
         AND INLIST(g_fontmap[m.i,3+m.aoff],UPPER(ALLTRIM(m.instyle)),m.asterisk)
      m.outface  = g_fontmap[m.i,4-m.aoff]

      IF g_fontmap[m.i,2+m.aoff] = 0   && wildcard match on size?
         m.outsize  = m.insize
      ELSE
         m.outsize  = g_fontmap[m.i,5-m.aoff]
      ENDIF

      IF g_fontmap[m.i,6-m.aoff] = m.asterisk   && wildcard match on style?
         m.outstyle = m.instyle
      ELSE
         m.outstyle = g_fontmap[m.i,6-m.aoff]
      ENDIF
      RETURN
   ENDIF
ENDFOR
* Let the operating system handle the font mapping
m.outface = m.inface
m.outsize = m.insize
m.outstyle = m.instyle
RETURN

*!*****************************************************************************
*!
*!      Procedure: REPLFONT
*!
*!*****************************************************************************
PROCEDURE replfont
PRIVATE m.theface, m.thesize, m.thestyle
* Replace the current font with a mapped one, if one matches
m.theface = ""
m.thesize = 0
m.thestyle = ""
DO mapfont WITH fontface, fontsize, num2style(fontstyle), ;
   m.theface, m.thesize, m.thestyle, _MAC
IF !EMPTY(m.theface)
   REPLACE fontface WITH m.theface, fontsize WITH m.thesize, ;
       fontstyle WITH style2num(m.thestyle)
ENDIF

*!*****************************************************************************
*!
*!      Procedure: MAKE2D
*!
*!*****************************************************************************
FUNCTION make2d
* Add a 2 to the control portion of the picture string
PARAMETER m.strg
m.strg = TRIM(m.strg)
PRIVATE m.sp_pos, m.ctrl

m.sp_pos = AT(" ",strg)
DO CASE
CASE m.sp_pos > 0 AND AT('@', m.strg) > 0
   m.ctrl = LEFT(m.strg, m.sp_pos - 1)
	IF AT(c_2dmark,m.ctrl) = 0
	   m.ctrl = m.ctrl + c_2dmark
   	m.strg = m.ctrl + SUBSTR(m.strg, m.sp_pos)
	ENDIF
CASE EMPTY(m.strg)
   m.strg = "@" + c_2dmark
CASE AT(c_2dmark,strg) = 0
	IF isquote(RIGHT(m.strg,1))
	   IF SUBSTR(m.strg,2,1) = "@"
		   * Something like "@!".  Make it "@!2"
         m.strg = SUBSTR(m.strg, 1, LEN(m.strg) - 1) + c_2dmark + RIGHT(m.strg,1)
		ELSE
		   * Something like "!!!".  Make it "@2 !!!"
         m.strg = SUBSTR(m.strg, 1, 1) + "@" + c_2dmark + " "+SUBSTR(m.strg,2)
		ENDIF
 	ELSE
	   IF SUBSTR(m.strg,2,1) = "@"
		   * Something like @!.  Make it @!2
         m.strg = m.strg + c_2dmark
		ELSE
		   * Something like !!!.  Make it @2 !!!
         m.strg =  "@" + c_2dmark + " " + m.strg
		ENDIF
	ENDIF
ENDCASE
RETURN m.strg

*!*****************************************************************************
*!
*!      Procedure: MAKE3D
*!
*!*****************************************************************************
FUNCTION make3d
* Add a 3 to the control portion of the picture string
PARAMETER m.strg
m.strg = TRIM(m.strg)
PRIVATE m.sp_pos, m.ctrl

m.sp_pos = AT(" ",strg)
DO CASE
CASE m.sp_pos > 0 AND AT('@', m.strg) > 0
   m.ctrl = LEFT(m.strg, m.sp_pos - 1)
	IF AT(c_3dmark,m.ctrl) = 0
	   m.ctrl = m.ctrl + c_3dmark
   	m.strg = m.ctrl + SUBSTR(m.strg, m.sp_pos)
	ENDIF
CASE EMPTY(m.strg)
   m.strg = "@" + c_3dmark
CASE AT(c_3dmark,strg) = 0
	IF isquote(RIGHT(m.strg,1))
	   IF SUBSTR(m.strg,2,1) = "@"
		   * Something like "@!".  Make it "@!3"
         m.strg = SUBSTR(m.strg, 1, LEN(m.strg) - 1) + c_3dmark + RIGHT(m.strg,1)
		ELSE
		   * Something like "!!!".  Make it "@3 !!!"
         m.strg = SUBSTR(m.strg, 1, 1) + "@" + c_3dmark + " "+SUBSTR(m.strg,2)
		ENDIF
 	ELSE
	   IF SUBSTR(m.strg,2,1) = "@"
		   * Something like @!.  Make it @!3
         m.strg = m.strg + c_3dmark
		ELSE
		   * Something like !!!.  Make it @3 !!!
         m.strg =  "@" + c_3dmark + " " + m.strg
		ENDIF
	ENDIF
ENDCASE
RETURN m.strg

*!*****************************************************************************
*!
*!      Function: ADDQUOTE
*!
*!*****************************************************************************
FUNCTION addquote
PARAMETER m.strg
* Add quotes if they aren't already there
IF !INLIST(LEFT(m.strg,1) , CHR(34) , CHR(39) , '[')
	DO CASE
	CASE AT('"', m.strg) = 0
   	m.strg = '"' + m.strg + '"'
	CASE AT("'", m.strg) = 0
   	m.strg = "'" + m.strg + "'"
   CASE AT('[', m.strg) = 0 AND AT(']', m.strg) = 0
		m.strg = '[' + m.strg + ']'
	OTHERWISE
	   * Take our best shot
   	m.strg = '"' + m.strg + '"'
	ENDCASE
ENDIF
RETURN m.strg
*!*****************************************************************************
*!
*!      Function: ISQUOTE
*!
*!*****************************************************************************
FUNCTION isquote
PARAMETER m.char
IF INLIST(m.char,CHR(34),CHR(39))
   RETURN .T.
ELSE
   RETURN .F.
ENDIF

*!*****************************************************************************
*!
*!      Procedure: FONTAVAIL
*!
*!*****************************************************************************
FUNCTION fontavail
PARAMETER m.thefont
m.thefont = UPPER(ALLTRIM(m.thefont))
IF ASCAN(g_fontavail, m.thefont) > 0
   RETURN .T.
ELSE
   RETURN .F.
ENDIF

*!*****************************************************************************
*!
*!      Procedure: FIXPEN
*!
*!*****************************************************************************
PROCEDURE fixpen
* Make sure that the pen_color fields don't overflow.  A bug in the beta
* version of FoxPro 2.5 sometimes caused this to happen.  It was corrected
* prior to release.
IF penred > 65536
   REPLACE penred WITH 0
ENDIF
IF pengreen > 65536
   REPLACE pengreen WITH 0
ENDIF
IF penblue > 65536
   REPLACE penblue WITH 0
ENDIF

*!*****************************************************************************
*!
*!      Procedure: ASSERT
*!
*!*****************************************************************************
PROCEDURE assert
PARAMETER condition, strg
IF debugversion
   IF !condition
      WAIT WINDOW T_ASSERTFAIL_LOC+strg
   ENDIF
ENDIF
*: EOF: TRANSPRT.PRG
