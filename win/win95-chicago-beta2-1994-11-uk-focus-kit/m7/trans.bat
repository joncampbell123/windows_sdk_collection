@echo off
if NOT "%1"=="" goto itsok
echo *************************************************************************
echo *                                                                       *
echo *                  Windows 95 1.44Mb floppy disks                       *
echo *                                                                       *
echo *************************************************************************
echo *                                                                       *
echo *  Requires twenty (20) 1.44Mb floppy diskettes                         *
echo *                                                                       *
echo *  USAGE:  TRANSFER drive-spec:                                         *
echo *  WHERE:  drive-spec: is your 1.44Mb floppy drive                      *
echo *     IE:  TRANSFER b:                                                  *
echo *                                                                       *
echo *  Creates 1.44Mb floppy disks                                          *
echo *                                                                       *
echo *************************************************************************
goto egress
:itsok
echo 
echo Transferring Windows 95 to drive %1
..\utils\choice /c:ca "Continue or Abort"
if ErrorLevel==2 goto EGRESS


:DISK1
cls
echo Windows 95 Disk #1: Insert blank formatted 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK2
copy dossetup.bin       %1
copy winsetup.bin       %1
copy mini.cab           %1
copy precopy1.cab       %1
copy deltemp.com        %1
copy extract.exe        %1
copy scandisk.exe       %1
copy setup.exe          %1
copy smartdrv.exe       %1
copy xmsmmgr.exe        %1
copy readme.txt         %1

:DISK2
echo 
echo Windows 95 Disk #2: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK3
copy PRECOPY2.CAB   %1
copy win95_02.CAB     %1

:DISK3
echo 
echo Windows 95 Disk #3: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK4
copy win95_03.CAB     %1

:DISK4
echo 
echo Windows 95 Disk #4: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK5
copy win95_04.CAB     %1

:DISK5
echo 
echo Windows 95 Disk #5: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK6
copy win95_05.CAB     %1

:DISK6
echo 
echo Windows 95 Disk #6: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK7
copy win95_06.CAB     %1

:DISK7
echo 
echo Windows 95 Disk #7: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK8
copy win95_07.CAB     %1

:DISK8
echo 
echo Windows 95 Disk #8: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK9
copy win95_08.CAB     %1

:DISK9
echo 
echo Windows 95 Disk #9: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK10
copy win95_09.CAB    %1

:DISK10
echo 
echo Windows 95 Disk #10: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK11
copy win95_10.CAB   %1

:DISK11
echo 
echo Windows 95 Disk #11: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK12
copy win95_11.CAB   %1

:DISK12
echo 
echo Windows 95 Disk #12: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK13
copy win95_12.CAB   %1

:DISK13
echo 
echo Windows 95 Disk #13: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK14
copy win95_13.CAB   %1

:DISK14
echo 
echo Windows 95 Disk #14: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK15
copy win95_14.CAB   %1

:DISK15
echo 
echo Windows 95 Disk #15: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK16
copy win95_15.CAB   %1

:DISK16
echo 
echo Windows 95 Disk #16: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK17
copy win95_16.CAB   %1
           
:DISK17
echo 
echo Windows 95 Disk #17: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK18
copy win95_17.CAB   %1

:DISK18
echo 
echo Windows 95 Disk #18: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK19
copy win95_18.CAB   %1

:DISK19
echo 
echo Windows 95 Disk #19: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DISK20
copy win95_19.CAB   %1

:DISK20
echo 
echo Windows 95 Disk #20: Insert 1.44M disk in %1
..\utils\choice /c:msx "Make, Skip, eXit"
if ERRORLEVEL 3 goto EGRESS
if ERRORLEVEL 2 goto DONE
copy win95_20.CAB   %1

:DONE
echo All done!
pause
:EGRESS
echo on
