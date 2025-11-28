*-- (c) Microsoft Corporation 1995

#INCLUDE "INCLUDE\TASTRADE.H"

*-- DECLARE DLL statements for reading/writing to private INI files
DECLARE INTEGER GetPrivateProfileString IN Win32API  AS GetPrivStr ;
  String cSection, String cKey, String cDefault, String @cBuffer, ;
  Integer nBufferSize, String cINIFile

DECLARE INTEGER WritePrivateProfileString IN Win32API AS WritePrivStr ;
  String cSection, String cKey, String cValue, String cINIFile

*-- DECLARE DLL statements for reading/writing to system registry
DECLARE Integer RegOpenKeyEx IN Win32API ;
  Integer nKey, String @cSubKey, Integer nReserved,;
  Integer nAccessMask, Integer @nResult

DECLARE Integer RegQueryValueEx IN Win32API ;
  Integer nKey, String cValueName, Integer nReserved,;
  Integer @nType, String @cBuffer, Integer @nBufferSize

DECLARE Integer RegCloseKey IN Win32API ;
  Integer nKey

*-- DECLARE DLL statement for Windows 3.1 API function GetProfileString
DECLARE INTEGER GetProfileString IN Win32API AS GetProStr ;
  String cSection, String cKey, String cDefault, ;
  String @cBuffer, Integer nBufferSize

CLEAR

*-- Ensure the project manager is closed, or we may run into
*-- conflicts when trying to KEYBOARD a hot-key
DEACTIVATE WINDOW "Project Manager"

*-- All public vars will be released as soon as the application
*-- object is created. 

IF SET('TALK') = 'ON'
  SET TALK OFF
  PUBLIC gcOldTalk
  gcOldTalk = 'ON'
ELSE
  PUBLIC gcOldTalk
  gcOldTalk = 'OFF'
ENDIF

PUBLIC gcOldDir, gcOldPath, gcOldClassLib, gcOldEscape, gTTrade
gcOldEscape   = SET('ESCAPE')
gcOldDir       = FULLPATH(CURDIR())
gcOldPath     = SET('PATH')
gcOldClassLib = SET('CLASSLIB')
gTTrade = .T.

*-- Set up the path so we can instantiate the application object
IF SetPath()
  PUBLIC oApp
  oApp = CREATEOBJECT("TasTrade")
  IF TYPE('oApp') = "O"
    *-- Release all public vars, since their values were
    *-- picked up by the Environment class
    RELEASE gcOldDir, gcOldPath, gcOldClassLib, gcOldTalk, gcOldEscape
    oApp.Do()
  ENDIF
ENDIF

CLEAR DLLS
RELEASE ALL EXTENDED
CLEAR ALL

FUNCTION SetPath()
  LOCAL lcSys16, ;
        lcProgram

  lcSys16 = SYS(16)
  lcProgram = SUBSTR(lcSys16, AT(":", lcSys16) - 1)

  CD LEFT(lcProgram, RAT("\", lcProgram))
  *-- If we are running MAIN.PRG directly, then
  *-- CD up to the parent directory
  IF RIGHT(lcProgram, 3) = "FXP"
    CD ..
  ENDIF
  SET PATH TO PROGS, FORMS, LIBS, ;
        MENUS, DATA, OTHER, ;
        REPORTS, INCLUDE, HELP, ;
        BITMAPS
  SET CLASSLIB TO MAIN, TSGEN
ENDFUNC
