#DEFINE CRLF CHR(13)+CHR(10)
#DEFINE WWWCAPTION_LOC 'WWW Data Server'
#DEFINE NOFXTOOLS_LOC	'Missing FOXTOOLS.FLL'
#DEFINE NOIDC_LOC	"No .IDC file was specified.  The server cannot continue."
#DEFINE IDCNOTFOUND1_LOC	"Specified .IDC file ("
#DEFINE IDCNOTFOUND2_LOC	") not found.  The server cannot continue."
#DEFINE IDCBADDATA1_LOC ") does not contain correct data or cannot be accessed. The template entry could not be located.  The server cannot continue."
#DEFINE IDCBADDATA2_LOC	") does not contain correct data or cannot be accessed. The SQL statement entry could not be located.  The server cannot continue."
#DEFINE BADSQL_LOC "The SQL statement supplied by the IDC file could not be understood.  The server cannot continue."
#DEFINE NOSQL1_LOC	'The SQL statement FROM table ['
#DEFINE NOSQL2_LOC	'] not found.<BR>Table specified must be in PATH of data server specified in VFPIS.INI.<BR>'+CRLF+'SQL String: <HR>'
#DEFINE BADCMD_LOC	'The command generated an error.<BR>Please Contact the system administrator.<BR>'+CRLF+'SQL String: <HR>'
#DEFINE BADCONN1_LOC	"The connection to "
#DEFINE BADCONN2_LOC	" as "
#DEFINE BADCONN3_LOC	" could not be made.  The server cannot continue."
#DEFINE NOTEMPLATE_LOC 'The template file could not be located.  The server cannot continue.'
#DEFINE BADTEMPLATE1_LOC	'The template file ('
#DEFINE BADTEMPLATE2_LOC	') could not be opened successfully.  The server cannot continue.'
#DEFINE BADTEMPLATE3_LOC	") contained a mismatched BeginDetail/EndDetail structure.  The server cannot continue."
#DEFINE BADTEMPLATE4_LOC	"The detail line in "
#DEFINE BADTEMPLATE5_LOC	" contains a mismatched symbol structure.  The server cannot continue."
#DEFINE BADTEMPLATE6_LOC	"The template file contained an improperly formed IF construct.  The server cannot continue."
#DEFINE BADTEMPLATE7_LOC	" contained a mismatched If/EndIf structure.  The server cannot continue."
#DEFINE NODATASTREAM_LOC	"An error occurred trying to create your data stream.  The server cannot continue."
#DEFINE NOMATCHES_LOC		"No matches found."
#DEFINE RETURNED_LOC		"Returned"
#DEFINE FAILEDSEARCH_LOC	"FoxPro Search Failed"
#DEFINE FALSE_LOC	"FALSE"
#DEFINE TRUE_LOC	"TRUE"
#DEFINE ERROCCUR_LOC	"Error occured :  "
#DEFINE ERRMESS_LOC		"Error message :  "
#DEFINE ERRNUM_LOC		"Error number  :  "
#DEFINE ERRPROC_LOC		"Procedure name:  "
#DEFINE ERRLINE_LOC		"Line number   :  "
#DEFINE ERRALIAS_LOC	"Alias         :  "
#DEFINE ERRREC_LOC		"Record number :  "


LOCAL lcProgram,lcFullPath,lnAtPos,lcFoxTools,lcError,lcFileName
LOCAL lcScreenIcon,lcScreenCaption,lcSetPath,lnSelect
PRIVATE gcINIFile,gcHTTPRoot,gcScriptRoot,gcSemaphoreRoot,gcPath

SET TALK OFF
SET ESCAPE OFF
SET COLLATE TO 'MACHINE'
SET COMPATIBLE OFF
SET CONFIRM ON
SET DECIMALS TO 9
SET EXACT OFF
SET EXCLUSIVE OFF
SET MEMOWIDTH TO 1024
SET MULTILOCKS ON
SET POINT TO '.'
SET SAFETY OFF
SET UDFPARMS TO VALUE
SET MESSAGE TO ' '
lcProgram=SYS(16)
lnAtPos=RATC('\',lcProgram)
lcFullPath=LEFTC(lcProgram,lnAtPos)
CD (lcFullPath)
lcFoxTools='foxtools.fll'
IF NOT FILE(lcFoxTools)
	lcFoxTools=HOME()+lcFoxTools
ENDIF
IF NOT FILE(lcFoxTools)
	=MESSAGEBOX(NOFXTOOLS_LOC,16,_screen.Caption)
	RETURN .F.
ENDIF
ON ERROR
ERASE ERROR.txt
SET LIBRARY TO (lcFoxTools) ADDITIVE

lnSelect=SELECT()
lcSetPath=SET('PATH')
lcOnError=ON('ERROR')
lcScreenIcon=_screen.Icon
_screen.Icon='net13.ico'
lcScreenCaption=_screen.Caption
_screen.Caption=WWWCAPTION_LOC 

gcINIFile="vfpis.ini"
gcHTTPRoot=""
gcScriptRoot=""
gcSemaphoreRoot=FULLPATH('\temp\')
ON ERROR =.F.
MD (gcSemaphoreRoot)
ON ERROR
gcPath=""

ON ERROR DO ErrorHandler WITH ERROR(),MESSAGE(),PROGRAM(),LINENO(),MESSAGE(1)

*Read the initialization file and set up root paths.  If the
*INI file doesn't exist or is empty, ask the user to set one up.

IF FILE(gcINIFile)
	=readini(gcINIFile)
ENDIF
IF EMPTY(gcHTTPRoot)
	DO FORM SpecRoot
ENDIF
IF EMPTY(gcScriptRoot)
	gcScriptRoot=gcHTTPRoot
ENDIF
SET PATH TO (gcPath)
CLOSE ALL DATABASES
CLOSE ALL
lcFileName=LOWER(FULLPATH('querylog.dbf',lcProgram))
IF NOT FILE(lcFileName)
	CREATE TABLE (lcFileName) (TimeStamp T, IDCFile C(32), Parameters M)
	USE
ENDIF
USE (lcFileName) ALIAS QueryLog EXCLUSIVE

DO FORM server
CLOSE ALL DATABASES
CLOSE ALL
SELECT (lnSelect)
IF NOT EMPTY(lcScreenCaption)
	_screen.Caption=lcScreenCaption
ENDIF
IF NOT EMPTY(lcScreenIcon)
	_screen.Icon=lcScreenIcon
ENDIF
SET MESSAGE TO
SET PATH TO (lcSetPath)
IF EMPTY(lcError)
	ON ERROR
ELSE
	ON ERROR &lcError
ENDIF

RETURN



FUNCTION executeprocess(tcFileName)
LOCAL lcDataFile,lcAckFile,lnDFH,lnAFH,lcParameter

	lcDataFile=gcSemaphoreRoot+JustStem(ALLTRIM(tcFileName))+".dat"
	lcAckFile=gcSemaphoreRoot+JustStem(ALLTRIM(tcFileName))+".ack"
	lnDFH=FOPEN(lcDataFile)
	lcParameter=""
	IF lnDFH > 0
		DO WHILE NOT FEOF(lnDFH)
			lcParameter=lcParameter+FREAD(lnDFH,1000)
		ENDDO
		=FCLOSE(lnDFH)
		STRTOFILE(GenPage(lcParameter),lcDataFile)
		* Create Acknowledgement file
		STRTOFILE(" ",lcAckFile)
	ELSE
		* Error opening data file
	ENDIF
ENDFUNC


* HTML Page Generation Program
* This program takes a SQL Query, and several other parameters and
* generates an output document in HTML which can be used by a WWW
* Browser.
* This function goes for bulletproof simple error handling when it is interpreting
* an .HTX file.  If it runs into a logical error, it will simply attempt to continue.
****************
FUNCTION genpage(cParameters)

	LOCAL lnAtPos,lcFileName,lcAlias
	LOCAL lFailure, cResultPage, lcError, lnSelect
	LOCAL cSQLStatement, cKeyColumn, cDescriptColumn, ;
		cBackgroundImg, iCount, cTmpString, cPrevNext, ;
		IDCFile, lcTemplate, lcLine, lcTmpLine, ;
		lcLineCopy, lFailure, cExecSQLString, lhTemplate, llDone, ;
		llGetNewLine, lcTmpExp, lcExp1, lcExp2, lcOperator, lcIfStatement, ;
		lcTrueStatement, lcFalseStatement, lcHTMLPath, lcIDCFile, ;
		lcDefErr, llDefaultError, lcReturnData, llReturnData

	*These symbols, we want available in the sub programs.  They will all be available,
	*along with all of the parsed in environment variables, to the functions that execute
	*conditionals and detail lines.  This allows those functions to simply utilize their
	*environment.

	PRIVATE laEnvVariables, lnEnvVariables, IDC_DataSource, IDC_Template, ;
		IDC_SQLStatement, IDC_DefaultParameters, IDC_Expires, IDC_MaxFieldSize, ;
		IDC_Password, IDC_RequiredParameters, IDC_Username, laDefaultParameters, ;
		laRequiredParameters, CurrentRecord, laTables, CommandSuccess, ;
		lnRecordsReturned, IDC_MaxRecords

	CommandSuccess="FALSE"
	lcAlias=''

	*Parse out all of the environment variables and HTML variables that are
	*sent to us via the CGI script (contained in cParameters) and place them
	*in an array for ease of reference.  The variables are placed in an array
	*as VARIABLE_NAME, VALUE pairs.
	lnEnvVariables=0
	DIMENSION laEnvVariables[1,2]

	IF LEFT(cParameters,1)=='&'
		cParameters=ALLTRIM(SUBSTR(cParameters,2))
	ENDIF

	lnEnvVariables=ParseVars(cParameters,@laEnvVariables,.T.)

	*Parse out the contents of QUERY_STRING if it is not empty.
	IF NOT EMPTY(getparam("QUERY_STRING"))
		lnEnvVariables=ParseVars(getparam("QUERY_STRING"),@laEnvVariables)
	ENDIF

	*Find out if the user has turned off default error processing for the
	*executable command.
	lcDefErr=getparam("DefError")
	IF UPPER(ALLT(lcDefErr))=="OFF"
		llDefaultError=.F.
	ELSE
		llDefaultError=.T.
	ENDIF

	*Find out if the user would like the data back as a block of data.
	lcReturnData=getparam("ReturnAsFile")
	IF UPPER(ALLT(lcReturnData))=="ON"
		llReturnData=.T.
	ELSE
		llReturnData=.F.
	ENDIF

	*Build an absolute path representing where the calling HTML page was located.
	lcHTMLPath=BldPath()

	*Get pointer to .IDC file via passed in HTML parameter
	lcIDCFile=getparam("IDCFile")
	IF NOT EMPTY(lcIDCFile)
		m.IDCFile=LOWER(FULLPATH(lcIDCFile))
		IF NOT FILE(m.IDCFile)
			m.IDCFile=LOWER(FULLPATH(gcScriptRoot+lcIDCFile))
			IF NOT FILE(IDCFile)
				m.IDCFile=LOWER(FULLPATH(lcHTMLPath+lcIDCFile))
				IF NOT FILE(m.IDCFile)
					m.IDCFile=LOWER(FULLPATH(gcHTTPRoot+lcIDCFile))
				ENDIF
			ENDIF
		ENDIF
		m.IDCFile=LOWER(FULLPATH(m.IDCFile))
	ENDIF

	*Append query log
	lnSelect=SELECT()
	SELECT QueryLog
	IF RECCOUNT()>=1000
		ZAP
	ENDIF
	INSERT INTO QueryLog (TimeStamp, IDCFile, Parameters) ;
			VALUES (DATETIME(), lcIDCFile, cParameters)
	SELECT 0

	IF EMPTY(m.IDCFile)
		=Cleanup()
		RETURN errorpage(NOIDC_LOC)
	ENDIF
	IF NOT FILE(m.IDCFile)
		=Cleanup()
		RETURN errorpage(IDCNOTFOUND1_LOC+lcIDCFile+IDCNOTFOUND2_LOC)
	ENDIF

	*Verify required IDC information
	IDC_DataSource=parmsub(getidcp(m.IDCFile,"DataSource"))
	IDC_Template=parmsub(getidcp(m.IDCFile,"Template"))
	IF EMPTY(IDC_Template)
		=Cleanup()
		RETURN errorpage(IDCNOTFOUND1_LOC+IDCFile+IDCBADDATA1_LOC)
	ENDIF
	IDC_SQLStatement=parmsub(getidcp(m.IDCFile,"SQLStatement"))
	IF EMPTY(IDC_SQLStatement)
		=Cleanup()
		RETURN errorpage(IDCNOTFOUND1_LOC+IDCFile+IDCBADDATA2_LOC)
	ENDIF
	DIMENSION laDefaultParameters(1,2)
	IDC_DefaultParameters=getidcp(m.IDCFile,"DefaultParameters",@laDefaultParameters)
	IDC_MaxRecords=VAL(parmsub(getidcp(m.IDCFile,"MaxRecords")))
	IDC_UserName=parmsub(getidcp(m.IDCFile,"UserName"))
	IDC_Password=parmsub(getidcp(m.IDCFile,"Password"))
	IDC_Expires=parmsub(getidcp(m.IDCFile,"Expires"))
	IDC_MaxFieldSize=parmsub(getidcp(m.IDCFile,"MaxFieldSize"))
	DIMENSION laRequiredParameters(1,2)
	IDC_RequiredParameters=getidcp(m.IDCFile,"RequiredParameters",@laRequiredParameters)
	lcSQLStatement=IDC_SQLStatement
	lcTemplate=IDC_Template

	*Save server settings
	lcError = ON('ERROR')

	*Initialize result page
	cResultPage = 'Content-Type: text/html'+CRLF+CRLF

	lFailure = .F.

	*Check to see whether we will be accessing an ODBC datasource or native data
	IF EMPTY(IDC_DataSource)
		*NATIVE DATA
		cExecSQLString=lcSQLStatement
		IF EMPTY(cExecSQLString)
			=Cleanup()
			RETURN errorpage(BADSQL_LOC)
		ENDIF

		*Execute SQL String and trap for a failure
		_TALLY=0
		lFailure = .F.
		cSQLStatement=cExecSQLString
		*Convert string to UPPERCASE, TRIM, and remove TABs for easy
		*syntax checking.
		cExecSQLString=UPPER(ALLTRIM(STRTRAN(cExecSQLString,CHR(9),' ')))
		*Special case the general SELECT statement without an INTO (the default
		*for Wizard generated stuff.)
		IF cExecSQLString="SELECT " AND ATC(" INTO ",cExecSQLString)=0
			* cExecSQLString needs to carry through the case sensitivity of the
			* original SQL SELECT
			cExecSQLString = cSQLStatement + " INTO CURSOR TempResult"
		ELSE
			IF cExecSQLString="SELECT " OR;
					cExecSQLString="DELETE " OR;
					cExecSQLString="INSERT " OR;
					cExecSQLString="UPDATE " OR;
					cExecSQLString="ALTER TABLE " OR;
					cExecSQLString="CREATE CURSOR " OR;
					cExecSQLString="CREATE TABLE "
				* cExecSQLString needs to carry through the case sensitivity of the
				* original SQL SELECT
				cExecSQLString = cSQLStatement
			ELSE
				lFailure = .T.
			ENDIF
		ENDIF
		* At this point, if there has been some error evaluating the SQL statement,
		* or if the SQL statement is not one of the above valid types, the lFailure
		* flag is set, and the SQL statement will not be executed.
		IF NOT lFailure
			lnAtPos=ATC(' FROM ',cExecSQLString)
			IF lnAtPos>0
				lcAlias=ALLTRIM(SUBSTR(cExecSQLString,lnAtPos+6))
				lnAtPos=AT(' ',lcAlias)
				IF lnAtPos>0
					lcAlias=ALLTRIM(LEFT(lcAlias,lnAtPos-1))
				ENDIF
				IF LEFT(lcAlias,1)=="'" OR LEFT(lcAlias,1)=='"' OR ;
						LEFT(lcAlias,1)=='['
					lcAlias=EVALUATE(lcAlias)
				ENDIF
				lcAlias=UPPER(lcAlias)
				lcFileName=LOWER(lcAlias)
				lnAtPos=AT('.',lcAlias)
				IF lnAtPos>0
					lcAlias=ALLTRIM(LEFT(lcAlias,lnAtPos-1))
				ENDIF
				IF NOT '.'$lcFileName
					lcFileName=lcFileName+'.dbf'
				ENDIF
				lcFileName=LOWER(lcFileName)
				IF NOT FILE(lcFileName)
					=Cleanup()
					RETURN errorpage(NOSQL1_LOC+lcFileName+NOSQL2_LOC+cSQLStatement)
				ENDIF
			ENDIF
			ON ERROR lFailure = .T.
			&cExecSQLString
			ON ERROR &lcError
			IF USED(lcAlias)
				USE IN (lcAlias)
			ENDIF
		ENDIF


		IF lFailure = .T.
			IF llDefaultError
				=Cleanup()
				RETURN errorpage(BADCMD_LOC+cSQLStatement)
			ELSE
				CommandSuccess="FALSE"
			ENDIF
		ELSE
			CommandSuccess="TRUE"
		ENDIF

		lnRecordsReturned=_TALLY
		IF lnRecordsReturned = 0
			CurrentRecord=0
		ELSE
			CurrentRecord=1
			IF llReturnData
				RETURN makedata()
			ENDIF
		ENDIF

	ELSE
		cExecSQLString=lcSQLStatement
		IF EMPTY(cExecSQLString)
			=Cleanup()
			RETURN errorpage(BADSQL_LOC)
		ENDIF

		lnConn=SQLCONNECT(IDC_DataSource,IDC_Username,IDC_Password)
		IF lnConn <= 0
			=Cleanup()
			RETURN errorpage(BADCONN1_LOC+IDC_DataSource+BADCONN2_LOC+IDC_Username+BADCONN3_LOC)
		ENDIF

		*Execute SQL String and trap for a failure
		cSQLStatement=cExecSQLString

		lnExecRet=0
		DO WHILE lnExecRet=0
			lnExecRet = SQLEXEC(lnConn,cSQLStatement,'TempResult')
		ENDDO

		IF lnExecRet < 0
			lFailure = .T.
		ENDIF

		=SQLDISCONNECT(lnConn)

		IF lFailure = .T.
			IF llDefaultError
				=Cleanup()
				RETURN errorpage(BADCMD_LOC+cSQLStatement)
			ELSE
				CommandSuccess="FALSE"
			ENDIF
		ELSE
			CommandSuccess="TRUE"
		ENDIF

		lnRecordsReturned=RECCOUNT('TempResult')
		IF lnRecordsReturned = 0
			CurrentRecord=0
		ELSE
			CurrentRecord=1
			IF llReturnData
				=Cleanup()
				RETURN makedata()
			ENDIF
		ENDIF

	ENDIF

	*Create HTML return page from .HTX and data

	*Verify the existence of the Template (.HTX) file.  It must be next to the .IDC file,
	*pathed relative to the .IDC file, or in the Script root.
	lcTmpFile=lcTemplate
	lcTemplate=addbs(justpath(m.IDCFile))+lcTemplate
	IF NOT FILE(lcTemplate)
		lcTemplate=gcScriptRoot+lcTmpFile
		IF NOT FILE(lcTemplate)
			=Cleanup()
			RETURN errorpage(NOTEMPLATE_LOC)
		ENDIF
	ENDIF

	lhTemplate=FOPEN(lcTemplate)
	IF lhTemplate < 0
		=Cleanup()
		RETURN errorpage(BADTEMPLATE1_LOC+lcTemplate+BADTEMPLATE2_LOC)
	ENDIF

	llGetNewLine=.T.

	DO WHILE NOT FEOF(lhTemplate)
		IF llGetNewLine
			lcLine=FGETS(lhTemplate)
		ELSE
			*Toggle GetNewLine back to True
			llGetNewLine=.T.
		ENDIF
		lcLineCopy=UPPER(lcLine)
		DO CASE
			*The BeginDetail structure is linear, but must be repeated for
			*each record in the return set.  IFs can be nested within a Detail
			*section, so, once the detail section is loaded, it must be parsed
			*for IFs.  However
		CASE "<%BEGINDETAIL%>" $ lcLineCopy
			*If the BEGINDETAIL is not at the beginning of the line, put the
			*prefix into the Result page.  Then work on the detail chunk.
			IF lcLineCopy != "<%BEGINDETAIL%>"
				cResultPage=cResultPage+SUBSTR(lcLine,1,AT("<%BEGINDETAIL%>",lcLineCopy)-1)
				lcLine=SUBSTR(lcLine,AT("<%BEGINDETAIL%>",lcLineCopy)+15)+CRLF
			ELSE
				*If there's stuff after the BEGINDETAIL symbol, stuff it into the lcLine
				IF LEN(lcLine)>LEN("<%BEGINDETAIL%>")
					lcLine=SUBSTR(lcLine,16)
				ELSE
					lcLine=""
				ENDIF
			ENDIF
			llDone=.F.
			IF NOT EMPTY(lcLine)
				IF "<%ENDDETAIL%>" $ UPPER(lcLine)
					lcTmpLine=lcLine
					IF UPPER(lcTmpLine)!="<%ENDDETAIL%>"
						lcLine=SUBSTR(lcTmpLine,1,AT("<%ENDDETAIL%>",UPPER(lcTmpLine))-1)
					ENDIF
					IF LEN(lcTmpLine)>LEN("<%ENDDETAIL%>")
						lcSuffix=SUBSTR(lcTmpLine,AT("<%ENDDETAIL%>",UPPER(lcTmpLine))+13)
					ELSE
						lcSuffix=""
					ENDIF
					llDone=.T.
				ENDIF
			ENDIF
			DO WHILE NOT llDone AND NOT FEOF(lhTemplate)
				lcTmpLine=FGETS(lhTemplate)
				IF NOT("<%ENDDETAIL%>" $ UPPER(lcTmpLine))
					*Add to the block until you hit an ENDDETAIL
					lcLine=lcLine+lcTmpLine+CRLF
				ELSE
					*Add everything up to the beginning of the ENDDETAIL and store everything
					*afterward in lcSuffix for processing later.
					IF UPPER(lcTmpLine)!="<%ENDDETAIL%>"
						lcLine=lcLine+SUBSTR(lcTmpLine,1,AT("<%ENDDETAIL%>",UPPER(lcTmpLine))-1)
					ENDIF
					IF LEN(lcTmpLine)>LEN("<%ENDDETAIL%>")
						lcSuffix=SUBSTR(lcTmpLine,AT("<%ENDDETAIL%>",UPPER(lcTmpLine))+13)
					ELSE
						lcSuffix=""
					ENDIF
					llDone=.T.
				ENDIF
			ENDDO
			IF NOT llDone
				*Error: Mismatched Begin/EndDetail, close template file and exit
				=FCLOSE(lhTemplate)
				=Cleanup()
				RETURN errorpage(BADTEMPLATE1_LOC+lcTemplate+BADTEMPLATE3_LOC)
			ENDIF
			IF lnRecordsReturned > 0
				lcDetailExec=''
				DO WHILE AT("<%",lcLine) > 0
					lcDetailExec=lcDetailExec+'"'+STRTRAN(SUBSTR(lcLine,1,AT("<%",lcLine)-1),'"','"+["]+"')+'"+'
					lcLine=SUBSTR(lcLine,AT("<%",lcLine))
					IF UPPER(lcLine)="<%IF "
						*Process the IF structure into an IIF
						*Trim the IF and ENDIF symbols.
						lcIfStatement=SUBSTR(lcLine,6,AT("<%ENDIF%>",UPPER(lcLine))-6)
						*Strip out Expression 1, Expression 2, and the Operator
						lcExp1=ALLT(SUBSTR(lcIfStatement,1,AT(" ",lcIfStatement)-1))
						lcIfStatement=ALLT(SUBSTR(lcIfStatement,AT(" ",lcIfStatement)))
						lcOperator=ALLT(SUBSTR(lcIfStatement,1,AT(" ",lcIfStatement)-1))
						lcIfStatement=ALLT(SUBSTR(lcIfStatement,AT(" ",lcIfStatement)))
						lcExp2=ALLT(SUBSTR(lcIfStatement,1,AT("%>",lcIfStatement)-1))
						lcIfStatement=SUBSTR(lcIfStatement,AT("%>",lcIfStatement)+2)
						IF NOT('"'$lcExp1 OR "'"$lcExp1 OR '['$lcExp1)
							lcExp1=UPPER(lcExp1)
							lcExp1=STRTRAN(lcExp1,"IDC.","IDC_")
						ENDIF
						IF NOT('"'$lcExp2 OR "'"$lcExp2 OR '['$lcExp2)
							lcExp2=UPPER(lcExp2)
							lcExp2=STRTRAN(lcExp2,"IDC.","IDC_")
						ENDIF
						lcOperator=UPPER(lcOperator)
						DO CASE
						CASE lcOperator="CONTAINS"
							lcTmpExp=lcExp2
							lcExp2=lcExp1
							lcExp1=lcTmpExp
							lcOperator="$"
						CASE lcOperator="EQ"
							lcOperator="="
						CASE lcOperator="GT"
							lcOperator=">"
						CASE lcOperator="LT"
							lcOperator="<"
						ENDCASE
						IF "<%ELSE%>"$UPPER(lcIfStatement)
							lcTrueStatement=SUBSTR(lcIfStatement,1,AT("<%ELSE%>",UPPER(lcIfStatement))-1)
							lcIfStatement=SUBSTR(lcIfStatement,AT("<%ELSE%>",UPPER(lcIfStatement))+8)
							lcFalseStatement=lcIfStatement
							lcFalseStatement=STRTRAN(lcFalseStatement,'"','"+["]+"')
						ELSE
							lcTrueStatement=lcIfStatement
							lcFalseStatement=""
						ENDIF
						lcTrueStatement=STRTRAN(lcTrueStatement,'"','"+["]+"')
						lcIIF='IIF('+lcExp1+lcOperator+lcExp2+',"'+lcTrueStatement+'","'+lcFalseStatement+'")+'
						lcDetailExec=lcDetailExec+lcIIF
						IF LEN(lcLine)>AT("<%ENDIF%>",UPPER(lcLine))+9
							lcLine=SUBSTR(lcLine,AT("<%ENDIF%>",UPPER(lcLine))+9)
						ELSE
							lcLine=""
						ENDIF
					ELSE
						*This is a symbol structure.  If it is valid, extract the symbol and place it into
						*the executable line.
						IF AT("%>",lcLine)=0
							=FCLOSE(lhTemplate)
							=Cleanup()
							RETURN errorpage(BADTEMPLATE4_LOC+lcTemplate+BADTEMPLATE5_LOC)
						ENDIF
						lcSymbol=SUBSTR(lcLine,3,AT("%>",lcLine)-3)
						lcDetailExec=lcDetailExec+'EXPTOC('+ALLT(lcSymbol)+')+'
						IF LEN(lcLine)>LEN(lcSymbol)+4
							lcLine=SUBSTR(lcLine,AT("%>",lcLine)+2)
						ELSE
							lcLine=""
						ENDIF
					ENDIF
				ENDDO
				lcDetailExec=lcDetailExec+'"'+STRTRAN(lcLine,'"','"+["]+"')+'"'
				=DetailEx(lcDetailExec,@laEnvVariables,@cResultPage)
			ENDIF
			*If the suffix has stuff to process, then don't get a new file line, start with the suffix.
			IF NOT EMPTY(lcSuffix)
				llGetNewLine=.F.
				lcLine=lcSuffix
			ENDIF
		CASE "<%IF" $ lcLineCopy
			IF lcLineCopy != "<%IF "
				cResultPage=cResultPage+SUBSTR(lcLine,1,AT("<%IF ",lcLineCopy)-1)
				lcLine=SUBSTR(lcLine,AT("<%IF ",lcLineCopy)+5)+CRLF
			ELSE
				IF LEN(lcLine)>LEN("<%IF ")
					lcLine=SUBSTR(lcLine,5)
				ELSE
					=FCLOSE(lhTemplate)
					=Cleanup()
					RETURN errorpage(BADTEMPLATE6_LOC)
				ENDIF
			ENDIF
			llDone=.F.
			DO WHILE NOT llDone AND NOT FEOF(lhTemplate)
				lcTmpLine=FGETS(lhTemplate)
				IF NOT("<%ENDIF%>" $ UPPER(lcTmpLine))
					lcLine=lcLine+lcTmpLine+CRLF
				ELSE
					*Add everything up to the beginning of the ENDIF and store everything
					*afterward in lcSuffix for processing later.
					IF UPPER(lcTmpLine)!="<%ENDIF%>"
						lcLine=lcLine+SUBSTR(lcTmpLine,1,AT("<%ENDIF%>",UPPER(lcTmpLine))-1)
					ENDIF
					IF LEN(lcTmpLine)>LEN("<%ENDIF%>")
						lcSuffix=SUBSTR(lcTmpLine,AT("<%ENDIF%>",UPPER(lcTmpLine))+9)
					ELSE
						lcSuffix=""
					ENDIF
					llDone=.T.
				ENDIF
			ENDDO
			IF NOT llDone
				*Error: Mismatched If/Endif, close template file and exit
				=FCLOSE(lhTemplate)
				=Cleanup()
				RETURN errorpage(BADTEMPLATE1_LOC+lcTemplate+BADTEMPLATE7_LOC)
			ENDIF
			*At this point, the entire structure between the <%IF and the <%ENDIF%> non inclusive is
			*in lcLine.  This will include the conditional parameters and an <%ELSE%> if such a thing exists.
			*In addition, everything after the structure will be contained in lcSuffix.
			lcIfStatement=LTRIM(lcLine)
			*Strip out Expression 1, Expression 2, and the Operator
			lcExp1=ALLT(SUBSTR(lcIfStatement,1,AT(" ",lcIfStatement)-1))
			lcIfStatement=ALLT(SUBSTR(lcIfStatement,AT(" ",lcIfStatement)))
			lcOperator=ALLT(SUBSTR(lcIfStatement,1,AT(" ",lcIfStatement)-1))
			lcIfStatement=ALLT(SUBSTR(lcIfStatement,AT(" ",lcIfStatement)))
			lcExp2=ALLT(SUBSTR(lcIfStatement,1,AT("%>",lcIfStatement)-1))
			lcIfStatement=SUBSTR(lcIfStatement,AT("%>",lcIfStatement)+2)
			*If the expressions aren't character literals, make them uppercase
			*for case insensitivity.  Also check to see if they reference IDC.
			*variables at this point.
			IF NOT('"'$lcExp1 OR "'"$lcExp1 OR '['$lcExp1)
				lcExp1=UPPER(lcExp1)
				lcExp1=STRTRAN(lcExp1,"IDC.","IDC_")
			ENDIF
			IF NOT('"'$lcExp2 OR "'"$lcExp2 OR '['$lcExp2)
				lcExp2=UPPER(lcExp2)
				lcExp2=STRTRAN(lcExp2,"IDC.","IDC_")
			ENDIF
			lcOperator=UPPER(lcOperator)
			DO CASE
			CASE lcOperator="CONTAINS"
				lcTmpExp=lcExp2
				lcExp2=lcExp1
				lcExp1=lcTmpExp
				lcOperator="$"
			CASE lcOperator="EQ"
				lcOperator="="
			CASE lcOperator="GT"
				lcOperator=">"
			CASE lcOperator="LT"
				lcOperator="<"
			ENDCASE
			IF "<%ELSE%>"$UPPER(lcIfStatement)
				lcTrueStatement=SUBSTR(lcIfStatement,1,AT("<%ELSE%>",UPPER(lcIfStatement))-1)
				lcIfStatement=SUBSTR(lcIfStatement,AT("<%ELSE%>",UPPER(lcIfStatement))+8)
				lcFalseStatement=lcIfStatement
			ELSE
				lcTrueStatement=lcIfStatement
				lcFalseStatement=""
			ENDIF
			IF EvalCond(lcExp1+lcOperator+lcExp2,@laEnvVariables)
				lcLine=lcTrueStatement+lcSuffix
			ELSE
				lcLine=lcFalseStatement+lcSuffix
			ENDIF
			IF NOT EMPTY(lcLine)
				llGetNewLine=.F.
			ENDIF
		OTHERWISE
			cResultPage=cResultPage+lcLine+CRLF
		ENDCASE
	ENDDO
	IF lnRecordsReturned=0
		cResultPage=cResultPage+NOMATCHES_LOC+CRLF
	ENDIF

	*Append query log
	lnSelect=SELECT()
	SELECT QueryLog
	INSERT INTO QueryLog (TimeStamp, IDCFile, Parameters) ;
			VALUES (DATETIME(), lcIDCFile, RETURNED_LOC)
	SELECT 0

	=FCLOSE(lhTemplate)

	=Cleanup()
	RETURN cResultPage

PROCEDURE Cleanup()
	LOCAL lnCount

	IF USED('TempResult')
		USE IN TempResult
	ENDIF

PROCEDURE errorpage (lcErrorMessage)
	LOCAL cResultPage
	cResultPage = 'Content-Type: text/html'+CRLF+CRLF

	cResultPage = cResultPage + ;
		'<html><head><title>'+FAILEDSEARCH_LOC+'</title></head>'+ ;
		CRLF
	cResultPage = cResultPage + ;
		'<body>'+CRLF
	cResultPage = cResultPage + ;
		'<h1>'+FAILEDSEARCH_LOC+'</h1>'+CRLF

	cResultPage = cResultPage + ;
		' '+ALLT(lcErrorMessage)+'<hr></body></html>'+CRLF

	RETURN cResultPage

FUNCTION FormFld(FldValue)
	FldValue=STRTRAN(FldValue,CRLF,"<BR>")
	IF RIGHT(FldValue,4)="<BR>"
		FldValue=SUBSTR(FldValue,1,LEN(FldValue)-4)
	ENDIF
	RETURN FldValue

FUNCTION MakeData
	LOCAL cResultPage,cFileName,nFH,nFSize
	cResultPage="<Content-type:text/plain>"+CRLF
	cFileName=SYS(3)+'.TXT'
	SELECT TempResult
	IF IDC_MaxRecords > 0
		LIST NEXT IDC_MaxRecords TO FILE (cFileName) NOCONSOLE
	ELSE
		LIST TO FILE (cFileName) NOCONSOLE
	ENDIF
	nFH=FOPEN(cFileName)

	IF nFH > 0
		nFSize=FSEEK(nFH,0,2)
		=FSEEK(nFH,0,0)
		cResultPage=cResultPage+FREAD(nFH,nFSize)
	ELSE
		RETURN ErrorPage(NODATASTREAM_LOC)
	ENDIF
	=FCLOSE(nFH)
	ERASE (cFileName)
	RETURN cResultPage



FUNCTION bldpath
	LOCAL lcPath, lcRefer, lcSubPath

	lcPath=gcHTTPRoot

	lcRefer=getparam("HTTP_REFERER")
	IF NOT EMPTY(lcRefer)
		lcRefer=STRTRAN(lcRefer,"//","")
		lcRefer=STRTRAN(lcRefer,"/","\")
		lcRefer=SUBSTR(lcRefer,AT("\",lcRefer)+1)
		lcRefer=LEFT(lcRefer,RAT("\",lcRefer))
	ENDIF

	lcPath=lcPath+lcRefer

	RETURN lcPath



FUNCTION DetailEx(lcDetailLine,laSymbolTable,cResultPage)
	EXTERNAL ARRAY laSymbolTable
	LOCAL lnMaxRecords,lnCount

	IF IDC_MaxRecords > 0
		lnMaxRecords=MIN(IDC_MaxRecords,lnRecordsReturned)
	ELSE
		lnMaxRecords=lnRecordsReturned
	ENDIF

	*Execute IIF with local environment
	lcDetailLine=STRTRAN(lcDetailLine,CRLF,"")

	SELECT TempResult

	lnCount=0
	SCAN REST
		IF lnCount>=lnMaxRecords
			EXIT
		ENDIF
		lnCount=lnCount+1
		cResultPage=cResultPage+EVALUATE(lcDetailLine)+CRLF
		CurrentRecord=CurrentRecord+1
	ENDSCAN

	IF CurrentRecord > 0
		CurrentRecord=1
	ENDIF

	RETURN



	*This function takes a conditional statement and evaluates it based on the
	*entire environment state at this time.

FUNCTION EvalCond(lcConditional,laSymbolTable)
	EXTERNAL ARRAY laSymbolTable

	RETURN EVALUATE(lcConditional)



FUNCTION exptoc(eExprVal)

	DO CASE
	CASE ISNULL(eExprVal)
		RETURN 'NULL'
	CASE TYPE('eExprVal') $ 'CM'
		RETURN ALLTRIM(eExprVal)
	CASE TYPE('eExprVal') $ 'NFYB'
		RETURN ALLTRIM(STR(eExprVal))
	CASE TYPE('eExprVal') $ 'L'
		RETURN ALLTRIM(IIF(eExprVal,TRUE_LOC,FALSE_LOC))
	CASE TYPE('eExprVal') $ 'D'
		RETURN ALLTRIM(DTOC(eExprVal))
	CASE TYPE('eExprVal') $ 'T'
		RETURN ALLTRIM(TTOC(eExprVal))
	OTHERWISE
		RETURN '*****'
	ENDCASE



	* Function GETIDCP (Get IDC Parameters)
	* Fetches requested parameter from the IDC file and returns the
	* value as a character string.  This function has a polymorphic
	* return type: in the case where the DefaultParameters parameter is
	* fetched, the function returns a numeric value indicating how many
	* default parameters were fetched.  The parameters and values are stored
	* in the passed-by-reference array as a parameter,value pair.
	* Errors result in the null string being returned.

FUNCTION getidcp(cIDCFile,cParameter,aDefArray)

	IF PARAMETERS()<2
		RETURN ""
	ENDIF

	llFound=.F.

	cParameter=ALLT(cParameter)
	cIDCFile=ALLT(cIDCFile)

	lhIDC=FOPEN(cIDCFile)

	IF lhIDC < 0
		RETURN ""
	ENDIF

	DO WHILE NOT llFound AND NOT FEOF(lhIDC)
		lcLine=FGETS(lhIDC)
		IF NOT EMPTY(lcLine)
			lcLineTok=UPPER(STRTRAN(lcLine," ",""))
			IF (UPPER(cParameter)+":")$lcLineTok
				llFound=.T.
			ENDIF
		ENDIF
	ENDDO

	IF NOT llFound
		=FCLOSE(lhIDC)
		RETURN ""
	ENDIF

	llDone=.F.
	DO WHILE NOT llDone AND NOT FEOF(lhIDC)
		lcAddLine=ALLT(FGETS(lhIDC))
		IF LEFT(lcAddLine,1) = "+"
			lcLine=lcLine+" "+ALLT(SUBSTR(lcAddLine,2))
		ELSE
			llDone=.T.
		ENDIF
	ENDDO

	=FCLOSE(lhIDC)

	*Add filler so AT will never fail.
	lcLine=lcLine+" "

	lcValue=LTRIM(SUBSTR(lcLine,AT(":",lcLine)+1))

	IF UPPER(cParameter)="DEFAULTPARAMETERS" OR UPPER(cParameter)="REQUIREDPARAMETERS"
		lnParameters=0
		DO WHILE AT("=",lcValue)>0
			* Add filler so loop works in 0 case
			lcValue=lcValue+", "
			lnParameters=lnParameters+1
			DIMENSION aDefArray(lnParameters,2)
			aDefArray(lnParameters,1)=ALLT(SUBSTR(lcValue,1,AT("=",lcValue)-1))
			aDefArray(lnParameters,2)=ALLT(SUBSTR(lcValue,AT("=",lcValue)+1,AT(",",lcValue)-(AT("=",lcValue)+1)))
			lcValue=SUBSTR(lcValue,AT(",",lcValue)+1)
		ENDDO
		RETURN lnParameters
	ENDIF

	RETURN lcValue



FUNCTION getparam
	LPARAMETER cVar
	EXTERNAL ARRAY laEnvVariables

	LOCAL nLocation, llDone, nFrom

	llDone=.F.
	nFrom=1

	*Find the given variable in the first column of the environment variables array if
	*it's there at all.
	DO WHILE llDone=.F.
		nLocation=ASCAN(laEnvVariables,cVar,nFrom)
		IF nLocation=0
			llDone=.T.
		ELSE
			IF ASUBSCRIPT(laEnvVariables,nLocation,2) = 1
				llDone=.T.
			ELSE
				nFrom=nLocation+1
			ENDIF
		ENDIF
	ENDDO

	IF nLocation > 0
		RETURN laEnvVariables[ASUBSCRIPT(laEnvVariables,nLocation+1,1),ASUBSCRIPT(laEnvVariables,nLocation+1,2)]
	ELSE
		RETURN ""
	ENDIF



FUNCTION GetToken(INSTR,innum,insep)
	**  GetToken.
	**	Parameters:
	**	1) Input string to be parsed.
	**  2) Number of token to return.
	**	3) The delimiter that seperates tokens.  (Default ",")
	PRIVATE ALL

	IF PARAM() < 2
		RETURN ""
	ENDIF

	IF PARAM() < 3
		insep = ","
	ENDIF

	maxnum = OCCURS(insep,INSTR)

	DO CASE
	CASE innum <= 0
		rval = ""
	CASE maxnum = 0 AND innum = 1
		rval = INSTR
	CASE maxnum = 0 AND innum > 1
		rval = ""
	CASE maxnum > 0 AND innum = 1
		rval = SUBSTR(INSTR,1,ATC(insep,INSTR)-1)
	CASE innum = maxnum + 1 AND LEN(INSTR) < ATC(insep,INSTR,maxnum)+1
		rval = ""
	CASE innum = maxnum + 1
		start = ATC(insep,INSTR,maxnum)+1
		stop = LEN(INSTR)
		rval = SUBSTR(INSTR,start,stop-start+1)
	CASE innum < maxnum + 1
		start = ATC(insep,INSTR,innum-1)+1
		stop = ATC(insep,INSTR,innum)-1
		rval = SUBSTR(INSTR,start,stop-start+1)
	CASE innum > maxnum + 1
		rval = ""

	ENDCASE
	RETURN rval



FUNCTION isnum(lcNumber)

	IF EMPTY(lcNumber)
		RETURN .F.
	ENDIF

	lcNumber=UPPER(lcNumber)

	*Make sure there's not more than one of each E,-, and .

	IF STRTRAN(lcNumber,".","",1,1)!=STRTRAN(lcNumber,".","")
		RETURN .F.
	ENDIF
	IF STRTRAN(lcNumber,"E","",1,1)!=STRTRAN(lcNumber,"E","")
		RETURN .F.
	ENDIF
	IF STRTRAN(lcNumber,"-","",1,1)!=STRTRAN(lcNumber,"-","")
		RETURN .F.
	ENDIF

	IF CHRTRAN(lcNumber,"0123456789.E-",REPL(CHR(0),13))==REPL(CHR(0),LEN(lcNumber))
		RETURN .T.
	ELSE
		RETURN .F.
	ENDIF



FUNCTION parmsub(lcRootString)
	EXTERNAL ARRAY laEnvVariables

	lcFilledString=lcRootString

	*Iterate through for each possible environment variable.  Faster than parsing everything out.

	FOR nCnt=1 TO lnEnvVariables
		lcFilledString=STRTRAN(lcFilledString,"%"+ALLT(laEnvVariables[nCnt,1])+"%",laEnvVariables[nCnt,2])
	ENDFOR

	RETURN lcFilledString



FUNCTION ParseVars(cParameters,laEnvVariables,llCreate)

	LOCAL nVCount, cToken

	IF EMPTY(cParameters)
		RETURN 0
	ENDIF

	IF llCreate
		nVCount=1
	ELSE
		nVCount=ALEN(laEnvVariables,1)+1
	ENDIF
	cToken=GETTOKEN(cParameters,1,"&")
	DO WHILE NOT EMPTY(cToken)
		DIMENSION laEnvVariables[nVCount,2]
		laEnvVariables[nVCount,1]=ALLT(SUBSTR(cToken,1,ATC("=",cToken)-1))
		IF ATC("=",cToken)=LEN(cToken)
			laEnvVariables[nVCount,2]=""
		ELSE
			cToken = SUBSTR(cToken,ATC("=",cToken)+1)
			cToken = StripASCII(cToken)
			laEnvVariables[nVCount,2] = STRTRAN(cToken,"+"," ")
		ENDIF
		nVCount=nVCount+1
		cToken=GETTOKEN(cParameters,nVCount,"&")
	ENDDO

	nVCount=nVCount-1

	RETURN nVCount

FUNCTION StripASCII
	LPARAMETER clString
	* clString is ASCII, but could contain hex of DBCS
	LOCAL m.cstr, m.nextflag, m.lastflag, m.numflags
	
	m.nextflag = AT_C("%",m.clString)
	IF m.nextflag = 0
		*  If no % symbol, do not need to modify string.
		RETURN m.clString
	ENDIF
	
	m.cStr = ""
	m.lastflag = 1
	m.numflags = 1
	DO WHILE .T.
		*  Get text before current % flag.
		m.cStr = m.cStr + ;
				SUBSTR(m.clString,m.lastflag, m.nextflag - m.lastflag) + ;
				CHR(EVALUATE("0x" + SUBSTR(m.clString,m.nextflag + 1,2)))
		m.lastflag = m.nextflag + 3	&& skip past item.
		m.numflags = m.numflags + 1
		m.nextflag = AT_C("%", m.clString, m.numflags)
		IF m.nextflag = 0
			m.cStr = m.cStr + SUBSTR(m.clString, m.lastflag)
			EXIT
		ENDIF
	ENDDO
	RETURN m.cStr


FUNCTION readini(filename)

	PRIVATE pcfg,prdline,ptoken,pvalue

	pcfg = FOPEN(filename)

	IF pcfg < 0
		RETURN .F.
	ENDIF

	DO WHILE NOT(FEOF(pcfg))
		prdline = FGETS(pcfg)
		IF NOT EMPTY(prdline)
			ptoken = UPPER(ALLTRIM(SUBSTR(prdline,1,AT("=",prdline)-1)))
			pvalue = UPPER(ALLTRIM(SUBSTR(prdline,AT("=",prdline)+1)))
			DO CASE
			CASE ptoken == "HTTPROOT"
				gcHTTPRoot = pvalue
			CASE ptoken == "SCRIPTROOT"
				gcScriptRoot = pvalue
			CASE ptoken == "SEMAPHORE"
				gcSemaphoreRoot = pvalue
			CASE ptoken == "PATH"
				gcPath = pvalue
			ENDCASE
		ENDIF
	ENDDO

	=FCLOSE(pcfg)

	RETURN .T.



FUNCTION runspec
	gcINIFile='is.ini'
	gcHTTPRoot=""
	gcScriptRoot=""

	DO FORM specroot
	RETURN



FUNCTION ErrorHandler(tnErrorNo,tcMsg,tcProgramName,tnLineNo,tcCodeLine)
LOCAL laDir[1]

	IF ADIR(laDir,'error.txt')>0
		IF laDir[2]>65535 OR (DATE()-laDir[3])>14
			ERASE error.txt
		ENDIF
	ENDIF
	SET TEXTMERGE OFF
	SET TEXTMERGE TO ERROR ADDITIVE
	SET TEXTMERGE ON NOSHOW
	
	\<<ERROCCUR_LOC>><<DATETIME()>>
	\<<ERRMESS_LOC>><<tcMsg>>
	\<<ERRNUM_LOC>><<tnErrorNo>>
	\<<ERRPROC_LOC>><<PROPER(tcProgramName)>>
	\<<ERRLINE_LOC>><<tnLineNo>>
	IF NOT EMPTY(ALIAS())
	\<<ERRALIAS_LOC>>[<<ALIAS()>>]
	\<<ERRREC_LOC>><<RECNO()>>
	ENDIF
	\<<tcCodeLine>>
	\
	\
	SET TEXTMERGE OFF
	SET TEXTMERGE TO
ENDFUNC