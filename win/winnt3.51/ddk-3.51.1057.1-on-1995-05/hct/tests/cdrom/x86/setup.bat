@echo
@echo Insert the Windows NT CD Boot Disk in drive A:
@echo Have a blank, formatted floppy of the same size to swap.
@echo
@echo This script will do a diskcopy using drive A:
@echo from the Boot Disk to the blank disk.
@echo
@echo press CTRL-C to cancel
pause

diskcopy a: a:

@echo Ensure that the new copy disk is in drive A:
pause

copy setupapp.exe a:\
copy arctest.inf a:\
