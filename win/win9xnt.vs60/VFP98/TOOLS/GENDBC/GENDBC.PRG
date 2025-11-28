** For best results, view this file with: Courier New / Regular / 10
** TabWidth set to: 4
**************************************************************************
** Program Name : GENDBC.PRG
** Creation Date: 94.12.01
**
** Purpose		:	To take an existing FoxPro 3.0/5.0 database and
**					generate an output program that can be used
**					to "re-create" that database.
** Parameters	:	cOutFile - A character string that contains
**					the name of an output file.  This
**					can contain path information and
**					an extension.  If no extension is
**					supplied, one will be provided.
**
**					lskipdisplay - Whether to include status
**					messages in generated output.
** Modification History:
**
** 1994.12.01 KRT  Created program, runs on Build 329 of VFP
** 1994.12.02 KRT  Added GetView function and cleaned up all code
** 1994.12.05 KRT  Modified some areas and verified run on Build 335
** 1994.12.06 KRT  Added things for international conversion
** 1994.12.08 KRT  Added function ADBOBJECTS() to code
** 1994.12.12 KRT  Added commands COPY PROCEDURES TO
** 1995.01.04 KRT  Added connection properties
** 1995.01.05 KRT  Added support for long filenames - thierryp
** 1995.01.05 KRT  Added support for RI
** 1995.01.06 KRT  Added support for MS-DOS short filenames (NAME clause)
** 1995.01.08 KRT  Added status bar line
** 1995.01.26 KRT  Fixed a few file bugs and localization bug
** 1995.02.19 KRT  Took advantage of AFIELDS() command
** 1995.02.22 KRT  Fixed AUSED() bug
** 1995.03.01 KRT  Removed ON ERROR problem
** 1995.03.20 KRT  Fixed "Exclusive" Error / Procedures created before
**                 Tables
** 1995.03.22 KRT  Allowed user to open database if one is not set current
**                 / Set SAFETY OFF in resulting code to prevent errors
**                   when validating rules/triggers that don't exist
** 1995.04.07 KRT  Put any database procedures into a seperate file to
**                 prevent "Program Too Large" errors
** 1995.04.20 KRT  Only change SAFETY when appending procedures
**
** 3.0b - Changes/Fixes
** 1995.09.15 KRT  Changed ADBOBJECTS() to DBF() in GETTABLE procedure.
**                 This allows for "real" table names instead of alias'
** 1995.09.15 KRT  Take into account CR+LF in comments for fields
** 1995.09.15 KRT  Store Source and Object code into external file
**                 So the code can be executed from the run-time version
** 1995.10.23 KRT  Changed DBF(cTableName ) to DBF(ALIAS() ) because
**                 VFP will automatically add underscores where spaces
**                 should be and I need to know what it did to the alias
**                 [Regress]
** 1995.10.25 KRT  Added OVERWRITE to append memo's [Regress]
** 1995.10.25 KRT  Added support for CR+LF in comments for Table
** 1995.10.25 KRT  Close all tables in generated code before adding
**                 Source and Object code from external file[Regress]
** 1995.10.25 KRT  Added warning about filter indices on Unique and
**                 Candidate keys (Not supported via language )
** 1995.12.20 KRT  Better lookup for adding RI information to database
** 1995.12.20 KRT  Added support for filter expressions on CANDIDATE keys
** 
** 5.0 - Changes/Fixes
** 1996.03.27 KRT  Added "exact match" support for Locate command
** 1996.04.12 KRT  Added new properties for Views, Fields and Connections
** 1996.05.14 KRT  Adjusted for some logical properties return spaces
** 1996.05.16 KRT  Added even more new properties for Views
** 1996.05.16 KRT  Add M. in front of all memory variables to prevent
**                 confusion with table fields and names
** 1996.06.01 KRT  Added support for Collate sequence on index files
** 1996.06.26 KRT  Added support for ParameterList in Views
** 1996.07.19 KRT  Added support for Offline Views
** 1996.08.07 KRT  Added support for comments and default values in views
** 1997.10.22 RB   Breakup data creation into procedures to eliminate 64K proc limit
**************************************************************************
LPARAMETERS cOutFile,lskipdisplay

PRIVATE ALL EXCEPT g_*
*! Public Variables
IF SET("TALK") = "ON"						&& To restore SET TALK after use
	SET TALK OFF							&& -- Have to do it this way so
	m.g_cSetTalk = "ON"						&& -- nothing get's on screen
ELSE
	m.g_cSetTalk = "OFF"
ENDIF
m.g_lskipdisplay = IIF(vartype(lskipdisplay)#"L",.F.,lskipdisplay)
m.g_cFullPath = SET("FULLPATH")				&& To restore old FULLPATH setting
m.g_cOnError = ON("ERROR")					&& To restore old ON ERROR condition
m.g_cSetDeleted = SET("DELETED")			&& To restore SET DELETED later
m.g_cSetStatusBar = SET("STATUS BAR")		&& To restore STATUS bar
m.g_cStatusText = SYS(2001, "MESSAGE", 1)	&& To restore text that may be on it
m.g_nMax = 7								&& For status line information
m.g_nCurrentStat = 1							&& For status line information
m.g_cFilterExp = ""                         && For Non-Supported Filter Info
DIMENSION g_aProcs[1]
SET DELETED ON
SET FULLPATH ON
IF m.g_cSetStatusBar = "OFF"
	SET STATUS BAR ON
ENDIF

*! Our generic error handling routine
ON ERROR DO GenDBC_Error WITH MESSAGE(), LINENO()

**************************************************************************
** Constants
**************************************************************************
#DEFINE CRLF CHR(13) + CHR(10)
#DEFINE DBCS_LOC 			"81 82 86 88"

**************************************************************************
** Error Messages
**************************************************************************
#DEFINE NO_DATABASE_IN_USE_LOC	"No Database is in use. " + ;
								"This program must have a database " + ;
								"available."
#DEFINE INVALID_PARAMETERS_LOC	"Invalid Parameters..." + CRLF + ;
								"An output file must be specified." + CRLF +;
								'ie: DO GENDBC WITH "filename.prg"'
#DEFINE INVALID_DESTINATION_LOC	"Invalid Destination File "
#DEFINE NO_TEMP_FILE_LOC		"Could not create temporary file: "
#DEFINE NO_OUTPUT_WRITTEN_LOC   "Could not create or write to output file"
#DEFINE ERROR_TITLE_LOC			"Aborting GenDBC..."
#DEFINE UNRECOVERABLE_LOC		"Unrecoverable Error: "
#DEFINE AT_LINE_LOC				" At Line: "
#DEFINE NO_FIND_LOC				"Could not set RI Information."
#DEFINE NO_FILE_FOUND_LOC		"Warning! No Procedure File Found!"
#DEFINE GETFILE_GEN_LOC			"Generate..."
#DEFINE NOT_SUPPORTED_LOC		"Filters on PRIMARY keys are not supported at this time. " + ;
                                "A comment will be added to your output file specifying the filters."
#DEFINE NS_COMMENT_LOC			"****** These filters need to be added manually ******"
#DEFINE WARNING_TITLE_LOC		"GenDBC Warning..." 

**************************************************************************
** Comments And Other Information
**************************************************************************
#DEFINE MESSAGE_START_LOC	"Creating database..."
#DEFINE MESSAGE_DONE_LOC	"Finished."
#DEFINE MESSAGE_MAKETABLE_LOC 	"Creating table "
#DEFINE MESSAGE_MAKEVIEW_LOC	"Creating view "
#DEFINE MESSAGE_MAKECONN_LOC	"Creating connection "
#DEFINE MESSAGE_MAKERELATION_LOC	"Creating persistent relations..."
#DEFINE MESSAGE_MAKERI_LOC	"Creating relational integrity rules..."
#DEFINE MESSAGE_END_LOC		"..."
#DEFINE BEGIN_RELATION_LOC		"*************** Begin Relations Setup **************"
#DEFINE BEGIN_TABLE_LOC			"Table setup for "
#DEFINE BEGIN_INDEX_LOC			"Create each index for "
#DEFINE BEGIN_PROP_LOC			"Change properties for "
#DEFINE BEGIN_VIEW_LOC			"View setup for "
#DEFINE BEGIN_PROC_LOC			"Procedure Re-Creation"
#DEFINE BEGIN_CONNECTIONS_LOC	"Connection Definitions"
#DEFINE BEGIN_RI_LOC			"Referential Integrity Setup"
#DEFINE OPEN_DATABASE_LOC		"Select Database..."
#DEFINE SAVE_PRG_NAME_LOC		"Enter output program name..."
#DEFINE NO_MODIFY_LOC			"*** WARNING *** DO NOT MODIFY THIS FILE IN ANY WAY! *** WARNING ***"
#DEFINE TABLE_NAME_LOC			"*        Table Name: "
#DEFINE PRIMARY_KEY_LOC			"*       Primary Key: "
#DEFINE FILTER_EXP_LOC			"* Filter Expression: "
#DEFINE HEADING_1_LOC	"* *********************************************************" + CRLF + ;
						"* *" + CRLF
#DEFINE HEADING_2_LOC	"* *" + CRLF + ;
						"* *********************************************************" + CRLF + ;
						"* *" + CRLF + ;
						"* * Description:" + CRLF + ;
						"* * This program was automatically generated by GENDBC" + CRLF + ;
						"* * Version 2.26.67" + CRLF + ;
						"* *" + CRLF + ;
						"* *********************************************************" + CRLF

*! Make sure a database is open
IF EMPTY(DBC())
	m.g_cFullDatabase = GETFILE("DBC", OPEN_DATABASE_LOC, GETFILE_GEN_LOC, 0)
	IF EMPTY(m.g_cFullDatabase)
		FatalAlert(NO_DATABASE_IN_USE_LOC, .F.)
	ENDIF
	OPEN DATABASE (m.g_cFullDatabase)
ENDIF

*! Set global variable to the database name and format it
m.g_cDatabase = DBC()
IF RAT("\", m.g_cDatabase) > 0
	m.g_cDatabase = SUBSTR(m.g_cDatabase, RAT("\", m.g_cDatabase) + 1)
ENDIF

*! Get the fullpath of database
m.g_cFullDatabase = DBC()

*! Check for valid parameters
IF PARAMETERS() < 1 OR TYPE("cOutFile")#"C" OR EMPTY(cOutFile)
	m.cOutFile = ""
	m.cOutFile = PUTFILE(SAVE_PRG_NAME_LOC, (SUBSTR(m.g_cDatabase, 1, RAT(".", m.g_cDatabase)) + "PRG"), "PRG")
	IF EMPTY(cOutFile)
		FatalAlert(INVALID_PARAMETERS_LOC, .F.)
	ENDIF
ENDIF

*! Check for proper extensions or add one if none specified
IF RAT(".PRG", m.cOutFile) = 0 AND RAT(".", m.cOutFile) = 0
	m.cOutFile = m.cOutFile + ".PRG"
ENDIF

*! Make sure the output file is valid
m.hFile = FCREATE(m.cOutFile)
IF m.hFile <= 0
	FatalAlert(INVALID_DESTINATION_LOC + m.cOutFile, .F.)
ENDIF

FCLOSE(m.hFile)
ERASE (m.cOutFile)

*! Remember all our tables that are open for this database
m.g_nTotal_Tables_Used = AUSED(g_aAlias_Used)
IF m.g_nTotal_Tables_Used > 0
	DIMENSION m.g_aTables_Used(m.g_nTotal_Tables_Used)

	*! Get Real Names of tables opened
	FOR m.nLoop = 1 TO m.g_nTotal_Tables_Used
		g_aTables_Used(m.nLoop) = DBF(g_aAlias_Used(m.nLoop, 1))
	ENDFOR
ENDIF

*! Get number of tables contained in database
m.nTotal_Tables = ADBOBJECTS(aAll_Tables, "Table")
m.g_nMax = m.g_nMax + m.nTotal_Tables
Stat_Message()

*! Get number of views contained in database
m.nTotal_Views = ADBOBJECTS(aAll_Views, "View")
m.g_nMax = m.g_nMax + m.nTotal_Views
Stat_Message()

*! Get number of connections contained in database
m.nTotal_Connections = ADBOBJECTS(aAll_Connections, "Connection")
m.g_nMax = m.g_nMax + m.nTotal_Connections
Stat_Message()

*! Get number of relations contained in database
m.nTotal_Relations = ADBOBJECTS(aAll_Relations, "Relation")
m.g_nMax = m.g_nMax + m.nTotal_Relations
Stat_Message()
CLOSE DATABASE

SELECT 0
*! Check for this database... If it's there, we must have left it
*! here because of an error last time.
IF FILE("GENDBC.DBF")
	ERASE "GENDBC.DBF"
	ERASE "GENDBC.FPT"
ENDIF

CREATE TABLE GenDBC (Program M)
APPEND BLANK
USE

**************************
*** Get Stored Procedures
**************************
*! Create an output file that will be appended to the database
*! as procedures
m.cFile = UPPER(SUBSTR(m.cOutFile, 1, RAT(".", m.cOutFile))) + "krt"

*! Place Header Information For Source/Object
m.hFile = FCREATE(m.cFile)
IF m.hFile <= 0
	FCLOSE(m.hFile)
	FatalAlert(NO_OUTPUT_WRITTEN_LOC, .T.)
ENDIF

FPUTS(m.hFile, NO_MODIFY_LOC)
FCLOSE(m.hFile)

*! No we are going to copy the object and source code
*! For the stored procedures
COMPILE DATABASE (m.g_cFullDatabase)
USE (m.g_cFullDatabase)
LOCATE FOR Objectname = 'StoredProceduresSource'
IF FOUND()
	COPY MEMO Code TO (m.cFile) ADDITIVE
ENDIF
ADIR(aTemp, m.cFile)
m.nSourceSize = aTemp(1, 2) - LEN(NO_MODIFY_LOC)

LOCATE FOR Objectname = 'StoredProceduresObject'
IF FOUND()
	COPY MEMO Code TO (m.cFile) ADDITIVE
ENDIF
USE

*! Open the database again
OPEN DATABASE (m.g_cFullDatabase) EXCLUSIVE

ADIR(aTemp, m.cFile)

*! Check for actual output file being created sans header
IF aTemp(1, 2) > LEN(NO_MODIFY_LOC) + 2
	**************************
	*** Make the output file
	*** re-create the proced-
	*** ure file via code.
	**************************
	m.hOutFile = FCREATE("GenDBC.$$$")
	IF m.hOutFile <= 0
		= FCLOSE(m.hFile)
		= FatalAlert(NO_OUTPUT_WRITTEN_LOC, .T.)
	ENDIF
	WriteFile(m.hOutFile, "")
	WriteFile(m.hOutFile, "********* " + BEGIN_PROC_LOC + " *********")
	WriteFile(m.hOutFile, "IF !FILE([" + SUBSTR(m.cFile, RAT("\", m.cFile) + 1) + "])")
	WriteFile(m.hOutFile, "    ? [" + NO_FILE_FOUND_LOC + "]")
	WriteFile(m.hOutFile, "ELSE")
	WriteFile(m.hOutFile, "	CLOSE DATABASE")
	WriteFile(m.hOutFile, "	USE '" +  m.g_cDatabase + "'")
	WriteFile(m.hOutFile, "	g_SetSafety = SET('SAFETY')")
	WriteFile(m.hOutFile, "	SET SAFETY OFF")
	WriteFile(m.hOutFile, "	LOCATE FOR Objectname = 'StoredProceduresSource'")
	WriteFile(m.hOutFile, "	IF FOUND()")
    WriteFile(m.hOutFile, "        APPEND MEMO Code FROM [" + SUBSTR(m.cFile, RAT("\", m.cFile) + 1) + "] OVERWRITE")
	WriteFile(m.hOutFile, "	    REPLACE Code WITH SUBSTR(Code, " + ALLTRIM(STR(LEN(NO_MODIFY_LOC) + 3)) + ", " + ALLTRIM(STR(m.nSourceSize - 2)) + ")")
	WriteFile(m.hOutFile, "	ENDIF")
	WriteFile(m.hOutFile, "	LOCATE FOR Objectname = 'StoredProceduresObject'")
	WriteFile(m.hOutFile, "	IF FOUND()")
    WriteFile(m.hOutFile, "        APPEND MEMO Code FROM [" + SUBSTR(m.cFile, RAT("\", m.cFile) + 1) + "] OVERWRITE")
	WriteFile(m.hOutFile, "        REPLACE Code WITH SUBSTR(Code, " + ALLTRIM(STR(LEN(NO_MODIFY_LOC) + m.nSourceSize + 1)) + ")")
	WriteFile(m.hOutFile, "	ENDIF")
	WriteFile(m.hOutFile, "    SET SAFETY &g_SetSafety")
	WriteFile(m.hOutFile, "	USE")
	WriteFile(m.hOutFile, "	OPEN DATABASE [" + m.g_cDatabase + "]")
	WriteFile(m.hOutFile, "ENDIF")
	WriteFile(m.hOutFile, "")
	FCLOSE(m.hOutFile)
	USE GenDBC EXCLUSIVE
	APPEND MEMO Program FROM "GENDBC.$$$"
	ERASE "GENDBC.$$$"
	USE
ELSE
	ERASE (m.cFile)
ENDIF
Stat_Message()

* Write out database creation routines
UpdateProcArray("DisplayStatus(["+MESSAGE_START_LOC+"])")
UpdateProcArray("CLOSE DATA ALL")
UpdateProcArray("CREATE DATABASE '" + m.g_cDatabase + "'")

**************************
*** Get Tables
**************************
IF m.nTotal_Tables > 0
	FOR m.nLoop = 1 TO m.nTotal_Tables
		DO GetTable WITH ALLTRIM(aAll_Tables(m.nLoop)), "GenDBC.tmp"
		Stat_Message()
		USE GenDBC EXCLUSIVE
		APPEND MEMO Program FROM "GenDBC.tmp"
		USE
		ERASE "GenDBC.tmp"
		UpdateProcArray("DisplayStatus(["+MESSAGE_MAKETABLE_LOC+aAll_Tables(m.nLoop)+MESSAGE_END_LOC+"])")
		UpdateProcArray("MakeTable_"+FixName(aAll_Tables(m.nLoop))+"()")
	ENDFOR
ENDIF

**************************
*** Get Connections
**************************
IF m.nTotal_Connections > 0
	FOR m.nLoop = 1 TO m.nTotal_Connections
		DO GetConn WITH aAll_Connections(m.nLoop), "GenDBC.tmp"
		Stat_Message()
		USE GenDBC EXCLUSIVE
		APPEND MEMO Program FROM "GenDBC.tmp"
		USE
		ERASE "GenDBC.tmp"
		UpdateProcArray("DisplayStatus(["+MESSAGE_MAKECONN_LOC+aAll_Connections(m.nLoop)+MESSAGE_END_LOC+"])")
		UpdateProcArray("MakeConn_"+FIXNAME(aAll_Connections(m.nLoop))+"()")
	ENDFOR
ENDIF

**************************
*** Get Views
**************************
IF m.nTotal_Views > 0
	FOR m.nLoop = 1 TO m.nTotal_Views
		DO GetView WITH ALLTRIM(aAll_Views(m.nLoop)), "GenDBC.tmp"
		Stat_Message()
		USE GenDBC EXCLUSIVE
		APPEND MEMO Program FROM "GenDBC.tmp"
		USE
		ERASE "GenDBC.tmp"
		UpdateProcArray("DisplayStatus(["+MESSAGE_MAKEVIEW_LOC+aAll_Views(m.nLoop)+MESSAGE_END_LOC+"])")
		UpdateProcArray("MakeView_"+FIXNAME(aAll_Views(m.nLoop))+"()")
	ENDFOR
ENDIF

**************************
*** Get Relations
**************************
IF m.nTotal_Relations > 0
	USE GenDBC EXCLUSIVE
	REPLACE Program WITH BEGIN_RELATION_LOC + CRLF ADDITIVE
	UpdateProcArray("DisplayStatus(["+MESSAGE_MAKERELATION_LOC+"])")
	FOR m.nLoop = 1 TO m.nTotal_Relations
		REPLACE Program WITH CRLF + "FUNCTION MakeRelation_"+TRANS(m.nLoop)+CRLF+;
							 "ALTER TABLE '" + aAll_Relations(m.nLoop, 1) +;
							 "' ADD FOREIGN KEY TAG " +;
							 aAll_Relations(m.nLoop, 3) +;
							 " REFERENCES " + ;
							 aAll_Relations(m.nLoop, 2) +;
							 " TAG " + aAll_Relations(m.nLoop, 4) + ;
							 CRLF +"ENDFUNC"+CRLF+CRLF ADDITIVE
		UpdateProcArray("MakeRelation_"+TRANS(m.nLoop)+"()")
	Stat_Message()
	ENDFOR
ENDIF

CLOSE DATABASE  && Because we're going to start peeking into the
				&& table structure of the DBC

**************************
*** Get RI Info
**************************
IF m.nTotal_Relations > 0
	DO GetRI WITH "GenDBC.tmp"
	IF FILE("GenDBC.tmp")
		USE GenDBC EXCLUSIVE
		APPEND MEMO Program FROM "GenDBC.tmp"
		USE
		ERASE "GenDBC.tmp"
		UpdateProcArray("DisplayStatus(["+MESSAGE_MAKERI_LOC+"])")
		UpdateProcArray("MakeRI()")
	ENDIF
ENDIF
UpdateProcArray("DisplayStatus(["+MESSAGE_DONE_LOC+"])")
Stat_Message()

*! Make it a permanent file
USE GenDBC EXCLUSIVE
lcprocstr = ""
FOR i = 1 TO ALEN(g_aprocs)
	lcprocstr = lcprocstr + g_aprocs[m.i] + CRLF
ENDFOR
lcMessageStr =  "FUNCTION DisplayStatus(lcMessage)"+CRLF+;
				"WAIT WINDOW NOWAIT lcMessage"+CRLF+;
				"ENDFUNC"			
REPLACE Program WITH HEADING_1_LOC + "* * " + DTOC(DATE()) +;
					 SPACE(19 - LEN(m.g_cDatabase) / 2) + ;
					 m.g_cDatabase + SPACE(19 - LEN(m.g_cDatabase) / 2) +;
					 TIME() + CRLF + HEADING_2_LOC + CRLF + ;
					 IIF(!EMPTY(m.g_cFilterExp), NS_COMMENT_LOC +  m.g_cFilterExp + ;
					 CRLF + REPLICATE("*", 52) + CRLF, "") + ;
					 CRLF + lcprocstr + CRLF + Program +;
					 CRLF + lcMessageStr +CRLF
										 
COPY MEMO Program TO (m.cOutFile)
USE
ERASE "GenDBC.DBF"
ERASE "GenDBC.FPT"

Stat_Message()

*! Exit Program
COMPILE (m.cOutFile)
GenDBC_CleanUp(.T.)
*********************** END OF PROGRAM ***********************

**************************************************************************
**
** Function Name: GETRI(<ExpC>)
** Creation Date: 1994.12.02
** Purpose:
**
**      To take existing FoxPro 3.0/5.0 RI Infomration, and generate an output
**      program that can be used to "re-create" this.
**
** Parameters:
**
**      cOutFileName - A character string containing the name of the
**                     output file
**
** Modification History:
**
**  1995.01.05  KRT		Created function
**  1995.12.20  KRT		Allow better lookup when trying to find
**						the right record to add the RI information
**************************************************************************
PROCEDURE GetRI
	LPARAMETERS m.cOutFileName

	PRIVATE ALL EXCEPT g_*

	*! Create the output file
	m.hGTFile = FCREATE(m.cOutFileName)
	IF m.hGTFile < 1
		FatalAlert(NO_TEMP_FILE_LOC + m.cOutFileName, .T.)
	ENDIF

	*! USE the database
	USE (m.g_cFullDatabase) EXCLUSIVE

	LOCATE FOR ObjectType = "Relation" AND !EMPTY(RiInfo)
	IF FOUND()
		WriteFile(m.hGTFile, "FUNCTION MakeRI")
		WriteFile(m.hGTFile, "***** " + BEGIN_RI_LOC + " *****")
		WriteFile(m.hGTFile, "CLOSE DATABASE")
		WriteFile(m.hGTFile, "USE '" +  m.g_cDatabase + "'")
		DO WHILE FOUND()
			*! Have to get the parent name to verify we are adding
			*! Information to the right record.
			m.nParentID = ParentID
			*! We use select so we won't mess up our LOCATE ... CONTINUE command
			SELECT ObjectName FROM (m.g_cFullDatabase) WHERE ObjectID = nParentID INTO ARRAY aTableName
			m.nStart = 1
			m.cITag = ""
			m.cTable = ""
			m.cRTag = ""
			DO WHILE m.nStart <= LEN(Property)
				nSize = ASC(SUBSTR(Property, m.nStart, 1)) +;
				(ASC(SUBSTR(Property, m.nStart + 1, 1)) * 256) +;
				(ASC(SUBSTR(Property, m.nStart + 2, 1)) * 256^2) + ;
				(ASC(SUBSTR(Property, m.nStart + 3, 1)) * 256^3)

				m.nKey = ASC(SUBSTR(Property, m.nStart + 6, 1))

				DO CASE
					CASE m.nKey = 13
						m.cITag = SUBSTR(Property, m.nStart + 7, m.nSize - 8)
					CASE m.nKey = 18
						m.cTable = SUBSTR(Property, m.nStart + 7, m.nSize - 8)
					CASE m.nKey = 19
						m.cRTag = SUBSTR(Property, m.nStart + 7, m.nSize - 8)
				ENDCASE
				m.nStart = m.nStart + m.nSize
			ENDDO
			WriteFile(m.hGTFile, "LOCATE FOR ObjectType = 'Table' AND ObjectName = '" + ;
						 ALLTRIM(aTableName(1)) + "'")
			WriteFile(m.hGTFile, "IF FOUND()")
			WriteFile(m.hGTFile, "    nObjectID = ObjectID")
			WriteFile(m.hGTFile, "    LOCATE FOR ObjectType = 'Relation' AND '" + m.cITag + ;
						"'$Property AND '" + m.cTable + "'$Property AND '" + m.cRTag + ;
						"'$Property AND ParentID = nObjectID")
			WriteFile(m.hGTFile, "    IF FOUND()")
			WriteFile(m.hGTFile, "	      REPLACE RiInfo WITH '" + RiInfo + "'")
			WriteFile(m.hGTFile, "    ELSE")
			WriteFile(m.hGTFile, '       ? "' + NO_FIND_LOC + '"')
			WriteFile(m.hGTFile, "    ENDIF")
			WriteFile(m.hGTFile, "ENDIF")
			CONTINUE
		ENDDO
		WriteFile(m.hGTFile, "USE")
		WriteFile(m.hGTFile, "ENDFUNC")
		WriteFile(m.hGTFile, "")
		FCLOSE(m.hGTFile)
	ELSE
		FCLOSE(m.hGTFile)
	ERASE (m.cOutFileName)
	ENDIF
	USE
RETURN

**************************************************************************
**
** Function Name: GETTABLE(<ExpC>, <ExpC>)
** Creation Date: 1994.12.01
** Purpose        :
**
**              To take an existing FoxPro 3.0/5.0 Table, and generate an output
**              program that can be used to "re-create" that Table.
**
** Parameters:
**
**      cTableName   -  A character string representing the name of the
**                      existing Table
**      cOutFileName -  A character string containing the name of the
**                      output file
**
** Modification History:
**
**  1994.12.02  KRT             Created function
**  1994.12.05  KRT             Made it a function and cleaned it up
**  1994.12.08  KRT             Assume Database is open to speed up operation
**  1995.09.15  KRT             Use DBF() to find the real table name
**  1995.09.15  KRT             Take into account CR+LF in comment fields
**  1996.04.12  KRT             Added new properties for Visual FoxPro 5.0
**                              InputMask / Format / DisplayClass
**                              DisplayClassLibrary
**  1996.06.01  KRT             Added support for Collate sequence on index
**************************************************************************
PROCEDURE GetTable
	LPARAMETERS m.cTableName, m.cOutFileName

	PRIVATE ALL EXCEPT g_*

	*! Create the output file
	m.hGTFile = FCREATE(m.cOutFileName)
	IF m.hGTFile < 1
		FatalAlert(NO_TEMP_FILE_LOC + m.cOutFileName, .T.)
	ENDIF

	*! Open Table to get field info
	USE (m.cTableName) EXCLUSIVE

	*! Get all the fields
	m.nNumberOfFields = AFIELDS(aAll_Fields)

	*! Header Information
	WriteFile(m.hGTFile, "FUNCTION MakeTable_"+FIXNAME(m.cTableName))
	WriteFile(m.hGTFile, "***** " + BEGIN_TABLE_LOC + m.cTableName + " *****")

	*! NOTE * NOTE * NOTE
	*! If the table is greater than 8 characters then it will fail on platforms that
	*! do not support this (Such as Win32s).
	m.cOldSetFullPath = SET("FULLPATH")
	SET FULLPATH OFF
	m.cTableFileName = DBF(ALIAS())
	SET FULLPATH &cOldSetFullPath
	m.cTableFileName = SUBSTR(m.cTableFileName, RAT(":", m.cTableFileName) + 1)
	m.cCreateTable = "CREATE TABLE '" + m.cTableFileName + "' NAME '" + m.cTableName + "' ("
	
	*! Information about each field that can been written with CREATE TABLE - SQL
	FOR m.nInner_Loop = 1 TO m.nNumberOfFields
		IF m.nInner_Loop = 1
			m.cCreateTable = m.cCreateTable + aAll_Fields(m.nInner_Loop, 1) + " "
		ELSE
			m.cCreateTable = SPACE(LEN(m.cTableName) + 15) + ;
							aAll_Fields(m.nInner_Loop, 1) + " "
		ENDIF
		m.cCreateTable = m.cCreateTable + aAll_Fields(m.nInner_Loop, 2)
		DO CASE
			CASE aAll_Fields(m.nInner_Loop, 2) == "C"
				m.cCreateTable = m.cCreateTable + "(" + ;
								ALLTRIM(STR(aAll_Fields(m.nInner_Loop, 3))) + ")"
				IF aAll_Fields(m.nInner_Loop, 6)
					m.cCreateTable = m.cCreateTable + " NOCPTRANS"
				ENDIF
			CASE aAll_Fields(m.nInner_Loop, 2) == "M"
				IF aAll_Fields(m.nInner_Loop, 6)
					m.cCreateTable = m.cCreateTable + " NOCPTRANS"
				ENDIF
			CASE aAll_Fields(m.nInner_Loop, 2) == "N" OR ;
				aAll_Fields(m.nInner_Loop, 2) == "F"
				cCreateTable = m.cCreateTable + "(" + ;
				ALLTRIM(STR(aAll_Fields(m.nInner_Loop, 3))) + ;
				", " + ALLTRIM(STR(aAll_Fields(m.nInner_Loop, 4))) + ")"
			CASE aAll_Fields(m.nInner_Loop, 2) == "B"
				m.cCreateTable = m.cCreateTable + "(" + ;
				ALLTRIM(STR(aAll_Fields(m.nInner_Loop, 4))) ;
				+ ")"
		ENDCASE

		IF aAll_Fields(m.nInner_Loop, 5)
			m.cCreateTable = m.cCreateTable + " NULL"
		ELSE
			m.cCreateTable = m.cCreateTable + " NOT NULL"
		ENDIF

		*! Get properties for fields
		IF !EMPTY(aAll_Fields(m.nInner_Loop, 7))
			m.cCreateTable = m.cCreateTable + " CHECK " + aAll_Fields(m.nInner_Loop, 7)
		ENDIF

		IF !EMPTY(aAll_Fields(m.nInner_Loop, 8))
			m.cCreateTable = m.cCreateTable + " ERROR " + aAll_Fields(m.nInner_Loop, 8)
		ENDIF

		IF !EMPTY(aAll_Fields(m.nInner_Loop, 9))
			m.cCreateTable = m.cCreateTable + " DEFAULT " + aAll_Fields(m.nInner_Loop, 9)
		ENDIF

		IF m.nInner_Loop <> m.nNumberOfFields
			m.cCreateTable = m.cCreateTable + ", ;"
		ELSE
			m.cCreateTable = m.cCreateTable + ")"
		ENDIF

		WriteFile(m.hGTFile, m.cCreateTable)
	ENDFOR

	*! Get Index Information
	WriteFile(m.hGTFile, CRLF + "***** " + BEGIN_INDEX_LOC + m.cTableName + " *****")
	m.cCollate = ""
	FOR m.nInner_Loop = 1 TO TAGCOUNT()
		m.cTag = UPPER(ALLTRIM(TAG(m.nInner_Loop)))
		IF m.cCollate <> IDXCOLLATE(m.nInner_Loop)
			m.cCollate = IDXCOLLATE(m.nInner_Loop)
			WriteFile(m.hGTFile, "SET COLLATE TO '" + m.cCollate + "'")
		ENDIF
		IF !EMPTY(m.cTag)
			DO CASE
				CASE PRIMARY(m.nInner_Loop)
				     IF !EMPTY(SYS(2021, m.nInner_Loop))
				     	IF EMPTY(m.g_cFilterExp)
					        MessageBox(NOT_SUPPORTED_LOC, 64, WARNING_TITLE_LOC)
					    ENDIF
				        m.g_cFilterExp = m.g_cFilterExp + CRLF + ;
				                       TABLE_NAME_LOC + m.cTableName + CRLF + ;
				                       PRIMARY_KEY_LOC + SYS(14, m.nInner_Loop) + CRLF + ;
				                       FILTER_EXP_LOC + SYS(2021, m.nInner_Loop)
				     ENDIF
                     WriteFile(m.hGTFile, "ALTER TABLE '" + m.cTableName + ;
                                  "' ADD PRIMARY KEY " + SYS(14, m.nInner_Loop) ;
                                  + " TAG " + m.cTag)
				CASE CANDIDATE(m.nInner_Loop)
					IF EMPTY(SYS(2021, m.nInner_Loop))
						WriteFile(m.hGTFile, "INDEX ON " + SYS(14, m.nInner_Loop) + ;
				     				 " TAG " + m.cTag + " CANDIDATE")
					ELSE
				     	WriteFile(m.hGTFile, "INDEX ON " + SYS(14, m.nInner_Loop) + ;
				     				 " TAG " + m.cTag + " FOR " + SYS(2021, m.nInner_Loop) + ;
				     				 + " CANDIDATE")
					ENDIF
				CASE UNIQUE(m.nInner_Loop)
					IF(EMPTY(SYS(2021, m.nInner_Loop)))
					   WriteFile(m.hGTFile, "INDEX ON " + SYS(14, m.nInner_Loop) + ;
					                " TAG " + m.cTag + " UNIQUE")
					ELSE
					   WriteFile(m.hGTFile, "INDEX ON " + SYS(14, m.nInner_Loop);
                                    + " TAG " + m.cTag + " FOR " + SYS(2021, m.nInner_Loop) ;
                                    + " UNIQUE")
                    ENDIF
				OTHERWISE
					IF(EMPTY(SYS(2021, m.nInner_Loop)))
						WriteFile(m.hGTFile, "INDEX ON " + SYS(14, m.nInner_Loop) + ;
							" TAG " + m.cTag + ;
							IIF(DESCENDING(m.nInner_Loop), " DESCENDING ", ""))
					ELSE
						WriteFile(m.hGTFile, "INDEX ON " + SYS(14, m.nInner_Loop);
							+ " TAG " + m.cTag + " FOR " + SYS(2021, m.nInner_Loop) + ;
							IIF(DESCENDING(m.nInner_Loop), " DESCENDING ", ""))
					ENDIF                    
			ENDCASE
		ELSE
			EXIT FOR
		ENDIF
	ENDFOR

	*! Get Properties For Table
	WriteFile(hGTFile, CRLF + "***** " + BEGIN_PROP_LOC + m.cTableName + " *****")
	FOR m.nInner_Loop = 1 TO m.nNumberOfFields
		m.cFieldAlias = m.cTableName + "." + aAll_Fields(m.nInner_Loop, 1)
		m.cFieldHeaderAlias = [DBSETPROP('] + m.cFieldAlias + [', 'Field', ]
		m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "Caption")
		IF !EMPTY(cTemp)
			m.cTemp = STRTRAN(m.cTemp, ["], ['])
			WriteFile(hGTFile, m.cFieldHeaderAlias + ['Caption', "] + m.cTemp + [")])
		ENDIF
		m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "Comment")
		IF !EMPTY(m.cTemp)
			m.cTemp = STRTRAN(m.cTemp, ["], ['])
			*! Strip Line Feeds
			m.cTemp = STRTRAN(m.cTemp, CHR(10)) 
			*! Convert Carriage Returns To Programmatic Carriage Returns
			m.cTemp = STRTRAN(m.cTemp, CHR(13), '" + CHR(13) + "')
			WriteFile(m.hGTFile, m.cFieldHeaderAlias + ['Comment', "] + m.cTemp + [")])
		ENDIF
		m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "InputMask")
		IF !EMPTY(m.cTemp)
			m.cTemp = STRTRAN(m.cTemp, ["], ['])
			WriteFile(m.hGTFile, m.cFieldHeaderAlias + ['InputMask', "] + m.cTemp + [")])
		ENDIF
		m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "Format")
		IF !EMPTY(m.cTemp)
			m.cTemp = STRTRAN(m.cTemp, ["], ['])
			WriteFile(m.hGTFile, m.cFieldHeaderAlias + ['Format', "] + m.cTemp + [")])
		ENDIF
		
		m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "DisplayClass")
		IF !EMPTY(m.cTemp)
			m.cTemp = STRTRAN(m.cTemp, ["], ['])
			WriteFile(m.hGTFile, m.cFieldHeaderAlias + ['DisplayClass', "] + m.cTemp + [")])
		ENDIF

		m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "DisplayClassLibrary")
		IF !EMPTY(m.cTemp)
			m.cTemp = STRTRAN(m.cTemp, ["], ['])
			WriteFile(m.hGTFile, m.cFieldHeaderAlias + ['DisplayClassLibrary', "] + m.cTemp + [")])
		ENDIF
	ENDFOR
	
	m.cTemp = DBGETPROP(m.cTableName, "Table", "Comment")
	IF !EMPTY(m.cTemp)
		m.cTemp = STRTRAN(m.cTemp, ["], ['])
                *! Strip Line Feeds
                m.cTemp = STRTRAN(m.cTemp, CHR(10))
                *! Convert Carriage Returns To Programmatic Carriage Returns
                m.cTemp = STRTRAN(m.cTemp, CHR(13), '" + CHR(13) + "')
		WriteFile(m.hGTFile, [DBSETPROP('] + m.cTableName + [', 'Table', ] + ['Comment', "] + m.cTemp + [")])
	ENDIF

	m.cTemp = DBGETPROP(m.cTableName, "Table", "DeleteTrigger")
	IF !EMPTY(m.cTemp)
		WriteFile(hGTFile, "CREATE TRIGGER ON '" + m.cTableName + ;
							  "' FOR DELETE AS " + m.cTemp)
	ENDIF

	m.cTemp = DBGETPROP(m.cTableName, "Table", "InsertTrigger")
	IF !EMPTY(m.cTemp)
		WriteFile(m.hGTFile, "CREATE TRIGGER ON '" + m.cTableName + ;
							  "' FOR INSERT AS " + m.cTemp)
	ENDIF

	m.cTemp = DBGETPROP(m.cTableName, "Table", "UpdateTrigger")
	IF !EMPTY(m.cTemp)
		WriteFile(m.hGTFile, "CREATE TRIGGER ON '" + m.cTableName + ;
							  "' FOR UPDATE AS " + m.cTemp)
	ENDIF

	m.cTemp = DBGETPROP(m.cTableName, "Table", "RuleExpression")
	IF !EMPTY(m.cTemp)
		m.cError = DBGETPROP(m.cTableName, "Table", "RuleText")
		IF !EMPTY(cError)
			WriteFile(m.hGTFile, "ALTER TABLE '" + m.cTableName + ;
								  "' SET CHECK " + m.cTemp + " ERROR " + ;
								  m.cError)
		ELSE
			WriteFile(m.hGTFile, "ALTER TABLE '" + m.cTableName + ;
								  "' SET CHECK " + m.cTemp)
		ENDIF
	ENDIF
	WriteFile(m.hGTFile, "ENDFUNC")
	WriteFile(m.hGTFile, "")
	FCLOSE(m.hGTFile)
RETURN

**************************************************************************
**
** Function Name: GETVIEW(<ExpC>, <ExpC>)
** Creation Date: 1994.12.01
** Purpose        :
**
**              To take an existing FoxPro 3.0/5.0 View, and generate an output
**              program that can be used to "re-create" that view.
**
** Parameters:
**
**      cViewName    -  A character string representing the name of the 
**                      existing view
**      cOutFileName -  A character string containing the name of the 
**                      output file
**
** Modification History:
**
**  1994.12.01  JHL             Created Program, runs on Build 329 of FoxPro 3.0
**  1994.12.02  KRT             Added to GenDBC, removed third parameter, cleaned up
**  1994.12.08  KRT             Assume Database is open to speed up operation
**  1996.04.12  KRT             Added new properties for Visual FoxPro 5.0
**                              Prepared / CompareMemo / FetchAsNeeded
**	1996.05.14  KRT             Added more properties for views
** 	1996.05.16  KRT             Adjusted for return a blank string instead of a logical
**                              value on Prepared, etc.. if the field does not exist
**                              in the database (Version 3.0 database converted to 5.0)
**  1996.05.16  KRT             Added the DataType property
**  1996.06.26  KRT             Added support for ParameterList
**  1996.07.19  KRT             Added support for Offline Views
**  1996.08.07  KRT				Added support for BatchUpdateCount, Comment
***************************************************************************************
PROCEDURE GetView

	LPARAMETERS cViewName, cOutFileName

	PRIVATE ALL EXCEPT g_*

	m.nFileHand = FCREATE(m.cOutFileName, 0)
	IF m.nFileHand < 1
		FatalAlert(NO_TEMP_FILE_LOC + m.cOutFileName, .T.)
	ENDIF

	*! Get View Information for later use
	m.nSourceType = DBGetProp(m.cViewName, 'View', 'SourceType')
	m.cConnectName = ALLTRIM(DBGetProp(m.cViewName, 'View', 'ConnectName'))
	m.cSQL = ALLTRIM(DBGetProp(m.cViewName, 'View', 'SQL'))
	m.cnUpdateType = ALLTRIM(STR(DBGetProp(m.cViewName, 'View', 'UpdateType')))
	m.cnWhereType = ALLTRIM(STR(DBGetProp(m.cViewName, 'View', 'WhereType')))
	m.clFetchMemo = IIF(DBGetProp(m.cViewName, 'View', 'Fetchmemo'),'.T.','.F.')
	m.clShareConnection = IIF(DBGetProp(m.cViewName, 'View', 'ShareConnection'),'.T.','.F.')
	m.clSendUpdates = IIF(DBGetProp(m.cViewName, 'View', 'SendUpdates'),'.T.','.F.')
	m.cnUseMemoSize = ALLTRIM(STR(DBGetProp(m.cViewName, 'View', 'UseMemoSize')))
	m.cnFetchSize = ALLTRIM(STR(DBGetProp(m.cViewName, 'View', 'FetchSize')))
	m.cnMaxRecords = ALLTRIM(STR(DBGetProp(m.cViewName, 'View', 'MaxRecords')))
	m.ccTables = ALLTRIM(DBGetProp(m.cViewName, 'View', 'Tables'))
	m.clPrepared = IIF(!EMPTY(DBGetProp(m.cViewName, 'View', 'Prepared')), '.T.', '.F.')
	m.clCompareMemo = IIF(!EMPTY(DBGetProp(m.cViewName, 'View', 'CompareMemo')), '.T.', '.F.')
	m.clFetchAsNeeded = IIF(!EMPTY(DBGetProp(m.cViewName, 'View', 'FetchAsNeeded')), '.T.', '.F.')
	m.cParams = ALLTRIM(DBGetProp(m.cViewName, 'View', 'ParameterList'))
	m.lOffline = DBGetProp(m.cViewName, 'View', 'Offline')
	m.cComment = DBGETPROP(m.cViewName, 'View', 'Comment')
	IF !EMPTY(m.cComment )
		m.cComment = STRTRAN(m.cComment , ["], ['])
		*! Strip Line Feeds
		m.cComment = STRTRAN(m.cComment , CHR(10)) 
		*! Convert Carriage Returns To Programmatic Carriage Returns
		m.cComment = STRTRAN(m.cComment , CHR(13), '" + CHR(13) + "')
	ENDIF
	m.cnBatchUpdateCount = ALLTRIM(STR(DBGetProp(m.cViewName, 'View', 'BatchUpdateCount')))
	
	*! Generate Comment Block
	WriteFile(m.nFileHand, "FUNCTION MakeView_"+FIXNAME(m.cViewName))

	m.cCommentBlock = "***************** " + BEGIN_VIEW_LOC + m.cViewName + ;
                      " ***************" + CRLF

	WriteFile(m.nFileHand, m.cCommentBlock)

	*! Generate CREATE VIEW command
	m.cCreateString = 'CREATE SQL VIEW "'+ALLTRIM(m.cViewName)+'" ; '+CRLF

	IF m.nSourceType != 1     && If it isn't a local view
		m.cCreateString = m.cCreateString + '   REMOTE '
		IF !EMPTY(m.cConnectName)
			m.cCreateString = m.cCreateString + 'CONNECT "' + m.cConnectName + '" ; '+CRLF
		ENDIF
	ENDIF
	m.cCreateString = m.cCreateString + '   AS '+ m.cSQL + CRLF

	WriteFile(m.nFileHand, m.cCreateString)

	*! GENERATE code to Set View Level Properties
	m.cViewDBSetPrefix = [DBSetProp(']+m.cViewName+[', 'View', ]

	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['UpdateType', ] + m.cnUpdateType + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['WhereType', ] + m.cnWhereType + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['FetchMemo', ] + m.clFetchMemo + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['SendUpdates', ] + m.clSendUpdates + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['UseMemoSize', ] + m.cnUseMemoSize + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['FetchSize', ] + m.cnFetchSize + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['MaxRecords', ] + m.cnMaxRecords + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['Tables', '] + m.ccTables + [')])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['Prepared', ] + m.clPrepared + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['CompareMemo', ] + m.clCompareMemo + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['FetchAsNeeded', ] + m.clFetchAsNeeded + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['FetchSize', ] + m.cnFetchSize + [)])
	IF !EMPTY(m.cParams)
		WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['ParameterList', "] + m.cParams + [")])
	ENDIF
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['Comment', "] +  m.cComment + [")])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['BatchUpdateCount', ] + m.cnBatchUpdateCount + [)])
	WriteFile(m.nFileHand, m.cViewDBSetPrefix + ['ShareConnection', ] + m.clShareConnection + [)])
	IF m.lOffline
		WriteFile(m.nFileHand, 'CREATEOFFLINE("' + m.cViewName + '")')
	ENDIF
		

	*! GENERATE code to Set Field Level Properties
	USE (DBC()) AGAIN IN 0 ALIAS GenViewCursor EXCLUSIVE
	SELECT GenViewCursor
	LOCATE FOR ALLTRIM(UPPER(GenViewCursor.ObjectName)) == m.cViewName AND ;
    	GenViewCursor.ObjectType = 'View'
	m.nObjectId = GenViewCursor.ObjectId
	SELECT ObjectName FROM GenViewCursor ;
			WHERE GenViewCursor.ParentId = m.nObjectId ;
			INTO ARRAY aViewFields
	USE in GenViewCursor
	WriteFile(m.nFileHand, CRLF + '*!* Field Level Properties for ' + m.cViewName)

	IF _TALLY # 0
		FOR m.nLoop = 1 TO ALEN(aViewFields, 1)
			m.cFieldAlias = m.cViewName + "." + ALLTRIM(aViewFields(nLoop, 1))
			m.clKeyField = IIF(DBGetProp(m.cFieldAlias, 'Field', 'KeyField'),'.T.','.F.')
			m.clUpdatable = IIF(DBGetProp(m.cFieldAlias, 'Field', 'Updatable'),'.T.','.F.')
			m.ccUpdateName = ALLTRIM(DBGetProp(m.cFieldAlias, 'Field', 'UpdateName'))
			m.cViewFieldSetPrefix = [DBSetProp(']+m.cFieldAlias+[', 'Field', ]
			
			WriteFile(m.nFileHand, '* Props for the '+m.cFieldAlias+' field.')
			WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['KeyField', ] + m.clKeyField + [)])
			WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['Updatable', ] + m.clUpdatable + [)])
			WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['UpdateName', '] + m.ccUpdateName + [')])
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "RuleExpression")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['RuleExpression', "]+m.cTemp+[")])
			ENDIF
			
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "RuleText")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['RuleText', "]+m.cTemp+[")])
			ENDIF
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "Caption")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['Caption', "] + m.cTemp + [")])
			ENDIF
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "Comment")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				*! Strip Line Feeds
				m.cTemp = STRTRAN(m.cTemp, CHR(10)) 
				*! Convert Carriage Returns To Programmatic Carriage Returns
				m.cTemp = STRTRAN(m.cTemp, CHR(13), '" + CHR(13) + "')
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['Comment', "] + m.cTemp + [")])
			ENDIF
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "InputMask")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['InputMask', "] + m.cTemp + [")])
			ENDIF
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "Format")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['Format', "] + m.cTemp + [")])
			ENDIF
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "DisplayClass")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['DisplayClass', "] + m.cTemp + [")])
			ENDIF
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "DisplayClassLibrary")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['DisplayClassLibrary', "] + m.cTemp + [")])
			ENDIF
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "DataType")
			IF !EMPTY(m.cTemp)
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['DataType', "] + m.cTemp + [")])
			ENDIF
			m.cTemp = DBGETPROP(m.cFieldAlias, "Field", "DefaultValue")
			IF !EMPTY(m.cTemp)
				m.cTemp = STRTRAN(m.cTemp, ["], ['])
				WriteFile(m.nFileHand, m.cViewFieldSetPrefix + ['DefaultValue', "] + m.cTemp + [")])
			ENDIF
		ENDFOR
	ENDIF

	WriteFile(m.nFileHand, "ENDFUNC")
	WriteFile(m.nFileHand, " ")

	*! Close output file
	FCLOSE(m.nFileHand)

RETURN

**************************************************************************
**
** Function Name: GETCONN(<ExpC>, <ExpC>)
** Creation Date: 1995.01.03
** Purpose        :
**
**              To take an existing FoxPro 3.0/5.0 Connection, and generate
**              an output program that can be used to "re-create" that connection.
**
** Parameters:
**
**      cConnectName -  A character string representing the name of the 
**                      existing connection
**      m.cOutFileName -  A character string containing the name of the 
**                      output file
**
** Modification History:
**
**  1995.01.03  JHL     Created Program, runs on Build 329 of FoxPro 3.0
**  1995.01.05  KRT     Incorporated into GenDBC with modifications
**  1996.04.12  KRT     Added new property for Visual FoxPro 5.0 (Database)
***************************************************************************************
PROCEDURE GetConn
	LPARAMETERS cConnectionName, m.cOutFileName

	PRIVATE ALL EXCEPT g_*

	m.nFileHand = FCREATE(m.cOutFileName, 0)
	IF m.nFileHand < 1
		FatalAlert(NO_TEMP_FILE_LOC + m.cOutFileName, .T.)
	ENDIF

	*! Get Connection Information for later use
	m.clAsynchronous = IIF(DBGetProp(m.cConnectionName, 'Connection', 'Asynchronous'),'.T.','.F.')
	m.clBatchMode = IIF(DBGetProp(m.cConnectionName, 'Connection', 'BatchMode'),'.T.','.F.')
	m.ccComment = ALLTRIM(DBGetProp(m.cConnectionName, 'Connection', 'Comment'))
	m.ccConnectString = ALLTRIM(DBGetProp(m.cConnectionName, 'Connection', 'ConnectString'))
	m.cnConnectTimeOut = ALLTRIM(STR(DBGetProp(m.cConnectionName, 'Connection', 'ConnectTimeOut')))
	m.ccDataSource = ALLTRIM(DBGetProp(m.cConnectionName, 'Connection', 'DataSource'))
	m.cnDispLogin = ALLTRIM(STR(DBGetProp(m.cConnectionName, 'Connection', 'DispLogin')))
	m.clDispWarnings = IIF(DBGetProp(m.cConnectionName, 'Connection', 'DispWarnings'),'.T.','.F.')
	m.cnIdleTimeOut = ALLTRIM(STR(DBGetProp(m.cConnectionName, 'Connection', 'IdleTimeOut')))
	m.ccPassword = ALLTRIM(DBGetProp(m.cConnectionName, 'Connection', 'Password'))
	m.cnQueryTimeOut = ALLTRIM(STR(DBGetProp(m.cConnectionName, 'Connection', 'QueryTimeOut')))
	m.cnTransactions = ALLTRIM(STR(DBGetProp(m.cConnectionName, 'Connection', 'Transactions')))
	m.ccUserId = ALLTRIM(DBGetProp(m.cConnectionName, 'Connection', 'UserId'))
	m.cnWaitTime = ALLTRIM(STR(DBGetProp(m.cConnectionName, 'Connection', 'WaitTime')))
	m.ccDatabase = DBGetProp(m.cConnectionName, 'Connection', 'Database')

	*! Generate Comment Block
	m.cCommentBlock = "***************** " + BEGIN_CONNECTIONS_LOC + " " + m.cConnectionName + ;
		" ***************" + CRLF

	WriteFile(m.nFileHand, "FUNCTION MakeConn_"+FIXNAME(m.cConnectionName))
	WriteFile(m.nFileHand, m.cCommentBlock)

	*! Generate CREATE Connection command
	m.cCreateString = 'CREATE CONNECTION '+ALLTRIM(m.cConnectionName)+' ; '+CRLF

	IF EMPTY(ALLTRIM(m.ccConnectString))  && If connectstring not specified
		m.cCreateString = m.cCreateString + '   DATASOURCE "' + ALLT(m.ccDataSource) + '" ; ' + CRLF
		m.cCreateString = m.cCreateString + '   USERID "' + ALLT(m.ccUserId) + '" ; ' + CRLF
		m.cCreateString = m.cCreateString + '   PASSWORD "'+ ALLT(m.ccPassword) + '"' + CRLF
	ELSE
		m.cCreateString = m.cCreateString + '   CONNSTRING "' + ALLT(m.ccConnectString) + '"'
	ENDIF

	WriteFile(m.nFileHand, m.cCreateString)

	*! GENERATE code to Set Connection Level Properties
	m.cConnectionDBSetPrefix = [DBSetProp(']+m.cConnectionName+[', 'Connection', ]

	m.cConnectionProps = '****' + CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['Asynchronous', ] + m.clAsynchronous + [)]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['BatchMode', ] + m.clBatchMode + [)]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['Comment', '] + m.ccComment + [')]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['DispLogin', ] + m.cnDispLogin + [)]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['ConnectTimeOut', ] + m.cnConnectTimeOut + [)]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['DispWarnings', ] + m.clDispWarnings + [)]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['IdleTimeOut', ] + m.cnIdleTimeOut + [)]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['QueryTimeOut', ] + m.cnQueryTimeOut + [)]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
						['Transactions', ] + m.cnTransactions + [)]+ CRLF
	m.cConnectionProps = m.cConnectionProps + m.cConnectionDBSetPrefix + ;
					    ['Database', '] + m.ccDatabase + [')] + CRLF
					    
	WriteFile(m.nFileHand, m.cConnectionProps)
	*! Close output file
	WriteFile(m.nFileHand, "ENDFUNC")
	WriteFile(m.nFileHand, " ")

	FCLOSE(m.nFileHand)
RETURN

**************************************************************************
**
** Function Name: FATALALERT(<ExpC>)
** Creation Date: 1994.12.02
** Purpose:
**
**              Place a message box to alert user of a fatal error.
**
** Parameters:
**
**      cAlert_Message - Message to display to user
**      lCleanup       - If we should try to restore environment
**
** Modification History:
**
**      1994.12.02  KRT  Added to GenDBC
**************************************************************************
PROCEDURE FatalAlert
	LPARAMETERS cAlert_Message, lCleanup

	MESSAGEBOX(m.cAlert_Message, 16, ERROR_TITLE_LOC)

	GenDBC_CleanUp(m.lCleanup)

	CANCEL
RETURN

**************************************************************************
**
** Function Name: GenDBC_CleanUp(<ExpL>)
** Creation Date: 1995.03.01
** Purpose:
**
**              Restore the environment
**
** Parameters:
**
**      lCleanup - If we should try to restore tables open
**
** Modification History:
**
**      1994.03.01  KRT         Added to GenDBC
**************************************************************************
PROCEDURE GenDBC_CleanUp
	LPARAMETERS lCleanup

	*! Restore everything
	IF !EMPTY(m.g_cOnError)
		ON ERROR &g_cOnError
	ELSE
		ON ERROR
	ENDIF
	
	IF !EMPTY(m.g_cSetTalk)
		SET TALK &g_cSetTalk
	ENDIF
	
	IF !EMPTY(m.g_cSetDeleted)
		SET DELETED &g_cSetDeleted
	ENDIF
	
	IF m.g_cSetStatusBar = "OFF"
		SET STATUS BAR OFF
	ENDIF
	
	IF !EMPTY(m.g_cStatusText)
		SET MESSAGE TO (m.g_cStatusText)
	ELSE
		SET MESSAGE TO
	ENDIF
	
	SET FULLPATH &g_cFullPath
	CLOSE ALL

	IF m.lCleanUp
		IF !EMPTY(m.g_cFullDatabase) AND m.lCleanUp == .T.
			OPEN DATABASE (m.g_cFullDatabase) EXCLUSIVE
			IF m.g_nTotal_Tables_Used > 0
				FOR m.nLoop = 1 TO m.g_nTotal_Tables_Used
					IF UPPER(JUSTEXT(m.g_aTables_Used(m.nLoop)))="TMP"
						LOOP
					ENDIF
					USE (m.g_aTables_Used(m.nLoop)) IN (m.g_aAlias_Used(m.nLoop, 2)) EXCLUSIVE;
						ALIAS (m.g_aAlias_Used(m.nLoop, 1))
				ENDFOR
			ENDIF
		ENDIF
	ENDIF
RETURN

**************************************************************************
**
** Function Name: WRITEFILE(<ExpN>, <ExpC>)
** Creation Date: 1994.12.02
** Purpose        :
**
**              Centralized file output routine to check for proper output
**
** Parameters:
**
**      hFileHandle - Handle of output file
**      cText       - Contents to write to file
**
** Modification History:
**
**      1994.12.02  KRT         Added to GenDBC
**************************************************************************
PROCEDURE WriteFile
	LPARAMETERS hFileHandle, cText

	m.nBytesSent = FPUTS(m.hFileHandle, m.cText)
	IF  m.nBytesSent < LEN(m.cText)
		FatalAlert(NO_OUTPUT_WRITTEN_LOC, .T.)
	ENDIF
RETURN

**************************************************************************
**
** Function Name: GenDBC_Error(<expC>, <expN>)
** Creation Date: 1994.12.02
** Purpose        :
**
**              Generalized Error Routine
**
** Parameters:
**
**      cMess   - Message to give user
**      nLineNo - Line Number Error Occurred
**
** Modification History:
**
**      1994.12.02  KRT         Added to GenDBC
**************************************************************************
PROCEDURE GenDBC_Error
	LPARAMETERS cMess, nLineNo

	FatalAlert(UNRECOVERABLE_LOC + CRLF + m.cMess + CRLF + ;
				  AT_LINE_LOC + ALLTRIM(STR(m.nLineNo)), .T.)
RETURN

**************************************************************************
**
** Function Name: Stat_Message()
** Creation Date: 1994.01.08
** Purpose        :
**
**              Generalized Status Bar Progression
**
** Parameters:
**
**              None
**
** Modification History:
**
**      1994.01.08  KRT         Added to GenDBC
**************************************************************************
PROCEDURE Stat_Message
	PRIVATE ALL EXCEPT g_*
	
	m.nStat = m.g_nCurrentStat * (160 / g_nMax)
	SET MESSAGE TO REPLICATE("|", m.nStat) + " " + ;
		ALLTRIM(STR(INT(100 * (m.g_nCurrentStat / m.g_nMax)))) + "%"
	m.g_nCurrentStat = m.g_nCurrentStat + 1
RETURN

**************************************************************************
**
** Function Name: UpdateProcArray(<ExpC>)
** Creation Date: 1997.10.22
** Purpose        :
**
**              Update g_aprocs array with procedure name
**
** Parameters:
**
**      cText       - Name of procedure to add to array
**
** Modification History:
**
**      1997.10.22  RB         Added to GenDBC
**************************************************************************
PROCEDURE UpdateProcArray(lcProcName)
	IF g_lskipdisplay AND ATC("DisplayStatus",lcprocname)#0
		RETURN
	ENDIF 
	IF !EMPTY(g_aprocs[ALEN(g_aprocs)])
		DIMENSION g_aprocs[ALEN(g_aprocs)+1]
	ENDIF
	g_aprocs[ALEN(g_aprocs)] = lcProcName
ENDPROC

**************************************************************************
**
** Function Name: FixName(<ExpC>)
** Creation Date: 1997.10.22
** Purpose        :
**
**              Fixes procedure name to remove bad chars
**
** Parameters:
**
**      cText       - Name of procedure to add fix
**
** Modification History:
**
**      1997.10.22  RB         Added to GenDBC
**************************************************************************
PROCEDURE FixName(lcProcName)
	lcProcName=ALLTRIM(lcProcName)
	IF VERSION(3) $ DBCS_LOC
		cbadchars = '/,-=:;!@#$%&*.<>()?[]\'+;
		   '+'+CHR(34)+CHR(39)+" "
	ELSE
		cbadchars = 'ÅÇÉÑÖÜáàâäãåéèêëíìîïñóòôö†°¢£§•/\,-=:;{}[]!@#$%^&*.<>()?'+;
		   '+|Äõúùûü¶ß®©™´¨≠ÆØ∞±≤≥¥µ∂∑∏π∫ªºΩæø¿¡¬√ƒ≈∆«»… ÀÃÕŒœ'+;
		   '–—“”‘’÷◊ÿŸ⁄€‹›ﬁﬂ‡·‚„‰ÂÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜¯˘˙˚¸˝˛'+CHR(34)+CHR(39)+" "
	ENDIF
	lcProcName = CHRTRAN(lcProcName,cbadchars ,REPL('_',LEN(cbadchars)))
	RETURN lcProcName
ENDPROC