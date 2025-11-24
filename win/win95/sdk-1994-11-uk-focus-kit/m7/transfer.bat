@echo off
echo *************************************************************************
echo *                                                                       *
echo *                  Windows 95 1.44Mb floppy disks                       *
echo *                                                                       *
echo *              THESE DISKS ARE MICROSOFT CONFIDENTIAL AND               *
echo *              ARE SUBJECT TO THE NON-DISCLOSURE AGREEMENT              *
echo *            AND OTHER TERMS AND RESTRICTIONS OF THE WINDOWS 95         *
echo *                          BETA PROGRAM                                 *
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
if NOT "%1"=="" goto itsok
..\utils\choice /c:abx "Drive a, b or exit"
if ERRORLEVEL 3 goto EXIT
if ERRORLEVEL 2 goto BDRIVE

call trans a:
goto EXIT

:BDRIVE
call trans b:
goto EXIT

:itsok
call trans %1

:EXIT
