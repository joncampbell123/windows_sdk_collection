NDISTEST -- THE NEW NETCARD TEST TOOL


INTRODUCTION

This document gives a brief description of the new netcard test tool.
A new test tool was considered necessary because of the difficulty
many people had using the old tool, which was known as tpctl.  Many
of these problems were related to getting the environment all set up
to run that test tool.  The use of "golden logs" made it very tedious
to make even minor fixes to the old tester and its scripts.


INSTALLATION

The ndistest tool is installed from the HCT or DDK CD-ROM.  Basically
all that is necessary is to run ndsetup.bat from the "newndis" directory.
The only argument necessary is the directory in which you want to place
the test tool.  For example, "ndsetup d:\ndis" will install ndistest in
directory d:\ndis.  The last step of this installation requires you to
have administrator privileges, as it has to set some entries in the
registry.  If this step fails, logoff, log back on as an administrator,
and run "regini ndistest.ini" from the directory in which you installed
ndistest.  The driver portion of ndistest will now be loaded when the
system starts.  Note that you can leave the old tester (tpctl) installed.
Ndistest and tpctl can co-exist, although you should not run both at
the same time.


FILES

This is a brief description of the files that will be copied to your
system during the installation process.  The new tester uses HAPI, which
is basically a script interpreter used in much of our internal testing.

    files in installation directory

    hapi.exe     -- program that starts up the tester.
    hapisum.exe  -- program that scans all the test log files and produces
                    the test summary, which will be put in file hapi.sum
    regini.exe   -- program used to initialize the registry information
                    during installation
    h_core.dll   -- DLL that provides base functionality for the tester.
                    Basically, it's a general purpose script interpreter.
    ndistest.dll -- DLL that supports the special functionality for the
                    netcard testing.  It communicates to NDIS and the
                    netcard through ndistest.sys.
    ndistest.sys -- driver that is installed in directory
                    %SystemRoot%\system32\drivers.  It takes commands from
                    ndistest.dll and sends them to the NDIS wrapper and/or
                    to the netcard itself.
    ndistest.ini -- contains information written to registry during
                    installation
    hapi.ini     -- contains information needed by HAPI on startup.  More
                    specifically, it lists the special purpose DLLs that
                    it will need.
    ndsetup.bat  -- used to install the new tester
    ndtest.bat   -- used to run HCT tests for netcard
    ndstress.bat -- used to run just the stress tests
    ndfunc.bat   -- used to run just the functional tests
    server.bat   -- used to start server on a second machine, which will
                    participate in the various tests
    repro.bat    -- used to help reproduce problems

    files in scripts subdirectory

    ndis.tst     -- the master script file, called by ndtest.bat
    stress.tst   -- script file called by ndstress.bat, which performs just
                    the stress tests
    funct.tst    -- script file called by ndfunc.bat, which performs just
                    the functional tests
    repro.tst    -- script file called by repro.bat.  It is the same as
                    ndis.tst except that "verbose" debugger info is turned
                    on, and all tests are commented out.
    server.tst   -- script file called by server.bat, which runs the server
                    in a multiple machine test
    1m1c*.tst    -- 1 machine, 1 card, 1 open functional test scripts
    1m2o*.tst    -- 1 machine, 1 card, 2 open functional test scripts
    1m2c*.tst    -- 1 machine, 2 card functional test scripts
    2m2c*.tst    -- 2 machine, 2 card functional test scripts
    str_*.tst    -- stress test scripts


RUNNING THE TESTS

Before you run the tests, make sure all bindings are removed from the
test card and the trusted card on the main test machine, as well as from
the server card on the server machine (if used).  The cards involved in
the test should be connected on a private network.  Also, the test machine
should be set up for debugging, with the appropriate serial connection to
a debugger machine.  If there are problems during a test, the debugger will
usually have some useful information to help you track it down.  After
everything is set up, reboot the machine if necessary.

Make sure the appropriate netcard drivers are loaded.  Use control panel,
devices to load if necessary.  Or, you can use "net start".

If you will be running two machine tests, you need to start the server.
On the server machine, open a command window, CD to the directory where
the ndis tester is installed, and run "server <servercardname>".  If
unsure of the name of the server card, use winmsd or look in the registry.
Make sure you include the instance number.  Ie, use "server lance2", not
"server lance".

On the test machine, open a command window, CD to the proper directory,
and run "ndtest <testcardname> [trustedcardname]".  The card names can
be obtained from winmsd or the registry as before.  The first argument,
<testcardname>, is required.  This is the card you are testing.  The
second argument, [trustedcardname], is optional.  If used, this card will
be used in the 1 machine, 2 card tests.  If the second argument is omitted
OR the specified trusted card cannot be opened, the 1 machine, 2 card
tests will be omitted.  The 2 machine tests will be done if the test card
is able to contact the server.  If not, those tests will be omitted.

"ndfunc" and "ndstress" are run the same as ndtest.  "repro" is similar,
except that you must first enable the desired test(s) by un-commenting
it/them in file scripts\repro.tst.  '#' is the script file comment
character.  Just remove the '#' before the appropriate tests, then run
"repro".


INTERPRETING TEST RESULTS

The last step of the batch files produces a summary of the test results
and writes them to the command window and to file hapi.sum.  There is no
need to look at any of the log files unless the summary indicates errors,
warnings, or blocked tests.  Then just edit that log file with your
favorite editor, searching for the following strings:

    "FAILURE" if you had a failed variation
    "WARNING" if you had a warned variation
    "BLOCKED" if you had a blocked variation

The information on that line should help you find what didn't work.
Sometimes you may have to look up a few lines to find exactly what test
was being run.  Log entries identifying new sets of tests start with
"***".  The top of a loop, where the packet filters for a set of tests
are set, contains multiple "LOOPTOP"s.  Often the debugger may also
contain useful information.

Note that there are breakpoints in the driver that you might hit under
certain situations.  Hopefully, these are all proceeded by a string sent
to the debugger identifying the problem.  Most of these are related to
packet data corruption.  The breakpoints are provided since you might
find it useful to discover exactly what/where data was corrupted.


NEW TESTER VS OLD TESTER

Advantages of new tester:
   Less disk space
   Easier set up
   Easier to use
   No golden logs or log comparisons
   Graphical interface
   Scripts language is C-like (easy to write)
   Better test coverage

Disadvantages:
   No interactive mode (may make repros more difficult)
   Entire test suite takes longer to run



KNOWN PROBLEMS

These will be fixed by final release:

   Doesn't work yet with Arcnet cards
   Minor problems with Windows interface
       (scroll bars don't work, buffer can fill up)
   Performance tests not yet included in tests
   Miniports will currently return the full global OID_GEN_SUPPORTED_LIST
       for both a global and a transport query.  This will result in a
       number of failures being recorded in 1m1c_02.log and 1m2o_01.log.
   Token ring drivers that use the Token Ring filter library will receive
       packets sent to the global address even when they do not have the
       global filter active.  This will result in a number of failures
       being recorded in 1m1c_10c.log, 1m2o_09c.log, 1m2c_03c.log, and
       2m2c_03c.log.


MISCELLANEOUS NOTES

The final release (3.51 HCT) will contain information on writing your
own scripts.  Some of the command formats may change before that time.

"Version" information for the driver is written to the debugger on
startup.  For the DLL, it is written to ndis.log.

Please send questions, comments, error reports, and suggestions to
ndiststr@microsoft.com.  If an immediate response is required, please
include your phone number.



