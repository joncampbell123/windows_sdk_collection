* TRANSFRM.PRG - Transformer.
*
* Copyright (c) 1996 Microsoft Corp.
* 1 Microsoft Way
* Redmond, WA 98052
*
* Description:
* Transformer for PJX/VCX/SCX files.
*

#INCLUDE "transfrm.h"

LPARAMETERS tcFormClass,tcTransformClass
LOCAL lcFormClass,lcTransformClass,lcFormClassLibrary,lcTransformClass
LOCAL oForm,lcProgramName,lnAtPos,llAbort
LOCAL lcLastSetClassLib,lcLastSetProcedure
LOCAL lcLastSetTalk,lcLastSetESC,lcLastSetUDFParms
LOCAL laInstances[1]

lcLastSetTalk=SET('TALK')
SET TALK OFF
lcLastSetESC=SET('ESCAPE')
SET ESCAPE OFF
lcLastSetUDFParms=SET('UDFPARMS')
SET UDFPARMS VALUE
SET COLLATE TO 'MACHINE'
llAbort=.F.
lcProgramName=LOWER(SYS(16))
lcLastSetClassLib=SET('CLASSLIB')
lcLastSetProcedure=SET('PROCEDURE')
lcFormClass=IIF(TYPE('tcFormClass')#'C' OR EMPTY(tcFormClass), ;
		'Transformer',tcFormClass)
lcTransformClass=IIF(TYPE('tcTransformClass')#'C' OR ;
		EMPTY(tcTransformClass),'TransformFiles',tcTransformClass)
lcFormClassLibrary=LOWER(FULLPATH('transfrm',lcProgramName))
lcTransformClassLibrary=''
SET CLASSLIB TO (lcFormClassLibrary) ADDITIVE
lnAtPos=AT(',',lcFormClass)
IF lnAtPos>0
	lcFormClassLibrary=LOWER(ALLTRIM(MLINE(LEFT(lcFormClass,lnAtPos-1),1)))
	IF NOT '.'$lcFormClassLibrary
		lcFormClassLibrary=lcFormClassLibrary+'.vcx'
	ENDIF
	lcFormClass=LOWER(ALLTRIM(SUBSTR(lcFormClass,lnAtPos+1)))
	IF NOT FILE(lcFormClassLibrary)
		=FileNotFoundMsg(lcFormClassLibrary)
		llAbort=.T.
	ENDIF
ENDIF
lnAtPos=AT(',',lcTransformClass)
IF lnAtPos>0
	lcTransformClassLibrary=LOWER(ALLTRIM(MLINE(LEFT(lcTransformClass,lnAtPos-1),1)))
	IF NOT '.'$lcTransformClassLibrary
		lcTransformClassLibrary=lcTransformClassLibrary+'.vcx'
	ENDIF
	lcTransformClass=LOWER(ALLTRIM(SUBSTR(lcTransformClass,lnAtPos+1)))
	IF NOT FILE(lcTransformClassLibrary)
		=FileNotFoundMsg(lcTransformClassLibrary)
		llAbort=.T.
	ENDIF
ENDIF
IF lcFormClassLibrary==lcTransformClassLibrary
	lcTransformClassLibrary=''
ENDIF
IF NOT llAbort AND AINSTANCE(laInstances,lcFormClass)=0
	IF RIGHT(lcFormClassLibrary,4)=='.prg'
		SET PROCEDURE TO (lcFormClassLibrary) ADDITIVE
	ELSE
		SET CLASSLIB TO (lcFormClassLibrary) ADDITIVE
	ENDIF
	oForm=CREATEOBJECT(lcFormClass)
	IF TYPE('oForm')#'O'
		llAbort=.T.
	ENDIF
	IF NOT llAbort AND TYPE('oForm.oTransform')=='U'
		IF NOT EMPTY(lcTransformClassLibrary)
			IF RIGHT(lcTransformClassLibrary,4)=='.prg'
				SET PROCEDURE TO (lcTransformClassLibrary) ADDITIVE
			ELSE
				SET CLASSLIB TO (lcTransformClassLibrary) ADDITIVE
			ENDIF
		ENDIF
		oForm.AddObject('oTransform',lcTransformClass)
		oForm.oTransform.Name='oTransform'
		IF TYPE('oForm.oTransform')#'O'
			llAbort=.T.
		ENDIF
	ENDIF
	IF NOT SET('CLASSLIB')==lcLastSetClassLib
		IF NOT RIGHT(lcFormClassLibrary,4)=='.prg'
			RELEASE CLASSLIB (lcFormClassLibrary)
		ENDIF
		IF NOT EMPTY(lcTransformClassLibrary) AND ;
				NOT RIGHT(lcTransformClassLibrary,4)=='.prg'
			RELEASE CLASSLIB (lcTransformClassLibrary)
		ENDIF
	ENDIF
	IF NOT SET('PROCEDURE')==lcLastSetProcedure
		IF RIGHT(lcFormClassLibrary,4)=='.prg'
			RELEASE PROCEDURE (lcFormClassLibrary)
		ENDIF
		IF NOT EMPTY(lcTransformClassLibrary) AND ;
				RIGHT(lcTransformClassLibrary,4)=='.prg'
			RELEASE PROCEDURE (lcTransformClassLibrary)
		ENDIF
	ENDIF
	IF NOT llAbort
		oForm.Show()
	ENDIF
ENDIF
IF TYPE('lcLastSetUDFParms')=='C' AND lcLastSetUDFParms=='REFERENCE'
	SET UDFPARMS REFERENCE
ELSE
	SET UDFPARMS VALUE
ENDIF
IF TYPE('lcLastSetESC')=='C' AND lcLastSetESC=='ON'
	SET ESCAPE ON
ELSE
	SET ESCAPE OFF
ENDIF
IF TYPE('lcLastSetTalk')=='C' AND lcLastSetTalk=='ON'
	SET TALK ON
ELSE
	SET TALK OFF
ENDIF
IF TYPE('llAbort')#'L'
	RETURN .T.
ENDIF
RETURN llAbort



FUNCTION ShowMsgBox
LPARAMETERS tcMessage,tnType,tcTitle
LOCAL lcMessage,lnResult,lnType,lcTitle

lcMessage=IIF(TYPE('tcMessage')=='C',tcMessage,'')
lnType=IIF(TYPE('tnType')=='N',tnType,48)
lcTitle=IIF(TYPE('tcTitle')=='C',tcTitle,'Visual FoxPro Transformer')
WAIT CLEAR
lnResult=MESSAGEBOX(lcMessage,lnType,lcTitle)
RETURN lnResult



FUNCTION FileNotFoundMsg
LPARAMETERS tcFileName

RETURN ShowMsgBox(FILENOTFOUND1_LOC +LOWER(tcFileName)+FILENOTFOUND2_LOC)



DEFINE CLASS TransformFiles AS Custom


	Name='oTransformFiles'
	PROTECTED lAddFontRules
	cGetFileExt='pjx|scx|vcx'
	PROTECTED cText
	cText=''
	PROTECTED cFileName
	cFileName=''
	PROTECTED lSearchSubfolders
	PROTECTED lCreateLogOnly
	PROTECTED lLogToFile
	PROTECTED cLogToFile
	cLogToFile=''
	PROTECTED nStartSeconds
	nStartSeconds=0
	PROTECTED nEndSeconds
	nEndSeconds=0
	PROTECTED aProcessedFiles[1]
	PROTECTED nProcessedFileCount
	nProcessedFileCount=0
	PROTECTED nTransformedFileCount
	nTransformedFileCount=0
	PROTECTED nFileCount
	nFileCount=0
	PROTECTED aFileList[1]
	PROTECTED nRuleCount
	nRuleCount=0
	PROTECTED aRules[1]
	PROTECTED aFileRule[1]
	PROTECTED nFileRuleCount
	nFileRuleCount=0
	PROTECTED lOverridePropertyDefaults
	PROTECTED aExcludeFilesRule[1]
	PROTECTED aClassRule[1]
	PROTECTED nClassRuleCount
	nClassRuleCount=0
	PROTECTED aExcludeClassesRule[1]
	PROTECTED lLastLockScreen
	PROTECTED nFontMatchCount
	nFontMatchCount=0


	FUNCTION Do(tlAddFontRules)
	LOCAL lcMsg

	this.parent.lError=.F.
	this.lLastLockScreen=this.parent.LockScreen
	this.lAddFontRules=tlAddFontRules
	this.lSearchSubfolders=this.parent.pgfTransformer. ;
			fpgFiles.chkSearchSubfolders.Value
	this.lCreateLogOnly=this.parent.pgfTransformer. ;
			fpgFiles.chkCreateLogOnly.Value
	this.lLogToFile=this.parent.pgfTransformer. ;
			fpgFiles.chkLogToFile.Value
	this.cLogToFile=ALLTRIM(this.parent.pgfTransformer. ;
			fpgFiles.txtLogToFile.Value)
	this.nFileCount=this.parent.pgfTransformer.fpgFiles. ;
			lstSelectedFiles.ListCount
	IF this.nFileCount=0
		this.NoFilesErrorMsg()
		RETURN ''
	ENDIF
	IF NOT this.lAddFontRules AND NOT this.lCreateLogOnly AND ;
			ShowMsgBox(ALTERFILES_LOC,289)#1
		RETURN ''
	ENDIF
	this.nStartSeconds=SECONDS()
	ACTIVATE SCREEN
	this.InitalizeRules()
	this.ProcessStart()
	IF NOT this.ProcessFileList() OR this.parent.lError
		this.AddText('',1)
		IF this.parent.lError
			this.AddText(ABORT1_LOC,1)
		ELSE
			this.AddText(ABORT2_LOC,1)
		ENDIF
	ENDIF
	this.nEndSeconds=SECONDS()
	IF NOT RIGHT(this.cText,2)==(CR+CR)
		this.AddText('',1)
	ENDIF
	DO CASE
		CASE this.nTransformedFileCount=0
			this.AddText(NOFILES_LOC)
		CASE this.nTransformedFileCount=1
			this.AddText(HAS1FILE_LOC)
		OTHERWISE
			this.AddText(ALLTRIM(STR(this.nTransformedFileCount))+HASMANYFILE_LOC)
	ENDCASE
	this.AddText(' '+IIF(this.lCreateLogOnly,SCANNED_LOC,PROCESSED_LOC)+' in '+ ;
			ALLTRIM(STR(ABS(this.nEndSeconds-this.nStartSeconds),9,3))+ ;
			SECONDS_LOC,1)
	this.ProcessEnd()
	this.DisplayText()
	this.parent.lError=.F.
	this.parent.Refresh()
	this.parent.LockScreen=this.lLastLockScreen
	CLEAR TYPEAHEAD
	IF this.lAddFontRules
		WAIT CLEAR
		lcMsg=IIF(this.nFontMatchCount=0,NOFONTRULE_LOC, ;
				ALLTRIM(STR(this.nFontMatchCount))+FONTRULE1_LOC+ ;
				IIF(this.nFontMatchCount=1,'','s'))+FONTRULE2_LOC
			ShowMsgBox(lcMsg,64,FONTRULE3_LOC)
		RETURN ''
	ENDIF
	this.parent.pgfTransformer.fpgLog.edtTransformerLog.SetFocus()
	KEYBOARD '{DNARROW}' PLAIN
	RETURN this.cText
	ENDFUNC
	
	
	FUNCTION WildCardMatch(tcMatchExpList,tcExpressionSearched)
	LOCAL lcMatchExpList,lcMatchExp,lcExpressionSearched
	LOCAL lnMatchLen,lnExpressionLen,lnMatchCount,lnCount,lnCount2,lnAtPos

	IF EMPTY(tcExpressionSearched)
		RETURN .F.
	ENDIF
	lcExpressionSearched=LOWER(ALLTRIM(tcExpressionSearched))
	lnExpressionLen=LEN(lcExpressionSearched)
	lcMatchExpList=LOWER(ALLTRIM(tcMatchExpList))
	lnMatchCount=OCCURS(',',lcMatchExpList)+1
	IF lnMatchCount>1
		lcMatchExpList=','+ALLTRIM(lcMatchExpList)+','
	ENDIF
	FOR lnCount = 1 TO lnMatchCount
		IF lnMatchCount=1
			lcMatchExp=LOWER(ALLTRIM(lcMatchExpList))
			lnMatchLen=LEN(lcMatchExp)
		ELSE
			lnAtPos=AT(',',lcMatchExpList,lnCount)
			lnMatchLen=AT(',',lcMatchExpList,lnCount+1)-lnAtPos-1
			lcMatchExp=LOWER(ALLTRIM(SUBSTR(lcMatchExpList,lnAtPos+1, ;
					lnMatchLen)))
		ENDIF
		FOR lnCount2 = 1 TO OCCURS('?',lcMatchExp)
			lnAtPos=AT('?',lcMatchExp)
			IF lnAtPos>lnExpressionLen
				IF (lnAtPos-1)=lnExpressionLen
					lcExpressionSearched=lcExpressionSearched+'?'
				ENDIF
				EXIT
			ENDIF
			lcMatchExp=STUFF(lcMatchExp,lnAtPos,1, ;
					SUBSTR(lcExpressionSearched,lnAtPos,1))
		ENDFOR
		IF EMPTY(lcMatchExp) OR lcExpressionSearched==lcMatchExp OR ;
				lcMatchExp=='*' OR lcMatchExp=='?' OR lcMatchExp=='%%'
			RETURN
		ENDIF
		IF LEFT(lcMatchExp,1)=='*'
			RETURN (SUBSTR(lcMatchExp,2)==RIGHT(lcExpressionSearched, ;
					LEN(lcMatchExp)-1))
		ENDIF
		IF LEFT(lcMatchExp,1)=='%' AND RIGHT(lcMatchExp,1)=='%' AND ;
				SUBSTR(lcMatchExp,2,lnMatchLen-2)$lcExpressionSearched
			RETURN
		ENDIF
		lnAtPos=AT('*',lcMatchExp)
		IF lnAtPos>0 AND (lnAtPos-1)<=lnExpressionLen AND ;
				LEFT(lcExpressionSearched,lnAtPos-1)==LEFT(lcMatchExp, ;
						lnAtPos-1)
			RETURN
		ENDIF
	ENDFOR
	RETURN .F.
	ENDFUNC


	FUNCTION TrimPath(tcFileName,tlTrimExt)
	LOCAL lcFileName,lnAtPos

	IF EMPTY(tcFileName)
		RETURN ''
	ENDIF
	lcFileName=tcFileName
	lnAtPos=AT(':',lcFileName)
	IF lnAtPos>0
		lcFileName=SUBSTR(lcFileName,lnAtPos+1)
	ENDIF
	IF tlTrimExt
		lcFileName=this.TrimExt(lcFileName)
	ENDIF
	lcFileName=ALLTRIM(SUBSTR(lcFileName,AT('\',lcFileName,;
			MAX(OCCURS('\',lcFileName),1))+1))
	DO WHILE LEFT(lcFileName,1)=='.'
		lcFileName=ALLTRIM(SUBSTR(lcFileName,2))
	ENDDO
	DO WHILE RIGHT(lcFileName,1)=='.'
		lcFileName=ALLTRIM(LEFT(lcFileName,LEN(lcFileName)-1))
	ENDDO
	RETURN lcFileName
	ENDFUNC


	FUNCTION TrimExt(tcFileName)
	LOCAL lcFileName,lnAtPos,lnAtPos2

	lcFileName=tcFileName
	lnAtPos=RAT('.',lcFileName)
	IF lnAtPos>0
		lnAtPos2=RAT(':',lcFileName)
		IF lnAtPos>lnAtPos2
			lcFileName=LEFT(lcFileName,lnAtPos-1)
		ENDIF
	ENDIF
	RETURN ALLTRIM(lcFileName)
	ENDFUNC


	PROTECTED FUNCTION Error(tnError,tcMethod,tnLine)

	RETURN thisform.Error(tnError,tcMethod,tnLine,this)
	ENDFUNC


	PROTECTED FUNCTION FormatValue(tcProperty,tcValue)
	LOCAL lcValue,lcType

	lcType=TYPE('tcValue')
	DO CASE
		CASE lcType=='C'
			=.F.
		CASE lcType=='U'
			RETURN tcValue
		CASE lcType=='L'
			tcValue=IIF(tcValue,'.T.','.F.')
		CASE lcType=='N'
			tcValue=ALLTRIM(STR(tcValue))
		CASE lcType=='D'
			tcValue=DTOC(tcValue)
		CASE lcType=='T'
			tcValue=TTOC(tcValue)
		CASE lcType=='Y'
			tcValue=ALLTRIM(STR(MTON(tcValue)))
		OTHERWISE
			RETURN tcValue
	ENDCASE
	lcType=TYPE(tcValue)
	DO CASE
		CASE lcType=='N' OR lcType=='D' OR lcType=='T'
			lcValue=ALLTRIM(tcValue)
		CASE lcType=='L'
			lcValue=IIF(EVALUATE(tcValue),'.T.','.F.')
		CASE EMPTY(tcValue)
			lcValue=ALLTRIM(tcValue)
		CASE LEFT(tcValue,1)=='='
			lcValue='('+SUBSTR(tcValue,2)+')'
		CASE NOT LEFT(tcValue,1)=='"' AND ;
				INLIST(tcProperty,'caption','tag','comment','name','fontname', ;
				'controlsource','recordsource','format','inputmask', ;
				'statusbartext','tooltiptext','memowindow','passwordchar', ;
				'columnwidths','rowsource','lineslant','childorder') OR ;
				INLIST(tcProperty,'linkmaster','dynamicbackcolor', ;
				'dynamicforecolor','dynamicfontbold','dynamicfontitalic', ;
				'dynamicfontname','dynamicfontoutline','dynamicfontsize', ;
				'dynamicfontshadow','dynamicfontstrikethru', ;
				'dynamicfontunderline','dynamicalignment', ;
				'dynamiccurrentcontrol','hostname','relationalexpr')
			lcValue='"'+tcValue+'"'
		OTHERWISE
			lcValue=ALLTRIM(tcValue)
	ENDCASE
	RETURN lcValue
	ENDFUNC


	PROTECTED FUNCTION DisplayMessage(tcText)
	
	IF ISNULL(tcText)
		SET MESSAGE TO
		RETURN
	ENDIF
	IF EMPTY(tcText)
		SET MESSAGE TO SCANNING_LOC
		RETURN
	ENDIF
	SET MESSAGE TO SCANNING2_LOC+tcText
	ENDFUNC


	PROTECTED FUNCTION NoFilesErrorMsg
	RETURN ShowMsgBox(NOFILESTOPROCESS_LOC,,this.parent.Caption)
	ENDFUNC
	
	
	PROTECTED FUNCTION ClearText
	
	IF this.lAddFontRules
		RETURN .F.
	ENDIF
	this.cText=''
	ENDFUNC


	PROTECTED FUNCTION AddText(tcText,tnLines)
	LOCAL lnLines
	
	IF this.lAddFontRules
		RETURN .F.
	ENDIF
	lnLines=IIF(TYPE('tnLines')=='N',tnLines,0)
	this.cText=this.cText+tcText+REPLICATE(CR,lnLines)
	ENDFUNC
	
	
	PROTECTED FUNCTION LogToFile
	LOCAL lcFileName

	IF this.lAddFontRules
		RETURN .F.
	ENDIF
	lcFileName=this.cLogToFile
	IF NOT this.lLogToFile OR EMPTY(lcFileName)
		RETURN .F.
	ENDIF
	SET TEXTMERGE OFF
	SET TEXTMERGE TO (lcFileName)
	IF NOT FILE(lcFileName)
		SET TEXTMERGE OFF
		RETURN .F.
	ENDIF
	SET TEXTMERGE ON NOSHOW
	TEXT
	<<this.cText+CR>>
	ENDTEXT
	SET TEXTMERGE OFF
	SET TEXTMERGE TO
	ENDFUNC


	PROTECTED FUNCTION HeaderText

	this.TitleText(LOGSTART_LOC)
	this.LineBreakText()
	ENDFUNC
	

	PROTECTED FUNCTION FooterText

	this.LineBreakText()
	this.TitleText(LOGEND_LOC)
	ENDFUNC
	

	PROTECTED FUNCTION TitleText(tcText)

	this.AddText('*** '+tcText+' ***  '+TTOC(DATETIME()),1)
	ENDFUNC


	PROTECTED FUNCTION UnableToProcessFileText(tcFileName)

	this.AddText(NOPROCESS_LOC+tcFileName,1)
	ENDFUNC


	PROTECTED FUNCTION LineBreakText

	this.AddText(REPLICATE('_',65),2)
	ENDFUNC
	

	PROTECTED FUNCTION DisplayText

	IF this.lAddFontRules
		RETURN .F.
	ENDIF
	this.parent.pgfTransformer.fpgLog.edtTransformerLog.Value=this.cText
	ENDFUNC
	

	PROTECTED FUNCTION InitalizeRules
	LOCAL lcItem,lnCount,lcFiles,lcClasses

	this.nRuleCount=this.parent.pgfTransformer.fpgRules.lstSelectedRules. ;
			ListCount
	DIMENSION this.aRules[MAX(this.nRuleCount,1)]
	this.aRules=''
	this.nFileRuleCount=0
	DIMENSION this.aFileRule[1]
	this.aFileRule=''
	DIMENSION this.aExcludeFilesRule[1]
	this.aExcludeFilesRule=.F.
	this.nClassRuleCount=0
	DIMENSION this.aClassRule[1]
	this.aClassRule=''
	DIMENSION this.aExcludeClassesRule[1]
	this.aExcludeClassesRule=.F.
	this.lOverridePropertyDefaults=.F.
	FOR lnCount = 1 TO this.nRuleCount
		lcItem=this.parent.pgfTransformer.fpgRules. ;
				lstSelectedRules.List[lnCount]
		this.aRules[lnCount]=lcItem
		this.parent.GetRule(lcItem)
		IF NOT EMPTY(this.parent.aRule[1])
			LOOP
		ENDIF
		lcFiles=LOWER(this.parent.aRule[4])
		lcClasses=LOWER(this.parent.aRule[6])
		IF NOT EMPTY(lcFiles)
			this.nFileRuleCount=this.nFileRuleCount+1
			DIMENSION this.aFileRule[this.nFileRuleCount]
			DIMENSION this.aExcludeFilesRule[this.nFileRuleCount]
			this.aFileRule[this.nFileRuleCount]=lcFiles
			this.aExcludeFilesRule[this.nFileRuleCount]=this.parent.aRule[5]
		ENDIF
		IF NOT EMPTY(lcClasses)
			this.nClassRuleCount=this.nClassRuleCount+1
			DIMENSION this.aClassRule[this.nClassRuleCount]
			DIMENSION this.aExcludeClassesRule[this.nClassRuleCount]
			this.aClassRule[this.nClassRuleCount]=lcClasses
			this.aExcludeClassesRule[this.nClassRuleCount]=this.parent.aRule[7]
		ENDIF
		IF this.parent.aRule[8]
			this.lOverridePropertyDefaults=.T.
		ENDIF
	ENDFOR
	this.nFontMatchCount=0
	ENDFUNC


	PROTECTED FUNCTION ProcessStart
	LOCAL lnFileNo,lcFileName

	this.DisplayMessage()
	this.ClearText()
	this.DisplayText()
	IF this.lAddFontRules
		this.parent.pgfTransformer.ActivePage= ;
				this.parent.pgfTransformer.fpgRules.PageOrder
		this.parent.pgfTransformer.fpgRules.lstSelectedRules.SetFocus()
		this.parent.LockScreen=.T.
	ELSE
		this.parent.pgfTransformer.ActivePage= ;
				this.parent.pgfTransformer.fpgLog.PageOrder
		this.parent.pgfTransformer.fpgLog.edtTransformerLog.SetFocus()
	ENDIF
	this.cFileName=''
	this.nProcessedFileCount=0
	DIMENSION this.aProcessedFiles[1]
	this.aProcessedFiles=''
	this.nTransformedFileCount=0
	DIMENSION this.aFileList[MAX(this.nFileCount,1)]
	this.aFileList=''
	FOR lnFileNo = 1 TO this.nFileCount
		lcFileName=ALLTRIM(this.parent.pgfTransformer.fpgFiles. ;
				lstSelectedFiles.List[lnFileNo])
		this.aFileList[lnFileNo]=lcFileName
	ENDFOR
	this.HeaderText()
	ENDFUNC
	

	PROTECTED FUNCTION ProcessEnd

	this.FooterText()
	this.LogToFile()
	this.DisplayMessage(.NULL.)
	ENDFUNC


	PROTECTED FUNCTION ProcessFileList
	LOCAL lnFileNo,lcFileName

	FOR lnFileNo = 1 TO this.nFileCount
		IF this.parent.lError
			RETURN .F.
		ENDIF
		lcFileName=this.aFileList[lnFileNo]
		IF FILE(lcFileName)
			IF NOT this.ProcessFile(lcFileName)
				RETURN .F.
			ENDIF
		ELSE
			IF NOT this.ProcessFolder(lcFileName)
				RETURN .F.
			ENDIF
		ENDIF
	ENDFOR
	ENDFUNC


	PROTECTED FUNCTION ProcessFolder(tcFolderName)
	LOCAL lnFileCount,lnFileNo,lcFileName,lcAttrib
	LOCAL laFiles[1,5]

	IF this.parent.lError OR (CHRSAW() AND INKEY('HM')=27)
		RETURN .F.
	ENDIF
	this.DisplayMessage(tcFolderName)
	lnFileCount=ADIR(laFiles,tcFolderName+'*.*','D')
	IF lnFileCount=0
		RETURN
	ENDIF
	FOR lnFileNo = 1 TO lnFileCount
		lcFileName=laFiles[lnFileNo,1]
		lcAttrib=laFiles[lnFileNo,5]
		IF EMPTY(lcFileName) OR LEFT(lcFileName,1)=='.'
			LOOP
		ENDIF
		lcFileName=LOWER(tcFolderName+lcFileName)
		IF 'D'$lcAttrib
			IF NOT this.lSearchSubfolders
				LOOP
			ENDIF
			lcFileName=lcFileName+'\'
			IF NOT this.ProcessFolder(lcFileName)
				RETURN .F.
			ENDIF
		ELSE
			IF NOT this.ProcessFile(lcFileName)
				RETURN .F.
			ENDIF
		ENDIF
	ENDFOR
	ENDFUNC


	PROTECTED FUNCTION ProcessFile(tcFileName)
	LOCAL lcFileName,lcFileExt,lcFileExt2,lcFileName,lcAttrib
	LOCAL lcObjName,lcBaseClass,lcClasses,lnRecNo,lcGetFileList
	LOCAL lnLastSelect,lcAlias,lnCount,llMatch,llWildCardMatch
	LOCAL laFiles[1,5]

	IF this.parent.lError OR (CHRSAW() AND INKEY('HM')=27)
		RETURN .F.
	ENDIF
	IF this.nProcessedFileCount>0 AND ;
			ASCAN(this.aProcessedFiles,tcFileName+MARKER)>0
		RETURN
	ENDIF
	lcFileName=LOWER(this.TrimPath(tcFileName))
	IF this.nFileRuleCount>0
		llMatch=.T.
		FOR lnCount = 1 TO this.nFileRuleCount
			llWildCardMatch=this.WildCardMatch(this.aFileRule[lnCount], ;
					lcFileName)
			IF this.aExcludeFilesRule[lnCount]
				llWildCardMatch=(NOT llWildCardMatch)
			ENDIF
			IF NOT llWildCardMatch
				llMatch=.F.
				EXIT
			ENDIF
		ENDFOR
		IF NOT llMatch
			RETURN
		ENDIF
	ENDIF
	lcGetFileList=LOWER(STRTRAN('|'+this.cGetFileExt,'|','.'))
	lcFileExt=LOWER(RIGHT(tcFileName,4))
	IF NOT lcFileExt$lcGetFileList OR ADIR(laFiles,tcFileName)=0
		RETURN
	ENDIF
	this.nProcessedFileCount=this.nProcessedFileCount+1
	DIMENSION this.aProcessedFiles[this.nProcessedFileCount]
	this.aProcessedFiles[this.nProcessedFileCount]=tcFileName+MARKER
	lcAttrib=laFiles[1,5]
	IF NOT lcFileExt=='.pjx' AND LEFT(lcAttrib,1)=='R'
		this.AddText('Unabled to process read-only file:  '+tcFileName,1)
		RETURN
	ENDIF
	lnLastSelect=SELECT()
	lcAlias='_'+SYS(3)
	IF USED(lcAlias)
		USE IN (lcAlias)
	ENDIF
	IF lcFileExt=='.pjx'
		SELECT 0
		USE (tcFileName) ALIAS (lcAlias)
		IF NOT USED(lcAlias)
			SELECT (lnLastSelect)
			this.UnableToProcessFileText(tcFileName)
			RETURN .F.
		ENDIF
		IF FCOUNT()#28
			USE IN (lcAlias)
			SELECT (lnLastSelect)
			RETURN
		ENDIF
		SET FILTER TO NOT DELETED()
		LOCATE
		this.DisplayMessage(tcFileName)
		this.AddText(PROJECT_LOC+tcFileName,1)
		SCAN ALL FOR NOT Type=='H' AND NOT EMPTY(Name)
			lcFileName=LOWER(ALLTRIM(STRTRAN(MLINE(Name,1),CHR(0),'')))
			lcFileExt2=LOWER(RIGHT(lcFileName,4))
			IF NOT lcFileExt2$lcGetFileList
				LOOP
			ENDIF
			lcFileName=LOWER(FULLPATH(lcFileName,tcFileName))
			this.ProcessFile(lcFileName)
		ENDSCAN
		USE IN (lcAlias)
		SELECT (lnLastSelect)
		RETURN
	ENDIF
	SELECT 0
	USE (tcFileName) ALIAS (lcAlias)
	IF NOT USED(lcAlias)
		SELECT (lnLastSelect)
		this.UnableToProcessFileText(tcFileName)
		RETURN .F.
	ENDIF
	SET FILTER TO NOT DELETED()
	LOCATE
	IF (INLIST(lcFileExt,'.vcx','.scx','.mnx') AND FCOUNT()#23) OR ;
			(lcFileExt=='.frx' AND FCOUNT()#75)
		USE IN (lcAlias)
		SELECT (lnLastSelect)
		RETURN
	ENDIF
	this.DisplayMessage(tcFileName)
	this.nTransformedFileCount=this.nTransformedFileCount+1
	this.cFileName=tcFileName
	DO CASE
		CASE lcFileExt=='.vcx'
			this.AddText(CLASSLIB_LOC+tcFileName,1)
		CASE lcFileExt=='.scx'
			this.AddText(FORM_LOC+tcFileName,1)
		CASE lcFileExt=='.mnx'
			this.AddText(MENU_LOC+tcFileName,1)
		CASE lcFileExt=='.frx'
			this.AddText(REPORT_LOC+tcFileName,1)
		OTHERWISE
			SELECT (lnLastSelect)
			this.cFileName=''
			this.UnableToProcessFileText(tcFileName)
			RETURN .F.
	ENDCASE
	SCAN ALL
		IF lcFileExt=='.mnx'
			this.ProcessMenuRules()
			LOOP
		ENDIF
		IF lcFileExt=='.frx'
			this.ProcessReportRules()
			LOOP
		ENDIF
		IF EMPTY(Platform) OR Platform=='COMMENT '
			LOOP
		ENDIF
		lcObjName=LOWER(ALLTRIM(MLINE(ObjName,1)))
		lcBaseClass=LOWER(ALLTRIM(MLINE(BaseClass,1)))
		IF INLIST(lcBaseClass,'dataenvironment','cursor','relation')
			LOOP
		ENDIF
		IF this.nClassRuleCount>0
			llMatch=.T.
			IF EMPTY(lcBaseClass)
				LOOP
			ENDIF
			FOR lnCount = 1 TO this.nClassRuleCount
				lcClasses=LOWER(this.aClassRule[lnCount])
				llWildCardMatch=(this.WildCardMatch(lcClasses,lcBaseClass) OR ;
						this.WildCardMatch(lcClasses,lcObjName))
				IF this.aExcludeClassesRule[lnCount]
					llWildCardMatch=(NOT llWildCardMatch)
				ENDIF
				IF NOT llWildCardMatch
					llMatch=.F.
					EXIT
				ENDIF
			ENDFOR
			IF NOT llMatch
				LOOP
			ENDIF
		ENDIF
		IF NOT this.lAddFontRules
			IF NOT this.ProcessRules()
				USE IN (lcAlias)
				SELECT (lnLastSelect)
				this.cFileName=''
				RETURN .F.
			ENDIF
			LOOP
		ENDIF
		IF lcFileExt=='.scx'
			LOCATE FOR Platform=='COMMENT ' AND UniqueID=='FONTINFO  '
			IF NOT EOF()
				this.AddFontRules(lcFileName)
			ENDIF
			EXIT
		ENDIF
		lnRecNo=RECNO()
		LOCATE FOR LOWER(ALLTRIM(MLINE(ObjName,1)))==lcObjName AND ;
				Platform=='COMMENT ' AND UniqueID=='FONTINFO  '
		IF EOF()
			GO lnRecNo
			LOOP
		ENDIF
		this.AddFontRules(lcFileName,lcObjName)
		GO lnRecNo
	ENDSCAN
	USE IN (lcAlias)
	SELECT (lnLastSelect)
	this.cFileName=''
	ENDFUNC


	PROTECTED FUNCTION AddFontRules(tcFileName,tcObjName)
	LOCAL lcFileName,lcObjName,lcFontInfo,lnFontStyle
	LOCAL lcProperties,oListBox,lcProperty,lcValue,lcCondition
	LOCAL lcFontName,lcFontStyle,lnFontSize,lnHeight,lnWidth
	LOCAL lcFontName2,lcFontStyle2,lnFontSize2,lnHeight2,lnWidth2
	LOCAL lnFM1,lnFM5,lnFM6
	LOCAL lnFontNo,lnFontSizeCount,lnFontSizeNo,lnAtPos
	LOCAL laFont[1],laFonts[1]

	IF NOT AFONT(laFonts)
		RETURN .F.
	ENDIF
	lcFileName=IIF(TYPE('tcFileName')=='C',tcFileName,'')
	lcObjName=IIF(TYPE('tcObjName')=='C',tcObjName,'')
	oListBox=this.parent.pgfTransformer.fpgRules.lstSelectedRules
	lcProperties=STRTRAN(ALLTRIM(Properties),LF,CR_LF)
	lcCondition=IIF(_windows,'_WINDOWS','_MAC')
	_mline=0
	DO WHILE .T.
		lcFontInfo=ALLTRIM(MLINE(lcProperties,1,_mline))
		IF EMPTY(lcFontInfo)
			EXIT
		ENDIF
		IF NOT RIGHT(lcFontInfo,1)==','
			lcFontInfo=lcFontInfo+','
		ENDIF
		lnAtPos=AT(',',lcFontInfo)
		IF lnAtPos<=1
			LOOP
		ENDIF
		lcFontName=ALLTRIM(LEFT(lcFontInfo,lnAtPos-1))
		lcFontInfo=ALLTRIM(SUBSTR(lcFontInfo,lnAtPos+1))
		lnAtPos=AT(',',lcFontInfo)
		IF lnAtPos<=1
			LOOP
		ENDIF
		lnFontStyle=VAL(LEFT(lcFontInfo,lnAtPos-1))
		IF lnFontStyle=0
			lcFontStyle='N'
		ELSE
			lcFontStyle=''
			IF BITTEST(lnFontStyle,0)
				lcFontStyle=lcFontStyle+'B'
			ENDIF
			IF BITTEST(lnFontStyle,1)
				lcFontStyle=lcFontStyle+'I'
			ENDIF
		ENDIF
		lcFontInfo=ALLTRIM(SUBSTR(lcFontInfo,lnAtPos+1))
		lnAtPos=AT(',',lcFontInfo)
		IF lnAtPos<=1
			LOOP
		ENDIF
		lnFontSize=VAL(LEFT(lcFontInfo,lnAtPos-1))
		lcFontInfo=ALLTRIM(SUBSTR(lcFontInfo,lnAtPos+1))
		lnAtPos=AT(',',lcFontInfo)
		IF lnAtPos<=1
			LOOP
		ENDIF
		lnFM6=VAL(LEFT(lcFontInfo,lnAtPos-1))
		lcFontInfo=ALLTRIM(SUBSTR(lcFontInfo,lnAtPos+1))
		lnAtPos=AT(',',lcFontInfo)
		IF lnAtPos<=1
			LOOP
		ENDIF
		lnFM1=VAL(LEFT(lcFontInfo,lnAtPos-1))
		lcFontInfo=ALLTRIM(SUBSTR(lcFontInfo,lnAtPos+1))
		lnAtPos=AT(',',lcFontInfo,3)
		IF lnAtPos<=1
			LOOP
		ENDIF
		lcFontInfo=ALLTRIM(SUBSTR(lcFontInfo,lnAtPos+1))
		lnAtPos=AT(',',lcFontInfo)
		IF lnAtPos<=1
			LOOP
		ENDIF
		lnFM5=VAL(LEFT(lcFontInfo,lnAtPos-1))
		lcFontInfo=ALLTRIM(SUBSTR(lcFontInfo,lnAtPos+1))
		IF AFONT(laFont,lcFontName,lnFontSize)
			LOOP
		ENDIF
		lnHeight=FONTMETRIC(1,lcFontName,lnFontSize,lcFontStyle)+ ;
				FONTMETRIC(5,lcFontName,lnFontSize,lcFontStyle)
		lnWidth=FONTMETRIC(6,lcFontName,lnFontSize,lcFontStyle)
		FOR lnFontNo = 1 TO ALEN(laFonts)
			lcFontName2=laFonts[lnFontNo]
			IF NOT AFONT(laFont,lcFontName2)
				LOOP
			ENDIF
			lcFontStyle2=lcFontStyle
			IF laFont[1]>0
				lnFontSizeCount=ALEN(laFont)
				lnFontSizeNo=0
			ELSE
				lnFontSizeCount=0
				lnFontSizeNo=-1
			ENDIF
			DO WHILE .T.
				IF lnFontSizeCount>0
					lnFontSizeNo=lnFontSizeNo+1
					IF lnFontSizeNo>lnFontSizeCount
						EXIT
					ENDIF
					lnFontSize2=laFont[lnFontSizeNo]
				ELSE
					lnFontSizeNo=lnFontSizeNo+1
					IF lnFontSizeNo>8
						EXIT
					ENDIF
					IF lnFontSizeNo<=4
						lnFontSize2=lnFontSize+lnFontSizeNo
					ELSE
						lnFontSize2=lnFontSize-lnFontSizeNo
					ENDIF
				ENDIF
				lnHeight2=FONTMETRIC(1,lcFontName2,lnFontSize2,lcFontStyle2)+ ;
						FONTMETRIC(5,lcFontName2,lnFontSize2,lcFontStyle2)
				lnWidth2=FONTMETRIC(6,lcFontName2,lnFontSize2,lcFontStyle2)
				IF lnHeight2#lnHeight OR lnWidth2#lnWidth
					LOOP
				ENDIF
			ENDDO
		ENDFOR
		IF oListBox.AddItem(this.parent.AddRule('FontName',lcFontName, ;
				lcCondition,lcFileName,,lcObjName))
			this.nFontMatchCount=this.nFontMatchCount+1
		ENDIF
		IF oListBox.AddItem(this.parent.AddRule('FontSize', ;
				ALLTRIM(STR(lnFontSize)),lcCondition,lcFileName,,lcObjName))
			this.nFontMatchCount=this.nFontMatchCount+1
		ENDIF
	ENDDO
	ENDFUNC


	PROTECTED FUNCTION ProcessRules
	LOCAL lcItem,lcFileName,lcObjName,lcClass,lcBaseClass,lcParent
	LOCAL llWildCardMatch,lnRuleNo,lnCount,lcText,lcObjectName,lnObjectCount
	LOCAL lnPropertyAtPos,lnNameAtPos,lnAtPos,lnOccurance,lnStartPos,lnEndPos
	LOCAL lcProperties,lcMembers,lcMember,lcOldExpr,lcNewExpr
	LOCAL lcProperty,lcValue,lcCondition,lcFiles,llExcludeFiles
	LOCAL lcClasses,llExcludeClasses,llOverridePropertyDefaults
	LOCAL laObjects[1]

	lcFileName=LOWER(this.TrimPath(this.cFileName))
	lcObjName=LOWER(ALLTRIM(MLINE(ObjName,1)))
	lcClass=LOWER(ALLTRIM(MLINE(Class,1)))
	lcBaseClass=LOWER(ALLTRIM(MLINE(BaseClass,1)))
	lcParent=LOWER(ALLTRIM(MLINE(Parent,1)))
	FOR lnRuleNo = 1 TO this.nRuleCount
		lcItem=this.aRules[lnRuleNo]
		this.parent.GetRule(lcItem)
		lcProperty=LOWER(this.parent.aRule[1])
		lcValue=this.parent.aRule[2]
		IF EMPTY(lcProperty) OR EMPTY(lcValue)
			LOOP
		ENDIF
		lcCondition=this.parent.aRule[3]
		lcFiles=LOWER(this.parent.aRule[4])
		llExcludeFiles=this.parent.aRule[5]
		lcClasses=LOWER(this.parent.aRule[6])
		llExcludeClasses=this.parent.aRule[7]
		llOverridePropertyDefaults=(this.lOverridePropertyDefaults OR this.parent.aRule[8])
		IF NOT EMPTY(lcFiles)
			llWildCardMatch=this.WildCardMatch(lcFiles,lcFileName)
			IF llExcludeFiles
				llWildCardMatch=(NOT llWildCardMatch)
			ENDIF
			IF NOT llWildCardMatch
				LOOP
			ENDIF
		ENDIF
		IF NOT EMPTY(lcClasses)
			llWildCardMatch=(this.WildCardMatch(lcClasses,lcBaseClass) OR ;
					this.WildCardMatch(lcClasses,lcObjName))
			IF llExcludeClasses
				llWildCardMatch=(NOT llWildCardMatch)
			ENDIF
			IF NOT llWildCardMatch
				LOOP
			ENDIF
		ENDIF
		lcValue=this.FormatValue(lcProperty,lcValue)
		lcProperties=ALLTRIM(Properties)
		lcMembers=ALLTRIM(Reserved3)
		DIMENSION laObjects[1]
		laObjects=''
		lnObjectCount=0
		lnPropertyAtPos=-1
		lnNameAtPos=-1
		lnOccurance=0
		DO WHILE lnPropertyAtPos#0 OR lnNameAtPos#0
			IF this.parent.lError OR (CHRSAW() AND INKEY('HM')=27)
				RETURN .F.
			ENDIF
			lcNewExpr=lcValue
			lnPropertyAtPos=0
			IF lnOccurance=0
				lnPropertyAtPos=ATC(CR_LF+lcProperty+' = ',CR_LF+lcProperties+CR_LF)
				lnOccurance=lnOccurance+1
				lcObjectName=''
			ELSE
				lnNameAtPos=ATC('.name = ',CR_LF+lcProperties+CR_LF, ;
						lnOccurance)
				lnOccurance=lnOccurance+1
				IF lnNameAtPos=0
					LOOP
				ENDIF
				lnNameAtPos=lnNameAtPos-1
				lnAtPos=RAT(CR_LF,LEFT(lcProperties,lnNameAtPos))
				lcObjectName=LOWER(SUBSTR(lcProperties,lnAtPos+2, ;
						lnNameAtPos-lnAtPos-3))
				IF lnObjectCount>0 AND ASCAN(laObjects,lcObjectName)>0
					LOOP
				ENDIF
				lnPropertyAtPos=ATC(CR_LF+lcObjectName+'.'+lcProperty+' = ', ;
						CR_LF+lcProperties+CR_LF)
				lnObjectCount=lnObjectCount+1
				DIMENSION laObjects[lnObjectCount]
				laObjects[lnObjectCount]=lcObjectName
			ENDIF
 			IF lnPropertyAtPos=0 AND NOT llOverridePropertyDefaults AND ;
					ATC(CR_LF+lcProperty+CR_LF,CR_LF+lcMembers+CR_LF)=0
				LOOP
			ENDIF
			lcText='    '+IIF(EMPTY(lcParent),'','    ')+lcObjName
			IF NOT EMPTY(lcObjectName)
				lcText=lcText+'.'+lcObjectName
			ENDIF
			lcText=lcText+' ('+lcBaseClass+'):  '+lcItem
			IF this.lCreateLogOnly
				this.AddText(lcText,1)
				LOOP
			ENDIF
			lcOldExpr=''
			lnStartPos=0
			lnEndPos=0
			IF lnPropertyAtPos>0
				lnStartPos=lnPropertyAtPos+LEN(lcProperty)-2
				IF NOT EMPTY(lcObjectName)
					lnStartPos=lnStartPos+LEN(lcObjectName)+1
				ENDIF
				DO WHILE .T.
					lcOldExpr=SUBSTR(lcProperties,lnStartPos)
					lnEndPos=AT(CR,lcOldExpr)
					IF lnEndPos>0
						EXIT
					ENDIF
					lcProperties=lcProperties+CR_LF
				ENDDO
				lcOldExpr=MLINE(SUBSTR(lcOldExpr,6),1)
				lnEndPos=lnStartPos+lnEndPos
			ENDIF
			IF NOT EMPTY(lcCondition)
				IF EMPTY(lcOldExpr)
					lcOldExpr=PROPER(IIF(EMPTY(lcClass),lcBaseClass, ;
							lcClass))+'::'+lcProperty
				ENDIF
				IF UPPER(LEFT(lcOldExpr,5))=='(IIF('
					lnAtPos=AT(',',lcOldExpr)
					IF lnAtPos>0
						IF LOWER(lcCondition)==LOWER(SUBSTR(lcOldExpr,6,lnAtPos-6))
							lnAtPos=RAT(',',lcOldExpr)
							IF lnAtPos>0
								lcOldExpr=SUBSTR(lcOldExpr,lnAtPos+1, ;
										LEN(lcOldExpr)-lnAtPos-2)
							ENDIF
						ENDIF
					ENDIF
				ENDIF
				IF lcNewExpr==lcOldExpr
					lcNewExpr=this.FormatValue(lcProperty,lcNewExpr)
				ELSE
					lcNewExpr='(IIF('+lcCondition+','+lcNewExpr+','+ ;
							lcOldExpr+'))'
				ENDIF
			ENDIF
			IF lnStartPos>0
				lcProperties=LEFT(lcProperties,lnStartPos+4)+lcNewExpr+ ;
						SUBSTR(lcProperties,lnEndPos-1)
			ELSE
				IF EMPTY(lcObjectName)
					lnAtPos=ATC(CR_LF+'name = ',CR_LF+lcProperties+CR_LF)
					lcObjectName=lcProperty
				ELSE
					lnAtPos=ATC(CR_LF+lcObjectName+'.name = ', ;
							CR_LF+lcProperties+CR_LF)
					lcObjectName=lcObjectName+'.'+lcProperty
				ENDIF
				IF lnAtPos>0
					lcProperties=LEFT(lcProperties,lnAtPos-1)+ ;
							lcObjectName+' = '+lcNewExpr+CR_LF+ ;
							SUBSTR(lcProperties,lnAtPos)
				ELSE
					lcProperties=lcProperties+lcObjectName+' = '+lcNewExpr+CR_LF
				ENDIF
			ENDIF
		ENDDO
		DO WHILE LEFT(lcProperties,2)==CR_LF
			lcProperties=SUBSTR(lcProperties,3)
		ENDDO
		IF NOT Properties==lcProperties
			REPLACE Properties WITH lcProperties
			this.AddText(lcText,1)
		ENDIF
	ENDFOR
	ENDFUNC


	PROTECTED FUNCTION ProcessMenuRules
	ENDFUNC


	PROTECTED FUNCTION ProcessReportRules
	ENDFUNC


ENDDEFINE

*-- end TRANSFRM.PRG
