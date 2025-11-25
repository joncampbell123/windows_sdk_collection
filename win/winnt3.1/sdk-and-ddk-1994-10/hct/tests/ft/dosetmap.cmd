@rem -------------------------------------------------------------------------
@rem 
@rem   dosetmap.cmd
@rem   ------------
@rem   Script to run fsetmap.exe and verify whether it worked or not.
@rem
@rem   Usage:
@rem   ------
@rem   dosetmap <filename> <percent_to_fill> <logfile>
@rem
@rem   Note that the percentage is computed up front, so doing this
@rem   from 2 processes at the same time will not work.
@rem
@rem -------------------------------------------------------------------------

echo.                                            >> %3
echo BEGIN_TEST: Start of fsetmap -f%1 -u%2 test >> %3
echo.                                            >> %3

fsetmap -f%1 -u%2 >> %3

if errorlevel 1 goto fail_fsetmap
echo VERIFY PASS: fsetmap successfully wrote to test drive >> %3
goto done

:fail_fsetmap
echo VERIFY FAIL: fsetmap failed >> %3
if "%nopause%" == "" pause
goto done

:done
echo.                                        >> %3
echo END_TEST: End of fsetmap -f%1 -u%2 test >> %3
echo.                                        >> %3
