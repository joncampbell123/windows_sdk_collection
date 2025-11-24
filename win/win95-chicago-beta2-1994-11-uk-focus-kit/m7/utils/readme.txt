Windows NT CHKDSK & LABEL Utilities

Running the shipped version of Windows NT's CHKDSK utility 
on a Chicago volume will destroy all the long file names on 
the FAT volume.  Running the shipped version of Windows NT's 
LABEL utility on a Chicago volume could potentially destroy 
the first long file name in the root if it is before the 
Label for the volume.

If you are multi-booting Windows NT and Chicago, please copy 
the following three files to your  <NT_Dir>\system32 directory.  
This is usually c:\winnt\system32.  The three files are:

	UFAT.DLL
	AUTOCHK.EXE
	FASTFAT.SYS

These files can be found in the \UTILS directory on the 
Chicago Pre-Release CD-ROM. At this point you will have a 
new version of Windows NT's CHKDSK and NT's LABEL command
that will not destroy the long file names on the FAT volume. 

The NT convert program in NT 3.1 will still destroy the long 
file names on the FAT volume while converting the files to the
NTFS volume.