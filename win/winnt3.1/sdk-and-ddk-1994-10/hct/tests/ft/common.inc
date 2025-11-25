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

CONST MIRROR$="mirror"
CONST CPBT_MIRROR$="cpbtmir"
CONST BROKEN_MIRROR$="broken_mirror"
CONST STRWP$="strwp"
CONST STRWOP$="strwop"
CONST VOLUME$="volume"
CONST PARTITION$="partition"

CONST LOG_SCREEN=1
CONST LOG_DISK=2

GLOBAL disk%(8)     '*** the regions selected
GLOBAL pos%(8)
GLOBAL toExtend%(8)
GLOBAL MAXSELECT%
GLOBAL gDisk%,gPos% '*** (gDisk,gSpacePos) holds the currently selected region

GLOBAL gLogFileHandle%
GLOBAL gLogFileOptions%

DECLARE SUB WndWait(title$)
DECLARE SUB AvoidLeadingPopups()
DECLARE SUB AvoidSpecificPopups(title$)
DECLARE SUB WaitForShutdown()

DECLARE SUB OpenLog(logFile$,options%)
DECLARE SUB CloseLog()
DECLARE SUB WriteLog(aLine$)
DECLARE SUB LogBanner(aLine$)

DECLARE SUB SelectRegion(disk%,pos%)
DECLARE SUB GetConfig(fileName$,ftType$,fileSystem$,szMB$,driveLet$,numRegions%)
DECLARE SUB CreateNewConfig(oldFileName$,newFileName$,ftType$)


'*** Let us initialize
gDisk=0
gPos=0
MAXSELECT=8

STATIC SUB SelectRegion(disk%,pos%)
 '***(disk%,pos%) the distination of pointer

  WriteLog STR$(disk)+" "+STR$(pos)

  WHILE gDisk > disk     '*** If the current location is to great move up
    gDisk=gDisk-1
    gPos=0
    DoKeys "{UP}"
  WEND

  WHILE gDisk < disk     '*** If the current location is to low move down
    gDisk=gDisk+1
    gPos=0
    DoKeys "{DOWN}"
  WEND

  WHILE gPos > pos       '*** If the current location is too right mv left
    gPos=gPos-1
    DoKeys "{LEFT}"
  WEND

  WHILE gPos < pos       '*** If the current location is too left mv right
    gPos=gPos+1
    DoKeys "{RIGHT}"
  WEND

  QuePause 1000
  QueKeys "^({ENTER})"   '*** select the next partition
  QueFlush 1
END SUB




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

  PRINT driveLet

  IF numRegions > MAXSELECT THEN
    PRINT "The number of regions exceeds the maxium allowed"
    END
  END IF

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


'*** Creates a New config file with a differrent ftType
'*** same file format (for broken_mirror)

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

  IF numRegions > MAXSELECT THEN
    PAUSE "The number of regions exceeds the maxium allowed"
    END
  END IF

  i=0
  WHILE i < numRegions        '*** write the regions
    PRINT #outfile, disk(i)
    PRINT #outfile, pos%(i)
    PRINT #outfile, toExtend%(i)
    i=i+1
  WEND

  CLOSE #outfile

END SUB

'*** These are debugging logging routines


'*** This routine opens the log file to a disk or the viewport
STATIC SUB OpenLog(logFile$,options%)
  gLogFileOptions=options

  IF options AND LOG_DISK THEN
    gLogFileHandle=FREEFILE
    OPEN logFile FOR OUTPUT AS #gLogFileHandle
  ELSE
    gLogFileHandle=0
  END IF
END SUB

'*** This routine closes the log file to a disk or the viewport
STATIC SUB CloseLog()
  IF gLogFileOptions AND LOG_DISK THEN
    CLOSE #gLogFileHandle
  END IF
END SUB

'*** This routine sends the logging data to the opened loging devices
STATIC SUB WriteLog(aLine$)
  IF gLogFileOptions AND LOG_SCREEN THEN
    PRINT aLine$
  END IF

  IF gLogFileOptions AND LOG_DISK THEN
    PRINT #gLogFileHandle, aLine$
  END IF
END SUB

'*** this routine boaders the line with asterixs and cernters the line
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

'*** This routine waits 16 seconds or until the expected window pops up
'*** if after the expected time the window has not appered send an error
'*** popup and reset the do.cmd script to donothin.0
'***
'*** I added a 2nd sleep and 2nd check for the first check does not seem
'*** to wait for the full time limit.  (I tried increasing the number without
'*** success.)  So I am forcing a pause if the first check fails and then
'*** rechecking
STATIC SUB WndWait(title$)
  SLEEP(1)
  IF WFndWndWait(title$,FW_PART,15) = 0 THEN
    SLEEP(2)                                          '*** This is a hack
    IF WFndWndWait(title$,FW_PART,120) = 0 THEN       '*** to work around
      SHELL "copy donothin.0 donext"                  '*** aysnc behavior
      PAUSE "Window '"+title$+"' could not be found!" '***
      CloseLog
      END
    END IF
  END IF
END SUB


'*** This routine is a hack to avoid the System Process popups for orphans.

STATIC SUB AvoidSpecificPopups(title$)
  SLEEP(1)
  IF NOT WFndWndWait(title$,FW_PART,5) = 0 THEN
    IF WButtonExists("OK") THEN
      WButtonClick "OK"
    ELSE
      '*** this is an error
      PAUSE "Window "+title$+" in unexpected format"
      END
    END IF
  ELSE                        '*** give it a second chance
    SLEEP(2)
    IF NOT WFndWndWait(title$,FW_PART,5) = 0 THEN
      IF WButtonExists("OK") THEN
        WButtonClick "OK"
      ELSE
        '*** this is an error
        PAUSE "Window "+title$+" in unexpected format"
        END
      END IF
    ELSE
      PAUSE "Popup not found.  Adjust orphan factor."
      END
    END IF
  END IF
END SUB


'*** This little routine gets rid of the nasty popups that appear either as
'*** a result of a change of disk configuration or because the diskman
'*** has never been run.
'*** This little routine assumes the popups have "OK" buttons and the
'*** actual diskman.exe does not.  It just click the OK button until no more
'*** remain.
STATIC SUB AvoidLeadingPopups()
  count%=10
  WHILE WButtonExists("OK")  AND count% > 0   '** Get rid of any leading popups
    WButtonClick "OK"
    SLEEP(1)
    count=count-1
  WEND

  IF count%=0 THEN
    SHELL "copy donothin.0 donext"
    PAUSE "Unexpected popup loop.  This should never happen."
    END
  END IF
END SUB


'*** wait for the reboot since the normal exit of testdrvr before reboot
'*** will run the next do script prematurely.

STATIC SUB WaitForShutdown()
  SLEEP 30
' PAUSE "Shutdown Failed!  Forcing a shutdown"
  SHELL "shutdown -f -r -t 0"
END SUB

