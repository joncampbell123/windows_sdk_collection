*!*****************************************************************************
*!
*!     Function: MIGDB4.PRG
*!     (c) Microsoft Corp. 1994, 1995
*!
*!     Migrates DB-IV LBL, FRM and SCR files to FoxPro equivalents
*!
*!     Parameter:	csrcfile	C	fully qualified file to convert
*!					oConv		O	ForeignConverter object
*!
*!     Returns .T. if successful
*!             .F. if failed
*!
*!     changes:  modified version of FP 2.6 code
*!
*!*****************************************************************************
*-FUNCTION migdb4
PARAMETER m.csrcfile, m.oConv

#INCLUDE "convert.h"
#INCLUDE "migdb4.h"


PRIVATE tempstr
PRIVATE cFmtConverterClass
cFmtConverterClass = "FmtConverter"

PRIVATE oldexact
m.oldexact = SET("EXACT")
SET EXACT ON      && For ascan()

PRIVATE oldalias
m.oldalias = ALIAS()

PRIVATE m.fhandle, m.fsize
m.fhandle = -1
IF TYPE("m.csrcfile") <> "C"
	*- no parm passed
	SET EXACT &oldexact
	RETURN .F.
ENDIF

m.fhandle = FOPEN(m.csrcfile, 2)
IF m.fhandle = -1
	=falldown(E_NOOPEN_LOC + JustFName(UPPER(m.csrcfile)))
	SET EXACT &oldexact
	RETURN .F.
ENDIF

*- get file size, and return pointer to top of file
m.fsize = FSEEK(m.fhandle,0,2)
=FSEEK(m.fhandle,0)

PRIVATE m.rootname,m.cpathname, m.lretval
m.rootname = JustStem(m.csrcfile)

*- include original path for saving migrated forms (01/27/94 jd)
m.cpathname = AddBS(JustPath(m.csrcfile))		&& use AddBS -- smarter about the last directory separator (jd 03.24.96)

IF LEFT(FREAD(m.fhandle,41),40) # "dBASE IV Generic Design File Version 1.0"
	IF UPPER(TRIM(JustExt(m.csrcfile)))$"LBL|FRM|FRX|LBX"
		PRIVATE listhere,m.nctr9, ctmpext
		=FCLOSE(m.fhandle)

		*- what kind of file are we dealing with?
		IF JUSTEXT(m.csrcfile) $ "LBL|LBX"
			m.cfext = ".LBX"
			m.cmext = ".LBT"
		ELSE
			m.cfext = ".FRX"
			m.cmext = ".FRT"
		ENDIF

		IF _DOS
			*- disguise this as an .FRX file
			COPY FILE (csrcfile) TO (m.cpathname + m.rootname + m.cfext)
			SET EXACT &oldexact
			RETURN .T.
		ENDIF
		FOR  m.nctr9 = 1 TO 999
			IF !FILE(m.cpathname + rootname + '.' + RIGHT(STR(m.nctr9 + 1000,4),3))
				EXIT
			ENDIF
		NEXT
		IF m.nctr9 = 1000
			SET EXACT &oldexact
			RETURN .F.
		ENDIF
		ctmpext = '.' + RIGHT(STR(m.nctr9 + 1000,4),3)
		*- save their old file
		RENAME (m.csrcfile) TO (m.cpathname + m.rootname + m.ctmpext)
		COPY FILE (m.cpathname + m.rootname + m.ctmpext) TO (m.csrcfile)
		gOTherm.SetTitle(C_THERMMSG10_LOC + LOWER(PARTIALFNAME(m.csrcfile,C_FILELEN)))
		DO (gTransport) WITH m.csrcfile,IIF(m.cfext = ".FRX",23,24),.F.,gAShowMe, m.gOTherm,m.csrcfile
		*- change the name to .FRX, and restore original filename
		DO CASE
			CASE FILE(m.cpathname + m.rootname + ".FPT")
				*- assume this means they converted
				*- assume this means they converted
				IF FILE(m.cpathname + m.rootname + m.cfext)
					ERASE (m.cpathname + m.rootname + m.cfext)
				ENDIF
				IF FILE(m.cpathname + m.rootname + m.cmext)
					ERASE (m.cpathname + m.rootname + m.cmext)
				ENDIF
				RENAME (m.cpathname + m.rootname + ".FPT") TO (m.cpathname + m.rootname + m.cmext)
				RENAME (m.csrcfile) TO (m.cpathname + m.rootname + m.cfext)
				RENAME (m.cpathname + m.rootname + m.ctmpext) TO (m.csrcfile)
				IF FILE(m.cpathname + m.rootname + ".TBK")
					ERASE (m.cpathname + m.rootname +  ".TBK")
				ENDIF
				m.lretval = .T.
			CASE FILE(m.cpathname + m.rootname + m.cmext)
				*- assume this means they converted
				IF FILE(m.cpathname + m.rootname + m.ctmpext)
					ERASE (m.cpathname + m.rootname + m.ctmpext)
				ENDIF
				IF FILE(m.cpathname + m.rootname + ".TBK")
					ERASE (m.cpathname + m.rootname +  ".TBK")
				ENDIF
				m.lretval = .T.
			OTHERWISE
				*- erase the copy we made
				IF FILE(m.csrcfile)
					ERASE (m.csrcfile)
				ENDIF
				RENAME (m.cpathname + m.rootname + m.ctmpext) TO (m.csrcfile)
				m.lretval = .F.
		ENDCASE
		SET EXACT &oldexact
		RETURN m.lretval
	ELSE
		IF !FILE(m.cpathname + m.rootname + ".FMT")
			*-=falldown(C_ERRNOFMT_LOC)
			=falldown("")
		ELSE
			=FCLOSE(m.fhandle)
******----> Use class instead of PRG.
			LOCAL oConvObject

			oConvObject = CREATE(cFmtConverterClass, @aParms)
			IF TYPE("oConvObject") # 'O'
				*- object was not created
				*-THIS.lHadError = .T.
				RETURN .F.
			ENDIF
			
			IF oConvObject.lHadError
				*- error creating SCX object: 
				*- assume error has already been presented to user
				*THIS.lHadError = .T.
				RELEASE oConvObject
				RETURN
			ENDIF

			oConvObject.Converter()

			*- IF fmt2scxw(m.cpathname + m.rootname + ".FMT")
			IF oConvObject.lHadError
				SET EXACT &oldexact
				oConvObject = .NULL.
				RELEASE oConvObject
				RETURN .F.
			ENDIF

			RELEASE oConvObject
			SET EXACT &oldexact
			RETURN .T.
******---->
		ENDIF
	ENDIF
	SET EXACT &oldexact
	RETURN .F.
ENDIF

PRIVATE m.filevers
m.filevers = ASC(FREAD(m.fhandle,1))
PRIVATE m.filetype
m.filetype = ASC(FREAD(m.fhandle,1))
IF m.filetype <> scr_type AND m.filetype <> lbl_type AND m.filetype <> frm_type
	=falldown(C_ERRFTYPE_LOC)
	SET EXACT &oldexact
	RETURN .F.
ENDIF
=FSEEK(m.fhandle,13,1)          && Skip 13 bytes.
PRIVATE num_bands
num_bands = ASC(FREAD(m.fhandle,1))
PRIVATE m.targetname
DO CASE
	CASE m.filetype = scr_type
		IF FILE(m.rootname+".SCX") AND NOT ok2nuke(m.rootname+".SCX")
			=FCLOSE(m.fhandle)
			SET EXACT &oldexact
			RETURN .F.
		ENDIF
		m.targetname = m.cpathname + m.rootname + ".SCX"
		PRIVATE m.tempname
		m.tempname = SYS(3)
		SELECT SELECT(1)
		CREATE DBF (m.tempname)   ;
			( platform   c(8),    ;
			uniqueid   c(10),   ;
			timestamp  n(10),   ;
			objtype    n(2),    ;
			objcode    n(3),    ;
			name       m,       ;
			expr       m,       ;
			vpos       n(7,3),  ;
			hpos       n(7,3),  ;
			height     n(7,3),  ;
			width      n(7,3),  ;
			style      n(2),    ;
			picture    m,       ;
			order      m,       ;
			"unique"   l,       ;
			comment    m,       ;
			environ    l,       ;
			boxchar    c( 1),   ;
			fillchar   c( 1),   ;
			tag        m(10),   ;
			tag2       m(10),   ;
			penred     n(5),    ;
			pengreen   n(5),    ;
			penblue    n(5),    ;
			fillred    n(5),    ;
			fillgreen  n(5),    ;
			fillblue   n(5),    ;
			pensize    n(5),    ;
			penpat     n(5),    ;
			fillpat    n(5),    ;
			fontface   m,       ;
			fontstyle  n(3),    ;
			fontsize   n(3),    ;
			mode       n(3),    ;
			ruler      n(1),    ;
			rulerlines n(1),    ;
			grid       l,       ;
			gridv      n(2),    ;
			gridh      n(2),    ;
			scheme     n(2),    ;
			scheme2    n(2),    ;
			colorpair  c(8),    ;
			lotype     n(1),    ;
			rangelo    m,       ;
			hitype     n(1),    ;
			rangehi    m,       ;
			whentype   n(1),    ;
			when       m,       ;
			validtype  n(1),    ;
			valid      m,       ;
			errortype  n(1),    ;
			error      m,       ;
			messtype   n(1),    ;
			message    m,       ;
			showtype   n(1),    ;
			show       m,       ;
			activtype  n(1),    ;
			activate   m,       ;
			deacttype  n(1),    ;
			deactivate m,       ;
			proctype   n(1),    ;
			proccode   m,       ;
			setuptype  n(1),    ;
			setupcode  m,       ;
			float      l,       ;
			close      l,       ;
			minimize   l,       ;
			border     n(1),    ;
			shadow     l,       ;
			center     l,       ;
			refresh    l,       ;
			disabled   l,       ;
			scrollbar  l,       ;
			addalias   l,       ;
			tab        l,       ;
			initialval m,       ;
			initialnum n(3),    ;
			spacing    n(6,3),  ;
			curpos     l        ;
			)
		USE (m.tempname) ALIAS newfile
		APPEND BLANK
		REPLACE newfile.objtype WITH 1,  ;
			newfile.objcode WITH 63 &&10
		*- new options added by JD for Wizards
		*- window title, float, centered, single border, + support for 
		*- PG UP & PG DN
		*- add color scheme (02/08/94 jd)
		REPLACE newfile.style WITH 2,;
				newfile.tag WITH '"' + PROPER(JustStem(m.csrcfile)) + '"',;
				newfile.gridv WITH 1,;
				newfile.gridh WITH 1,;
				newfile.whentype WITH 1,;
				newfile.validtype WITH 1,;
				newfile.showtype WITH 1,;
				newfile.activtype WITH 1,;
				newfile.deacttype WITH 1,;
				newfile.proctype WITH 1,;
				newfile.setuptype WITH 1,;
				newfile.float WITH .T.,;
				newfile.close WITH .T.,;
				newfile.border WITH 4,;
				newfile.center WITH .T.,;
				newfile.minimize WITH .T.,;
				newfile.scheme WITH 8,;
				newfile.scheme2 WITH 9,;
				newfile.deactivate WITH "CLEAR READ" + C_CRLF,;
				newfile.setupcode WITH "PUSH KEY" + C_CRLF + ;
					"ON KEY LABEL PGUP DO dopgup" + C_CRLF + ;
					"ON KEY LABEL PGDN DO dopgdn" + C_CRLF + ;
					"ON KEY LABEL CTRL+PGUP DO ctlpgup" + C_CRLF + ;
					"ON KEY LABEL CTRL+PGDN DO ctlpgdn" + C_CRLF + ;
					"IF EOF()" + C_CRLF + ;
					"  GO BOTTOM" + C_CRLF + ;
					"ENDIF" + C_CRLF
			*- Append navigation code & alert screen from
			*- file included in project (01/27/94 JD)
			APPEND MEMO newfile.proccode FROM mignavpr.txt OVERWRITE
	CASE m.filetype = frm_type
		IF FILE(m.cpathname + m.rootname+".FRX") AND NOT ok2nuke(m.cpathname + m.rootname+".FRX")
			=FCLOSE(m.fhandle)
			SET EXACT &oldexact
			RETURN .F.
		ENDIF
		m.targetname = m.cpathname + m.rootname + ".FRX"
		SELECT SELECT(1)
		CREATE DBF (m.targetname) ;
			( platform   c(8),    ;
			uniqueid   c(10),   ;
			timestamp  n(10),   ;
			objtype    n(2),    ;
			objcode    n(3),    ;
			name       m,       ;
			expr       m,       ;
			vpos       n(9,3),  ;
			hpos       n(9,3),  ;
			height     n(9,3),  ;
			width      n(9,3),  ;
			style      m,       ;
			picture    m,       ;
			order      m,       ;
			"unique"   l,       ;
			comment    m,       ;
			environ    l,       ;
			boxchar    c(1),    ;
			fillchar   c(1),    ;
			tag        m,       ;
			tag2       m,       ;
			penred     n(5),    ;
			pengreen   n(5),    ;
			penblue    n(5),    ;
			fillred    n(5),    ;
			fillgreen  n(5),    ;
			fillblue   n(5),    ;
			pensize    n(5),    ;
			penpat     n(5),    ;
			fillpat    n(5),    ;
			fontface   m,       ;
			fontstyle  n(3),    ;
			fontsize   n(3),    ;
			mode       n(3),    ;
			ruler      n(1),    ;
			rulerlines n(1),    ;
			grid       l,       ;
			gridv      n(2),    ;
			gridh      n(2),    ;
			float      l,       ;
			stretch    l,       ;
			stretchtop l,       ;
			top        l,       ;
			bottom     l,       ;
			suptype    n(1),    ;
			suprest    n(1),    ;
			norepeat   l,       ;
			resetrpt   n( 2),   ;
			pagebreak  l,       ;
			colbreak   l,       ;
			resetpage  l,       ;
			general    n(3),    ;
			spacing    n(3),    ;
			double     l,       ;
			swapheader l,       ;
			swapfooter l,       ;
			ejectbefor l,       ;
			ejectafter l,       ;
			plain      l,       ;
			summary    l,       ;
			addalias   l,       ;
			offset     n(3),    ;
			topmargin  n(3),    ;
			botmargin  n(3),    ;
			totaltype  n(2),    ;
			resettotal n(2),    ;
			resoid     n(3),    ;
			curpos     l,       ;
			supalways  l,       ;
			supovflow  l,       ;
			suprpcol   n(1),    ;
			supgroup   n(2),    ;
			supvalchng l,       ;
			supexpr    m        ;
			)

		USE (m.targetname) ALIAS newfile
		APPEND BLANK
		REPLACE newfile.objtype WITH 1,  ;
			newfile.objcode WITH 53  && Header record.
		REPLACE newfile.height WITH 66      && _plength in IV...
		APPEND BLANK
		REPLACE objtype WITH 21 && Means there is an Intro (title) band??
	CASE m.filetype = lbl_type
		IF FILE(m.cpathname + m.rootname+".LBX") AND NOT ok2nuke(m.cpathname + m.rootname+".LBX")
			=FCLOSE(m.fhandle)
			SET EXACT &oldexact
			RETURN .F.
		ENDIF
		m.targetname = m.cpathname + m.rootname + ".LBX"
		SELECT SELECT(1)
		create dbf (m.targetname) ;
			( platform   c(8),    ;
			uniqueid   c(10),   ;
			timestamp  n(10),   ;
			objtype    n(2),    ;
			objcode    n(3),    ;
			name       m,       ;
			expr       m,       ;
			vpos       n(9,3),  ;
			hpos       n(9,3),  ;
			height     n(9,3),  ;
			width      n(9,3),  ;
			style      m,       ;
			picture    m,       ;
			order      m,       ;
			"unique"   l,       ;
			comment    m,       ;
			environ    l,       ;
			boxchar    c(1),    ;
			fillchar   c(1),    ;
			tag        m,       ;
			tag2       m,       ;
			penred     n(5),    ;
			pengreen   n(5),    ;
			penblue    n(5),    ;
			fillred    n(5),    ;
			fillgreen  n(5),    ;
			fillblue   n(5),    ;
			pensize    n(5),    ;
			penpat     n(5),    ;
			fillpat    n(5),    ;
			fontface   m,       ;
			fontstyle  n(3),    ;
			fontsize   n(3),    ;
			mode       n(3),    ;
			ruler      n(1),    ;
			rulerlines n(1),    ;
			grid       l,       ;
			gridv      n(2),    ;
			gridh      n(2),    ;
			float      l,       ;
			stretch    l,       ;
			stretchtop l,       ;
			top        l,       ;
			bottom     l,       ;
			suptype    n(1),    ;
			suprest    n(1),    ;
			norepeat   l,       ;
			resetrpt   n( 2),   ;
			pagebreak  l,       ;
			colbreak   l,       ;
			resetpage  l,       ;
			general    n(3),    ;
			spacing    n(3),    ;
			double     l,       ;
			swapheader l,       ;
			swapfooter l,       ;
			ejectbefor l,       ;
			ejectafter l,       ;
			plain      l,       ;
			summary    l,       ;
			addalias   l,       ;
			offset     n(3),    ;
			topmargin  n(3),    ;
			botmargin  n(3),    ;
			totaltype  n(2),    ;
			resettotal n(2),    ;
			resoid     n(3),    ;
			curpos     l,       ;
			supalways  l,       ;
			supovflow  l,       ;
			suprpcol   n(1),    ;
			supgroup   n(2),    ;
			supvalchng l,       ;
			supexpr    m        ;
			)
		USE (m.targetname) ALIAS newfile
		SELECT SELECT(1)
		PRIVATE m.tempname
		m.tempname = SYS(3)
		CREATE DBF (m.tempname)   ;
			( expr      c(254),   ;
			vpos      n(  3),   ;
			hpos      n(  3),   ;
			width     n(  3)    ;
			)
		USE (m.tempname) ALIAS tempdbf
		SELECT newfile
		APPEND BLANK
		REPLACE newfile.objtype WITH 30
ENDCASE
PRIVATE isrecnofld
isrecnofld = .F.

gOTherm.Update(0)

=FSEEK(m.fhandle,1,1)           && Skip 1 byte. (Page heading flag.)
PRIVATE m.numfields
m.numfields = word2num(FREAD(m.fhandle,2))
DO CASE
	CASE m.filetype = scr_type    && If screen,
		=FSEEK(m.fhandle,40,1)    &&  skip 40 bytes.
		REPLACE newfile.height WITH 25 && Height in file seems incorrect...
	CASE m.filetype = frm_type    && If report,
		=FSEEK(m.fhandle,40,1)    &&  skip 12 bytes (reserved).
		** RF tempnote: num fields, band ids, here...needed???
	CASE m.filetype = lbl_type    && If label,
		=FSEEK(m.fhandle,24,1)    &&  skip 24 bytes.
		HEIGHT = word2num(FREAD(m.fhandle,2))
		REPLACE newfile.width WITH word2num(FREAD(m.fhandle,2)),      ;
			newfile.hpos WITH word2num(FREAD(m.fhandle,2)),       ;
			newfile.height WITH word2num(FREAD(m.fhandle,2)),  ;
			newfile.penblue WITH word2num(FREAD(m.fhandle,2)),   ;
			newfile.vpos WITH word2num(FREAD(m.fhandle,2))
		=FSEEK(m.fhandle,4,1)
		APPEND BLANK
		REPLACE newfile.objtype WITH 9,     ;
			newfile.objcode WITH 4,     ;
			newfile.height WITH m.height
		* Pro labels have exactly one record per line for contents.
		DO WHILE RECCOUNT() <= m.height + 1
			APPEND BLANK
			REPLACE newfile.objtype WITH 19
		ENDDO

		GOTO 1
ENDCASE
=readstring(m.fhandle)          && Throw away print template name.
IF _DOS
	*?" Internal name: " + readstring(m.fhandle)
	*?
	=readstring(m.fhandle)
ELSE
	=readstring(m.fhandle)
ENDIF
GO TOP
REPLACE comment WITH readstring(m.fhandle)
PRIVATE linecount
* ID bytes in band descriptor
#DEFINE page_header_iv        0
#DEFINE report_intro_iv       1
#DEFINE group_intro_iv        2
#DEFINE detail_iv             3
#DEFINE group_summary_iv      4
#DEFINE report_summary_iv     5
#DEFINE page_footer_iv        6


#DEFINE named_calc_variable   98
* Traverse band descriptors.

PRIVATE band_type, whicheditr, group_type, num_recs, group_expr, kludgename
FOR m.bandnum = 1 TO num_bands
	IF m.filetype <> frm_type
		=FSEEK(m.fhandle,44,1)          && Skip 44 bytes.
		=readstring(m.fhandle)          && Throw away "field to group by."
		=readstring(m.fhandle)          && Throw away "group by" expr.
		LOOP                            && Only hits here once anyway...
	ENDIF
	=FSEEK(m.fhandle,12,1)          && Skip 12 bytes (reserved)
	band_type = ASC(FREAD(m.fhandle,1))
	GO BOTTOM
	IF newfile.objtype = named_calc_variable
		DO WHILE newfile.objtype = named_calc_variable
			SKIP -1
		ENDDO
		SKIP 1
		INSERT BLANK BEFORE
	ELSE
		APPEND BLANK
	ENDIF
	REPLACE newfile.objtype WITH 9
	DO CASE
		CASE m.band_type = page_header_iv
			REPLACE newfile.objcode WITH 1
		CASE m.band_type = report_intro_iv
			REPLACE newfile.objcode WITH 0
		CASE m.band_type = detail_iv
			REPLACE newfile.objcode WITH 4
		CASE m.band_type = page_footer_iv
			REPLACE newfile.objcode WITH 7
		CASE m.band_type = report_summary_iv
			REPLACE newfile.objcode WITH 8
		OTHERWISE
			IF m.bandnum < CEILING(num_bands/2)
				REPLACE objcode WITH 3        && Pre-detail group band.
			ELSE
				REPLACE objcode WITH 5        && Post-detail group band.
			ENDIF
	ENDCASE
	=FSEEK(m.fhandle,1,1)           && Toss group # for now.
	m.whicheditr = ASC(FREAD(m.fhandle,1))
	IF m.whicheditr = 1
		REPLACE newfile.comment WITH "Converted dBASE IV WordWrap band."
	ENDIF
	=FSEEK(m.fhandle,8,1)           && Skip 8 bytes (reserved).
	PRIVATE m.isopen
	m.isopen = IIF(ASC(FREAD(m.fhandle,1)) = 1, .T., .F.)
	m.group_type = ASC(FREAD(m.fhandle,1))  && 0=field,3=records, otherwise
	&& expr datatype (N/L/C/F/D/N).
	=FSEEK(m.fhandle,4,1)           && Skip 4 bytes ("spare").
	m.num_recs = word2num(FREAD(m.fhandle,2))
	=FSEEK(m.fhandle,4,1)          && Skip 4 bytes ("reserved").
	PRIVATE eachpage, newpage
	m.eachpage = ASC(FREAD(m.fhandle,1))
	m.newpage  = ASC(FREAD(m.fhandle,1))
	IF m.eachpage = 0
		REPLACE newfile.norepeat WITH .T.
	ENDIF
	IF m.newpage = 1
		REPLACE newfile.pagebreak WITH .T.
	ENDIF
	=FSEEK(m.fhandle,2,1)          && Skip 2 bytes (pitch/quality).
	REPLACE newfile.objtype WITH 9
	m.linecount = word2num(FREAD(m.fhandle,2))
	IF m.filetype = frm_type
		REPLACE newfile.height WITH IIF(m.isopen,MAX(m.linecount,1),0)
	ENDIF
	=FSEEK(m.fhandle,1,1)           && Skip band spacing.
	=word2num(FREAD(m.fhandle,2))   && Throw away band size (get later).

	group_expr = readstring(m.fhandle)    && Field to group on.
	IF m.group_type < 3
		REPLACE newfile.expr WITH m.group_expr
	ENDIF
	group_expr = readstring(m.fhandle)    && Expr to group on.
	IF m.group_type > 1
		IF m.group_type = 3           && Group by record count.
			kludgename = "_bandrec" + LTRIM(STR(m.bandnum))
			REPLACE newfile.expr WITH kludgename,     ;
				newfile.norepeat WITH .T. &&???

			isrecnofld = .T.
			APPEND BLANK
			REPLACE newfile.objtype WITH named_calc_variable,     ;
				newfile.objcode WITH 0,                       ;
				newfile.name WITH UPPER(kludgename),          ;
				newfile.expr WITH                             ;
				"iif(mod(reccnt," + LTRIM(STR(m.num_recs)) + ") = 0," + ;
				"iif("+kludgename +"=1,0,1),"+kludgename+")", ;
				newfile.tag WITH "0",                         ;
				unique WITH .T.
		ELSE
			REPLACE newfile.expr WITH m.group_expr
		ENDIF
	ENDIF
ENDFOR

* Next, step through "fields" (anything but text).
PRIVATE fieldname, datatype, fieldtype, fieldpic, fieldpfunc,;
		calias, naliaspos

#DEFINE dbftype         0
#DEFINE calctype        1
#DEFINE summarytype     2     && Shouldn't be needed!
#DEFINE predeftype      3
#DEFINE memvartype      4

DECLARE avline[256,4]               && keep track of vertical lines: for each col, 1 = start, 2 = end, 3 = colorpr, 4 = single?
STORE -1 TO avline

PRIVATE workareas
DECLARE workareas[25]
FOR m.fieldnum = 1 TO m.numfields
	IF FEOF(m.fhandle)
		*- EOF has been reached prematurely, so bail out (2/23/94)
		EXIT
	ENDIF
	gOTherm.Update(m.fieldnum/m.numfields * 100)
	DO CASE
		CASE m.filetype = frm_type
			APPEND BLANK
			REPLACE newfile.height WITH 1,   ;
				newfile.objtype WITH 8,  ;
				newfile.objcode WITH 0
		CASE m.filetype = scr_type
			APPEND BLANK
			REPLACE newfile.height WITH 1,   ;
				newfile.refresh WITH .F.,;
				newfile.objcode WITH 1,  ;
				newfile.objtype WITH 15
		CASE m.filetype = lbl_type
			SELECT tempdbf
			APPEND BLANK
			SELECT newfile
	ENDCASE
	=FSEEK(m.fhandle,5,1)     && Skip 5 bytes.
	fieldname = xtrim(FREAD(m.fhandle,11))
	IF "QBE__" $ fieldname
		*- strip off alias, since we assume it refers to a temp
		*- file from a DB4 query (01/27/94 jd)
		*- determine alias name
		m.naliaspos = AT("QBE__",fieldname)
		m.calias = SUBS(fieldname,m.naliaspos,AT(".",SUBS(fieldname,m.naliaspos)))
		m.fieldname = STRTRAN(fieldname,m.calias,"")
	ENDIF
	fieldtype = ASC(FREAD(m.fhandle,1))
	datatype  = FREAD(m.fhandle,1)
	=FSEEK(m.fhandle,12,1)          && Skip reserved bytes.
	WIDTH = ASC(FREAD(m.fhandle,1))
	PRIVATE supprepeat, ishidden
	supprepeat = IIF(ASC(FREAD(m.fhandle,1)) = 1, .T., .F.)
	ishidden   = IIF(ASC(FREAD(m.fhandle,1)) = 1, .T., .F.)
	DO CASE
		CASE m.filetype = scr_type
			REPLACE newfile.width WITH m.width,             ;
				newfile.fillchar WITH IIF(m.datatype = "F", "N", m.datatype)

		CASE m.filetype = frm_type
			REPLACE newfile.width WITH m.width,             ;
				newfile.norepeat WITH m.supprepeat,     ;
				newfile.fillchar WITH IIF(m.datatype = "F", "N", m.datatype)
		CASE m.filetype = lbl_type
			REPLACE tempdbf.width WITH m.width
	ENDCASE
	IF m.fieldtype = summarytype
		PRIVATE summ_field
		summ_field = xtrim(FREAD(m.fhandle,11))
		=FSEEK(m.fhandle,4,1)                       && Reserved.
		PRIVATE summ_op
		#DEFINE summ_avg_iv     0
		#DEFINE summ_cnt_iv     1
		#DEFINE summ_max_iv     2
		#DEFINE summ_min_iv     3
		#DEFINE summ_sum_iv     4
		#DEFINE summ_std_iv     5
		#DEFINE summ_var_iv     6

		summ_op = ASC(FREAD(m.fhandle,1))
		#DEFINE reset_never     0
		#DEFINE reset_page      1
		#DEFINE reset_group     2
		PRIVATE reset_when
		reset_when = ASC(FREAD(m.fhandle,1))
		=FSEEK(m.fhandle,73,1)                 && Rest of ext. info.
	ELSE
		PRIVATE predeftyp
		predeftyp = ASC(FREAD(m.fhandle,1))
		PRIVATE dbfname
		dbfname = CHR(m.predeftyp) + xtrim(FREAD(m.fhandle,80))
		=FSEEK(m.fhandle,9,1)                 && Last 9 of ext. info.
	ENDIF
	IF m.fieldtype = dbftype
		dbfname = xtrim(JustFName(m.dbfname))
		IF ASCAN(workareas, m.dbfname) = 0
			workareas[ascan(workareas, .F.)] = m.dbfname
		ENDIF
		IF !("QBE__" $ m.dbfname)
			*- ignore alias if from a DB4 query temp file name
			fieldname = m.dbfname + "." + m.fieldname
		ENDIF
	ENDIF
	=FSEEK(m.fhandle,4,1)                       && Reserved.
	fieldpict  = readstring(m.fhandle)
	fieldpfunc = readstring(m.fhandle)
	IF "V"$m.fieldpfunc AND m.filetype = frm_type
		fieldpfunc = STRTRAN(m.fieldpfunc,"V","")
		REPLACE newfile.stretch WITH .T.
	ENDIF
	pict_func = ""
	IF "" <> m.fieldpfunc
		pict_func = "@" + m.fieldpfunc
	ENDIF
	IF "" <> m.fieldpict
		IF "" <> m.pict_func
			pict_func = m.pict_func + " "
		ENDIF
		pict_func = m.pict_func + m.fieldpict
	ENDIF
	IF "" <> m.pict_func AND TYPE("newfile.picture") <> "U"
		REPLACE newfile.picture WITH '"' + m.pict_func + '"'
	ENDIF
	PRIVATE delimiter1, delimiter2
	DO CASE
		CASE m.datatype = "D"
			delimiter1 = "{"
			delimiter2 = "}"
		CASE m.datatype = "C"
			delimiter1 = '"'
			delimiter2 = '"'
		CASE m.datatype = "M"
			REPLACE newfile.picture WITH ""
			* ProWin 2.5 rel. 1 bug: no pictures on memos!
		OTHERWISE
			delimiter1 = ""
			delimiter2 = ""
	ENDCASE
	DO CASE
		CASE m.fieldtype = dbftype OR m.fieldtype = memvartype
			DO CASE
				CASE m.filetype = scr_type
					isedit = (ASC(FREAD(m.fhandle,1)) <> 0)
					IF m.isedit
						REPLACE newfile.name WITH m.fieldname
					ELSE
						REPLACE newfile.expr WITH m.fieldname,    ;
							newfile.refresh WITH .T.,         ;
							newfile.objcode WITH 0
					ENDIF
					=FSEEK(m.fhandle,1,1)     && Skip carry flag.(??)
					REPLACE newfile.rangelo WITH readstring(m.fhandle),     ;
						newfile.rangehi WITH readstring(m.fhandle),     ;
						newfile.initialval WITH readstring(m.fhandle),  ;
						newfile.when WITH readstring(m.fhandle),        ;
						newfile.valid WITH readstring(m.fhandle),       ;
						newfile.error WITH readstring(m.fhandle),       ;
						newfile.message WITH readstring(m.fhandle)

					* Brute force method here, Q&D but should work.
					IF "" <> newfile.rangelo AND LEFT(newfile.rangelo,1) <> m.delimiter1
						REPLACE newfile.rangelo WITH m.delimiter1 +     ;
							newfile.rangelo + m.delimiter2
					ENDIF
					IF "" <> newfile.rangehi AND LEFT(newfile.rangehi,1) <> m.delimiter1
						REPLACE newfile.rangehi WITH m.delimiter1 +     ;
							newfile.rangehi + m.delimiter2
					ENDIF
					IF "" <> newfile.message
						REPLACE newfile.message WITH '"' +              ;
							newfile.message + '"'
					ENDIF
					IF "" <> newfile.error
						REPLACE newfile.error WITH '"' + newfile.error + '"'
					ENDIF

					choicelist = readstring(m.fhandle)
					IF LEN(m.choicelist) > 0
						REPLACE newfile.picture WITH '"@M '+choicelist+'"'
						* Overrides any previous picture and/or function.
					ENDIF
					scrolwidth=word2num(FREAD(m.fhandle,2))
					IF m.scrolwidth > 0
						IF AT('S',newfile.picture) > 0
							REPLACE newfile.picture WITH ;
								LEFT(newfile.picture,AT('S',newfile.picture)) + ;
								LTRIM(STR(m.scrolwidth)) +  ;
								SUBSTR(newfile.picture,AT('S',newfile.picture)+1)
						ENDIF
					ENDIF
				CASE m.filetype = frm_type
					REPLACE newfile.expr WITH m.fieldname
				CASE m.filetype = lbl_type
					REPLACE tempdbf.expr WITH m.fieldname
			ENDCASE

		CASE m.fieldtype = calctype
			IF m.filetype = scr_type
				REPLACE newfile.objtype WITH 15, newfile.objcode WITH 0
			ENDIF
			IF TYPE("newfile.comment") <> "U"
				REPLACE newfile.comment WITH newfile.comment +  ;
					readstring(m.fhandle)
			ELSE
				=readstring(m.fhandle)
			ENDIF
			exprvar = readstring(m.fhandle)
			IF "QBE__" $ exprvar 
				*- strip off alias, since we assume it refers to a temp
				*- file from a DB4 query (01/27/94 jd)
				*- determine alias name
				m.naliaspos = AT("QBE__",exprvar )
				m.calias = SUBS(exprvar ,m.naliaspos,AT(".",SUBS(exprvar ,m.naliaspos)))
				m.exprvar = STRTRAN(exprvar ,m.calias,"")
			ENDIF
			DO CASE
				CASE m.filetype = lbl_type
					REPLACE tempdbf.expr WITH m.exprvar
				CASE m.filetype = scr_type
					REPLACE newfile.expr WITH m.exprvar
				CASE m.filetype = frm_type
					IF "" = m.fieldname
						REPLACE newfile.expr WITH m.exprvar
						* Just a dynamically updated expression.
					ELSE
						REPLACE newfile.expr WITH m.fieldname
						* Create report variable.
						IF m.ishidden
							REPLACE newfile.objtype WITH 18,          ;
								newfile.name WITH m.fieldname,    ;
								newfile.expr WITH m.exprvar, ;
								newfile.tag WITH "0"
						ELSE
							APPEND BLANK
							REPLACE newfile.objtype WITH named_calc_variable,     ;
								newfile.name WITH m.fieldname,   ;
								newfile.expr WITH m.exprvar, ;
								newfile.tag WITH "0"
							SKIP -1
						ENDIF
					ENDIF
			ENDCASE

		CASE m.fieldtype = summarytype
			*            replace summary with .t. &&????????? Nahhh....

			#DEFINE summ_cnt_pro    1
			#DEFINE summ_sum_pro    2
			#DEFINE summ_avg_pro    3
			#DEFINE summ_min_pro    4
			#DEFINE summ_max_pro    5
			#DEFINE summ_std_pro    6
			#DEFINE summ_var_pro    7
			IF "" = m.fieldname
				REPLACE newfile.expr WITH m.summ_field
				* Just a dynamically updated expression.
			ELSE
				REPLACE newfile.expr WITH m.fieldname
				* Create report variable.
				IF m.ishidden
					REPLACE newfile.objtype WITH 18,          ;
						newfile.name WITH m.fieldname,    ;
						newfile.expr WITH m.summ_field, ;
						newfile.tag WITH "0"
				ELSE
					APPEND BLANK
					REPLACE newfile.objtype WITH named_calc_variable,     ;
						newfile.name WITH m.fieldname,   ;
						newfile.expr WITH m.summ_field, ;
						newfile.tag WITH "0"
					*                        skip -1
				ENDIF
			ENDIF
			*            replace newfile.expr with m.summ_field
			DO CASE
				CASE m.summ_op = summ_cnt_iv
					REPLACE newfile.totaltype WITH summ_cnt_pro
					REPLACE newfile.expr WITH "1" && (?!)
				CASE m.summ_op = summ_sum_iv
					REPLACE newfile.totaltype WITH summ_sum_pro
				CASE m.summ_op = summ_avg_iv
					REPLACE newfile.totaltype WITH summ_avg_pro
				CASE m.summ_op = summ_min_iv
					REPLACE newfile.totaltype WITH summ_min_pro
				CASE m.summ_op = summ_max_iv
					REPLACE newfile.totaltype WITH summ_max_pro
				CASE m.summ_op = summ_std_iv
					REPLACE newfile.totaltype WITH summ_std_pro
				CASE m.summ_op = summ_var_iv
					REPLACE newfile.totaltype WITH summ_var_pro
			ENDCASE
			IF TYPE("newfile.comment") <> "U"
				REPLACE newfile.comment WITH readstring(m.fhandle)
			ELSE
				=readstring(m.fhandle)    && Toss description.
			ENDIF
			PRIVATE band_id
			band_id = word2num(FREAD(m.fhandle,2))
			IF m.reset_when < reset_group
				REPLACE newfile.resettotal WITH m.reset_when
			ELSE
				REPLACE newfile.resettotal WITH m.band_id
				*** Is this always right?  Better be...
			ENDIF
			IF "" <> m.fieldname AND NOT m.ishidden
				SKIP -1     && Necessary?
			ENDIF
			=FSEEK(m.fhandle,2,1)                      && Toss hiword/bandID.
		CASE m.fieldtype = predeftype
			DO CASE
				CASE m.predeftyp = 0
					m.fieldname = "DATE()"
				CASE m.predeftyp = 1
					m.fieldname = "TIME()"
				CASE m.predeftyp = 2
					IF m.filetype = frm_type
						*                        fieldname = "'converted RECNO field'"
					ELSE
						fieldname = "RECNO()"
					ENDIF
				CASE m.predeftyp = 3
					fieldname = "_pageno"
			ENDCASE
			DO CASE
				CASE m.filetype = scr_type
					REPLACE newfile.objtype WITH 15, newfile.objcode WITH 0
					REPLACE newfile.expr WITH m.fieldname
				CASE m.filetype = frm_type
					REPLACE newfile.objtype WITH 8, newfile.objcode WITH 0
					IF m.predeftyp = 2
						REPLACE newfile.expr WITH "'converted RECNO field'"
						REPLACE totaltype WITH 1 && Count
					ELSE
						REPLACE newfile.expr WITH m.fieldname
					ENDIF
				CASE m.filetype = lbl_type
					REPLACE tempdbf.expr WITH m.fieldname
			ENDCASE
	ENDCASE
	* Handle concatenation of label fields on one line
	*  by making everything alpha.
	*IF m.filetype = lbl_type AND m.datatype <> "C" ;
	*		AND "" <> ALLTRIM(tempdbf.expr)
	*- removed test for datatype, since we want to migrate all picture clauses? (jd 6/23/94)
	IF m.filetype = lbl_type AND "" <> ALLTRIM(tempdbf.expr)
		REPLACE tempdbf.expr WITH "TRANSFORM(" + TRIM(tempdbf.expr) ;
			+ ',"' + pict_func + '")'
	ENDIF
ENDFOR

*- under certain condtions, EOF will have been
*- reached prematurely, so skip rest of file processing in that case (2/22/94)
IF !FEOF(m.fhandle)
	PRIVATE gl_ruler, bookmark
	gl_ruler = FREAD(m.fhandle,68)
	IF m.filetype = frm_type
		bookmark = RECNO()
		GOTO TOP
		REPLACE newfile.width WITH word2num(SUBSTR(m.gl_ruler,5,2)) + 1
		GOTO m.bookmark
	ENDIF

	PRIVATE bandmessg
	FOR m.bandnum = 1 TO num_bands
		bandmessg = dump_band(m.bandnum)
		IF m.bandmessg <> "OK"
			=falldown(bandmessg)
			SET EXACT &oldexact
			RETURN .F.
		ENDIF
	ENDFOR
ENDIF && !FEOF(m.fhandle)

*- resolve any lingering vertical lines
=FixVert(@avline)
gOTherm.Update(.98)
IF m.isrecnofld   && Reports only.
	APPEND BLANK
	*** CHECK IT OUT!  .NAME below MUST be upper case!
	REPLACE newfile.objtype WITH named_calc_variable,    ;
		newfile.objcode WITH 0,     ;
		newfile.name WITH "RECCNT", ;
		newfile.expr WITH "reccnt", ;
		newfile.tag WITH "0"        ;
		newfile.unique WITH .T.,    ;
		newfile.totaltype WITH 1  && (count)
ENDIF
=FCLOSE(m.fhandle)
PRIVATE warea
FOR warea = 1 TO 25
	IF TYPE("workareas[m.warea]") = "L"
		EXIT
	ENDIF
	IF LEFT(workareas[m.warea],5) = "QBE__"
		*- assume a temp file from a DB4 query, so ignore (01/27/94 jd)
		LOOP
	ENDIF
	APPEND BLANK
	REPLACE newfile.objtype WITH 2,                       ;
		newfile.objcode WITH m.warea,                 ;
		newfile.name WITH workareas[m.warea]+".DBF",  ;
		newfile.tag WITH workareas[m.warea]
ENDFOR
IF m.warea > 1
	GOTO TOP
	REPLACE newfile.environ WITH .T.
ENDIF
REPLACE ALL newfile.platform WITH "DOS", newfile.uniqueid WITH SYS(2015)
DO CASE
	CASE m.filetype = scr_type
		PRIVATE scrheight,m.nmaxwidth, m.nmaxheight
		GO TOP
		scrheight = newfile.height
		SCAN
			scrheight = MAX(m.scrheight, newfile.vpos + newfile.height)
		ENDSCAN
		GO TOP
		REPLACE newfile.height WITH m.scrheight
		SORT ON vpos, hpos TO (m.targetname)
		USE (m.targetname)
		CALCULATE MAX(hpos + width),MAX(vpos + height) FOR RECNO() > 1 ;
			TO m.nmaxwidth, m.nmaxheight
		GO TOP
		REPLACE width WITH MAX(width,m.nmaxwidth + 2),;
			height WITH MAX(height,m.nmaxheight + 2) 
		USE
		DELETE FILE &tempname..dbf
		DELETE FILE &tempname..fpt	&&added by bobfor
		=CPTAG(m.targetname,CPCURRENT(2))
	CASE m.filetype = lbl_type
		SELECT tempdbf
		USE
		DELETE FILE &tempname..dbf
		SELECT newfile
		GOTO 3
		SCAN WHILE .NOT. EOF()
			REPLACE newfile.width WITH 0
		ENDSCAN
		USE
		=CPTAG(m.targetname,CPCURRENT(2))
	CASE m.filetype = frm_type
		SELECT newfile
		REPLACE ALL objtype WITH 18 FOR newfile.objtype = named_calc_variable

		* Now get tricky and find empty open bands, make sure they print.
		PRIVATE bandheight, hasobjects
		GO TOP
		LOCATE FOR objtype = 9  && Find first band.
		bandstart = 0
		SCAN WHILE newfile.objtype = 9
			IF newfile.height = 0
				bandstart = m.bandstart + 1
				LOOP
			ENDIF
			bookmark = RECNO()
			hasobjects = .F.
			bandheight = newfile.height
			SCAN REST
				IF newfile.vpos >= m.bandstart AND newfile.vpos < (m.bandstart + m.bandheight)
					hasobjects = .T.
					EXIT
				ENDIF
			ENDSCAN
			IF !m.hasobjects
				APPEND BLANK
				REPLACE newfile.objtype WITH 5,           ;
					newfile.objcode WITH 0,           ;
					newfile.height WITH 1,            ;
					newfile.width WITH 1,             ;
					newfile.expr WITH '" "'           ;
					newfile.vpos WITH m.bandstart,    ;
					newfile.hpos WITH 0,              ;
					newfile.uniqueid WITH SYS(2015),  ;
					newfile.platform WITH "DOS"
			ENDIF
			GOTO m.bookmark
			bandstart = m.bandstart + newfile.height
		ENDSCAN
		* Now replace all "closed" band sizes with 1 so they show on surface.
		REPLACE ALL newfile.height WITH 1 FOR newfile.height = 0

		* Hokay, now make sure that all of the object in the report
		* fit in the margins.
		PRIVATE repwidth
		GOTO TOP
		repwidth = newfile.width
		LOCATE FOR newfile.objtype = 9
		SCAN WHILE newfile.objtype = 9
		ENDSCAN
		SCAN WHILE .NOT. EOF()
			repwidth = MAX(m.repwidth, newfile.hpos + newfile.width)
		ENDSCAN
		GO TOP
		REPLACE newfile.width WITH m.repwidth
		USE
		=CPTAG(m.targetname,CPCURRENT(2))
ENDCASE

gOTherm.Complete

USE
SET EXACT &oldexact.
IF "" <> oldalias
	SELECT (oldalias)
ENDIF
RETURN .T.

******************
FUNCTION dump_band      && Call funcs to dump wordwrap or layout band.
PARAMETER bandnum
PRIVATE bandstart && Starting row of band on display, used in both band dumps.
bandstart = 0
IF m.filetype <> scr_type
	GOTO 3
	SCAN WHILE newfile.objtype = 9 AND RECNO() < m.bandnum + 2
		bandstart = m.bandstart + MAX(newfile.height,1)
	ENDSCAN
ENDIF
IF m.filetype = frm_type
	GOTO m.bandnum + 2
	IF "WORDWRAP" $ UPPER(newfile.comment)
		RETURN dmprapband(m.bandnum)
	ENDIF
ENDIF
RETURN dumplayout(m.bandnum)

*******************
FUNCTION dmprapband     && No not Vanilla Ice...
PARAMETER bandnum

PRIVATE bandsize
bandsize = word2num(FREAD(m.fhandle,2))
PRIVATE wholeband
m.wholeband = FREAD(m.fhandle, m.bandsize)
PRIVATE marker
m.marker = 1
PRIVATE paragraph, linetext, LINENO, linemark, fieldmark, COLUMN, fieldwidth
PRIVATE lmargin, lindent
LINENO = -1
DO WHILE m.marker < m.bandsize
	LINENO = m.lineno + 1
	m.paragraph = SUBSTR(m.wholeband, m.marker)
	m.paragraph = LEFT(m.paragraph, AT(CHR(0), m.paragraph) - 1)
	m.marker = m.marker + LEN(m.paragraph) + 104
	m.lmargin = VAL(SUBSTR(m.wholeband,m.marker - 102,3))
	m.lindent = VAL(SUBSTR(m.wholeband,m.marker - 96,3))
	IF m.lindent = 0 AND SUBSTR(m.wholeband,m.marker - 95,2) <> " 0"
		IF SUBSTR(m.wholeband, m.marker - 95,1) <> " "
			lindent = (48-ASC(SUBSTR(m.wholeband,m.marker - 95,1))) * 10
		ENDIF
		m.lindent = m.lindent + 48 - ASC(SUBSTR(m.wholeband,m.marker - 94,1))
		m.lindent = 0 - m.lindent
	ENDIF
	COLUMN = m.lmargin + m.lindent

	DO WHILE LEN(m.paragraph) > 0
		m.linemark = AT(CHR(141)+CHR(10), m.paragraph)
		m.fieldmark = AT(CHR(1), m.paragraph)
		DO CASE
			CASE m.linemark > 0 AND (m.linemark < m.fieldmark OR m.fieldmark = 0)
				linetext = LEFT(m.paragraph, m.linemark - 1)
				paragraph = SUBSTR(m.paragraph, LEN(m.linetext) + 3)
			CASE m.fieldmark > 0 AND (m.fieldmark < m.linemark OR m.linemark = 0)
				linetext = LEFT(m.paragraph, m.fieldmark - 1)
				=gotofrmfld(VAL(SUBSTR(m.paragraph, LEN(m.linetext) + 3, 2)))
				REPLACE newfile.hpos WITH m.column + LEN(m.linetext),       ;
					newfile.vpos WITH m.lineno + m.bandstart,           ;
					newfile.float WITH .T.

				m.fieldwidth = newfile.width
				m.paragraph = SUBSTR(m.paragraph, LEN(m.linetext) + 12)
			OTHERWISE
				m.linetext = m.paragraph
				m.paragraph = ""
		ENDCASE
		APPEND BLANK
		REPLACE newfile.objtype WITH 5,                       ;
			newfile.objcode WITH 0,                       ;
			newfile.expr WITH '"' + m.linetext + '"'      ;
			newfile.height WITH 1,                        ;
			newfile.width WITH LEN(m.linetext),           ;
			newfile.vpos WITH m.lineno + m.bandstart,     ;
			newfile.hpos WITH m.column,                   ;
			newfile.float WITH .T.
		IF m.linemark > 0 AND (m.linemark < m.fieldmark OR m.fieldmark = 0)
			m.lineno = m.lineno + 1
		ENDIF
		IF m.fieldmark > 0 AND (m.fieldmark < m.linemark OR m.linemark = 0)
			m.column = m.column + LEN(m.linetext) + m.fieldwidth
		ELSE
			m.column = m.lmargin
		ENDIF
	ENDDO
ENDDO
RETURN "OK"

*******************
FUNCTION gotofrmfld     && Recno() + 2 + bands = field info but compensate
&& for Report Variable records (temptype=NAMED_CALC_VARIABLE).
PARAMETER pseekrec
PRIVATE seekrec
seekrec = m.pseekrec
GOTO 2 + m.num_bands

SET FILTER TO newfile.objtype <> named_calc_variable
* "Artificial" hidden fields.

SKIP m.pseekrec
SET FILTER TO
RETURN m.pseekrec

*******************
FUNCTION dumplayout     && Dump layout bands to FRX/LBX/SCX.
PARAMETER bandnum

PRIVATE bandsize
bandsize = word2num(FREAD(m.fhandle,2))
PRIVATE bandend
bandend = FSEEK(m.fhandle,0,1) + bandsize
bandversn = ASC(FREAD(m.fhandle,1))
* Update header record.
GOTO TOP
IF m.filetype = frm_type
	=FSEEK(m.fhandle,2,1)
ELSE
	REPLACE newfile.width WITH MAX(newfile.width,word2num(FREAD(m.fhandle,2)))
ENDIF
=FSEEK(m.fhandle,6,1)     && Screens: skip max height (not there?)
&& and #rows (not max), numtextrows (not correct?)

PRIVATE mrow && For labels, new row for field expr.
PRIVATE mexp && For labels, transfer expr from "order".
DO WHILE FREAD(m.fhandle,1) = ","     && Constant spacer--starts each field.
	&& (Should be "." if end of list.)
	gOTherm.Update(FSEEK(m.fhandle,0,1)/m.fsize * 100)			&& progress therm
	DO CASE
		CASE m.filetype = lbl_type
			GOTO word2num(FREAD(m.fhandle, 2)) IN tempdbf
		CASE m.filetype = frm_type
			=gotofrmfld(word2num(FREAD(m.fhandle,2)))
		CASE m.filetype = scr_type
			GOTO word2num(FREAD(m.fhandle, 2)) + 1
	ENDCASE
	IF m.bandversn = 4
		=FSEEK(m.fhandle,12,1)
	ENDIF
	IF m.filetype = scr_type OR m.filetype = frm_type
		IF EOF()    && ...Pro skips next three reads, eerie.
			=FSEEK(m.fhandle,6,1)
		ELSE
			REPLACE newfile.vpos WITH word2num(FREAD(m.fhandle,2)),   ;
				newfile.hpos WITH word2num(FREAD(m.fhandle,2)),   ;
				newfile.width WITH word2num(FREAD(m.fhandle,2))
			IF m.filetype = frm_type
				REPLACE newfile.vpos WITH newfile.vpos + m.bandstart
			ENDIF
		ENDIF
	ELSE
		REPLACE tempdbf.vpos WITH word2num(FREAD(m.fhandle,2)),   ;
			tempdbf.hpos WITH word2num(FREAD(m.fhandle,2)),   ;
			tempdbf.width WITH word2num(FREAD(m.fhandle,2))
	ENDIF
	IF m.bandversn = 4
		=FSEEK(m.fhandle,4,1)
	ENDIF
	IF ASC(FREAD(m.fhandle,1)) <> 0       && Window flag.
		IF m.bandversn = 4
			=FSEEK(m.fhandle,8,1)
		ENDIF
		=FSEEK(m.fhandle,8,1) && Skip window frame.
		winfo = FREAD(m.fhandle,9)
		IF SUBSTR(m.winfo,9,1) <> CHR(0)          && Open window memo.
			REPLACE newfile.objcode WITH 2      && "Edit" window.
			REPLACE newfile.hpos WITH word2num(SUBSTR(m.winfo,1,2)),    ;
				newfile.vpos WITH word2num(SUBSTR(m.winfo,3,2)),      ;
				newfile.width WITH word2num(SUBSTR(m.winfo,5,2)),     ;
				newfile.scrollbar WITH .T.,                           ;
				newfile.height WITH word2num(SUBSTR(m.winfo,7,2))
		ENDIF
		=FSEEK(m.fhandle,2,1)     && Toss attribute for now...
		IF m.bandversn = 4
			=FSEEK(m.fhandle,3,1)
		ENDIF
	ENDIF
ENDDO

* Read the box(/line) descriptors.  Method: read 19 bytes at
* a time.  When the first byte of the 19 is seen to be the list
* terminator, rewind 18 that belong to the next section.

PRIVATE m.boxdlength
IF m.bandversn = 5
	m.boxdlength = 19
ELSE
	m.boxdlength = 28
ENDIF
PRIVATE m.boxdescrpt
m.boxdescrpt = FREAD(m.fhandle,m.boxdlength)
DO WHILE LEFT(m.boxdescrpt, 1) = ","
	gOTherm.Update(FSEEK(m.fhandle,0,1)/m.fsize * 100)			&& progress therm
	IF m.filetype = lbl_type
		m.boxdescrpt = FREAD(m.fhandle,m.boxdlength)
		* Boxes/lines not supported in labels.
		LOOP
	ENDIF
	APPEND BLANK
	REPLACE newfile.objtype WITH 7
	IF m.bandversn = 4
		m.boxdescrpt = SUBSTR(m.boxdescrpt,9)
	ENDIF
	DO CASE
		CASE SUBSTR(m.boxdescrpt,2,1) = "Í"
			REPLACE newfile.objcode WITH 5      && Double-line box
		CASE SUBSTR(m.boxdescrpt,2,1) = "Ä"
			REPLACE newfile.objcode WITH 4      && Single-line box
		OTHERWISE
			REPLACE newfile.objcode WITH 7      && Special-char box
			REPLACE newfile.boxchar WITH SUBSTR(m.boxdescrpt,2,1)
	ENDCASE
	REPLACE newfile.fillchar WITH CHR(0)      && Make transparent.
	REPLACE newfile.hpos WITH word2num(SUBSTR(m.boxdescrpt,10,2)),      ;
		newfile.vpos WITH word2num(SUBSTR(m.boxdescrpt,12,2)),      ;
		newfile.width WITH word2num(SUBSTR(m.boxdescrpt,14,2)),     ;
		newfile.height WITH word2num(SUBSTR(m.boxdescrpt,16,2))
	IF m.filetype = scr_type
		IF bandversn = 4
			REPLACE newfile.colorpair WITH ;
				bits2color(word2num(SUBSTR(m.boxdescrpt,19,2)))
		ELSE
			REPLACE newfile.colorpair WITH ;
				bits2color(word2num(SUBSTR(m.boxdescrpt,18,2)))
		ENDIF
	ENDIF
	IF m.filetype = frm_type
		REPLACE newfile.vpos WITH newfile.vpos + m.bandstart
	ENDIF
	m.boxdescrpt = FREAD(m.fhandle,m.boxdlength)
ENDDO

=FSEEK(m.fhandle,1-m.boxdlength,1)   && Rewind, we read an extra box descriptor above.

* Now get the text items, stepping through the layout band contents.
* This involves some concatenation of label stuff since Fox only
* allows one expression per line on labels.

PRIVATE size_row, this_row, this_col, scanned, packed_row, ;
	value_type, textlength, VALUE, attr_change, bytesread

this_row    = 0
bytesread   = 0
DO WHILE FSEEK(m.fhandle,0,1)  < bandend  && AND !FEOF(m.fhandle) && While bytes left in this band...
	gOTherm.Update(FSEEK(m.fhandle,0,1)/m.fsize * 100)			&& progress therm
	scanned = 0
	this_col = 0
	this_row = m.this_row + word2num(FREAD(m.fhandle, 2))
	size_row = word2num(FREAD(m.fhandle,2))
	IF size_row < 1
		LOOP
	ENDIF
	bytesread = m.bytesread + m.size_row
	packed_row = FREAD(m.fhandle, m.size_row)
	PRIVATE newcolor
	newcolor = ""

	* Flag values for tokens in a "text row."
	#DEFINE eorow           0
	#DEFINE skipcolumns     1
	#DEFINE k_field         2
	#DEFINE k_text          3
	#DEFINE styleattrib     4
	#DEFINE displattrib     5
	#DEFINE pagebreak       8
	PRIVATE newstyle
	newstyle = ""
	DO WHILE m.scanned < m.size_row - 1
		gOTherm.Update(FSEEK(m.fhandle,0,1)/m.fsize * 100)			&& progress therm
		scanned = m.scanned + 1
		value_type = ASC(SUBSTR(m.packed_row, m.scanned,1))

		m.value = SUBSTR(m.packed_row, m.scanned + 1)
		* May not be used, depends on value_type.

		DO CASE
			CASE m.value_type = eorow
				m.this_col = 0
				m.newstyle = ""
			CASE m.value_type = skipcolumns
				m.scanned = m.scanned + 1
				m.this_col = m.this_col + ASC(m.value)
			CASE m.value_type = k_field
				DO CASE
					CASE m.filetype = frm_type
						SCAN FOR newfile.objtype = 8 && fields
							IF newfile.vpos = m.this_row + m.bandstart AND ;
									newfile.hpos = m.this_col
								m.this_col = m.this_col + newfile.width
								EXIT
							ENDIF
						ENDSCAN
					CASE m.filetype = scr_type
						SCAN FOR newfile.objtype = 15 && fields
							IF newfile.vpos = m.this_row AND ;
									newfile.hpos = m.this_col
								m.this_col = m.this_col + newfile.width
								EXIT
							ENDIF
						ENDSCAN
					CASE m.filetype = lbl_type
						SELECT tempdbf
						SCAN
							IF tempdbf.vpos = m.this_row AND ;
									tempdbf.hpos = m.this_col
								SELECT newfile
								GOTO m.this_row + 3
								IF "" <> ALLTRIM(newfile.expr)
									REPLACE newfile.expr WITH ;
										ALLTRIM(newfile.expr) +  '+'
								ENDIF
								IF newfile.width < m.this_col
									REPLACE newfile.expr WITH     ;
										ALLTRIM(newfile.expr) +  '"' + ;
										SPACE(m.this_col - newfile.width) +;
										'"+'
								ENDIF
								REPLACE newfile.expr WITH newfile.expr + ALLTRIM(tempdbf.expr)
								m.this_col = m.this_col + tempdbf.width
								REPLACE newfile.width WITH m.this_col
								EXIT
							ENDIF
						ENDSCAN
						SELECT newfile
				ENDCASE

				*      newcolor = ""
				IF m.filetype = frm_type
					REPLACE newfile.style WITH m.newstyle
				ENDIF
				m.scanned = m.scanned + 4
			CASE m.value_type = k_text
				textlength = AT(CHR(0), m.value)
				IF m.filetype = scr_type OR m.filetype = frm_type
					APPEND BLANK
					REPLACE newfile.objtype WITH 5,           ;
						newfile.objcode WITH 0,           ;
						newfile.vpos WITH m.this_row,     ;
						newfile.hpos WITH m.this_col      ;
						newfile.height WITH 1,            ;
						newfile.width WITH textlength - 1
					IF m.filetype = scr_type
						REPLACE newfile.colorpair WITH m.newcolor
					ELSE
						REPLACE newfile.vpos WITH newfile.vpos + m.bandstart
					ENDIF
					*                        newcolor = ""
					REPLACE newfile.expr WITH newfile.expr + '"' +     ;
						LEFT(m.value,m.textlength-1) + '"'
				ELSE     && labels
					GOTO m.this_row + 3
					VALUE = LEFT(m.value,textlength-1)
					IF "" <> ALLTRIM(newfile.expr)
						REPLACE newfile.expr WITH newfile.expr +  ;
							'+'
					ENDIF
					IF newfile.width < m.this_col - 1
						VALUE = SPACE(m.this_col - newfile.width) ;
							+ m.value
						*                              textlength = m.textlength + m.this_col - newfile.width
					ENDIF
					REPLACE newfile.expr WITH newfile.expr + '"' +     ;
						m.value + '"'
					REPLACE newfile.width WITH m.this_col + LEN(m.value) &&textlength + 1
				ENDIF
				scanned = m.scanned + m.textlength
				this_col = m.this_col + m.textlength - 1
				IF m.filetype = frm_type
					REPLACE style WITH newstyle
				ENDIF
				*- Added by JD 12/29/93 -- check to see if text describes a line
				*- if so, convert to line type object!
				=cvtLine(@avline)
			CASE m.value_type = styleattrib
				newstyle = cvtstyle(word2num(LEFT(m.value, 2)))
				scanned = m.scanned + 2
			CASE m.value_type = displattrib
				scanned = m.scanned + 2
				newcolor = bits2color(word2num(LEFT(m.value, 2)))
			CASE m.value_type = pagebreak && N/A??
				*                  newstyle = ""
			OTHERWISE
				RETURN '"Text row" value type incorrect!'
		ENDCASE
	ENDDO
ENDDO
RETURN "OK"


*****************
FUNCTION falldown       && Show error message and cancel.
*****************
	PARAMETER m.pmessage
	IF !EMPTY(m.pmessage)
		=MESSAGEBOX(m.pmessage, 0 + 48, C_THERMTITLE_LOC)
	ENDIF
	=FCLOSE(m.fhandle)
	SET EXACT &oldexact.
	IF "" <> oldalias
		SELECT (oldalias)
	ENDIF
	IF TYPE("field(1,'newfile')") <> "U"
		SELECT newfile
		USE
	ENDIF
	IF TYPE("field(1,'tempdbf')") <> "U"
		SELECT tempdbf
		USE
		DELETE FILE &tempname..dbf
		DELETE FILE &tempname..fpt  &&changed from .dbt by bobfor
	ENDIF
	IF TYPE("m.targetname") = "C" AND FILE(m.targetname)
		DELETE FILE &m.targetname.
		PRIVATE m.memoname
		DO CASE
			CASE m.filetype = frm_type
				m.memoname = STRTRAN(m.targetname,"FRX","FRT")
			CASE m.filetype = scr_type
				m.memoname = STRTRAN(m.targetname,"SCX","SCT")
			CASE m.filetype = lbl_type
				m.memoname = STRTRAN(m.targetname,"LBX","LBT")
		ENDCASE
		IF FILE(m.memoname)
			DELETE FILE &m.memoname.
		ENDIF
	ENDIF
	RETURN
ENDFUNC

*****************
FUNCTION word2num       && Convert 2-byte word to Pro-type number.
PARAMETER bytes
RETURN ASC(m.bytes) + ASC(SUBSTR(m.bytes,2,1)) * 256

*******************
FUNCTION readstring     && Read a string formatted as 2-byte length + C-string.
PARAMETER phandle
PRIVATE m.strlength
m.strlength = word2num(FREAD(m.phandle,2))
IF m.strlength = 0
	RETURN ""
ENDIF
RETURN xtrim(FREAD(m.phandle, m.strlength))

**************
FUNCTION xtrim          && Trim spaces and nulls.
PARAMETER pstring
PRIVATE pos
m.pos = AT(CHR(0), m.pstring)
IF m.pos > 0
	m.pstring = LEFT(pstring, m.pos-1)
ELSE
	m.pos = LEN(m.pstring)
	DO WHILE m.pos > 1 AND ASC(RIGHT(m.pstring, 1)) = 0
		m.pstring = LEFT(m.pstring, LEN(m.pstring) - 1)
		m.pos = m.pos - 1
	ENDDO
ENDIF
IF m.pos = 1 AND ASC(m.pstring) = 0
	RETURN ""
ENDIF
RETURN ALLTRIM(m.pstring)

*******************
FUNCTION cvtstyle     && Change 2-word IV style code to Pro style string.
PARAMETER styleword
PRIVATE m.retstring, stylesstring
m.retstring = ""
stylesstring = "        BIURL"
bitval = 2^8
FOR bit = 9 TO  13
	IF isbitset(m.styleword, m.bitval)
		m.retstring = m.retstring + SUBSTR(m.stylesstring, m.bit, 1)
	ENDIF
	bitval = m.bitval * 2
ENDFOR
RETURN ALLTRIM(m.retstring)

*******************
FUNCTION bits2color     && Change 2-word IV color code to Pro color text.
*******************
	PARAMETER attrword
	#DEFINE defaultattr 32768     && If bit 15 set, use default attribute.
	PRIVATE m.retstring, colorstring
	m.retstring = ""
	colorstring = "BGR+BGR + U*"
	IF defaultattr = m.attrword
		RETURN ""
	ENDIF
	bitval = 1
	FOR bit = 1 TO  4
		IF isbitset(m.attrword, m.bitval)
			m.retstring = m.retstring + SUBSTR(m.colorstring, m.bit, 1)
		ENDIF
		bitval = m.bitval * 2
	ENDFOR
	IF "" = m.retstring
		m.retstring = "N"
	ENDIF
	m.retstring = m.retstring + "/"
	FOR bit = 5 TO 12
		IF isbitset(m.attrword, m.bitval)
			m.retstring = m.retstring + SUBSTR(m.colorstring, m.bit, 1)
		ENDIF
		bitval = m.bitval * 2
	ENDFOR
	RETURN STRTRAN(m.retstring,"BGR","W")
ENDFUNC

*****************
FUNCTION isbitset
*****************
	PARAMETERS bitfield, bitval
	IF MOD(m.bitfield, m.bitval*2) / m.bitval >= 1
		RETURN .T.
	ENDIF
	RETURN .F.
ENDFUNC

****************
FUNCTION ok2nuke
****************
*- Emulate SAFETY ON

	PARAMETER filename

	PRIVATE m.nresult

	m.nresult = MESSAGEBOX(JustFName(UPPER(m.filename)) + C_OVERWRITE_LOC,4 + 32 + 256,C_THERMTITLE_LOC)
	*- fix return values (jd 6/24/94)
	RETURN (m.nresult = IDYES)
ENDFUNC

*- eof MIGDB4.PRG