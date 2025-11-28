* GenHTML.PRG - HTML Generator.
*
* Copyright (c) 1998 Microsoft Corp.
* 1 Microsoft Way
* Redmond, WA 98052
*
* Description:
* HTML generator using classes in _HTML.VCX.
*
* Parameter list:
* cOutFile:			Specifies the name of the output .HTM file.  If a file name without
*					an extention is specified, .HTM is used.
* vSource:			Specifies the source file name, alias or object.
* nShow: 			0/.F./Empty = Generate output file only.
*					1 = Create output file and view generated file.
*					2 = Create output file and show generated file in Internet Explorer.
*					3 = Create output file and show based on Save As HTML dialog selection.
*					4 = Create PUBLIC _oHTML object and generate file.
*					5 = Create PUBLIC _oHTML object without generating file.
* vIELink:			Specifies a link to Internet Explorer object or Web Browser control.
*					.F./Empty = No link is created.
*					.T. = Automatically create instance of Internet Explorer.
*					Object = Reference to Internet Explorer or Web Browser control.
* cStyle:			Specifies Style ID reference in GenHTML.dbf.
* cScope:			Specifies the scope of scan by setting .cScope.
* cHTMLClass:		Specifies the Class, and optionally, the class library and module,
*					that is	instantiated for HTML object.
*					Syntax: Class[,ClassLibrary[,Module]]


*-- Types
#DEFINE VFP_DEFAULT_ID				"VFPDefault"

*-- Messages
#DEFINE M_CLASS_LOC					"Class"
#DEFINE M_COULD_NOT_BE_INST_LOC		"could not be instantiated"
#DEFINE M_COULD_NOT_OPENED_EXCL_LOC	"could not be opened exclusively to update the table structure."
#DEFINE M_FILE_LOC					"File"
#DEFINE M_FILE_ALREADY_EXISTS_LOC	"This file already exists."
#DEFINE M_FILE_TYPE_LOC				"File type"
#DEFINE M_GENHTML_LOC				"GenHTML"
#DEFINE M_INVALID_SOURCE_REF_LOC	"Invalid source reference"
#DEFINE M_NOT_FOUND_LOC				"not found"
#DEFINE M_NOT_SUPPORTED_LOC			"not supported"
#DEFINE M_OF_LOC					"of"
#DEFINE M_PROPERTIES_LOC			"Properties"
#DEFINE M_STYLE_LOC					"style"
#DEFINE M_REPLACE_EXISTING_FILE_LOC	"Replace existing file"
#DEFINE M_UNABLE_TO_CREATE_FILE_LOC	"Unable to create file"
#DEFINE M_UNABLE_TO_FIND_LOC		"Unable to find"
#DEFINE M_UNABLE_TO_OPEN_FILE_LOC	"Unable to open file"

*-- ASCII codes
#DEFINE TAB		CHR(9)
#DEFINE LF		CHR(10)
#DEFINE CR		CHR(13)
#DEFINE CR_LF	CR+LF


LPARAMETERS tcOutFile,tvSource,tnShow,tvIELink,tcStyle,tcScope,tcHTMLClass
LOCAL lcOutFile,lvSource,oSource,lnShow,lcStyle,lcScope,lcHTMLClass,lcHTMLClassLib
LOCAL lcHTMLModule,lcHTMLVCX,lnSourceListCount,lcLastOnError,llSaveAsHTML,lcText
LOCAL oRecord,lcGenHTMLTable,lcGenHTMLAlias,lnGenHTMLRecNo,oSaveEnvironment
LOCAL lcProgramPath,lcIELinkType,llIELink,oSaveAsHTMLForm,oSaveAsHTML,lnCount
LOCAL lcTitle,lcSourceVarType,lcSourceFileExt,ll_oHTMLPublic,llSuccessful,lnAtPos
LOCAL laSourceList[1,1],laSelObj[1],laLines[1]

oSaveEnvironment=NEWOBJECT("_SaveEnvironment")
lcProgramPath=JUSTPATH(LOWER(SYS(16)))+"\"
lcHTMLVCX=IIF(VERSION(2)=0,"",HOME()+"FFC\")+"_HTML.vcx"
lcOutFile=IIF(VARTYPE(tcOutFile)=="C",LOWER(ALLTRIM(tcOutFile)),"")
IF NOT EMPTY(lcOutFile) AND EMPTY(JUSTEXT(lcOutFile))
	lcOutFile=FORCEEXT(lcOutFile,"htm")
ENDIF
lnShow=IIF(VARTYPE(tnShow)=="N" OR VARTYPE(tnShow)=="I",MIN(MAX(INT(tnShow),0),5),0)
lcSourceVarType=VARTYPE(tvSource)
DO CASE
	CASE lcSourceVarType=="C"
		lvSource=ALLTRIM(tvSource)
	CASE lcSourceVarType=="O"
		lvSource=tvSource
	OTHERWISE
		lvSource=""
		lcSourceVarType="C"
ENDCASE
laSourceList=""
lnSourceListCount=0
IF lcSourceVarType=="C"
	IF EMPTY(lvSource) AND lnShow#5
		lvSource=LOWER(GETFILE("dbf;frx;lbx;mnx;scx"))
		IF EMPTY(lvSource)
			RETURN .NULL.
		ENDIF
	ELSE
		IF NOT CR$lvSource
			lvSource=STRTRAN(lvSource,",",CR)
		ENDIF
		IF MEMLINES(lvSource)>=2
			lnSourceListCount=ALINES(laLines,lvSource)
			lvSource=laLines[1]
			lnSourceListCount=lnSourceListCount-1
			ADEL(laLines,1)
			DIMENSION laLines[lnSourceListCount]
			lnCount=0
			DO WHILE .T.
				lnCount=lnCount+1
				IF lnCount>lnSourceListCount
					EXIT
				ENDIF
				IF NOT EMPTY(laLines[lnCount])
					LOOP
				ENDIF
				ADEL(laLines,lnCount)
				lnSourceListCount=lnSourceListCount-1
			ENDDO
			IF lnSourceListCount>=1
				DIMENSION laSourceList[lnSourceListCount,2]
				FOR lnCount = 1 TO lnSourceListCount
					lcText=ALLTRIM(laLines[lnCount])
					lnAtPos=AT("@",lcText)
					IF lnAtPos>0
						laSourceList[lnCount,1]=ALLTRIM(SUBSTR(lcText,lnAtPos+1))
						laSourceList[lnCount,2]=ALLTRIM(LEFT(lcText,lnAtPos-1))
					ELSE
						laSourceList[lnCount,1]=lcText
						laSourceList[lnCount,2]=""
					ENDIF
				ENDFOR
			ENDIF
		ENDIF
	ENDIF
	lcSourceFileExt=LOWER(JUSTEXT(lvSource))
	IF NOT "!"$lvSource AND NOT EMPTY(lcSourceFileExt)
		lvSource=LOWER(FULLPATH(lvSource))
	ENDIF
ENDIF
lcIELinkType=VARTYPE(tvIELink)
llIELink=INLIST(lcIELinkType,"L","O")
lcStyle=IIF(VARTYPE(tcStyle)=="C",LOWER(ALLTRIM(tcStyle)),"")	
lcScope=IIF(VARTYPE(tcScope)=="C",LOWER(ALLTRIM(tcScope)),"ALL")
lcHTMLClass=IIF(VARTYPE(tcHTMLClass)=="C",LOWER(ALLTRIM(tcHTMLClass)),"")
llSaveAsHTML=(lnShow=3)
lcGenHTMLTable=lcProgramPath+"GenHTML.dbf"
lcGenHTMLAlias=LOWER(SYS(2015))
SELECT 0
lcLastOnError=ON("ERROR")
ON ERROR =.F.
USE (lcGenHTMLTable) AGAIN SHARED ALIAS (lcGenHTMLAlias)
IF EMPTY(lcLastOnError)
	ON ERROR
ELSE
	ON ERROR &lcLastOnError
ENDIF
IF NOT _CheckGenHTMLTableStructure(lcGenHTMLTable,lcGenHTMLAlias,lcHTMLVCX)
	RETURN .NULL.
ENDIF
SET FILTER TO NOT DELETED()
LOCATE
oSaveEnvironment.cGenHTMLTable=lcGenHTMLTable
oSaveEnvironment.cGenHTMLAlias=lcGenHTMLAlias
IF EMPTY(lcStyle)
	lcStyle=LOWER(VFP_DEFAULT_ID)
ENDIF
LOCATE FOR LOWER(ALLTRIM(ID))==lcStyle
IF EOF()
	USE
	_MsgBox(M_UNABLE_TO_FIND_LOC+[ ]+M_STYLE_LOC+[ "]+lcStyle+[".])
	RETURN .NULL.
ENDIF
lnGenHTMLRecNo=RECNO()
oSaveEnvironment.nGenHTMLRecNo=lnGenHTMLRecNo
SCATTER MEMO NAME oRecord
_EvalLinks(Links,oRecord)
GO lnGenHTMLRecNo
SELECT 0
IF EMPTY(lcHTMLClass) AND NOT EMPTY(oRecord.ClassName)
	lcHTMLClass=LOWER(ALLTRIM(MLINE(oRecord.ClassName,1)))
ENDIF
IF EMPTY(lcHTMLClassLib) AND NOT EMPTY(oRecord.ClassLib)
	lcHTMLClassLib=LOWER(ALLTRIM(MLINE(oRecord.ClassLib,1)))
ENDIF
IF EMPTY(lcHTMLModule) AND NOT EMPTY(oRecord.Module)
	lcHTMLModule=LOWER(ALLTRIM(MLINE(oRecord.Module,1)))
ENDIF
IF lnShow=3
	oSaveAsHTMLForm=NEWOBJECT("_HTMLSaveAsDialog",lcHTMLVCX)
	IF VARTYPE(oSaveAsHTMLForm)#"O"
		RETURN .NULL.
	ENDIF
	oSaveAsHTML=NEWOBJECT("Custom")
	oSaveAsHTML.AddProperty("cOutFile","")
	oSaveAsHTML.AddProperty("nShow",0)
	WITH oSaveAsHTMLForm
		.oSaveAsHTML=oSaveAsHTML
		.txtOutFile.Value=lcOutFile
		.Show
		lcOutFile=.oSaveAsHTML.cOutFile
		lnShow=.oSaveAsHTML.nShow
		IF EMPTY(lcOutFile)
			RETURN .NULL.
		ENDIF
	ENDWITH
	IF EMPTY(lcOutFile)
		RETURN .NULL.
	ENDIF
ENDIF
IF (llSaveAsHTML OR oSaveEnvironment.cLastSetSafety=="ON") AND ;
		NOT EMPTY(lcOutFile) AND FILE(lcOutFile) AND ;
	_MsgBox(lcOutFile+CR_LF+M_FILE_ALREADY_EXISTS_LOC+CR_LF+CR_LF+ ;
			M_REPLACE_EXISTING_FILE_LOC+"?",292)#6
	RETURN .NULL.
ENDIF
ll_oHTMLPublic=(TYPE("_oHTML")#"U")
IF ll_oHTMLPublic AND VARTYPE(_oHTML)=="O" AND PEMSTATUS(_oHTML,"Release",5)
	_oHTML.Release
ENDIF
RELEASE _oHTML
IF ll_oHTMLPublic OR lnShow=4 OR lnShow=5
	ll_oHTMLPublic=.T.
	PUBLIC _oHTML
ELSE
	PRIVATE _oHTML
ENDIF
_oHTML=.NULL.
lcHTMLModule=""
lnAtPos=AT(",",lcHTMLClass)
IF lnAtPos>0
	lcHTMLClassLib=ALLTRIM(SUBSTR(lcHTMLClass,lnAtPos+1))
	lcHTMLClass=ALLTRIM(LEFT(lcHTMLClass,lnAtPos-1))
	lnAtPos=AT(",",lcHTMLClassLib)
	IF lnAtPos>0
		lcHTMLModule=ALLTRIM(SUBSTR(lcHTMLClassLib,lnAtPos+1))
		lcHTMLClassLib=ALLTRIM(LEFT(lcHTMLClassLib,lnAtPos-1))
	ENDIF
ELSE
	IF EMPTY(lcHTMLClassLib)
		lcHTMLClassLib=LOWER(lcHTMLVCX)
	ENDIF
ENDIF
IF LEFT(lcHTMLClass,1)=="(" AND RIGHT(lcHTMLClass,1)==")"
	lcHTMLClass=EVALUATE(SUBSTR(lcHTMLClass,2,LEN(lcHTMLClass)-2))
ENDIF
IF LEFT(lcHTMLClassLib,1)=="(" AND RIGHT(lcHTMLClassLib,1)==")"
	lcHTMLClassLib=EVALUATE(SUBSTR(lcHTMLClassLib,2,LEN(lcHTMLClassLib)-2))
ENDIF
IF LEFT(lcHTMLModule,1)=="(" AND RIGHT(lcHTMLModule,1)==")"
	lcHTMLModule=EVALUATE(SUBSTR(lcHTMLModule,2,LEN(lcHTMLModule)-2))
ENDIF
IF NOT EMPTY(lcHTMLClassLib) AND EMPTY(JUSTEXT(lcHTMLClassLib))
	lcHTMLClassLib=FORCEEXT(lcHTMLClassLib,"vcx")
ENDIF
IF NOT EMPTY(lcHTMLModule) AND EMPTY(JUSTEXT(lcHTMLModule))
	lcHTMLModule=FORCEEXT(lcHTMLModule,"app")
ENDIF
IF NOT EMPTY(lcHTMLClassLib) AND NOT FILE(lcHTMLClassLib)
	_MsgBox(M_FILE_LOC+[ "]+lcHTMLClassLib+[" ]+M_NOT_FOUND_LOC+[.])
	RETURN .NULL.
ENDIF
IF NOT EMPTY(lcHTMLModule) AND NOT FILE(lcHTMLModule)
	_MsgBox(M_FILE_LOC+[ "]+lcHTMLModule+[" ]+M_NOT_FOUND_LOC+[.])
	RETURN .NULL.
ENDIF
oSource=.NULL.
DO CASE
	CASE NOT EMPTY(lcHTMLClass)
		=.F.
	CASE lcSourceVarType=="O"
		oSource=lvSource
		lcHTMLClass="_HTMLObject"
	CASE lcSourceVarType=="C" AND NOT EMPTY(lvSource)
		DO CASE
			CASE EMPTY(lcSourceFileExt) AND NOT EMPTY(lvSource)
				lcHTMLClass="_HTMLTable"
			CASE NOT FILE(lvSource)
				_MsgBox([File "]+lvSource+[" not found.])
				RETURN .NULL.
			CASE lcSourceFileExt=="dbf"
				lcHTMLClass="_HTMLTable"
			CASE lcSourceFileExt=="frx"
				lcHTMLClass="_HTMLReport"
			CASE lcSourceFileExt=="lbx"
				lcHTMLClass="_HTMLLabel"
			CASE lcSourceFileExt=="mnx"
				lcHTMLClass="_HTMLMenu"
			CASE lcSourceFileExt=="scx"
				lcHTMLClass="_HTMLObject"
				oSaveEnvironment.cWindow=LOWER(SYS(2015))
				DEFINE WINDOW (oSaveEnvironment.cWindow) FROM 0,0 TO 0,0 NONE
				MODIFY FORM (lvSource) IN WINDOW (oSaveEnvironment.cWindow) NOWAIT
				IF ASELOBJ(laSelObj,1)#1
					RELEASE WINDOW (oSaveEnvironment.cWindow)
					_MsgBox(M_UNABLE_TO_OPEN_FILE_LOC+[ "]+lvSource+[".])
					RETURN .NULL.
				ENDIF
				oSource=laSelObj[1]
				IF WVISIBLE(M_PROPERTIES_LOC)
					oSaveEnvironment.lWindow=.T.
					HIDE WINDOW (M_PROPERTIES_LOC)
				ENDIF
			OTHERWISE
				IF EMPTY(lvSource)
					_MsgBox(M_INVALID_SOURCE_REF_LOC+".")
				ELSE
					_MsgBox(M_FILE_TYPE_LOC+" "+lvSource+" "+M_NOT_SUPPORTED_LOC+".")
				ENDIF
				RETURN .NULL.
		ENDCASE
	OTHERWISE
		lcHTMLClass="_HTMLDocument"
ENDCASE
lcLastOnError=ON("ERROR")
ON ERROR =.F.
_oHTML=NEWOBJECT(lcHTMLClass,lcHTMLClassLib)
IF EMPTY(lcLastOnError)
	ON ERROR
ELSE
	ON ERROR &lcLastOnError
ENDIF
IF VARTYPE(_oHTML)#"O"
	_oHTML=.NULL.
	_MsgBox(M_CLASS_LOC+[ (]+lcHTMLClass+[) ]+M_OF_LOC+[ "]+LOWER(lcHTMLClassLib)+ ;
			[" ]+M_COULD_NOT_BE_INST_LOC+[.])
	RETURN .NULL.
ENDIF
IF lnShow#5 AND EMPTY(lcOutFile)
	lcOutFile=_oHTML.GetFile()
	IF EMPTY(lcOutFile)
		IF VARTYPE(_oHTML)=="O"
			_oHTML.Release
		ENDIF
		_oHTML=.NULL.
		RETURN .NULL.
	ENDIF
ENDIF
IF llIELink
	DO CASE
		CASE lcIELinkType=="L" AND tvIELink
			_oHTML.CreateIELink
		CASE lcIELinkType=="O"
			_oHTML.IE=tvIELink
	ENDCASE
ENDIF
_oHTML.oRecord=oRecord
_oHTML.cGenHTMLTable=lcGenHTMLTable
_oHTML.cGenHTMLAlias=lcGenHTMLAlias
_oHTML.lMessageBar=.T.
_oHTML.cOutFile=lcOutFile
_oHTML.oSource=oSource
_oHTML.cSourceFile=IIF(VARTYPE(lvSource)=="C",lvSource,"")
_oHTML.nSourceListCount=lnSourceListCount
ACOPY(laSourceList,_oHTML.aSourceList)
_oHTML.cScope=lcScope
_oHTML.oProperties=_oHTML.NewTag()
_oHTML.oProperties.SetProperties(oRecord.Properties)
_oHTML.RunCode(_oHTML.oRecord.PreScript)
IF PEMSTATUS(_oHTML,"Head",5)
	_oHTML.Head.AddItem(_oHTML.oRecord.HeadStart)
	IF EMPTY(_oHTML.cSourceFile)
		IF VARTYPE(lvSource)=="O" AND PEMSTATUS(lvSource,"Name",5)
			lcTitle=ALLTRIM(TRANSFORM(lvSource.Name))
		ELSE
			lcTitle=""
		ENDIF
	ELSE
		lcTitle=_oHTML.cSourceFile
	ENDIF
	_oHTML.Head.Title.Item=lcTitle
ENDIF
IF PEMSTATUS(_oHTML,"Body",5)
	_oHTML.Body.AddItem(_oHTML.oRecord.BodyStart)
ENDIF
_oHTML.nWorkArea=oSaveEnvironment.nLastSelect
IF lnShow=5
	llSuccessful=.T.
ELSE
	llSuccessful=_oHTML.Generate()
ENDIF
SELECT 0
IF NOT llSuccessful
	IF VARTYPE(_oHTML)=="O"
		_oHTML.Release
	ENDIF
	_oHTML=.NULL.
	RETURN .NULL.
ENDIF
_oHTML.RunCode(_oHTML.oRecord.PostScript)
IF PEMSTATUS(_oHTML,"Head",5)
	IF NOT EMPTY(_oHTML.oRecord.Style)
		_oHTML.Head.CSS=_oHTML.Head.AddTag("style")
		_oHTML.Head.CSS.AddItem(_oHTML.oRecord.Style)
	ENDIF
	_oHTML.Head.AddItem(_oHTML.oRecord.HeadEnd)
ENDIF
IF PEMSTATUS(_oHTML,"Body",5) AND NOT ISNULL(_oHTML.Body)
	_oHTML.Body.AddItem(_oHTML.oRecord.BodyEnd)
ENDIF
DO CASE
	CASE lnShow=1
		llSuccessful=_oHTML.ViewSource()
	CASE lnShow=2
		llSuccessful=_oHTML.Show()
	CASE lnShow#5
		llSuccessful=_oHTML.SaveFile()
ENDCASE
IF VARTYPE(_oHTML)#"O"
	_oHTML=.NULL.
	RETURN .NULL.
ENDIF
IF NOT llSuccessful
	_MsgBox(M_UNABLE_TO_CREATE_FILE_LOC+[ "]+_oHTML.cOutFile+[".])
ENDIF
IF NOT ll_oHTMLPublic
	_oHTML.Release
	_oHTML=.NULL.
	RETURN .NULL.
ENDIF
RETURN _oHTML



FUNCTION _EvalLinks(tcLinks,toObject,tcType)
LOCAL lcLinks1,lcLinks2,lcLink,lnLinkTotal,lnCount,lnAtPos,lnLastRecNo

lcLinks1=_TransformLinks(tcLinks)
IF EMPTY(lcLinks1)
	RETURN ""
ENDIF
lnLastRecNo=IIF(EOF() OR RECNO()>RECCOUNT(),0,RECNO())
lcLinks2=""
lnLinkTotal=(OCCURS(";",lcLinks1)+1)
FOR lnCount = 1 TO lnLinkTotal
	IF lnCount<lnLinkTotal
		lnAtPos=AT(";",lcLinks1)
		lcLink=ALLTRIM(LEFT(lcLinks1,lnAtPos-1))
		lcLinks1=ALLTRIM(SUBSTR(lcLinks1,lnAtPos+1))
	ELSE
		lcLink=ALLTRIM(lcLinks1)
		lcLinks1=""
	ENDIF
	IF EMPTY(lcLink)
		LOOP
	ENDIF
	LOCATE FOR LOWER(ALLTRIM(ID))==LOWER(ALLTRIM(lcLink))
	IF NOT EOF() AND (EMPTY(tcType) OR ALLTRIM(UPPER(tcType))==ALLTRIM(UPPER(Type)))
		SCATTER MEMO NAME oNewObject
		_InheritProperties(toObject,oNewObject)
		lcLink=_EvalLinks(Links,toObject,tcType)
		IF EMPTY(lcLink)
			LOOP
		ENDIF
	ENDIF
	lcLinks2=lcLinks2+lcLink+";"
ENDFOR
IF lnLastRecNo>0
	GO lnLastRecNo
ENDIF
RETURN lcLinks2
ENDFUNC



FUNCTION _TransformLinks(tcLinks)
LOCAL lcLinks

IF EMPTY(tcLinks)
	RETURN ""
ENDIF
lcLinks=STRTRAN(STRTRAN(STRTRAN(STRTRAN(ALLTRIM(tcLinks),CR_LF,";"), ;
		LF,";"),CR,";"),",",";")
IF LEFT(lcLinks,1)==";"
	lcLinks=ALLTRIM(SUBSTR(lcLinks,2))
ENDIF
IF RIGHT(lcLinks,1)==";"
	lcLinks=ALLTRIM(LEFT(lcLinks,LEN(lcLinks)-1))
ENDIF
RETURN lcLinks
ENDFUNC



FUNCTION _InheritProperties(toObject,toNewObject)

IF EMPTY(toObject.Type) AND NOT EMPTY(toNewObject.Type)
	toObject.Type=toNewObject.Type
ENDIF
IF EMPTY(toObject.Text) AND NOT EMPTY(toNewObject.Text)
	toObject.Text=toNewObject.Text
ENDIF
IF EMPTY(toObject.Desc) AND NOT EMPTY(toNewObject.Desc)
	toObject.Desc=toNewObject.Desc
ENDIF
IF EMPTY(toObject.ClassName) AND NOT EMPTY(toNewObject.ClassName)
	toObject.ClassName=toNewObject.ClassName
ENDIF
IF EMPTY(toObject.ClassLib) AND NOT EMPTY(toNewObject.ClassLib)
	toObject.ClassLib=toNewObject.ClassLib
ENDIF
IF EMPTY(toObject.Module) AND NOT EMPTY(toNewObject.Module)
	toObject.Module=toNewObject.Module
ENDIF
IF EMPTY(toObject.Picture) AND NOT EMPTY(toNewObject.Picture)
	toObject.Picture=toNewObject.Picture
ENDIF
toObject.Properties=_InheritProperty(toObject.Properties,toNewObject.Properties)
toObject.HTML=_InheritProperty(toObject.HTML,toNewObject.HTML)
toObject.Style=_InheritProperty(toObject.Style,toNewObject.Style)
toObject.Script=_InheritProperty(toObject.Script,toNewObject.Script)
toObject.PreScript=_InheritProperty(toObject.PreScript,toNewObject.PreScript)
toObject.GenScript=_InheritProperty(toObject.GenScript,toNewObject.GenScript)
toObject.PostScript=_InheritProperty(toObject.PostScript,toNewObject.PostScript)
toObject.HeadStart=_InheritProperty(toObject.HeadStart,toNewObject.HeadStart)
toObject.BodyStart=_InheritProperty(toObject.BodyStart,toNewObject.BodyStart)
toObject.BodyEnd=_InheritProperty(toObject.BodyEnd,toNewObject.BodyEnd)
toObject.HeadEnd=_InheritProperty(toObject.HeadEnd,toNewObject.HeadEnd)
IF NOT EMPTY(toNewObject.BodyEnd)
	IF EMPTY(toObject.BodyEnd)
		toObject.BodyEnd=toNewObject.BodyEnd
	ELSE
		toObject.BodyEnd=toObject.BodyEnd+CR_LF+toNewObject.BodyEnd
	ENDIF
ENDIF
IF EMPTY(toObject.Comment) AND NOT EMPTY(toNewObject.Comment)
	toObject.Comment=toNewObject.Comment
ENDIF
IF EMPTY(toObject.User) AND NOT EMPTY(toNewObject.User)
	toObject.User=toNewObject.User
ENDIF
ENDFUNC



FUNCTION _InheritProperty(tcValue,tcNewValue)

IF EMPTY(tcNewValue)
	RETURN tcValue
ENDIF
IF EMPTY(tcValue)
	RETURN tcNewValue
ENDIF
IF RIGHT(tcNewValue,2)==CR_LF
	RETURN tcNewValue+tcValue
ENDIF
RETURN tcNewValue+CR_LF+tcValue
ENDFUNC



FUNCTION _CheckGenHTMLTableStructure(tcFileName,tcAlias,tcHTMLVCX)
LOCAL lcFileName,lcPath,lcAlias2,lcLastOnError,oRecord,lcID,lcVersion
LOCAL lcFileName2DBF,lcFileName2FPT,oHTMLCreateTable

lcFileName=LOWER(tcFileName)
lcPath=IIF(EMPTY(lcFileName),"",JUSTPATH(lcFileName)+"\")
IF RECCOUNT()=0
	USE
	oHTMLCreateTable=NEWOBJECT("_HTMLCreateTable",tcHTMLVCX,"")
	oHTMLCreateTable.CreateTable(lcFileName)
	lcLastOnError=ON("ERROR")
	ON ERROR =.F.
	USE (lcFileName) AGAIN SHARED ALIAS (tcAlias)
	IF EMPTY(lcLastOnError)
		ON ERROR
	ELSE
		ON ERROR &lcLastOnError
	ENDIF
	IF NOT USED()
		_MsgBox(M_UNABLE_TO_CREATE_FILE_LOC+[ "]+lcFileName+[".])
		RETURN .F.
	ENDIF
	RETURN
ENDIF
LOCATE FOR LOWER(ALLTRIM(ID))="vfpdefault"
lcVersion=IIF(TYPE(tcAlias+".Version")=="M",ALLTRIM(Version),"")
oHTMLCreateTable=NEWOBJECT("_HTMLCreateTable",tcHTMLVCX,"")
IF VARTYPE(oHTMLCreateTable)#"O" OR (FCOUNT()>=25 AND FIELD(25)=="SAVE" AND ;
		lcVersion>=oHTMLCreateTable.cTableVersion)
	RETURN
ENDIF
lcLastOnError=ON("ERROR")
ON ERROR =.F.
USE (lcFileName) EXCLUSIVE ALIAS (tcAlias)
IF EMPTY(lcLastOnError)
	ON ERROR
ELSE
	ON ERROR &lcLastOnError
ENDIF
IF NOT USED()
	_MsgBox(M_FILE_LOC+[ "]+lcFileName+[" ]+M_COULD_NOT_OPENED_EXCL_LOC)
	RETURN .F.
ENDIF
lcFileName2DBF=LOWER(FORCEPATH(FORCEEXT(lcFileName,"")+"__2",lcPath))
lcFileName2FPT=lcFileName2DBF+".fpt"
lcFileName2DBF=lcFileName2DBF+".dbf"
ERASE (lcFileName2DBF)
ERASE (lcFileName2FPT)
IF TYPE(tcAlias+".Save")=="L"
	COPY TO (lcFileName2DBF) FOR Save AND NOT LOWER(ALLTRIM(ID))="vfpdefault"
ENDIF
USE
ERASE (lcFileName)
oHTMLCreateTable.CreateTable(lcFileName)
lcLastOnError=ON("ERROR")
ON ERROR =.F.
USE (lcFileName) AGAIN SHARED ALIAS (tcAlias)
IF EMPTY(lcLastOnError)
	ON ERROR
ELSE
	ON ERROR &lcLastOnError
ENDIF
IF NOT USED()
	ERASE (lcFileName2DBF)
	ERASE (lcFileName2FPT)
	_MsgBox(M_UNABLE_TO_CREATE_FILE_LOC+[ "]+lcFileName+[".])
	RETURN .F.
ENDIF
IF NOT FILE(lcFileName2DBF)
	RETURN
ENDIF
lcAlias2=LOWER(SYS(2015))
SELECT 0
lcLastOnError=ON("ERROR")
ON ERROR =.F.
USE (lcFileName2DBF) EXCLUSIVE ALIAS (lcAlias2)
SCAN ALL
	lcID=LOWER(ALLTRIM(ID))
	SCATTER MEMO NAME oRecord
	SELECT (tcAlias)
	IF EMPTY(lcID)
		APPEND BLANK
	ELSE
		LOCATE FOR LOWER(ALLTRIM(ID))==lcID
		IF EOF()
			APPEND BLANK
		ENDIF
	ENDIF
	GATHER MEMO NAME oRecord
	SELECT (tcAlias)	
ENDSCAN
IF EMPTY(lcLastOnError)
	ON ERROR
ELSE
	ON ERROR &lcLastOnError
ENDIF
USE
ERASE (lcFileName2DBF)
ERASE (lcFileName2FPT)
SELECT (tcAlias)
LOCATE
ENDFUNC



FUNCTION _MsgBox(tcMessage,tnType)
LOCAL lnType,lnResult,lnLastSelect

lnType=IIF(VARTYPE(tnType)=="N",tnType,16)
lnLastSelect=SELECT()
SELECT 0
WAIT CLEAR
lnResult=MESSAGEBOX(tcMessage,lnType,M_GENHTML_LOC)
WAIT CLEAR
SELECT (lnLastSelect)
RETURN lnResult
ENDFUNC



DEFINE CLASS _SaveEnvironment AS Custom


	cGenHTMLAlias=""
	nGenHTMLRecNo=0
	cGenHTMLTable=""
	nLastSelect=0
	nLastRecNo=0
	nLastSetMemoWidth=0
	cLastSetMessageBar=""
	cLastSetSafety=""
	cLastSetTalk=""
	cWindow=""
	lWindow=.F.


	FUNCTION Init
	this.cLastSetTalk=SET("TALK")
	SET TALK OFF
	this.cLastSetSafety=SET("SAFETY")
	SET SAFETY OFF
	this.nLastSetMemoWidth=SET("MEMOWIDTH")
	SET MEMOWIDTH TO 1024
	this.cLastSetMessageBar=SET("MESSAGE",1)
	SET MESSAGE TO ""
	this.nLastSelect=SELECT()
	this.nLastRecNo=IIF(EOF() OR RECNO()>RECCOUNT(),0,RECNO())
	ENDFUNC
	

	FUNCTION Destroy
	IF NOT EMPTY(this.cWindow) AND WEXIST(this.cWindow)
		IF this.lWindow
			SHOW WINDOW (M_PROPERTIES_LOC)
		ENDIF
		RELEASE WINDOW (this.cWindow)
	ENDIF
	SET MEMOWIDTH TO (this.nLastSetMemoWidth)
	IF USED(this.cGenHTMLAlias)
		USE IN (this.cGenHTMLAlias)
	ENDIF
	SELECT (this.nLastSelect)
	IF USED() AND this.nLastRecNo>0
		GO this.nLastRecNo
	ENDIF
	IF EMPTY(this.cLastSetMessageBar)
		SET MESSAGE TO
	ELSE
		SET MESSAGE TO (this.cLastSetMessageBar)
	ENDIF
	IF this.cLastSetSafety=="ON"
		SET SAFETY ON
	ELSE
		SET SAFETY OFF
	ENDIF
	IF this.cLastSetTalk=="ON"
		SET TALK ON
	ELSE
		SET TALK OFF
	ENDIF
	ENDFUNC


ENDDEFINE



*-- end GenHTML.PRG
