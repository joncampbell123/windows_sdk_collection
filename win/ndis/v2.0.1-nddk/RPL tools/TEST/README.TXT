TESTING THE RPL PROM
--------------------

The files included will help you set up a RPLed wksta (DOS or OS/2) for
mulitple reboot.  The following files are included:

                     1) startup.cmd
                     2) autoexec.bat
                     3) autoadd.bat
                     4) getremtm.exe (setrmtod.cmd/bat)
                     5) rbtos2.sys
                     6) rbtos2.exe
                     7) rbtdos.exe


STARTUP.CMD

This startup.cmd file can be used to automatically reboot a RPLed wksta
and log its boot time.  Set the environment variables to the correct
names for the machine running the RPL service, the machine providing
the RPLed wksta file shares, and the wksta's computername.  Usually the
RPLSERVER and the FILESERVER will be the same, but this is not required.



AUTOEXEC.BAT

This is the standard autoexec.bat used with the makeimg.exe program, except
an additional line has been added at the end to call a secondary batch file,
autoadd.bat, for testing purposes.



AUTOADD.BAT

This secondary batch file makes a redirected drive use to a share on the server
containing test tools, then calls those tools to log the boot time and reboot
the wksta.


GETREMTM.EXE

This is a bound executable (works on both DOS and OS/2) that gets the current
time and date from a server running the TIMESOURCE service.  The time and date
are stored in the batch file SETRMTOD.CMD/BAT as environment variables REMTIME
REMDATE.



SETRMTOD.CMD/BAT

This batch file is created by GETREMTM.EXE.  Call this batch file from
STARTUP.CMD or AUTOEXEC.BAT to set the environment variables.



RBTOS2.SYS/EXE

Include the line DEVICE=RBTOS2.SYS in the OS/2 wksta's CONFIG.SYS to enable
it to reboot from the command line.  In your batch file, simply execute
RBTOS2.EXE to reboot the wksta.


RBTDOS.EXE

In your batch file, simply execute RBTDOS.EXE to reboot your DOS wksta.
No corresponding .SYS file is required.
