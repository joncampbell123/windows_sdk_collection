*- Foreign.PRG
*-
*- FoxPro 3.0 Converter Utility - Foreign File Conversion Parts
*-
*- (c) Microsoft Corporation 1995
*-


#INCLUDE "convert.h"
#INCLUDE "foreign.h"

EXTERNAL ARRAY gAShowMe

**********************************************
DEFINE CLASS ForeignConverterBase AS ConverterBase
**********************************************
	*- This is a parent class for all classes 
	*- that migrate files fron non-FoxPro platforms
	*- including FoxBASE+ and DBase

	*----------------------------------
	PROCEDURE Create25SCX
	*----------------------------------
		*- create a 2.5 type screen file, which
		*- can be converted later to a 3.0 scx file

		PARAMETER cNew25File

		SELECT(SELECT(1))
		CREATE DBF (m.cNew25File)   ;
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
			  "unique"    l,       ;
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

		IF USED("newfile")
			USE IN newfile
		ENDIF
		USE (m.cNew25File) ALIAS newfile EXCLUSIVE
		RETURN
	ENDPROC


ENDDEFINE


**********************************************
DEFINE CLASS DB4CatConverter AS FPCConverter
**********************************************

	fmtConverterClass = "FmtConverter"
	cErrStr = ""

	*------------------------------------
	PROCEDURE Init
	*------------------------------------
		*- only if called from Project
		PARAMETER aParms

		*- THIS.aParms[2] = m.pFiletype
		*- THIS.aParms[3] = m.pVersion
		*- THIS.aParms[4] = m.pFilename

		LOCAL m.nct2, m.nct3

		gOPJX = THIS

		SET ESCAPE ON
		ON ESCAPE DO EscHandler

		THIS.isproj = .F.		&& not really a project

		THIS.nTimeStamp = THIS.TStamp()
		THIS.pjxName = aParms[4]
		THIS.pjxVersion = aParms[3]
		THIS.cBackDir = aParms[1]

		THIS.lDevMode = aParms[7]
		THIS.cCodeFile = aParms[8]
		THIS.lLog = aParms[9]
		THIS.cLogFile = aParms[10]
		THIS.cCurrentFile = THIS.pjxName

		THIS.WriteLog(C_CONVLOG_LOC + THIS.pjxName,"")
		THIS.WriteLog(C_CONVVERS_LOC + C_CONVERSION_LOC,"")
		THIS.WriteLog("","")

		THIS.pjx25Alias = THIS.OpenFile(THIS.pjxName)

		IF EMPTY(THIS.pjx25Alias)
			=MESSAGEBOX(E_NOOPEN_LOC + THIS.pjxName + ".")
			THIS.lHadError = .T.
			RETURN
		ENDIF

		*- don;t show dialog for files when they get transported
		FOR i = 1 TO N_MAXTRANFILETYPES
			gAShowMe[i,1] = .F.		&& show the dialog?
			gAShowMe[i,2] = 1		&& choice
			gAShowMe[i,3] = ""		&& font name
			gAShowMe[i,4] = 0		&& font size
			gAShowMe[i,5] = ""		&& font style
			gAShowMe[i,6] = ""		&& from platform
		NEXT

		*- Add records from old PJX file
		SELECT (THIS.pjx25Alias)
		COUNT FOR !DELETED() AND type = "scr" TO m.nct2
		COUNT FOR !DELETED() TO m.nct3
		THIS.nScreenSets = m.nct2 + m.nct3
		THIS.curscxid = 0

		gOTherm.Update2(0,C_CAT2FPC_LOC)
		
		gOTherm.visible = .T.

		IF !THIS.CATConv()
			*- error message displayed in CATConv()
			THIS.lHadError = .T.
			RETURN
		ENDIF

		gOTherm.Update2(.03,c_BACKFILES_LOC)

		THIS.cBackDir = ADDBS(THIS.cBackDir)
		THIS.cHomeDir = ADDBS(JustPath(THIS.pjxName))

		IF gOMaster.lHadError
			THIS.CloseFiles
			THIS.lHadError = .T.
			RETURN .F.
		ENDIF

		gOTherm.Update2((1 - N_THERM2X) * 100,C_PROJTASK3_LOC)		&& update therm with next task

	ENDPROC		&&	DB4CatConverter:Init

	*------------------------------------
	PROCEDURE Error
	*------------------------------------
		PARAMETER ErrorNum, Method, Line
		THIS.lHadError = .T.
		*- check if minor error, which we can recover from
		IF THIS.lLocalErr
			THIS.cErrStr = ALLTRIM(STR(ERROR()))+' ('+MESSAGE()+')'
			RETURN
		ENDIF

		*- call the ancestor;s error handler
		Cvt::Error(ErrorNum, Method, Line)

	ENDPROC		&&  Error

	*------------------------------------
	FUNCTION Converter			&& DB4CatConverter
	*------------------------------------
		*- convert objects within the .CAT file first, to 2.6 version
		*- convert FPC itself
		*- convert each of the objects in it to 3.0

		PRIVATE cOld, cTmpFile

		cOld = THIS.pjx25Alias

		*- convert objects within the .CAT file first, to 2.6 version
		THIS.ConvertDB2FP()
		IF THIS.lHadError
			THIS.CloseFiles
			RETURN
		ENDIF

		IF USED(THIS.pjx25Alias)
			USE IN (THIS.pjx25Alias)
		ENDIF
		THIS.pjx25alias = THIS.new30alias

		*- call superclass converter
		RETURN FPCConverter::Converter()

	ENDPROC

	*------------------
	PROCEDURE ClosePJX		&& DB4CatConverter
	*------------------

		FPCConverter::ClosePJX

		*- erase the intermediate 2.6 catalog files
		IF FILE(FORCEEXT(THIS.cFull30PJXName,".FPC"))
			ERASE (FORCEEXT(THIS.cFull30PJXName,".FPC"))
		ENDIF
		IF FILE(FORCEEXT(THIS.cFull30PJXName,".FCT"))
			ERASE (FORCEEXT(THIS.cFull30PJXName,".FCT"))
		ENDIF

	ENDPROC		&& DB4CatConverter:ClosePJX

	*------------------------------------
	FUNCTION CatConv
	*------------------------------------
		*- Converts a dBASE catalog file to FoxPro 2.6 Catalog. 

		PRIVATE m.wziselect, m.wzsdirname, m.wzs, ;
			m.wzlreturn, ;
			m.wzsnewname, m.wzlautoname, m.wzstmpname, m.wzsDefault

		LOCAL m.ctmppath, m.cnewpath, cTmpFnameOld, cTmpFname

		m.wzsDefault = SET('default') + CURDIR()
		THIS.lLocalErr = .T.

		*- get catalog name, fix code page
		m.wzstmpname=''
		IF !EMPTY(CPCURRENT()) .AND. EMPTY(CPDBF())
			m.wzstmpname=AddBS(SYS(2023))+SYS(3)+'.CAT'
			DO WHILE FILE(m.wzstmpname)
				m.wzstmpname = AddBS(SYS(2023))+SYS(3)+'.CAT'
			ENDDO
			USE IN (THIS.pjx25Alias)
			COPY FILE (THIS.pjxName) TO (m.wzstmpname)
			IF THIS.lHadError
				=MESSAGEBOX(STRTRAN(E_COPY_LOC,"@1",THIS.cErrStr) + ;
					m.wzstmpname + ".")
				THIS.lHadError = .F.
				THIS.lLocalErr = .F.
				RETURN .F.
			ENDIF
			IF !cptag(m.wzstmpname,CPCURRENT(2))
				ERASE (m.wzstmpname)
				RETURN .F.
			ELSE
				USE (m.wzstmpname) ALIAS (THIS.pjx25Alias) IN 0
				IF THIS.lHadError
					=MESSAGEBOX(STRTRAN(E_OPEN_LOC,"@1",THIS.cErrStr) + ;
						m.wzstmpname + ".")
					ERASE (m.wzstmpname)
					THIS.lHadError = .F.
					THIS.lLocalErr = .F.
					RETURN .F.
				ENDIF
			ENDIF
		ENDIF
		THIS.lLocalErr = .F.

		*- get name for converted 2.6 (FPC) catalog
		m.wzlautoname=.F.
		m.wzsnewname=ForceExt(SYS(2027,THIS.pjxName),'FPC')
		IF FILE(m.wzsnewname) .OR. FILE(ForceExt(SYS(2027,m.wzsnewname),'FPT'))
			m.wzsnewname=AutoName(m.wzsnewname,'FPC',.T.)
			m.wzlautoname=.T.
		ENDIF
		
		m.wzlreturn=.T.

		*- make new FPC catalog
		IF EMPTY(THIS.FPCNew(m.wzsnewname,.T.,IIF(!EMPTY(CPCURRENT()),CPCURRENT(2),.F.)))
			DO WHILE .T.
				m.wzlreturn=.T.
				m.wzsdirname = JustPath(THIS.pjxName)
				m.wzsnewname=m.wzsdirname+ForceExt(JustFName(THIS.pjxName),'FPC')
				IF FILE(m.wzsnewname) .OR. FILE(ForceExt(SYS(2027,m.wzsnewname),'FPT'))
					m.wzsnewname=autoname(m.wzsnewname,'FPC',.T.)
					m.wzlautoname=.T.
				ELSE
					m.wzlautoname=.F.
				ENDIF
				IF !EMPTY(THIS.FPCNew(m.wzsnewname,.T.,IIF(!EMPTY(CPCURRENT()),CPCURRENT(2),.F.)))
					EXIT
				ENDIF
			ENDDO
		ENDIF

		*- move data from DB4 CAT to new FPC catalog
		IF m.wzlreturn
			APPEND FIELDS path, file_name, alias, type, ;
				title, code, tag FROM (DBF(THIS.pjx25Alias)) FOR !DELETED()
			m.ctmppath = JustPath(SYS(2027,THIS.pjxName))
			SET DEFAULT TO (m.ctmppath)
			REPLACE ALL path WITH FULLPATH(ALLTRIM(path)), ;
				file_name WITH ALLTRIM(file_name), ;
				title WITH ALLTRIM(title)
			m.ctmppath = JustPath(SYS(2027,m.wzsnewname))
			SET DEFAULT TO (m.ctmppath)
			IF !(JustPath(THIS.pjxName) == JustPath(THIS.pjxName))
				DO updpaths WITH THIS.pjxName && IN fpcopen
			ENDIF
			*- make sure files are there, and if not, ask user to locate them
			m.cnewpath = ""
			SCAN FOR !FILE(TRIM(path))
				IF !EMPTY(m.cnewpath)
					*- check where the last place we found files
					IF FILE(m.cnewpath + JUSTFNAME(path))
						REPLACE path WITH m.cnewpath + JUSTFNAME(path)
						LOOP
					ENDIF
				ENDIF
				m.cTmpFnameOld = path
				m.cTmpFname = ""
				m.cTmpFname = GETFILE(JUSTEXT(m.cTmpFnameOld),C_LOCFILE_LOC + JUSTFNAME(m.cTmpFnameOld))

				*- if found, update project
				IF !EMPTY(m.cTmpFname)
					REPLACE path WITH m.cTmpFname
					*- remember this location
					m.cnewpath = ADDBS(JUSTPATH(path))
				ENDIF
			ENDSCAN

			THIS.pjxName = m.wzsnewname
			IF m.wzlautoname
				*- let the user know an auto-name was used
				=MESSAGEBOX(E_NAMEPROB_LOC + THIS.pjxName + ".")
			ENDIF
		ENDIF
		
		IF !EMPTY(m.wzstmpname)
			USE IN (THIS.pjx25Alias)
			ERASE (m.wzstmpname)
		ENDIF

		THIS.pjxName = m.wzsnewname

		SET DEFAULT TO (m.wzsDefault)

		RETURN m.wzlreturn
		
	ENDPROC

	*------------------------------------
	FUNCTION ConvertDB2FP
	*------------------------------------
		*- Converts a each dBASE item to its 2.x equivalent
		LOCAL m.wzsDefault, m.wzsMode, m.ctmppath, wzsfpcalias, wzsFName

		m.wzsDefault = SET('default')+CURDIR()

		SELECT (THIS.new30alias)
		SCAN FOR !fox_file AND !DELETED()
			*- only look at db4files

			m.wzsMode = IIF(type = C_DB4SCREENTYPE,"FORM",;
						IIF(type = C_DB4SQLQUERYTYPE OR type = C_DB4UPQUERYTYPE, "QUERY",;
						IIF(type = C_DB4REPORTTYPE, "REPORT",;
						IIF(type = C_DB4LABELTYPE,"LABEL","OTHER"))))

			DO CASE
				CASE INLIST(m.wzsMode,'FORM','REPORT','LABEL','QUERY')

					m.ctmppath = JustPath(SYS(2027,ALLT(path)))
					m.wzsFName = path

					IF !FILE(m.wzsfname)
						*- file isn;t there, and they had a chance earlier to locate it
						*- so log it, and continue
						*- if not there, log the error and continue
						THIS.WriteLog(JUSTFNAME(STRTRAN(m.wzsfname,C_NULL)),E_NOFILE_LOC + C_CONVERT3c_LOC + E_NOBACKUP_LOC)
						LOOP
					ENDIF

					SET DEFAULT TO (m.ctmppath)
					IF !THIS.Migrate(@m.wzsFName, m.wzsMode)				&& do migration
						LOOP
					ELSE
						wzsfpcalias = THIS.new30alias
						DO CASE
							CASE m.wzsmode = 'QUERY'
								*- Migration of a .QBE could result in an auto-name, so
								*- update the filename to be sure.
								m.wziselect = SELECT()
								SELECT (THIS.new30alias)
								REPLACE path WITH UPPER(m.wzsfname)
								SELECT (m.wziselect)
								THIS.SetFoxFile(LOWER(justext(m.wzsfname)),.T.)
							CASE m.wzsmode = 'FORM'
								THIS.SetFoxFile(C_FPCSCREENTYPE,.T.)
								IF !_DOS
									DO (gTransport) WITH (&wzsfpcalias..path), 12, .F., gAShowMe,m.gOTherm,(&wzsfpcalias..path)
								ENDIF
							CASE m.wzsmode = 'REPORT'
								THIS.SetFoxFile(C_FPCREPORTTYPE,.T.)
								IF !_DOS
									DO (gTransport) WITH (&wzsfpcalias..path), 13, .F., gAShowMe,m.gOTherm,(&wzsfpcalias..path)
								ENDIF
							CASE m.wzsmode = 'LABEL'
								THIS.SetFoxFile(C_FPCLABELTYPE,.T.)
								IF !_DOS
									DO (gTransport) WITH (&wzsfpcalias..path), 14, .F., gAShowMe,m.gOTherm,(&wzsfpcalias..path)
								ENDIF
						ENDCASE

					ENDIF

					THIS.curscxid = THIS.curscxid + 1

					IF THIS.nScreenSets > 0
						gOTherm.Update2((THIS.curscxid/(THIS.nScreenSets + 1) + (1 - N_THERM2X)) * 100)
					ENDIF

				OTHERWISE
			ENDCASE

		ENDSCAN

		SET DEFAULT TO (m.wzsDefault)

		RETURN .T.

	ENDPROC	&& ConvertDB2FP

	*------------------------------------
	FUNCTION fpcnew
	*------------------------------------
		*- Creates a new FoxPro catalog file with the name
		*- m.wzsFName.
		
		PARAMETERS m.wzsfname, m.wzlleaveopen, m.wzicodepage
		PRIVATE m.wzsprompt, m.wzs, m.wziselect

		m.wziselect=SELECT()
		IF EMPTY(m.wzsfname)
			*- make sure old file gets erased (jd 5/12/94)
			m.wzsfname=putname('CATALOG','',.T.)
			IF EMPTY(m.wzsfname)
				RETURN ''
			ENDIF
		ENDIF

		THIS.lLocalErr = .T.
		
		*- NOTE: CHANGES MADE HERE NEED TO BE REFLECTED IN FPCOPEN.PRG!!!
		*- (FPCOpen() verifies the structure of the FPC being opened.)
		*- Elsewhere in the code, specific elements in this array are
		*- being referenced. Any change to the order of the fields
		*- requires a sweep through the code to check for usage.
		
		CREATE TABLE (m.wzsfname) ;
			(path		m, ;
			file_name	m, ;
			alias		C(10), ;
			type		C(3), ;
			title		m, ;
			code		N(6,0), ;
			tag			C(4), ;
			fox_file	l, ;
			indexes		m, ;
			wizard		l)

		THIS.new30alias = ALIAS()

		THIS.lLocalErr = .F.
		IF THIS.lHadError
			THIS.lHadError = .F.
			=MESSAGEBOX(STRTRAN(E_CREATE_LOC,"@1",THIS.cErrStr) + SYS(2027,m.wzsfname))
			RETURN ''
		ELSE
			IF !EMPTY(m.wzicodepage)
				USE
				IF !cptag(m.wzsfname,m.wzicodepage)
					ERASE (m.wzsfname)
					RETURN ''
				ENDIF
				THIS.lLocalErr = .T.
				USE (m.wzsfname)
				THIS.lLocalErr = .F.
				IF THIS.lHadError
					=MESSAGEBOX(STRTRAN(E_OPEN_LOC,"@1",THIS.cErrStr) + SYS(2027,m.wzsfname))
					ERASE (m.wzsfname)
					THIS.lHadError = .F.
					RETURN ''
				ENDIF
			ENDIF
			*- change filetype if mac
			IF _MAC
				PRIVATE wznerror
				wznerror = fxsettype(SYS(2027,m.wzsfname),'????','FOXX')
				IF m.wznerror <> 0
					*- error setting filetype
					=MESSAGEBOX(STRTRAN(E_OPEN_LOC,"@1",LTRIM(STR(m.wznerror))) + SYS(2027,m.wzsfname))
					ERASE (m.wzsfname)
					RETURN ''
				ENDIF
				RELEASE wznerror
			ENDIF

			*- add the FPC record
			THIS.addfpcrec
			IF !m.wzlleaveopen
				USE
				SELECT (m.wziselect)
			ENDIF
		ENDIF
		RETURN m.wzsfname

	ENDPROC


	*------------------------------------
	FUNCTION addfpcrec
	*------------------------------------
		APPEND BLANK
		REPLACE PATH WITH DBF(), TYPE WITH 'fpc', fox_file WITH .T.

	ENDPROC

	*------------------------------------
	FUNCTION SetFoxFile
	*------------------------------------
		PARAMETERS m.wzstype, m.wzlForceExt

		PRIVATE m.wzsselect, m.wzs1, m.wzs2

		m.wzsselect = SELECT()
		SELECT (THIS.new30alias)
		m.wzs1 = path
		m.wzs2 = file_name
		IF m.wzlForceExt
			REPLACE path WITH UPPER(ForceExt(SYS(2027,m.wzs1),m.wzstype)), ;
				file_name WITH UPPER(ForceExt(SYS(2027,m.wzs2),m.wzstype)), ;
				fox_file WITH .T., ;
				type WITH m.wzstype
		ELSE
			REPLACE fox_file WITH .T., ;
				type WITH m.wzstype
		ENDIF
		SELECT (m.wzsselect)

	ENDPROC

	*------------------------------------
	FUNCTION migrate
	*------------------------------------
		PARAMETERS m.wzsfname, m.wzstype, m.wzlconfirm
		
		PRIVATE m.wzsnewname, m.savearea

		LOCAL m.oThis, m.npos

		IF m.wzlconfirm
			IF MESSAGEBOX(STRTRAN(C_CONFIRM1_LOC,"@1",PROPER(m.wzstype)),MB_YESNO) = IDNO
				RETURN .F.
			ENDIF
		ENDIF

		THIS.WriteLog(m.wzsfname,E_MIGSTART_LOC)
		
		m.oThis = THIS
		
		DO CASE
			CASE m.wzstype='QUERY'
				*- strip off binary portion of QBE file, and
				*- write file out as a .PRG
				*- binary portion begins after a CTL-Z
				m.savearea = SELECT()

				IF USED("_FOX3SPR")
				  USE IN _FOX3SPR
				ENDIF

				CREATE CURSOR _FOX3SPR (temp1 m)
				APPEND BLANK

				m.wzsnewname = ForceExt(SYS(2027,m.wzsfname),'PRG')
				APPEND MEMO _FOX3SPR.temp1 FROM (m.wzsfname) OVERWRITE
				m.npos = AT(C_DBASEEOF,_FOX3SPR.temp1)
				IF m.npos > 0
					REPLACE _FOX3SPR.temp1 WITH LEFT(_FOX3SPR.temp1,m.npos - 1)
				ENDIF
				COPY MEMO _FOX3SPR.temp1 TO (m.wzsnewname)
				USE IN _FOX3SPR
				SELECT (m.savearea)

			CASE m.wzstype='FORM'
				m.wzsnewname=ForceExt(SYS(2027,m.wzsfname),'SCX')
				IF !MigDB4(m.wzsfname, @oThis)
					THIS.WriteLog(m.wzsfname,E_NOMIG_LOC)
					RETURN .F.
				ENDIF
			CASE m.wzstype='REPORT'
				m.wzsnewname=ForceExt(SYS(2027,m.wzsfname),'FRX')
				IF !MigDB4(m.wzsfname, @oThis)
					THIS.WriteLog(m.wzsfname,E_NOMIG_LOC)
					RETURN .F.
				ENDIF
			CASE m.wzstype='LABEL'
				m.wzsnewname=ForceExt(SYS(2027,m.wzsfname),'LBX')
				IF !MigDB4(m.wzsfname, @oThis)
					THIS.WriteLog(m.wzsfname,E_NOMIG_LOC)
					RETURN .F.
				ENDIF
		ENDCASE
		
		IF !FILE(m.wzsnewname)
			=MESSAGEBOX(STRTRAN(E_NOFIND_LOC,"@1",PROPER(m.wzsfname)))
			THIS.WriteLog(m.wzsfname,E_NOMIG_LOC)
			RETURN .F.
		ENDIF
		
		m.wzsfname=m.wzsnewname

		THIS.WriteLog(m.wzsfname,E_MIGEND_LOC)

		RETURN.T.

	ENDFUNC

ENDDEFINE	&& DB4CatConverter

**********************************************
DEFINE CLASS FmtConverter AS ForeignConverterBase
**********************************************

	lDBFOpen = .F.			&& flag when fmtDbf is open
	lThisOpen = .F.			&& flag set true if cThisAlias is open
	cThisAlias = ""
	cFmtAlias = ""			&& alias of work fmtDbf workarea
	lNixNewSCX = .F.		&& flag set .T. if need to erase new scx file when cancel
	barCount = 0
	cFmtDbf = ""
	chooseVar = 3
	finish = .F.			&& flag set to true if READ statement encountered; tells thermometer to fill
	cRootName = ""
	cPathName = ""
	cTempDBF = ""			&& name of table into which fmt is read--erase in cleanup
	cTempName = ""			&& alias of tempdbf -- close at cleanup
	lTempOpen = .F.			&& flag set true if tempDbf is open

	*------------------------------------
	PROCEDURE Init			&& FmtConverter
	*------------------------------------
		*- only if called from Project
		PARAMETER aParms
		
		LOCAL nFileReady
		
		THIS.old25file = aParms[4]
		THIS.cRootName = JustStem(THIS.old25file)
		THIS.cPathName = JustPath(THIS.old25file) + "\"
		THIS.cNew30File = LEFT(THIS.old25file,(LEN(THIS.old25file)-4)) + "." + C_SCXEXT

		*- Check to see if the fmt file is readable
		IF !Readable(THIS.old25file)
			=MESSAGEBOX(E_NOOPENSRC_LOC + ALLT(THIS.old25file))
			THIS.lHadError = .T.
			THIS.CleanUp
			RETURN
		ENDIF
		
		*- Check for a screen with same name; ask to overwrite if yes
		*IF FILE(THIS.cNew30File) AND !THIS.Ok2Nuke(THIS.cNew30File)
		*	THIS.CleanUp
		*	THIS.lHadError = .T.
		*	RETURN
		*ELSE
			THIS.lNixNewSCX = .T.	&& flag to let abort know to erase cNew30File
		*ENDIF

		RETURN
		
	*------------------------------------
	FUNCTION Converter			&& FmtConverter
	*------------------------------------
		
		DECLARE avline[256,4]               && keep track of vertical lines: for each col, 1 = start, 2 = end, 3 = colorpr, 4 = single?
		STORE -1 TO avline

		THIS.nTmpCount = 1
		THIS.nRecCount = 100

		gOTherm.SetTitle(C_THERMMSG13_LOC + LOWER(PARTIALFNAME(THIS.old25file,C_FILELEN)))
		gOTherm.Update(THIS.nTmpCount/THIS.nRecCount*100)

		THIS.cTempDBF = SYS(3)
		CREATE TABLE (THIS.cTempDBF) (LINE C(254))		&& Temporary table to hold text file
		THIS.cTempName = ALIAS()
		THIS.lTempOpen = .T.

		*- Read in the data from the text file
		APPEND FROM (THIS.old25file) SDF

		*- set codepage
		USE
		IF !CPTag(THIS.cTempDBF + ".DBF",CPCURRENT(2))
			*- reopen so Cleanup can dispose of properly
			USE (THIS.cTempDBF)
			THIS.CleanUp
			THIS.lHadError = .T.
			RETURN .F.
		ENDIF
		USE (THIS.cTempDBF) EXCLUSIVE

		THIS.nRecCount = RECC()

		THIS.Create25SCX(THIS.cNew30File)
		THIS.cThisAlias = ALIAS()
		THIS.lThisOpen = .T.

		APPEND BLANK
		REPLACE newfile.objtype WITH 1,  ;
			newfile.objcode WITH 63 &&10

		*- Options for Wizards
		*- window title, float, centered, single border, + support for 
		*- PG UP & PG DN
		REPLACE newfile.style WITH 2,;
				newfile.height WITH 20,;
				newfile.width WITH 76,;
				newfile.tag WITH '"' + PROPER(JustStem(THIS.old25file)) + '"',;
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
			APPEND MEMO newfile.proccode FROM mignavpr.txt OVERWRITE

			REPLACE newfile.environ with .T.

			*- determine which tables are used in the screen
			PRIVATE aScrTables
			DECLARE aScrTables[1]
			STORE "" TO aScrTables
			SELECT (THIS.cTempName)
			THIS.GetTables(@aScrTables)

			SELECT newfile
			m.ntables = ALEN(aScrTables)
			IF m.ntables = 1 AND EMPTY(aScrTables[1])
				*- no tables were found
			ELSE
				REPLACE environ WITH .T.
				FOR m.nctr = 1 TO ALEN(aScrTables)
					append blank
					replace objtype with 2, objcode with m.nctr, name with aScrTables[m.nctr] + ".DBF", unique with .T. ;
						tag with ALLT(aScrTables[m.nctr])
				NEXT
			ENDIF

			DIMENSION pname[10]                              &&define @ options names in an array
			pname[1]="SAY"
			pname[2]="GET"
			pname[3]="PICT"
			pname[4]="FUNC"
			pname[5]="VALID"
			pname[6]="WHEN"
			pname[7]="COLO"
			pname[8]="MESS"
			pname[9]="RANG"
			pname[10]="ERRO"

			SELECT (THIS.cTempName)
			*- Start processing each line in the temporary file
			SCAN
			   gOTherm.Update(RECNO()/RECC() * 100)
			   ucline=UPPER(ALLTRIM(line))
			   IF ucline="READ"
			      finish=.T.
			      gOTherm.Complete
			      EXIT
			   ENDIF

			   *- Ignore lines not starting with @ or those containing CLEAR TO or FILL TO
			   IF LEFT(ucline,1) <>"@" .OR. "CLEAR TO"$ucline .OR. "FILL TO"$ucline .OR. "PROM"$ucline .OR. "MENU"$ucline
			      LOOP
			   ENDIF
			   getsay = .F.                                    && is there a get and a say
			   
			   txt = ALLTRIM(SUBS(LINE,AT("@",LINE)+1))
			   IF RIGHT(txt,1)=";"                           && line continues
			      txt=";"
			      SCAN WHILE RIGHT(txt,1)=";"
			         txt=LEFT(txt,LEN(txt)-1)+ALLTRIM(LINE)
			      ENDSCAN
			      SKIP-1                                     && reposition pointer
			      txt=ALLTRIM(SUBS(txt,AT("@",txt)+1))       &&remove @
			   ENDIF
			   
			   DO WHILE "  "$txt                             &&remove double spaces
			      txt=STRTRAN(txt,"  "," ")                  &&remove double spaces
			   ENDDO
			   *- accommodate single quotes inside messages etc. (jd 6/23/94)
			   *txt=STRTRAN(txt,"'",'"')                      &&convert single quotes to double
			   txt=STRTR(txt,CHR(9)," ")                     &&Replace TABS with a space
			   m.row=LEFT(txt,AT(",",txt)-1)
			   txt=LTRIM(SUBS(txt,AT(",",txt)+1))
			   m.col=LEFT(txt,AT(" ",txt)-1)
			   IF " BOX "$ucline
			      txt=LTRIM(SUBS(txt,AT(",",txt)+1))
			   ELSE
			      txt=LTRIM(SUBS(txt,AT(" ",txt)+1))
			   ENDIF
			   SELECT (THIS.cThisAlias)
			   APPEND BLANK
			   REPLACE vpos WITH VAL(m.row),hpos WITH VAL(m.col)
			   
			   *- LINE OR BOX
			   IF UPPER(LEFT(txt,3))="TO" .OR. " BOX "$ucline
			      IF " DOUB"$ucline
			         REPLACE objcode WITH 5,objtype WITH 7,fillchar WITH CHR(0)
			      ELSE
			         REPLACE objcode WITH 4,objtype WITH 7,fillchar WITH CHR(0)
			      ENDIF
			      IF UPPER(txt)="TO"
			         txt=SUBS(txt,3)
			      ENDIF
			      m.height=VAL(LEFT(txt,AT(",",txt)-1))+1
			      txt=SUBS(txt,AT(",",txt)+1)
			      m.width=VAL(txt)+1
			      REPLACE HEIGHT WITH m.height-vpos,WIDTH WITH IIF(m.width-hpos=0,1,m.width-hpos)
				  =cvtLine(@avline)
			      LOOP
			   ENDIF
			   
			   DIMENSION gpos[11],options[10]
			   FOR x=1 TO 10
			      gpos[X]=AT(pname[X],UPPER(txt))
			   ENDFOR
			   gpos[11]=LEN(txt)+1
			   =ASORT(gpos)
			   IF "GET"$ucline .AND. "SAY"$ucline
			      getsay=.T.
			   ENDIF
			   FOR x=1 TO 10
			      IF gpos[X]=0                               && option not used
			         options[X]=""
			         LOOP
			      ENDIF
			      options[X]=SUBS(txt,gpos[X],gpos[X+1]-gpos[X])
			      thisopt=ALLTRIM(SUBS(options[X],AT(" ",options[X])+1))
			      optname=LEFT(UPPER(options[X]),4)
			      
			      IF optname="SAY"
			         *- accommodate single quotes inside messages etc. (jd 6/23/94)
			         IF OCCURS('"',thisopt)=2 .AND. LEFT(thisopt,1)='"' .AND. RIGHT(thisopt,1)='"' OR ;
			            OCCURS("'",thisopt)=2 .AND. LEFT(thisopt,1)="'" .AND. RIGHT(thisopt,1)="'" &&text
			            REPLACE objtype WITH 5,objcode WITH 0,expr WITH thisopt,;
			               HEIGHT WITH 1,WIDTH WITH LEN(thisopt)-2 &&-2 for quotes
			         ELSE                                    &&expression
			            REPLACE objtype WITH 15,objcode WITH 0,expr WITH thisopt,;
			               HEIGHT WITH 1,WIDTH WITH 10,REFRESH WITH .T.
			         ENDIF
			      ENDIF
			      pictwidth=0                                &&set picture width to 0
			      IF optname="GET"                           &&get
			         IF getsay
			            thisvpos=vpos
			            thishpos=hpos+LEN(expr)-1
			            APPEND BLANK
			            REPLACE vpos WITH thisvpos,hpos WITH thishpos
			         ENDIF
			         IF " "$thisopt                          &&strip out options
			            thisopt=LEFT(thisopt,AT(" ",thisopt)-1)
			         ENDIF
			         REPLACE objtype WITH 15,objcode WITH 1,name WITH thisopt,;
			            HEIGHT WITH 1
			         thiswidth=0
			         IF THIS.ldbfopen
			            IF ">"$thisopt                       &&If alias found, find alias name
			               aname=LEFT(thisopt,AT(">",thisopt)-2)
			               fldname=SUBS(thisopt,AT(">",thisopt)+1) &&find field name
			            ELSE
			               aname=fmt_alias &&ALIAS(1)
			               fldname=thisopt
			            ENDIF
			            IF aname=fmt_alias
			            	thiswidth=FSIZE(fldname,aname)
			            ENDIF
			         ENDIF
			         IF thiswidth=0                          &&Maybe it's a variable
			            DO CASE
			            CASE TYPE(thisopt)="C"
			               thiswidth=LEN(&thisopt)
			            CASE TYPE(thisopt)="N"
			               thiswidth=LEN(STR(&thisopt))
			            CASE TYPE(thisopt)="D"
			               thiswidth=8
			            ENDCASE
			         ENDIF
			         REPLACE WIDTH WITH IIF(thiswidth=0,10,thiswidth)
			      ENDIF
			      IF optname="PICT"
			         REPLACE PICTURE WITH thisopt,WIDTH WITH LEN(PICTURE)-2
			      ENDIF
			      IF optname="FUNC"
			         IF LEN(PICTURE)>0
			            thispict=SUBS(PICTURE,2,LEN(PICTURE)-2)
			            thisopt='"@'+SUBS(thisopt,2,LEN(thisopt)-2)+' '+thispict
			         ELSE
			            thisopt=STUF(thisopt,2,0,"@")
			         ENDIF
			         REPLACE PICTURE WITH thisopt
			      ENDIF
			      IF optname="VALI"
			         REPLACE VALID WITH thisopt
			      ENDIF
			      IF optname="WHEN"
			         REPLACE WHEN WITH thisopt
			      ENDIF
			      IF optname="COLO"
			         thisopt=SUBS(thisopt,2,LEN(thisopt)-2)  &&STRIP OUT QUOTES
			         REPLACE colorpair WITH thisopt
			      ENDIF
			      IF optname="MESS"
			         REPLACE MESSAGE WITH thisopt
			      ENDIF
			      IF optname="RANG"
			         REPLACE rangelo WITH LEFT(thisopt,AT(",",thisopt)-1)
			         IF ","$thisopt
			            REPLACE rangehi WITH SUBS(thisopt,AT(",",thisopt)+1)
			         ENDIF
			      ENDIF
			      IF optname="ERRO"
			         REPLACE ERROR WITH thisopt
			      ENDIF
			   ENDFOR
			   SELECT (THIS.cTempName)
			ENDSCAN
			=FixVert(@avline)
			select (THIS.cThisAlias)
			replace all platform with "DOS", uniqueid with sys(2015)
			PRIVATE scrheight,m.nmaxwidth, m.nmaxheight
			GO TOP
			scrheight = newfile.height
			SCAN
				scrheight = MAX(m.scrheight, newfile.vpos + newfile.height)
			ENDSCAN
			GO TOP
			REPLACE newfile.height WITH m.scrheight
			*SORT ON vpos, hpos TO (m.targetname)
			*USE (m.targetname)
			CALCULATE MAX(hpos + width),MAX(vpos + height) FOR RECNO() > 1 ;
				TO m.nmaxwidth, m.nmaxheight
			GO TOP
			REPLACE width WITH MAX(width,m.nmaxwidth + 2),;
				height WITH MAX(height,m.nmaxheight + 2) 

			select (THIS.cTempName)

			THIS.cleanup


			RETURN .T.
			
	ENDFUNC		&&  Converter
	
	*------------------------------------
	FUNCTION GetTables			&& FmtConverter
	*------------------------------------
		PARAMETER aScrTables

		LOCAL ctable, noldlen
		
		*- go through open file, and look for table names
		*- assume table is open with text of FMT file, and selected
		
		LOCATE FOR "->" $ line
		DO WHILE NOT EOF()
			m.ctable = LEFT(line,AT("->",line) - 1)
			m.ctable = SUBS(m.ctable, RAT(" ",m.ctable) + 1)
			IF FILE(THIS.cpathname + m.ctable + ".DBF")
				*- add to array?
				IF ASCAN(aScrTables,m.ctable) = 0
					m.noldlen = ALEN(aScrTables)
					IF m.noldlen = 1 AND EMPTY(aScrTables[1])
						aScrTables[1] = ALLT(m.ctable)
					ELSE
						DECLARE aScrTables[m.noldlen + 1]
						aScrTables[m.noldlen + 1] = ALLT(m.ctable)
					ENDIF
				ENDIF
			ENDIF
			CONTINUE
		ENDDO
		RETURN

	ENDFUNC
	
	*------------------------------------
	PROCEDURE Cleanup			&& FmtConverter
	*------------------------------------
		   IF USED(THIS.cthisalias)
			   SELECT (THIS.cthisalias)
			   USE
		   ENDIF
		   
		   IF USED(THIS.cTempName)
				SELECT (THIS.cTempName)
		   		USE
		   		IF FILE(THIS.cTempDBF+".dbf")
					ERASE (THIS.cTempDBF+".dbf")
				ENDIF
		   ENDIF
		       
			IF USED(THIS.cfmtAlias)
				SELECT (THIS.cfmtAlias)
				USE
			ENDIF
			
		RETURN

	ENDFUNC
	

ENDDEFINE			&& FmtConverter

*-
*- eof Foreign.PRG
*-