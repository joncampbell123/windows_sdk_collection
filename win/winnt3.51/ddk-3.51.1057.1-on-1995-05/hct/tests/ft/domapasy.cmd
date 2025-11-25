@rem -------------------------------------------------------------------------
@rem 
@rem   domapasy.cmd
@rem   ------------
@rem   Script to run fsetmap.exe asynchronously.  This is used by the swp
@rem   regeneration tests to perform reads and writes while the swp is
@rem   regenerating.
@rem
@rem   Usage:
@rem   ------
@rem    <filename> <percent_to_fill> <logfile>
@rem
@rem   Note that the percentage is computed up front, so doing this
@rem   from 2 processes at the same time will not work.
@rem
@rem -------------------------------------------------------------------------

@rem
@rem Give it realtime scheduling priority so that it completes by the
@rem time chkdsk is run.
@rem

@rem
@rem Make sure the signal is removed.
@rem
del xymapasy.sig

echo fsetmap -f%1 -u%2 -p117 ^>^> %3 2^>^&1  >  xymapasy.cmd 
echo endtomd %1 -p117  ^>^> %3 2^>^&1        >> xymapasy.cmd

@rem 
@rem This is so we wait before deleting this guy so we don't get orphan 
@rem popup and the write/read completes
@rem

sleep 5

echo echo This is a signal   ^> xymapasy.sig >> xymapasy.cmd
echo exit                                    >> xymapasy.cmd

start /realtime xymapasy.cmd

