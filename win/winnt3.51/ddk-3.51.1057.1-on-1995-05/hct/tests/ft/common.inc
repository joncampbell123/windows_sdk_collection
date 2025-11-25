'*****************************************************************************
' Copyright (c) 1991 Microsoft Corporation
'
' Module Name:
'   common.inc
'
' Abstract:
'   Contains the commonly shared procedures, constants, and global varables
'   for the different mstest scripts.
'
'     SelectRegion() - selects the specified disk region in diskman
'     GetConfig()    - reads the create.dat file so the disk component can be
'                      created/broken/deleted etc.
'     CreateNewConfig() - updates the ft type field of the creat.dat file to
'                         reflect a change in the state of the ft componet
'
' Author:
'   Kenieth Peery (t-kenpe) 12-9-92
'
' Revision History:
'  12-19-92 modifed the create.dat fileio for creating extended regions
'*****************************************************************************

'$DEFINE DEBUG_VER  '*** the chkbuild version

CONST MIRROR$="Fresh Mirror"
CONST CPBT_MIRROR$="Copy On Boot Mirror"
CONST BROKEN_MIRROR$="broken_mirror"
CONST STRWP$="Stripe With Parity"
CONST STRWOP$="Stripe Without Parity"
CONST VOLSET$="Volume Set"
CONST EXTVOL$="Extended Volume Set"
CONST PARTITION$="Simple Partition"
CONST NTFS$="ntfs"
CONST FAT$="fat"
CONST UNKNOWABLE$="Unknowable"

CONST LOG_SCREEN=1
CONST LOG_DISK=2

CONST  MAXSTRLEN=32
CONST  MAX_NUMREGIONS=32

GLOBAL disk%(MAX_NUMREGIONS)     '*** the regions selected
GLOBAL pos%(MAX_NUMREGIONS)
GLOBAL toExtend%(MAX_NUMREGIONS)
GLOBAL gDisk%,gPos% '*** (gDisk,gSpacePos) holds the currently selected region

GLOBAL gLogFileHandle%
GLOBAL gLogFileOptions%
GLOBAL gForceShutDown%
GLOBAL gAlwaysShutDown%

TYPE CAPTIONS
  winCapStartup AS STRING * MAXSTRLEN       '*** caption of startup group
  winCapFtOrphaning AS STRING * MAXSTRLEN       '*** caption of ft orphaning
  winCapDiskAdministrator AS STRING * MAXSTRLEN '*** caption of windisk
  winCapConfirm AS STRING * MAXSTRLEN
  winCapFormat AS STRING * MAXSTRLEN
  winCapFormatting AS STRING * MAXSTRLEN
  winCapFormatComplete AS STRING * MAXSTRLEN
  winCapCreatePrimaryPartition AS STRING * MAXSTRLEN
  winCapCreateExtendedPartition AS STRING * MAXSTRLEN
  winCapCreateLogicalDrive AS STRING * MAXSTRLEN

  butCapOk  AS STRING * MAXSTRLEN               '*** caption ok button
END TYPE

DECLARE SUB AvoidLeadingPopups(butCapOk$)

DECLARE SUB OpenLog(logFile$,options%)
DECLARE SUB CloseLog()
DECLARE SUB WriteLog(aLine$)
DECLARE SUB LogBanner(aLine$)

DECLARE SUB SelectRegion(disk%,pos%)
DECLARE SUB GetConfig(fileName$,ftType$,fileSystem$,szMB$,driveLet$,numRegions%)
DECLARE SUB CreateNewConfig(oldFileName$,newFileName$,ftType$)

DECLARE SUB GetIntlStrs(fileName$, intlCaptions AS CAPTIONS)

DECLARE FUNCTION GetFtVersion$()
DECLARE SUB GiveWindiskFocus(title$,butCapOk$)
DECLARE SUB CommitAndExit(title$,butCapOk$,winCapConfirm$)
DECLARE SUB Do10StyleCommitAndExit()
DECLARE SUB Do10AStyleCommitAndExit(title$,butCapOk$,winCapConfirm$)
DECLARE FUNCTION StrPrint$(src$,subsitute$)

'*** Let us initialize
gDisk=0
gPos=0
gForceShutDown=FALSE     'BUGBUG: This should default to FALSE
gAlwaysShutDown=FALSE

'*** define common key sequences for  WINDISK
Version$ = GetFtVersion$()

IF ( Version$ = "1.0" ) THEN
    BreakSeq$        = "%(){RIGHT}{ENTER}{DOWN}{ENTER}"
    CreateExtSeq$    = "%(){ENTER}{DOWN}{ENTER}"
    CreatePriSeq$    = "%(){ENTER}{ENTER}"
    CreateSWOPSeq$   = "%(){ENTER}{DOWN}{DOWN}{DOWN}{DOWN}{DOWN}{ENTER}"
    CreateSWPSeq$    = "%(){RIGHT}{ENTER}{DOWN}{DOWN}{ENTER}"
    CreateVolSetSeq$ = "%(){ENTER}{DOWN}{DOWN}{DOWN}{ENTER}"
    DeleteSeq$       = "%(){ENTER}{DOWN}{DOWN}{ENTER}"
    DriveLetterSeq$  = "%(){ENTER}{UP}{UP}{UP}{ENTER}"
    EstabMirSeq$     = "%(){RIGHT}{ENTER}{ENTER}"
    RegenSeq$        = "%(){RIGHT}{ENTER}{UP}{ENTER}"
ELSEIF ( Version$ = "1.0A" ) THEN
    BreakSeq$         = "%(){RIGHT}{DOWN}{DOWN}{ENTER}"
    CreateExtSeq$     = "%(){DOWN}{DOWN}{ENTER}"
    CreatePriSeq$     = "%(){DOWN}{ENTER}"
    CreateSWOPSeq$    = "%(){DOWN}{DOWN}{DOWN}{DOWN}{DOWN}{DOWN}{ENTER}"
    CreateSWPSeq$     = "%(){RIGHT}{DOWN}{DOWN}{DOWN}{ENTER}"
    CreateVolSetSeq$  = "%(){DOWN}{DOWN}{DOWN}{DOWN}{ENTER}"
    ExtendVolSetSeq$  = "%(){DOWN}{DOWN}{DOWN}{DOWN}{DOWN}{ENTER}"
    DeleteSeq$        = "%(){DOWN}{DOWN}{DOWN}{ENTER}"
    DriveLetterSeq$   = "%(){RIGHT}{RIGHT}{DOWN}{DOWN}{DOWN}{ENTER}"
    EstabMirSeq$      = "%(){RIGHT}{DOWN}{ENTER}"
    FormatDriveSeq$   = "%(){RIGHT}{RIGHT}{DOWN}{ENTER}"
    CommitChangesSeq$ = "%(){UP}{UP}{UP}{ENTER}"
    RegenSeq$         = "%(){RIGHT}{DOWN}{DOWN}{DOWN}{DOWN}{ENTER}"
END IF


'***-----------------------------------------------------------------------
'***
'*** SelectRegion
'***
'*** Routine to select the given region
'***
'*** The global vars 'gDisk' and 'gPos' are used and updated to indicate
'*** the current selector position.
'***
'***-----------------------------------------------------------------------

STATIC SUB SelectRegion(disk%,pos%)

  '***(disk%,pos%) the distination of pointer

  WriteLog STR$(disk)+" "+STR$(pos)

  WHILE gDisk > disk     '*** If the current location is to great move up
    gDisk=gDisk-1
    gPos=0
    QueKeys "{UP}"
  WEND

  WHILE gDisk < disk     '*** If the current location is to low move down
    gDisk=gDisk+1
    gPos=0
    QueKeys "{DOWN}"
  WEND

  WHILE gPos > pos       '*** If the current location is too right mv left
    gPos=gPos-1
    QueKeys "{LEFT}"
  WEND

  WHILE gPos < pos       '*** If the current location is too left mv right
    gPos=gPos+1
    QueKeys "{RIGHT}"
  WEND

  QuePause 500
  QueKeys "^({ENTER})"   '*** select the next partition
  QueFlush 1
END SUB

'***-----------------------------------------------------------------------
'***
'*** The file format
'*** Window Captions
'*** winCapDiskAdministrator$ winCapFtOphaning$
'*** Button Captions
'*** butCapOk$ butCapYes$ butCapNo$
'***
'***-----------------------------------------------------------------------

STATIC SUB GetIntlStrs(fileName$, intlCaptions AS CAPTIONS)
DIM infile%
DIM tmp$

  infile=FREEFILE
  OPEN fileName$ FOR INPUT AS #infile

  LINE INPUT #infile, tmp$
  intlCaptions.winCapStartup=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapFtOrphaning=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapDiskAdministrator=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapConfirm=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapFormat=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapFormatting=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapFormatComplete=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapCreatePrimaryPartition=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapCreateExtendedPartition=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.winCapCreateLogicalDrive=tmp$

  LINE INPUT #infile, tmp$
  intlCaptions.butCapOk=tmp$

  CLOSE #infile

END SUB

'***-----------------------------------------------------------------------
'***
'*** The file format
'*** ftType$ (i.e.  mirror broken_mirror strip_wp stripe_wop volume partition)
'*** fileSystem$ (fat hpfs ntfs)
'*** szMB$
'*** driveLet$ (the drive letter) (a broken_mirror 2nd part is driveLet+1)
'*** numPrim              '*** only needed for first region
'*** numRegions%
'*** disk%
'*** pos%
'*** ...
'*** disk%() and pos%() passed globally
'***
'***-----------------------------------------------------------------------

STATIC SUB GetConfig(fileName$,ftType$,fileSystem$,szMB$,driveLet$,numRegions%)
DIM infile%, i%, tmp$

  infile=FREEFILE
  OPEN fileName$ FOR INPUT AS #infile
  LINE INPUT #infile, tmp       '*** number of ftComponets
  LINE INPUT #infile, ftType
  LINE INPUT #infile, fileSystem
  LINE INPUT #infile, szMB
  LINE INPUT #infile, driveLet
  LINE INPUT #infile, tmp
  numRegions=VAL(tmp)

  i=0
  WHILE NOT(EOF(infile)) AND  (i < numRegions) '*** get the regions
    LINE INPUT #infile, tmp
    disk(i)=VAL(tmp)
    LINE INPUT #infile, tmp
    pos%(i)=VAL(tmp)
    LINE INPUT #infile, tmp
    toExtend%(i)=VAL(tmp)
    i=i+1
  WEND

  CLOSE #infile

  IF i < numRegions THEN
    PAUSE "Unexpected End of File"
    END
  END IF
END SUB

'***-----------------------------------------------------------------------
'***
'*** Creates a New config file with a differrent ftType
'*** same file format (for broken_mirror)
'***
'***-----------------------------------------------------------------------

STATIC SUB CreateNewConfig(oldFileName$,newFileName$,ftType$)

DIM oldftType$,fileSystem$,szMB$,driveLet$,numRegions%
DIM outfile%, i%, tmp$

  GetConfig oldFileName,oldFtType,fileSystem,szMB,driveLet,numRegions

  outfile=FREEFILE
  OPEN newFileName$ FOR OUTPUT AS #outfile

  PRINT #outfile, 1
  PRINT #outfile, ftType
  PRINT #outfile, fileSystem
  PRINT #outfile, szMB
  PRINT #outfile, driveLet
  PRINT #outfile, numRegions

  i=0
  WHILE i < numRegions        '*** write the regions
    PRINT #outfile, disk(i)
    PRINT #outfile, pos%(i)
    PRINT #outfile, toExtend%(i)
    i=i+1
  WEND

  CLOSE #outfile

END SUB

'***-----------------------------------------------------------------------
'***
'*** This routine opens the log file to a disk or the viewport
'***
'***-----------------------------------------------------------------------

STATIC SUB OpenLog(logFile$,options%)
  gLogFileOptions=options

  IF options AND LOG_DISK THEN
    gLogFileHandle=FREEFILE
    OPEN logFile FOR OUTPUT AS #gLogFileHandle
  ELSE
    gLogFileHandle=0
  END IF
END SUB

'***-----------------------------------------------------------------------
'***
'*** This routine closes the log file to a disk or the viewport
'***
'***-----------------------------------------------------------------------

STATIC SUB CloseLog()
  IF gLogFileOptions AND LOG_DISK THEN
    CLOSE #gLogFileHandle
  END IF
END SUB

'***-----------------------------------------------------------------------
'***
'*** This routine sends the logging data to the opened loging devices
'***
'***-----------------------------------------------------------------------

STATIC SUB WriteLog(aLine$)
  IF gLogFileOptions AND LOG_SCREEN THEN
    PRINT aLine$
  END IF

  IF gLogFileOptions AND LOG_DISK THEN
    PRINT #gLogFileHandle, aLine$
  END IF
END SUB

'***-----------------------------------------------------------------------
'***
'*** this routine boaders the line with asterixs and cernters the line
'***
'***-----------------------------------------------------------------------

STATIC SUB LogBanner(aLine$)
  DIM spaces$

  WriteLog "***************************************************************"
  WriteLog ""

  numSpaces%=0
  IF LEN(aLine$) < 80 THEN
    numSpaces=40-(LEN(aLine$)/2)
  END IF

  spaces=""
  WHILE numSpaces%
    spaces=spaces+" "
    numSpaces=numSpaces-1
  WEND

  WriteLog spaces$+aLine$
  WriteLog ""
  WriteLog "***************************************************************"

END SUB


'***-----------------------------------------------------------------------
'***
'*** This little routine gets rid of the nasty popups that appear either as
'*** a result of a change of disk configuration or because the diskman
'*** has never been run.
'*** This little routine assumes the popups have "OK" buttons and the
'*** actual diskman.exe does not.  It just click the OK button until no more
'*** remain.
'***
'***-----------------------------------------------------------------------

STATIC SUB AvoidLeadingPopups(butCapOk$)
  count%=10
  SLEEP(1)
  WHILE WButtonExists(butCapOk$) AND count% > 0  '** Clear leading popups
    WButtonClick butCapOk$
    SLEEP(1)
    count=count-1
  WEND

  IF count%=0 THEN
    SHELL "copy donothin.0 donext"
    PAUSE "Unexpected popup loop.  This should never happen."
    END
  END IF
END SUB

'***-----------------------------------------------------------------------
'***
'*** GiveWindiskFocus
'***
'*** Routine to do just that.  It will RUN windisk.exe iff it is not
'*** already running.
'***
'***-----------------------------------------------------------------------

STATIC SUB GiveWindiskFocus(title$,butCapOk$)

    SetDeadlockTimeout(120000)  '*** extend the deadlock timeout to 2 minutes

    TrimTitle$    = RTRIM$(title$)
    trimButCapOk$ = RTRIM$(butCapOk$)

    winHandle% = WFndWndWait(TrimTitle$, FW_PART, 0)

    IF ( winHandle% = 0 ) THEN

        RUN "windisk.exe", NOWAIT

        AvoidLeadingPopups trimButCapOk$

        winHandle% = WFndWndWait(TrimTitle$, FW_FOCUS, 15)

        IF ( winHandle% = 0 ) THEN
            PAUSE "Could not start windisk.exe"
            EXIT SUB
        END IF

    END IF

    wMaxWnd winHandle%
    wSetActWnd(winHandle%)

END SUB

'***-----------------------------------------------------------------------
'***
'*** Function to get the "ft_version".
'***
'*** Possible return values are the following:
'***   1.0   - version 1.0 of ft (for nt 1.0)
'***   1.0A  - version 1.0 of ft (for nt 1.0A)
'***   blank - error occured
'***
'*** The ft_version is currently indicated by the environment
'*** variable ft_version.  The current default is 1.0.
'***
'*** If ft_version is set to some unknown value, this routine
'*** will execute a pause, and return the empty string.
'***
'***-----------------------------------------------------------------------

STATIC FUNCTION GetFtVersion$()

    ' Get the env setting

    Version$ = ENVIRON$("ft_version")

    ' Give it a default if not set

    IF ( Version$ = "" ) THEN
        Version$ = "1.0"
    END IF

    ' If OK, return it.  Else inform user and pause.

    IF ( Version$ = "1.0" ) THEN
        GetFtVersion = Version$
    ELSEIF ( Version$ = "1.0A" ) THEN
        GetFtVersion = Version$
    ELSE
        PAUSE "Unknown version - fix ft_version env setting"
        GetFtVersion = ""
    END IF

END FUNCTION

'***-----------------------------------------------------------------------
'***
'*** Routine to commit the changes and to exit diskman.
'***
'*** If a reboot is necessary for the changes to become active, then
'*** this processing creates a file named "shutdown.sig".  The existance
'*** of this file is used by startup.cmd to determine whether a shutdown
'*** needs to occur or not.  This code assumes that "shutdown.sig" does
'*** not initially exist.
'***
'***-----------------------------------------------------------------------

STATIC SUB CommitAndExit(title$,butCapOk$, winCapConfirm$)

    trimTitle$=RTRIM$(title$)
    trimOk$=RTRIM$(butCapOk$)
    trimCapConfirm$=RTRIM$(winCapConfirm$)

    Version$ = GetFtVersion()

    IF ( Version$ = "1.0" ) THEN
        Do10StyleCommitAndExit
    ELSEIF ( Version$ = "1.0A" ) THEN
        Do10AStyleCommitAndExit trimTitle$, trimOk$, trimCapConfirm$
    ELSE
        PAUSE "Unknown Version"
    END IF

END SUB

'***-----------------------------------------------------------------------
'***
'*** Routine that will commit changes, exit windisk and force a
'*** shutdown in a 1.0A style.  CommitAndExit calls this routine.
'***
'*** Verion 1.0 always requires a reboot.
'***
'***-----------------------------------------------------------------------

STATIC SUB Do10StyleCommitAndExit()

    SHELL "shutting down > shutdown.sig"

    QueKeys "%(){ENTER}{UP}{ENTER}" '*** Exit stage right
    QuePause 1000
    QueKeys "{ENTER}"               '*** save changes
    QuePause 1000
    QueKeys "{ENTER}"               '*** Disks Updated succesfully
                                    '*** need something conditional
                                    '*** here if not
    QueKeys "{ENTER}"               '*** Reboot

    QuePause 1000
    QueFlush 1

END SUB

'***-----------------------------------------------------------------------
'***
'*** Routine that will exit windisk and force a shutdown in a 1.0 style.
'*** CommitAndExit calls this routine.
'***
'*** Verion 1.0A does not always require a reboot, and this routine
'*** will autodetect if a boot is needed.
'***
'***-----------------------------------------------------------------------

STATIC SUB Do10AStyleCommitAndExit(title$, butCapOk$, winCapConfirm$)

    '
    ' Always leave the current selection at 0 in case we don't exit
    ' windisk.  This way, the next ms-test script can "re-attach"
    ' and know where the current selector position is.
    '

    SelectRegion 0, 0
    SelectRegion 0, 0   '*** a second selection is a un-selection

    '
    ' Send the 1.0 stuff until the new windisk arrives with commit etc...
    '

    QueKeys "%(){ENTER}{UP}{ENTER}" '*** Exit stage right
    QuePause 800
    QueKeys "{ENTER}"               '*** save changes
    QuePause 1000
    QueKeys "{ENTER}"               '*** Disks updated successfully 
    QueFlush 1


    '*** BUGBUG this is a gross on localizable hack this is bad
    '*** BUGBUG this must go away for localization
    '*** BUGBUG this is to work around the 2 confirm popup sillness when
    '*** BUGBUG we break mirrors
'
'    IF NOT WFndWndWait(winCapConfirm$,FW_PART,5) = 0 THEN
'      QueKeys "{ENTER}"             '*** YES
'      QuePause 1000
'      QueFlush 1
'    END IF
'

    IF NOT WFndWndWait(title$,FW_PART,5) = 0 THEN
      IF WButtonExists(butCapOk$) THEN
        QueKeys "{ENTER}"                   '*** Disks Updated succesfully
        QuePause 1000
        QueFlush 1

        '
        ' If we are in the AlwaysShutDown mode, shut the system down
        '

        IF gAlwaysShutDown THEN
          SHELL "echo shutting down > shutdown.sig"
          IF gForceShutDown THEN
            SHELL "shutdown -f -r -t 0"
          ELSE
            SHELL "shutdown -r -t 0"
          END IF
        END IF

        '
        ' Autodetect the "you must shutdown" dialog
        '

        IF NOT WFndWndWait(title$,FW_PART,5) = 0 THEN
          IF WButtonExists(butCapOk$) THEN

            '
            ' We must shutdown, do a forced shutdown if requested, else
            ' shutdown via windisk
            '

            SHELL "echo shutting down > shutdown.sig"
            IF gForceShutDown THEN
              SHELL "shutdown -f -r -t 0"
            ELSE
              QuePause 1000
              QueKeys "{ENTER}"             '***
              QuePause 1000                 '*** shutdown dialog found
              QueFlush 1                    '***
            END IF
          END IF
        END IF

      END IF
    END IF
    QuePause 1000
    QueFlush 1

END SUB



'***-----------------------------------------------------------------------
'***
'*** This Routine will take a string of the form "{string1}%{string2}%{string}"
'*** and another string to subistute for the %{string2}% substring.
'*** 
'*** This is used to paste together an expected title string.
'***
'***-----------------------------------------------------------------------

STATIC FUNCTION StrPrint$(src$,subsitute$)
  DIM strLen%,i%
  DIM frontStr$, endStr$

  frontStr=""
  endStr=""
  strLen=LEN(src$)
  i=1 
  WHILE i <= strLen 
    '***
    '*** Find the Marker to subsitute for
    '***
    IF MID$(src$,i,1)="%" THEN
      frontStr=MID$(src,1,i-1)
      i=i+1
      WHILE i <= strLen
        '***
        '*** Find the end of the marker
        '***
        IF MID$(src$,i,1)="%" THEN
          endStr=MID$(src$,i+1,strLen-i)
          i=strLen     '*** exit the loops
        END IF
        i=i+1
      WEND
    END IF 
    i=i+1
  WEND

  StrPrint$=frontStr+subsitute$+endStr

END FUNCTION

