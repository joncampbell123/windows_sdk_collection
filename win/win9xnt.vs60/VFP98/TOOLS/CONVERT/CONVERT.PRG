*- See C_CONVERSION_LOC in CONVERT.H for current version #

*- Visual FoxPro 3.0 Converter Utility
*- (c) Microsoft Corporation 1995,1996,1998
*-
*-
*- Developer Note:
*-
*- The following hidden properties are used in converted forms:
*-
*- 1)  ReleaseErase
*- 
*- 	This is used by controls to indicate that when they are
*- released, their image should remain on the form. Read uses this
*- because when you do after a READ terminates, the images of the
*- objects remain on the form.
*- 
*- 2)  ReleaseWindows
*- 
*- 	This is used by converted forms to support the Release
*- Windows checkbox in the generate or project manager dialog.
*- 
*- 3)  ErasePage
*- 
*- 	Nested READs within a form are accomplished using "invisible"
*- form pages.  When READ switches form pages, he doesn't want to erase
*- the objects at the prior read level, so he makes the ErasePage
*- property be false.
*- 
*- 
*- Changes: Support for Macintosh (10/16/95 jd)
*-          Support for Visual Foxpro 5.0 (05/16/96 jd)
*-

#INCLUDE "convert.h"

PARAMETER pFilename,pFiletype,pVersion,pProgCall

*- pFileName:	fully qualified name of file to convert (C)
*- pFileType:	type of file (C)
*- pVersion:	version of file (C)
*- pProgCall:	new param -- if .T., is being called from within another app (jd 07/25/96)

LOCAL i

PRIVATE gTransport			&& which transporter to use
PRIVATE gReturnVal			&& name of file to return to VFP
PRIVATE gLog				&& accumulate log file info
PRIVATE gError				&& global error flag
PRIVATE gAShowMe			&& display transporter dialog?
PRIVATE gOPJX				&& PJX object
PRIVATE gOMaster			&& Master object
PRIVATE gOTherm				&& thermometer object
PRIVATE giCallingProg		&& index into PROGRAM()

gTransport = ""
gReturnVal = -1
gLog = ""
gError = .F.

*- hold responses to transporter dialog
DIMENSION gAShowMe[N_MAXTRANFILETYPES,9]
FOR i = 1 TO N_MAXTRANFILETYPES
	gAShowMe[i,1] = .T.		&& show the dialog?
	gAShowMe[i,2] = 1		&& choice
	gAShowMe[i,3] = ""		&& font name
	gAShowMe[i,4] = 0		&& font size
	gAShowMe[i,5] = ""		&& font style
	gAShowMe[i,6] = ""		&& from platform
	gAShowMe[i,7] = .T.		&& convert new objects
	gAShowMe[i,8] = .T.		&& convert more recently modified objects
	gAShowMe[i,9] = .T.		&& replace all objects
NEXT

gOPJX = .NULL.
gOTherm = .NULL.
giCallingProg = 0

DO CASE
	CASE PARAMETERS() = 0
		*- new feature for selectively converting 3.0 SCX and VCX files
		m.pFileType = C_SCREENTYPEPARM
		m.pFileName = ""
		m.pVersion = C_30VERS
	CASE PARAMETERS() == 3 AND PROGRAM(0) = "CONVERT"
		*- okay -- classic mode of calling convert.app
	CASE TYPE("pProgCall") == 'L' AND pProgCall
		*- is being called from another program -- need to trap index into PROGRAM()
		LOCAL i
		FOR i = 1 TO 128
			IF PROGRAM() == PROGRAM(i)
				giCallingProg = MAX(1,i - 1)
				EXIT
			ENDIF
		NEXT
		*- note: if this fails (i.e., can;t locate current program in the PROGRAM()
		*- array, giCallingProg will remain at 0 (MASTER)
	OTHERWISE
		*- called with wrong parameters
		=MESSAGEBOX(E_BADCALL_LOC)
		RETURN gReturnVal
ENDCASE

IF VERSION(4) < C_LATESTVER
	=MESSAGEBOX(E_BADFOX1_LOC + C_LATESTVER + E_BADFOX2_LOC)
	RETURN gReturnVal
ENDIF

IF EMPTY(_transport)
	*- if not specified, use the built-in one
	gTransport = "transprt"
ELSE
	IF FILE(_transport)
		gTransport = _transport
	ELSE
		=MESSAGEBOX(E_NOTRANS_LOC)
		RETURN
	ENDIF
ENDIF

gOMaster = CREATE("MasterConvert",m.pFilename,m.pFiletype,m.pVersion)
IF TYPE("gOMaster") # "O"
	*- error creating object, so fail
	=MESSAGEBOX(E_NOSTART_LOC)
	RETURN gReturnVal
ENDIF

IF gOMaster.lHadError
	*- problem with parms, or some other kind of initialization problem
	=MESSAGEBOX(E_NOSTART_LOC)
ELSE
	gOMaster.DoConvert
ENDIF

gOPJX = .NULL.
gOTherm = .NULL.
gOMaster = .NULL.

RELEASE gOTherm, gOPJX, gOMaster

IF gError
	*- some kind of fatal error occurred, so clean up as best we can
	*- CLEAR ALL
	RETURN -1
ENDIF

RETURN gReturnVal

*------------------------------------------------*


**************************************************
******************* Classes **********************
**************************************************

*************************************
DEFINE CLASS Cvt AS custom
*************************************
	*- this class is a base class for all other classes
	*- and provides for certain properties and methods they have in common

	lHadError = .F.
	lLog = .F.				&& log file?
	cLogFile = ""			&& the file to write the log to
	lDevMode = .F.			&& dev mode?
	cCodeFile = ""			&& cCodeFile
	lLocalErr = .F.			&&
	lHadLocErr = .F.
	lShown1994 = .F.
	cCurrentFile = ""		&& the file currently being processed
	dtStartTime = DATETIME(1995,1,1)
	dtEndTime = DATETIME(1995,1,1)

	*------------------------------------
	PROCEDURE Error				&& Cvt
	*------------------------------------
		PARAMETER errorNum, method, line

		LOCAL iCtr, iFH, cErrMsg

		IF errorNum = 39	&&benign error sometimes caused by Gather
			RETURN
		ENDIF
		
		IF THIS.lLocalErr
			*- enable testing for certain conditions without 
			*- bringing down the house
			THIS.lHadLocErr = .T.
			RETURN
		ENDIF
			
		*- turn off Escape
		ON ESCAPE
		SET ESCAPE OFF

		IF errorNum = 0
			*- user pressed <escape>, and wants to cancel
			cErrMsg = C_ESCLOGMSG_LOC
		ELSE
			ON ERROR m.err = .T.		&& keep from returning here
			cErrMsg = E_FATAL1_LOC + C_CRLF + ;
					E_ERR1_LOC + message() + C_CRLF + ;
					E_ERR2_LOC + ALLT(STR(m.Errornum)) + C_CRLF + ;
					E_ERR3_LOC + m.Method + C_CRLF + ;
					E_ERR5_LOC + message(1)+ C_CRLF + ;
					E_ERR6_LOC + IIF(EMPTY(THIS.cCurrentFile),E_ERR7_LOC,THIS.cCurrentFile)

		ENDIF

		THIS.lHadError = .T.

		*- force write an error to a logfile
		IF EMPTY(THIS.cLogFile) AND errorNum # 0
			*- not logging, so make up a name
			THIS.cLogFile = SET("DEFA") + CURDIR() + C_ERRLOG_LOC + '.' + C_LOGEXT
			iCtr = 1
			DO WHILE FILE(THIS.cLogFile) AND m.iCtr < 99
				THIS.cLogFile = SET("DEFA") + CURDIR() + LEFT(C_ERRLOG_LOC,6) + RIGHT(STR(iCtr + 100),2) + '.' + C_LOGEXT
				m.iCtr = m.iCtr + 1
			ENDDO
		ENDIF

		*- open logfile
		IF !EMPTY(THIS.cLogFile)
			IF m.errornum == I_DISKFULLERR
				*- disk is full, so use MESSAGEBOX
				=MESSAGEBOX(E_DISKFULL_LOC)
			ELSE
				iFH = FCREATE(THIS.cLogFile)
				IF iFH >= 0
					*- opened file okay
					*- add error info
					IF TYPE("glog") # 'C'
						*- something happened -- maybe the Transporter killed it
						gLog = ""
					ENDIF
					m.gLog = m.gLog + C_CRLF + m.cErrMsg + C_CRLF
					=FWRITE(iFH,m.gLog)
					=FCLOSE(iFH)
				ENDIF
			ENDIF		&& disk is full
		ENDIF

		IF L_DEBUG AND L_DEBUGSUSPEND
			ACTI WIND DEBUG
			SUSPEND
		ENDIF

		*- attempt to extricate from this mess
		*- maybe the erring object was an object on a screen
		DO CASE
			CASE TYPE("THIS.formRef") = 'O'
				*- true if SCX Object
				THIS.formRef.Cleanup
				IF THIS.formRef.projCall
					*- the erring object was part of a project
					gOPJX.Cleanup
				ENDIF
			CASE TYPE("THIS.projcall") = "L"
				*- true if a screen or report
				IF THIS.projCall
					gOPJX.Cleanup
				ELSE
					THIS.Cleanup
				ENDIF
			OTHERWISE
			*CASE TYPE("THIS.pjx25alias") = "C"
				*- true if a project
				THIS.Cleanup
		ENDCASE
		IF ErrorNum # 0
			gOTherm = .NULL.
			IF m.errornum # I_DISKFULLERR
				IF MESSAGEBOX(E_FATAL_LOC + E_FATAL2_LOC + SYS(2027,THIS.cLogFile) + E_FATAL3_LOC,MB_YESNO) = IDYES
					MODIFY FILE (THIS.cLogFile)
				ENDIF
			ENDIF
		ENDIF
		gError = .T.
		IF giCallingProg > 1
			*- special case, if converter was called from within a program (jd 07/25/96)
			LOCAL cCallingProg
			cCallingProg = PROGRAM(giCallingProg)
			RETURN TO &cCallingProg
		ELSE
			RETURN TO MASTER
		ENDIF
	
	ENDPROC		&&  Error

	*------------------------------------
	PROCEDURE WriteLog			&& Cvt
	*------------------------------------
		PARAMETER cFileName,cAction

		*- changed to always accumulate log -- THIS.llog isn't always available! (jd 02/13/96)
		m.gLog = m.gLog + UPPER(cFileName) + IIF(EMPTY(cAction),"",": " + cAction) + C_CRLF

	ENDPROC

	*------------------------------------
	PROCEDURE BeginLog			&& Cvt
	*------------------------------------
		PARAMETER cFileName

		THIS.dtStartTime = DATETIME()
		THIS.WriteLog(m.cFileName,C_BEGIN_LOC + " " + TTOC(DATETIME()))

	ENDPROC

	*------------------------------------
	PROCEDURE EndLog			&& Cvt
	*------------------------------------
		PARAMETER cFileName

		THIS.dtEndTime = DATETIME()
		THIS.WriteLog(SYS(2027,m.cFileName),C_END_LOC + ": " + TTOC(DATETIME()) + " " + ;
			IIF(THIS.dtStartTime # DATETIME(1995,1,1),"(" + C_SUCCESSCONV_LOC + LTRIM(STR(THIS.dtEndTime - THIS.dtStartTime)) + ;
			C_SECONDS_LOC + ")",""))

	ENDPROC


	*------------------------------------
	PROCEDURE IsDBF				&& Cvt
	*------------------------------------
	*- Check to see if the file can be opened as a DBF

		PARAMETER cFile

		LOCAL m.lDBF, m.savearea

		m.lDBF = .T.

		m.savearea = SELECT()
		SELECT 0

		THIS.lLocalErr = .T.
		USE (m.cFile) 
		THIS.lLocalErr = .F.

		IF THIS.lHadLocErr
			m.lDBF = .F.
			THIS.lHadLocErr = .F.
		ENDIF
		USE

		SELECT (m.savearea)

		RETURN m.lDBF

	ENDPROC

	*------------------------------------
	PROCEDURE GetPlatformCount
	*------------------------------------
		*- Get a list of the platforms in this file.
		*- return in passed array

		PARAMETER cSCXFile, aPlatforms

		IF !FILE(cSCXFile) OR !Readable(m.cSCXFile) OR !THIS.IsDBF(cSCXFile)
			*- We have a problem. Return and it will be sorted out later
			RETURN
		ENDIF

		USE (cSCXFile) ALIAS _temppct IN 0
		SELECT DISTINCT platform ;
			FROM _temppct ;
			WHERE !DELETED() ;
			INTO ARRAY aPlatforms

		USE IN _temppct

	ENDPROC

	*------------------------------------
	FUNCTION GetPlatform		&& ConverterBase
	*------------------------------------
		PARAMETER iPlatform
		*- need to deal with creating files for other platforms than the current one
		iPlatform = IIF(PARAMETERS() = 0, 1, iPlatform)
		IF iPlatform = 1
			DO CASE
				CASE _windows
					RETURN C_WINDOWS
				CASE _mac
					RETURN C_MAC
				CASE _dos
					RETURN C_DOS
				CASE _unix	
					RETURN C_UNIX
			ENDCASE
		ELSE
			DO CASE
				CASE _windows
					RETURN C_MAC
				CASE _mac
					RETURN C_WINDOWS
			ENDCASE
		ENDIF
	ENDFUNC

	*------------------------------------
	PROCEDURE Cleanup			&& Cvt
	*------------------------------------
		*- this proc is called by Error, and tries to put things back the way they were
		*- this should be overridden, and given something to do
	ENDPROC

ENDDEFINE

*************************************
DEFINE CLASS MasterConvert AS Cvt
*************************************
	
	*- Classes which can be replaced with others if
	*- MasterConvert is sub-classed
	scxConverterClass = "SCXSingleScreenConverter"
	scx30ConverterClass = "SCX30Converter"
	pjxConverterClass = "PJXConverter"
	mnxConverterClass = ""
	frxConverterClass = "FRXConverter"
	fpcConverterClass = "FPCConverter"
	db4ScrConverterClass = "DB4ScrConverter"
	db4FrmConverterClass = "DB4FrmConverter"
	db4LblConverterClass = "DB4LblConverter"
	db4CatConverterClass = "DB4CatConverter"
	db4QbeConverterClass = "DB4QbeConverter"
	fmtConverterClass = "FmtConverter"
	
	*- Environment vars to save/restore
	PROTECTED oldtalk, oldsafe, oldnotify, oldMess, oldmemowid,;
		oldClass, oldProc, oldData, oldCompat, oldCollate, oldBlock,;
		oldTrBe, oldFullPath, oldUDFParm, oldOnEscape, oldDevo, oldDebug,;
		oldError, oldExact, oldDefault, oldKeyComp, oldCPDialog,;
		oldPoint, oldSep, oldPath, oldLibr

	*- the SET() function here stores the default values, not the current settings!
	oldtalk     = SET("TALK")
	oldsafe     = SET("SAFETY")
	oldescape   = SET("ESCAPE")
	oldOnEscape	= ON("ESCAPE")
	oldnotify   = SET("NOTIFY")
	oldmess     = SET("MESSAGE",1)
	oldmemowid  = SET("MEMOWIDTH")
	oldClass    = SET("CLASS")
	oldDefault	= SET("DEFA") + CURDIR()
	oldProc     = SET("PROC")
	oldData     = DBC()						&& remember full path to database (jd 04/15/96)
	oldCompat   = SET("COMPATIBLE")
	oldExclus   = SET("EXCLUSIVE")
	oldCollate  = SET("COLLATE")
	oldBlock    = SET("BLOCK")
	oldTrBe     = SET("TRBE")
	oldFullPath = SET("FULLPATH")
	oldUDFParm  = SET("UDFP")
	oldDevo		= SET("DEVE")
	oldDebug	= SET("DEBUG")
	oldError	= ON("ERROR")
	oldExact	= SET("EXACT")
	oldKeyComp	= SET("KEYCOMP")
	oldCPDialog	= SET("CPDIALOG")
	oldPoint	= SET("POINT")
	oldSep		= SET("SEPARATOR")
	oldPath		= SET("PATH")
	oldLibr		= SET("LIBRARY")			&& remember libraries -- may conflict with converter PROCs (jd 06/20/96)

	nCurrentWorkarea = 0
	nNewWorkarea = 0
	lUserCall = .T.				&& converter was called by program vs. _CONVERTER

	cBackDir = ""				&& backup directory for project conversions

	lHandled = .F.				&& has it been dealt with?

	*- PARAMETERS and other settings
	DIMENSION aConvParms[13]
	aConvParms[ 1] = ""			&& FP25 file name
	aConvParms[ 2] = ""			&& file type
	aConvParms[ 3] = ""			&& file version
	aConvParms[ 4] = ""			&& FP30 file name
	aConvParms[ 5] = .T.		&& platform only
	aConvParms[ 6] = .F.		&& special effect
	aConvParms[ 7] = .F.		&& developer mode
	aConvParms[ 8] = ""			&& code file if dev mode
	aConvParms[ 9] = .F.		&& create log file?
	aConvParms[10] = ""			&& logfile name
	aConvParms[11] = .T.		&& make backup
	aConvParms[12] = 1			&& only convert records for current platform
	aConvParms[13] = 1			&& which platform?

	*----------------------------------
	PROCEDURE Init		&& MasterConvert
	*----------------------------------
		PARAMETER pFilename, pFiletype, pVersion
	
		*- Check for program intercepted via FoxPro _CONVERTER
		*- Note:  If you are subclassing converter, make sure 
		*- to properly handle parameter.
		*- Converter now supports no params, so only check for file if something was passed
		IF PARAMETERS() = 3 AND TYPE('m.pFilename') = "C" ;
		  AND IIF(EMPTY(m.pFilename),.T.,FILE(m.pFilename)) AND ;
		  TYPE('m.pFiletype') = "C"
			THIS.lUserCall = .F.
			THIS.aConvParms[2] = m.pFiletype
			THIS.aConvParms[3] = m.pVersion
			THIS.aConvParms[4] = IIF(EMPTY(m.pFilename),"",SYS(2027,m.pFilename))			&& use platform specific name
		ELSE
			*- incorrect set of parms passed
			THIS.lHadError = .T.
			RETURN
		ENDIF

		THIS.nCurrentWorkarea = SELECT()
		THIS.lUserCall = .T.				&& converter was called by program vs. _CONVERTER

		*- values were set above
				
		SET TALK OFF
		SET SAFETY OFF
		SET NOTIFY OFF
		SET LIBRARY TO						&& close libraries -- may conflict with converter PROCs (jd 06/20/96)
		

		IF L_SHOWVERSION
			SET MESSAGE TO C_CONVERSION_LOC + IIF(L_DEBUG," -- DEBUGGING ON","")
		ELSE
			SET MESSAGE TO " "
		ENDIF

		SET ESCAPE OFF

		SET PROCEDURE TO "foreign" ADDITIVE		&& This has the subclasses for converting non-FoxPro files
		SET PROCEDURE TO "conprocs" ADDITIVE

		SET MEMOWIDTH TO 256
		SET COMPATIBLE OFF
		SET EXCLUSIVE ON
		SET COLLATE TO "MACHINE"
		SET BLOCK TO N_BLOCKSZ
		SET TRBETWEEN OFF
		SET FULLPATH ON
		SET UDFPARMS TO VALUE

		IF !L_DEBUG
			SET DEVELOPMENT OFF
			SET DEBUG OFF
		ELSE
			SET DEVELOPMENT ON
			SET DEBUG ON
		ENDIF

		SET EXACT OFF

		*- set KEYCOMP to appropriate platform
		IF _mac
			*SET KEYCOMP TO MAC
		ELSE
			SET KEYCOMP TO WINDOWS
		ENDIF

		*- turn off code page conversion during conversion
		SET CPDIALOG OFF

		ON ERROR

		*- for now, close data -- need to close only VFP databases when it;s working
		IF !EMPTY(THIS.olddata)
		*-	CLOSE DATABASE
			SET DATABASE TO
		ENDIF

		SET CLASS TO SprTherm ADDITIVE			&& thermometer classes
		SET CLASS TO CvtAlert ADDITIVE			&& alert classes

		SET POINT TO '.'						&& RED00NXM -- prevent invalid values in spinners  (jd 04/15/95)
		SET SEPARATOR TO ','

		SELECT 0
		THIS.nNewWorkarea = SELECT()

	ENDPROC		&&  Init
	
	*----------------------------------
	PROCEDURE Destroy
	*----------------------------------
		LOCAL cOld

		IF THIS.oldsafe = "ON"
			SET SAFETY ON
		ENDIF

		IF THIS.oldnotify = "ON"
			SET NOTIFY ON
		ENDIF

		IF EMPTY(THIS.oldmess)
			SET MESSAGE TO
		ELSE
			SET MESSAGE TO THIS.oldmess
		ENDIF

		IF THIS.oldtalk= "ON"
			SET TALK ON
		ENDIF

		SET MEMOWIDTH TO (THIS.oldmemowid)

		m.cOld = THIS.oldOnEscape
		ON ESCAPE &cOld

		m.cOld = THIS.oldescape
		SET ESCAPE &cOld

		m.cOld = THIS.oldCompat
		SET COMPATIBLE &cOld

		m.cOld = THIS.oldExclus
		SET EXCLUSIVE &cOld

		SET BLOCK TO THIS.oldBlock
		SET COLLATE TO THIS.oldCollate

		m.cOld = THIS.OldClass
		SET CLASS TO &cOld

		SET DEFAULT TO (THIS.oldDefault)

		IF "FOREIGN" $ SET("PROCEDURE")
			RELEASE PROCEDURE foreign
		ENDIF
		IF "CONPROCS" $ SET("PROCEDURE")
			RELEASE PROCEDURE conprocs
		ENDIF

		m.cOld = THIS.oldTrbe
		SET TRBETWEEN &cOld

		m.cOld = THIS.oldFullPath
		SET FULLPATH &cOld

		m.cOld = THIS.oldUDFParm
		SET UDFPARMS TO &cOld

		m.cOld = THIS.oldDevo
		SET DEVELOPMENT &cOld	&&crashes build 4.104

		m.cOld = THIS.oldDebug
		SET DEBUG &cOld

		m.cOld = THIS.oldExact
		SET EXACT &cOld

		m.cOld = THIS.oldKeyComp
		SET KEYCOMP TO &cOld

		m.cOld = THIS.oldCPDialog
		SET CPDIALOG &cOld

		SET POINT TO (THIS.oldPoint)		&& reset these values (jd 04/15/96)
		SET SEPARATOR TO (THIS.oldSep)

		m.cOld = THIS.oldPath				&& restore path, in case we changed it (jd 06/11/96)
		SET PATH TO &cOld
		
		m.cOld = THIS.oldLibr				&& remember libraries -- may conflict with converter PROCs (jd 06/20/96)
		SET LIBRARY TO &cOld
		
		m.cOld = THIS.oldError
		ON ERROR &cOld

		IF !EMPTY(THIS.olddata)				&& restore database (jd 04/15/96)
			OPEN DATABASE (THIS.olddata)
		ENDIF

		SELECT (THIS.nCurrentWorkarea)

	ENDPROC		&&  Masterconvert:Destroy


	*----------------------------------
	PROCEDURE DoConvert		&& masterconvert
	*----------------------------------
		PRIVATE g_platforms
		PRIVATE aParms, oConvObject
		PRIVATE g_platforms
		PRIVATE cConvType
		

		*- needed for GENSCRN stuff
		DIMENSION g_platforms[1]
		g_platforms = ""

		*- THIS.aConvParms[2] = m.pFiletype
		*- THIS.aConvParms[3] = m.pVersion
		*- THIS.aConvParms[4] = m.pFilename

		m.cConvType = THIS.aConvParms[2]
		
		DO CASE
			CASE m.cConvType = C_SCREENTYPEPARM AND THIS.aConvParms[3] = C_30VERS AND ;
				EMPTY(THIS.aConvParms[4])
				*- new option (6/10/96) -- converter was called with no parameters, so show dialog
				*- for selecting SCX and VCX files, for conversion
				*- get the file or folder to update
				
				
				m.iFileDir = 1
				m.cFile = ""
				m.lSCX = .T.
				m.lVCX = .T.
				m.lRecurse = .F.
				
				*- note: we use aParms[12] to hold the lSet30Defaults value
				
				IF !THIS.GetOpts("cvtdlog30scx",C_SELECTFILE_LOC)
					RETURN
				ENDIF

				THIS.aConvParms[1] = ""			&& this was filled in by GetOpts -- clear so it causes no problems later

				gOTherm = CREATEOBJ(C_THERMCLASS1,C_THERMTITLE_LOC, ;
					C_THERMMSG2_LOC + LOWER(PARTIALFNAME(THIS.aConvParms[4],C_FILELEN)))
				IF THIS.aConvParms[12] > 1
					gOTherm.Update(0)
				ENDIF
				
				*- if only doing a file, we don;t care about these values
				lSCX = IIF(iFileDir == 1,.T.,lSCX)
				lVCX = IIF(iFileDir == 1,.T.,lVCX)
				
				UpdateSCX(m.cFile, lRecurse)		&& in ConProcs.PRG
				
			CASE m.cConvType = C_SCREENTYPEPARM AND THIS.aConvParms[3] = C_30VERS
				*- a 3.0 SCX or VCX -- we need to re-compile, and maybe
				*- set properties that match VFP 3.0 defaults

				*- verify user wants to convert
				IF !THIS.GetOpts("cvtalert30scx",C_CONVERT1_LOC + PARTIALFNAME(THIS.aConvParms[4],C_FILELEN) + C_CONVERT2_LOC)
					RETURN
				ENDIF

				THIS.aConvParms[1] = ""			&& this was filled in by GetOpts -- clear so it causes no problems later

				gOTherm = CREATEOBJ(C_THERMCLASS1,C_THERMTITLE_LOC, ;
					C_THERMMSG2_LOC + LOWER(PARTIALFNAME(THIS.aConvParms[4],C_FILELEN)))
				IF THIS.aConvParms[12] > 1
					gOTherm.Update(0)
				ENDIF

				=ACOPY(THIS.aConvParms,aParms)
				oConvObject = CREATE(THIS.scx30ConverterClass, @aParms, .T.)

				IF TYPE("oConvObject") # 'O'
					*- object was not created
					THIS.lHadError = .T.
					gReturnVal = -1
					RETURN
				ENDIF

				IF oConvObject.lHadError
					*- error creating converter object: 
					*- assume error has already been presented to user
					THIS.lHadError = .T.
					RELEASE oConvObject
					gReturnVal = -1
					RETURN
				ENDIF

				gReturnVal = oConvObject.Converter()

				RELEASE oConvObject

			CASE m.cConvType = C_SCREENTYPEPARM
			
				*- determine number of platforms
				DIMENSION aPlatforms[1]
				aPlatforms[1] = ""
				THIS.GetPlatformCount(THIS.aConvParms[4], @aPlatforms)
				IF EMPTY(aPlatforms[1])
					*- was unable to determine platforms
					RETURN
				ENDIF
				IF ALEN(aPlatforms,1) > 1
					IF ASCAN(aPlatforms,C_WINDOWS) > 0 AND ASCAN(aPlatforms,C_MAC) > 0
						*- both platforms are there
						THIS.aConvParms[12] = 2
					ENDIF
				ENDIF

				*- verify user wants to convert
				IF !THIS.GetOpts("cvtalertscx",C_CONVERT1_LOC + PARTIALFNAME(THIS.aConvParms[4],C_FILELEN) + C_CONVERT2_LOC)
					RETURN
				ENDIF

				THIS.aConvParms[1] = ""			&& this was filled in by GetOpts -- clear so it causes no problems later

				gOTherm = CREATEOBJ(C_THERMCLASS1,C_THERMTITLE_LOC, ;
					C_THERMMSG2_LOC + LOWER(PARTIALFNAME(THIS.aConvParms[4],C_FILELEN)))
				gOTherm.Update(0)

				*- of both platforms are present, and user wants to convert both,
				*- need to create two files
				FOR kPlat = 1 TO THIS.aConvParms[12]

						=ACOPY(THIS.aConvParms,aParms)

						aParms[13] = m.kPlat

						oConvObject = CREATE(THIS.scxConverterClass, @aParms, (kPlat == 1))	&& only back up first time around

						IF TYPE("oConvObject") # 'O'
							*- object was not created
							THIS.lHadError = .T.
							gReturnVal = -1
						RETURN
					ENDIF
					
					IF oConvObject.lHadError
						*- error creating converter object: 
						*- assume error has already been presented to user
						THIS.lHadError = .T.
						RELEASE oConvObject
						gReturnVal = -1
						RETURN
					ENDIF

					gReturnVal = oConvObject.Converter()

					RELEASE oConvObject

				NEXT

				IF TYPE(gReturnVal) == "C"
					*- successful conversion
					*- if multiple platforms, return form name for the current platform
					IF THIS.aConvParms[12] > 1 AND _mac
						gReturnVal = JustStem(gReturnVal) + C_MACEXT + "." + JustExt(gReturnVal)
					ENDIF
				ENDIF

				SET MESSAGE TO C_PROJTASK1_LOC


				lHandled = .T.			&& it's been dealt with?


			CASE m.cConvType = C_PROJECTTYPEPARM OR ;
					m.cConvType = C_CATALOGTYPEPARM
				*- a project or catalog

				LOCAL theClass, cFileType, lMakeBackDir, cobjname, cCvtMsg
				PRIVATE cFileName

				cFileName = THIS.aConvParms[4]
				lMakeBackDir = .T.

				*- always show "both platforms" option if project...
				THIS.aConvParms[12] = 2
				
				cCvtMsg = C_CONVERT4_LOC

				DO CASE
					CASE m.cConvType = C_PROJECTTYPEPARM AND THIS.aConvParms[3] = "3.0"
						*- FP 3.x project
						m.theClass = THIS.pjxConverterClass
						m.cFileType = C_CONVERT3p_LOC
						m.cobjname = "cvtalertpjx3"
						cCvtMsg = C_CONVERT2_LOC
					CASE m.cConvType = C_PROJECTTYPEPARM
						*- FP 2.x project
						m.theClass = THIS.pjxConverterClass
						m.cFileType = C_CONVERT3p_LOC
						m.cobjname = "cvtalertpjx"
					CASE m.cConvType = C_CATALOGTYPEPARM AND THIS.aConvParms[3] = C_FOXVERSIONPARM
						*- FP 2.6 catalog
						m.theClass = THIS.fpcConverterClass
						m.cFileType = C_CONVERT3c_LOC
						m.cobjname = "cvtalertpjx"
					CASE m.cConvType = C_CATALOGTYPEPARM AND THIS.aConvParms[3] = C_DB4VERSIONPARM
						*- dBASE IV catalog
						lMakeBackDir = .F.
						m.theClass = THIS.db4CatConverterClass
						m.cFileType = C_CONVERT3c_LOC
						m.cobjname = "cvtalertcat"
					OTHERWISE
						*- ? error -- bad parm
						RETURN
				ENDCASE

				*- verify user wants to convert
				IF !THIS.GetOpts(m.cobjname, C_CONVERT3_LOC + m.cFileType + PARTIALFNAME(SYS(2027,pFileName),C_FILELEN) + m.cCvtMsg)
					RETURN
				ENDIF

				m.gOTherm = CREATEOBJ(C_THERMCLASS2,C_THERMTITLE_LOC, "", 0, 0,;
					C_THERMMSG1_LOC + LOWER(PARTIALFNAME(THIS.aConvParms[4],C_FILELEN)))
				gOTherm.Update(0)

				=ACOPY(THIS.aConvParms,aParms)
				oConvObject = CREATE(m.theClass, @aParms)

				IF TYPE("oConvObject") # 'O' OR THIS.lHadError
					*- object was not created
					THIS.lHadError = .T.
					RETURN
				ENDIF
				
				IF oConvObject.lHadError
					*- error creating converter object: 
					*- assume error has already been presented to user
					THIS.lHadError = .T.
					RELEASE oConvObject
					RETURN
				ENDIF

				SET MESSAGE TO C_PROJTASK4_LOC

				gReturnVal = oConvObject.Converter()

				RELEASE oConvObject

				lHandled = .T.			&& it's been dealt with?
				
			CASE m.cConvType = C_REPORTTYPEPARM

				*- verify user wants to convert
				IF !THIS.GetOpts("cvtalertfrx",C_CONVERT3_LOC + ;
					IIF(JUSTEXT(THIS.aConvParms[4]) = "LBX",C_CONVERT3l_LOC,C_CONVERT3r_LOC) + ;
					PARTIALFNAME(THIS.aConvParms[4],C_FILELEN) + C_CONVERT2_LOC)
					RETURN
				ENDIF

				THIS.aConvParms[1] = ""			&& this was filled in by GetOpts -- clear so it causes no problems later
				=ACOPY(THIS.aConvParms,aParms)

				gOTherm = CREATEOBJ(C_THERMCLASS1,C_THERMTITLE_LOC, ;
					C_THERMMSG2_LOC + LOWER(PARTIALFNAME(THIS.aConvParms[4],C_FILELEN)))
				gOTherm.Update(0)

				oConvObject = CREATE(THIS.frxConverterClass, @aParms, .T.)

				IF TYPE("oConvObject") # 'O'
					*- object was not created
					THIS.lHadError = .T.
					RETURN
				ENDIF
				
				IF oConvObject.lHadError
					*- error creating converter object: 
					*- assume error has already been presented to user
					oConvObject = .NULL.
					RELEASE oConvObject
					THIS.lHadError = .T.
					RETURN
				ENDIF
				
				IF oConvObject.lConverted
					*- file was transported from one 3.0 platform to another
					gReturnVal = oConvObject.cFRX2files
					RETURN
				ENDIF

				SET MESSAGE TO C_PROJTASK2_LOC

				gReturnVal = oConvObject.Converter()

				RELEASE oConvObject

				lHandled = .T.			&& it's been dealt with?

			
			CASE m.cConvType = C_LABELTYPEPARM
			
			CASE m.cConvType = C_MENUTYPEPARM
			
			CASE m.cConvType = C_DB4QUERYTYPEPARM
			
			CASE m.cConvType = C_DB4FORMTYPEPARM
			
			CASE m.cConvType = C_DB4REPORTTYPEPARM
			
			CASE m.cConvType = C_DB4LABELTYPEPARM
			
			CASE m.cConvType = C_FMTTYPEPARM
						
				m.gOTherm = CREATEOBJ(C_THERMCLASS1,C_THERMTITLE_LOC, ;
					C_THERMMSG3_LOC + LOWER(PARTIALFNAME(THIS.aConvParms[4],C_FILELEN)))
				gOTherm.Update(0)

				=ACOPY(THIS.aConvParms,aParms)
				oConvObject = CREATE(THIS.fmtConverterClass, @aParms)
				IF TYPE("oConvObject") # 'O'
					*- object was not created
					THIS.lHadError = .T.
					RETURN
				ENDIF
				
				IF oConvObject.lHadError
					*- error creating SCX object: 
					*- assume error has already been presented to user
					THIS.lHadError = .T.
					RELEASE oConvObject
					RETURN
				ENDIF

				gReturnVal = oConvObject.Converter()

				RELEASE oConvObject

				lHandled = .T.			&& it's been dealt with?

			CASE m.cConvType = C_FPLUSFRXTYPEPARM
			

		ENDCASE

	ENDPROC		&&  DoConvert

	*----------------------------------
	FUNCTION GetOpts		&& masterconvert
	*----------------------------------
		*- display confirmation dialog, with options
		PARAMETER objname,cMsg

		PRIVATE oAlert, cBackDir, cLogFile, cCodeFile, lLog, lDevMode, lBackUp
		PRIVATE nOptDev, nCvt, iPlatformCount

		STORE "" TO cBackDir, cLogFile, cCodeFile
		STORE .F. TO lLog, lDevMode
		lBackup = .T.
		iBothPlat = 0

		m.cBackDir = ADDBS(JUSTPATH(THIS.aConvParms[4]))

		nOptDev = 1
		nCvt = 0
		oAlert = CREATEOBJ(m.objname,cMsg)
		IF TYPE("oAlert.chkBothPlat") == "O"
			oAlert.chkBothPlat.Enabled = (THIS.aConvParms[12] > 1)
		ENDIF
		IF TYPE("oAlert.chkSet30Def") == "O"
			oAlert.chkSet30Def.Enabled = .T.
		ENDIF
		oAlert.Show
		oAlert = .NULL.
		RELEASE oALert
		IF m.nCvt # 1
			RETURN .F.
		ENDIF
		THIS.aConvParms[ 7] = m.lDevMode		&& developer mode
		THIS.aConvParms[ 8] = m.cCodeFile		&& code file if dev mode
		THIS.aConvParms[ 9] = m.llog			&& create log file?
		THIS.aConvParms[10] = m.cLogFile		&& logfile name
		THIS.aConvParms[ 1] = m.cBackDir		&& backup directory
		THIS.aConvParms[11] = m.lBackup			&& make backup
		THIS.aConvParms[12] = m.iBothPlat + 1	&& only convert screen records for current platform
	ENDFUNC

	*----------------------------------
	FUNCTION GetVersion
	*----------------------------------
		RETURN C_CONVERSION_LOC

	ENDFUNC

		
ENDDEFINE		&&  MasterConvert


**********************************************
DEFINE CLASS ConverterBase AS Cvt
**********************************************
*- This is the abstract base class for all converter 
*- objects. A converter object converts a particular
*- item, e.g., a project, a screen, etc.

	*- global instance variables
	nTimeStamp = 0 

	old25file = ""			&& old 2.5 file name
	c25alias = ""			&& old 2.5/2.6 alias
	cNew30File = ""			&& new 3.0 file name
	new30alias = ""			&& new 3.0 file alias  (or 4.0 for PJX)

	platonly = ""			&& platform only option (Windows)
	oldfiletype = ""		&& file type
	oldfilever = ""			&& file version
	platform = C_ALL		&& platform name

	cSetSkip = ""			&& SET SKIP TO commands
	lAutoOpen = .F.			&& Auto open tables in dataenvironment?
	lAutoClose = .F.		&& Auto close tables in dataenvironment?

	lTransDlog = .F.		&& Force the transporter dialog to show

	DIMENSION a_dimes[1]	&& arrays that need to be "externalized"
	a_dimes = ""

	*- these are used by most of the children, so put here
	nDeffont1 = 1 	&& default screen FONT(1) -- height
	nDeffont5 = 0 	&& default screen FONT(5) -- extra leading
	nDeffont6 = 1 	&& default screen FONT(1) -- width
	cDefcolor = ""	&& default background color

	nRecCount = 0
	nTmpCount = 1

	lBackup = .F.				&& backup files (as .S2X, .S2T)
	iPlatformCount = 1			&& only convert records for current platform

	*------------------
	PROCEDURE PreForm
	*------------------
		*- this is an external hook to preprocess
		*- form object via subclassing
	ENDPROC

	*------------------
	PROCEDURE PostForm
	*------------------
		*- this is an external hook to postprocess
		*- form object via subclassing
		
	ENDPROC

	*------------------------------------
	FUNCTION TStamp
	*------------------------------------
		* Generates a FoxPro 2.5-style timestamp
		PARAMETER wzpdate,wzptime
		PRIVATE d,t

		m.d = IIF(EMPTY(m.wzpdate),DATE(),m.wzpdate)
		m.t = IIF(EMPTY(m.wzptime),TIME(),m.wzptime)

		RETURN ((YEAR(m.d)-1980) * 2 ** 25);
	     + (MONTH(m.d)           * 2 ** 21);
	     + (DAY(m.d)             * 2 ** 16);
	     + (VAL(LEFT(m.t,2))     * 2 ** 11);
	     + (VAL(SUBSTR(m.t,4,2)) * 2 **  5);
	     +  VAL(RIGHT(m.t,2))
	ENDFUNC		&&  TStamp
	
	*---------------------------
	FUNCTION AddQuotes
	*---------------------------
		PARAMETER pstring
		DO CASE
		CASE AT('"',m.pstring) = 0
			RETURN '"' + m.pstring + '"'	
		CASE AT("'",m.pstring) = 0
			RETURN "'" + m.pstring + "'"	
		OTHERWISE
			RETURN '[' + m.pstring + ']'	
		ENDCASE
	ENDFUNC

	*---------------------------
	FUNCTION AddProp
	*---------------------------

		*- Adds property to property list
		*- Converts if necessary
		*- Parameters:
		*-	fp30prop - property name
		*-	fp25fld - 2.5 field contents
		*-	fp30parent - parent reference (optional if needed)
		PARAMETER fp30prop,fp25fld,fp30parent

		IF INLIST(m.fp30prop,;
					M_BACKCOLOR,;
					M_MODE,;
					M_PENSIZE,;
					M_FONTBOLD)
			IF THIS.IsDefault(fp30prop,m.fp25fld)
				RETURN
			ENDIF
		ENDIF

		DO CASE
			CASE EMPTY(m.fp30prop)
				RETURN
				
			CASE TYPE('m.fp25fld') = "C"
				DO CASE
					CASE LEN(m.fp25fld)=0
						RETURN
					CASE INLIST(m.fp30prop,;
						M_PEN,;
						M_BACKCOLOR,;
						M_FILLCOLOR,;
						M_DISFORECOLOR,;
						M_DISBACKCOLOR,;
						M_ITEMFORECOLOR,;
						M_ITEMBACKCOLOR,;
						M_DISITEMFORECOLOR,;
						M_DISITEMBACKCOLOR,;
						M_SELITEMBACKCOLOR,;
						M_BORDERCOLOR,;
						M_FPICTURE,;
						M_ICON)
						m.fp25fld = ALLTRIM(m.fp25fld)
					CASE INLIST(m.fp30prop,;
						M_FONTFACE,;
						M_NAME,;
						M_DATASOURCE,;
						M_ALIAS,;
						M_ORDER,;
						M_CHILDALIAS,;
						M_CHILDINDEXTAG,;
						M_PARENTALIAS,;
						M_PARENTINDEXEXPR,;
						M_INITIALALIAS,;
						M_ASSOCWINDS)
						*- catch indirect references here
				  		m.fp25fld = IIF(LEFT(ALLTRIM(m.fp25fld),1) = "(",;
				  			ALLTRIM(m.fp25fld),;
				  			THIS.addquotes(ALLTRIM(m.fp25fld)))
					CASE INLIST(m.fp30prop,;
							M_CURSORSRC)
						m.fp25fld = ALLTRIM(m.fp25fld)
					CASE INLIST(m.fp30prop,;
							M_CAPTION,;
							M_VALUE) AND LEFT(m.fp25fld,1) $ ["'[]
						*- an expression in a label/say
						*- or a value that needs parens
						m.fp25fld = "(" + ALLTRIM(m.fp25fld) + ")"
					CASE INLIST(m.fp30prop,;
							M_CAPTION) OR AT(LEFT(m.fp25fld,1),["'[]) # 0
						m.fp25fld = ALLTRIM(m.fp25fld)
					OTHERWISE
						m.fp25fld = "(" + ALLTRIM(m.fp25fld) + ")"
				ENDCASE
			CASE TYPE('m.fp25fld') = "N"
				m.fp25fld= ALLTRIM(STR(m.fp25fld))
			CASE TYPE('m.fp25fld') = "L"
				m.fp25fld= IIF(m.fp25fld,".T.",".F.")
			CASE TYPE('m.fp25fld') = "D"
				m.fp25fld= "{" + DTOC(m.fp25fld) + "}"
		ENDCASE
		
		IF TYPE('m.fp30parent')='C' AND !EMPTY(m.fp30parent)
			m.fp30prop = m.fp30parent + "." + m.fp30prop
		ENDIF

		IF LEN(m.fp25Fld) > 254		&& RED00N4G
			*- changed action here -- now logs error and continues on
			THIS.WriteLog(E_WARNING_LOC,E_PROPTOOLONG_LOC + ALLTRIM(STR(recno())))
			IF TYPE("oForm.cCurrentFile") == 'C'
				=MESSAGEBOX(E_WARNING_LOC + " - " + E_PROPTOOLONG_LOC + ALLTRIM(STR(recno())) + ;
				', ' + ALLT(oForm.cCurrentFile) + '.' + E_EXPRNOCONV_LOC + E_SEELOGFILE_LOC)
			ELSE
				=MESSAGEBOX(E_WARNING_LOC + " - " + E_PROPTOOLONG_LOC + ALLTRIM(STR(recno())) + '.' + ;
				E_EXPRNOCONV_LOC+ E_SEELOGFILE_LOC)
			ENDIF
			*- oForm.lHadError = .T.
			RETURN
		ENDIF
		
		THIS.fp3prop = THIS.fp3prop + ;
			m.fp30prop + C_SEP + m.fp25fld + C_CRLF

		RETURN
		
	ENDFUNC		&& AddProp
	
	*---------------------------
	FUNCTION IsDefault
	*---------------------------
		PARAMETER cProp,cValue

		DO CASE
			CASE TYPE('m.cValue') = "N"
				m.cValue = ALLT(STR(cValue))
			CASE TYPE('m.cValue') = "L"
				m.cValue = IIF(cValue,".T.",".F")
		ENDCASE

		DO CASE
			CASE cProp = M_BACKCOLOR
				RETURN .F.
				*RETURN (cValue == "255,255,255")
			CASE cProp = M_MODE
				RETURN (cValue == "1")
			CASE cProp = M_PENSIZE
				RETURN (cValue == "1")
			CASE cProp = M_FONTBOLD
				RETURN (cValue == ".T.")
			OTHERWISE
				RETURN .F.
		ENDCASE

	ENDFUNC

	*---------------------------
	FUNCTION ClearProp
	*---------------------------
		*- clear accumulated properties
		THIS.fp3prop = ""
		
	ENDFUNC		&& ClearProp

	*------------------------------------
	FUNCTION AddMethods			&& ConverterBase
	*------------------------------------
		PARAMETER newmethod,fp25fld,ctype,newprop
		*- newmethod - method if procedure
		*- ctype= 0 -> Expression
		*- ctype= 1 -> Proc
		*- newprop - property if expression
		*- if newprop empty used newmethod

		LOCAL cTmp, savearea, gendir, m.newmethod2, m.gendircount


		IF EMPTY(ALLT(m.fp25fld))
		  RETURN
		ENDIF

		IF m.ctype = 0	&& expression used
			IF PARAMETERS() > 3 && expression with property
				THIS.AddProp(m.newprop,m.fp25fld)
				RETURN
			ENDIF
			*- check for GENSCRN tricks
			IF INLIST(UPPER(LEFT(ALLTRIM(m.fp25fld),3)),".T.",".F.") ;
				AND LEN(ALLTRIM(m.fp25fld)) > 3
				m.fp25fld = LEFT(ALLTRIM(m.fp25fld),3) + ;
					" " + CHR(38) + CHR(38) + SUBSTR(ALLTRIM(m.fp25fld),4)
			ENDIF
			IF "SYS(16" $ UPPER(m.fp25fld)
				m.fp25fld = STRTRAN(m.fp25fld,"SYS(16","_SYS(16")
				m.fp25fld = STRTRAN(m.fp25fld,"sys(16","_sys(16")
				m.fp25fld = STRTRAN(m.fp25fld,"Sys(16","_Sys(16")
			ENDIF
			m.fp25fld = "RETURN " +  m.fp25fld
		ENDIF

		m.savearea = SELECT()
		
		REPLACE _FOX3SPR.temp1 WITH m.fp25fld
		REPLACE _FOX3SPR.temp4 WITH CleanWhite(_FOX3SPR.temp1)
		SELECT _FOX3SPR
		THIS.FindArry

		*- comment out #REGION directives
		DO WHILE .T.
			m.gendir = MemoFind("#REGI","temp4",.T.,.F.,.F.,.F.,.T.)
			IF m.gendir = C_NULL
				EXIT
			ENDIF

			*- Comment out directive
			_MLINE = 0
			nLine = MemoFind("#REGI","temp4",.T.,.T.,m.gendircount,.F.,.T.)
			cTmp = MLINE(temp1,nLine - 1)						&& set _MLINE
			REPLACE temp1 WITH STUFF(temp1,_MLINE + IIF(nLine <= 1,0,2),0,'*')
			cTmp = MLINE(temp4,nLine - 1)						&& set _MLINE
			REPLACE temp4 WITH STUFF(temp4,_MLINE + IIF(nLine <= 1,0,1),0,'*')
		ENDDO
		m.fp25fld = temp1

		IF USED(THIS.c25alias)
			SELECT (THIS.c25alias)
		ELSE
			SELECT (m.savearea)
		ENDIF

		*- see if multiple procs in the procedure section
		*- if so, strip out and move to .SPR file, since 3.0
		*- won't expect more than one proc in a method...
		*- there's no PROC or FUNC line for the VALID
		IF INLIST(m.newmethod,;
					M_VALID,;
					M_VALID2,;
					M_ERROR,;
					M_MESSAGE,;
					M_ACTIVATE,;
					M_DEACTIVATE,;
					M_SHOW,;
					M_WHEN,;
					M_WHEN2)
			m.cTmp = UPPER(m.fp25fld)
			IF OCCURS("PROC",m.cTmp) + OCCURS("FUNC",m.cTmp) > 0
				*- get other procs, and put them in SPR file
				*- use correct field name
				m.newmethod2 = IIF(UPPER(LEFT(m.newmethod,4)) = "READ",SUBS(m.newmethod,5),;
								IIF(UPPER(LEFT(m.newmethod,5)) = "ERROR",LEFT(m.newmethod,5),m.newmethod))
				DO extractprocs WITH 0,m.newmethod2
				*- Move the stuff to a holding place for now
				IF !EMPTY(_FOX3SPR.sprmemo) AND m.newmethod # M_SHOW
					REPLACE _FOX3SPR.temp3 WITH C_CRLF + _FOX3SPR.sprmemo ADDITIVE
					REPLACE _FOX3SPR.sprmemo WITH ""
				ENDIF
				IF INLIST(m.newmethod,;
					M_VALID2,;
					M_WHEN2)
					*- throw away everything up to first PROC or FUNC
					LOCAL iFunc,iProc
					cTmp = C_CR + STRTRAN(m.fp25fld,C_CRLF,C_CR)
					iFunc = AT(C_CR + "FUNC",cTmp)
					iProc = AT(C_CR + "PROC",cTmp)
					iFunc = IIF(iFunc = 0,99999999,iFunc)
					iProc = IIF(iProc = 0,99999999,iProc)
					m.fp25fld = SUBS(cTmp,2,MIN(iFunc, iProc) - 2)	&& we added a CR at the beginning
				ENDIF
			ENDIF
		ENDIF
		*- Add method code to fp3method
		IF !EMPTY(m.fp25fld)
			THIS.fp3method = THIS.fp3method + ;
				"PROCEDURE " + m.newmethod + C_CRLF + ;
					m.fp25fld + IIF(RIGHT(m.fp25fld,2) = C_CRLF OR ;
					 RIGHT(m.fp25fld,1) = C_CR,"",C_CRLF) + ;
				"ENDPROC" + C_CRLF + C_CRLF
		ENDIF
		
		SELECT (m.savearea)

		RETURN
	ENDFUNC		&& ConverterBase:AddMethods


	*--------------------------------------
	FUNCTION FindArry		&& ConverterBase
	*--------------------------------------
		*- look for DIMENSION or DECLARE, and add to list a_Dimes list, so they
		*- can be "externalized" in the .SPR file

		*- assume that code to be searched is in _FOX3SPR.temp1, and that table
		*- is open in the current work area
		*- also, code in temp4 is stripped of leading white space

		LOCAL m.nArryCt, m.cArryName, m.nArryLen, m.j, m.iPlace, m.cTemp, m.nCtr

		m.nArryCt = 1
		DO WHILE .T.
			m.cArryName = UPPER(MemoFind("DECL","temp4",.T.,.F.,m.nArryCt,.T.,.T.))
			IF m.cArryName = C_NULL
				EXIT
			ENDIF
			IF RIGHT(TRIM(m.cArryName),1) = ";"
				*- a continued DECLARE, so take some special measures
				m.cArryName = ""
				iPlace = MemoFind("DECL","temp4",.T.,.T.,m.nArryCt,.T.,.T.)
				IF m.iPlace = 0
					*- this should be an impossibility -- couldn;t find where it was
					EXIT
				ENDIF
				cTemp = MLINE(temp1,m.iPlace)
				*- check for continued DECLARE
				m.nCtr = 1
				DO WHILE .T.
					m.cArryName = m.cArryName + m.cTemp
					IF RIGHT(m.cTemp,1) # ";"
						EXIT
					ELSE
						m.cTemp = MLINE(temp1,m.iPlace + m.nCtr)
						m.nCtr = m.nCtr + 1
					ENDIF
				ENDDO
				m.cArryName = STRTRAN(SUBS(m.cArryName,AT("DECL",UPPER(m.cArryName)) + 4),";","")
			ENDIF
			m.nArryCt = m.nArryCt + 1
			m.nArryLen = ALEN(THIS.a_Dimes)
			IF !EMPTY(THIS.a_Dimes[1])
				DIMENSION THIS.a_Dimes[m.nArryLen + 1]
				m.nArryLen = m.nArryLen + 1
			ENDIF
			IF  UPPER(LEFT(m.cArryName,2)) == "A " OR ;
				UPPER(LEFT(m.cArryName,3)) == "AR " OR ;
				UPPER(LEFT(m.cArryName,4)) == "ARE "
				THIS.a_Dimes[m.nArryLen] = SUBS(m.cArryName,AT(' ',m.cArryName) + 1)
			ELSE
				THIS.a_Dimes[m.nArryLen] = m.cArryName
			ENDIF
		ENDDO
		m.nArryCt = 1
		DO WHILE .T.
			m.cArryName = UPPER(MemoFind("DIME","temp4",.T.,.F.,m.nArryCt,.T.,.T.))
			IF m.cArryName = C_NULL
				EXIT
			ENDIF
			IF RIGHT(TRIM(m.cArryName),1) = ";"
				*- a continued DECLARE, so take some special measures
				m.cArryName = ""
				iPlace = MemoFind("DIME","temp4",.T.,.T.,m.nArryCt,.T.,.T.)
				IF m.iPlace = 0
					*- this should be an impossibility -- couldn;t find where it was
					EXIT
				ENDIF
				cTemp = MLINE(temp1,m.iPlace)
				*- check for continued DECLARE
				m.nCtr = 1
				DO WHILE .T.
					m.cArryName = m.cArryName + m.cTemp
					IF RIGHT(m.cTemp,1) # ";"
						EXIT
					ELSE
						m.cTemp = MLINE(temp1,m.iPlace + m.nCtr)
						m.nCtr = m.nCtr + 1
					ENDIF
				ENDDO
				m.cArryName = STRTRAN(SUBS(m.cArryName,AT("DIME",UPPER(m.cArryName)) + 4),";","")
			ENDIF
			m.nArryCt = m.nArryCt + 1
			m.nArryLen = ALEN(THIS.a_Dimes)
			IF !EMPTY(THIS.a_Dimes[1])
				DIMENSION THIS.a_Dimes[m.nArryLen + 1]
				m.nArryLen = m.nArryLen + 1
			ENDIF
			IF  UPPER(LEFT(m.cArryName,2)) == "N " OR ;
				UPPER(LEFT(m.cArryName,3)) == "NS " OR ;
				UPPER(LEFT(m.cArryName,4)) == "NSI " OR ;
				UPPER(LEFT(m.cArryName,5)) == "NSIO " OR ;
				UPPER(LEFT(m.cArryName,6)) == "NSION "
				THIS.a_Dimes[m.nArryLen] = SUBS(m.cArryName,AT(' ',m.cArryName) + 1)
			ELSE
				THIS.a_Dimes[m.nArryLen] = m.cArryName
			ENDIF
		ENDDO

	ENDFUNC		&& ConverterBase:FindArry

	*------------------------------------
	FUNCTION OpenFile		&& ConverterBase
	*------------------------------------
		PARAMETER cFile

		PRIVATE cThisAlias
		LOCAL m.nfh
				
		*- cFile is the file to open
		m.cThisAlias= "S" + LEFT(SYS(3),7)

		SELECT 0
		
		*- now try to open file
		IF FILE(m.cFile)
			DO CASE
				CASE !Readable(m.cFile)
					*- file is already open
					THIS.WriteLog(JUSTFNAME(m.cFile),TRIM(E_FILE_LOC) + E_NOCONVERT2_LOC)
					=MESSAGEBOX(E_NOOPEN_LOC + m.cFile + ". " + E_FILEOPEN_LOC)
					THIS.lHadError = .T.
				CASE pReadOnly(cFile)
					*- if read only, we can;t do anything with it... (jd 04/15/95, RED00M77)
					THIS.WriteLog(JUSTFNAME(m.cFile),TRIM(E_FILE_LOC) + E_NOCONVERT3_LOC)
					=MESSAGEBOX(TRIM(m.cFile) + E_NOCONVERT3_LOC)
					THIS.lHadError = .T.
				CASE THIS.IsDBF(m.cFile)
					*- success
					USE (m.cFile) ALIAS (m.cThisAlias) EXCLUSIVE
				OTHERWISE
					THIS.WriteLog(JUSTFNAME(m.cFile),E_INVALDBF_LOC)
					=MESSAGEBOX(E_FILE_LOC + m.cFile + E_NOCONVERT2_LOC)
					THIS.lHadError = .T.
			ENDCASE
		ELSE
			*- log the error
			THIS.WriteLog(JUSTFNAME(m.cFile),TRIM(E_FILE_LOC) + E_NOCONVERT1_LOC)
			=MESSAGEBOX(E_FILE_LOC + m.cFile + E_NOCONVERT1_LOC)
			THIS.lHadError = .T.
		ENDIF

		*- Check if had error opening file
		IF THIS.lHadError
			RETURN ""
		ENDIF

		RETURN m.cThisAlias

	ENDFUNC		&& ConverterBase:OpenFile
	

	*------------------------------------
	FUNCTION GetVarPrefix		&& ConverterBase
	*------------------------------------
		PARAMETER cClassType

		DO CASE
			CASE cClassType = T_LABEL
				RETURN "lbl"
			CASE cClassType = T_SAY
				RETURN "txt"
			CASE cClassType = T_EDIT
				RETURN "edt"
			CASE cClassType = T_LINE
				RETURN "lin"
			CASE cClassType = T_SHAPE
				RETURN "shp"
			CASE cClassType = T_INV
				RETURN "cmg"
			CASE cClassType = T_BTNGRP OR;
				cClassType = T_BTN
				RETURN "cmg"
			CASE cClassType = T_PICT
				RETURN "img"
			CASE cClassType = T_RADIO
				RETURN "opt"
			CASE cClassType = T_RADIOGRP
				RETURN "opg"
			CASE cClassType = T_CBOX
				RETURN "chk"
			CASE cClassType = T_POPUP
				RETURN "cbo"
			CASE cClassType = T_SPIN
				RETURN "spn"
			CASE cClassType = T_OLE
				RETURN "ole"
			CASE cClassType = T_LIST
				RETURN "lst"
			CASE cClassType = T_DATANAV
				RETURN "de"
			CASE cClassType = T_CURSOR
				RETURN "crs"
			CASE cClassType = T_RELATION
				RETURN "rel"
			CASE cClassType = T_FSET
				RETURN "frs"
			CASE cClassType = T_FORM
				RETURN "frm"
			CASE cClassType = T_PAGE
				RETURN "pgf"
			OTHERWISE
				RETURN "c"
		ENDCASE
	ENDFUNC

	*------------------------------------
	FUNCTION Converter
	*------------------------------------
		*- this method should always be overridden
		WAIT WINDOW "Called Converter function that wasn't overridden!"
		RETURN -1
	ENDFUNC
	
	*------------------------------------
	FUNCTION ok2nuke
	*------------------------------------
		*- Emulate SAFETY ON

		PARAMETER cFileName
		LOCAL m.nresult

		DO CASE
			CASE _WINDOWS
				RETURN MESSAGEBOX(UPPER(JustFName(m.cFileName)) + C_OVERWRITE_LOC,MB_YESNO) = IDYES
			OTHERWISE
				*- not implemented yet
				RETURN .F.
		ENDCASE

	ENDFUNC		&& ok2nuke

	*----------------------------------
	PROCEDURE CompileFRX
	*----------------------------------
		*- take code from TAG field of DataEnvironment record, and 
		*- compile it as an FXP, then append it into the TAG2 field
		*- There is no "COMPILE REPORT" command
		*- Assume we are positioned on the appropriate record
		LOCAL cTmpFile

		cTmpFile = SYS(3) + ".PRG"
		
		IF !EMPTY(tag)
			COPY MEMO tag TO (cTmpFile)
			COMPILE (cTmpFile)
			IF !_mac
				APPEND MEMO tag2 FROM (FORCEEXT(cTmpFile,"FXP"))
			ELSE
				LOCAL iFH, iBuffer
				iFH = FOPEN(FORCEEXT(cTmpFile,"FXP"))
				IF iFH # -1
					DO WHILE !FEOF(iFH)
						iBuffer = FREAD(iFH, N_BUFFSZ)
						REPLACE tag2 WITH iBuffer ADDITIVE
					ENDDO
					=FCLOSE(iFH)
				ELSE
					*- some kind of error
				ENDIF
			ENDIF
			ERASE (cTmpFile)
			ERASE (FORCEEXT(cTmpFile,"FXP"))
		ENDIF
		RETURN

	ENDPROC		&& CompileFRX


ENDDEFINE		&&  ConverterBase 


**********************************************
DEFINE CLASS PJXConverterBase AS ConverterBase
**********************************************

	*- methods that might be changed in a sub-classed
	scxConverterClass = "SCXSingleScreenConverter"
	scx30ConverterClass = "SCX30Converter"
	frxConverterClass = "FRXConverter"

	curscxid = 1		&& project current setid
	highscxid = 0		&& project high setid
	isproj = .T.
	cHomeDir = ""
	cBackDir = ""
	cOutFile = ""
	cStubFile = ""
	pjxName = ""
	pjx25Alias = ""		&& 2.5 project alias
	pjxVersion = ""
	lIsMain = .F.
	lEncrypt = .F.
	lSaveCode = .T.		&& saveCode option (6.8.96 jd)
	cDevInfo = ""
	lDebug = .F.
	lExclude = .F.
	nScreenSets = 0		&& number of undeleted screen sets in project
	nScreenCtr = 0		&& counter for screens actually converted
	cFull30PJXName = ""	&& fully-qualifed name of new PJX
	f2Files = ""		&& holder for 2.x FRX file name
	f3files = ""		&& holder for 3.x FRX file name
	cMemoExt = ""		&& extension for memo files for files of this type

	DIMENSION a_pjxsets[10]

	*----------------------------------
	PROCEDURE CreatePJX		&& PJXConverterBase
	*----------------------------------
		PRIVATE cTmpPJXName

		m.cTmpPJXName = ADDBS(JUSTPATH(THIS.pjxName)) + "P" + LEFT(SYS(3),7) + ".PJX"

		*- create new PJX file
		CREATE TABLE (m.cTmpPJXName) ;
			(name        m,;
			type        c(1),;
			id			n(10),;
			timestamp   n(10),;
			outfile     m,;
			homedir     m,;
			exclude     l,;
			mainprog    l,;
			savecode    l,;
			debug       l,;
			encrypt     l,;
			nologo      l,;
			cmntstyle   n(1),;
			objrev      n(5),;
			devinfo     m,;
			symbols     m,;
			object      m,;
			ckval       n(6),;
			cpid        n(5),;
			ostype      c(4),;
			oscreator   c(4),;
			comments    m,;
			reserved1   m,;
			reserved2   m,;
			sccdata     m,;
			local       l,;
			key         c(32),;
			user		m)

		THIS.new30alias = ALIAS()

		*- Add header comment record  && added SaveCode (6.8.96 jd)
		INSERT INTO (THIS.new30alias) ;
			(name, timestamp, type, homedir, saveCode, objrev, debug, encrypt, devinfo);
			VALUES (JUSTFNAME(THIS.pjxName),THIS.nTimeStamp,;
				"H", JUSTPATH(THIS.pjxName) + C_NULL, ;
				THIS.lSaveCode, C_PJXVERSTAMP, THIS.lDebug, THIS.lEncrypt, THIS.cDevInfo)
		THIS.p30to40
		
	ENDPROC		&&   CreatePJX

	*----------------------------------
	PROCEDURE InsertSCX			&& PJXConverterBase
	*----------------------------------
		*- add appropriate record(s) to 3.0 PJX file
		*- additional work is done in subclassed InsertSCX
		IF !THIS.lDevMode
			*- if Dev Mode (aka "Visual Conversion"), don;t need SPRs
			*- also add record for [new] SPR file
			INSERT INTO (THIS.new30alias) ;
					(name,;
					timestamp,;
					type,;
					exclude,;
					mainprog,;
					cpid,;
					key);
				VALUES(;
					SYS(2014,THIS.cStubFile,DBF(THIS.new30alias)) + CHR(0),;
					THIS.nTimeStamp,;
					C_PRGTYPE,;
					THIS.lExclude,;
					THIS.lIsMain,;
					IIF(CPCURRENT() # 0, CPCURRENT(),CPDBF(THIS.new30alias)),;
					UPPER(LEFT(JUSTSTEM(THIS.cOutFile),LEN(key))))
		ENDIF

	ENDPROC		&& InsertSCX

	*------------------------------------
	PROCEDURE CloseFiles			&& PJXConverterBase
	*------------------------------------
		IF USED(THIS.pjx25Alias)
			USE IN (THIS.pjx25Alias)
		ENDIF

		IF USED(THIS.new30alias)
			IF THIS.lLog
				SELECT (THIS.new30alias)
				IF TYPE("user") = "M"
					GO TOP
					REPLACE user WITH m.gLog + C_CRLF + C_LOGEND_LOC + " [" + TTOC(DATETIME()) + "]" + C_CRLF
					COPY MEMO user TO (THIS.cLogFile)
				ENDIF
			ENDIF
			USE IN (THIS.new30alias)
		ENDIF			

	ENDPROC

	*------------------
	PROCEDURE BackFiles
	*------------------
		PARAMETER cField, cType, cDesc
		PRIVATE cBackName,cTmpFname, m.cTmpFnameOld, m.lNoFinds, m.savearea,;
			cCurDir, i, nCvt

		LOCAL cOldPath, cNewPath, cThisPath, cFName

		*- make sure project file locations are up-to-date
		m.savearea = SELECT()
		SELECT (THIS.pjx25Alias)

		*- copy first time only
		IF !FILE(THIS.cBackDir + JUSTFNAME(THIS.cCurrentFile)) AND THIS.lBackUp
			COPY FILE (THIS.cCurrentFile) TO THIS.cBackDir + JUSTFNAME(THIS.cCurrentFile)
			COPY FILE (FORCEEXT(THIS.cCurrentFile,THIS.cMemoExt)) TO (FORCEEXT(THIS.cBackDir + JUSTFNAME(THIS.cCurrentFile),THIS.cMemoExt))
		ENDIF

		cOldPath = SET("PATH")
		cNewPath = m.cOldPath
		
		nCvt = 0

		SCAN FOR !(type $ 'SH') AND !DELETED()			&& type $ m.cType AND !DELETED()
			cFName = ALLTRIM(IIF(AT(CHR(0),&cField) > 0,LEFT(&cField,AT(CHR(0),&cField)-1),&cField))
			IF !_MAC AND OCCURS(":",cFName) > 1
				*- FULLPATH will fail on this path, and since it is invalid for DOS etc.,
				*- translate ":" into "\" and let user find the files
				cFName = STRTRAN(cFName,":","\")
			ENDIF
			cTmpFname = FULLPATH(cFName,THIS.cHomeDir)
			IF !FILE(cTmpFname)
				*- it;s not in the place where it's supposed to be
				*- is it in the path?
				IF FILE(JustFName(cTmpFname))
					*- let Fox find the correct path, in the SET PATH
					m.cTmpFname = FULLPATH(JustFName(m.cTmpFname))
				ELSE
					*- try to locate file first
					IF m.nCvt = 3
						*- ignore all
						THIS.WriteLog(cTmpFname,E_FILE_LOC + JustFName(cTmpFname) + E_NOCONVERT1_LOC)
						LOOP
					ENDIF
					*- cvtLocate is based on the cvtAlertFrx. so it uses the same variables
					oLocate = CREATEOBJECT("cvtLocate",C_LOCFILE3_LOC)
					oLocate.Show
					RELEASE oLocate
					DO CASE
						CASE m.nCvt = 1
							*- locate
							m.cTmpFnameOld = m.cTmpFname
							m.cTmpFname = ""
							THIS.lLocalErr = .T.
							m.cTmpFname = LOCFILE(JUSTFNAME(m.cTmpFnameOld),JUSTEXT(m.cTmpFnameOld),C_LOCFILE2_LOC)
							THIS.lLocalErr = .F.
						CASE m.nCvt = 2 OR m.nCvt = 3
							*- ignore
							THIS.WriteLog(cTmpFname,E_FILE_LOC + JustFName(cTmpFname) + E_NOCONVERT1_LOC)
							LOOP
						OTHERWISE
							*- cancel
							m.cTmpFname = ""
					ENDCASE
				ENDIF

				*- if found, update project
				IF EMPTY(m.cTmpFname)
					*- cancelled, so quit conversion
					THIS.lHadError = .T.
					RETURN
				ELSE
					REPLACE &cField WITH m.cTmpFname
					*- and remember this location
					m.cThisPath = JUSTPATH(cTmpFname)
					IF !(m.cThisPath $ m.cNewPath)
						cNewPath = m.cNewPath + "," + m.cThisPath
						SET PATH TO &cNewPath
					ENDIF
				ENDIF
			ENDIF

		ENDSCAN

		*- restore path
		SET PATH TO &cOldPath

		SELECT (m.savearea)
		
		IF THIS.lBackUp
			gOTherm.Update2(0,C_BACKFILES_LOC)
		
			SELECT &cField, type FROM DBF(THIS.pjx25Alias) ;
			    WHERE type $ m.cType AND !DELETED() ;
				INTO ARRAY tmparr

			IF _TALLY = 0
				RETURN
			ENDIF

			m.lNoFinds = .F.

			FOR i = 1 TO _TALLY
				*- Copy files to backup directory
				m.cTmpFname = FULLPATH(ALLTRIM(IIF(AT(CHR(0),tmparr[m.i,1]) > 0,;
					LEFT(tmparr[m.i,1],AT(CHR(0),tmparr[m.i,1])-1),tmparr[m.i,1])),THIS.cHomeDir)
				m.cBackName = THIS.cBackDir + JUSTFNAME(STRTRAN(tmparr[m.i,1],C_NULL))

				IF !FILE(m.cTmpFname)   && is file there?
					*- if not there, log the error and continue
					THIS.WriteLog(JUSTFNAME(STRTRAN(tmparr[m.i,1],C_NULL)),E_NOFILE_LOC + TRIM(m.cDesc) + E_NOBACKUP_LOC)
					m.lNoFinds = .T.
					LOOP
				ENDIF

				IF !FILE(m.cBackName)   && check if file already copied over
					COPY FILE (m.cTmpFname) TO (m.cBackName)
					DO CASE
						CASE tmparr[m.i,2] $ 'sK' OR tmparr[m.i,2] = 'scx'
							*- assume screen/form
							COPY FILE (FORCEEXT(m.cTmpFname,C_SCTEXT)) TO (FORCEEXT(m.cBackName,C_SCTEXT))
						CASE tmparr[m.i,2] == 'V' OR tmparr[m.i,2] = 'vcx'
							*- assume visual class
							COPY FILE (FORCEEXT(m.cTmpFname,C_VCTEXT)) TO (FORCEEXT(m.cBackName,C_VCTEXT))
						CASE tmparr[m.i,2] = 'R' OR tmparr[m.i,2] = 'frx'
							*- assume report form
							COPY FILE (FORCEEXT(m.cTmpFname,"FRT")) TO (FORCEEXT(m.cBackName,"FRT"))
						CASE tmparr[m.i,2] $ 'B' OR tmparr[m.i,2] = 'lbx'
							*- assume report form
							COPY FILE (FORCEEXT(m.cTmpFname,"LBT")) TO (FORCEEXT(m.cBackName,"LBT"))
					ENDCASE
				ENDIF
			ENDFOR
			
			IF m.lNoFinds
				IF MESSAGEBOX(E_NOFINDS_LOC,1) = IDCANCEL
					gOMaster.lHadError = .T.
				ENDIF
			ENDIF

		ENDIF		&& THIS.lBackUp


	ENDPROC		&& PJXConverterBase:BackFiles
	
	*----------------------------------
	PROCEDURE CompileAllScx			&& PJXConverterBase
	*----------------------------------
		*- compile the SCX files in the converted project (save till end)
		LOCAL cFile, lIs30File, cPath, cErrFile

		THIS.WriteLog("","")

		SELECT (THIS.new30alias)
		
		cPATH = SET("PATH") + ',' + JustPath(THIS.pjxName)
		
		SCAN FOR type = C_SCXTYPE OR type = C_VCXTYPE

			m.lIs30File = .F.

			IF FILE(THIS.cHomeDir + STRTRAN(name,C_NULL,""))
				cFile = THIS.cHomeDir + STRTRAN(name,C_NULL,"")
				*- make sure file is converted and openable (maybe user bailed from transporting)
				IF Readable(m.cFile) AND THIS.IsDBF(m.cFile)
					SELECT 0
					USE (m.cFile) EXCLUSIVE
					IF FCOUNT() = C_30SCXFLDS AND FIELD(4) = "CLASS"
						*- okay -- it's a 3.0 file
						m.lIs30File = .T.
					ENDIF
					USE
					SELECT (THIS.new30alias)
				ENDIF
				IF m.lIs30File
					THIS.WriteLog(SYS(2027,m.cFile),C_COMPILE_LOC)
					THIS.lLocalErr = .T.
					
					cErrFile = AddBS(JustPath(m.cFile)) + JustStem(m.cFile) + ".ERR"
					IF FILE(m.cErrFile)
						ERASE (m.cErrFile)
					ENDIF
					
					COMPILE FORM (m.cFile)					

					IF THIS.lHadLocErr OR FILE(m.cErrFile)
						THIS.WriteLog(SYS(2027,m.cFile),E_NOINCLUDE_LOC)
						=MESSAGEBOX(E_NOINCLUDE1_LOC + SYS(2027,m.cFile) + E_NOINCLUDE2_LOC)
						THIS.lHadLocErr = .F.
					ENDIF
					THIS.lLocalErr = .F.
				ENDIF
			ENDIF
		ENDSCAN

	ENDPROC		&& PJXConverterBase:CompileAllScx

#if 0
	*------------------------------------
	PROCEDURE Error				&& Cvt
	*------------------------------------
		PARAMETER errorNum, method, line

		IF errorNum = 1994
			*- can;t find .h file
			THIS.lHadLocErr = .T.
			
			RETURN
		ENDIF
		
		ConverterBase::Error(errorNum, method, line)
		
	ENDPROC
#endif

	*----------------------------------
	PROCEDURE CompileAllFrx			&& PJXConverterBase
	*----------------------------------
		*- compile the FRX files in the converted project (save till end)
		LOCAL cFile

		SELECT (THIS.new30alias)
		SCAN FOR type $ C_REPORT + C_LABEL

			IF FILE(THIS.cHomeDir + STRTRAN(name,C_NULL,""))
				cFile = THIS.cHomeDir + STRTRAN(name,C_NULL,"")
				*- make sure file is converted and openable (maybe user bailed from transporting)
				IF Readable(m.cFile) AND THIS.IsDBF(m.cFile)
					SELECT 0
					USE (m.cFile) EXCLUSIVE
					IF FCOUNT() = C_30FRXFLDS AND FIELD(75) = "USER"
						*- okay -- it's a 3.0 file
						LOCATE FOR INLIST(objtype, N_FRX_DATAENV, N_FRX_CURSOR) AND platform = THIS.platform
						IF FOUND()
							THIS.CompileFRX
							THIS.WriteLog(SYS(2027,m.cFile),C_COMPILE_LOC)
						ENDIF
					ENDIF
					USE
					SELECT (THIS.new30alias)
				ENDIF
			ENDIF
		ENDSCAN

	ENDPROC		&& CompileAllFrx

	*----------------------------------
	PROCEDURE CompileAllDBC
	*----------------------------------
		*- compile the DBC files in the converted project (save till end)
		LOCAL cFile, lIs30File, lOpenShared, i, iCnt, cOldDataBase
		LOCAL ARRAY aDBC[1]
		
		THIS.WriteLog("","")

		cOldDataBase = SET("DATABASE")
		
		SELECT (THIS.new30alias)
		SCAN FOR type = C_DBCTYPE

			m.lIs30File = .F.

			IF FILE(THIS.cHomeDir + STRTRAN(name,C_NULL,""))
				cFile = THIS.cHomeDir + STRTRAN(name,C_NULL,"")
				*- make sure file is converted and openable (maybe user bailed from transporting)
				IF Readable(m.cFile) AND THIS.IsDBF(m.cFile)
					SELECT 0
					USE (m.cFile) EXCLUSIVE
					IF FCOUNT() = C_30DBCFLDS AND FIELD(4) == "OBJECTNAME"
						*- okay -- it's a 3.0 DBC file
						m.lIs30File = .T.
					ENDIF
					USE
					SELECT (THIS.new30alias)
				ENDIF
				IF m.lIs30File
					lOpenShared = .F.
					THIS.WriteLog(SYS(2027,m.cFile),C_COMPILE_LOC)
					IF DBUSED(m.cFile)
						iCnt = ADATABASES(aDBC)
						FOR i = 1 TO iCnt
							IF SYS(2027,aDBC[i,2]) = SYS(2027,m.cFile)
								IF !ISEXCLUSIVE(JustStem(m.cFile))
									m.lOpenShared = .T.
									SET DATABASE TO (aDBC[i,1])
									CLOSE DATABASE
									OPEN DATABASE (SYS(2027,m.cFile)) EXCLUSIVE
								ENDIF
								EXIT
							ENDIF
						NEXT
					ENDIF
					COMPILE DATABASE (m.cFile)
					IF m.lOpenShared
						CLOSE DATABASE
						OPEN DATABASE (SYS(2027,m.cFile)) SHARED
					ENDIF
				ENDIF
			ENDIF
		ENDSCAN

		SET DATABASE TO &cOldDataBase
		
	ENDPROC		&& CompileAllDBC


	*----------------------------------
	PROCEDURE Destroy
	*----------------------------------
	ENDPROC

ENDDEFINE	&& PJXConverterBase 


**********************************************
DEFINE CLASS PJXConverter AS PJXConverterBase
**********************************************

	platform = ""
	winobj = ""
	DIMENSION aMasterParms[1]
	DIMENSION a_s2files[1,3]
	DIMENSION a_s3files[1]
	DIMENSION a_scx[1]			&& array of unique screens in this project
	lSet30Defaults = .F.		&& if converting a 3.0 project, may need to set 3.0 defaults


	*------------------
	PROCEDURE Init	&& PJXConverter
	*------------------
		PARAMETER aParms
		
		*- temp until array stuff fixed
		PRIVATE tmparr
		LOCAL m.cold, m.i, m.j, m.nlen, m.nct, m.cTmpPjxName,m.setid

		m.setid = 0 && in case PJX 3.0

		gOPJX = THIS

		SET ESCAPE ON
		ON ESCAPE DO EscHandler

		=ACOPY(aParms,THIS.aMasterParms)

		THIS.nTimeStamp	= THIS.TStamp()
		THIS.pjxName	= aParms[4]
		THIS.cBackDir	= aParms[1]
		THIS.lDevMode	= aParms[7]
		THIS.cCodeFile	= aParms[8]
		THIS.lLog		= aParms[9]
		THIS.cLogFile	= aParms[10]
		THIS.lBackup	= aParms[11]
		THIS.iPlatformCount	= aParms[12]

		THIS.cCurrentFile = THIS.pjxName
		THIS.cMemoExt = "PJT"

		IF THIS.lLog
			THIS.WriteLog(C_CONVLOG_LOC + THIS.cCurrentFile + " [" + TTOC(DATETIME()) + "]","")
			THIS.WriteLog(C_CONVVERS_LOC + C_CONVERSION_LOC,"")
			THIS.WriteLog("","")
		ENDIF

		*- make a working copy of the PJX file, in case of transporting, so if
		*- user cancels, original will still be available
		cTmpPjxName = ADDBS(JUSTPATH(THIS.pjxName)) + 'P' + LEFT(SYS(3),7) + ".PJX"
		COPY FILE (THIS.pjxName) TO (m.cTmpPjxName)
		IF FILE(FORCEEXT(THIS.pjxName,"PJT"))
			COPY FILE (FORCEEXT(THIS.pjxName,"PJT")) TO (FORCEEXT(m.cTmpPjxName,"PJT"))
		ENDIF
		THIS.pjxName = m.cTmpPjxName

		*- project PJX file
		THIS.pjx25Alias = THIS.OpenFile(THIS.pjxName)
		IF EMPTY(THIS.pjx25Alias)
			THIS.lHadError = .T.
			RETURN .F.
		ENDIF

		gOTherm.visible = .T.

		*- Check for proper file format
		DO CASE
			CASE FCOUNT() = C_PJX40FLDS AND FIELD(1) = "NAME"		
				*- legal VFP4 PJX 
			CASE FCOUNT() = C_PJX30FLDS AND FIELD(1) = "NAME"
				*- legal VFP3 PJX
				THIS.lSet30Defaults = (THIS.iPlatformCount > 1)
				THIS.iPlatformCount = 1
			CASE FCOUNT() = C_PJX25FLDS AND FIELD(1) = "NAME"
				*- check for 2.5 PJX type
			CASE FCOUNT() = c_pjx20flds AND FIELD(1) = "NAME"
				*- check for 2.0 PJX type - if so, call transporter first.
				USE IN (THIS.pjx25Alias)
				*-=MESSAGEBOX(E_HAS20FILE_LOC)
				IF !THIS.Conv20PJX()
					THIS.lHadError = .T.
					RETURN
				ENDIF
			OTHERWISE
				USE IN (THIS.pjx25Alias)
				THIS.lHadError=.T.
				=MESSAGEBOX(E_INVALPJX_LOC)
				RETURN
		ENDCASE
		

		THIS.cBackDir = AddBS(THIS.cBackDir)

		LOCATE FOR Type = "H"
		
		THIS.lDebug = debug
		THIS.lEncrypt = encrypt
		THIS.cDevInfo = devinfo
		THIS.highscxid = setid
		THIS.cHomeDir = ADDBS(JUSTPATH(THIS.pjxName))

		COUNT FOR type = 'S' AND !DELETED() TO THIS.nScreenSets
		THIS.nScreenCtr = 1

		THIS.BackFiles("name", "s|R|B|V|K", C_CONVERT3p_LOC)	&& backup screen, report and label files to back dir

		IF gOMaster.lHadError OR THIS.lHadError
			THIS.Cleanup
			THIS.lHadError = .T.
			RETURN .F.
		ENDIF

		*- this cursor is for working with SPR code if devmode
		IF USED("_FOX3PJX")
			USE IN _FOX3PJX
		ENDIF

		CREATE CURSOR _FOX3PJX (sprmemo m)
		APPEND BLANK

		gOTherm.Update2((1 - N_THERM2X) * 100,C_PROJTASK1_LOC)		&& update therm with next task
		
	ENDPROC	&& PJXConverter:Init

	*------------------------------------
	PROCEDURE Cleanup		&& PJXConverter
	*------------------------------------
		*- this proc is called by Error, and tries to put things back the way they were
		*- copy files in backdir to original location

		LOCAL i, cPJXname, cBackName, cTmpFname, cExt, nTables, cOldExact
		PRIVATE af, au

		cOldExact = SET("EXACT")
		SET EXACT OFF			&& for ASCAN

		IF WEXIST("transdlg")	&& just in case (jd 03/23/96)
			RELEASE WINDOW transdlg
		ENDIF

		m.cPJXname = ""

		IF USED(THIS.pjx25Alias)
			SELECT UPPER(name), type FROM DBF(THIS.pjx25Alias) ;
			    WHERE type $ C_SCREENSET + C_REPORT + C_LABEL AND !DELETED() ;
				INTO ARRAY tmparr

			IF USED(THIS.new30alias)
				m.cPJXname = FULLPATH(DBF(THIS.new30alias),THIS.cHomeDir)
			ENDIF

		ELSE
			_TALLY = 0
		ENDIF

		*- force deletion of any lingering temp screen or report files
		IF _TALLY > 0
			m.nTables = AUSED(au)
			FOR i = 1 TO m.nTables
				IF LEFT(au[i,1],1) = "S" OR LEFT(au[i,1],1) = "F"
					cTmpFname = DBF(au[i,1])
					cExt = UPPER(JustExt(cTmpFname))
					IF ASCAN(tmparr,UPPER(JustFName(cTmpFName))) > 0
						*- oops -- it;s one of the real ones
						LOOP
					ENDIF
					IF INLIST(cExt,C_SCXEXT,"FRX","LBX")
						USE IN (au[i,1])
						ERASE (cTmpFname)
						ERASE FORCEEXT(cTmpFname,IIF(m.cExt = C_SCXEXT,C_SCTEXT,;
							IIF(m.cExt = "FRX","FRT","LBT")))
					ENDIF
				ENDIF
			NEXT
		ENDIF
		
		CLOSE TABLES

		IF THIS.lBackUp
			FOR i = 1 TO _TALLY
				*- Copy files from backup directory
				m.cTmpFname = FULLPATH(ALLTRIM(IIF(AT(CHR(0),tmparr[m.i,1]) > 0,;
					LEFT(tmparr[m.i,1],AT(CHR(0),tmparr[m.i,1])-1),tmparr[m.i,1])),THIS.cHomeDir)
				m.cBackName = THIS.cBackDir + JUSTFNAME(STRTRAN(tmparr[m.i,1],C_NULL))
				IF FILE(m.cBackName)   && is file there?
					IF FILE(m.cTmpFname)
						*- old file is there
						*- try to get rid of it
						DELETE FILE (m.cTmpFname)
						IF FILE(m.cTmpFname)
							*- still there -- maybe it is read-only, so skip
							LOOP
						ENDIF
					ENDIF
					COPY FILE (m.cBackName) TO (m.cTmpFname)
					DO CASE
						CASE tmparr[m.i,2] = 's' OR tmparr[m.i,2] = 'scx'
							*- assume screen/form
							IF FILE(FORCEEXT(m.cBackName,C_SCTEXT))
								COPY FILE (FORCEEXT(m.cBackName,C_SCTEXT)) TO (FORCEEXT(m.cTmpFname,C_SCTEXT))
							ENDIF
						CASE tmparr[m.i,2] = 'R' OR tmparr[m.i,2] = 'frx'
							*- assume report form
							IF FILE(FORCEEXT(m.cBackName,"FRT"))
								COPY FILE (FORCEEXT(m.cBackName,"FRT")) TO (FORCEEXT(m.cTmpFname,"FRT"))
							ENDIF
						CASE tmparr[m.i,2] $ 'B' OR tmparr[m.i,2] = 'lbx'
							*- assume label
							IF FILE(FORCEEXT(m.cBackName,"LBT"))
								COPY FILE (FORCEEXT(m.cBackName,"LBT")) TO (FORCEEXT(m.cTmpFname,"LBT"))
							ENDIF
					ENDCASE
				ENDIF
			NEXT

			*-now, erase files in backup dir
			=ADIR(af,THIS.cBackDir + "*.*")
			IF TYPE("af") # 'U'
				FOR i = 1 TO ALEN(af,1)
					IF af[i,5] $ 'RD'
						LOOP
					ENDIF
					DELETE FILE (THIS.cBackDir + af[i,1])
				NEXT
				RELEASE af
			ENDIF
			IF ADIR(af,THIS.cBackDir + "*.*") = 0
				IF TYPE("af") = 'U'
					RD (THIS.cBackDir)
				ENDIF
			ENDIF
		ENDIF && THIS.lBackUp

		*- erase temp files
		IF FILE(m.cPJXname)
			DELETE FILE (m.cPJXname)
		ENDIF
		IF FILE(FORCEEXT(m.cPJXname,"PJT"))
			DELETE FILE (FORCEEXT(m.cPJXname,"PJT"))
		ENDIF

		IF FILE(THIS.PJXname)
			DELETE FILE (THIS.PJXname)
		ENDIF
		IF FILE(FORCEEXT(THIS.PJXname,"PJT"))
			DELETE FILE (FORCEEXT(THIS.PJXname,"PJT"))
		ENDIF

		SET EXACT &cOldExact

	ENDPROC	&& PJXConverter:Cleanup

	*----------------------------------
	PROCEDURE Converter		&& PJXConverter
	*----------------------------------
		*- convert PJX itself
		*- convert each of the objects in it

		PRIVATE i, z, oForm, cSaveArea
		LOCAL cOld, iWhichPlat, cExt1, cExt2
		DIMENSION aPlatforms[1]

		cOld = THIS.pjx25Alias
		SELECT (m.cOld)

		*- Get record count for thermometer
		THIS.nRecCount = RECC()
		THIS.nTmpCount = 1  	&& reset

		*- External preprocessor hook
	  	THIS.PreForm

		*- Create new PJX file here
		THIS.CreatePJX

		*- Add records from old PJX file
		SELECT (THIS.pjx25Alias)

		*- next, do all the screens
		FOR m.z = 1 TO THIS.highscxid	&& will be 0 for 3.0
		
			*- Get screen set array to process screen sets
			THIS.lIsMain = .F.
			THIS.GetNextFset(m.z)
			
			*- Check to see if screen missing from project such as 
			*- when it is deleted since FoxPro uses next highest number.
			IF EMPTY(THIS.cOutFile)
				LOOP
			ENDIF

			*- Call preprocessor external hook
			THIS.extproj

			aParms[12] = 1

			IF THIS.iPlatformCount > 1
				*- convert multiple platforms in present
				*- if multiple platforms, cycle through for each platform
				DIMENSION aPlatforms[1]
				aPlatforms[1] = ""
				THIS.GetPlatformCount(THIS.a_s3files[1], @aPlatforms)
				IF EMPTY(aPlatforms[1])
					*- was unable to determine platforms
					LOOP
				ENDIF
				IF ALEN(aPlatforms,1) > 1
					IF ASCAN(aPlatforms,C_WINDOWS) > 0 AND ASCAN(aPlatforms,C_MAC) > 0
						*- both platforms are there
						aParms[12] = 2
					ENDIF
				ENDIF
			ELSE
				aPlatforms[1] = THIS.GetPlatForm()
			ENDIF

			FOR m.iWhichPlat = 1 TO aParms[12]

				*- Initialize form set object
				aParms[13] = m.iWhichPlat
				oForm = CREATEOBJ(THIS.scxConverterClass, @aParms, .F., .T.,.T.,.T.)	&& no backup, 
																						&& is a project, 
																						&& show ext transpo dlog
																						&& don;t compile (we do that all at once at the end)

				IF TYPE("oForm") # "O"
					*- object wasn't created (?)
					THIS.lHadError = .T.
				ELSE
					IF oForm.lHadError
						THIS.lHadError = .T.
						*- try and close the file (jd 02/13/96)
						IF USED(oForm.c25alias)
							USE IN (oForm.c25alias)
						ENDIF
						RELEASE oForm			&& dispose of object
					ENDIF
				ENDIF
				

				IF !THIS.lHadError
					IF !oForm.lConverted
						*- only convert the unconverted
						oForm.Converter
					ELSE
						*- still need to know .SPR file location though
						SELECT (THIS.pjx25Alias)
						LOCATE FOR setid = m.z AND type = "S"
						IF FOUND()
							*- create an SPR file from the stored SPR code
							THIS.cStubFile = FULLPATH(ALLTRIM(SUBSTR(outfile,1,AT(C_NULL,outfile)-1)),THIS.cHomeDir)

							*- make sure this is a valid path
							IF !IsDir(JUSTPATH(THIS.cStubFile))
								*- invalid path, so try and make a good one
								THIS.cStubFile = AddBS(THIS.cHomeDir) + JUSTFNAME(THIS.cStubFile)
							ENDIF

							SELECT (oForm.c25alias)
							GO TOP
							IF !EMPTY(user)
								COPY MEMO user TO (THIS.cStubFile)
							ENDIF
							THIS.WriteLog(THIS.cOutFile,C_FILECONV_LOC + C_CREATMSG_LOC)
							IF USED(oForm.c25alias)
								USE IN (oForm.c25alias)
							ENDIF
						ELSE
							*- error			
							*- try and close the file (jd 02/13/96)
							IF USED(oForm.c25alias)
								USE IN (oForm.c25alias)
							ENDIF
							THIS.lHadError = .T.
							RELEASE oForm			&& dispose of object
							LOOP					&& continue converting
						ENDIF
					ENDIF
				ELSE
					*- reset this flag
					THIS.lHadError = .F.
				ENDIF && THIS.lHadError

			NEXT		&& going through each platform

			*- get rid of temp files we created
			FOR m.i = 1 TO ALEN(THIS.a_s2files,1)
				IF FILE(THIS.a_s2files[i,1])
					DELETE FILE (THIS.a_s2files[i,1])
				ENDIF
				IF FILE(FORCEEXT((THIS.a_s2files[i,1]),C_SCTEXT))
					DELETE FILE (FORCEEXT((THIS.a_s2files[i,1]),C_SCTEXT))
				ENDIF
			NEXT

			*- add records to PJX file, even if not converted
			THIS.InsertSCX

			THIS.nScreenCtr = THIS.nScreenCtr + 1

		ENDFOR	&& end of screen set loop

		gOTherm.Update2(N_THERM3X * 100,C_PROJTASK2_LOC)	&& update project therm

		*- move other files to new project
		SELECT (THIS.pjx25Alias)
		SCAN FOR !DELETED()
			DO CASE
				CASE type = C_SCREENSET
					*- already handled above
					LOOP
				CASE type = C_SCREEN
					*- already handled above
					LOOP
				CASE type = C_HEADER
					*- already handled above
					LOOP
				CASE type $ C_VCXTYPE + C_SCXTYPE
					*- we may need to set the 30 properties
					IF THIS.lSet30Defaults
						*- setup arrays with file names
						*- dimension 1 = name of actual file that will be converted
						*-           2 = arranged
						*-           3 = original name of screen

						DIMENSION THIS.a_s2files[1,3]
						DIMENSION THIS.a_s3files[1]

						THIS.a_s3files[1] = FULLPATH(ALLTRIM(;
							IIF(AT(CHR(0),name) > 0,;
								SUBSTR(name,1,AT(CHR(0),name)-1),;
								name)),;
							THIS.cHomeDir)
						THIS.a_s2files[1,3] = JUSTFNAME(STRTRAN(name,C_NULL))
						cext1 = JustExt(THIS.a_s2files[1,3])
						cext2 = IIF(cext1 == C_VCXEXT, C_VCTEXT, C_SCTEXT)
						THIS.a_s2files[1,1] = "S" + RIGHT(SYS(3),7) + '.' + m.cext1
						IF THIS.lBackup
							*- make sure the files are there
							IF FILE(THIS.cBackDir + THIS.a_s2files[1,3]) AND FILE(FORCEEXT(THIS.cBackDir + THIS.a_s2files[1,3],m.cext2))
								COPY FILE (THIS.cBackDir + THIS.a_s2files[1,3]) TO (THIS.a_s2files[1,1])
								COPY FILE (FORCEEXT(THIS.cBackDir + THIS.a_s2files[1,3],m.cext2)) TO ;
									(FORCEEXT(THIS.a_s2files[1,1],m.cext2))
							ELSE
								*- file not found
								THIS.WriteLog(E_FILE_LOC + THIS.a_s2files[1,3] + E_NOCONVERT1_LOC,"")
								THIS.cOutFile = ""
								LOOP
							ENDIF
						ELSE
							IF !FILE(THIS.a_s3files[1]) OR !FILE(FORCEEXT(THIS.a_s3files[1],m.cext2))
								*- file not found
								THIS.WriteLog(E_FILE_LOC + THIS.a_s3files[1] + E_NOCONVERT1_LOC,"")
								THIS.cOutFile = ""
								LOOP
							ENDIF
							COPY FILE (THIS.a_s3files[1]) TO (THIS.a_s2files[1,1])
							COPY FILE (FORCEEXT(THIS.a_s3files[1],m.cext2)) TO ;
								(FORCEEXT(THIS.a_s2files[1,1],m.cext2))
						ENDIF

						*- we set the 30 default values in the INIT, so throw object away when done
						oForm = CREATEOBJ(THIS.scx30ConverterClass, @aParms, .F., .T.,.T.,.T.)	&& no backup, 
																								&& is a project, 
																								&& show ext transpo dlog
																								&& don;t compile (we do that all at once at the end)

						IF TYPE("oForm") # "O"
							*- object wasn't created (?)
							THIS.lHadError = .T.
						ELSE
							IF oForm.lHadError
								THIS.lHadError = .T.
								*- try and close the file
								IF USED(oForm.c25alias)
									USE IN (oForm.c25alias)
								ENDIF
							ENDIF

							oForm.Converter

							oForm.oConvForm = .NULL.

							RELEASE oForm			&& dispose of object
						ENDIF

						
						*- get rid of temp files we created
						IF FILE(THIS.a_s2files[1,1])
							COPY FILE (THIS.a_s2files[1,1]) TO (THIS.a_s3files[1])
							DELETE FILE (THIS.a_s2files[1,1])
						ENDIF
						IF FILE(FORCEEXT((THIS.a_s2files[1,1]),m.cext2))
							COPY FILE (FORCEEXT(THIS.a_s2files[1,1],m.cext2)) TO ;
								(FORCEEXT(THIS.a_s3files[1],m.cext2))
							DELETE FILE (FORCEEXT((THIS.a_s2files[1,1]),m.cext2))
						ENDIF

			
					ENDIF

					THIS.cOutFile = STRTRAN(EVAL(cOld + '.name'),C_NULL)
					INSERT INTO (THIS.new30alias) ;
						(name,mainprog,type,timestamp,homedir,exclude,key);
						VALUES(EVAL(cOld + '.name'),;
							EVAL(cOld + '.mainprog'),;
							EVAL(cOld + '.type'),;
							EVAL(cOld + '.timestamp'),;
							EVAL(cOld + '.homedir'),;
							EVAL(cOld + '.exclude'),;
							UPPER(LEFT(JUSTSTEM(THIS.cOutFile),LEN(key))))
					THIS.p30to40

				CASE type = C_REPORT OR type = C_LABEL
					LOCAL m.cfrxext, m.cfrtext
					m.cfrxext = IIF(type = C_REPORT,"FRX","LBX")
					m.cfrtext = IIF(type = C_REPORT,"FRT","LBT")
					*- Initialize report object
					THIS.f2files = "F" + RIGHT(SYS(3),7) + '.' + m.cfrxext
					THIS.f3files = FULLPATH(ALLTRIM(;
						IIF(AT(CHR(0),name) > 0,;
							SUBSTR(name,1,AT(CHR(0),name)-1),;
							name)),;
						THIS.cHomeDir)
					IF !FILE(IIF(THIS.lBackUp,THIS.cBackDir + JUSTFNAME(THIS.f3files),THIS.f3files)) OR ;
						!FILE(FORCEEXT(IIF(THIS.lBackUp,THIS.cBackDir + JUSTFNAME(THIS.f3files),THIS.f3files),m.cfrtext))
						*- file not found
						THIS.WriteLog(E_FILE_LOC + THIS.f3files + E_NOCONVERT1_LOC,"")
						LOOP
					ENDIF
					COPY FILE (IIF(THIS.lBackUp,THIS.cBackDir + JUSTFNAME(THIS.f3files),THIS.f3files)) TO (THIS.f2files)
					COPY FILE (FORCEEXT(IIF(THIS.lBackUp,THIS.cBackDir + JUSTFNAME(THIS.f3files),THIS.f3files),m.cfrtext)) TO ;
						(FORCEEXT(THIS.f2files,m.cfrtext))
					oForm = CREATEOBJ(THIS.frxConverterClass, @aParms, .F., .T., .T., .T.)

					IF TYPE("oForm") # "O"
						*- object wasn't created (?)
						THIS.lHadError = .T.
					ENDIF

					IF !THIS.lHadError
						IF !oForm.lHadError
							IF !oForm.lConverted
								*- only convert the unconverted
								oForm.Converter
							ELSE
								THIS.WriteLog(THIS.f3files,C_FILECONV_LOC + '.' + C_CRLF)
								IF USED(oForm.c25alias)
									USE IN (oForm.c25alias)
								ENDIF
							ENDIF
						ENDIF
					ELSE
						*- reset this flag
						THIS.lHadError = .F.
					ENDIF

					*- erase temp files
					IF FILE(THIS.f2files)
						DELETE FILE (THIS.f2files)
					ENDIF
					IF FILE (FORCEEXT(THIS.f2files,m.cfrtext))
						DELETE FILE (FORCEEXT(THIS.f2files,m.cfrtext))
					ENDIF

					INSERT INTO (THIS.new30alias) ;
						(name,mainprog,type,timestamp,homedir,exclude,key);
						VALUES(EVAL(cOld + '.name'),;
							EVAL(cOld + '.mainprog'),;
							EVAL(cOld + '.type'),;
							EVAL(cOld + '.timestamp'),;
							EVAL(cOld + '.homedir'),;
							EVAL(cOld + '.exclude'),;
							UPPER(LEFT(JUSTSTEM(THIS.f3files),LEN(key))))
					THIS.p30to40
					
				CASE type = C_PRGTYPE
					*- program -- check to make sure it isn;t a duplicate 
					*- of an SPR already in the file
					cSaveArea = SELECT()
					THIS.cOutFile = STRTRAN(EVAL(cOld + '.name'),C_NULL)
					SELECT (THIS.new30alias)
					LOCATE FOR UPPER(JUSTFNAME(THIS.cOutFile)) $ UPPER(name)
					IF FOUND()
						*- .SPR file is already there, or it is a duplicate
						*- log and continue
						THIS.WriteLog(THIS.cOutFile,C_FILEFOUNDMSG_LOC)
					ELSE
						INSERT INTO (THIS.new30alias) ;
							(name,mainprog,type,timestamp,homedir,exclude,key);
							VALUES(EVAL(cOld + '.name'),;
								EVAL(cOld + '.mainprog'),;
								EVAL(cOld + '.type'),;
								EVAL(cOld + '.timestamp'),;
								EVAL(cOld + '.homedir'),;
								EVAL(cOld + '.exclude'),;
								UPPER(LEFT(JUSTSTEM(THIS.cOutFile),LEN(key))))
						THIS.p30to40
						THIS.WriteLog(THIS.cOutFile,C_NOCONVMSG_LOC)
					ENDIF
					SELECT (m.cSaveArea)
				OTHERWISE
					THIS.cOutFile = STRTRAN(EVAL(cOld + '.name'),C_NULL)
					INSERT INTO (THIS.new30alias) ;
						(name,mainprog,type,timestamp,homedir,exclude,key);
						VALUES(EVAL(cOld + '.name'),;
							EVAL(cOld + '.mainprog'),;
							EVAL(cOld + '.type'),;
							EVAL(cOld + '.timestamp'),;
							EVAL(cOld + '.homedir'),;
							EVAL(cOld + '.exclude'),;
							UPPER(LEFT(JUSTSTEM(THIS.cOutFile),LEN(key))))
					THIS.p30to40
					THIS.WriteLog(THIS.cOutFile,C_NOCONVMSG_LOC)
			ENDCASE
		ENDSCAN

		SELECT (THIS.new30Alias)
		REPLACE TIMESTAMP WITH this.tstamp() + RECNO() ,;
			ID WITH timestamp FOR TYPE != 'H'

		*- compile the SCX files
		THIS.CompileAllScx

		*- compile the FRX files
		THIS.CompileAllFRX

		*- compile the DBC files
		THIS.CompileAllDBC

		*- Close project
		THIS.ClosePJX

		*- close up gOTherm
		gOTherm.Complete2

		gOPJX = .NULL.

		RETURN THIS.cCurrentFile

	ENDPROC	&& PJXConverter:Converter

	*------------------
	PROCEDURE p30to40
	*------------------
	*- if it's a 30 pjx, copy USER and COMMENTS fields
		SELECT (this.new30Alias)
		IF TYPE(this.pjx25Alias+".Comments") # 'U'
			REPLACE Comments WITH ;
				(EVAL(this.pjx25Alias+".Comments"))
		ENDIF
		IF TYPE(this.pjx25Alias+".User") # 'U'
			REPLACE User WITH ;
				(EVAL(this.pjx25Alias+".User"))
		ENDIF
	ENDPROC
	
	*------------------
	PROCEDURE ExtProj
	*------------------
	*- this is an external hook to preprocess
	*- project object via subclassing
	ENDPROC

	*----------------------------------
	PROCEDURE InsertSCX			&& PJXConverter
	*----------------------------------
		*- add appropriate record(s) to 3.0 PJX file
		PARAMETER lNoAddSPR

		LOCAL cFileName

		cFileName = SYS(2014,THIS.cOutFile,DBF(THIS.new30alias))

		INSERT INTO (THIS.new30alias) ;
			(name,timestamp,type,exclude,key);
			VALUES(;
				cFileName + CHR(0),;
				THIS.nTimeStamp,;
				C_SCXTYPE,;
				THIS.lExclude,;
				UPPER(LEFT(JustFName(THIS.cOutFile),LEN(key))))

		IF THIS.iPlatformCount > 1 AND aParms[12] > 1
			*- converting more than one platform (i.e., also Mac) so add Mac form record
			m.cFileName = JustStem(cFileName) + C_MACEXT + "." + JustExt(cFileName)
			INSERT INTO (THIS.new30alias) ;
				(name,timestamp,type,exclude,key);
				VALUES(;
					cFileName + CHR(0),;
					THIS.nTimeStamp,;
					C_SCXTYPE,;
					THIS.lExclude,;
					UPPER(LEFT(JustStem(THIS.cOutFile) + C_MACEXT + JustExt(THIS.cOutFile),LEN(key))))
		ENDIF

		IF !lNoAddSPR
			PJXConverterBase::InsertSCX
		ENDIF

	ENDPROC

	*------------------
	PROCEDURE Conv20PJX
	*------------------
		*- This converts a 2.0 project to a 2.5 one.
		*- transprt is built into this app
		LOCAL m.oldudfp

		gOTherm.SetTitle2(C_THERMMSG6_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
		m.oldudfp = SET("UDFP")
		SET UDFP TO REFERENCE
		DO (gTransport) WITH THIS.pjxName,1,.F.,gAShowMe,m.gOTherm,THIS.cCurrentFile
		SET UDFP TO &oldudfp
		SET MESSAGE TO C_PROJTASK4_LOC
		gOTherm.SetTitle2(C_THERMMSG1_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
		THIS.pjx25Alias = THIS.OpenFile(THIS.pjxName)
		IF FCOUNT() # C_PJX25FLDS OR FIELD(1) # "NAME"
			USE IN (THIS.pjx25Alias)
			THIS.lHadError = .T.
			RETURN .F.
		ENDIF
		RETURN .T.
	ENDPROC				&& PJXConverter

	*------------------
	PROCEDURE ClosePJX	&& PJXConverter
	*------------------
		m.cTmpFname = FULLPATH(THIS.cCurrentFile,THIS.cHomeDir)
		*m.cTmpFname = FULLPATH(DBF(THIS.pjx25Alias),THIS.cHomeDir)
		m.cBackName = IIF(!EMPTY(THIS.cBackDir),THIS.cBackDir,"") + JUSTFNAME(THIS.cCurrentFile)	&& JUSTFNAME(DBF(THIS.pjx25Alias))

		IF USED("_FOX3PJX")
			IF THIS.lDevMode
				COPY MEMO _FOX3PJX.sprmemo TO (THIS.cCodeFile)
				*- add the devmode code file to the project
				INSERT INTO (THIS.new30alias) ;
					(name,mainprog,type,timestamp,homedir,exclude,key);
					VALUES(THIS.cCodeFile,;
						.F.,;
						C_PRGTYPE,;
						THIS.nTimeStamp,;
						THIS.cHomeDir,;
						.F.,;
						UPPER(LEFT(JUSTSTEM(THIS.cCodeFile),LEN(key))))
			ENDIF
			USE IN _FOX3PJX
		ENDIF

		THIS.CloseFiles

		*- get rid of temp file
		IF FILE(THIS.pjxName)
			DELETE FILE (THIS.pjxName)
		ENDIF
		IF FILE(FORCEEXT(THIS.pjxName,THIS.cMemoExt))
			DELETE FILE (FORCEEXT(THIS.pjxName,THIS.cMemoExt))
		ENDIF

		*- Rename new FP3 project
		DELETE FILE (m.cTmpFname)
		DELETE FILE (FORCEEXT(m.cTmpFname,"PJT"))

		IF FILE(ADDBS(JUSTPATH(m.cTmpFname)) + THIS.new30alias + ".PJX")
			RENAME (ADDBS(JUSTPATH(m.cTmpFname)) + THIS.new30alias + ".PJX") TO (m.cTmpFname)
		ENDIF
		IF FILE(ADDBS(JUSTPATH(m.cTmpFname)) + THIS.new30alias + ".PJT")
			RENAME (ADDBS(JUSTPATH(m.cTmpFname)) + THIS.new30alias + ".PJT") TO (FORCEEXT(m.cTmpFname,"PJT"))
		ENDIF

	ENDPROC		&&   ClosePJX

	*---------------------
	PROCEDURE GetNextFset
	*---------------------
		PARAMETER pSetid
		PRIVATE tmparr,i 
		
		THIS.curscxid = m.pSetid	&& get current screen ID number
				
		*- Handle project PJX file
		SELECT (THIS.pjx25Alias)
		
		SELECT name,arranged,exclude FROM DBF(THIS.pjx25Alias) ;
		    WHERE type = "s" AND setid = pSetid AND !DELETED() ;
			ORDER by scrnorder ;
			INTO ARRAY tmparr

		IF _TALLY = 0
			*- Screen is missing from set, so skip and go to next one.
			THIS.cOutFile = ""
			RETURN
		ENDIF
		
		*- setup arrays with file names
		*- dimension 1 = name of actual file that will be converted
		*-           2 = arranged
		*-           3 = original name of screen
		DIMENSION THIS.a_s2files[_TALLY,3]
		DIMENSION THIS.a_s3files[_TALLY]

		THIS.lExclude = tmparr[1,3]

		FOR i = 1 TO _TALLY
			THIS.a_s3files[m.i] = FULLPATH(ALLTRIM(;
				IIF(AT(CHR(0),tmparr[m.i,1]) > 0,;
					SUBSTR(tmparr[m.i,1],1,AT(CHR(0),tmparr[m.i,1])-1),;
					tmparr[m.i,1])),;
				THIS.cHomeDir)
			THIS.a_s2files[m.i,1] = "S" + RIGHT(SYS(3),7) + ".SCX"
			THIS.a_s2files[m.i,3] = JUSTFNAME(STRTRAN(tmparr[m.i,1],C_NULL))
			IF THIS.lBackup
				*- make sure the files are there
				IF FILE(THIS.cBackDir + THIS.a_s2files[m.i,3]) AND FILE(FORCEEXT(THIS.cBackDir + THIS.a_s2files[m.i,3],C_SCTEXT))
					COPY FILE (THIS.cBackDir + THIS.a_s2files[m.i,3]) TO (THIS.a_s2files[m.i,1])
					COPY FILE (FORCEEXT(THIS.cBackDir + THIS.a_s2files[m.i,3],C_SCTEXT)) TO ;
						(FORCEEXT(THIS.a_s2files[m.i,1],C_SCTEXT))
				ELSE
					*- Screen is missing, so skip
					THIS.cOutFile = ""
					RETURN
				ENDIF
			ELSE
				IF !FILE(THIS.a_s3files[m.i]) OR !FILE(FORCEEXT(THIS.a_s3files[m.i],C_SCTEXT))
					*- file not found
					THIS.WriteLog(E_FILE_LOC + THIS.a_s3files[m.i] + E_NOCONVERT1_LOC,"")
					THIS.cOutFile = ""
					RETURN
				ENDIF
				COPY FILE (THIS.a_s3files[m.i]) TO (THIS.a_s2files[m.i,1])
				COPY FILE (FORCEEXT(THIS.a_s3files[m.i],C_SCTEXT)) TO ;
					(FORCEEXT(THIS.a_s2files[m.i,1],C_SCTEXT))
			ENDIF
			THIS.a_s2files[m.i,2] = tmparr[m.i,2]
		ENDFOR

		*- Get output file SPR name and screen set settings
		LOCATE FOR setid = m.pSetid AND type = "S"
		IF FOUND()
			THIS.cStubFile = FULLPATH(ALLTRIM(SUBSTR(outfile,1,AT(C_NULL,outfile)-1)),THIS.cHomeDir)
			*- make sure this is a valid path
			IF !IsDir(JUSTPATH(THIS.cStubFile))
				*- invalid path, so try and make a good one
				THIS.cStubFile = AddBS(THIS.cHomeDir) + JUSTFNAME(THIS.cStubFile)
			ENDIF
		ELSE
			THIS.cOutFile = ""
			RETURN			
		ENDIF

		THIS.cOutFile = THIS.a_s3files[1,1]

		*- also, remember the main
		THIS.lIsMain = mainprog
			
		*- populate settings array with Project options
		THIS.a_pjxsets[A_OPENFILES]  = openFiles	&& open DBF files
		THIS.a_pjxsets[A_CLOSEFILES] = closeFiles	&& close DBF files
		THIS.a_pjxsets[A_DEFWINDOWS] = defwinds		&& define windows
		THIS.a_pjxsets[A_RELWINDOWS] = relwinds		&& release windows
		THIS.a_pjxsets[A_READMODAL]  = IIF(!EMPTY(assocwinds),.F.,modal)		&& READ MODAL
		THIS.a_pjxsets[A_GETBORDERS] = nologo		&& border for GETs
		THIS.a_pjxsets[A_READCYCLE]  = readcycle	&& READ CYCLE
		THIS.a_pjxsets[A_READNOLOCK] = nolock		&& READ NOLOCK
 		THIS.a_pjxsets[A_MULTIREADS] = multreads	&& multiple READs
 		THIS.a_pjxsets[A_ASSOCWINDS] = assocwinds	&& associated windows

		IF !THIS.a_pjxsets[9]  AND ALEN(THIS.a_s3files) > 1		&& multireads
			DIMENSION THIS.a_s3files[1]
		ENDIF

		gOTherm.Update2((THIS.nScreenCtr/(THIS.nScreenSets + 1) * N_THERM2X + (1 - N_THERM2X)) * 100)	&& (1) account for jump start we gave when backing up
				
	ENDPROC		&&   GetNextFset

ENDDEFINE		&&  PJXConvert 


**********************************************
DEFINE CLASS FPCConverter AS PJXConverterBase
**********************************************
	*- class for converting 2.6 Catalog Files (FPC files)
	*- simpler file structure than projects
	*- files are converted individually -- just as if selecting
	*- each one by one

	*------------------
	PROCEDURE Init
	*------------------
		PARAMETER aParms

		*- temp until array stuff fixed
		PRIVATE tmparr
		LOCAL m.nct
			
		gOPJX = THIS

		SET ESCAPE ON
		ON ESCAPE DO EscHandler

		THIS.isproj = .F.		&& not really a project

		THIS.nTimeStamp	= THIS.TStamp()
		THIS.pjxName	= aParms[4]
		THIS.pjxVersion = aParms[3]
		THIS.cBackDir	= aParms[1]
		THIS.lDevMode	= aParms[7]
		THIS.cCodeFile	= aParms[8]
		THIS.lLog		= aParms[9]
		THIS.cLogFile	= aParms[10]
		THIS.lBackup	= aParms[11]
		THIS.iPlatformCount	= aParms[12]

		THIS.cCurrentFile = THIS.pjxName
		THIS.cMemoExt = "FCT"

		IF THIS.lLog
			THIS.WriteLog(C_CONVLOG_LOC + THIS.pjxName + " [" + TTOC(DATETIME()) + "]","")
			THIS.WriteLog(C_CONVVERS_LOC + C_CONVERSION_LOC,"")
			THIS.WriteLog("","")
		ENDIF

		*- make a working copy of the PJX file, in case of transporting, so if
		*- user cancels, original will still be available
		*- this is here for compatibility with PJXConverter
		cTmpPjxName = ADDBS(JUSTPATH(THIS.pjxName)) + 'P' + LEFT(SYS(3),7) + ".FPC"
		COPY FILE (THIS.pjxName) TO (m.cTmpPjxName)
		IF FILE(FORCEEXT(THIS.pjxName,THIS.cMemoExt))
			COPY FILE (FORCEEXT(THIS.pjxName,THIS.cMemoExt)) TO (FORCEEXT(m.cTmpPjxName,THIS.cMemoExt))
		ENDIF
		THIS.pjxName = m.cTmpPjxName

		THIS.pjx25Alias = THIS.OpenFile(THIS.pjxName)
		IF EMPTY(THIS.pjx25Alias)
			THIS.lHadError = .T.
			RETURN
		ENDIF

		*- Check for proper file format
		IF !(FCOUNT() = C_FPCFLDS AND FIELD(8) = "FOX_FILE")
			USE IN (THIS.pjx25Alias)
			THIS.lHadError=.T.
			=MESSAGEBOX(E_INVALFPC_LOC)
			RETURN
		ENDIF

		*- Add records from old CAT file
		SELECT (THIS.pjx25Alias)
		COUNT FOR !DELETED() AND INLIST(type,C_FPCSCREENTYPE,C_FPCLABELTYPE,C_FPCREPORTTYPE) TO THIS.nScreenSets
		THIS.nScreenSets = THIS.nScreenSets + 2
		THIS.curscxid = 1

		gOTherm.Update2(0,C_PROJTASK5_LOC)
		
		gOTherm.visible = .T.

		THIS.cBackDir = ADDBS(THIS.cBackDir)
		THIS.cHomeDir = ADDBS(JUSTPATH(THIS.pjxName))

		THIS.BackFiles("path", "scx|frx|lbx", C_CONVERT3c_LOC)	&& backup screen, report & label files to back dir
		IF gOMaster.lHadError OR THIS.lHadError
			THIS.Cleanup
			THIS.lHadError = .T.
			RETURN .F.
		ENDIF

		gOTherm.Update2((THIS.curscxid/(THIS.nScreenSets + 1)) * 100,C_PROJTASK5_LOC)		&& update therm with next task
	
	ENDPROC		&&	FPCConverter:Init

	*----------------------------------
	PROCEDURE Converter		&& FPCConverter
	*----------------------------------
		*- convert FPC itself
		*- convert each of the objects in it

		PRIVATE i, z, oForm, cOld, cTmpFile
		PRIVATE g_platforms
		PRIVATE aParms,oConvObject
		LOCAL	iWhichPlat
		PRIVATE aPlatforms

		cOld = THIS.pjx25Alias

		*- Get record count for Thermometer

		*- External preprocessor hook
	  	THIS.PreForm

		*- Create new PJX file here
		THIS.CreatePJX

		*- Add records from old PJX file
		SELECT (THIS.pjx25Alias)

		SCAN FOR !DELETED()

			DO CASE
				CASE type = C_FPCCATTYPE
					THIS.WriteLog(SYS(2027,EVAL(cOld + '.path')),C_CONVMSG_LOC)
				CASE type = C_FPCSCREENTYPE
					THIS.curscxid = THIS.curscxid + 1

					IF THIS.nScreenSets > 0
						gOTherm.Update2((THIS.curscxid/THIS.nScreenSets) * 100,C_PROJTASK1_LOC)
					ENDIF

					* needed for GENSCRN stuff
					DIMENSION g_platforms[1]
					g_platforms = ""

					*- simulate call from _converter
					DIMENSION aParms[13]
					aParms[ 2] = C_SCREENTYPEPARM		&& file type
					aParms[ 3] = THIS.pjxVersion		&& file version
					aParms[ 4] = ALLT(SYS(2027,path))	&& FP30 file name
					aParms[ 5] = .T.					&& platform only
					aParms[ 6] = .F.					&& special effect
					aParms[ 7] = THIS.lDevMode			&& developer mode
					aParms[ 8] = THIS.cCodeFile			&& code file if dev mode
					aParms[ 9] = THIS.llog				&& create log file?
					aParms[10] = THIS.cLogFile			&& logfile name
					aParms[12] = THIS.iPlatformCount	&& current platform only?
					*- some other values are set below, in the FOR ... NEXT loop

					THIS.cStubFile = FORCEEXT(aParms[4],C_SPREXT)
					THIS.cOutFile = aParms[4]

					IF THIS.iPlatformCount > 1
						*- convert multiple platforms in present
						*- if multiple platforms, cycle through for each platform
						DIMENSION aPlatforms[1]
						aPlatforms[1] = ""
						THIS.GetPlatformCount(aParms[4], @aPlatforms)
						IF EMPTY(aPlatforms[1])
							*- was unable to determine platforms
							LOOP
						ENDIF
						IF ALEN(aPlatforms,1) > 1
							IF ASCAN(aPlatforms,C_WINDOWS) > 0 AND ASCAN(aPlatforms,C_MAC) > 0
								*- both platforms are there
								aParms[12] = 2
							ENDIF
						ENDIF
					ELSE
						aPlatforms[1] = THIS.GetPlatForm()
					ENDIF

					FOR m.iWhichPlat = 1 TO aParms[12]

						aParms[ 1] = ""						&& backup dir (not used)
						aParms[13] = m.iWhichPlat

						oConvObject = CREATE(THIS.scxConverterClass, @aParms,.F.,.F.,.T.,.T.)	&& no backup, 
																								&& not a project, 
																								&& show ext transpo dlog
																								&& don;t compile (we do that all at once at the end)
						IF TYPE("oConvObject") # 'O'
							*- object was not created
							THIS.lHadError = .T.
								THIS.WriteLog(SYS(2027,THIS.cOutFile),C_NOTCONVMSG_LOC)
						ENDIF

						IF !THIS.lHadError AND !oConvObject.lHadError 
							IF !oConvObject.lConverted 
								oConvObject.Converter
							ELSE
								*- still need to know .SPR file location though
								SELECT (THIS.pjx25Alias)
								*- create an SPR file from the stored SPR code
								SELECT (oConvObject.c25alias)
								GO TOP
								IF !EMPTY(user)
									COPY MEMO user TO (THIS.cStubFile)
									THIS.WriteLog(SYS(2027,THIS.cOutFile),C_FILECONV_LOC + C_CREATMSG_LOC)
									IF USED(oConvObject.c25alias)
										USE IN (oConvObject.c25alias)
									ENDIF
									THIS.EndLog(THIS.cOutFile)
								ELSE
									*- error			
									THIS.lHadError = .T.
									RELEASE oConvObject			&& dispose of object
									LOOP						&& continue converting
								ENDIF
							ENDIF
						ENDIF

					NEXT		&& going through each platform


					*- add records to PJX file
					THIS.InsertSCX

					RELEASE oConvObject

				CASE type = C_FPCREPORTTYPE OR ;
					type = C_FPCLABELTYPE
					*- Initialize report object
					*- simulate call from _converter
					THIS.curscxid = THIS.curscxid + 1

					IF THIS.nScreenSets > 0
						gOTherm.Update2((THIS.curscxid/THIS.nScreenSets) * 100,C_PROJTASK2_LOC)
					ENDIF

					DIMENSION aParms[13]
					aParms[ 1] = ""						&& backup dir (not used)
					aParms[ 2] = "REPORT"				&& file type
					aParms[ 3] = THIS.pjxVersion		&& file version
					aParms[ 4] = ALLT(SYS(2027,path))	&& FP30 file name
					aParms[ 5] = .T.					&& platform only
					aParms[ 6] = .F.					&& special effect
					aParms[ 7] = THIS.lDevMode			&& developer mode
					aParms[ 8] = THIS.cCodeFile			&& code file if dev mode
					aParms[ 9] = THIS.llog				&& create log file?
					aParms[10] = THIS.cLogFile			&& logfile name

					THIS.f3files = aParms[4]
					THIS.f2files = IIF(THIS.lBackup,THIS.cBackDir,"") + JUSTFNAME(STRTRAN(path,C_NULL))
					oForm = CREATEOBJ(THIS.frxConverterClass, @aParms, .F.,.F.,.T.,.T.)

					IF TYPE("oForm") # "O"
						*- object wasn't created (?)
						THIS.lHadError = .T.
					ENDIF

					IF !THIS.lHadError
						IF !oForm.lHadError AND !THIS.lHadError
							IF !oForm.lConverted
								*- only convert the unconverted
								oForm.Converter
							ELSE
								THIS.WriteLog(SYS(2027,THIS.f3files),C_FILECONV_LOC + '.' + C_CRLF)
								IF USED(oForm.c25alias)
									USE IN (oForm.c25alias)
								ENDIF
							ENDIF
						ENDIF
					ELSE
						*- reset this flag
						THIS.lHadError = .F.
					ENDIF

					INSERT INTO (THIS.new30alias) ;
						(name,mainprog,type,timestamp,homedir,exclude,comments,key);
						VALUES(EVAL(cOld + '.File_name'),;
							.F.,;
							IIF(EVAL(cOld + '.type') = C_FPCLABELTYPE,C_LABEL,C_REPORT),;
							THIS.nTimeStamp,;
							JUSTPATH(EVAL(cOld + '.path')),;
							.F.,;
							EVAL(cOld + '.Title') + C_NULL,;
							UPPER(LEFT(JUSTSTEM(EVAL(cOld + '.path')),LEN(key))))


				OTHERWISE
					PRIVATE cType
					m.cTmpFile = EVAL(cOld + '.path')
					m.cType = EVAL(cOld + '.type')
					m.cType = IIF(m.cType = C_FPCDBFTYPE,"D",;
							  IIF(m.cType = C_FPCCSQUERYTYPE OR ;
									m.ctype = C_FPCUPQUERYTYPE OR ;
									m.ctype = C_FPCSQLQUERYTYPE,"P",;
							  IIF(m.cType = C_FPCREPORTTYPE,"R",;
							  IIF(m.cType = C_FPCLABELTYPE,"B",;
							  IIF(m.cType = C_FPCPRGTYPE,"P",;
							  IIF(m.cType = C_FPCAPPTYPE,"Z","x"))))))
					INSERT INTO (THIS.new30alias) ;
						(name,type,timestamp,exclude,comments,key);
						VALUES(m.cTmpFile,;
							m.cType,;
							THIS.nTimeStamp,;
							.F.,;
							EVAL(cOld + '.Title') + C_NULL,;
							UPPER(LEFT(JUSTSTEM(m.cTmpFile),LEN(key))))
					THIS.WriteLog(SYS(2027,m.cTmpFile),C_NOCONVMSG_LOC)
			ENDCASE
		ENDSCAN

		*- compile the SCX files
		THIS.CompileAllScx

		*- compile the FRX files
		THIS.CompileAllFRX

		*- Close project
		THIS.ClosePJX

		*- close up gOTherm
		gOTherm.Complete2

		gOPJX = .NULL.

		RETURN THIS.cFull30PJXName

	ENDPROC		&& FPCConverter:Converter


	*----------------------------------
	PROCEDURE InsertSCX			&& FPCConverter
	*----------------------------------
		*- add appropriate record(s) to 3.0 PJX file
		LOCAL cFileName

		cFileName = SYS(2014,THIS.cOutFile,DBF(THIS.new30alias))

		INSERT INTO (THIS.new30alias) ;
			(name,timestamp,type,exclude,comments,key);
			VALUES(;
				m.cFileName + CHR(0),;
				THIS.nTimeStamp,;
				C_SCXTYPE,;
				THIS.lExclude,;
				EVAL(cOld + '.Title') + C_NULL,;
				UPPER(LEFT(JUSTSTEM(THIS.cOutFile),LEN(key))))

		IF THIS.iPlatformCount > 1 AND aParms[12] > 1
			*- converting more than one platform (i.e., also Mac) so add Mac form record
			m.cFileName = JustStem(cFileName) + C_MACEXT + "." + JustExt(cFileName)
			INSERT INTO (THIS.new30alias) ;
				(name,timestamp,type,exclude,comments,key);
				VALUES(;
					cFileName + CHR(0),;
					THIS.nTimeStamp,;
					C_SCXTYPE,;
					THIS.lExclude,;
					EVAL(cOld + '.Title') + C_NULL,;
					UPPER(LEFT(JustStem(THIS.cOutFile) + C_MACEXT + JustExt(THIS.cOutFile),LEN(key))))
		ENDIF

		PJXConverterBase::InsertSCX

	ENDPROC		&& FPCConverter:InsertSCX


	*------------------------------------
	PROCEDURE Cleanup					&& FPCConverter
	*------------------------------------
		*- this proc is called by Error, and tries to put things back the way they were
		*- for catalogs, the originals have the x2x type extension. They need to be renamed.

		IF USED(THIS.pjx25Alias)
			SELECT file_name, type FROM DBF(THIS.pjx25Alias) ;
			    WHERE type $ C_FPCSCREENTYPE + C_FPCREPORTTYPE + C_FPCLABELTYPE AND !DELETED() ;
				INTO ARRAY tmparr
		ELSE
			_TALLY = 0
		ENDIF

		*- force deletion of any lingering temp screen or report files
		IF _TALLY > 0
			m.nTables = AUSED(au)
			FOR i = 1 TO m.nTables
				IF LEFT(au[i,1],1) = "S" OR LEFT(au[i,1],1) = "F"
					cTmpFname = DBF(au[i,1])
					cExt = UPPER(JustExt(cTmpFname))
					IF ASCAN(tmparr,UPPER(JustFName(cTmpFName))) > 0
						*- oops -- it;s one of the real ones
						LOOP
					ENDIF
					IF INLIST(cExt,C_SCXEXT,"FRX","LBX")
						USE IN (au[i,1])
						ERASE (cTmpFname)
						ERASE FORCEEXT(cTmpFname,IIF(m.cExt = C_SCXEXT,C_SCTEXT,;
							IIF(m.cExt = "FRX","FRT","LBT")))
					ENDIF
				ENDIF
			NEXT
		ENDIF

		CLOSE TABLES
		IF THIS.lBackUp
			FOR i = 1 TO _TALLY
				m.cTmpFname = FULLPATH(ALLTRIM(IIF(AT(CHR(0),tmparr[m.i,1]) > 0,;
					LEFT(tmparr[m.i,1],AT(CHR(0),tmparr[m.i,1])-1),tmparr[m.i,1])),THIS.cHomeDir)
				IF FILE(m.cTmpFname)
					DO CASE
						CASE tmparr[m.i,1] = C_SCXTYPE
							IF	FILE(m.cTmpFname) AND ;
								FILE(FORCEEXT(m.cTmpFname,C_SCTEXT)) AND ;
								FILE(FORCEEXT(m.cTmpFname,C_SCXBACKEXT)) AND ;
								FILE(FORCEEXT(m.cTmpFname,C_SCTBACKEXT))
								DELETE FILE (m.cTmpFname)
								DELETE FILE (FORCEEXT(m.cTmpFname,C_SCTEXT))
								RENAME (FORCEEXT(m.cTmpFname,C_SCXBACKEXT)) TO (m.cTmpFname)
								RENAME (FORCEEXT(m.cTmpFname,C_SCTBACKEXT)) TO (FORCEEXT(m.cTmpFname,C_SCTEXT))
							ENDIF
						CASE tmparr[m.i,1] = C_REPORT
							IF	FILE(m.cTmpFname) AND ;
								FILE(FORCEEXT(m.cTmpFname,"LBT")) AND ;
								FILE(FORCEEXT(m.cTmpFname,C_LBXBACKEXT)) AND ;
								FILE(FORCEEXT(m.cTmpFname,C_LBTBACKEXT))
								DELETE FILE (m.cTmpFname)
								DELETE FILE (FORCEEXT(m.cTmpFname,"LBT"))
								RENAME (FORCEEXT(m.cTmpFname,C_LBXBACKEXT)) TO (m.cTmpFname)
								RENAME (FORCEEXT(m.cTmpFname,C_LBTBACKEXT)) TO (FORCEEXT(m.cTmpFname,"LBT"))
							ENDIF
						CASE tmparr[m.i,1] = C_LABEL
							IF	FILE(m.cTmpFname) AND ;
								FILE(FORCEEXT(m.cTmpFname,"FRT")) AND ;
								FILE(FORCEEXT(m.cTmpFname,C_FRXBACKEXT)) AND ;
								FILE(FORCEEXT(m.cTmpFname,C_FRTBACKEXT))
								DELETE FILE (m.cTmpFname)
								DELETE FILE (FORCEEXT(m.cTmpFname,"FRT"))
								RENAME (FORCEEXT(m.cTmpFname,C_FRXBACKEXT)) TO (m.cTmpFname)
								RENAME (FORCEEXT(m.cTmpFname,C_FRTBACKEXT)) TO (FORCEEXT(m.cTmpFname,"FRT"))
							ENDIF
					ENDCASE
				ENDIF && cTmpFname (new file) exists
			NEXT

			*-now, erase files in backup dir
			=ADIR(af,THIS.cBackDir + "*.*")
			IF TYPE("af") # 'U'
				FOR i = 1 TO ALEN(af,1)
					IF af[i,5] $ 'RD'
						LOOP
					ENDIF
					DELETE FILE (THIS.cBackDir + af[i,1])
				NEXT
				RELEASE af
			ENDIF
			IF ADIR(af,THIS.cBackDir + "*.*") = 0
				IF TYPE("af") = 'U'
					RD (THIS.cBackDir)
				ENDIF
			ENDIF
		ENDIF && THIS.lBackUp

		IF FILE(THIS.PJXname)
			DELETE FILE (THIS.PJXname)
		ENDIF
		IF FILE(FORCEEXT(THIS.PJXname,"PJT"))
			DELETE FILE (FORCEEXT(THIS.PJXname,"PJT"))
		ENDIF
		IF FILE(FORCEEXT(THIS.PJXname,"FCT"))
			DELETE FILE (FORCEEXT(THIS.PJXname,"FCT"))
		ENDIF

	ENDPROC					&& FPCConverter:Cleanup

	*------------------
	PROCEDURE ClosePJX		&& FPCConverter
	*------------------
		m.cTmpFname = FULLPATH(THIS.cCurrentFile,THIS.cHomeDir)
	
		THIS.CloseFiles

		THIS.cFull30PJXName = FORCEEXT(m.cTmpFname,"PJX")

		*- get rid of temp file
		IF FILE(THIS.pjxName)
			DELETE FILE (THIS.pjxName)
		ENDIF
		IF FILE(FORCEEXT(THIS.pjxName,THIS.cMemoExt))
			DELETE FILE (FORCEEXT(THIS.pjxName,THIS.cMemoExt))
		ENDIF

		*- get rid of original -- back-up has been saved away...
		IF THIS.lBackUp
			IF FILE(THIS.cCurrentFile)
				DELETE FILE (THIS.cCurrentFile)
			ENDIF
			IF FILE(FORCEEXT(THIS.cCurrentFile,THIS.cMemoExt))
				DELETE FILE (FORCEEXT(THIS.cCurrentFile,THIS.cMemoExt))
			ENDIF
		ENDIF

		IF FILE(THIS.cFull30PJXName)
			DELETE FILE (THIS.cFull30PJXName)
		ENDIF
		RENAME (ADDBS(JUSTPATH(m.cTmpFname)) + THIS.new30alias + ".PJX") TO (THIS.cFull30PJXName)

		IF FILE(FORCEEXT(m.cTmpFname,"PJT"))
			DELETE FILE (FORCEEXT(m.cTmpFname,"PJT"))
		ENDIF
		RENAME (ADDBS(JUSTPATH(m.cTmpFname)) + THIS.new30alias + ".PJT") TO (FORCEEXT(m.cTmpFname,"PJT"))

	ENDPROC		&& FPCConverter:ClosePJX

ENDDEFINE		&&  FPCConverter

**********************************************
DEFINE CLASS SCXSingleScreenConverter AS ConverterBase
**********************************************

	*- If someone wants to do their own converter:
	*- create their own object classes, 
	*- sub-class this class, 
	*- store their object class names in these variables

	formclass		= "fp25form"
	labelclass		= "fp25lbl"
	sayclass		= "fp25say"
	lineclass		= "fp25line"
	shapeclass		= "fp25shape"
	editclass		= "fp25edit"
	getclass		= "fp25get"
	spinclass		= "fp25spin"
	cboxclass		= "fp25cbox"
	listclass		= "fp25list"
	popupclass		= "fp25popup"
	pictclass		= "fp25pict"
	radioclass		= "fp25radio"
	*-btnclass    	= "fp25btn"
	btnclass    	= "fp25btngrp"
	btngclass		= "fp25btngrp"
	*-invclass		= "fp25invbtn"
	invclass		= "fp25invgrp"
	invgclass		= "fp25invgrp"
	oleclass		= "fp25ole"
	datanavclass	= "fpdatanav"				&& data navigation object & cursor object
	datanavRelationClass = "fpdatanavRelation"	&& data navigation relation object
	
	specialfx = .T.								&& add special fx? (i.e., make controls 3D?)

	oConvForm = .NULL.

	*- Object instance variables
	nObjCount = 0
	cStubFile = ""
	scxcount = 0
	timestamp = 0
	cNewScx = ""				&& New SCX file name
	isMultiPlat = .F.
	curPlat = ""
	savedPlat = ""				&& the platform we save may not be the one we are on (VFP Mac SCX's always have WINDOWS)
	platform = ""
	parentName = ""
	cParms = ""					&& parameter statement -- if any
	formnum =  1
	fp3prop	= ""				&& properties
	fp3method = ""				&& methods
	GetBorder = .T.
	itse_expr = ""
	read_expr = ""
	wclause_expr = ""
	cWnameExpr = ""
	noReadExpr = .F.
	noReadPlainExpr = .F.
	fontsub = ""
	lMultiReads = .F.
	cBackDir = ""
	lUserCall = .T.
	projcall = .F.
	lHasDataNavObj = .F.
	lIndirectWinName = .F.
	cIndirectWinName = ""
	cFormName =""
	nDNOCount = 0
	nDNORecNo = 0				&& remember record # of DataEnvironment (nee DNO) record
	lConverted = .F.			&& catch scx's in 3.0 format
	cReadShow = ""				&& collects code for SAYs that need to be refreshed
	nFSetRecno = 0				&& formset record number
	nFormRecno = 0				&& form record number
	cHeaderID = "Screen"		&& UniqueID for header 1 record
	cDefineWin = ""				&& DEFINE WINDOW command
	lHasInvis = .F.				&& flag for if has invisible buttons
	cProcs = ""					&& accumulate Cleanup procs from multiple screens in set
	lHasIDX	= .F.				&& IDX files used in environment?
	cMainCurs = ""				&& alias of main table
	cWinNames = ""				&& accumulate list of window names, to prevent duplicates
	iFormSetCtr = 0				&& counter for formsets
	cFormSetName = ""			&& formset name

	lhasSys16 = .F.				&& does SYS(16) appear in code?
	lHasReturn = .F.			&& code returns a value
	lNoCompile = .F.			&& flag to determine whether to compile right away, or later, in batch

	iPlatformCount = 1			&& how many platforms do we need to deal with?
	iWhichPlat = 1				&& which platform are we doing?

	*- Arrays
	DIMENSION a_plat[1]
	a_plat = ""
	DIMENSION a_reads[8]
	a_reads = ""
	DIMENSION a_scx2files[1,3]
	a_scx2files = ""
	DIMENSION a_scx2alias[1]
	a_scx2alias = ""
	DIMENSION a_scx3files[1]
	a_scx3files = ""
	DIMENSION a_scx3alias[1]
	a_scx3alias = ""
	DIMENSION a_pjxsets[10]
	a_pjxsets= ""

	DIMENSION a_tables[1]
	DIMENSION a_torder[1]
	a_tables = ""
	a_torder = ""

	*------------------------------------
	PROCEDURE Init				&& SCXSingleScreenConverter
	*------------------------------------
		PARAMETER aParms, lBackup, lProjCall, lForceTransportDlog, lNoCompile

		LOCAL m.imaxThisTime, m.imaxOtherTime

		THIS.oConvForm = THIS

		THIS.projcall = lProjCall

		THIS.lBackup = m.lBackup

		THIS.lTransDlog = lForceTransportDlog

		THIS.lNoCompile = lNoCompile

		THIS.lDevMode = aParms[7]
		THIS.cCodeFile = aParms[8]
		THIS.llog = aParms[9]
		THIS.cLogFile = aParms[10]
		THIS.iPlatformCount = MAX(aParms[12],1)
		THIS.iWhichPlat = aParms[13]
		THIS.savedPlat = C_WINDOWS				&& always WINDOWS on VFP Mac (jd 03/27/96)

		PRIVATE j

		DIMENSION a_pjxsets[10]
		a_pjxsets= ""

		*- populate settings array with default options
		THIS.a_pjxsets[A_OPENFILES]  = IIF(THIS.projcall,gOPJX.a_pjxsets[A_OPENFILES],.T.)	&& open DBF files
		THIS.a_pjxsets[A_CLOSEFILES] = IIF(THIS.projcall,gOPJX.a_pjxsets[A_CloseFiles],.T.)	&& close DBF files
		THIS.a_pjxsets[A_DEFWINDOWS] = IIF(THIS.projcall,gOPJX.a_pjxsets[A_DEFWINDOWS],.T.)	&& define windows
		THIS.a_pjxsets[A_RELWINDOWS] = IIF(THIS.projcall,gOPJX.a_pjxsets[A_RELWINDOWS],.T.)	&& release windows
		THIS.a_pjxsets[A_READMODAL]  = IIF(THIS.projcall,gOPJX.a_pjxsets[A_READMODAL],.F.)	&& READ MODAL
		THIS.a_pjxsets[A_GETBORDERS] = IIF(THIS.projcall,gOPJX.a_pjxsets[A_GETBORDERS],.T.)	&& border for GETs
		THIS.a_pjxsets[A_READCYCLE]  = IIF(THIS.projcall,gOPJX.a_pjxsets[A_READCYCLE],.T.)	&& READ CYCLE
		THIS.a_pjxsets[A_READNOLOCK] = IIF(THIS.projcall,gOPJX.a_pjxsets[A_READNOLOCK],.F.)	&& READ NOLOCK
 		THIS.a_pjxsets[A_MULTIREADS] = IIF(THIS.projcall,gOPJX.a_pjxsets[A_MULTIREADS],.F.)	&& multiple READs
 		THIS.a_pjxsets[A_ASSOCWINDS] = IIF(THIS.projcall,gOPJX.a_pjxsets[A_ASSOCWINDS],"")	&& associated windows
		THIS.lMultiReads = THIS.a_pjxsets[A_MULTIREADS]

		THIS.lAutoClose = THIS.a_pjxsets[A_CLOSEFILES]

		IF THIS.projcall										&& called from project
			THIS.cBackDir = gOPJX.cBackDir						&& used only for project
			DIMENSION THIS.a_scx2files[ALEN(gOPJX.a_s2files,1),3]
			DIMENSION THIS.a_scx3files[ALEN(gOPJX.a_s3files,1)]
			=ACOPY(gOPJX.a_s2files,THIS.a_scx2files)
			=ACOPY(gOPJX.a_s3files,THIS.a_scx3files)			
			THIS.cStubFile = gOPJX.cStubFile
			THIS.cNewScx = THIS.a_scx3files[1]
			IF !THIS.lMultiReads
				DIMENSION THIS.a_scx3files[1]
			ENDIF
			THIS.cCurrentFile = THIS.a_scx2files[1,3]
		ELSE
			IF EMPTY(aParms[1])
				THIS.lUserCall = .F.	&& assume called without output file
				aParms[1] = ADDBS(JUSTPATH(aParms[4])) + LEFT(SYS(3),7) + ".SCX"
				THIS.cNewScx = aParms[4]
			ELSE
				THIS.cNewScx = aParms[1]
			ENDIF
			THIS.a_scx3files[1] = aParms[1]
			THIS.cStubFile = FORCEEXT(THIS.cNewScx,C_SPREXT)
			DIMENSION THIS.a_scx2files[1,3]
			THIS.a_scx2files[1,1] = aParms[4]
			THIS.a_scx2files[1,2] = ""
			THIS.a_scx2files[1,3] = aParms[4]
			THIS.cCurrentFile = THIS.a_scx2files[1,3]
			*- go ahead and make backup now, before we start
			*- if screen needs to be transported, the original is around
			IF THIS.lBackUp
				*- copy old screen with S2X,S2T extensions. No need to backup .SCR files
				IF FILE(THIS.a_scx2files[1]) AND UPPER(JUSTEXT(THIS.a_scx2files[1])) = C_SCXEXT
					COPY FILE (THIS.a_scx2files[1]) TO (FORCEEXT(THIS.a_scx2files[1],C_SCXBACKEXT))
				ENDIF
				IF FILE(FORCEEXT(THIS.a_scx2files[1],C_SCTEXT))
					COPY FILE (FORCEEXT(THIS.a_scx2files[1],C_SCTEXT)) TO (FORCEEXT(THIS.a_scx2files[1],C_SCTBACKEXT))
				ENDIF
				*- backup has been done
				THIS.lBackUp = .F.
			ENDIF
		ENDIF

		gOTherm.SetTitle(C_THERMMSG2_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
		gOTherm.Update(0,"")
		gOTherm.visible = .T.

		THIS.platform = THIS.GetPlatForm(THIS.iWhichPlat)

		THIS.WriteLog("","")					&& force a blank line
		THIS.BeginLog(SYS(2027,THIS.cNewScx) + IIF(THIS.platform == C_MAC AND THIS.iPlatformCount > 1, " " + C_MACLOGMSG_LOC,""))

		THIS.nTimeStamp = THIS.TStamp()
		THIS.scxcount = ALEN(THIS.a_scx2files,1)

		DIMENSION THIS.a_scx2alias[THIS.scxcount]

		FOR j = 1 TO THIS.scxcount

			IF !THIS.KnownFile(THIS.a_scx2files[m.j,1])
				*- unknown file format -- error has already been written to the log
				THIS.oConvForm = .NULL.
				IF !THIS.projcall
					*- attempt to remove backup files (jd 04/16/96)
					THIS.EraseBackup
				ENDIF
				RETURN .F.
			ENDIF

			*- also check Read-Only status if project call (04/15/96 jd)
			IF THIS.projcall AND pReadOnly(THIS.cNewSCX)
				THIS.WriteLog(JUSTFNAME(THIS.cNewSCX),TRIM(E_FILE_LOC) + E_NOCONVERT3_LOC)
				RETURN .F.
			ENDIF

			*- now try to open SCX file -- returns alias
			THIS.a_scx2alias[m.j] = THIS.OpenFile(THIS.a_scx2files[m.j,1])
				
			IF EMPTY(THIS.a_scx2alias[m.j])
				*- error has been logged
				THIS.oConvForm = .NULL.
				IF !THIS.projcall
					*- attempt to remove backup files (jd 04/16/96)
					THIS.EraseBackup
				ENDIF
				RETURN .F.
			ENDIF

			IF j = 1
				THIS.c25alias = THIS.a_scx2alias[m.j]
			ENDIF

			*- Check for file format
			DO CASE
				CASE FCOUNT() = C_SCXFLDS AND FIELD(1) = "PLATFORM"
					*- 2.5 SCX type
					LOCATE FOR PLATFORM = THIS.platform
					IF !FOUND()
						*-  no platform objects for the current platform
						IF !THIS.Conv20SCX(12)
							THIS.lHadError = .T.
							THIS.oConvForm = .NULL.
							THIS.EraseBackup
							RETURN
						ENDIF
					ELSE
						*- check to see if any records are later than current platform records
						*- if so, call transporter
						CALCULATE MAX(timestamp) FOR platform = THIS.platform TO m.imaxThisTime
						CALCULATE MAX(timestamp) FOR platform # THIS.platform TO m.imaxOtherTime
						IF m.imaxOtherTime > m.imaxThisTime
							IF !THIS.Conv20SCX(12)
								THIS.lHadError = .T.
								THIS.oConvForm = .NULL.
								THIS.EraseBackup
								RETURN
							ENDIF
						ENDIF
					ENDIF
				CASE FCOUNT() = C_20SCXFLDS AND FIELD(1) = "OBJTYPE"
					*- 2.0 SCX type
					*- Invoke Transporter
					*- =MESSAGEBOX(E_HAS20FILE_LOC)
					IF !THIS.Conv20SCX(2)
						THIS.lHadError = .T.
						THIS.oConvForm = .NULL.
						THIS.EraseBackup
						RETURN
					ENDIF
				CASE FCOUNT() = C_30SCXFLDS AND FIELD(4) = "CLASS"
					*- assume 3.0 format, screen was already converted
					*- (this could be called while converting a 2.x Project
					*- that contains scx's that have already been converted)
					IF j = 1
						*- some of the defaults for VFP 4.0 have changed. So if user
						*- elected to retain the 3.0 behavior/defaults, we need to explicitly
						*- write out those properties (05/14/96 jd)
						*- this means that the checkbox is checked
						IF !THIS.Set30Defaults()
							THIS.lHadError = .T.
							THIS.oConvForm = .NULL.
							THIS.EraseBackup
						ENDIF
						THIS.lConverted = .T.	&& only set this if main screen
						RETURN
					ELSE
						*- screen is converted, but part of a screen set, so allow to continue
					ENDIF
				OTHERWISE
					USE IN (THIS.a_scx2alias[m.j])
					THIS.WriteLog(JUSTFNAME(THIS.a_scx2files[m.j,1]),TRIM(E_WRONGFMT_LOC) + E_NOCONVERT_LOC)
					THIS.oConvForm = .NULL.
					RETURN
			ENDCASE
		
		ENDFOR	&& end of opening SCX files in set
		
		*- this cursor is for working with memo fields
		IF USED("_FOX3SPR")
		  USE IN _FOX3SPR
		ENDIF

		CREATE CURSOR _FOX3SPR (sprmemo m, temp1 m, temp2 m, temp3 m, temp4 m,defines m, load m, code m)
		APPEND BLANK
		REPLACE _FOX3SPR.load WITH "PROCEDURE " + C_DELOAD_METH + C_CR, ;
			_FOX3SPR.code WITH C_CRLF + C_CODEHDR1_LOC + C_CODEHDR_LOC + THIS.a_scx2files[1,3] + C_CRLF + C_CODEHDR1_LOC + C_CRLF

		IF THIS.lDevMode AND THIS.projcall AND USED("_FOX3PJX")
			*- accumulate all of code in final screen
			REPLACE  _FOX3SPR.sprmemo WITH _FOX3PJX.sprmemo + C_CR
		ENDIF

		SELECT (THIS.c25alias)

	ENDPROC		&&  SCXSingleScreenConverter:Init
	
	*----------------------------------
	PROCEDURE Converter		&& SCXSingleScreenConverter
	*----------------------------------
		PRIVATE i, j
		LOCAL oRec, nrec, cFormset

		*- Get platforms used in Screen Set
		*- returns total records to process for Thermometer
		THIS.nRecCount = THIS.ScanPlat(THIS.platform) * THIS.iPlatformCount		&& accommodate doing multiple platforms
		THIS.nTmpCount = 1  	&& reset

		gOTherm.SetTitle(C_THERMMSG2_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
		gOTherm.Update(0)

		*- External preprocessor hook
	  	THIS.PreForm

		*- Create new SCX file(s) here
		*- Multiple files created if MultiReads option.
		THIS.CreateSCX
		THIS.new30alias = THIS.a_scx3alias[1]

		*- Add objects by platform
		FOR m.i = 1 TO 1		&& ALEN(THIS.a_plat)

			*- Add environment info
			SELECT (THIS.c25alias)		&& only check first file

			GO TOP
			IF environ
				LOCATE FOR INLIST(objtype,2,3,4) AND platform = THIS.a_plat[m.i] 
				IF FOUND()
					THIS.AddSRecs(m.i,C_DNO)
				ENDIF
			ENDIF
			
			*- check for indirect reference to window name (name expr)
			IF LEFT(name,1) = "("
				*- indirect ref to window name
				*- make up a munged name
				m.ctmpname = ALLT(name)
				THIS.cFormName = T_FORM + "1"
				THIS.lIndirectWinName = .T.
				THIS.cIndirectWinName = SUBS(m.ctmpname,2,LEN(m.ctmpname) - 2)
			ENDIF

			*- Create Screen Set object
			THIS.AddFSet(THIS.a_plat[m.i])

			gOTherm.Update(THIS.nTmpCount/THIS.nRecCount * 100)

			FOR m.j = 1 TO THIS.scxcount

				*- Use separate files if Multi-Read option
				IF THIS.lMultiReads
					THIS.new30alias = THIS.a_scx3alias[m.j]
				ENDIF
				
				*- Select screen to process in screen set
				SELECT (THIS.a_scx2alias[m.j])
				THIS.formnum = m.j

				*- if this screen isn;t the main screen, it may already have been converted
				*- so... just add the records to the main screen
				IF FCOUNT() = C_30SCXFLDS AND FIELD(4) = "CLASS"
					*- this screen was already converted
					LOCATE FOR class = T_FSET
					m.cFormSet = objname
					USE
					SELECT (THIS.new30alias)
					m.nRec = RECC()
					APPEND FROM (THIS.cBackDir + THIS.a_scx2files[m.j,3]) FOR uniqueid # "Screen" AND uniqueid # "FONTINFO" AND LOWER(class) # "formset"
					REPLACE ALL uniqueID WITH "~" + CHR(64 + j) FOR RECNO() > m.nrec
					REPLACE ALL parent WITH STRTRAN(parent,m.cFormSet,THIS.cFormSetName)	&& change formset to this formset
					SELECT (THIS.c25alias)
					LOOP
				ENDIF

				*- Add Screen objects
				THIS.AddSRecs(m.i,C_CONTROLS)
				IF THIS.lHadError	&& RED00N4G
					RETURN -1
				ENDIF
			
				*- collect procs for this screen
				THIS.AddProcs1

				IF m.j = 1 OR THIS.lMultiReads
			    	*- Write FontInfo -- there is only a single record for 
			    	*- FontInfo regardless of number of forms.
					THIS.WriteFontSub()
				ENDIF

				IF m.j > 1 AND THIS.lMultiReads AND THIS.projcall
					LOCAL m.cOldOutFile
					m.cOldOutFile = goPJX.cOutFile
					goPJX.cOutFile = THIS.a_scx3Files[j]
					goPJX.InsertSCX(.T.)
					goPJX.cOutFile = m.cOldOutFile
				ENDIF

	   		ENDFOR  && end of screen loop


		ENDFOR	&& end of platform loop

		*- Add stub file (SPR) statements
		SELECT (THIS.c25alias)
		THIS.AddParm

		*- Add Cleanup snippet procs/funcs code here for SPR file
		THIS.AddGenProcs

		*- add SAY refresh code into FormSet.ReadShow method
		THIS.UpdMethods

		*- External postprocessor hook
  		THIS.PostForm

		*- Close screen files
		THIS.CloseFiles

		*- close up gOTherm
		IF THIS.iWhichPlat == THIS.iPlatformCount
			*- don;t stop thermometer unless we are really done...
			gOTherm.Complete
		ENDIF

		*- release the reference
		THIS.oConvForm = .NULL.

		*- write to log file
		THIS.EndLog(THIS.cNewScx + IIF(THIS.platform == C_MAC AND THIS.iPlatformCount > 1, " " + C_MACLOGMSG_LOC,""))

		RETURN THIS.a_scx2files[1,1]

	ENDPROC	&& SCXSingleScreenConverter:Converter

	*------------------
	PROCEDURE EraseBackup			&& SCXSingleScreenConverter
	*------------------
		IF FILE(FORCEEXT(THIS.a_scx2files[1],C_SCXBACKEXT))
			ERASE (FORCEEXT(THIS.a_scx2files[1],C_SCXBACKEXT))
		ENDIF
		IF FILE(FORCEEXT(THIS.a_scx2files[1],C_SCTBACKEXT))
			ERASE (FORCEEXT(THIS.a_scx2files[1],C_SCTBACKEXT))
		ENDIF
	ENDPROC

	*------------------
	PROCEDURE KnownFile			&& SCXSingleScreenConverter
	*------------------
		*- verify that the file is a known format
		*- we only test for previous versions of FoxPro, .FMT files, and dBASE IV .scr files
		PARAMETER cFileName

		LOCAL oThis, cNewSCXName
		oThis = THIS
		cNewSCXName = m.cFileName

		*- does file exist?
		IF FILE(m.cFileName)
			*- can it be opened?
			IF Readable(m.cFileName)
				*- is it a DBF?
				IF !THIS.IsDBF(m.cFileName)
					*- not a DBF, so try and migrate it
					SET MESSAGE TO C_MIGRATEMSG_LOC
					gOTherm.SetTitle(C_THERMMSG11_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
					IF !MigDB4(m.cFileName, @oThis)
						THIS.WriteLog(JUSTFNAME(m.cFileName),E_NOMIG_LOC)
						=MESSAGEBOX(E_WRONGFMT_LOC + " " + E_NOCONVERT_LOC)
						THIS.lHadError=.T.
						oThis = .NULL.
						RETURN .F.
					ELSE
						*- go ahead and transport
						IF FILE(FORCEEXT(m.cNewSCXName,C_SCXEXT)) AND FILE(FORCEEXT(m.cNewSCXName,C_SCTEXT))
							*- assume migrated okay, so update names of files we are working with
							THIS.a_scx2files[m.j,1] = FORCEEXT(m.cNewSCXName,C_SCXEXT)
							THIS.cCurrentFile = THIS.a_scx2files[m.j,1]
						ELSE
							=MESSAGEBOX(E_NOMIG_LOC + " " + E_NOCONVERT_LOC)
							THIS.lHadError=.T.
							oThis = .NULL.
							RETURN .F.
						ENDIF
						gOTherm.SetTitle(C_THERMMSG7_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
						gOTherm.Update(0,"")
						LOCAL m.lOldScxShow
						m.lOldScxShow = gAShowMe[N_TRANFILE_SCX,1]
						gAShowMe[N_TRANFILE_SCX,1] = .F.
						DO (gTransport) WITH THIS.cCurrentFile,12,.F.,gAShowMe, m.gOTherm,THIS.cCurrentFile
						gAShowMe[N_TRANFILE_SCX,1] = m.lOldScxShow
					ENDIF
				ENDIF
			ENDIF
		ENDIF
		oThis = .NULL.

	ENDFUNC		&& SCXSingleScreenConverter:KnownFile

	*------------------
	PROCEDURE CreateSCX			&& SCXSingleScreenConverter
	*------------------
		PRIVATE m.cScxName,m.tmpalias,m.j
		*- Note: only create more than 1 new SCX table if 
		*- Multi-Read generate option is selected. Otherwise
		*- terminate at end of first loop.
		
		FOR m.j = 1 TO THIS.scxcount
			m.cScxName = THIS.a_scx3files[m.j]
			DIMENSION THIS.a_scx3alias[m.j]

			*- create new SCX file
			CREATE TABLE (m.cScxName) ;
				(platform c(8),;
				uniqueid c(10),;
				timestamp n(10),;
				class m,;
				classloc m,;
				baseclass m,;
				objname m,;
				parent m,;
				properties m,;
				protected m,;
				methods m,;
				objcode m,;
				ole m,;
				ole2 m,;
				reserved1 m,;
				reserved2 m,;
				reserved3 m,;
				reserved4 m,;
				reserved5 m,;
				reserved6 m,;
				reserved7 m,;
				reserved8 m,;
				user m)

			m.tmpalias = ALIAS()
			THIS.a_scx3alias[m.j] = ALIAS()

			*- Add header comment record
			INSERT INTO (m.tmpalias) ;
				(platform,uniqueid,reserved1);
				VALUES ("COMMENT",THIS.cHeaderID,C_SCXVERSTAMP)

			IF !THIS.lMultiReads
				EXIT
			ENDIF
		ENDFOR

	ENDPROC			&& SCXSingleScreenConverter
	
	*------------------
	PROCEDURE Conv20SCX
	*------------------
	*- This converts a foreign scx to a 2.x current platform.
		PARAMETER m.scxtype
		*- m.scxtype = 12		&& FP2.5 SCX format
		*- m.scxtype = 2		&& FP2.0 SCX format
		LOCAL m.oldudfp
		LOCAL m.cOldMess

		USE IN (THIS.a_scx2alias[m.j])
		gOTherm.SetTitle(C_THERMMSG7_LOC + LOWER(PARTIALFNAME(THIS.a_scx2files[m.j,3],C_FILELEN)))
		m.oldudfp = SET("UDFP")
		SET UDFP TO REFERENCE
		m.cOldMess = SET("MESSAGE",1)
		DO (gTransport) WITH THIS.a_scx2files[m.j,1],m.scxtype,.F.,gAShowMe, m.gOTherm,THIS.a_scx2files[m.j,3],THIS.lTransDlog
		SET UDFP TO &oldudfp
		SET MESSAGE TO (cOldMess)
		THIS.a_scx2alias[m.j] = THIS.OpenFile(THIS.a_scx2files[m.j,1])
		THIS.c25alias = THIS.a_scx2alias[m.j]
		IF !EMPTY(THIS.a_scx2alias[m.j])
			IF FCOUNT() = C_SCXFLDS AND FIELD(1) = "PLATFORM"
				LOCATE FOR Platform = THIS.Platform
				IF FOUND()
					RETURN .T.
				ENDIF
			ENDIF
			USE IN (THIS.a_scx2alias[m.j])
		ENDIF
		THIS.lHadError=.T.
		RETURN .F.
	ENDPROC

	*------------------------------------
	PROCEDURE AddParm			&& SCXSingleScreenConverter
	*------------------------------------
		PRIVATE j,cScxName
		LOCAL nParmCount, cParmCont, k, cParmCode, cParmA, npos
		LOCAL aTemp, cTmpText, cItem, nArryLen, cPubList, k, cDoForm

		cPubList = ""

		*- Base parameter statement on first SCX only in FP25.
		LOCATE FOR objtype = 1
		THIS.cParms = GetParam("setupcode")
		
		*- assume THIS.cParms was set in AddReads earlier
		IF !EMPTY(THIS.cParms)
			REPLACE _FOX3SPR.sprmemo WITH ;
				C_PARM1_CMMT_LOC + ;
				"PARAMETERS " + THIS.cParms + C_CRLF ADDITIVE

			*- need to add special code in case no parms passed, so 
			*- we don;t pass on default parms that shouldn;t be there
			m.nParmCount = OCCURS(",",THIS.cParms) + 1
			cParmCode = C_CRLF + ;
						"LOCAL _aParm, _cparmstr, _nctr" + C_CRLF + ;
						"DIMENSION _aParm[" + LTRIM(STR(nParmCount)) + "]" + C_CRLF
			*- assign parameter to an array, to build up a parameter clause to pass on
			m.npos = 1
			FOR m.k = 1 TO m.nParmCount
				*- determine the parameter
				IF m.k = m.nParmCount
					cParmA = SUBS(THIS.cParms,npos)
				ELSE
					cParmA = SUBS(THIS.cParms,npos,AT(",",THIS.cParms,m.k) - npos)
				ENDIF
				m.cParmCode = m.cParmCode + ;
						"_aParm[" + LTRIM(STR(m.k)) + "] = [" + ALLT(cParmA) + "]" + C_CRLF
				m.npos = AT(",",THIS.cParms,m.k) + 1
			NEXT

			cParmCode = m.cParmCode + ;
						C_CRLF + ;
						"_cparmstr = []" + C_CRLF + ;
						C_CRLF + ;
						"IF PARAMETERS() > 0" + C_CRLF + ;
						C_TAB + "_cparmstr = [WITH ]" + C_CRLF + ;
						C_TAB + "_cparmstr = _cparmstr + _aParm[1]" + C_CRLF + ;
						C_TAB + "FOR m._nctr = 2 TO PARAMETERS()" + C_CRLF + ;
						C_TAB + C_TAB + "_cparmstr = _cparmstr + [,] + _aParm[m._nctr]" + C_CRLF + ;
						C_TAB + "NEXT" + C_CRLF + ;
						"ENDIF" + C_CRLF + C_CRLF

			REPLACE _FOX3SPR.SPRMEMO WITH cParmCode ADDITIVE
		ENDIF

		*- add in EXTERNAL lines
		IF !EMPTY(THIS.a_Dimes)
			m.nArryLen = ALEN(THIS.a_Dimes)
			FOR m.j = 1 TO m.nArryLen
				*- expand array list (maybe multiple arrays were declared 
				*- with one DIMENSION or DECLARE) (jd 03/11/96)
				DECLARE a_Dimes2[1]
				=GetArray(THIS.a_Dimes[j],@a_dimes2)
				IF ALEN(a_dimes2,1) > 1
					*- copy rest of array
					THIS.a_Dimes[j] = a_dimes2[1]										&& replace long item with just the first item
					DIMENSION THIS.a_Dimes[ALEN(THIS.a_Dimes) + ALEN(a_dimes2,1) - 1]	&& grow array
					=ACOPY(a_dimes2, THIS.a_Dimes,2,-1,ALEN(THIS.a_Dimes) - ALEN(a_dimes2,1) + 2)	&& 
				ENDIF
			NEXT
			*- remove the array dimensions
			FOR m.j = 1 TO ALEN(THIS.a_Dimes)
				THIS.a_Dimes[j] = StripParen(THIS.a_Dimes[j],'(',')')
				THIS.a_Dimes[j] = StripParen(THIS.a_Dimes[j],'[',']')
			NEXT
			*- remove duplicates
			DECLARE aTemp[1]
			aTemp = ""
			m.nArryLen = 1
			FOR m.j = 1 TO ALEN(THIS.a_Dimes)
				m.cTmpText = ALLT(THIS.a_Dimes[m.j])
				IF ISALPHA(m.cTmpText) OR LEFT(m.cTmpText,1) == "_"
					*- valid name (not a name expression)
					m.cTmpText = ALLT(IIF("&" + "&" $ m.cTmpText,LEFT(m.cTmpText,AT("&" + "&",m.cTmpText) - 1), m.cTmpText))
					m.cTmpText = ALLT(IIF(";" $ m.cTmpText,LEFT(m.cTmpText,AT(";",m.cTmpText) - 1), m.cTmpText))
					DO WHILE !EMPTY(m.cTmpText)
						IF "," $ m.cTmpText
							m.cItem = LEFT(m.cTmpText,AT(",",m.cTmpText) - 1)
							m.cTmpText = SUBS(m.cTmpText,AT(",",m.cTmpText) + 1)
						ELSE
							m.cItem = m.cTmpText
							m.cTmpText = ""
						ENDIF
						IF !EMPTY(aTemp[1])
							DIMENSION aTemp[m.nArryLen + 1]
							m.nArryLen = m.nArryLen + 1
						ENDIF
						aTemp[m.nArryLen] = LOWER(ALLT(m.cItem))
					ENDDO
				ENDIF
			NEXT

			=ASORT(aTemp)
			m.cArrays = C_EXTERN_CMMT_LOC
			FOR m.j = 1 TO ALEN(aTemp)
				IF m.j = 1 OR aTemp[m.j-1] != aTemp[m.j]
					m.cArrays = m.cArrays + "EXTERNAL ARRAY " + aTemp[m.j] + C_CRLF
				ENDIF
			NEXT
			REPLACE _FOX3SPR.SPRMEMO WITH m.cArrays + C_CRLF ADDITIVE
		ENDIF

		m.cScxName = THIS.cNewScx
		FOR m.j = 1 TO THIS.scxcount
			IF m.j # 1
				m.cScxName = THIS.a_scx3files[m.j]
			ENDIF
			IF THIS.lHasReturn
				REPLACE _FOX3SPR.SPRMEMO WITH C_RETVAL_CMMT_LOC + ;
					"LOCAL _rval" + C_CRLF + C_CRLF ADDITIVE
			ENDIF

			IF THIS.lHasDataNavObj AND THIS.a_pjxsets[A_OPENFILES]
				*- create PUBLIC variables for cursors, to hold record pointers
				cPubList = C_GOTOVAR1_CMMT_LOC
				FOR k = 1 TO ALEN(THIS.a_tables)
					cCursVar = THIS.MakeVar(THIS.a_tables[k])
					cPubList = m.cPubList + "PUBLIC " + m.cCursVar + C_CRLF
				NEXT
				REPLACE _FOX3SPR.SPRMEMO WITH ;
					m.cPubList + C_CRLF ADDITIVE
			ENDIF

			IF !EMPTY(THIS.cParms)
				cDoForm = [EXTERNAL PROC ] + JUSTFNAME(m.cScxName) + C_CRLF + C_CRLF + ;
						IIF(THIS.iPlatformCount > 1,C_TAB,"") + [DO FORM "] + JUSTFNAME(m.cScxName) + [" NAME ] + SYS(2015) + [ LINKED ] + CHR(38) + "_cparmstr" + ;
						IIF(THIS.noReadPlainExpr OR THIS.noReadExpr," NOREAD","") + ;
						IIF(THIS.lHasReturn," TO m._rval" + C_CRLF + "RETURN m._rval" + C_CRLF,C_CRLF)
   				IF THIS.iPlatformCount > 1
	 				*- only write out filename, not full path
					REPLACE _FOX3SPR.SPRMEMO WITH ;
						[IF _mac] + C_CRLF + ;
						C_TAB + [EXTERNAL PROC ] + JUSTSTEM(m.cScxName) + C_MACEXT + "." + JUSTEXT(m.cScxName) + C_CRLF + C_CRLF + ;
						C_TAB + [DO FORM "] + JUSTSTEM(m.cScxName) + C_MACEXT + "." + JUSTEXT(m.cScxName) + ;
							[" NAME ] + SYS(2015) + [ LINKED ] + CHR(38) + "_cparmstr" + ;
						IIF(THIS.noReadPlainExpr OR THIS.noReadExpr," NOREAD","") + ;
						IIF(THIS.lHasReturn," TO m._rval" + C_CRLF + "RETURN m._rval" + C_CRLF,C_CRLF) + ;
						[ELSE] + C_CRLF + ;
						C_TAB + m.cDoForm + ;
						[ENDIF] + C_CRLF ;
						ADDITIVE
				ELSE
					REPLACE _FOX3SPR.SPRMEMO WITH m.cDoForm ADDITIVE
				ENDIF
   			ELSE
   				cDoForm = [EXTERNAL PROC ] + JUSTFNAME(m.cScxName) + C_CRLF + C_CRLF + ;
						IIF(THIS.iPlatformCount > 1,C_TAB,"") + [DO FORM "] + JUSTFNAME(m.cScxName) + [" NAME ] + SYS(2015)+ [ LINKED ] + ;
						IIF(THIS.noReadPlainExpr OR THIS.noReadExpr," NOREAD","") + ;
						IIF(THIS.lHasReturn," TO m._rval" + C_CRLF + "RETURN m._rval" + C_CRLF,C_CRLF)
  				IF THIS.iPlatformCount > 1
	   				*- only write out filename, not full path
	 				REPLACE _FOX3SPR.SPRMEMO WITH ;
						[IF _mac] + C_CRLF + ;
						C_TAB + [EXTERNAL PROC ] + JUSTSTEM(m.cScxName) + C_MACEXT + "." + JUSTEXT(m.cScxName) + C_CRLF + C_CRLF + ;
						C_TAB + [DO FORM "] + JUSTSTEM(m.cScxName) + C_MACEXT + "." + JUSTEXT(m.cScxName) + [" NAME ] + SYS(2015)+ [ LINKED ] + ;
						IIF(THIS.noReadPlainExpr OR THIS.noReadExpr," NOREAD","") + ;
						IIF(THIS.lHasReturn," TO m._rval" + C_CRLF + "RETURN m._rval" + C_CRLF,C_CRLF) + ;
						[ELSE] + C_CRLF + ;
						C_TAB + m.cDoForm + ;
						[ENDIF] + C_CRLF ;
						ADDITIVE
				ELSE
					REPLACE _FOX3SPR.SPRMEMO WITH m.cDoForm ADDITIVE
				ENDIF
			ENDIF

			IF THIS.lHasDataNavObj AND THIS.a_pjxsets[A_OPENFILES]
				*- add code to release public variables we created above
				*cPubList = C_CRLF + C_GOTOVAR2_CMMT_LOC
				*FOR k = 1 TO ALEN(THIS.a_tables)
				*	cPubList = m.cPubList + "RELEASE " + THIS.MakeVar(THIS.a_tables[k]) + C_CRLF
				*NEXT
				*REPLACE _FOX3SPR.SPRMEMO WITH ;
					m.cPubList + C_CRLF ADDITIVE
			ENDIF

			IF !THIS.lMultiReads
				EXIT
			ENDIF
		ENDFOR
	ENDPROC		&&   AddParm

	*------------------
	PROCEDURE PostForm			&& SCXSingleScreenConverter
	*------------------
		*- this is an external hook to postprocess
		*- form object
		
		*- make sure count of DNO items is recorded in DataNavigation
		*- object header record (in the Reserved2 field)

		LOCAL m.savearea, m.tempfile, cTemp, j
		m.savearea = SELECT()

		FOR j = 1 TO IIF(THIS.lMultiReads,THIS.scxCount,1)
			SELECT (THIS.a_scx3alias[j])

			IF THIS.nDNORecNo > 0 AND THIS.nDNOCount > 0
				GO THIS.nDNORecNo
				REPLACE reserved2 WITH LTRIM(STR(THIS.nDNOCount)),methods WITH IIF(THIS.lHasIDX,_FOX3SPR.load,methods)
			ENDIF

			*- record count of objects for each form in the SCX file
			LOCATE FOR class == C_FORMCLASS

			DO WHILE !EOF()
				m.nFormRec1 = RECNO()											&& remember form record #
				SKIP															&& move past it
				SCAN REST WHILE class # C_FORMCLASS AND !EMPTY(class)			&& look for next form record, or fontinfo
				ENDSCAN
				m.nFormRec2 = RECNO()											&& remember that record #
				GO m.nformRec1													&& jump back to previous form record
				REPLACE reserved2 WITH LTRIM(STR(m.nFormRec2 - m.nformRec1))	&& store # of records
				GO MIN(RECC(),m.nFormRec2)										&& jump ahead to where we were...
				IF !EOF()														&& and move past it
					SKIP
				ENDIF
			ENDDO	&& going through file, tabulating #s of objects per form	&& repeat

			*- sort on uniqueid
			IF THIS.lHasInvis
				*- push all invisible buttons to end of file, so they will be created on top
				*- pictures, since pictures will absorb mouse clicks in FP 3.0

				*- create temporary file, and sort on uniqueID into it
				REPLACE ALL uniqueID WITH "~Z" FOR uniqueID = "FONTINFO"

				m.tempfile = ADDBS(JUSTPATH(THIS.a_scx3files[1])) + "S" + LEFT(SYS(3),7) +  "." + C_SCXEXT
				SORT ON uniqueID TO (m.tempfile)

				*- open the file, and change the unique ID for invisible buttons to a real uniqueID
				SELECT 0
				USE (m.tempfile)
				REPLACE ALL uniqueid WITH SYS(2015) FOR "~A" $ uniqueid  OR '^' $ uniqueid

				*- close the file, and replace the new SCX file with this modified one
				USE
				SELECT (THIS.a_scx3alias[j])
				m.cTemp = DBF(THIS.a_scx3alias[j])
				USE
				DELETE FILE (m.cTemp)
				DELETE FILE (FORCEEXT(m.cTemp,C_SCTEXT))
				RENAME (m.tempfile) TO (m.cTemp)
				RENAME (FORCEEXT(m.tempfile,C_SCTEXT)) TO (FORCEEXT(m.cTemp,C_SCTEXT))

				*- reopen it
				USE (m.cTemp)
			ENDIF

			*- and fix the FONTINFO record
			REPLACE ALL uniqueID WITH "FONTINFO" FOR "~Z" $ uniqueID
			REPLACE ALL uniqueID WITH SYS(2015) FOR "~" $ uniqueID

		NEXT
		SELECT (m.savearea)

	ENDPROC		&& SCXSingleScreenConverter:PostForm			

	*------------------------------------
	PROCEDURE CloseFiles			&& SCXSingleScreenConverter
	*------------------------------------
		PRIVATE i
		LOCAL cHFile, iCtr, cTmpFile

		IF USED("_FOX3SPR")
			SELECT (THIS.new30alias)
			GO TOP
			SELE _FOX3SPR
			IF !EMPTY(_FOX3SPR.defines)
				*- write out #DEFINEs to new #INCLUDE file
				cHFile = FORCEEXT(THIS.cStubFile,"h")
				iCtr = 1
				DO WHILE FILE(m.cHFile) AND m.iCtr <= 99
					m.cHFile = FORCEEXT(THIS.cStubFile,"h" + RIGHT(STR(iCtr + 100),2))
					m.iCtr = m.iCtr + 1
				ENDDO
				IF m.iCtr > 99
					*- ?? very unlikely? go ahead and use original file
					cHFile = FORCEEXT(THIS.cStubFile,"h")
				ENDIF
				REPLACE defines WITH C_CONV_CMMT_LOC + JustFName(m.cHFile) + C_CR + ;
					C_H_CMMT_LOC + JustFName(THIS.cCurrentFile) + C_CR + C_CR + defines
				COPY MEMO defines TO (m.cHFile)
				*- remember the name of the #INCLUDE file
				SELECT (THIS.new30alias)
				REPLACE reserved8 WITH JUSTFNAME(m.cHFile)
				*- add #INCLUDE to .SPR code
				REPLACE _fox3spr.sprmemo WITH C_INCLUDE_CMMT_LOC + ;
					"#INCLUDE " + JUSTFNAME(ALLT(reserved8)) + ;
					C_CRLF + C_CRLF + _fox3spr.sprmemo
				SELE _FOX3SPR
			ENDIF
			IF THIS.lDevMode
				IF !EMPTY(_fox3spr.sprmemo)
					REPLACE code WITH C_SEPARATOR + C_CODESRC_LOC + C_CRLF + _fox3spr.sprmemo ADDITIVE
				ENDIF
				IF !USED("_FOX3PJX")
					*- write out code (it _FOX3PJX is used, it will be written out below)
					COPY MEMO code TO (THIS.cCodeFile)
				ENDIF
			ELSE 
				COPY MEMO sprmemo TO (THIS.cStubFile)
			ENDIF
			*- remember SPR code, in case it needs to be spit out in the future
			SELECT (THIS.new30alias)
			REPLACE user WITH _fox3spr.sprmemo
			IF THIS.lDevMode AND THIS.projcall AND USED("_FOX3PJX")
				REPLACE _FOX3PJX.sprmemo WITH _FOX3SPR.code
			ENDIF

			USE IN _FOX3SPR
		ENDIF

		FOR i = 1 TO ALEN(THIS.a_scx2alias)
			IF USED(THIS.a_scx2alias[m.i])
				USE IN (THIS.a_scx2alias[m.i])
			ENDIF
		ENDFOR

		FOR m.i = 1 TO ALEN(THIS.a_scx3alias)
			IF USED(THIS.a_scx3alias[m.i])
				USE IN (THIS.a_scx3alias[m.i])
			ENDIF
		ENDFOR

		*- if lUserCall = .T., means called via project
		*- if lUserCall = .F., means screen opened individually (also used for catalogs)
		IF THIS.lUserCall
			*- only thing we need to do with Project here is to change the
			*- name of the Mac form if converting all platforms and Mac
			*- platform is present... (12/16/95 jd)
			IF THIS.platform = C_MAC AND THIS.iPlatformCount > 1
				m.cTmpFile = AddBS(JustPath(THIS.a_scx3files[1])) + ;
					JustStem(THIS.a_scx3files[1]) + C_MACEXT + "." + C_SCXEXT
				COPY FILE (THIS.a_scx3files[1]) TO (m.cTmpFile)
				COPY FILE (FORCEEXT(THIS.a_scx3files[1],C_SCTEXT)) TO (FORCEEXT(m.cTmpFile,C_SCTEXT))
			ENDIF	
		ELSE
			IF THIS.lHadError
				*- Delete temp files if we had an error
				IF FILE(THIS.a_scx3files[1])
					DELETE FILE (THIS.a_scx3files[1])
					DELETE FILE FORCEEXT(THIS.a_scx3files[1],C_SCTEXT)
				ENDIF
			ELSE
				IF THIS.lBackUp
					*- erase existing backup file if it;s there
					THIS.EraseBackUp

					*- Rename old screen with S2X,S2T extensions
					*- unless converting multiple platforms, and this one is Mac (need to leave original around 
					*-   for Windows conversion) so copy.
					IF THIS.iPlatformCount > 1 AND THIS.platform = C_MAC
						COPY FILE (THIS.a_scx2files[1]) TO (FORCEEXT(THIS.a_scx2files[1],C_SCXBACKEXT))
						COPY FILE (FORCEEXT(THIS.a_scx2files[1],C_SCTEXT)) TO (FORCEEXT(THIS.a_scx2files[1],C_SCTBACKEXT))
					ELSE
						RENAME (THIS.a_scx2files[1]) TO (FORCEEXT(THIS.a_scx2files[1],C_SCXBACKEXT))
						RENAME (FORCEEXT(THIS.a_scx2files[1],C_SCTEXT)) TO (FORCEEXT(THIS.a_scx2files[1],C_SCTBACKEXT))
					ENDIF
				ELSE
					*- don;t delete if multi-platform and Mac, since we might need this later
					IF !(THIS.platform = C_MAC AND THIS.iWhichPlat < THIS.iPlatformCount)
						DELETE FILE (THIS.a_scx2files[1])
						DELETE FILE (FORCEEXT(THIS.a_scx2files[1],C_SCTEXT))
					ENDIF
				ENDIF

				*- Rename new FP3 screen
				IF THIS.iPlatformCount > 1 AND THIS.platform = C_MAC
					*- converting more than one platform, and this one is Mac, so add _mac
					*- extension to the file name
					m.cTmpFile = (AddBS(JustPath(THIS.a_scx2files[1])) + ;
						JustStem(THIS.a_scx2files[1]) + C_MACEXT + "." + C_SCXEXT)
					IF FILE(m.cTmpFile)
						*- erase it first
						ERASE (m.cTmpFile)
					ENDIF
					RENAME (THIS.a_scx3files[1]) TO (m.cTmpFile)
					IF FILE(FORCEEXT(m.cTmpFile,C_SCTEXT))
						*- erase it first
						ERASE (FORCEEXT(m.cTmpFile,C_SCTEXT))
					ENDIF
					RENAME (FORCEEXT(THIS.a_scx3files[1],C_SCTEXT)) TO (FORCEEXT(m.cTmpFile,C_SCTEXT))
				ELSE
					RENAME (THIS.a_scx3files[1]) TO (THIS.a_scx2files[1])
					RENAME (FORCEEXT(THIS.a_scx3files[1],C_SCTEXT)) TO (FORCEEXT(THIS.a_scx2files[1],C_SCTEXT))
				ENDIF

				*- Compile form
				IF !THIS.lNoCompile
					IF THIS.iPlatformCount > 1 AND THIS.platform = C_MAC
						*- use proper name for Mac specific form
						*- cTmpFile is set just above...
						IF FILE(m.cTmpFile)
							COMPILE FORM (m.cTmpFile)
						ENDIF
					ELSE
						IF FILE(THIS.a_scx2files[1])
							COMPILE FORM (THIS.a_scx2files[1])
						ENDIF
					ENDIF
				ENDIF

			ENDIF

		ENDIF

	ENDPROC		&&   SCXSingleScreenConverter:CloseFiles

	*------------------------------------
	PROCEDURE Cleanup				&& SCXSingleScreenConverter
	*------------------------------------
		*- this proc is called by Error, and tries to put things back the way they were
		*- if cleaning up from a crashed project conversion, the pjx cleanup will
		*- handle the screens
		LOCAL i

		IF !THIS.lUserCall 
			*- screen opened individually
			*- Delete temp files if we had an error
			CLOSE TABLES
			IF FILE(THIS.a_scx3files[1])
				DELETE FILE (THIS.a_scx3files[1])
				DELETE FILE (FORCEEXT(THIS.a_scx3files[1],C_SCTEXT))
			ENDIF
			IF !THIS.lBackUp
				*- a backup could have already been made (e.g., a 2.0 file was being converted
				*- restore old screen from S2X,S2T extensions
				FOR i = 1 TO ALEN(THIS.a_scx2files,1)
					IF	FILE(THIS.a_scx2files[i,1]) AND ;
						FILE(FORCEEXT(THIS.a_scx2files[i,1],C_SCXBACKEXT)) AND ;
						FILE(FORCEEXT(THIS.a_scx2files[i,1],C_SCTEXT)) AND ;
						FILE((FORCEEXT(THIS.a_scx2files[i,1],C_SCTBACKEXT)))
						*- all of the files are there to attempt this, so...
						DELETE FILE (THIS.a_scx2files[i,1])
						DELETE FILE (FORCEEXT(THIS.a_scx2files[i],C_SCTEXT))
						IF !FILE(THIS.a_scx2files[i,1])
							RENAME (FORCEEXT(THIS.a_scx2files[i,1],C_SCXBACKEXT)) TO (THIS.a_scx2files[i,1])
						ENDIF
						IF !FILE(FORCEEXT(THIS.a_scx2files[i,1],C_SCTEXT))
							RENAME (FORCEEXT(THIS.a_scx2files[i,1],C_SCTBACKEXT)) TO (FORCEEXT(THIS.a_scx2files[i,1],C_SCTEXT))
						ENDIF
					ENDIF
					*- under certain circumstances, these may be left around
					IF FILE(FORCEEXT(THIS.a_scx2files[i,1],C_SCXBACKEXT))
						DELETE FILE (FORCEEXT(THIS.a_scx2files[i,1],C_SCXBACKEXT))
					ENDIF
					IF FILE(FORCEEXT(THIS.a_scx2files[i,1],C_SCTBACKEXT))
						DELETE FILE (FORCEEXT(THIS.a_scx2files[i,1],C_SCTBACKEXT))
					ENDIF
				NEXT
			ENDIF
		ENDIF

		THIS.oConvForm = .NULL.

	ENDPROC

	
	*---------------------------------------
	FUNCTION Set30Defaults			&& SCXSingleScreenConverter
	*---------------------------------------
		*- set properties to VFP 3.0 defaults
		*- the following properties need to be set:
		*-		FontBold (default was bold in 3.0)
		*-		FontSize (default was 10 in 3.0)
		*-		ColorSource (default was 0 in 3.0)
		*-			Pages, pageframes  are set to 0
		*-			Forms are set to 5 (per allisonk 6/20/96)
		*- if the CLASSLOC field is not empty, that means the object has
		*- a parent, so we skip it -- it will pick up its behavior from one of
		*- its ancestors

		PRIVATE llUpdated, cProp
		LOCAL	cLine, cText, iRecc

		iRecc = RECC()

		SCAN FOR EMPTY(classloc)

			gOTherm.Update(RECNO()/iRecc * 100,C_PROJTASK6_LOC)		&& update therm with next task

			llUpdated = .F.
			cProp = properties

			*- look for relevant properties in properties field
			*- if class is a container, look into the "contained" objects too
			IF INLIST(LOWER(baseclass), ;
				"form","checkbox","combobox","commandbutton",;
				"editbox","grid","header","label","listbox","page",;
				"spinner","textbox")
				IF !INLIST(LOWER(baseclass),"pageframe","formset")
					*- check for, add FontBold and FontSize
					THIS.Add40Property("FontBold",".T.","")
					THIS.Add40Property("FontSize","10","")
				ENDIF
			ENDIF
			
			IF INLIST(LOWER(baseclass), ;
				"checkbox","combobox","commandbutton",;
				"editbox","label","listbox","spinner",;
				"shape","textbox","pageframe")
				*- check for, add ColorSource
				THIS.Add40Property("ColorSource","0","")
			ENDIF

			IF INLIST(LOWER(baseclass), ;
				"commandgroup","optiongroup","grid","pageframe")
				*- these are containers -- they will have sub-items (textboxes, 
				*- optionbuttons, pages etc.) that will need to be checked
				cText = m.cProp
				FOR i = 1 TO MEMLINES(m.cProp)
					cLine = MLINE(m.cText, ATCLINE(".name = ", m.cText))
					cObject = LEFT(cLine, AT(".",cLine))
					IF LOWER(baseclass) # "pageframe"
						THIS.Add40Property("FontBold",".T.",cObject)
						THIS.Add40Property("FontSize","10",cObject)
					ENDIF
					IF LOWER(baseclass) # "grid"
						THIS.Add40Property("ColorSource","0",cObject)
					ENDIF
					cText = SUBS(cText,ATC(".name = ",cText) + 8)
				NEXT
			ENDIF
			
			IF TRIM(LOWER(baseclass)) == "form"
				*- check for, add ColorSource
				THIS.Add40Property("ColorSource","5","")
			ENDIF

			IF m.llUpdated
				REPLACE properties WITH m.cProp
			ENDIF

		ENDSCAN

		RETURN .T.

	ENDFUNC
	
	*---------------------------------------
	FUNCTION Add40Property			&& SCXSingleScreenConverter
	*---------------------------------------
		PARAMETER lcProperty, lcValue, lcObject

		LOCAL iPos, lUseLF
		IF ATC(lcObject + lcProperty, m.cProp) == 0
			*- property is missing
			iPos = ATC(C_CRLF + m.lcObject + "name =",m.cProp)
			IF iPos == 0
				lUseLF = .F.
				iPos = ATC(C_CR + m.lcObject + "name =",m.cProp)
			ELSE
				lUseLF = .T.
			ENDIF
			IF iPos # 0
				m.cProp = LEFT(m.cProp,iPos - 1) + ;
					IIF(m.lUseLF,C_CRLF,C_CR) + m.lcObject + lcProperty + " = " + lcValue + ;
					SUBS(m.cProp,iPos)
				llUpdated = .T.
			ENDIF
		ENDIF
	ENDFUNC


	*---------------------------------------
	PROCEDURE AddFontSub			&& SCXSingleScreenConverter
	*---------------------------------------
		PRIVATE fontstr
		m.fontstr=ALLT(a_scx2fld[A_FONTFACE])+", "+ ;
			ALLT(STR(a_scx2fld[A_FONTSTYLE]))+", "+ ;
			ALLT(STR(a_scx2fld[A_FONTSIZE]))+", "+ ;
			ALLT(STR(a_scx2fld[A_HPOS]))+", "+ ;
			ALLT(STR(a_scx2fld[A_VPOS]))+", "+ ;
			ALLT(STR(a_scx2fld[A_HEIGHT]))+", "+ ;
			ALLT(STR(a_scx2fld[A_WIDTH]))+", "+ ;
			ALLT(STR(a_scx2fld[A_PENRED]))+", "+ ;
			ALLT(STR(a_scx2fld[A_PENGREEN]))+C_CRLF
		IF AT(m.fontstr,THIS.fontsub)=0
			THIS.fontsub = THIS.fontsub+m.fontstr
		ENDIF
	ENDPROC		&&   AddFontSub
	
	*---------------------------------------
	PROCEDURE WriteFontSub			&& SCXSingleScreenConverter
	*---------------------------------------
	  	INSERT INTO (THIS.new30alias) ;
		  (platform,uniqueid,properties) ;
		  VALUES ("COMMENT","~Z",THIS.fontsub)
		THIS.fontsub = ""	&&reset
	ENDPROC		&&   WriteFontSub
	
	*------------------------------------
	PROCEDURE AddFSet			&& SCXSingleScreenConverter
	*------------------------------------
		*-  Add the formset record here. We can use
		*-  "Formset1" name here since no 2.5 equivalent.

		PARAMETER getplat
		PRIVATE m.z,m.tmpstr,m.tmpcnt,m.j,lReadclause
		lReadclause = .F.

		THIS.curplat = m.getplat

		*- construct the formRef name
		THIS.iFormSetCtr = THIS.iFormSetCtr + 1
		THIS.cFormSetName = THIS.GetVarPrefix(T_FSET) + PROPER(LOWER(GoodName(JUSTSTEM(THIS.cCurrentFile)))) + LTRIM(STR(THIS.iFormSetCtr))
	  
		*- reset arrays and all
		STORE "" TO THIS.a_reads
		THIS.nObjCount = 0
		THIS.fp3prop = ""		&& properties
		THIS.fp3method = ""		&& methods

		*- Add formset properties
	  
		*- Add READ type here: Read - 2, Read Modal - 3
		IF THIS.lDevMode
			THIS.AddProp(M_READ,IIF(THIS.a_pjxsets[A_READMODAL],1,0))	  
		ELSE
			THIS.AddProp(M_READ,IIF(THIS.a_pjxsets[A_READMODAL],3,2))	  
		ENDIF

		*- Add READ CYCLE
  		THIS.AddProp(M_READCYCLE,THIS.a_pjxsets[A_READCYCLE])	  
	  
		*- Add READ NOLOCK
  		THIS.AddProp(M_READNOLOCK,!THIS.a_pjxsets[A_READNOLOCK])	  

		*- GET Borders
  		THIS.GetBorder = THIS.a_pjxsets[A_GetBorderS]

		*- Associated Windows
		IF !EMPTY(THIS.a_pjxsets[A_ASSOCWINDS])
			m.tmpstr = STRTRAN(THIS.a_pjxsets[A_ASSOCWINDS],CHR(13),',')
			THIS.AddProp(M_ASSOCWINDS,LEFT(m.tmpstr,LEN(m.tmpstr)-1))	  
		ENDIF

		*- Accumulate READ stuff
		FOR m.j = 1 TO THIS.scxcount

			*- Select screen to process in screen set
			SELECT (THIS.a_scx2alias[m.j])

			*- already converted, so skip
			IF FCOUNT() = C_30SCXFLDS AND FIELD(4) = "CLASS"
				*- already converted, so skip this
				LOOP
			ENDIF

			LOCATE FOR PLATFORM = THIS.curplat AND OBJTYPE = 1
			THIS.AddReads(m.j)
			
			*- RELEASE WINDOWS -- must be called after AddReads
			IF m.j = 1
		  		THIS.AddProp(M_RELEASEWIND,THIS.a_pjxsets[A_RELWINDOWS])
			ENDIF

			*- Handle Multiple READ option
			IF THIS.lMultiReads
				IF !EMPTY(THIS.read_expr)
  					THIS.ReadClause
				ENDIF

				*- Add Read snippets to Methods clause
				THIS.WriteReads
				
				*- Add formset name
				THIS.AddProp(M_NAME,THIS.cFormSetName)	&& formset name

				IF THIS.lDevMode
					INSERT INTO (THIS.a_scx3alias[m.j]) ;
					  (platform,uniqueid,timestamp,;
					  class,baseclass,objname,properties) ;
					  VALUES (THIS.savedPlat,sys(2015),THIS.nTimeStamp,;
					  T_FSET,T_FSET,THIS.cFormSetName,THIS.fp3prop)
					IF !EMPTY(THIS.fp3method)
						REPLACE _fox3spr.code WITH C_SEPARATOR + THIS.cFormSetName + C_CRLF + THIS.fp3method ADDITIVE
					ENDIF
				ELSE
					INSERT INTO (THIS.a_scx3alias[m.j]) ;
					  (platform,uniqueid,timestamp,;
					  class,baseclass,objname,properties,methods) ;
					  VALUES (THIS.savedPlat,sys(2015),THIS.nTimeStamp,;
					  T_FSET,T_FSET,THIS.cFormSetName,THIS.fp3prop,THIS.fp3method)
				ENDIF

				IF m.j = 1
					THIS.nFSetRecno = RECNO(THIS.a_scx3alias[1])
				ENDIF

				STORE "" TO THIS.a_reads
				THIS.fp3method = ""			&& methods
			ELSE
				*- Check for #READCLAUSE
				IF !EMPTY(THIS.read_expr) AND !m.lReadclause 
					THIS.ReadClause
					m.lReadclause= .T.
				ENDIF
			ENDIF
		ENDFOR

		THIS.new30alias = THIS.a_scx3alias[1]

		IF !THIS.lMultiReads
			*- Add Read snippets to Methods clause
			THIS.WriteReads

			*- Add formset name
			THIS.AddProp(M_NAME,THIS.cFormSetName)	&& default name

			IF THIS.lDevMode
				*- put methods someplace else
				INSERT INTO (THIS.new30alias) ;
				  (platform,uniqueid,timestamp,;
				  class,baseclass,objname,properties,reserved4) ;
				  VALUES (THIS.savedPlat,sys(2015),THIS.nTimeStamp,;
				  T_FSET,T_FSET,THIS.cFormSetName,THIS.fp3prop,;
				  IIF(!THIS.a_pjxsets[A_DEFWINDOWS],"NODEFINE",""))
				IF !EMPTY(THIS.fp3method)
					REPLACE _fox3spr.code WITH C_SEPARATOR + THIS.cFormSetName + C_CRLF + THIS.fp3method ADDITIVE
				ENDIF
			ELSE
				INSERT INTO (THIS.new30alias) ;
				  (platform,uniqueid,timestamp,;
				  class,baseclass,objname,properties,methods,reserved4) ;
				  VALUES (THIS.savedPlat,sys(2015),THIS.nTimeStamp,;
				  T_FSET,T_FSET,THIS.cFormSetName,THIS.fp3prop,THIS.fp3method,;
				  IIF(!THIS.a_pjxsets[A_DEFWINDOWS],"NODEFINE",""))
			ENDIF

			THIS.nFSetRecno = RECNO(THIS.a_scx3alias[1])

		ENDIF

		THIS.parentName = THIS.cFormSetName
	
		*- Select 1st screen
		SELECT (THIS.c25alias)
		
	ENDPROC		&& AddFSet


	*------------------------------------------------
	PROCEDURE ReadClause			&& SCXSingleScreenConverter
	*------------------------------------------------
	*- ReadClause -- process if #READ
	PRIVATE tmpstr,tmpcnt 

	*- Add READ MOUSE
	IF ATC("NOMOUSE",THIS.read_expr) # 0
		THIS.AddProp(M_READNOMOUSE,C_TRUE)	  
	ENDIF

	*- Add READ SAVE
	IF ATC("SAVE",THIS.read_expr) # 0
		THIS.AddProp(M_READSAVE,C_TRUE)	  
	ENDIF

	*- Add READ TIMEOUT
	IF ATC("TIME",THIS.read_expr) # 0
		m.tmpstr = SUBSTR(THIS.read_expr,ATC("TIME",THIS.read_expr))
		THIS.AddProp(M_READTIME,VAL(SUBSTR(m.tmpstr ,ATC(" ",m.tmpstr))) * K_TIMEOUT_FACTOR)  
	ENDIF
  
	*- Add READ OBJECT
	IF ATC("OBJECT",THIS.read_expr) # 0
		m.tmpstr = SUBSTR(THIS.read_expr,ATC("OBJECT",THIS.read_expr))
		THIS.AddProp(M_READOBJ,VAL(SUBSTR(m.tmpstr ,ATC(" ",m.tmpstr))))  
	ENDIF

	*- Color Scheme
	IF ATC("COLORSCHEME",THIS.read_expr) # 0
		m.tmpcnt = 1
		DO WHILE UPPER(WORDNUM(THIS.read_expr,m.tmpcnt))#"COLORSCHEME"
			m.tmpcnt = m.tmpcnt+1
		ENDDO
		m.tmpstr = WORDNUM(THIS.read_expr,m.tmpcnt+1)
		THIS.AddProp(M_SCHEME,m.tmpstr)
	ENDIF

	ENDPROC		&& SCXSingleScreenConverter:ReadClause
	
	*------------------------------------------------
	PROCEDURE WriteReads			&& SCXSingleScreenConverter
	*------------------------------------------------
		*- This routine writes the Methods field to the
		*- new 3.0 SCX file for the Formset record at
		*- the end since it combines the snippets of
		*- 2.5 screens from screen sets. 
		
		*- if #NOREAD PLAIN, toss any snippets
		DO CASE
			CASE THIS.noReadExpr
				THIS.AddMethods(M_WHEN,THIS.a_reads[4],1)
				THIS.AddMethods(M_VALID,THIS.a_reads[5],1)
				THIS.AddMethods(M_ACTIVATE,THIS.a_reads[7],1)
				THIS.AddMethods(M_DEACTIVATE,THIS.a_reads[8],1)
			CASE !THIS.noReadPlainExpr
				THIS.AddMethods(M_WHEN,THIS.a_reads[4],1)
				THIS.AddMethods(M_VALID,THIS.a_reads[5],1)
				THIS.AddMethods(M_ACTIVATE,THIS.a_reads[7],1)
				THIS.AddMethods(M_DEACTIVATE,THIS.a_reads[8],1)
			OTHERWISE
				THIS.cProcs = THIS.cProcs + THIS.a_reads[3]
		ENDCASE
		
	ENDPROC		&& WriteReads

	*------------------------------------
	PROCEDURE AddReads			&& SCXSingleScreenConverter
	*------------------------------------
		PARAMETER scrn_num
		*- This routine updates the Methods field in the
		*- new 3.0 SCX file. Since 2.5 screen sets combine
		*- these, we will combine here as well and update
		*- the formset record at the end.

		PRIVATE m.part1,m.part2,m.part2line,m.part2chr
		PRIVATE m.sect1,m.sect2,m.sect1line,m.sect2line,m.parmline 
		PRIVATE m.sect1chr,m.sect2chr,m.sect2start
		PRIVATE m.gendir,m.gendircount,savearea
		PRIVATE m.j
		LOCAL m.ctemptxt, ngendir
		LOCAL cDefine, cTmp, nCtr, cReplaceStr, cGenAction, cParmLine, nCtr, nLine

		STORE "" TO m.part1,m.part2,m.sect1,m.sect2	
		STORE 0 TO m.part2line,m.part2chr
		STORE 0 TO m.sect1line,m.sect2line,m.parmline
		STORE 0 TO m.sect1chr,m.sect2chr,m.sect2start
		STORE 1 TO m.gendircount
		STORE SELECT() TO m.savearea
		
		*- Get parameter statement here -- assume we always
		*- hit here at least once
		THIS.cParms = GetParam("setupcode")

		*- Preprocess here for screen directives
		*- Also remove (comment out) others
		
		REPLACE _FOX3SPR.temp1 WITH setupcode

		SELECT _FOX3SPR
		*- go ahead and strip out leading white space here, so MemoFind doesn;t need to do it each time
		*- temp4 will be a "cleaned-up" version of temp1, to aid in finding the right directives
		REPLACE temp4 WITH CleanWhite(temp1)

		*- alter references to SYS(16), because they will now be in a procedure
		IF "SYS(16" $ UPPER(_FOX3SPR.temp1)
			REPLACE _FOX3SPR.temp1 WITH STRTRAN(_FOX3SPR.temp1,"SYS(16","_SYS(16")
			REPLACE _FOX3SPR.temp1 WITH STRTRAN(_FOX3SPR.temp1,"sys(16","_sys(16")
			REPLACE _FOX3SPR.temp1 WITH STRTRAN(_FOX3SPR.temp1,"Sys(16","_Sys(16")
			THIS.lhasSys16 = .T.
		ENDIF

		DO WHILE .T.
			m.gendir = MemoFind("#","temp4",.T.,.F.,m.gendircount,.T.,.T.)
			DO CASE
				CASE m.gendir = C_NULL
					EXIT
				CASE UPPER(LEFT(m.gendir,4)) = "DEFI"
					*- accumulate
					IF RIGHT(TRIM(m.gendir),1) = ";"
						*- a continued #DEFINE, so take some special measures
						m.cDefine = ""
						m.ngendir = MemoFind("#","temp4",.T.,.T.,m.gendircount,.T.,.T.)
						IF m.ngendir = 0
							*- this should be an impossibility -- couldn;t find where it was
							EXIT
						ENDIF
						gendir = MLINE(temp1,m.ngendir)
						*- check for continued #DEFINES
						m.nCtr = 1
						DO WHILE .T.
							m.cDefine = m.cDefine + m.gendir + C_CRLF
							IF RIGHT(m.gendir,1) # ";"
								EXIT
							ELSE
								m.gendir = MLINE(temp1,m.ngendir + m.nCtr)
								m.nCtr = m.nCtr + 1
							ENDIF
						ENDDO
						REPLACE _FOX3SPR.defines WITH m.cDefine ADDITIVE
					ELSE
						REPLACE _FOX3SPR.defines WITH ;
						  "#" + m.gendir + C_CRLF ADDITIVE
					ENDIF
				CASE UPPER(LEFT(m.gendir,2)) = "IF" ;
				  OR UPPER(LEFT(m.gendir,4)) = "ELSE" ;
				  OR UPPER(LEFT(m.gendir,4)) = "ELIF" ;
				  OR UPPER(LEFT(m.gendir,5)) = "ENDIF"
				  *- should accumulate these in with the defines, plus leave them in the methods
					REPLACE _FOX3SPR.defines WITH ;
					  "#" + m.gendir + C_CRLF ADDITIVE
					m.gendircount = m.gendircount + 1
					LOOP
				CASE UPPER(LEFT(m.gendir,4)) = "INSE" ;
				  OR UPPER(LEFT(m.gendir,4)) = "SECT"
					m.gendircount = m.gendircount + 1
					LOOP
				CASE UPPER(LEFT(m.gendir,4)) = "ITSE"
					THIS.itse_expr = UPPER(MemoFind("#ITSE","temp4",.T.,.F.))
				CASE UPPER(LEFT(m.gendir,4)) = "READ"
					THIS.read_expr = m.gendir
				CASE UPPER(LEFT(m.gendir,7)) = "WCLAUSE"
					THIS.wclause_expr = m.gendir
				CASE UPPER(LEFT(m.gendir,5)) = "WNAME"
					THIS.cWnameExpr = UPPER(MemoFind("#WNAME","temp4",.T.,.F.))
				CASE UPPER(LEFT(m.gendir,6)) = "NOREAD" AND ATC("PLAIN",m.gendir) # 0
					THIS.a_pjxsets[A_DEFWINDOWS] = .F.
					THIS.a_pjxsets[A_RELWINDOWS] = .F.
					THIS.noReadPlainExpr = .T.
				CASE UPPER(LEFT(m.gendir,6)) = "NOREAD"
					THIS.a_pjxsets[A_RELWINDOWS] = .F.
					THIS.noReadExpr = .T.
			ENDCASE
			*- Comment out directive
			m.cGenAction = IIF(" " $ ALLT(m.gendir), ;
				SUBS(m.gendir, AT(" ",LTRIM(m.gendir)) + 1),"")
			IF LEFT(LTRIM(m.cGenAction),1) = "&" AND ;
				IIF(LEN(m.cGenAction) > 1,SUBS(m.cGenAction,2,1)," ") # "&"
				*- a macro expression -- can;t handle it
				*- log it, and comment it in file 
				THIS.WriteLog(THIS.cCurrentFile,E_MACROEXPR1_LOC)
				m.cReplaceStr = C_MACRO_CMMT_LOC + '*'
			ELSE
				m.cReplaceStr = '*'
			ENDIF
			*- Comment out directive
			_MLINE = 0
			nLine = MemoFind("#","temp4",.T.,.T.,m.gendircount,.F.,.T.)
			cTmp = MLINE(temp1,nLine - 1)						&& set _MLINE
			REPLACE temp1 WITH STUFF(temp1,_MLINE + IIF(nLine <= 1,0,2),0,m.cReplaceStr)
			cTmp = MLINE(temp4,nLine - 1)						&& set _MLINE
			REPLACE temp4 WITH STUFF(temp4,_MLINE + IIF(nLine <= 1,0,1),0,m.cReplaceStr)
		ENDDO
		
		*- Get SETUP snippet SECTION 1 location
		*- If parameter statement, then we must start at 1st
		*- line following. Note: only 1st screen parameter
		*- statement is used.
		m.sect1line =  GetFirstLine("temp1","SECT","1")
		m.sect1chr = _MLINE + 2 && add CRLF
		IF sect1line # 0
			m.parmline = GetFirstLine("temp1","PARM")
			IF m.parmline # 0	&& has parameter
				*- check for continued parameter line
				m.nCtr = 0
				DO WHILE .T.
					cParmLine = MLINE(temp1,parmline + m.nCtr)
					IF RIGHT(m.cParmLine,1) # ";"
						EXIT
					ELSE
						m.nCtr = m.nCtr + 1
					ENDIF
				ENDDO
				m.sect1chr = _MLINE + m.nCtr + 1
			ENDIF
		ENDIF
		
		*- Get SETUP snippet SECTION 2 location
		m.sect2line =  GetFirstLine("temp1","SECT","2")
		IF m.sect2line # 0
			m.sect2chr = _MLINE + 2 && add CRLF
			m.sect2start = m.sect2chr-LEN(MLINE(temp1,m.sect2line))-2
		ENDIF
		
		DO CASE
			CASE m.sect1line = 0 AND m.sect2line = 0 AND !EMPTY(temp1)
				*- No #SECTION directives
				m.sect2 = temp1
			CASE m.sect1line # 0 AND m.sect2line = 0 AND m.parmline # 0
				*- Only #SECTION1 directive, with parameter
				m.sect1 = SUBSTR(temp1,m.sect1chr)
			CASE m.sect1line # 0 AND m.sect2line = 0
				*- Only #SECTION1 directive, without parameter
				m.sect1 = SUBSTR(temp1,m.sect1chr)
			OTHERWISE
				*- Has both #SECTION1 + #SECTION2 directives
				m.sect1 = SUBSTR(temp1,m.sect1chr,sect2start-m.sect1chr)
				m.sect2 = SUBSTR(temp1,m.sect2chr)
		ENDCASE

		SELECT (m.savearea)

		*- Get CLEANUP snippet stuff
		*- Check for duplicate procs in platforms
		m.part2line =  GetFirstLine("PROCCODE","PROC")
		IF m.part2line = 0	&& no procedures
			m.part1 = ALLTRIM(proccode)
		ELSE
			m.part2chr = _MLINE - LEN(MLINE(PROCCODE,m.part2line)) - 2
			m.part1 = SUBSTR(proccode,1,m.part2chr)
		ENDIF

		IF !EMPTY(m.part1)
			*- alter references to SYS(16), because they will now be in a procedure
			IF "SYS(16" $ UPPER(m.part1)
				m.part1 = STRTRAN(m.part1,"SYS(16","_SYS(16")
				m.part1 = STRTRAN(m.part1,"sys(16","_sys(16")
				m.part1 = STRTRAN(m.part1,"Sys(16","_Sys(16")
				THIS.lhasSys16 = .T.
			ENDIF

			*- See if Cleanup code is returning a value
			REPLACE _FOX3SPR.temp1 WITH m.part1
			SELECT _FOX3SPR
			REPLACE temp4 WITH CleanWhite(temp1)

			m.nmaxretu = OCCURS('RETU',m.part1)
			FOR m.j = 1 TO m.nmaxretu
				m.ctemptxt = UPPER(MemoFind("RETU","temp4",.F.,.F.,m.j,.T.,.T.))
				IF !EMPTY(ALLT(m.ctemptxt))
					*- There's a return statement in there -- does anything follow it?
					IF	' ' $ ALLT(m.ctemptxt) AND ;
						(UPPER(LEFT(m.ctemptxt,1)) = "R" OR ;
						UPPER(LEFT(m.ctemptxt,2)) = "RN")
						m.ctemptxt = SUBS(m.ctemptxt,AT(' ',m.ctemptxt) + 1)
					ENDIF
					*- make sure it's not just a double-ampersand comment
					IF !EMPTY(ALLT(m.ctemptxt)) AND LEFT(ALLT(m.ctemptxt),2) != '&' + '&'
						THIS.lHasReturn = .T.
						EXIT
					ENDIF
				ENDIF
			NEXT

		ENDIF

		SELECT (m.savearea)

		THIS.a_reads[1] = THIS.MergeMethods(THIS.a_reads[1],m.sect1,1,m.scrn_num)
		IF !EMPTY(THIS.cParms) AND (m.scrn_num = 1 OR THIS.lMultiReads) 
			THIS.a_reads[1] = "PARAMETERS " + THIS.cParms + C_CRLF + C_CRLF + THIS.a_reads[1] 
		ENDIF
		THIS.a_reads[2] = THIS.MergeMethods(THIS.a_reads[2],m.sect2,1,m.scrn_num)

		IF !THIS.noReadPlainExpr AND !THIS.noReadExpr AND EMPTY(THIS.a_reads[3])
			THIS.a_reads[3] = IIF(THIS.lDevMode,"",C_CLEANUP_CODE)
		ENDIF

		THIS.a_reads[3] = THIS.MergeMethods(THIS.a_reads[3],m.part1,1,m.scrn_num)
		THIS.a_reads[4] = THIS.MergeMethods(THIS.a_reads[4],when,whentype,m.scrn_num)
		THIS.a_reads[5] = THIS.MergeMethods(THIS.a_reads[5],valid,validtype,m.scrn_num)
		THIS.a_reads[6] = THIS.MergeMethods(THIS.a_reads[6],show,showtype,m.scrn_num)
		THIS.a_reads[7] = THIS.MergeMethods(THIS.a_reads[7],activate,activtype,m.scrn_num)
		THIS.a_reads[8] = THIS.MergeMethods(THIS.a_reads[8],deactivate,deacttype,m.scrn_num)
	ENDPROC		&&  SCXSingleScreenConverter:AddReads

	*--------------------------------------
	FUNCTION MergeMethods			&& SCXSingleScreenConverter
	*--------------------------------------
	PARAMETER mergeCode,fp30fld,codetype,snum
	PRIVATE m.insfile, savearea

		m.savearea = SELECT()
		*- codetype = 0 -> Expression
		*- codetype = 1 -> Proc
		IF EMPTY(ALLT(m.fp30fld))
		  RETURN m.mergeCode
		ENDIF
		
		IF m.codetype = 0		&& expression
			m.fp30fld = "RETURN "+ m.fp30fld 
		ELSE					&& procedure
			*- check for #INSERT directives
			REPLACE _FOX3SPR.temp1 WITH m.fp30fld
			SELECT _FOX3SPR
			REPLACE _FOX3SPR.temp4 WITH CleanWhite(_FOX3SPR.temp1)

			*- move any other #DEFINEs over
			THIS.GetDefine("temp1")

			THIS.FindArry

			DO WHILE .T.
				m.insfile = MemoFind("#INSE","temp4",.T.,.F.,.F.,.F.,.T.)
				IF m.insfile = C_NULL
					EXIT
				ENDIF

				IF TYPE('m.insfile') = 'C' AND !FILE(m.insfile)
					*- look for insert file in same dir as file we;re converting
					IF FILE(AddBS(JustPath(THIS.cCurrentFile)) + JustFName(m.insfile))
						m.insfile = AddBS(JustPath(THIS.cCurrentFile)) + JustFName(m.insfile)
					ENDIF
				ENDIF

				*- check for bad file
				DO CASE
					CASE TYPE('m.insfile') # 'C'
						REPLACE temp2 WITH C_INSMESS3_LOC
					CASE EMPTY(m.insfile) OR !FILE(m.insfile) 
						REPLACE temp2 WITH C_INSMESS3_LOC + " - " + m.insfile
					OTHERWISE	&& all is OK
						*- add file to temporary memo
						APPEND MEMO temp2 FROM (m.insfile) OVERWRITE

						*- go through #INSERT file, and if any #DEFINEs, move them
						*- to the .h file
						REPLACE temp2 WITH CleanWhite(temp2)
						THIS.GetDefine("temp2")

						REPLACE temp2 WITH C_INSMESS1_LOC + m.insfile + C_CR + ;
							temp2 + C_CR + C_INSMESS2_LOC + C_CR
				ENDCASE
			  
				*- replace directive with specified file
				=MemoStuff("#INSE","temp1",temp2,.T.)
				=MemoStuff("#INSE","temp4",temp2,.T.)

			ENDDO

			m.fp30fld = temp1	&& set the contents to this field
			SELECT (m.savearea)
		ENDIF
		*- m.fp30fld = "#REGION " + ALLT(STR(m.snum)) + C_CRLF + m.fp30fld
		*- Merge method code
		RETURN IIF(EMPTY(m.MergeCode),m.fp30fld,m.MergeCode + C_CRLF + m.fp30fld)
	ENDFUNC		&&  MergeMethods

	*------------------------------------
    PROCEDURE GetDefine			&& SCXSingleScreenConverter
	*------------------------------------
	*- grab the #DEFINEs, and move to the _Fox3SPR.Defines field
	*- assume cMemoFld (name of memo) is already cleaned of leading spaces/tabs
	PARAMETER cMemoFld

		LOCAL gendircount, ngendir, cDefine, gendir, nCtr, cTmp, nLine

		IF ('#' $ &cMemoFld)
			STORE 1 TO m.gendircount
			DO WHILE .T.
				m.ngendir = MemoFind("#",cMemoFld,.T.,.T.,m.gendircount,.T.,.T.)
				IF m.ngendir = 0
					EXIT
				ENDIF
				*-m.gendir = MLINE(&cMemoFld,1,m.ngendir - 1)
				m.gendir = ALLT(STRTRAN(MLINE(&cMemoFld,m.ngendir),CHR(9),' '))
				m.cDefine = ""
				DO CASE
					CASE UPPER(LEFT(m.gendir,5)) = "#DEFI"
						*- check for continued #DEFINES
						m.nCtr = 1
						DO WHILE .T.
							m.cDefine = m.cDefine + C_CRLF + m.gendir
							IF RIGHT(m.gendir,1) # ";"
								EXIT
							ELSE
								m.gendir = MLINE(&cMemoFld,m.ngendir + m.nCtr)
								m.nCtr = m.nCtr + 1
							ENDIF
						ENDDO
						*- accumulate
						REPLACE _FOX3SPR.defines WITH m.cDefine ADDITIVE
					OTHERWISE
						m.gendircount = m.gendircount + 1
						LOOP
				ENDCASE
				*- Comment out directive
				_MLINE = 0
				nLine = MemoFind("#",cMemoFld,.T.,.T.,m.gendircount,.F.,.T.)
				cTmp = MLINE(&cMemoFld,m.nLine - 1)						&& set _MLINE
				REPLACE (cMemoFld) WITH STUFF(&cMemoFld,_MLINE + IIF(m.nLine <= 1,0,1),0,'*')
			ENDDO
		ENDIF
	ENDPROC		&& GetDefine

	*------------------------------------
    PROCEDURE ScanPlat			&& SCXSingleScreenConverter
	*------------------------------------
		PARAMETER cplatonly
    	*- get all platforms for all screens in set

    	PRIVATE tmparr,tmp2arr,i,j,tmpcnt,totrecs

    	m.tmpcnt = 0
    	m.totrecs = 0

    	DIMENSION tmparr[1]
    	tmparr = ""
    	
    	*- get total records to process for thermometer
		FOR m.i = 1 TO THIS.scxcount
			SELECT (THIS.a_scx2alias[m.i])

			IF FCOUNT() = C_30SCXFLDS AND FIELD(4) = "CLASS"
				*- already converted, so skip this
				LOOP
			ENDIF

			IF m.cplatonly # C_All
			  COUNT TO m.tmpcnt ;
		  		FOR !INLIST(objtype,10,20,23) ;
		  		AND platform = m.cplatonly
		  	ELSE
			  COUNT TO m.tmpcnt ;
		  		FOR !INLIST(objtype,10,20,23) ;
		  		AND INLIST(platform,C_DOS,C_WINDOWS,C_MAC,C_UNIX)
			ENDIF
			m.totrecs= m.totrecs + m.tmpcnt
		ENDFOR

		SELECT (THIS.c25alias)
    	
    	FOR i = 1 TO THIS.SCXCOUNT
			IF FCOUNT() = C_30SCXFLDS AND FIELD(4) = "CLASS"
				*- already converted, so skip this
				LOOP
			ENDIF

      		IF m.cplatonly # C_ALL
    		  DIMENSION tmparr[1]
    		  tmparr[1] = m.cplatonly
    		  EXIT
    		ENDIF
  			SELECT DISTINCT platform;
    			FROM DBF(THIS.a_scx2alias[m.i]);
    	 		WHERE INLIST(platform,C_DOS,C_WINDOWS,C_MAC,C_UNIX) ;
    	  		INTO ARRAY tmp2arr
    	  	
    		IF EMPTY(tmparr)
    			=ACOPY(tmp2arr,tmparr)
    		ELSE
    			FOR j = 1 TO ALEN(tmp2arr)
    				IF ASCAN(tmparr,ALLT(tmp2arr[m.j])) = 0
    					DIMENSION tmparr[ALEN(tmparr)+1]
    					tmparr[ALEN(tmparr)]=ALLT(tmp2arr[m.j])
    				ENDIF
    			ENDFOR
    		ENDIF
    		IF ALEN(tmparr) = C_MAXPLATFORMS
    			EXIT
    		ENDIF
    	ENDFOR
    	
		*- This is for GENSCRN stuff in conprocs
		=ACOPY(tmparr,g_platforms)
		=ACOPY(tmparr,THIS.a_plat)

		THIS.isMultiPlat = (ALEN(tmparr) > 1)

		RETURN m.totrecs		
    ENDPROC		&&  ScanPlat

	*----------------------
	PROCEDURE AddProcs1				&& SCXSingleScreenConverter
	*----------------------
		PRIVATE m.saverec,m.part2line,m.part2,m.part2chr
		m.saverec = IIF(EOF(),1,RECNO())

     	LOCATE FOR objtype = 1 AND platform = THIS.a_plat[1]
		m.part2line =  GetFirstLine("PROCCODE","PROC")

		IF m.part2line = 0	&& no procedures
			GO m.saverec
			RETURN
		ELSE
			*- If it's the first line, don't subtract anything
			m.part2chr = _MLINE - LEN(MLINE(proccode,m.part2line)) - IIF(m.part2line > 1,2,0)
			m.part2 = C_CRLF + SUBSTR(proccode,m.part2chr)
		ENDIF

		 THIS.cProcs = THIS.cProcs + m.part2 + C_CRLF

	ENDPROC		&& AddProcs1

	*----------------------
	PROCEDURE AddGenProcs			&& SCXSingleScreenConverter
	*----------------------
		PRIVATE m.saverec,m.part2line,m.part2,m.part2chr
		m.saverec = IIF(EOF(),1,RECNO())

		*- add accumulated procs
		IF !EMPTY(THIS.cProcs)
			REPLACE _FOX3SPR.sprmemo WITH ;
				C_CRLF + C_PROCS_CMMT_LOC + ;
				THIS.cProcs + C_CRLF + ;
				C_PROCSEND_CMMT_LOC ADDITIVE
		ENDIF

		*- add in stuff from VALIDs
		IF !EMPTY(_FOX3SPR.temp3)
			REPLACE _FOX3SPR.sprmemo WITH ;
				C_CRLF + C_CRLF + C_VALID_CMMT_LOC + ;
				_FOX3SPR.temp3 + ;
				C_VALIDEND_CMMT_LOC ADDITIVE
		ENDIF

		IF THIS.lHasSys16
			*- add special function to workaround fact that SYS(16)
			*- will return something new, now that methods are buried
			*- inside forms
			REPLACE _FOX3SPR.sprmemo WITH ;
				C_CRLF + C_CRLF + C_SYS16_CMMT_LOC + ;
				"PROCEDURE _Sys" + C_CRLF + ;
				C_TAB + "PARAMETERS nCode, nDepth" + C_CRLF + ;
				C_TAB + "IF PARAMETERS() = 1" + C_CRLF + ;
				C_TAB2 + [RETURN IIF(LEFT(SYS(16),9) == "PROCEDURE" AND OCCURS(" ",SYS(16)) >= 2,SUBS(SYS(16),AT(" ",SYS(16),2) + 1),SYS(16))] +  C_CRLF + ;
				C_TAB + "ELSE" + C_CRLF + ;
				C_TAB2 + "RETURN SYS(16,nDepth)" + C_CRLF + ;
				C_TAB + "ENDIF" + C_CRLF + ;
				C_SYS16END_CMMT_LOC ADDITIVE
		ENDIF

		GO	m.saverec

	ENDPROC		&&  AddGenProcs

	*----------------------------------
	PROCEDURE AddSRecs			&& SCXSingleScreenConverter
	*----------------------------------
		*- Add Screen objects
		PARAMETER platnum,cRecType
		
		*- cRecType = C_DNO		environment info
		*-          = C_CONTROL	control items
		
		PRIVATE oRec, a_scx2fld
		LOCAL nOldSize

		SCAN FOR platform = THIS.a_plat[m.platnum]
			*- skip odd records such as WIZARD record
			IF !INLIST(PLATFORM,C_DOS,C_WINDOWS,C_MAC,C_UNIX)
				LOOP
			ENDIF
			*- 10 group items
			IF objtype = 10
				LOOP
			ENDIF
			
			IF (m.cRecType # C_DNO AND INLIST(objtype,2,3,4)) OR;
				m.cRecType = C_DNO AND !INLIST(objtype,2,3,4)
				*- environment records have already been dealt with
				LOOP
			ENDIF
			
			SCATTER MEMO TO a_scx2fld
			
			*- Add font substitution object
			IF objtype=23
				THIS.AddFontSub()
				LOOP	
			ENDIF
			
			*- get control type
			oRec = THIS.AddSCXObj(objtype)

			IF TYPE("oRec") # "O"
				*- some sort of problem -- couldn't create the object - 
				*- invalid or unknown object type?
				LOOP
			ENDIF

			*- map main components of 2.5 control to 3.0
			oRec.MapIt
		
			*- write out record
			oRec.AddRec

			*- update status bar
			gOTherm.Update(THIS.nTmpCount/THIS.nRecCount * 100)
			THIS.nTmpCount = THIS.nTmpCount + 1
			
			*- check for error and terminate early
			IF THIS.lHadError
				THIS.CloseFiles
				CLEAR EVENTS
		  		RETURN
			ENDIF

			*- move any array creation references to the screen;s list of arrays
			IF !EMPTY(oRec.a_Dimes)
				m.noldSize = ALEN(THIS.a_Dimes)
				DIMENSION THIS.a_Dimes[m.noldSize + ALEN(oRec.a_Dimes)]
				=ACOPY(oRec.a_Dimes, THIS.a_Dimes, 1, ALEN(oRec.a_Dimes), m.noldsize + 1)
			ENDIF

			*- we're done with the object, so dispose of it
			RELEASE oRec
			
		ENDSCAN
	ENDPROC		&&  SCXSingleScreenConverter:AddSRecs


	*----------------------------------
	PROCEDURE AddSCXObj			&& SCXSingleScreenConverter
	*----------------------------------
		PARAMETER m.nObjType

		PRIVATE ctrltype,tmpobj,objclass,oForm
		STORE "" TO m.ctrltype,m.objclassm

		oForm = THIS.oConvForm

		DO CASE
			CASE m.nObjType = 1											&& screen
				m.ctrltype = T_FORM
				m.objclass = THIS.formclass
			CASE m.nObjType = 2											&& table
				m.ctrltype = T_DATANAV
				m.objclass = THIS.datanavclass
			CASE m.nObjType = 3											&& index
				THIS.lHasIDX = .T.
				*- wd be an index -- not sure how this is to be handled...
				REPLACE _FOX3SPR.load WITH C_SETIDX_CMMT_LOC + ;
					"SELECT " + TRIM(a_scx2fld[A_TAG2]) + C_CR + ;
					"SET INDEX TO " + TRIM(a_scx2fld[A_NAME])  + C_CR + C_CR ADDITIVE
			CASE m.nObjType = 4											&& relation
				m.ctrltype = T_RELATION
				m.objclass = THIS.datanavRelationclass
			CASE m.nObjType = 5											&& label
				m.ctrltype = T_LABEL
				m.objclass = THIS.labelclass
			CASE m.nObjType = 6											&& line
				m.ctrltype = T_LINE
				m.objclass = THIS.lineclass
			CASE m.nObjType = 7											&& box
				m.ctrltype = T_SHAPE
				m.objclass = THIS.shapeclass
			CASE m.nObjType = 11											&& list
				m.ctrltype = T_LIST
				m.objclass = THIS.listclass
			CASE m.nObjType = 12 AND OCCUR(";",a_scx2fld[A_PICTURE])=0		&& single button
				m.ctrltype = T_BTN
				m.objclass = THIS.btnclass
			CASE m.nObjType = 12											&& buttongroup
				m.ctrltype = T_BTNGRP
				m.objclass = THIS.btngclass
			CASE m.nObjType = 13											&& radio
				m.ctrltype = T_RADIOGRP
				m.objclass = THIS.radioclass
			CASE m.nObjType = 14											&& checkbox
				m.ctrltype = T_CBOX
				m.objclass = THIS.cboxclass
			CASE m.nObjType = 15 AND objcode = 0							&& say
				m.ctrltype = T_SAY
				m.objclass = THIS.sayclass
			CASE m.nObjType = 15 AND objcode = 1							&& get
				m.ctrltype = T_GET
				m.objclass = THIS.getclass
			CASE m.nObjType = 15 AND objcode = 2							&& edit
				m.ctrltype = T_EDIT
				m.objclass = THIS.editclass
			CASE m.nObjType = 16											&& popup
				m.ctrltype = T_POPUP
				m.objclass = THIS.popupclass
			CASE m.nObjType = 17 AND ;
				(style = 0 OR (" BITMAP" $ UPPER(name))) 
				*- ("\" $ ALLT(name)) OR ('"' $ ALLT(name)) OR (":" $ ALLT(name)) OR ;
				*- ('.' $ ALLT(name)) OR (' ' $ ALLT(name)))	&& image
				*- assume if filename or quoted expression, it's not a GENERAL field, so not OLE!
				m.ctrltype = T_PICT
				objclass = THIS.pictclass
			CASE m.nObjType = 17 AND style = 1								&& ole
				m.ctrltype = T_OLE
				objclass = THIS.oleclass
			CASE m.nObjType = 20 AND OCCUR(";",a_scx2fld[A_PICTURE]) = 0	&& invisible button
				m.ctrltype = T_INV
				m.objclass = THIS.invclass
			CASE m.nObjType = 20											&& inv buttongroup
				m.ctrltype = T_INVGRP
				m.objclass = THIS.invgclass
			CASE m.nObjType = 22											&& spinner
				m.ctrltype = T_SPIN
				m.objclass = THIS.spinclass
		ENDCASE
		
		IF EMPTY(m.ctrltype)
			RETURN ""
		ELSE
			RETURN CREATEOBJ(m.objclass,m.ctrltype,@oForm)
		ENDIF
	ENDPROC		&& AddSCXObj

	*------------------
	PROCEDURE UpdMethods			&& SCXSingleScreenConverter
	*------------------
		*- update the Formset.Readshow method after all objects have been processed

		LOCAL cGoTo,cCursVar,k
		cGoTo = ""

		SELECT (THIS.a_scx3alias[1])

		IF THIS.nFSetRecno > 0 AND THIS.nFSetRecno <= RECC()
			
			GO THIS.nFSetRecno

			IF !THIS.noReadPlainExpr
				THIS.AddMethods(M_SETUP1,;
					IIF(!EMPTY(THIS.a_reads[1]),THIS.a_reads[1] + C_CRLF + C_CRLF, "") + ;
					IIF(THIS.lDevMode,"",C_SETUP_CODE) + ;
					IIF(THIS.lHasDataNavObj AND THIS.a_pjxsets[A_OPENFILES],C_OPENTAB_CMMT_LOC + C_DATANAVLOAD + ;
						IIF(!EMPTY(THIS.cSetSkip),C_CRLF + THIS.cSetSkip,C_CRLF) + ;
						C_GOTO1 + ;
						C_CRLF,"") + ;
					THIS.cDefineWin,1)		&& add #SECTION2 stuff to FORM.LOAD
				*- clear out setup var, so it isn;t added to more than one screen in set
				THIS.a_reads[1] = ""
				IF THIS.lHasDataNavObj AND THIS.a_pjxsets[A_OPENFILES]
					*- code to remember the record pointer
					cGoTo = C_GOTO2_CMMT_LOC
					FOR k = 1 TO ALEN(THIS.a_tables)
						cCursVar = THIS.MakeVar(THIS.a_tables[k])
						cGoTo = cGoTo +  C_GOTO2A + THIS.a_tables[k] + ;
							C_GOTO2 + THIS.a_tables[k] + C_CR + ;
							C_TAB + cCursVar + C_GOTO3 + C_CR
					NEXT
					THIS.a_reads[3] = cGoTo + C_CR + THIS.a_reads[3]
				ENDIF
				THIS.AddMethods(M_CLEANUP,THIS.a_reads[3],1)
			ENDIF
		
			*- add in the refreshed SAYs after the READ SHOW stuff already there
			THIS.cReadShow = THIS.a_reads[6] + ;
				IIF(!EMPTY(THIS.cReadShow),C_CRLF + C_SAYSCOMMENT_LOC + C_CRLF,"") + ;
				THIS.cReadShow
			THIS.AddMethods(M_SHOW,THIS.CleanProc(THIS.cReadShow),1)

			IF THIS.lDevMode
				REPLACE _fox3spr.code WITH C_SEPARATOR + THIS.cFormSetName + C_CRLF + THIS.fp3method ADDITIVE
			ELSE
				REPLACE methods WITH THIS.fp3method && + C_CRLF + C_CRLF + THIS.cReadShow
			ENDIF

		ENDIF
	ENDPROC

	*------------------
	PROCEDURE MakeVar			&& SCXSingleScreenConverter
	*------------------
		PARAMETER cName
		RETURN C_GOTOVARPRE + PROPER(TRIM(cName)) + C_GOTOVAREXT
	ENDPROC
	*------------------
	FUNCTION CleanProc			&& SCXSingleScreenConverter
	*------------------
		*- clean out any screen directives or other stuff from snippets 
		*- that will cause problems in VFP

		PARAMETER cSnippet
		PRIVATE m.gendircount,savearea
		LOCAL nLine, cTmp

		IF !('#' $ cSnippet)
			*- if no '#', no directives, so return
			RETURN m.cSnippet
		ENDIF

		STORE 1 TO m.gendircount
		STORE SELECT() TO m.savearea

		REPLACE _FOX3SPR.temp1 WITH m.cSnippet
		REPLACE _FOX3SPR.temp4 WITH CleanWhite(_FOX3SPR.temp1)
		SELECT _FOX3SPR

		DO WHILE .T.
			m.gendir = UPPER(MemoFind("#","temp4",.T.,.F.,m.gendircount,.T.,.T.))
			DO CASE
				CASE m.gendir = C_NULL
					EXIT
				CASE LEFT(m.gendir,4) = "NAME"
					*- all this to comment out this code
				OTHERWISE
					m.gendircount = m.gendircount + 1
					LOOP
			ENDCASE
			*- Comment out directive
			*REPLACE temp1 with STUFF(temp1,MemoFind("#","temp4",.T.,0,m.gendircount,.F.,.T.),0,'*')
			_MLINE = 0
			nLine = MemoFind("#","temp4",.T.,.T.,m.gendircount,.F.,.T.)
			cTmp = MLINE(temp1,nLine - 1)						&& set _MLINE
			REPLACE temp1 WITH STUFF(temp1,_MLINE + IIF(nLine <= 1,0,2),0,'*')
			cTmp = MLINE(temp4,nLine - 1)						&& set _MLINE
			REPLACE temp4 WITH STUFF(temp4,_MLINE + IIF(nLine <= 1,0,1),0,'*')

		ENDDO

		SELECT (m.savearea)

		RETURN _FOX3SPR.temp1

	ENDFUNC

ENDDEFINE	&& SCXSingleScreenConverter 



**********************************************
DEFINE CLASS SCX30Converter AS SCXSingleScreenConverter
**********************************************

	lSet30Defaults = .F.

	*------------------------------------
	PROCEDURE Init				&& SCX30Converter
	*------------------------------------
		PARAMETER aParms, lBackup, lProjCall, lForceTransportDlog, lNoCompile

		LOCAL cExt1, cExt2, cbackext1 , cbackext2

		THIS.projcall = lProjCall

		THIS.lBackup = m.lBackup

		THIS.lNoCompile = lNoCompile

		THIS.lSet30Defaults = (aParms[12] > 1)					&& use the check box setting
		THIS.iPlatformCount = 1

		IF THIS.projcall										&& called from project
			THIS.cBackDir = gOPJX.cBackDir						&& used only for project
			DIMENSION THIS.a_scx2files[ALEN(gOPJX.a_s2files,1),3]
			DIMENSION THIS.a_scx3files[1]
			=ACOPY(gOPJX.a_s2files,THIS.a_scx2files)
			=ACOPY(gOPJX.a_s3files,THIS.a_scx3files)			
			THIS.cNewScx = THIS.a_scx3files[1]
			THIS.cCurrentFile = THIS.a_scx2files[1,3]
		ELSE
			IF EMPTY(aParms[1])
				THIS.lUserCall = .F.	&& assume called without output file
				aParms[1] = ADDBS(JUSTPATH(aParms[4])) + LEFT(SYS(3),7) + ".SCX"
				THIS.cNewScx = aParms[4]
			ELSE
				THIS.cNewScx = aParms[1]
			ENDIF
			DIMENSION THIS.a_scx2files[1,3]
			THIS.a_scx2files[1,1] = aParms[4]
			THIS.a_scx2files[1,2] = ""
			THIS.a_scx2files[1,3] = aParms[4]
			THIS.cCurrentFile = THIS.a_scx2files[1,3]
			*- go ahead and make backup now, before we start
			*- if screen needs to be transported, the original is around
			cext1 = JustExt(THIS.a_scx2files[1])
			cext2 = IIF(m.cext1 == C_VCXEXT, C_VCTEXT, C_SCTEXT)
			cbackext1 = IIF(m.cext1 == C_VCXEXT, C_VCXBACKEXT, C_SCXBACKEXT)
			cbackext2 = IIF(m.cext1 == C_VCXEXT, C_VCTBACKEXT, C_SCTBACKEXT) 
			THIS.a_scx3files[1] = FORCEEXT(aParms[1],m.cext1)
			*- copy old screen with S2X,S2T extensions. No need to backup .SCR files
			IF FILE(THIS.a_scx2files[1]) AND UPPER(JUSTEXT(THIS.a_scx2files[1])) = m.cext1
				COPY FILE (THIS.a_scx2files[1]) TO (FORCEEXT(THIS.a_scx2files[1],m.cbackext1))
			ENDIF
			IF FILE(FORCEEXT(THIS.a_scx2files[1],m.cext2))
				COPY FILE (FORCEEXT(THIS.a_scx2files[1],m.cext2)) TO (FORCEEXT(THIS.a_scx2files[1],m.cbackext2))
			ENDIF
			*- backup has been done
			THIS.lBackUp = .F.
		ENDIF

		IF THIS.lSet30Defaults
			gOTherm.SetTitle(C_THERMMSG2_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
			gOTherm.Update(0,"")
			gOTherm.visible = .T.
		ENDIF

		THIS.WriteLog("","")					&& force a blank line
		THIS.BeginLog(SYS(2027,THIS.cNewScx) )

		IF !THIS.KnownFile(THIS.a_scx2files[1,1])
			*- unknown file format -- error has already been written to the log
			THIS.oConvForm = .NULL.
			IF !THIS.projcall
				*- attempt to remove backup files (jd 04/16/96)
				THIS.EraseBackup
			ENDIF
			RETURN .F.
		ENDIF

		*- also check Read-Only status if project call (04/15/96 jd)
		IF THIS.projcall AND pReadOnly(THIS.cNewSCX)
			THIS.WriteLog(JUSTFNAME(THIS.cNewSCX),TRIM(E_FILE_LOC) + E_NOCONVERT3_LOC)
			RETURN .F.
		ENDIF

			*- now try to open SCX file -- returns alias
			THIS.a_scx2alias[1] = THIS.OpenFile(THIS.a_scx2files[1,1])
				
			IF EMPTY(THIS.a_scx2alias[1])
				*- error has been logged
				THIS.oConvForm = .NULL.
				IF !THIS.projcall
					*- attempt to remove backup files (jd 04/16/96)
					THIS.EraseBackup
				ENDIF
				RETURN .F.
			ENDIF

			THIS.c25alias = THIS.a_scx2alias[1]

			IF FCOUNT() = C_30SCXFLDS AND FIELD(4) = "CLASS"
				*- assume 3.0 format
				RETURN .T.
			ELSE
				*- error -- isn't a 3.0 SCX or VCX
				USE IN (THIS.a_scx2alias[1])
				THIS.WriteLog(JUSTFNAME(THIS.a_scx2files[1,1]),TRIM(E_WRONGFMT_LOC) + E_NOCONVERT_LOC)
				THIS.oConvForm = .NULL.
				RETURN .F.
			ENDIF
	ENDPROC

	*----------------------------------
	PROCEDURE Converter		&& SCX30Converter
	*----------------------------------
		*- some of the defaults for VFP 4.0 have changed. So if user
		*- elected to retain the 3.0 behavior/defaults, we need to explicitly
		*- write out those properties (05/14/96 jd)
		*- this means that the checkbox is checked
		*- also need to fix colorsource property written out in a way that 4.0 can't handle
		REPLACE ALL properties WITH STRTRAN(properties,"ColorSource = 3","ColorSource = 5") ;
			FOR LOWER(baseclass) = "form"
		REPLACE ALL properties WITH STRTRAN(properties,"ColorSource = 3","ColorSource = 0") ;
			FOR LOWER(baseclass)= "line"
		IF THIS.lSet30Defaults
			THIS.Set30Defaults()
		ENDIF
		THIS.CloseFiles
		THIS.lConverted = .T.	&& only set this if main screen
		RETURN
	ENDPROC

	*------------------------------------
	PROCEDURE CloseFiles			&& SCX30Converter
	*------------------------------------
		PRIVATE i
		LOCAL cHFile, iCtr, cTmpFile, cExt1, cExt2, cbackext1 , cbackext2, cErrFile

		cext1 = JustExt(THIS.a_scx2files[1,3])
		cext2 = IIF(m.cext1 == C_VCXEXT, C_VCTEXT, C_SCTEXT)
		cbackext1 = IIF(m.cext1 == C_VCXEXT, C_VCXBACKEXT, C_SCXBACKEXT)
		cbackext2 = IIF(m.cext1 == C_VCXEXT, C_VCTBACKEXT, C_SCTBACKEXT) 

		FOR i = 1 TO ALEN(THIS.a_scx2alias)
			IF USED(THIS.a_scx2alias[m.i])
				USE IN (THIS.a_scx2alias[m.i])
			ENDIF
		ENDFOR

		FOR m.i = 1 TO ALEN(THIS.a_scx3alias)
			IF USED(THIS.a_scx3alias[m.i])
				USE IN (THIS.a_scx3alias[m.i])
			ENDIF
		ENDFOR

		*- if lUserCall = .T., means called via project
		*- if lUserCall = .F., means screen opened individually (also used for catalogs)
		IF !THIS.lUserCall
			IF THIS.lHadError
				*- Delete temp files if we had an error
				IF FILE(THIS.a_scx3files[1])
					DELETE FILE (THIS.a_scx3files[1])
					DELETE FILE FORCEEXT(THIS.a_scx3files[1],m.cext2)
				ENDIF
			ELSE
				*- Compile form
				IF !THIS.lNoCompile
					THIS.lLocalErr = .T.
					IF THIS.iPlatformCount > 1 AND THIS.platform = C_MAC
						*- use proper name for Mac specific form
						*- converting more than one platform, and this one is Mac, so add _mac
						*- extension to the file name
						m.cTmpFile = (AddBS(JustPath(THIS.a_scx2files[1])) + ;
							JustStem(THIS.a_scx2files[1]) + C_MACEXT + "." + C_SCXEXT)
					ELSE
						cTmpFile = THIS.a_scx2files[1]
					ENDIF
					cErrFile = AddBS(JustPath(m.cTmpFile)) + JustStem(m.cTmpFile) + ".ERR"
					IF FILE(m.cErrFile)
						ERASE (m.cErrFile)
					ENDIF
					IF FILE(m.cTmpFile)
						COMPILE FORM (m.cTmpFile)
					ENDIF

					IF THIS.lHadLocErr OR FILE(m.cErrFile)
						THIS.WriteLog(SYS(2027,THIS.cNewScx),E_NOINCLUDE_LOC)
						=MESSAGEBOX(E_NOINCLUDE1_LOC + SYS(2027,THIS.cNewScx) + E_NOINCLUDE2_LOC)
						THIS.lHadLocErr = .F.
					ENDIF
					THIS.lLocalErr = .F.
				ENDIF

			ENDIF

		ENDIF

	ENDPROC		&&   SCX30Converter:CloseFiles

ENDDEFINE


**********************************************
DEFINE CLASS FRXConverter AS ConverterBase
**********************************************
	*- class to handle the conversion of 2.6 FRX files to 3.0 files

	datanavclass = "fpFRXdatanav"						&& data navigation object & cursor object
	datanavRelationClass = "fpFRXDataNavRelation"		&& data navigation relation object

	oConvForm = .NULL.
	nObjCount = 0
	fp3prop	= ""				&& properties
	nDNOCount = 0
	nDNORecNo = 0				&& remember record # of DataEnvironment (nee DNO) record
	lConverted = .F.			&& catch frx;s in 3.0 format
	lBackup = .F.				&& backup files (as .S2X, .S2T)
	lHasDataNavObj = .F.
	projcall = .F.
	parentName = ""
	lHasInvis = .F.				&& flag for if has invisible buttons
	lHasIDX	= .F.				&& IDX files used in environment?
	lUserCall = .T.
	cBackDir = ""
	cNewFrx = ""				&& New FRX file name
	cMainCurs = ""				&& alias of main table
	lNoCompile = .F.			&& flag to determine whether to compile right away, or later, in batch

	lNeedsDE = .T.				&& need a DataEnvironment made? (transporter issue) (11/9/95 jd)

	cfrx2files = ""
	cfrx3files = ""
	cfrx3alias = ""

	DIMENSION a_scx3files[1]	&& for compatibility with SCX converter
	DIMENSION a_pjxsets[10]		&&  "         "        "   "      ""
	a_pjxsets = .F.

	formnum = 1

	DIMENSION a_tables[1]		&& used by datanav object
	DIMENSION a_torder[1]
	a_tables = ""
	a_torder = ""

	*------------------------------------
	PROCEDURE Init				&& FRXConverter
	*------------------------------------
		PARAMETER aParms, lBackup, lProjCall, lForceTransportDlog, lNoCompile

		PRIVATE cNewFrxName, oThis
		LOCAL m.cnewext1, m.cnewext2, m.coldext1, m.coldext2

		THIS.oConvForm = THIS
		m.oThis = THIS

		THIS.projcall = lProjCall

		THIS.lBackup = m.lBackup

		THIS.lTransDlog = lForceTransportDlog

		THIS.lNoCompile = lNoCompile

		THIS.llog = aParms[9]
		THIS.cLogFile = aParms[10]

		THIS.lAutoOpen = .T.	&& auto open tables for FRX files
		THIS.lAutoClose = .T.	&& auto close tables for FRX files
		
		IF THIS.projcall					&& called from project
			THIS.cBackDir = gOPJX.cBackDir  && used only for project
			THIS.cFrx2files = gOPJX.f2files
			THIS.cFrx3files = gOPJX.f3files
			THIS.cNewFrx = THIS.cFrx3files
			THIS.cCurrentFile = THIS.cFrx3files
			IF JUSTEXT(THIS.cFrx2Files) = "LBX"
				*- labels
				m.coldext1 = "LBX"
				m.coldext2 = "LBT"
				m.cnewext1 = C_LBXBACKEXT
				m.cnewext2 = C_LBTBACKEXT
			ELSE
				*- reports
				m.coldext1 = "FRX"
				m.coldext2 = "FRT"
				m.cnewext1 = C_FRXBACKEXT
				m.cnewext2 = C_FRTBACKEXT
			ENDIF
		ELSE
			THIS.cNewFrx = aParms[1]
			IF EMPTY(aParms[1])
				THIS.lUserCall = .F.	&& assume called without output file
				aParms[1] = ADDBS(JUSTPATH(aParms[4])) + "F" + RIGHT(SYS(3),7) + ".FRX"
				THIS.cNewFrx = aParms[4]
			ENDIF
			THIS.cFrx3files = aParms[1]
			THIS.cFrx2files = aParms[4]
			THIS.cCurrentFile = THIS.cFrx2files
			IF JUSTEXT(THIS.cFrx2Files) = "LBX"
				*- labels
				m.coldext1 = "LBX"
				m.coldext2 = "LBT"
				m.cnewext1 = C_LBXBACKEXT
				m.cnewext2 = C_LBTBACKEXT
			ELSE
				*- reports
				m.coldext1 = "FRX"
				m.coldext2 = "FRT"
				m.cnewext1 = C_FRXBACKEXT
				m.cnewext2 = C_FRTBACKEXT
			ENDIF
			*- go ahead and make backup now, before we start
			*- this is if form needs to be transported, the original is around
			IF THIS.lBackUp
				*- copy old form with F2X,F2T extensions
				COPY FILE (THIS.cFrx2files) TO (FORCEEXT(THIS.cFrx2files,m.cnewext1))
				IF FILE(FORCEEXT(THIS.cFrx2files,m.coldext2))
					COPY FILE (FORCEEXT(THIS.cFrx2files,m.coldext2)) TO (FORCEEXT(THIS.cFrx2files,m.cnewext2))
				ENDIF
				*- backup has been done
				THIS.lBackUp = .F.
			ENDIF
		ENDIF


		gOTherm.SetTitle(C_THERMMSG8_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
		gOTherm.Update(0,"")
		gOTherm.visible = .T.

		THIS.WriteLog("","")
		THIS.BeginLog(SYS(2027,THIS.cCurrentFile))

		THIS.platform = THIS.GetPlatForm()
		THIS.nTimeStamp = THIS.TStamp()

		*- also check Read-Only status if project call (04/15/96 jd)
		IF THIS.projcall AND pReadOnly(THIS.cCurrentFile)
			THIS.WriteLog(JUSTFNAME(THIS.cCurrentFile),TRIM(E_FILE_LOC) + E_NOCONVERT3_LOC)
			THIS.lHadError=.T.
			oThis = .NULL.
			RETURN .F.
		ENDIF

		*- now try to open FRX file -- returns alias
		*- first -- may not be a DBF (old style .FRM report), or a 1.02 FRX file
		IF FILE(THIS.cFrx2Files)
			IF Readable(THIS.cFrx2Files)
				IF !THIS.IsDBF(THIS.cFrx2Files)
					*- not a DBF, so try and migrate it
					m.cNewFrxName = ForceExt(SYS(2027,THIS.cFrx2Files),;
						IIF(UPPER(JUSTEXT(THIS.cFrx2Files)) $ "LBL|LBX",'LBX','FRX'))
					SET MESSAGE TO C_MIGRATEMSG_LOC
					gOTherm.SetTitle(C_THERMMSG12_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
					IF !MigDB4(THIS.cFrx2Files, @oThis)
						THIS.WriteLog(JUSTFNAME(THIS.cFrx2Files),E_NOMIG_LOC)
						=MESSAGEBOX(E_WRONGFMT2_LOC + " " + E_NOCONVERT_LOC)
						THIS.lHadError=.T.
						oThis = .NULL.
						RETURN .F.
					ELSE
						*- go ahead and transport
						gOTherm.SetTitle(C_THERMMSG9_LOC + LOWER(PARTIALFNAME(THIS.cFrx2Files,C_FILELEN)))
						gOTherm.Update(0,"")
						LOCAL m.lOldFrxShow
						m.lOldFrxShow = gAShowMe[N_TRANFILE_FRX,1]
						gAShowMe[N_TRANFILE_FRX,1] = .F.
						DO (gTransport) WITH m.cNewFrxName,13,.F.,gAShowMe, m.gOTherm,m.cNewFrxName
						gAShowMe[N_TRANFILE_FRX,1] = m.lOldFrxShow
						*- newly converted
						THIS.cFrx2Files = m.cNewFrxName
					ENDIF
				ENDIF
			ENDIF
		ENDIF

		oThis = .NULL.

		THIS.c25alias = THIS.OpenFile(THIS.cFrx2Files)
		IF EMPTY(THIS.c25alias)
			*- error has been logged
			*- attempt to remove backup files (jd 04/16/96)
			THIS.EraseBackup
			RETURN .F.
		ENDIF
		
		*- Check for file format
		DO CASE
			CASE FCOUNT() = C_FRXFLDS AND FIELD(1) = "PLATFORM"
				*- check for 2.5 FRX type and no platform objects
				LOCATE FOR platform = THIS.platform
				IF !FOUND()
					*- =MESSAGEBOX(E_NOPLATOBJS_LOC)
					IF !THIS.Conv20FRX(13)
						THIS.lHadError = .T.
						RETURN
					ENDIF
				ELSE
					*- check to see if any records are later than Windows records
					*- if so, call transporter
					CALCULATE MAX(timestamp) FOR platform = THIS.platform TO m.imaxThisTime
					CALCULATE MAX(timestamp) FOR platform # THIS.platform TO m.imaxOtherTime
					IF m.imaxOtherTime > m.imaxThisTime
						IF !THIS.Conv20FRX(13)
							THIS.lHadError = .T.
							RETURN
						ENDIF
					ENDIF
				ENDIF
			CASE FCOUNT() = C_20FRXFLDS AND FIELD(1) = "OBJTYPE"
				*- check for 2.0 FRX type
				*- Invoke Transporter
				IF !THIS.Conv20FRX(3)
					THIS.lHadError = .T.
					RETURN
				ENDIF
			CASE FCOUNT() = C_30FRXFLDS AND FIELD(75) = "USER"
				*- assume 3.0 format, report/label was already converted
				*- (this could be called while converting a 2.x Project
				*- that contains frx's that have already been converted)
				*-
				*- look to see if platform records exist for this platform
				*- if not, we will need to transport (10/26/95 jd)
				*-
				LOCATE FOR platform = THIS.platform
				IF !FOUND()
					*- call transporter, as if it were a 2.5 file
					THIS.Conv20FRX(13)
					*- since no records existed from this platform, we will just use whatever
					*- data environment stuff that was in original, and it has been transported over...
					THIS.lNeedsDE = .F.
					IF USED(THIS.c25alias)
						USE IN (THIS.c25alias)
					ENDIF
					IF USED(THIS.cFRX3alias)
						USE IN (THIS.cFRX3alias)
					ENDIF
					*- overwrite the old file with the new one
					*- new one will be deleted later, in pjxconverter.converter
					IF THIS.projcall
						COPY FILE (THIS.cFRX2files) TO (FORCEEXT(THIS.cFRX3files,m.coldext1))
						COPY FILE (FORCEEXT(THIS.cFRX2files,m.coldext2)) TO (FORCEEXT(THIS.cFRX3files,m.coldext2))
					ENDIF
				ELSE
					*- check to see if any records are are later than Mac records (11/7/95 jd)
					*-CALCULATE MAX(timestamp) FOR platform = THIS.platform TO m.imaxThisTime
					*-CALCULATE MAX(timestamp) FOR platform # THIS.platform TO m.imaxOtherTime

					*- only check header record (11/13/95 jd)
					LOCATE FOR platform = THIS.platform AND objtype = N_OTHEADER
					imaxThisTime = timestamp
					LOCATE FOR platform # THIS.platform AND objtype = N_OTHEADER
					imaxOtherTime = timestamp

					IF m.imaxOtherTime > m.imaxThisTime
						IF !THIS.Conv20FRX(13)
							THIS.lHadError = .T.
							RETURN
						ENDIF
					ENDIF
					GO TOP
					IF environ
						*- has records for this platform, but check to see if there is an
						*- environment but no DataEnvironment -- may need to make one
						LOCATE FOR platform = THIS.platform AND objtype = N_FRX_DATAENV
						THIS.lNeedsDE = !FOUND()
					ENDIF
				ENDIF
				IF !THIS.lNeedsDE
					THIS.lConverted = .T.
					THIS.oConvForm = .NULL.
					RETURN
				ENDIF
			CASE FCOUNT() = C_20LBXFLDS AND FIELD(1) = "OBJTYPE"
				*- assume a 2.0 LBX type
				*- invoke transporter
				IF !THIS.Conv20FRX(4)
					THIS.lHadError = .T.
					RETURN
				ENDIF
			OTHERWISE
				USE IN (THIS.c25alias)
				THIS.lHadError = .T.
				=MESSAGEBOX(E_WRONGFMT2_LOC)
				RETURN
		ENDCASE
	
		
		*- this cursor is for working with memo fields
		IF USED("_FOX3SPR")
		  USE IN _FOX3SPR
		ENDIF

		CREATE CURSOR _FOX3SPR (load m)
		APPEND BLANK
		SELECT (THIS.c25alias)

	ENDPROC		&& FRXConverter:Init

	*----------------------------------
	PROCEDURE Converter		&& FRXConverter
	*----------------------------------
		PRIVATE oRec, oForm, a_scx2fld
		LOCAL nRecCount, nCurRec, tempfile

		gOTherm.SetTitle(C_THERMMSG8_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
		gOTherm.Update(0)

		*- External preprocessor hook
	  	THIS.PreForm

		*- Create new FRX file here, and move records from 2.6 over
		THIS.CreateFRX
		THIS.new30alias = THIS.cFrx3Alias

		gOTherm.Update(33)

		*- the only thing we need to do is create a data environment, and take the data from the
		*- old environment records, which we left at the top of the file...
		SELECT (THIS.new30alias)
		GO TOP
		IF environ AND THIS.lNeedsDE
			*- no need to bother with this, if no environ (right?)

			oForm = THIS.oConvForm

			COUNT FOR INLIST(objtype,2,3,4) TO THIS.nRecCount
			THIS.nTmpCount = 1  	&& reset

			LOCATE FOR objtype = 2
			SCAN WHILE INLIST(objtype,2,3,4)
				oRec = .NULL.
				SCATTER MEMO TO a_scx2fld
				nCurRec = RECNO()

				DO CASE
					CASE objtype = 2
						*- table/cursor
						oRec = CREATEOBJ(THIS.dataNavClass,T_DATANAV,@oForm)
						oRec.fp3objtype = N_FRX_CURSOR
					CASE objtype = 4
						*- relation
						oRec = CREATEOBJ(THIS.datanavRelationclass,T_RELATION,@oForm)
						oRec.fp3objtype = N_FRX_RELATION
					CASE objtype = 3
						*- index
						REPLACE _FOX3SPR.load WITH "SELECT " + TRIM(tag) + C_CR + ;
							"SET INDEX TO " + TRIM(name)  + C_CR + C_CR ADDITIVE
						THIS.lHasIDX = .T.
				ENDCASE
				IF TYPE("oRec") == "O"
					*- created the object 
					*- map any values
					oRec.MapIt
				
					*- write out record -- but use our own device to do that, since
					*- the DE record was done for an SCX
					oRec.AddRec

					*- check for error and terminate early
					IF THIS.lHadError
						THIS.CloseFiles
						CLEAR EVENTS
				  		RETURN -1
					ENDIF
				ENDIF

				RELEASE oRec

				gOTherm.Update(THIS.nTmpCount/THIS.nRecCount * 57 + 33) && account for 33 already shown, + 10 to be added in closefiles!

				*- return to the record we were on
				GO nCurRec

			ENDSCAN

			*- add in LOAD method, if IDX files were present
			IF !EMPTY(THIS.cSetSkip)
				*- add in SET SKIP code
				REPLACE _FOX3SPR.load WITH THIS.cSetSkip ADDITIVE
			ENDIF

			*- add DESTROY method
			REPLACE _FOX3SPR.load WITH C_FRXDEDESTROY ADDITIVE

			IF BETWEEN(THIS.nDNORecNo,1,RECC())
				*- may say there's an environment, but no environment records are in the FRX
				GO THIS.nDNORecNo
				REPLACE tag WITH C_DATANAVOPEN + _FOX3SPR.load
				IF !THIS.lNoCompile
					THIS.CompileFRX
				ENDIF

				*- sort on platform, so dataenvironment is with other records from this platform
				m.tempfile = ADDBS(JUSTPATH(THIS.cfrx3files)) + "F" + LEFT(SYS(3),7) +  "." + "FRX"
				SORT ON platform TO (m.tempfile)
				*- close the file, and replace the new FRX file with this sorted one
				SELECT (THIS.new30alias)
				USE
				DELETE FILE (THIS.cfrx3files)
				DELETE FILE (FORCEEXT(THIS.cfrx3files,IIF(JUSTEXT(THIS.cfrx3files) = "LBX","LBT","FRT")))
				RENAME (m.tempfile) TO (THIS.cfrx3files)
				RENAME (FORCEEXT(m.tempfile,"FRT")) TO (FORCEEXT(THIS.cfrx3files,IIF(JUSTEXT(THIS.cfrx3files) = "LBX","LBT","FRT")))

				*- reopen it
				USE  (THIS.cfrx3files)

			ENDIF

			
			RELEASE oForm

		ENDIF && environ

		*- float and stretch are different properties in VFP. Explicitly set float if stretch
		REPLACE ALL float WITH .T. FOR stretch

		*- External postprocessor hook
  		THIS.PostForm

		gOTherm.Update(90)

		*- Close FRX files
		THIS.CloseFiles

		gOTherm.Complete

		*- write out end to log
		THIS.EndLog(THIS.cCurrentFile)

		RETURN THIS.cFRX2files

	ENDPROC

	*------------------------------------
	PROCEDURE CloseFiles			&& FRXConverter
	*------------------------------------
		PRIVATE i, m.cnewext1, m.cnewext2, m.coldext1, m.coldext2

		IF JUSTEXT(THIS.cFrx2Files) = "LBX"
			*- labels
			m.coldext1 = "LBX"
			m.coldext2 = "LBT"
			m.cnewext1 = C_LBXBACKEXT
			m.cnewext2 = C_LBTBACKEXT
		ELSE
			*- reports
			m.coldext1 = "FRX"
			m.coldext2 = "FRT"
			m.cnewext1 = C_FRXBACKEXT
			m.cnewext2 = C_FRTBACKEXT
		ENDIF

		IF USED("_FOX3SPR")
			*- may need to move index file code over
			USE IN _FOX3SPR
		ENDIF
		
		IF USED(THIS.c25alias)
			USE IN (THIS.c25alias)
		ENDIF

		IF USED(THIS.cFRX3alias)
			USE IN (THIS.cFRX3alias)
		ENDIF

		*- Compile forms
		IF .F. AND FILE(THIS.cFRX3files)
			COMPILE FORM (THIS.cFRX3files)
		ENDIF

		*- if lUserCall = .T., means called via project
		*- if lUserCall = .F., means report opened individually
		*- not used with project calls.
		IF !THIS.lUserCall
			IF THIS.lHadError
				*- Delete temp files if we had an error
				IF FILE(THIS.cFRX3files)
					DELETE FILE (THIS.cFRX3files)
					DELETE FILE (FORCEEXT(THIS.cFRX3files,m.coldext2))
				ENDIF
			ELSE
				IF THIS.lBackUp
					*- erase existing backup file if it;s there
					IF FILE(FORCEEXT(THIS.cFRX2files,m.cnewext1))
						DELETE FILE (FORCEEXT(THIS.cFRX2files,m.cnewext1))
					ENDIF
					IF FILE(FORCEEXT(THIS.cFRX2files,m.cnewext2))
						DELETE FILE (FORCEEXT(THIS.cFRX2files,m.cnewext2))
					ENDIF
					*- Rename old screen with F2X,F2T extensions
					RENAME (THIS.cFRX2files) TO (FORCEEXT(THIS.cFRX2files,m.cnewext1))
					RENAME (FORCEEXT(THIS.cFRX2files,m.coldext2)) TO (FORCEEXT(THIS.cFRX2files,m.cnewext2))
				ELSE
					DELETE FILE (THIS.cFRX2files)
					DELETE FILE (FORCEEXT(THIS.cFRX2files,m.coldext2))
				ENDIF

				*- Rename new FP3 report 
				RENAME (THIS.cFRX3files) TO (THIS.cFRX2files)
				*- temp new file is always named as an FRX file, so force FRT extension
				RENAME (FORCEEXT(THIS.cFRX3files,"FRT")) TO (FORCEEXT(THIS.cFRX2files,m.coldext2))
			ENDIF
		ENDIF

	ENDPROC		&&  CloseFiles

	*------------------
	PROCEDURE CreateFRX			&& FRXConverter
	*------------------
		*- need to add just one field

		SELECT (THIS.c25alias)
		COPY TO (THIS.cFRX3Files)

		SELECT 0
		USE (THIS.cFRX3Files)

		*- add the new user field
		IF TYPE("user") == 'U'
			ALTER TABLE (THIS.cFRX3Files) ADD user m

			*- ALTER TABLE leaves some mess behind...
			IF FILE(FORCEEXT(THIS.cFRX3Files,"BAK"))
				DELETE FILE (FORCEEXT(THIS.cFRX3Files,"BAK"))
			ENDIF
			IF FILE(FORCEEXT(THIS.cFRX3Files,"TBK"))
				DELETE FILE (FORCEEXT(THIS.cFRX3Files,"TBK"))
			ENDIF

		ENDIF

		*- remember the alias
		THIS.cFrx3Alias  = ALIAS()

	ENDPROC

	*------------------
	PROCEDURE Conv20FRX
	*------------------
	*- This transports an frx file

		PARAMETER m.frxtype
		*- m.frxtype = 13	&& FP2.5 FRX format
		*- m.frxtype = 3		&& FP2.0 FRX format
		*- m.frxtype = 4		&& FP2.0 LBX format
		LOCAL m.oldudfp
		LOCAL m.cOldMess

		USE IN (THIS.c25alias)
		gOTherm.SetTitle(C_THERMMSG9_LOC + LOWER(PARTIALFNAME(THIS.cCurrentFile,C_FILELEN)))
		m.oldudfp = SET("UDFP")
		SET UDFP TO REFERENCE
		m.cOldMess = SET("MESSAGE",1)
		DO (gTransport) WITH THIS.cFrx2files,m.frxtype,.F.,gAShowMe, m.gOTherm,THIS.cCurrentFile,THIS.lTransDlog
		SET UDFP TO &oldudfp
		SET MESSAGE TO (cOldMess)
		THIS.c25alias = THIS.OpenFile(THIS.cfrx2files)
		IF !EMPTY(THIS.c25alias)
			IF (FCOUNT() = C_FRXFLDS OR FCOUNT() = C_30FRXFLDS) AND FIELD(1) = "PLATFORM"		&& may be 3.0 transport, so check for 2.x + 3.0 field counts (jd 11/13/95)
				LOCATE FOR Platform = THIS.Platform
				IF FOUND()
					RETURN .T.
				ENDIF
			ENDIF
			USE IN (THIS.c25alias)
		ENDIF
		THIS.lHadError=.T.
		RETURN .F.
	ENDPROC


	*------------------
	PROCEDURE PostForm
	*------------------
		REPLACE ALL uniqueid WITH SYS(2015) FOR uniqueid = "~A" OR uniqueid = '^'
		REPLACE ALL timestamp WITH THIS.nTimeStamp FOR platform = THIS.platform
	ENDPROC

	*------------------------------------
	PROCEDURE Cleanup				&& FRXConverter
	*------------------------------------
		*- this proc is called by Error, and tries to put things back the way they were
		*- if cleaning up from a crashed project conversion, the pjx cleanup will
		*- handle the reports
		LOCAL i

		IF !THIS.lUserCall 
			*- report opened individually
			*- Delete temp files if we had an error
			CLOSE TABLES
			IF FILE(THIS.cFrx3files)
				DELETE FILE (THIS.cFrx3files)
				IF FILE(FORCEEXT(THIS.cFrx3files,IIF(JUSTEXT(THIS.cFrx3files) = "LBX","LBT","FRT")))
					DELETE FILE (FORCEEXT(THIS.cFrx3files,IIF(JUSTEXT(THIS.cFrx3files) = "LBX","LBT","FRT")))
				ENDIF
			ENDIF
			IF !THIS.lBackUp
				*- a backup could have already been made (e.g., a 2.0 file was being converted)
				*- restore old report/label from F2X,F2T extensions
				IF	FILE(THIS.cFrx2files) AND ;
					FILE(FORCEEXT(THIS.cFrx2files,C_FRXBACKEXT)) AND ;
					FILE(FORCEEXT(THIS.cFrx2files,"FRT")) AND ;
					FILE(FORCEEXT(THIS.cFrx2files,C_FRTBACKEXT))
					*- all of the files are there to attempt this, so...
					DELETE FILE (THIS.cFrx2files)
					DELETE FILE (FORCEEXT(THIS.cFrx2files,"FRT"))
					IF !FILE(THIS.cFrx2files)
						RENAME (FORCEEXT(THIS.cFrx2files,C_FRXBACKEXT)) TO (THIS.cFrx2files)
					ENDIF
					IF !FILE(FORCEEXT(THIS.cFrx2files,"FRT"))
						RENAME (FORCEEXT(THIS.cFrx2files,C_FRTBACKEXT)) TO (FORCEEXT(THIS.cFrx2files,"FRT"))
					ENDIF
				ENDIF
				*- under certain circumstances, these may be left around
				IF FILE(FORCEEXT(THIS.cFrx2files,C_FRXBACKEXT))
					DELETE FILE (FORCEEXT(THIS.cFrx2files,C_FRXBACKEXT))
				ENDIF
				IF FILE(FORCEEXT(THIS.cFrx2files,C_FRTBACKEXT))
					DELETE FILE (FORCEEXT(THIS.cFrx2files,C_FRTBACKEXT))
				ENDIF
			ENDIF
		ENDIF

		THIS.oConvForm = .NULL.

	ENDPROC

	*------------------
	PROCEDURE EraseBackup			&& FRXConverter
	*------------------
		*- get rid of backup files (jd 04/15/96)
		IF FILE(FORCEEXT(THIS.cFrx2files,C_FRXBACKEXT))
			ERASE FORCEEXT(THIS.cFrx2files,C_FRXBACKEXT)
		ENDIF
		IF FILE(FORCEEXT(THIS.cFrx2files,C_FRTBACKEXT))
			ERASE FORCEEXT(THIS.cFrx2files,C_FRTBACKEXT)
		ENDIF

	ENDPROC


ENDDEFINE		&& FRXConverter


**********************************************
DEFINE CLASS fp25obj AS ConverterBase
**********************************************
	
	*- Properties corresponding to new SCX form fields
	fp3plat 	= ""	&& platform
	fp3saveplat = C_WINDOWS		&& platform that is saved in SCX file (always WINDOWS on VFP Mac --jd 03/27/96)
	fp3id 		= ""	&& uniqueid
	fp3time 	= 0		&& timestamp
	fp3comment	= ""	&& comment
	fp3class	= ""	&& class
	fp3base		= ""	&& baseclass
	fp3name 	= ""	&& objname
	fp3prop		= ""	&& properties
	fp3method	= ""	&& methods
	fp3parent	= ""	&& parent
	fp3reserved1= ""	&& reserved1
	fp3reserved2= ""	&& reserved2
	fp3reserved6= ""	&& reserved6

	*- Fontmetrics
	fp3font		= ""	&& font face
	fp3fsize	= ""	&& font size
	fp3fstyle	= ""	&& font style
	fp3font1	= 1		&& FONT(TM_HEIGHT)
	fp3font5	= 0		&& FONT(TM_EXTERNALLEADING)
	fp3font6	= 1 	&& FONT(TM_AVECHARWIDTH)

	fp3fudge	= 0		&& fudge factor for horizontal positioning

	*- Object Code 
	fp25OT = 0
	fp25OC = 0
	
	*- Object References
	formRef = ""
	
	*- Picture clause parts (ex. "@KJ XXXXX")
	*- ex. picword1 = @KJ  
	*-     picword2 = XXXXX
	
	picword1 = ""
	picword2 = ""
	picword3 = ""
	hasitse  = .F.
	
	iColorSource = I_DEFCOLORSOURCE
	
	*----------------------
	PROCEDURE Init			&& fp25obj
	*----------------------
		PARAMETER parm1,parm2
		THIS.fp3class  = m.parm1			&& class
		THIS.fp3base   = m.parm1			&& baseclass
		THIS.formRef   = m.parm2			&& form reference
		THIS.fp25OT    = a_scx2fld[A_ObjType]
		THIS.fp25OC    = a_scx2fld[A_ObjCode]
		THIS.fp3prop   = ""					&& properties
		THIS.fp3method = ""					&& methods
	ENDPROC

	*----------------------
	PROCEDURE MapIt
	*----------------------
		*- This is the main mapping program.
		*- It uses FP25OBJ base methods unless
		*- overridden by subclass method.
		THIS.PreMap
		THIS.AddBasic
		THIS.AddFont
		THIS.AddPos
		THIS.AddColor
		THIS.AddMain
		THIS.PostMap
		THIS.WriteName	 && note Name property must be written last!
	ENDPROC

	*----------------------
	PROCEDURE AddMain
	*----------------------
	ENDPROC
	
	*----------------------
	PROCEDURE PreMap
	*----------------------
	ENDPROC
	
	*----------------------
	PROCEDURE PostMap
	*----------------------
	ENDPROC

	*----------------------
	FUNCTION FullBMP
	*----------------------
		*- returns fullpath of BMP
		PARAMETER bmpfpath
		RETURN bmpfpath
		
	ENDFUNC

	*----------------------
	FUNCTION GetPicPart
	*----------------------
		PARAMETER cPictStr
		*- picword1 (format)
		*- picword2 (inputmsk,bmp,caption)
		*- picword3 (misc #ITSE add-ons)
		
		PRIVATE spcloc,tmpstr,tmpcnt,tmpword,tmploc,tmpprestr
		IF EMPTY(ALLTRIM(m.cPictStr))
			RETURN
		ENDIF
		
		m.cPictStr = SUBS(m.cPictStr,2,LEN(m.cPictStr) - 2)	&& EVAL(m.cPictStr)	&& remove dbl quotes
		
		*- check for #ITSEXPRESSION
		IF !EMPTY(THIS.formRef.itse_expr) AND ;
			INLIST(a_scx2fld[A_ObjType],15,22) AND ;
			AT(THIS.formRef.itse_expr,m.cPictStr) # 0
			*- parse out GENSCRN tricks
			*- ex.   ~ [] NOMODIFY 
			*-       ~ [@!] COLOR ,N/N
			THIS.hasitse = .T.
			m.tmploc = AT(THIS.formRef.itse_expr,m.cPictStr)
			m.tmpstr = ALLT(SUBSTR(m.cPictStr,m.tmploc + 1))
			m.tmpprestr = LEFT(m.cPictStr,m.tmploc - 1)
			m.tmpcnt = 1
			DO WHILE .T.
				tmpword = WORDNUM(m.tmpstr,m.tmpcnt)
				DO CASE
					CASE EMPTY(m.tmpword)
						EXIT
					CASE UPPER(m.tmpword) $ "NOMODIFY FONT STYLE COLOR SIZE"
						THIS.picword3 = SUBSTR(m.tmpstr,AT(m.tmpword,m.tmpstr))
						m.tmpstr = RTRIM(LEFT(m.tmpstr,AT(m.tmpword,m.tmpstr)-1)) 				
						EXIT
				ENDCASE
				m.tmpcnt = m.tmpcnt+1
			ENDDO
			IF EMPTY(m.tmpprestr)
				m.cPictStr = m.tmpstr
			ENDIF
		ENDIF
		
		*- get location of space
		spcloc = ATC(" ",m.cPictStr)
		
		DO CASE
			CASE THIS.hasitse AND !EMPTY(m.tmpprestr)
				THIS.picword1 = THIS.addquotes(ALLTRIM(m.tmpprestr))
				THIS.picword2 = ALLTRIM(m.tmpstr)
			CASE THIS.hasitse AND AT("@",m.cPictStr)=0
				THIS.picword2 = m.cPictStr
			CASE THIS.hasitse
				THIS.picword1 = m.cPictStr
			CASE m.spcloc = 0 AND AT("@",m.cPictStr) # 0
				THIS.picword1 = m.cPictStr
			CASE m.spcloc = 0
				THIS.picword2 = m.cPictStr
			CASE AT("@",m.cPictStr) = 0
				THIS.picword2 = m.cPictStr
			CASE EMPTY(SUBSTR(m.cPictStr,m.spcloc))
				THIS.picword1 = m.cPictStr
			OTHERWISE
				THIS.picword1 = LEFT(m.cPictStr,m.spcloc-1)
				THIS.picword2 = ALLT(SUBSTR(m.cPictStr,m.spcloc+1))
		ENDCASE
		
		IF THIS.hasitse
			* special handling for #ITSEXPRESSION
			IF !EMPTY(THIS.picword1) AND EMPTY(m.tmpprestr) AND ;
				LEFT(LTRIM(THIS.picword1),1) # "+"
				THIS.picword1 = "+" + THIS.picword1
			ENDIF
			
			IF !EMPTY(THIS.picword2) AND ;
				LEFT(LTRIM(THIS.picword2),1) # "+"
				THIS.picword2 = "+" + THIS.picword2
			ENDIF		
		ENDIF
	ENDFUNC
	
	*----------------------
	PROCEDURE AddRec		&& fp25obj
	*----------------------
		*- Add record to FORM file
		*- NOTE: This is overridden in some cases (e.g. fp25form)
		*- where different value needs to be inserted into reserved4 field
		IF THIS.formRef.lDevMode
			*- put methods someplace else
			INSERT INTO (THIS.formRef.new30alias) ;
			 (platform,uniqueid,timestamp,;
			  class,baseclass,objname,parent,properties,;
			  user,reserved2);
			 VALUES(THIS.fp3saveplat,THIS.fp3id,THIS.fp3time,;
			  THIS.fp3class,THIS.fp3base,THIS.fp3name,;
			  THIS.fp3parent,THIS.fp3prop,;
			  THIS.fp3comment,;
			  THIS.fp3reserved2)
			IF !EMPTY(THIS.fp3method)
				REPLACE _fox3spr.code WITH C_SEPARATOR + THIS.fp3name + C_CRLF + THIS.fp3method ADDITIVE
			ENDIF
		ELSE
			INSERT INTO (THIS.formRef.new30alias) ;
			 (platform,uniqueid,timestamp,;
			  class,baseclass,objname,parent,properties,;
			  methods,user,reserved2);
			 VALUES(THIS.fp3saveplat,THIS.fp3id,THIS.fp3time,;
			  THIS.fp3class,THIS.fp3base,THIS.fp3name,;
			  THIS.fp3parent,THIS.fp3prop,;
			  THIS.fp3method,;
			  THIS.fp3comment,;
			  THIS.fp3reserved2)
		ENDIF
	ENDPROC

	*----------------------
	PROCEDURE AddBasic		&& fp25obj
	*----------------------
	    *- Update common fields
		THIS.fp3plat 	= a_scx2fld[A_PLATFORM]	 	&& platform
		THIS.fp3time 	= THIS.formRef.nTimeStamp 	&& timestamp
		THIS.fp3comment	= a_scx2fld[A_COMMENT]		&& comment
		THIS.fp3parent  = THIS.formRef.parentName
		THIS.fp3id 		= "~A"					 	&& we'll sort on this field later
		THIS.formRef.lHasInvis = .T.
	ENDPROC

	*----------------------
	PROCEDURE AddName	&& fp25obj
	*----------------------
		PARAMETER cAddname
		THIS.fp3name = m.cAddname
	ENDPROC

	*----------------------
	PROCEDURE WriteName	&& fp25obj
	*----------------------
		THIS.AddProp(M_NAME,THIS.fp3name)
	ENDPROC

	*----------------------
	PROCEDURE AddFont	&& fp25obj
	*----------------------
		PARAMETER m.nbtn
		
		*- check for non-GUI platform
		IF !INLIST(a_scx2fld[A_PLATFORM],C_MAC,C_WINDOWS)
			RETURN
		ENDIF
	
		THIS.fp3font  = ALLTRIM(a_scx2fld[A_FONTFACE])
		THIS.fp3fsize = a_scx2fld[A_FONTSIZE]
		THIS.fp3fstyle = THIS.GetStyle(a_scx2fld[A_FONTSTYLE])
		
		THIS.fp3font1 = FONT(1,THIS.fp3font,THIS.fp3fsize,THIS.fp3fstyle)
		THIS.fp3font5 = FONT(5,THIS.fp3font,THIS.fp3fsize,THIS.fp3fstyle)
		THIS.fp3font6 = FONT(6,THIS.fp3font,THIS.fp3fsize,THIS.fp3fstyle)
		
		THIS.nDeffont1 = THIS.formRef.nDeffont1 	&& default screen font1
		THIS.nDeffont5 = THIS.formRef.nDeffont5 	&& default screen font5
		THIS.nDeffont6 = THIS.formRef.nDeffont6 	&& default screen font6

		*- Add fontface and fontsize
		THIS.AddProp(M_FONTFACE,THIS.fp3font,m.nbtn)
		THIS.AddProp(M_FONTSIZE,THIS.fp3fsize,m.nbtn)
		
		*- Add font styles
		
		*- Bold is default on some so always add it
		THIS.AddProp(M_FONTBOLD,ATC("B",THIS.fp3fstyle) # 0,m.nbtn)
		
		*- Italic
		IF ATC("I",THIS.fp3fstyle) # 0 
			THIS.AddProp(M_FONTITAL,C_TRUE,m.nbtn)
		ENDIF
		
		*- Underline
		IF ATC("U",THIS.fp3fstyle) # 0 
			THIS.AddProp(M_FONTUNDER,C_TRUE,m.nbtn)
		ENDIF

		IF _mac
			*- these attributes only exist on the Mac (12/5/95 jd)
			IF ATC("O",THIS.fp3fstyle) # 0 
				THIS.AddProp(M_FONTOUTLINE,C_TRUE,m.nbtn)
			ENDIF

			*- these attributes only exist on the Mac (12/5/95 jd)
			IF ATC("S",THIS.fp3fstyle) # 0 
				THIS.AddProp(M_FONTSHADOW,C_TRUE,m.nbtn)
			ENDIF

			*- these attributes only exist on the Mac (12/5/95 jd)
			IF ATC("C",THIS.fp3fstyle) # 0 
				THIS.AddProp(M_FONTCONDENSE,C_TRUE,m.nbtn)
			ENDIF

			*- these attributes only exist on the Mac (12/5/95 jd)
			IF ATC("E",THIS.fp3fstyle) # 0 
				THIS.AddProp(M_FONTEXTEND,C_TRUE,m.nbtn)
			ENDIF

		ENDIF

	ENDPROC
	
	*----------------------
	PROCEDURE AddPos		&& fp25obj
	*----------------------
	    *- Add object positions in pixels (how FP3 stores it)
		*- VPOS,HPOS based on form font
		THIS.AddProp(M_VPOS,a_scx2fld[A_VPOS] * (THIS.nDeffont1+THIS.nDeffont5))		
		THIS.AddProp(M_HPOS,(a_scx2fld[A_HPOS] - THIS.fp3fudge) * THIS.nDeffont6)
		
		*- HEIGHT,WIDTH based on object font
		THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH] * THIS.fp3font6)
		THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT] * ;
			(THIS.fp3font1+THIS.fp3font5))

	ENDPROC

	*----------------------
	PROCEDURE AddColor	&& fp25obj
	*----------------------
	    *- Add colorstuff
		*- Add pen color
		PARAMETER m.btn

		THIS.AddProp(M_COLORSOURCE, THIS.iColorSource, m.btn)		&& made colorsource value a property, so it is easier too override (jd 06/20/96)

		IF a_scx2fld[A_PENRED] # -1
			
			THIS.AddProp(M_PEN,ALLT(STR(a_scx2fld[A_PENRED])) + ;
				"," + ALLT(STR(a_scx2fld[A_PENGREEN])) + ;
				"," + ALLT(STR(a_scx2fld[A_PENBLUE])),m.btn)

		ENDIF
		
		*- Add fill color
		IF a_scx2fld[A_FILLRED] = -1
		ELSE				
			THIS.AddProp(M_BACKCOLOR,ALLT(STR(a_scx2fld[A_FILLRED]))+;
				","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_FILLBLUE])),m.btn)
			THIS.AddProp(M_FILLCOLOR,ALLT(STR(a_scx2fld[A_FILLRED]))+;
				","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_FILLBLUE])),m.btn)
			THIS.AddProp(M_DISBACKCOLOR,ALLT(STR(a_scx2fld[A_FILLRED]))+;
				","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_FILLBLUE])),m.btn)
		ENDIF
	ENDPROC			&& fp25obj
	
	*----------------------
	FUNCTION GetStyle	&& fp25obj
	*----------------------
		*- Takes font style in SCX/FRX file and converts to
		*- font style code used by FONTMETRIC functions.
		PARAMETER iFntStyle
		LOCAL cFontStyle, i
		cFontStyle = "N"
		m.cStyleCodes = "BIUOSCE"   && bold (1), italic (2), underline (4), outline (8), 
									&& shadow (16), condensed (32), extended (64)

		IF m.iFntStyle > 0
			*- has an attribute
			FOR i = 6 TO 0 STEP -1
				IF m.iFntStyle >= 2^i
					m.cFontStyle = m.cFontStyle + SUBSTR(m.cStyleCodes,i + 1,1)
					m.iFntStyle = m.iFntStyle - 2^i
				ENDIF
			NEXT 
		ENDIF

		RETURN m.cFontStyle

		*DO CASE
		*	CASE m.iFntStyle = 1		&& BOLD
		*		RETURN 'B'
		*	CASE m.iFntStyle = 2		&& ITALIC
		*		RETURN 'I'
		*	CASE m.iFntStyle = 3		&& BOLD ITALIC
		*		RETURN 'BI'
		*	OTHERWISE  					&& NORMAL
		*		RETURN 'N'
		*ENDCASE
	ENDFUNC 
	
	*----------------------
	FUNCTION GetNewName	&& fp25obj
	*----------------------
		PARAMETER newobj
		PRIVATE cName

		cName = ALLT(a_scx2fld[A_NAME]) 
		*- use data source if available
		IF !EMPTY(cName)
			*- name could be an array element, so strip out
			m.cName = CHRTRANC(m.cName,",()[]","_")
			IF " " $ cName
				*- space in name (e.g. "varname BITMAP") -- 
				*- for now, take everything up to it
				cName = LEFT(cName, AT(" ",cName) - 1)
				IF ("\" $ cName) OR ("." $ cName) OR (":" $ cName)
					*- assume is some kind of filename
					*- strip off quotes if they are there
					m.cName = JustStem(StripQuote(m.cname))
				ENDIF
			ELSE
				m.cName = IIF("->" $ cName,SUBSTR(cName,AT("->",cName) + 2),;
						SUBSTR(cName,AT(".",cName) + 1))
			ENDIF
		ELSE
			m.cName = m.newobj
		ENDIF
		THIS.formRef.nObjCount = THIS.formRef.nObjCount + 1
		RETURN THIS.GetVarPrefix(m.newobj) + PROPER(ALLTRIM(m.cName)) + ALLTRIM(STR(THIS.formRef.nObjCount))
	ENDFUNC		&&  GetNewName

	*----------------------
	PROCEDURE AddFormat		&& fp25obj
	*----------------------
		*- Add function codes - format
		DO CASE 
			CASE EMPTY(THIS.picword1)
				*- skip
			CASE !THIS.hasitse
				THIS.AddProp(M_FORMAT,THIS.addquotes(THIS.picword1))
			OTHERWISE
				THIS.AddProp(M_FORMAT,THIS.picword1)
		ENDCASE
		
		*- Add picture codes - inputmask
		DO CASE
			CASE EMPTY(THIS.picword2)
			CASE !THIS.hasitse 
				THIS.AddProp(M_INPUTMSK,THIS.addquotes(THIS.picword2))
			OTHERWISE
				THIS.AddProp(M_INPUTMSK,THIS.picword2)
		ENDCASE
	ENDPROC		&&  AddFormat
	
	*----------------------
	PROCEDURE AddFX		&& fp25obj
	*----------------------
		*- Add Special Effects
		*- support 3-D effects from FoxMac 2.6 (12/5/95 jd)
		PARAMETER btn
		IF a_scx2fld[A_PLATFORM] = C_MAC AND "3" $ THIS.picword1
			THIS.AddProp(M_SPECIAL,N_3D,m.btn)
		ELSE
			THIS.AddProp(M_SPECIAL,N_PLAIN,m.btn)
		ENDIF
	ENDPROC

	*----------------------
	PROCEDURE AddMode		&& fp25obj
	*----------------------
		*- add the mode (opaque/transparent)
		*- if mode has to be converted, check for opaque w/ no fill pat
		PARAMETER lConvert,nMode,nFillPat,m.btn
		IF !lConvert
			THIS.AddProp(M_MODE,nMode,m.btn)
		ELSE
			*- reverse transparent/opaque value in 3.0
			THIS.AddProp(M_MODE,ABS(nMode - 1),m.btn)
		ENDIF
	ENDPROC

	*----------------------
	PROCEDURE Conv2Str
	*----------------------
		PARAMETER pstring
		DO CASE
		CASE TYPE(m.pstring) = "L"
		    RETURN "IIF("+m.pstring+",'T','F')"
		CASE INLIST(TYPE(m.pstring),"N","F")
		    RETURN "ALLTRIM(STR("+m.pstring+"))"
		CASE TYPE(m.pstring) = "D"
		    RETURN "DTOS("+m.pstring+")"
		OTHERWISE  	&& don't index
			RETURN m.pstring
		ENDCASE
	ENDPROC

ENDDEFINE		&&  fp25obj 


************************************
DEFINE CLASS fp25ctrl AS fp25obj
************************************
		
	*----------------------
	PROCEDURE AddBasic		&& fp25ctrl
	*----------------------
		fp25obj::AddBasic
		THIS.fp3id = "~B"		&& we'll sort on this field later
	ENDPROC

	*----------------------
	PROCEDURE AddMain		&& fp25ctrl
	*----------------------
		*- Now add control specific properties
		THIS.AddName(THIS.GetNewName(THIS.fp3class))
		
		*- add mode
		THIS.AddMode(L_CONVERT,a_scx2fld[A_MODE],a_scx2fld[A_FILLPAT])

		IF !THIS.formref.noReadPlainExpr OR (a_scx2fld[A_WHENTYPE] == 0)
			THIS.AddMethods(M_WHEN2,THIS.FormRef.CleanProc(a_scx2fld[A_WHEN]),a_scx2fld[A_WHENTYPE])
		ENDIF
		IF !THIS.formref.noReadPlainExpr OR (a_scx2fld[A_VALIDTYPE] == 0)
			THIS.AddMethods(M_VALID2,THIS.FormRef.CleanProc(a_scx2fld[A_VALID]),a_scx2fld[A_VALIDTYPE])
		ENDIF
		IF !THIS.formref.noReadPlainExpr OR (a_scx2fld[A_MESSTYPE] == 0)
			THIS.AddMethods(M_MESSAGE,a_scx2fld[A_MESSAGE],a_scx2fld[A_MESSTYPE])
		ENDIF
		IF !THIS.formref.noReadPlainExpr OR (a_scx2fld[A_ERRORTYPE] == 0)
			THIS.AddMethods(M_ERROR,a_scx2fld[A_ERROR],a_scx2fld[A_ERRORTYPE])
		ENDIF
		IF !THIS.formRef.lHasSys16
			THIS.formRef.lHasSys16 = ("SYS(16" $ UPPER(THIS.fp3method))
		ENDIF

		*- Get parts of Picture clause for use below
		THIS.GetPicPart(a_scx2fld[A_PICTURE])

		*- add releaseerase
		THIS.AddProp(M_RELEASEERASE,C_FALSE)

		*- DO specific action for control
		THIS.AddCtrl

		*- Add datasource (field, memvar, etc.)
		*- This has to be added after Value, RowSource etc,
		THIS.AddProp(M_DATASOURCE,ALLTRIM(a_scx2fld[A_NAME]))

		*- add 3D special effects
		THIS.AddFX


	ENDPROC	&& AddMain
	
	*----------------------
	PROCEDURE AddCtrl		&& fp25ctrl
	*----------------------
				
		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

	ENDPROC
	
	*----------------------
	PROCEDURE AddGroup		&& fp25ctrl
	*----------------------
		PARAMETER btnname,invisbtn,optbtn
		
		PRIVATE i,capname,btn,vspc,hspc,lhaspicts,totbtns
		PRIVATE unitwid,unithgt,st_left,st_top,lishoriz
				
		m.totbtns = OCCUR(";",a_scx2fld[A_PICTURE]) + 1
		m.lhaspicts = ATC("B",THIS.picword1) # 0
		m.lishoriz =  ATC("H",THIS.picword1) # 0

		*- VPOS,HPOS based on form font
		THIS.AddProp(M_VPOS,a_scx2fld[A_VPOS] * (THIS.nDeffont1 + THIS.nDeffont5))		
		THIS.AddProp(M_HPOS,a_scx2fld[A_HPOS] * THIS.nDeffont6)

		IF m.lishoriz  && horizontal buttons
			THIS.AddProp(M_WIDTH,((a_scx2fld[A_WIDTH]+a_scx2fld[A_SPACING]) * ;
				m.totbtns*THIS.fp3font6)-(a_scx2fld[A_SPACING]*THIS.fp3font6))
			THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT] * ;
				(THIS.fp3font1+THIS.fp3font5))
			m.vspc = 0
			m.hspc = a_scx2fld[A_SPACING]*THIS.fp3font6
		ELSE
			THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH] * THIS.fp3font6)
			THIS.AddProp(M_HEIGHT,((a_scx2fld[A_HEIGHT] + a_scx2fld[A_SPACING]) * ;
				m.totbtns * (THIS.fp3font1 + THIS.fp3font5)) - ;
				(a_scx2fld[A_SPACING] * (THIS.fp3font1 + THIS.fp3font5)))
			m.vspc = a_scx2fld[A_SPACING] * (THIS.fp3font1 + THIS.fp3font5)
			m.hspc = 0
		ENDIF

		IF m.btnName = "Option"
			THIS.AddMode(L_NOCONVERT,0)  	&& always transparent
		ELSE
			THIS.AddMode(L_NOCONVERT,0)  	&& always transparent
		ENDIF

		THIS.AddProp(M_BORDER,0)			&& no auto-border around group

		THIS.AddProp(M_BUTTONS,m.totbtns)	&& number of buttons
		
		m.unitwid = a_scx2fld[A_WIDTH] * THIS.fp3font6
		m.unithgt = a_scx2fld[A_HEIGHT] * (THIS.fp3font1 + THIS.fp3font5)

		*- buttons are offset from their container
		m.st_top = 0
		m.st_left = 0		

		*- Add name here - must be last
		THIS.AddProp(M_NAME,THIS.fp3name)
		
		*- Add specific buttons detail
		FOR i = 1 TO m.totbtns 
			m.btn = m.btnname + ALLTRIM(STR(m.i))
			DO CASE
				CASE m.totbtns = 1	&& single button
					m.capname = THIS.picword2	
				CASE m.i = 1
					m.capname = LEFT(THIS.picword2,AT(";",THIS.picword2)-1)
				CASE m.i = m.totbtns
					m.capname = SUBSTR(THIS.picword2,RAT(";",THIS.picword2)+1)
				OTHERWISE
					m.pos1 = AT(";",THIS.picword2,m.i-1)+1
					m.pos2 = AT(";",THIS.picword2,m.i)
					m.capname = SUBSTR(THIS.picword2,m.pos1,m.pos2-m.pos1)				
			ENDCASE
			
			*- individual buttons have font attributes
			THIS.AddFont(m.btn)

			*- individual buttons need color props set
			THIS.AddColor(m.btn)

			*- make plain
			THIS.AddFX(m.btn)
			*- THIS.AddProp(M_SPECIAL,N_PLAIN,m.btn)

			DO CASE
				CASE m.invisbtn			&& invisible buttons
					THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED],m.btn)
					THIS.AddProp(M_STYLE,1,m.btn)
				CASE m.lhaspicts 		&& picture buttons
					THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED],m.btn)
					THIS.AddProp(M_FPICTURE,THIS.FullBMP(m.capname),m.btn)
					THIS.AddProp(M_CAPTION,[""],m.btn)
					IF m.optbtn
						THIS.AddProp(M_STYLE,1,m.btn)
					ENDIF
				OTHERWISE
					IF "\\" $ m.capname
						*- disabled, so strip out double backslash
						m.capname = STRTRAN(m.capname,"\\")
						THIS.AddProp(M_ENABLED,.F.,m.btn)
					ELSE
						THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED],m.btn)
					ENDIF
					IF "\!" $ m.capname
						*- default, so strip out and set property
						m.capname = STRTRAN(m.capname,"\!")
						THIS.AddProp(M_DEFAULT,.T.,m.btn)
					ENDIF
					IF "\?" $ m.capname
						*- escape/cancel, so strip out and set property
						m.capname = STRTRAN(m.capname,"\?")
						THIS.AddProp(M_CANCEL,.T.,m.btn)
					ENDIF
					THIS.AddProp(M_CAPTION,THIS.addquotes(m.capname),m.btn)
			ENDCASE
			

			*- Add mode (Opaque,Trans), and other attributes peculiar to btnName
			IF m.btnName = "Option"
				*-THIS.AddMode(L_NOCONVERT,0,a_scx2fld[A_FILLPAT],m.btn)  	&& always transparent
				THIS.AddMode(L_CONVERT,a_scx2fld[A_MODE],a_scx2fld[A_FILLPAT],m.btn)
				*- Also add value for specific button
				IF THIS.iValue = i
					THIS.AddProp(M_VALUE,1,m.btn)
				ENDIF
			ELSE
				THIS.AddMode(L_CONVERT,ABS(a_scx2fld[A_MODE]-1),a_scx2fld[A_FILLPAT],m.btn)
			ENDIF


			THIS.AddProp(M_HEIGHT,m.unithgt,m.btn)
			THIS.AddProp(M_WIDTH,m.unitwid,m.btn)
			
			IF m.lishoriz  &&horizontal buttons
				THIS.AddProp(M_HPOS,m.st_left+;
					((m.hspc+m.unitwid)*(m.i-1)),m.btn)
				THIS.AddProp(M_VPOS,m.st_top,m.btn)
			ELSE
				THIS.AddProp(M_HPOS,m.st_left,m.btn)
				THIS.AddProp(M_VPOS,m.st_top+;
					((m.vspc+m.unithgt)*(m.i-1)),m.btn)
			ENDIF
			
			IF ATC("T",THIS.picword1) # 0	&& terminate read
				THIS.AddProp(M_TERMINATEREAD,.T.,m.btn)
			ELSE
				THIS.AddProp(M_TERMINATEREAD,.F.,m.btn)
			ENDIF

			*- add releaseerase
			THIS.AddProp(M_RELEASEERASE,C_FALSE,m.btn)

			THIS.AddProp(M_NAME,m.btn,m.btn)

		ENDFOR
	ENDPROC			&& fp25ctrl:AddGroup

	*----------------------
	PROCEDURE AddBtn		&& fp25ctrl
	*----------------------
		IF ATC("B",THIS.picword1) # 0	&& pictures
			THIS.AddProp(M_FPICTURE,THIS.FullBMP(THIS.picword2))
			THIS.AddProp(M_CAPTION,[""])
		ELSE
			THIS.AddProp(M_CAPTION,THIS.addquotes(THIS.picword2))
		ENDIF
		
		IF ATC("T",THIS.picword1) # 0	&& terminate read
			THIS.AddProp(M_TERMINATEREAD,.T.)
		ELSE
			THIS.AddProp(M_TERMINATEREAD,.F.)
		ENDIF

	ENDPROC	&& AddBtn
	
	*----------------------
	PROCEDURE AddValue	&& fp25ctrl
	*----------------------
		*- This routine sets Value property to DEFAULT 
		*- setting for a Textbox,Editbox or Spinner 
		*- control similar to GENDEFAULT in GENSCRN.
		
		PRIVATE ctempstr,cfillchar,cinitval
		
		m.cfillchar = a_scx2fld[A_FILLCHAR]
		m.cinitval = TRIM(a_scx2fld[A_INITIALVAL])
		
		IF EMPTY(m.cinitval) AND EMPTY(m.cfillchar)
   			RETURN
		ENDIF
		
		IF EMPTY(m.cinitval)
			DO CASE
				CASE m.cfillchar = "D"
	      			m.ctempstr = {  /  /  }
				CASE m.cfillchar = "C" OR fillchar = "M" OR fillchar = "G"
	    			RETURN
				CASE m.cfillchar = "L"
	    			m.ctempstr = .F.
				CASE m.cfillchar = "N"
	  				m.ctempstr = 0
				CASE m.cfillchar= "F"
					m.ctempstr = 0.0
			ENDCASE
		ELSE
			*- this only occurs with a spinner
			m.ctempstr = m.cinitval
		ENDIF
		THIS.AddProp(M_VALUE,m.ctempstr)
	ENDPROC			&& fp25ctrl:AddValue

ENDDEFINE		&&  fp25ctrl


************************************
DEFINE CLASS fp25list AS fp25ctrl
************************************

	*----------------------
	PROCEDURE AddCtrl	&& fp25list	
	*----------------------

		THIS.AddProp(M_EXPR,THIS.addquotes(a_scx2fld[A_EXPR]))

		*- add style specific stuff
		DO CASE
			CASE a_scx2fld[A_STYLE] = 0		&& array
				THIS.AddMethods(M_RANGE2LO,a_scx2fld[A_RANGELO],a_scx2fld[A_LOTYPE],M_1STELEMENT)
				THIS.AddMethods(M_RANGE2HI,a_scx2fld[A_RANGEHI],a_scx2fld[A_HITYPE],M_NUMELEMENTS)
				THIS.AddProp(M_LSTYLE,5)
				THIS.AddProp(M_VALUE,1)		&& set a default value, just in case
			CASE a_scx2fld[A_STYLE] = 1		&& popup
				THIS.AddProp(M_LSTYLE,9)
				THIS.AddProp(M_VALUE,1)		&& set a default value, just in case
			CASE a_scx2fld[A_STYLE] = 2		&& DBF structure
				THIS.AddProp(M_LSTYLE,8)
				THIS.AddProp(M_VALUE,[" "])	&& set a default value, just in case
			CASE a_scx2fld[A_STYLE] = 3		&& field
				THIS.AddProp(M_LSTYLE,6)
				THIS.AddProp(M_VALUE,[" "])	&& set a default value, just in case
			CASE a_scx2fld[A_STYLE] = 4		&& file skeleton
				THIS.AddProp(M_LSTYLE,7)
				THIS.AddProp(M_VALUE,[" "])	&& set a default value, just in case
		ENDCASE

		IF ATC("T",THIS.picword1) # 0	&& terminate read
			THIS.AddProp(M_TERMINATEREAD,.T.)
		ELSE
			THIS.AddProp(M_TERMINATEREAD,.F.)
		ENDIF

		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

	ENDPROC

	*----------------------
	PROCEDURE AddPos	&& fp25list
	*----------------------
		*- Lists need a ReadSize property, before top, left, height, width
		*- THIS.AddProp(M_READSIZE,.T.)
		THIS.fp3fudge = .2
	    *- Add object positions in pixels (how FP3 stores it)
		*- VPOS,HPOS based on form font
		THIS.AddProp(M_VPOS,a_scx2fld[A_VPOS] * (THIS.nDeffont1+THIS.nDeffont5))		
		THIS.AddProp(M_HPOS,(a_scx2fld[A_HPOS] - THIS.fp3fudge) * THIS.nDeffont6)
		
		*- HEIGHT,WIDTH based on object font
		THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH] * THIS.fp3font6)
		THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT] * ;
			(THIS.fp3font1+THIS.fp3font5) + 2)			&& the extra 2 is for the border, which wasn't included in 2.6 listbox height

	ENDPROC

	*----------------------
	PROCEDURE AddColor	&& fp25list
	*----------------------
		*- lists use different properties for colors
	    *- Add colorstuff
		*- Add pen color
		PARAMETER m.btn

		THIS.AddProp(M_COLORSOURCE,I_DEFCOLORSOURCE,m.btn)

		IF a_scx2fld[A_PENRED] # -1

			THIS.AddProp(M_ITEMFORECOLOR,ALLT(STR(a_scx2fld[A_PENRED]))+;
				","+ALLT(STR(a_scx2fld[A_PENGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_PENBLUE])))
		ENDIF
		
		*- Add fill color
		IF a_scx2fld[A_FILLRED] = -1
		ELSE				
			THIS.AddProp(M_ITEMBACKCOLOR,ALLT(STR(a_scx2fld[A_FILLRED]))+;
				","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_FILLBLUE])))
			THIS.AddProp(M_DISITEMBACKCOLOR,ALLT(STR(a_scx2fld[A_FILLRED]))+;
				","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_FILLBLUE])))
		ENDIF

	ENDPROC

ENDDEFINE


************************************
DEFINE CLASS fp25btn AS fp25ctrl
************************************

	*----------------------
	PROCEDURE AddCtrl
	*----------------------
		THIS.AddBtn
		
		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

	ENDPROC

ENDDEFINE


************************************
DEFINE CLASS fp25cbox AS fp25ctrl
************************************

	*----------------------
	PROCEDURE AddCtrl
	*----------------------
		THIS.AddBtn
		
		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

		IF ATC("B",THIS.picword1) # 0	&& pictures
			THIS.AddProp(M_STYLE,1)
		ELSE
			THIS.AddProp(M_STYLE,0)
		ENDIF
		
		THIS.AddProp(M_VALUE,a_scx2fld[A_INITIALNUM])
	ENDPROC

ENDDEFINE


************************************
DEFINE CLASS fp25btngrp AS fp25ctrl
************************************
	
	PROCEDURE AddPos
	ENDPROC

	PROCEDURE WriteName
	ENDPROC
	
	PROCEDURE AddCtrl
		THIS.AddGroup("Command")
	ENDPROC

	*----------------------
	PROCEDURE AddColor			&& fp25btngrp
	*----------------------
		PARAMETER m.btn

		*- buttons are colorless things

		*- but they still need a colorsource
		THIS.AddProp(M_COLORSOURCE,0,m.btn)

	ENDPROC


ENDDEFINE


***************************************
DEFINE CLASS fp25invgrp AS fp25ctrl
***************************************
	
	PROCEDURE AddPos
	ENDPROC

	PROCEDURE WriteName
	ENDPROC
	
	PROCEDURE AddCtrl
		THIS.AddGroup("Command",.T.)
		THIS.AddProp(M_VALUE,0)					&& set the default value
	ENDPROC

ENDDEFINE


***************************************
DEFINE CLASS fp25invbtn AS fp25ctrl
***************************************

	*----------------------
	PROCEDURE AddCtrl
	*----------------------
		THIS.AddBtn
		
		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

		THIS.AddProp(M_STYLE,1)
	ENDPROC

ENDDEFINE


************************************
DEFINE CLASS fp25get AS fp25ctrl
************************************
	*----------------------
	PROCEDURE AddMain		&& fp25get
	*----------------------

		fp25ctrl::AddMain

		*- Add alignment
		DO CASE
			CASE ATC("B",THIS.picword1) # 0				&& left - num type
				THIS.AddProp(M_ALIGN,0)
			CASE ATC("J",THIS.picword1) # 0				&& right - char type
				THIS.AddProp(M_ALIGN,1)
			CASE ATC("I",THIS.picword1) # 0				&& center - char type
				THIS.AddProp(M_ALIGN,2)
			CASE INLIST(TYPE(a_scx2fld[A_EXPR]),"N")	&& num, so make right just	
				THIS.AddProp(M_ALIGN,1)
		ENDCASE
	
		IF !EMPTY(THIS.picword3)
			*- check for color, color scheme, size, etc.
		ENDIF		

	ENDPROC	&& AddMain

	*----------------------
	PROCEDURE AddCtrl		&& fp25get
	*----------------------
		THIS.AddFormat
		THIS.AddValue
		
		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

		THIS.AddProp(M_MARGIN,0)	&& new 3.0 prop

		THIS.AddMethods(M_RANGE2LO,a_scx2fld[A_RANGELO],a_scx2fld[A_LOTYPE])	&& ,M_RANGELO)
		THIS.AddMethods(M_RANGE2HI,a_scx2fld[A_RANGEHI],a_scx2fld[A_HITYPE])	&& ,M_RANGEHI)
		THIS.AddProp(M_PENPAT,IIF(THIS.formRef.GetBorder,1,0))

		*- check for any #ITSE expressions

		IF !EMPTY(THIS.picword3)
			*- additional checks
		ENDIF
	ENDPROC
	
	*----------------------
	PROCEDURE AddPos		&& fp25get
	*----------------------
		*- Add object positions in pixels (how Taz stores it)
		*- FP2.5 added extra pixels for some reason
		*- VPOS = -1, HPOS = -2, WIDTH = 5,HEIGHT = 2
		*- VPOS,HPOS based on form font
		THIS.AddProp(M_VPOS, a_scx2fld[A_VPOS] * ;
			(THIS.nDeffont1 + THIS.nDeffont5) - 1)		
		THIS.AddProp(M_HPOS, a_scx2fld[A_HPOS] * THIS.nDeffont6 - 2)
		
		*- HEIGHT,WIDTH based on object font
		THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH] * THIS.fp3font6 + 5)
		THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT] * ;
				(THIS.fp3font1+THIS.fp3font5) + 2)
	ENDPROC

ENDDEFINE && fp25get


************************************
DEFINE CLASS fp25text AS fp25ctrl
************************************
	*- superclass for textboxes and says

	*----------------------
	PROCEDURE AddMain		&& fp25text
	*----------------------

		fp25ctrl::AddMain

		*- Add alignment
		DO CASE
			CASE ATC("B",THIS.picword1) # 0				&& left - num type
				THIS.AddProp(M_ALIGN,0)
			CASE ATC("J",THIS.picword1) # 0				&& right - char type
				THIS.AddProp(M_ALIGN,1)
			CASE ATC("I",THIS.picword1) # 0				&& center - char type
				THIS.AddProp(M_ALIGN,2)
			CASE INLIST(TYPE(a_scx2fld[A_EXPR]),"N")	&& num, so make right just	
				THIS.AddProp(M_ALIGN,1)
		ENDCASE
	
		IF !EMPTY(THIS.picword3)
			*- check for color, color scheme, size, etc.
		ENDIF		

	ENDPROC	&& AddMain

	*----------------------
	PROCEDURE AddCtrl		&& fp25text
	*----------------------
		THIS.AddFormat
		THIS.AddValue

		THIS.AddProp(M_MARGIN,0)	&& new 3.0 prop

	ENDPROC

	*----------------------
	PROCEDURE AddPos		&& fp25text
	*----------------------
		*- Add object positions in pixels (how Taz stores it)
		*- FP2.5 added extra pixels for some reason
		*- VPOS = -1, HPOS = -2, WIDTH = 2,HEIGHT = 2
		*- VPOS,HPOS based on form font
		THIS.AddProp(M_VPOS,a_scx2fld[A_VPOS] * ;
			(THIS.nDeffont1 + THIS.nDeffont5)-1)		
		THIS.AddProp(M_HPOS,a_scx2fld[A_HPOS] * THIS.nDeffont6-2)
		
		*- HEIGHT,WIDTH based on object font
		THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH] * THIS.fp3font6+2)
		THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT] * ;
				(THIS.fp3font1 + THIS.fp3font5) + 2)
	ENDPROC


ENDDEFINE	&& fp25text

************************************
DEFINE CLASS fp25edit AS fp25text
************************************

	*----------------------
	PROCEDURE AddCtrl
	*----------------------
		fp25text::AddCtrl

		IF !a_scx2fld[A_SCROLLBAR]
			THIS.AddProp(M_SCROLLBAR,0)
		ENDIF
		
		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

		THIS.AddProp(M_TAB,a_scx2fld[A_TAB])
		*-THIS.AddProp(M_MAXLEN,a_scx2fld[A_INITIALNUM])
		*-THIS.AddProp(M_PENPAT,IIF(THIS.formRef.GetBorder,1,0))
		*- check if person has .T. NOMODIFY kludge
		IF  ATC(" NOMO",THIS.fp3method) # 0 OR ;
			ATC(" NOMO",THIS.picword3) # 0
			*- comment out NOMODIFY in methods. Leave alone in picture clause
			THIS.fp3method = STRTRAN(THIS.fp3method," NOMO"," &" + "& NOMO")
			THIS.AddProp(M_READONLY,C_TRUE)
		ENDIF
	ENDPROC

	*----------------------
	PROCEDURE AddValue	&& fp25edit
	*----------------------
		*- This routine sets Value property to DEFAULT 
		*- Editbox. Overrides the fp25ctrl:AddValue
		*- 2.6 allowed a numeric type for the editbox, which is not allowed in 3.0
		
		RETURN
		
	ENDPROC			&& fp25edit:AddValue

ENDDEFINE && fp25edit

************************************
DEFINE CLASS fp25say AS fp25text
************************************
	*- a read-only descendent of fp25text

	*----------------------
	PROCEDURE AddCtrl
	*----------------------
		fp25text::AddCtrl

		THIS.AddProp(M_STYLE,1)			&& should set behavior like 2.6 SAY
		THIS.AddProp(M_READONLY,.T.)	&& can't change it
		THIS.AddProp(M_TABSTOP,.F.)		&& can't tab into it
		*-THIS.AddProp(M_ENABLED,.F.)		&& disabled

		THIS.AddProp(M_PENPAT,0)		&& no border

	ENDPROC

	*----------------------
	FUNCTION GetNewName	&& fp25say
	*----------------------
		*- use as much of EXPR as possible
		PARAMETER newobj
		LOCAL cName

		m.cName = ALLT(StripQuote(ALLT(a_scx2fld[A_EXPR])))
		IF !EMPTY(m.cName)
			m.cName = LEFT(m.cName,MIN(LEN(m.cName),8))
			m.newobj = THIS.GetVarPrefix(newobj) + ALLT(PROPER(GoodName(m.cName)))
		ELSE
			m.newobj = THIS.GetVarPrefix(newobj) + PROPER(m.newobj)
		ENDIF
		THIS.formRef.nObjCount = THIS.formRef.nObjCount + 1
		RETURN ALLTRIM(m.newobj) + ALLTRIM(STR(THIS.formRef.nObjCount))
	ENDFUNC		&&  GetNewName

	*----------------------
	PROCEDURE AddColor	&& fp25say
	*----------------------
	    *- Add colorstuff
		*- Add pen color
		PARAMETER m.btn

		THIS.AddProp(M_COLORSOURCE,I_DEFCOLORSOURCE)

		*- since SAYs are handled as disabled textboxes, make the disabled colors and
		*- the enabled colors the same as the 2.6 colors
		IF a_scx2fld[A_PENRED] = -1
		ELSE
			THIS.AddProp(M_DISFORECOLOR,ALLT(STR(a_scx2fld[A_PENRED]))+;
				","+ALLT(STR(a_scx2fld[A_PENGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_PENBLUE])))
			THIS.AddProp(M_PEN,ALLT(STR(a_scx2fld[A_PENRED]))+;
				","+ALLT(STR(a_scx2fld[A_PENGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_PENBLUE])))
		ENDIF

		IF a_scx2fld[A_FILLRED] = -1
		ELSE
			THIS.AddProp(M_BACKCOLOR,ALLT(STR(a_scx2fld[A_FILLRED]))+;
				","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_FILLBLUE])))
		ENDIF

	ENDPROC

	*----------------------
	PROCEDURE AddValue		&& fp25say
	*----------------------
		LOCAL m.cExpr, m.cParent

		DO CASE
			CASE !EMPTY(a_scx2fld[A_EXPR]) AND ;
				AT(LEFT(a_scx2fld[A_EXPR],1),["[']) # 0
				m.cExpr = a_scx2fld[A_EXPR]
			CASE !INLIST(TYPE(a_scx2fld[A_EXPR]),"C","M","U","N")
				m.cExpr = "(" + THIS.Conv2Str(a_scx2fld[A_EXPR]) + ")"
			OTHERWISE
				m.cExpr = a_scx2fld[A_EXPR]
		ENDCASE

		THIS.AddProp(M_VALUE,m.cExpr)

		IF a_scx2fld[A_REFRESH]
			*- add to READSHOW method of formset
			IF THIS.FormRef.lIndirectWinName
				*- need to slip in ref to the new, changed form name
				m.cParent = "THISFORMSET." + CHR(38) + THIS.FormRef.cIndirectWinName + ".." + ;
					SUBS(THIS.fp3parent,AT(".",THIS.fp3parent,2) + 1)
			ELSE
				m.cParent = "THISFORMSET." + ;
					SUBS(THIS.fp3parent,AT(".",THIS.fp3parent,1) + 1)
			ENDIF

			THIS.formRef.cReadShow = THIS.formRef.cReadShow + ;
				m.cParent + "." + THIS.fp3name + ".Value = " + ;
				m.cExpr + C_CRLF
		ENDIF

	ENDPROC

ENDDEFINE && fp25say

************************************
DEFINE CLASS fp25option AS fp25ctrl
************************************
*- this class allows both radios and popups to inherit this AddValue

	iValue = 0				&& need to remember the value, so the individual option button value can be set

	*----------------------
	PROCEDURE AddValue
	*----------------------
		*- This routine sets Value property to DEFAULT 
		*- setting for a popup/combobox
		*- control similar to GENDEFAULT in GENSCRN.
		
		LOCAL ctempval,cinitval,ctemp2
		
		m.cinitval = ALLT(a_scx2fld[A_INITIALVAL])
		m.ctemp2 = SUBS(m.cinitval,2,LEN(m.cinitval) - 2)
		
		DO CASE
			CASE EMPTY(m.cinitval)
				m.ctempval = 1
			CASE ALLT(STR(VAL(m.cinitval))) = m.cinitval
				*- is a number
				m.ctempval = VAL(m.cinitval)
			CASE THIS.fp25OC = 3
				*- from array -- set to 1
				m.ctempval = 1
			OTHERWISE
				*- text -- do without the quotes
				m.ctempval = m.cinitval
		ENDCASE
		
		THIS.AddProp(M_VALUE,m.ctempval)
		THIS.iValue = m.ctempval

	ENDPROC	&& AddValue

ENDDEFINE	&& fp25option


************************************
DEFINE CLASS fp25radio AS fp25option
************************************
	
	PROCEDURE AddPos
	ENDPROC

	PROCEDURE WriteName
	ENDPROC

	*----------------------
	PROCEDURE AddMain		&& fp25radio
	*----------------------

		fp25option::AddMain

		THIS.AddGroup("Option",.F.,.T.)		&& Add button groups AFTER ControlSource is set

	ENDPROC


	*----------------------
	PROCEDURE AddCtrl		&& fp25radio
	*----------------------
		*- set initial value
		
		*- set group Enabled property to .T., and individual buttons to whatever
		THIS.AddProp(M_ENABLED,.T.)

		IF ATC("B",THIS.picword1) = 0	&& text radios
			THIS.AddProp(M_VALUE,a_scx2fld[A_INITIALNUM])
			THIS.iValue = a_scx2fld[A_INITIALNUM]
		ENDIF

	ENDPROC

	*----------------------
	PROCEDURE AddMode		&& fp25radio
	*----------------------
		*- add the mode (opaque/transparent)
		*- if mode has to be converted, check for opaque w/ no fill pat
		PARAMETER lConvert,nMode,nFillPat,m.btn

		IF a_scx2fld[A_FILLRED] = -1
			*- force transparent mode if auto color
			THIS.AddProp(M_MODE,N_TRANSPARENT,m.btn)
		ELSE
			fp25option::AddMode(lConvert,nMode,nFillPat,m.btn)
		ENDIF
	ENDPROC

ENDDEFINE	&& fp25radio


************************************
DEFINE CLASS fp25popup AS fp25option
************************************

	*----------------------
	PROCEDURE AddCtrl
	*----------------------
		LOCAL cListSource
		
		THIS.AddProp(M_STYLE,2)
		
		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

		*- combo box items must appear in the following order:
		*- rowSource, rowSourceType, value, dataSource
		IF THIS.fp25OC = 1  && list popups
			THIS.AddProp(M_1STELEMENT,a_scx2fld[A_INITIALNUM])
			m.cListSource = STRTRAN(STRTRAN(THIS.picword2,",","."),";",",")	&& FP 3.0 delimits popups with commas,
			THIS.AddProp(M_EXPR,THIS.addquotes(m.cListSource))				&& so attempt to preserve commas that are there
			THIS.AddProp(M_LSTYLE,1)										&& Style = VALUE (1)
		ELSE  				&& array popups
			THIS.AddMethods(M_RANGE2LO,a_scx2fld[A_RANGELO],a_scx2fld[A_LOTYPE],M_1STELEMENT)
			THIS.AddMethods(M_RANGE2HI,a_scx2fld[A_RANGEHI],a_scx2fld[A_HITYPE],M_NUMELEMENTS)
			THIS.AddProp(M_LSTYLE,5)
			THIS.AddProp(M_EXPR,THIS.addquotes(a_scx2fld[A_EXPR]))
		ENDIF
		
		THIS.AddValue

	ENDPROC

ENDDEFINE	&& fp25popup

************************************
DEFINE CLASS fp25spin AS fp25ctrl
************************************

	PROCEDURE AddCtrl
		THIS.AddFormat
		THIS.AddValue
		
		THIS.AddProp(M_ENABLED,!a_scx2fld[A_DISABLED])

		THIS.AddProp(M_MARGIN,0)	&& new 3.0 prop

		THIS.AddProp(M_ALIGN,1)		&& force right align

		*- only add spinner high and low values if they have been set
		IF !EMPTY(a_scx2fld[A_TAG])
			THIS.AddProp(M_SPINLO,a_scx2fld[A_TAG])
		ENDIF
		IF !EMPTY(a_scx2fld[A_TAG2])
			THIS.AddProp(M_SPINHI,a_scx2fld[A_TAG2])
		ENDIF

		THIS.AddMethods(M_RANGE2LO,a_scx2fld[A_RANGELO],a_scx2fld[A_LOTYPE],M_KEYLO)
		THIS.AddMethods(M_RANGE2HI,a_scx2fld[A_RANGEHI],a_scx2fld[A_HITYPE],M_KEYHI)
	ENDPROC

	*----------------------
	PROCEDURE AddPos		&& fp25spin
	*----------------------
	    *- Add object positions in pixels (how FP3 stores it)
		*- VPOS,HPOS based on form font
		THIS.AddProp(M_VPOS,a_scx2fld[A_VPOS]*(THIS.nDeffont1+THIS.nDeffont5))		
		THIS.AddProp(M_HPOS,(a_scx2fld[A_HPOS] - .6) * THIS.nDeffont6)
		
		*- HEIGHT,WIDTH based on object font
		*- Spinner needs extra 19 pixels width for spinner control
		*-               extra  6 pixels height
		THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH] * THIS.fp3font6 + 19)
		THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT] * ;
				(THIS.fp3font1+THIS.fp3font5) + 7)
	ENDPROC

	*----------------------
	PROCEDURE AddValue	&& fp25spin
	*----------------------
		*- This routine sets Value property to DEFAULT 
		
		LOCAL cinitval

		IF !EMPTY(a_scx2fld[A_INITIALVAL])
			IF INT(VAL(a_scx2fld[A_INITIALVAL])) <> VAL(a_scx2fld[A_INITIALVAL])
				cinitval = ForceDec(a_scx2fld[A_INITIALVAL],;
					LEN(a_scx2fld[A_INITIALVAL]) - AT('.',a_scx2fld[A_INITIALVAL]))
			ELSE
				cinitval = ForceDec(a_scx2fld[A_INITIALVAL],3)
			ENDIF
		ELSE
			cinitval = "1.000"
		ENDIF
				
		THIS.AddProp(M_SPININC,cinitval)

		DO CASE
			CASE !EMPTY(TAG)
				m.cinitval = ForceDec(TRIM(a_scx2fld[A_TAG]),3)
				THIS.AddProp(M_VALUE,m.cinitval)
			CASE !EMPTY(tag2)
				m.cinitval = ForceDec(TRIM(a_scx2fld[A_TAG2]),3)
				THIS.AddProp(M_VALUE,m.cinitval)
			CASE EMPTY(TRIM(initialval))
				m.cinitval = "1"
				THIS.AddProp(M_VALUE,m.cinitval)
			OTHERWISE
			   fp25ctrl::AddValue
		ENDCASE

   		RETURN
		
ENDDEFINE


************************************
DEFINE CLASS fp25lbl AS fp25obj
************************************
*- Text objects
	
	*----------------------
	PROCEDURE AddMain		&& fp25lbl
	*----------------------

		*- Add label specific properties
		THIS.AddName(THIS.GetNewName(THIS.fp3class))

		*- Get parts of Picture clause for use below
		THIS.GetPicPart(a_scx2fld[A_PICTURE])

		IF THIS.fp25OT = 15	&& @..SAY
			DO CASE
				CASE !EMPTY(a_scx2fld[A_EXPR]) AND ;
					AT(LEFT(a_scx2fld[A_EXPR],1),["[']) # 0
					THIS.AddProp(M_CAPTION,a_scx2fld[A_EXPR])
				CASE !INLIST(TYPE(a_scx2fld[A_EXPR]),"C","M","U")
					THIS.AddProp(M_CAPTION,THIS.Conv2Str(a_scx2fld[A_EXPR]))
				OTHERWISE
					THIS.AddProp(M_CAPTION,a_scx2fld[A_EXPR])
			ENDCASE
			
			THIS.picword2 = ""
			THIS.AddFormat
		ELSE
			THIS.AddProp(M_CAPTION,STRTRAN(a_scx2fld[A_EXPR],CHR(13),'" + CHR(13) + "'))
		ENDIF

		THIS.AddProp(M_COLORSOURCE,I_DEFCOLORSOURCE)

		*- Add mode (Opaque,Trans)
		THIS.AddMode(L_CONVERT,ABS(a_scx2fld[A_MODE]),1)	&& labels have no fill

		*- Add alignment
		DO CASE
			CASE ATC("B",THIS.picword1) # 0	&& left - num type
				THIS.AddProp(M_ALIGN,0)
			CASE ATC("J",THIS.picword1) # 0	&& right - char type
				THIS.AddProp(M_ALIGN,1)
			CASE ATC("I",THIS.picword1) # 0	&& center - char type
				THIS.AddProp(M_ALIGN,2)
		ENDCASE
	
		IF !EMPTY(THIS.picword3)
			*- check for color, color scheme, size, etc.
		ENDIF		

		*- add releaseerase
		THIS.AddProp(M_RELEASEERASE,C_FALSE)

	ENDPROC	&& AddMain

	*----------------------
	FUNCTION GetNewName	&& fp25lbl
	*----------------------
		*- use as much of EXPR as possible
		PARAMETER newobj
		LOCAL cName

		m.cName = ALLT(StripQuote(ALLT(a_scx2fld[A_EXPR])))
		IF !EMPTY(m.cName)
			m.cName = LEFT(m.cName,MIN(LEN(m.cName),8))
			m.newobj = THIS.GetVarPrefix(m.newobj) + PROPER(GoodName(m.cName))
		ELSE
			m.newobj = THIS.GetVarPrefix(m.newobj) + PROPER(m.newobj)
		ENDIF
		THIS.formRef.nObjCount = THIS.formRef.nObjCount + 1
		RETURN ALLTRIM(m.newobj) + ALLTRIM(STR(THIS.formRef.nObjCount))
	ENDFUNC		&&  GetNewName


ENDDEFINE && fp25lbl


************************************
DEFINE CLASS fp25shape AS fp25obj
************************************

	*----------------------
	PROCEDURE AddBasic		&& fp25shape
	*----------------------
		fp25obj::AddBasic
		THIS.fp3id = SYS(2015)		&& we'll sort on this field later
	ENDPROC


	*- support 3-d from FoxMac 2.6 (12/5/95 jd)
	*----------------------
	PROCEDURE AddFX		&& fp25shape
	*----------------------
		*- Add Special Effects
		PARAMETER btn
		IF a_scx2fld[A_PLATFORM] = C_MAC AND a_scx2fld[A_PENPAT] == 100 AND a_scx2fld[A_PENSIZE] == 2
			a_scx2fld[A_PENSIZE] = 1		&& change this so new crame looks right
			THIS.AddProp(M_SPECIAL,N_3D)
		ELSE
			THIS.AddProp(M_SPECIAL,N_PLAIN)
		ENDIF
	ENDPROC

	*----------------------
	PROCEDURE AddMain		&& fp25shape
	*----------------------

		LOCAL m.nFillPat

		*- Now add shape specific properties
		THIS.AddName(THIS.GetNewName(THIS.fp3class))
		
		*- add 3-d if necessary -- do this right away, because it might
		*- change the PENSIZE value
		THIS.AddFx

		IF THIS.fp25OT = 7		&& normal boxes
			DO CASE
				CASE a_scx2fld[A_FILLPAT] = 0
					m.nFillPat = 1
				CASE a_scx2fld[A_FILLPAT] = 1
					m.nFillPat = 0
				CASE a_scx2fld[A_FILLPAT] = 4
					m.nFillPat = 5
				CASE a_scx2fld[A_FILLPAT] = 5
					m.nFillPat = 4
				OTHERWISE
					m.nFillPat = a_scx2fld[A_FILLPAT]
			ENDCASE

			THIS.AddProp(M_FILLPAT,m.nFillPat)
			IF m.nFillPat = 1				&& FillStyle 1 = transparent/none, 0 = opaque/solid
				*- If FillPat is transparent, also set BackStyle to transparent
				THIS.AddProp(M_MODE,0)		&& BackStyle 0 = transparent/none, 1 = opaque/solid
			ELSE
				*- Add mode (Opaque,Trans)
				THIS.AddMode(L_CONVERT,ABS(a_scx2fld[A_MODE]),a_scx2fld[A_FILLPAT])
			ENDIF
		ELSE
				*- Add mode (Opaque,Trans)
				THIS.AddMode(L_CONVERT,ABS(a_scx2fld[A_MODE]),a_scx2fld[A_FILLPAT])
		ENDIF		

		*- Add rounded rectangle stuff
		IF	a_scx2fld[A_STYLE] >= 12
			*THIS.AddProp(M_SHAPE,4)
			THIS.AddProp(M_CURVE,a_scx2fld[A_STYLE])
		ENDIF

		*- Add pen width
		THIS.AddProp(M_PENSIZE,MAX(a_scx2fld[A_PENSIZE],1))
		
		*- Add pen pattern
		DO CASE
			CASE a_scx2fld[A_PENPAT] = 8	&& solid
				*- default of 8 in 2.6/1 in VFP, so no need to add
			CASE a_scx2fld[A_PENPAT] = 1	&& dotted
				THIS.AddProp(M_PENPAT,3)			
			CASE a_scx2fld[A_PENPAT] = 2	&& dashed
				THIS.AddProp(M_PENPAT,2)			
			CASE a_scx2fld[A_PENPAT] = 3	&& dash dot
				THIS.AddProp(M_PENPAT,4)			
			CASE a_scx2fld[A_PENPAT] = 4	&& dash dot dot
				THIS.AddProp(M_PENPAT,5)		
			CASE a_scx2fld[A_PENPAT] = 0	&& "none"
				THIS.AddProp(M_PENPAT,0)
		ENDCASE

		*- add ReleaseErase
		THIS.AddProp(M_RELEASEERASE,C_FALSE)
		
	ENDPROC

	*----------------------
	PROCEDURE AddFont
	*----------------------
	ENDPROC
	
	*----------------------
	PROCEDURE AddPos		&& fp25shape
	*----------------------
		*- VPOS,HPOS based on form font
		THIS.AddProp(M_VPOS,a_scx2fld[A_VPOS]*(THIS.formRef.nDeffont1+THIS.formRef.nDeffont5))		
		THIS.AddProp(M_HPOS,a_scx2fld[A_HPOS]*THIS.formRef.nDeffont6)
		DO CASE
			CASE THIS.fp25OT = 6		&&lines
				IF a_scx2fld[A_STYLE] = 0 && vertical line
					THIS.AddProp(M_WIDTH,0)
					THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT]*(THIS.formRef.nDeffont1+THIS.formRef.nDeffont5))
				ELSE
					THIS.AddProp(M_HEIGHT,0)
					THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH]*THIS.formRef.nDeffont6)
				ENDIF
			CASE THIS.fp25OT = 7		&&boxes
				THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT]*(THIS.formRef.nDeffont1+THIS.formRef.nDeffont5))
				THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH]*THIS.formRef.nDeffont6)
		ENDCASE
	ENDPROC

	*----------------------
	PROCEDURE AddColor		&& fp25shape
	*----------------------
	    *- Add colorstuff
		*- Add pen color
		PARAMETER m.btn

		LOCAL lColorSource

		THIS.AddProp(M_COLORSOURCE,I_DEFCOLORSOURCE,m.btn)

		IF a_scx2fld[A_PENRED] # -1
			THIS.AddProp(M_BORDERCOLOR,ALLT(STR(a_scx2fld[A_PENRED]))+;
				","+ALLT(STR(a_scx2fld[A_PENGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_PENBLUE])))
		ENDIF
		
		*- Add fill color
		IF a_scx2fld[A_FILLRED] = -1
			RETURN
		ENDIF
				
		THIS.AddProp(M_FILLCOLOR,ALLT(STR(a_scx2fld[A_FILLRED]))+;
			","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
			","+ALLT(STR(a_scx2fld[A_FILLBLUE])))
		THIS.AddProp(M_BACKCOLOR,"255,255,255")

		*- set disabled color to be the same as the enabled color
		THIS.AddProp(M_DISFORECOLOR,ALLT(STR(a_scx2fld[A_PENRED])) + ;
			","+ALLT(STR(a_scx2fld[A_PENGREEN])) + ;
			","+ALLT(STR(a_scx2fld[A_PENBLUE])))

	ENDPROC
	
ENDDEFINE && fp25shape 

************************************
DEFINE CLASS fp25line AS fp25shape
************************************
	*- override AddColor
	*----------------------
	PROCEDURE AddColor
	*----------------------
	    *- Add colorstuff
		*- Add pen color
		PARAMETER m.btn

		THIS.AddProp(M_COLORSOURCE,I_DEFCOLORSOURCE,m.btn)

		IF a_scx2fld[A_PENRED] # -1
			THIS.AddProp(M_BORDERCOLOR,ALLT(STR(a_scx2fld[A_PENRED]))+;
				","+ALLT(STR(a_scx2fld[A_PENGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_PENBLUE])))
		ENDIF
		
		*- Add fill color
		IF a_scx2fld[A_FILLRED] = -1
			RETURN
		ENDIF
				
		THIS.AddProp(M_BACKCOLOR,ALLT(STR(a_scx2fld[A_FILLRED]))+;
			","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
			","+ALLT(STR(a_scx2fld[A_FILLBLUE])))

	ENDPROC

ENDDEFINE && fp25line 

************************************
DEFINE CLASS fp25pict AS fp25obj
************************************

	*----------------------
	PROCEDURE AddBasic		&& fp25pict
	*----------------------
		fp25obj::AddBasic
		THIS.fp3id = SYS(2015)		&& we'll sort on this field later
	ENDPROC

	*----------------------
	PROCEDURE AddMain		&& fp25pict
	*----------------------
	
		*- Now add form properties
		THIS.AddName(THIS.GetNewName(THIS.fp3class))
		
		*- Add picture for BMP file
		IF a_scx2fld[A_STYLE] = 0
			*- source of picture is a file, stored in PICTURE field
			THIS.AddProp(M_FPICTURE,THIS.FullBMP(EVAL(a_scx2fld[A_PICTURE])))
		ELSE
			IF (" BITMAP" $ UPPER(a_scx2fld[A_NAME]))
			*-IF	("\" $ ALLT(a_scx2fld[A_NAME])) OR ;
				("." $ ALLT(a_scx2fld[A_NAME])) OR ;
				(":" $ ALLT(a_scx2fld[A_NAME])) OR ;
				('"' $ ALLT(a_scx2fld[A_NAME])) OR ;
				(" " $ ALLT(a_scx2fld[A_NAME]))
				*- assume is filename of form <filename><space>BITMAP
				THIS.AddProp(M_FPICTURE,"(" + LEFT(ALLT(a_scx2fld[A_NAME]), AT(" ",ALLT(a_scx2fld[A_NAME])) - 1) + ")")
			ELSE
				THIS.AddProp(M_FPICTURE,"(" + THIS.GetNewName(THIS.fp3class) + ")")
			ENDIF
		ENDIF

		*- Add mode (Opaque,Trans)
		THIS.AddMode(L_CONVERT,ABS(a_scx2fld[A_MODE]),a_scx2fld[A_FILLPAT])

		*- add ReleaseErase
		THIS.AddProp(M_RELEASEERASE,C_FALSE)
				
	ENDPROC

	*----------------------
	PROCEDURE AddPos		&& fp25pict
	*----------------------
		*- Add stretch mode before height, width, etc.
		THIS.AddProp(M_STRETCH,a_scx2fld[A_BORDER])

		*- VPOS,HPOS based on form font
		THIS.AddProp(M_VPOS,a_scx2fld[A_VPOS]*(THIS.formRef.nDeffont1+THIS.formRef.nDeffont5))		
		THIS.AddProp(M_HPOS,a_scx2fld[A_HPOS]*THIS.formRef.nDeffont6)
		THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT]*(THIS.formRef.nDeffont1+THIS.formRef.nDeffont5))
		THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH]*THIS.formRef.nDeffont6)
	ENDPROC	
	
	*----------------------
	PROCEDURE AddFont
	*----------------------
	ENDPROC
	
	PROCEDURE AddColor
		PARAMETER m.btn
		THIS.AddProp(M_COLORSOURCE,I_DEFCOLORSOURCE,m.btn)
	ENDPROC


ENDDEFINE && fp25pict

************************************
DEFINE CLASS fp25ole AS fp25pict
************************************

	*----------------------
	PROCEDURE AddMain		&& fp25ole
	*----------------------
	
		*- Now add form properties
		THIS.AddName(THIS.GetNewName(THIS.fp3class))
		
		*- Add mode (Opaque,Trans)
		THIS.AddMode(L_CONVERT,ABS(a_scx2fld[A_MODE]),a_scx2fld[A_FILLPAT])

		*- add ReleaseErase
		THIS.AddProp(M_RELEASEERASE,C_FALSE)

		*- Add datasource (general field name)
		THIS.AddProp(M_DATASOURCE,ALLTRIM(a_scx2fld[A_NAME]))

		*- other specific actions for OLEBoundControls
		THIS.AddCtrl

	ENDPROC
	
	*----------------------
	PROCEDURE AddCtrl		&& fp25ole
	*----------------------
		
		THIS.AddProp(M_ENABLED,.F.)
		THIS.AddProp(M_GROW,.F.)
		THIS.AddProp(M_AUTOACTIVATE,0)		&& manual

	ENDPROC


ENDDEFINE	&& fp25ole

************************************
DEFINE CLASS fp25form AS fp25obj
************************************
	*- Subclass for handling the form

	cWinName = ""

	*----------------------
	PROCEDURE Init		&& fp25form
	*----------------------
		PARAMETER parm1,parm2
		fp25obj::Init(parm1,parm2)
		THIS.iColorSource = I_WINCPCOLORSOURCE		&& use "5" (Windows CP) for colorsource for forms
	ENDPROC
	
	*----------------------
	PROCEDURE AddBasic		&& fp25form
	*----------------------
		fp25obj::AddBasic
		THIS.fp3id = SYS(2015)		&& we'll sort on this field later
	ENDPROC

	*----------------------------------
	PROCEDURE AddMain		&& fp25form
	*----------------------------------
	
		PRIVATE m.tmpstr,m.tmpcnt
		LOCAL m.ctmpname, lZoom

		*- Now add form record
		*- get form name
		IF EMPTY(THIS.formRef.cWnameExpr)
			IF EMPTY(a_scx2fld[A_NAME])
				THIS.cWinName = THIS.GetNewName(THIS.fp3class)
			ELSE				
				IF LEFT(a_scx2fld[A_NAME],1) = "("
					*- indirect ref to window name
					*- cFormName has been created above, before formset
					*- is created (at same time environment/DNO is handled)
					THIS.cWinName = THIS.formRef.cFormName
				ELSE
					*- check to see if form name is already used
					IF TRIM(a_scx2fld[A_NAME]) $ THIS.formRef.cWinNames
						*- duplicate name, so use made up one
						THIS.cWinName = THIS.GetNewName(THIS.fp3class)
					ELSE
						THIS.cWinName = TRIM(a_scx2fld[A_NAME])
						THIS.formRef.cWinNames = THIS.formRef.cWinNames + "|" + THIS.cWinName	&& add
					ENDIF
				ENDIF
			ENDIF
		ELSE
			IF EMPTY(a_scx2fld[A_NAME])
				*- no name provided, so go ahead and use #WNAME directive value
				THIS.cWinName = THIS.formRef.cWnameExpr
				THIS.formRef.cWinNames = THIS.formRef.cWinNames + "|" + THIS.cWinName	&& add
			ELSE
				*- check to see if form name is already used
				IF TRIM(a_scx2fld[A_NAME]) $ THIS.formRef.cWinNames
					*- duplicate name, so use #WNAME one
					THIS.cWinName = THIS.formRef.cWnameExpr
					THIS.formRef.cWinNames = THIS.formRef.cWinNames + "|" + THIS.cWinName	&& add
				ELSE
					IF THIS.formRef.cWnameExpr == "WZ_WIN"
						*- assume this is a Wizard generated form, so use the #WNAME value (jd 7/15/96)
						THIS.cWinName = THIS.formRef.cWnameExpr
					ELSE
						*- name and #WNAME directive value provided, and not Wizard -- use name
						THIS.cWinName = TRIM(a_scx2fld[A_NAME])
					ENDIF
					THIS.formRef.cWinNames = THIS.formRef.cWinNames + "|" + THIS.cWinName	&& add
				ENDIF
			ENDIF
		ENDIF
		
		THIS.AddName(THIS.cWinName)

		*- adds form specific,window type properties
		*- window title (check for #itse)
		DO CASE
			CASE !EMPTY(THIS.formRef.itse_expr) AND SUBSTR(a_scx2fld[A_TAG],2,1) = THIS.formRef.itse_expr
				THIS.AddProp(M_CAPTION,SUBSTR(a_scx2fld[A_TAG],3, RAT('"',a_scx2fld[A_TAG])-3))
			CASE !EMPTY(a_scx2fld[A_TAG])
				THIS.AddProp(M_CAPTION,a_scx2fld[A_TAG])
			OTHERWISE
				*- no title
				THIS.AddProp(M_CAPTION,[""])
		ENDCASE
		
		IF !EMPTY(a_scx2fld[A_PICTURE])
			THIS.AddProp(M_FPICTURE,THIS.FullBMP(StripQuote(a_scx2fld[A_PICTURE]))) && wallpaper
		ENDIF
		IF !EMPTY(a_scx2fld[A_ORDER])
			THIS.AddProp(M_ICON,EVAL(a_scx2fld[A_ORDER])) &&icon
		ENDIF
		
		THIS.AddProp(M_BORDER,THIS.GetWBorder(a_scx2fld[A_BORDER]))

		IF INLIST(a_scx2fld[A_PLATFORM],C_DOS,C_UNIX)
			*- Add DOS footer (check for ITSE)
			DO CASE
				CASE !EMPTY(THIS.formRef.itse_expr) AND SUBSTR(a_scx2fld[A_TAG2],2,1) = THIS.formRef.itse_expr
					THIS.AddProp(M_TAGD,SUBSTR(a_scx2fld[A_TAG2],3, RAT('"',a_scx2fld[A_TAG2])-3))
				CASE !EMPTY(a_scx2fld[A_TAG2])
			  		THIS.AddProp(M_TAGD,a_scx2fld[A_TAG2]) 	
			ENDCASE
			*- Add shadow
			THIS.AddProp(M_SHADOW,a_scx2fld[A_SHADOW])
		ENDIF
						
		*- Now check #WCLAUSE directive
		*- IN DESKTOP / IN SCREEN / IN WINDOW
		DO CASE
			CASE ATC("IN DESKTOP",THIS.formRef.wclause_expr) # 0
				THIS.AddProp(M_DESKTOP,C_TRUE)
			CASE ATC("IN WINDOW",THIS.formRef.wclause_expr) # 0
				m.tmpcnt = 1
				DO WHILE UPPER(WORDNUM(THIS.formRef.wclause_expr,m.tmpcnt))#"WINDOW"
					m.tmpcnt = m.tmpcnt+1
				ENDDO
				m.tmpstr = WORDNUM(THIS.formRef.wclause_expr,m.tmpcnt+1)
				THIS.AddProp(M_WINDOW,m.tmpstr)
			CASE ATC("IN SCREEN",THIS.formRef.wclause_expr) # 0
				THIS.AddProp(M_WINDOW,C_TRUE)
		ENDCASE
		
		*- GROW
		THIS.AddProp(M_GROW,ATC(" GROW",THIS.formRef.wclause_expr) # 0)
		
		*- ZOOM - Maximize
		m.lZoom = ATC(" ZOOM",THIS.formRef.wclause_expr) # 0
				
		*- MDI
		THIS.AddProp(M_MDI,ATC(" MDI",THIS.formRef.wclause_expr) # 0)

		THIS.AddProp(M_MAXIMIZE,m.lZoom)
		THIS.AddProp(M_ZOOMBOX,m.lZoom)					&& for the Mac (jd 02.14.95)
		THIS.AddProp(M_FLOAT,a_scx2fld[A_FLOAT])
		THIS.AddProp(M_CLOSE,a_scx2fld[A_CLOSE])
		THIS.AddProp(M_MINIMIZE,a_scx2fld[A_MINIMIZE])
		IF (!a_scx2fld[A_FLOAT] AND !a_scx2fld[A_CLOSE] AND !a_scx2fld[A_MINIMIZE] AND !m.lZoom)
			THIS.AddProp(M_CONTROLBOX,.F.)
		ENDIF
			
		THIS.AddProp(M_HALF,a_scx2fld[A_TAB])


		*- Color Scheme
		IF ATC("COLORSCHEME",THIS.formRef.wclause_expr) # 0
			m.tmpcnt = 1
			DO WHILE UPPER(WORDNUM(THIS.formRef.wclause_expr,m.tmpcnt))#"COLORSCHEME"
				m.tmpcnt = m.tmpcnt+1
			ENDDO
			m.tmpstr = WORDNUM(THIS.formRef.wclause_expr,m.tmpcnt+1)
			THIS.AddProp(M_SCHEME,m.tmpstr)
		ENDIF
			
		*- support indirect reference to file names
		IF THIS.FormRef.lIndirectWinName
			THIS.AddMethods(M_INIT,;
				C_CRLF + ;
				"THIS.Name = " + ;
				THIS.FormRef.cIndirectWinName + ;
				C_CRLF,1)
		ENDIF

		*- section 2 code goes into FORM.LOAD method
		IF !EMPTY(THIS.formRef.a_reads[2])
			THIS.AddMethods(M_SETUP2,THIS.formRef.a_reads[2],1)
			*- clear out setup var, so it isn;t added to more than one screen in set
			THIS.formRef.a_reads[2] = ""
		ENDIF
		IF !EMPTY(THIS.formRef.cMainCurs)
			THIS.AddMethods(M_FORMACTIVATE,"SELECT " + THIS.formRef.cMainCurs + C_CRLF,1)
		ENDIF

	ENDPROC
	
	*----------------------------------
	PROCEDURE Premap		&& fp25form
	*----------------------------------
		*- We need to reset for screen sets
		THIS.formRef.parentName = THIS.formRef.cFormSetName
	ENDPROC
		
	*----------------------------------
	PROCEDURE Postmap		&& fp25form
	*----------------------------------

		THIS.formRef.parentName = THIS.formRef.parentName + "." + THIS.fp3name

		*- get form fontmetrics for other records
		IF INLIST(a_scx2fld[A_PLATFORM],C_MAC,C_WINDOWS)
			THIS.formRef.nDeffont1 = THIS.fp3font1 	&& default screen font1
			THIS.formRef.nDeffont5 = THIS.fp3font5 	&& default screen font5
			THIS.formRef.nDeffont6 = THIS.fp3font6 	&& default screen font6
		ENDIF
		
		*- Need to get default screen color for transparent objects
		IF a_scx2fld[A_FILLRED] = -1
			THIS.formRef.cDefcolor = "255,255,255"
		ELSE
			THIS.formRef.cDefcolor = ALLT(STR(a_scx2fld[A_FILLRED]))+;
				","+ALLT(STR(a_scx2fld[A_FILLGREEN]))+;
				","+ALLT(STR(a_scx2fld[A_FILLBLUE]))
		ENDIF
		
		*- Add form record here so that
		*- PageFrame rec gets added automatically
		THIS.AddProp(M_NAME,THIS.fp3name)
		
		*- Add Form / PageFrame / FormPage record
		IF !THIS.FormRef.lDevMode
			THIS.AddRec
			THIS.AddPage()
		ENDIF

	ENDPROC

	*----------------------------------
	PROCEDURE WriteName
	*----------------------------------
	ENDPROC
	
	*----------------------------------
	PROCEDURE AddPage
	*----------------------------------
		*- Note: we add an invisible form page here
		*- to simulate the first READ level.
		THIS.fp3class = T_PAGE			&& class
		THIS.fp3base = T_PAGE			&& baseclass
		THIS.fp3prop = ""				&& properties
		THIS.fp3method = ""				&& methods
		THIS.AddBasic
		THIS.fp3comment	= a_scx2fld[A_COMMENT]		 && comment
		THIS.fp3parent = THIS.formRef.parentName
		THIS.fp3name = C_PAGEFRAME
		THIS.AddProp(M_VPOS,0)
		THIS.AddProp(M_HPOS,0)
		THIS.AddProp(M_HEIGHT,30000)
		THIS.AddProp(M_WIDTH,30000)
		THIS.AddProp(M_FORMPAGES,1)
		THIS.AddProp(M_PENSIZE,0)	&&borderwidth
		THIS.AddProp(M_FORMTABS,C_FALSE)
		THIS.AddProp(M_ERASEPAGE,C_FALSE)
		THIS.AddProp(M_DRAWFRAME,C_FALSE)
		THIS.AddProp(M_NAME,C_PAGEFRAME)
		THIS.AddProp(C_DEFPAGE+"."+M_MODE,0)
		THIS.AddProp(C_DEFPAGE+"."+M_NAME,THIS.AddQuotes(C_DEFPAGE))
		
		THIS.formRef.parentName = THIS.formRef.parentName + ;
			"." + C_PAGEFRAME + "." + C_DEFPAGE
	ENDPROC

	*----------------------------------
	PROCEDURE AddPos		&& fp25form
	*----------------------------------
		*- Add object positions in pixels (how Taz stores it)
		
		PRIVATE m.arrange,m.larrflag,m.lcentflag,m.nrow,m.ncol
		STORE "" TO m.arrange
		STORE .F. TO m.larrflag,m.lcentflag
		STORE 0 TO m.nrow,m.ncol

		THIS.nDeffont1 = THIS.fp3font1
		THIS.nDeffont5 = THIS.fp3font5
		THIS.nDeffont6 = THIS.fp3font6

		*- WIDTH based on object font
		THIS.AddProp(M_WIDTH,a_scx2fld[A_WIDTH] * THIS.fp3font6)

		*- HEIGHT based on object font
		*- Add title bar height for 3.0 style forms
		THIS.AddProp(M_HEIGHT,a_scx2fld[A_HEIGHT]*;
		  (THIS.fp3font1+THIS.fp3font5))
		  
		*- check if we have arranged screen from project
		*- VPOS,HPOS based on form font

		m.arrange = THIS.formRef.a_scx2files[THIS.formRef.formnum,2]
		
		IF !EMPTY(m.arrange)
			=getarrange(m.arrange,ALLTRIM(a_scx2fld[A_PLATFORM]),@larrflag,@lcentflag,@nrow,@ncol,a_scx2fld[A_CENTER])
			IF m.larrflag AND !m.lcentflag
				*- FoxPro 2.x used the current screen settings to calculate the row and col in foxels
				THIS.AddProp(M_VPOS,m.nrow*FONT(1,WFONT(1,"screen"),WFONT(2,"screen"),WFONT(3,"screen"))) && (THIS.nDeffont1+THIS.nDeffont5)	
				THIS.AddProp(M_HPOS,m.ncol*FONT(6,WFONT(1,"screen"),WFONT(2,"screen"),WFONT(3,"screen"))) &&	THIS.nDeffont6
			ENDIF
		ELSE
			*- most likely a screen by itself
			m.lcentflag = a_scx2fld[A_CENTER]
		ENDIF
		
		IF !m.larrflag 
		  THIS.AddProp(M_VPOS,a_scx2fld[A_VPOS]*(THIS.nDeffont1+THIS.nDeffont5))		
		  THIS.AddProp(M_HPOS,a_scx2fld[A_HPOS]*THIS.nDeffont6)
		ENDIF
		
		*- add Autocenter property
		THIS.AddProp(M_CENTER,m.lcentflag)
		
	ENDPROC	&& AddPos

	*----------------------
	PROCEDURE AddRec		&& fp25form
	*----------------------
		*- Add record to FORM file	
		*- Override base class method, since form record needs
		*- special key word in reserved4 field
		IF THIS.formRef.lDevMode
			*- put methods someplace else
			INSERT INTO (THIS.formRef.new30alias) ;
				(platform,uniqueid,timestamp,;
				class,baseclass,objname,parent,properties,;
				user,reserved1,reserved4,reserved6);
			VALUES(THIS.fp3saveplat,THIS.fp3id,THIS.fp3time,;
				THIS.fp3class,THIS.fp3base,THIS.fp3name,;
				THIS.fp3parent,THIS.fp3prop,;
				THIS.fp3comment,;
				THIS.fp3reserved1,;
				IIF(!THIS.formref.a_pjxsets[A_DEFWINDOWS] AND ;
					THIS.fp3class = T_FORM,"NODEFINE",""),;
					THIS.fp3reserved6)
			IF !EMPTY(THIS.fp3method)
				REPLACE _fox3spr.code WITH C_SEPARATOR + THIS.fp3name + C_CRLF + THIS.fp3method ADDITIVE
			ENDIF
		ELSE
			*- put methods someplace else
			INSERT INTO (THIS.formRef.new30alias) ;
				(platform,uniqueid,timestamp,;
				class,baseclass,objname,parent,properties,;
				methods,user,reserved1,reserved4,reserved6);
			VALUES(THIS.fp3saveplat,THIS.fp3id,THIS.fp3time,;
				THIS.fp3class,THIS.fp3base,THIS.fp3name,;
				THIS.fp3parent,THIS.fp3prop,;
				THIS.fp3method,;
				THIS.fp3comment,;
				THIS.fp3reserved1,;
				IIF(!THIS.formref.a_pjxsets[A_DEFWINDOWS] AND ;
					THIS.fp3class = T_FORM,"NODEFINE",""),;
					THIS.fp3reserved6)
		ENDIF
	ENDPROC

	*----------------------------------
	FUNCTION GetWBorder
	*----------------------------------
		*- Note: FPW 2.x did not properly handle
		*- single and double border windows as in 3.0 
		
		PARAMETER wstyle
		DO CASE
			CASE m.wstyle = 0	&& no border ???
				RETURN 0
			CASE m.wstyle = 1	&& single border
				RETURN 1
			CASE m.wstyle = 2	&& double border
				RETURN 2
			CASE m.wstyle = 3	&& panel border
				RETURN 2
			CASE m.wstyle = 4	&& system border
				RETURN 2
		ENDCASE
	ENDFUNC

	
	*----------------------
	FUNCTION GetNewName	&& fp25form
	*----------------------
		*- override fp25obj -- generate a unique form name
		PARAMETER newobj

		RETURN SYS(2015)

	ENDFUNC		&& GetNewName
	

ENDDEFINE

************************************
DEFINE CLASS fpdatanav AS fp25obj
************************************
	
	cOldParentName = ""
	fp3objtype = 0

	*----------------------------------
	PROCEDURE Init			&& fpdatanav
	*----------------------------------
		PARAMETER parm1,parm2
		
		fp25obj::Init(parm1,parm2)
		
		THIS.cOldParentName = THIS.formRef.parentName
		
		IF !THIS.formRef.lHasDataNavObj
			*- add a data nav object for this screen, and
			*- set flag so we don't come back here again
			THIS.formRef.parentName = ""
			THIS.fp3comment	= ""		&& comment
			THIS.fp3objtype = N_FRX_DATAENV
			THIS.mapit
			THIS.addrec
			THIS.formRef.nDNORecNo = RECNO(THIS.formRef.new30alias)
			THIS.formRef.lHasDataNavObj = .T.
			THIS.ClearProp				&& clear properties before continuing
		ENDIF
		*- set class values to "cursor"
		THIS.formRef.parentName = C_DEFDATANAV
		THIS.fp3class  = T_CURSOR			&& class
		THIS.fp3base   = T_CURSOR			&& baseclass
		RETURN
	ENDPROC

	*----------------------
	PROCEDURE AddBasic		&& fpdatanav
	*----------------------
		fp25obj::AddBasic
		THIS.fp3id 		= '^'					 	&& we'll sort on this field later -- goes after alpha, and before "_"
	ENDPROC

	*----------------------------------
	PROCEDURE MapIt
	*----------------------------------
	*- overwrite the MapIt procedure
		THIS.PreMap
		THIS.AddBasic
		THIS.AddMain
		THIS.PostMap
		THIS.WriteName	 && note Name property must be written last!
		THIS.formRef.nDNOCount = THIS.formRef.nDNOCount + 1 
	ENDPROC
	
	*----------------------------------
	PROCEDURE AddMain		&& fpdatanav
	*----------------------------------

		LOCAL nlen, cSaveArea, nCurrec

		*- Now add DE specific properties
		IF !THIS.formRef.lHasDataNavObj
			THIS.AddName(THIS.fp3class)
		ELSE
			THIS.AddName(THIS.GetNewName(THIS.fp3class))
		ENDIF
		
		IF !THIS.formRef.lHasDataNavObj
			*- this is the data navigation container
			THIS.AddProp(M_AUTOLOADENV,THIS.formRef.lAutoOpen)
			THIS.AddProp(M_AUTOUNLOADENV,THIS.formRef.lAutoClose)
			STORE SELECT() TO m.savearea
			SELECT (THIS.formRef.c25alias)
			m.nCurrec = RECNO()
			LOCATE FOR objtype = 2 AND platform = THIS.formRef.platform
			SCAN WHILE objtype = 2 AND platform = THIS.formRef.platform
				THIS.formRef.cMainCurs = IIF(EMPTY(THIS.formRef.cMainCurs) AND unique,ALLT(tag),THIS.formRef.cMainCurs)
			ENDSCAN
			IF m.nCurrec > RECC()
				GO BOTTOM
				SKIP
			ELSE
				GO m.nCurrec
			ENDIF
			SELECT (m.savearea)
			THIS.AddProp(M_INITIALALIAS,THIS.formRef.cMainCurs)
		ELSE
			*- alias
			THIS.AddProp(M_ALIAS,a_scx2fld[A_TAG])
			
			*- cursor source
			THIS.AddProp(M_CURSORSRC,THIS.FullBMP(a_scx2fld[A_NAME]))
			
			*- order
			IF !EMPTY(ALLT(a_scx2fld[A_TAG2]))
				THIS.AddProp(M_ORDER,a_scx2fld[A_TAG2])
			ENDIF

			*- remember index orders
			m.nlen = ALEN(THIS.formRef.a_tables)
			IF !(m.nlen = 1 AND EMPTY(THIS.formRef.a_tables[1]))
				*- grow array
				m.nlen = m.nlen + 1
				DIMENSION THIS.formRef.a_tables[m.nlen]
				DIMENSION THIS.formRef.a_torder[m.nlen]
			ENDIF
			THIS.formRef.a_tables[m.nlen] = ALLTRIM(a_scx2fld[A_TAG])
			THIS.formRef.a_torder[m.nlen] = ALLTRIM(a_scx2fld[A_TAG2])

			*- filter?
		
		ENDIF
				
	ENDPROC		&&  fpdatanav:AddMain

	*----------------------------------
	FUNCTION GetNewName	&& fp25datanav
	*----------------------------------
		PARAMETER newobj
		THIS.formRef.nObjCount = THIS.formRef.nObjCount + 1
		RETURN ALLTRIM(m.newobj) + ALLTRIM(STR(THIS.formRef.nObjCount))
	ENDFUNC && fp25datanav:GetNewName

	*----------------------------------
	PROCEDURE Destroy
	*----------------------------------
		*- reset remembered parentName
		THIS.formRef.parentName = THIS.cOldParentName
	ENDPROC
ENDDEFINE

************************************
DEFINE CLASS fpFRXdatanav AS fpdatanav
************************************

	*------------------
	PROCEDURE AddRec			&& fpFRXdatanav
	*------------------
		THIS.fp3plat = THIS.GetPlatform()	 	&& platform: NOTE -- forces to be current platform

		*- add a record to the 3.0 FRX file
		*- environ == private data session, always set it to .F.
		INSERT INTO (THIS.formRef.new30alias) ;
				(platform,uniqueid,timestamp,objtype,name,expr,environ);
			VALUES(THIS.fp3plat,THIS.fp3id,THIS.fp3time,;
				THIS.fp3objtype,THIS.fp3class,THIS.fp3prop,.F.)

	ENDPROC

ENDDEFINE

************************************
DEFINE CLASS fpDataNavRelation AS fpdatanav
************************************

	cAlias = ""

	*----------------------------------
	PROCEDURE Init
	*----------------------------------
		PARAMETER parm1,parm2
		
		fp25obj::Init(parm1,parm2)
			
		THIS.cOldParentName = THIS.formRef.parentName
		THIS.formRef.parentName = C_DEFDATANAV
	
	ENDPROC		&&  fpDataNavRelation::Init
	
	*----------------------------------
	PROCEDURE AddMain		&& fpDataNavRelation
	*----------------------------------

		LOCAL npos, loldexact, nrec, m.savearea, nworkarea

		*- Now add relation properties
		IF !THIS.formRef.lHasDataNavObj
			THIS.AddName(THIS.fp3class)
		ELSE
			THIS.AddName(THIS.GetNewName(THIS.fp3class))
		ENDIF
		
		*- parent alias
		THIS.AddProp(M_PARENTALIAS,a_scx2fld[A_TAG2])
		
		*- parent index expr
		THIS.AddProp(M_PARENTINDEXEXPR,a_scx2fld[A_EXPR])
		
		*- child alias
		THIS.AddProp(M_CHILDALIAS,a_scx2fld[A_TAG])
		
		*- child index expr
		m.loldexact = SET("EXACT")
		npos = ASCAN(THIS.formRef.a_tables,TRIM(a_scx2fld[A_TAG]))
		IF m.npos > 0
			THIS.AddProp(M_CHILDINDEXTAG,THIS.formRef.a_torder[m.npos])
		ENDIF
		SET EXACT &loldexact

		*- always set one-to-many
		*- remember this record
		m.savearea = SELECT()
		nWorkArea = a_scx2fld[A_OBJCODE]
		SELECT (THIS.formRef.c25alias)
		m.nrec = RECNO()
		LOCATE FOR objtype = 2 AND objcode = m.nWorkArea AND platform = THIS.formRef.platform AND !EMPTY(expr) AND !environ
		IF FOUND()
			*- THIS.AddProp(M_ONETOMANY,.T.)
			*- do it the hard way
			THIS.FormRef.cSetSkip = THIS.FormRef.cSetSkip + C_SELECT + LOWER(ALLT(tag)) + C_CR + ;
				C_SETSKIP + LOWER(ALLT(expr)) + C_CR
			*- mark the record so we don;t hit it again -- this file is temporary, so we can touch it
			REPLACE environ WITH .T.
		ENDIF
		GOTO IIF(m.nrec > RECC(),RECC(),m.nrec)
		SELECT (m.savearea)

	ENDPROC		&&  fpDataNavRelation:AddMain

ENDDEFINE && fpDataNavRelation

************************************
DEFINE CLASS fpFRXDataNavRelation AS fpDataNavRelation
************************************

	*------------------
	PROCEDURE AddRec			&& fpFRXDataNavRelation
	*------------------
		THIS.fp3plat = THIS.GetPlatform()	 	&& platform: NOTE -- forces to be current platform

		*- add a record to the 3.0 FRX file
		*- environ == private data session, always set it to .F.
		INSERT INTO (THIS.formRef.new30alias) ;
				(platform,uniqueid,timestamp,objtype,name,expr,environ);
			VALUES(THIS.fp3plat,THIS.fp3id,THIS.fp3time,;
				THIS.fp3objtype,THIS.fp3class,THIS.fp3prop,.F.)

	ENDPROC

ENDDEFINE

*-
*- eof CONVERT.PRG
*-
