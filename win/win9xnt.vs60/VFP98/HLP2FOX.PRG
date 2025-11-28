LPARAMETERS cSample, cAction
IF PCOUNT() != 2 OR TYPE("cSample") != "C" OR TYPE("cAction") != "C"
	RETURN
ENDIF

LOCAL lcFile, lcSuffix, lcPath
m.lcFile = ""
m.lcPath = ""
cSample = UPPER(cSample)
cAction = UPPER(cAction)

m.lcSuffix = IIF(cAction = "MODIFY", ".PJX", ".APP")

#DEFINE ERROR_LOC "Error"
#DEFINE ERR1_LOC "File not found." + CHR(13) + "Please make sure the following file is installed:" + CHR(13)
#DEFINE SPATH_LOC "Solution\"
#DEFINE SFILE_LOC "solution"
#DEFINE TTPATH_LOC "Tastrade\"
#DEFINE TTFILE_LOC "tastrade"
#DEFINE CSPATH_LOC "CSApp\"
#DEFINE CSFILE_LOC "sample"
#DEFINE POOLPATH_LOC "Servers\Poolmgr\"
#DEFINE POOLFILE_LOC "pool"
#DEFINE GOPHERPATH_LOC "Servers\Gopher\"
#DEFINE GOPHERFILE_LOC "foxsrch"
#DEFINE ISAPIPATH_LOC "Servers\Foxisapi\FoxWeb\"
#DEFINE ISAPIFILE_LOC "foxweb"


DO CASE 
	CASE cSample = "SOLUTION"
	*------------------------
		m.lcPath = HOME(2) + SPATH_LOC
		m.lcFile = SFILE_LOC + m.lcSuffix
		
	CASE cSample = "TASTRADE"
	*------------------------
		m.lcPath = HOME(2) + TTPATH_LOC
		m.lcFile =  TTFILE_LOC + m.lcSuffix
		
	CASE cSample = "SAMPLE"
	*------------------------
		m.lcPath = HOME(2) + CSPATH_LOC
		m.lcFile = CSFILE_LOC + m.lcSuffix

	CASE cSample = "POOL"
	*------------------------
		m.lcPath = HOME(2) + POOLPATH_LOC
		m.lcFile = POOLFILE_LOC + m.lcSuffix
		cAction = "MODIFY"
		
	CASE cSample = "FOXSRCH"
	*------------------------
		m.lcPath = HOME(2) + GOPHERPATH_LOC
		m.lcFile = GOPHERFILE_LOC + m.lcSuffix
		cAction = "MODIFY"
		
	CASE cSample = "FOXWEB"
	*------------------------
		m.lcPath = HOME(2) + ISAPIPATH_LOC
		m.lcFile = ISAPIFILE_LOC + m.lcSuffix
		cAction = "MODIFY"
		
	OTHERWISE
	*------------------------
		RETURN		
ENDCASE
IF _SCREEN.WindowState = 1 && minimized
	_SCREEN.WindowState = 0 && normal
ENDIF

IF !FILE(m.lcPath + m.lcFile)
		MESSAGEBOX(ERR1_LOC + CHR(13) + CHR(13) + m.lcPath + m.lcFile, 48+0+0, ERROR_LOC)
		RETURN
	ENDIF
DO CASE 
	CASE cAction = "MODIFY"
		MODIFY PROJECT (m.lcPath + m.lcFile) NOWAIT
	CASE cAction = "RUN"
		CLOSE DATA ALL
		CD (lcPath)
		ACTIVATE WINDOW COMMAND
		KEYBOARD "DO " + m.lcFile + '{ENTER}'
ENDCASE

