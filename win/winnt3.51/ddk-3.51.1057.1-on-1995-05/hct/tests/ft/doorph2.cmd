@rem -------------------------------------------------------------------------
@rem
@rem   doorph2
@rem   -------
@rem
@rem   Script that is started asynchronously from the main test engine.
@rem   It will wait for some time and orphan a randomly selected member
@rem   of the current test partition.
@rem
@rem   All log output will be collected in orphan.log.
@rem
@rem   The caller should gather orphan.log into the main logfile after
@rem   this script has had time to complete.
@rem
@rem   Also, as a side effect of ftstate -o, orphaned.cmd will be
@rem   generated to indicate which member was orphaned.  See ftstate.c
@rem   for more details.
@rem
@rem   usage: doorph2 drive_letter size_partition time_factor
@rem
@rem -------------------------------------------------------------------------

del orphover.sig

@rem
@rem Make sure the members bound to the drive letter are healthy before
@rem doing the orphan.  Re-check the state of the component every 10 sec.
@rem
@rem Note that if the component never becomes healthy, then the main
@rem thread will stop and fail the tests for the same reason.
@rem

echo.                                                       >  orphan.log
echo TRACE: Make sure members all healthy before orphaning. >> orphan.log
echo.                                                       >> orphan.log

:recheck
echotime /t "doorph2.cmd: waiting for member to become healthy " >> orphan.log
ftstate.exe %1: -t:10 >> orphan.log
if errorlevel 1 goto recheck

:complete_wait_for_healthy

echotime /t "doorph2.cmd: Component is now healthy " >> orphan.log

@rem
@rem Run the orphan.exe which will select a member and orphan it.
@rem Go through some contortions to cat the orphan.log and still
@rem be able to check the errorlevel.
@rem

echo.                                      >> orphan.log
echo BEGIN_TEST: Start of doorph2 %1 %2 %3 >> orphan.log
echo.                                      >> orphan.log

set orphanexe_failed=

@rem
@rem Ok, now orphan
@rem

genbsec -o -d:%1 -t:%2 -f:%3 -c -l > orphan2.log

if errorlevel 1 set orphanexe_failed=1
type orphan2.log
type orphan2.log >> orphan.log
del  orphan2.log

echotime /t "doorph2.cmd: Orphaning has ocurred " >> orphan.log

if not "%orphanexe_failed%" == "" goto fail_orphan
echo VERIFY PASS: orphan.exe returned no errors >> orphan.log
goto end_test1

:fail_orphan
echo VERIFY FAIL: orphan.exe returned a failure >> orphan.log
goto end_test1

:end_test1
echo.                                  >> orphan.log
echo END_TEST: End of doorph2 %1 %2 %3 >> orphan.log
echo.                                  >> orphan.log

@rem
@rem Since swp (and now mirrors apparantly) does not like to orphan
@rem until a write ocurrs, perform a write.
@rem
@rem Important Note: the file "foo" already exists, it was created by
@rem doformat.cmd.  We are not changing its size, so that this write
@rem operation does not interfere with the fsetmap that is already
@rem running (i.e. it computed the amount of free space up front, we
@rem cannot compete with it for disk space, it will fail if we do).
@rem
@rem Equally as important is the contents.  We are writing the same
@rem data as is done in doformat.cmd.  This is necessary for partcomp
@rem to function properly.  There, we assume that it is valid to partcomp
@rem unless orphan_writes.  If we did not echo the same contents to an
@rem already existing file here, that assumption is broken.
@rem

echo hello > %driveLetter1%:\foo

@rem
@rem Give it a few seconds, and clear up that popup which should be there
@rem

sleep 5

echo.                                                   >> orphan.log
echo BEGIN_TEST: Clearing off the expected Orphan popup >> orphan.log
echo.                                                   >> orphan.log

mtrun orphan.pcd /h

echo.                                                 >> orphan.log
echo END_TEST: Clearing off the expected Orphan popup >> orphan.log
echo.                                                 >> orphan.log

@rem
@rem Create the "orphover.sig" file.  This tells the main thread
@rem that we finished.  If it does not find this file, it knows
@rem something is really wrong and will not allow the system to
@rem shutdown.
@rem

echo done > orphover.sig
