echo off
echo #		     Creating Debugging version of 1.03 Windows
echo #
echo # This batch file creates the debugging version of Windows
echo # by deleting and overwriting some of the files on the Windows 
echo # 1.03 setup disks.
echo # Before proceeding, backups of the Windows 1.03 setup disks
echo # must be made so that the non-debugging version of Windows can
echo # be restored for final testing of Windows applications.
echo #
echo # If backups of the Windows Setup disks have not been made yet,
echo # abort this batch file now.
echo #
pause

echo #
echo # This batch file will copy files from the Software Development Kit
echo # disks onto the Windows 1.03 setup disks. The following disks
echo # will be needed:
echo #	  Windows 1.03 setup disks
echo #	      Setup (Disk 1 of 5)
echo #	      Build (Disk 2 of 5)
echo #	  Software Development kit disks
echo #	      Debugging EXEs and SYM files (Disk 5 of 7)
echo #
pause

:notsetup
echo #
echo # Insert Debugging EXEs and SYM files disk (Disk 5 of 7) in drive A:
echo # Insert Setup disk (Disk 1 of 5) in drive B:
echo #
pause
if exist b:\kernel.exe goto setup
echo #
echo # ERROR Please check that you have the correct disk in drive B:
echo #
goto notsetup
:setup
del b:\ramdrive.sys
copy a:\debug\kernel.exe b:\

:notbuild
echo #
echo # Insert Build disk (Disk 2 of 5) in drive B:
echo #
pause
if exist b:\gdi.exe goto build
echo #
echo # ERROR Please check that you have the correct disk in drive B:
echo #
goto notbuild
:build
copy a:\debug\gdi.exe	 b:\
copy a:\debug\user.exe	 b:\

echo #
echo # Now run Setup from these modified disks to install the
echo # debugging version of Windows 1.03.
echo #
echo # After installation, copy a:\debug\*.sym
echo # to the directory that the 1.03 debugging
echo # version of Windows was installed in.
echo #
echo #	 To restore the non-debugging version of
echo #	 Windows, run Setup from the unmodified disks.
echo #	 After installation, copy a:\nodebug\*.sym
echo #	 to the directory that the 1.03 version of
echo #	 Windows was installed in.
