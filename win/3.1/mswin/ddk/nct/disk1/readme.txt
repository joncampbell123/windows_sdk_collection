ADDITIONAL INFORMATION ABOUT THE NCT FOR WINDOWS 3.1:


1.   To install the Windows 3.1 NCT on your hardware, you will need
     approximately 2.8Mb of free space on your hard disk.  Please ensure
     that you have enough free disk space before installing the NCT.

     You may need an additional 2Mb of free space on your hard disk to
     run all the tests.  Many of the tests create/copy temporary files
     to your NCT directory as well as create log files.  You should also
     ensure you have adequate free disk space on your network drive that
     you will use while running the NCT.


2.   To install the Windows NCT, perform the following:

	a) Create a directory for the NCT on your hard drive.
	b) Insert the NCT disk in the floppy drive.
	c) Run the INSTALL batch file from the floppy drive (without any
	   parameters).

	   The batch file will display the parameters needed to correctly
	   install the NCT files.
	

3.   NCT Disk #2 contains the NCT documentation. You have a choice of
     installing the NCT documentation onto your test machine during 
     the NCT installation.  If you do not install the NCT documentation, 
     you can view/print the documentation at any time by copying 
     the files from NCT Disk #2 (files are not compressed) onto any
     machine capable of opening the documents.
     
     The NCT documentation is NCT.DOC (Word for Windows v1.1 format)


4.   If you are experiencing problems running certain tests which launch
     multiple DOS VM's (enhanced mode), try adding the following
     to the [386Enh] section of your SYSTEM.INI:

	TimerCriticalSection=1000


5.   NCT tests will attempt to disconnect any printers or network
     drives that are connected if they match those devices specified in 
     WINPLAY.INI.  Therefore, if you begin a test with network
     drives J, K, & L already redirected, and you had specified drives
     J, K, & L in WINPLAY.INI to be used by the NCT, the tests will 
     disconnect drives J, K, & L and reconnect the drives to their
     appropriate shares/volumes as specified in WINPLAY.INI.

     The NCT tests will also attempt to delete directories/files that
     are remaining from a previous test (a test that may have failed
     or failed to delete it's files).


6.   Most of the NCT tests run in an automated fashion.  There will
     no dialogs to signify the beginning or ending of a specific test.

     The tests are ordered so that automated tests are run before
     tests which require user interaction.  For the enhanced mode
     tests, the File Manager tests thru the Int13 test require no
     user interaction.  The remaining few tests will require
     user interaction.  
     
     If you press the [GO] button without any tests listed in the 
     "Tests to be Run" listbox, the NCT will run all tests that have 
     not been run (beginning from the top of the "List of All Tests" 
     listbox).


7.   Several of the tests will display a top most dialog box in the 
     upper right hand corner, with [Pause] and [Abort] buttons.  These 
     controls allow you to pause/resume or abort the test that is running.
 
     It may take up to a minute before the test will actually abort.
     Note, however, that if you abort a test, applications may be left
     open or running (such as MS-DOS VM's).  It may be necessary to close 
     these windows manually.  


8.   These test scripts make use of a new test tool which compiles each
     test into compact pseudo-code prior to execution.  This will cause
     a brief delay when you begin each test script.   This test language
     is the predecessor to MS-TEST, our new Windows Testing language.


9.  If you are running the NCT with Novell NetWare:

     a) You must run the tests with NWShareHandles disabled.
	You can configure this parameter in the Networks section 
	of Control Panel.  Please refer to the online Help in
	the Networks section of Control Panel for more information.

     b) You will have to add the following entry to the [386enh]
	section of SYSTEM.INI:

		TimerCriticalSection=1000

	to run the NetBios VM Test.
	

10.  If you are running the NCT with Banyan Vines v4.1x:

     a)  If you use the Vines.drv driver, user interaction is required
	 when running the WinNet API Test.  The "Connection Root"
	 dialog box is displayed for each WNetAddConnection API call
	 made during the test.  This requires you to select [OK] in
	 order for the test to continue.


11.  If you are running the NCT WinNet API Test on 3Com's 3+Share,
     3Com's 3+Open, or Artisoft's Lantastic, you must set the
     DOS environment variable "INCLUDE" to the directory where
     you installed the NCT files.

     Example:  If you installed the NCT files in c:\testapps\nct
	       then type the following at the MS-DOS Prompt prior
	       to starting Windows:

	       SET INCLUDE = c:\testapps\nct


12.  The MakeBatchFile variable in WINPLAY.INI is not discussed in the NCT
     documentation.

     The default value for MakeBatchFile is N (No).  Several of the tests 
     create batch files and PIF files (based on WINPLAY.INI variables) 
     as the tests are run (they are saved in the NCT directory).  This 
     variable, when set to No, allows you to bypass the creation of the 
     batch files and PIF files for tests that have already been run and 
     it uses the files that were previously created.  This allows you to 
     rerun a test without recreating these files.  
     
     If you are running a test for the first time the batch files 
     and PIF files will be created regardless of this switch setting.

     Feel free to modify any of the batch files that several of the tests
     creates.  Using the MakeBatchFile=N option in WINPLAY.INI, the tests
     will not create new batch files, but will run the modified batch files.

     If you change variable settings in WINPLAY.INI, you must change
     MakeBatchFile to Y (yes) so tests can create new batch files with
     these new WINPLAY.INI variables.  Once batch files have been created
     with the new WINPLAY.INI settings, you can switch MakeBatchFile back
     to N.


13.  Examples of WINPLAY.INI for particular networks are provided on
     NCT Disk #2 (text files).  

	a) WINPLAY.NW   - WINPLAY.INI for Novell NetWare.
	b) WINPLAY.LM   - WINPLAY.INI for LanManger/MS-NET based Networks
	c) WINPLAY.BV   - WINPLAY.INI for Banyan Vines
	

14.  The DOS Multi-VM Test and Network Stress Test can be configured to
     run longer durations if desired.

     a)  DOS Multi-VM Test:  you can change the length of time that the test
	 will run by changing a loop variable in the file:  DOSVM.WTD
	 (in your NCT directory).  Using your favorite text editor, 
	 open DOSVM.WTD and modify line #827, which appears as:
	 
	    for i% = 1 to 4      'increase this loop for longer test times!
	    ret%=SleepDelay(59)  'wait 1 minute i times 
	    next

	 By increasing the FOR loop from:

		 for i% = 1 to 4
	 to:
		 for i% = 1 to 40

	 will increase the duration of the test by approximately 10 times.

	 The test duration is highly dependent on the CPU type and the
	 network.  It is recommended that you run the test with the
	 default loop counter and check the log files to see how long
	 the test takes on your particular configuration.


     b)  Network Stress Test:  you can change the length of time that the 
	 test will run by changing a loop variable in the file:  STRSSLP.INC
	 (in your NCT directory).  Using your favorite text editor, 
	 open STRSSLP.INC and increase the FOR loop on line #32.
     

15.  Another useful Windows Application is included in the NCT directory.
     This application is called:  NETOSCT.EXE

     This application allows you to call each of the Windows Network API's 
     individually with your parameters.  
     

16.  If you are running DOS versions 4.x, the DOS Multi-VM, Stress Test, and
     Standard Mode Task Swapper Test will fail.  You will need to check the
     log files to find out where the actual failures occurred.  The failures
     should only occur where the batch files are attempting to attrib the
     files.  If you find that these are the only errors, you may consider 
     this test PASSED.


17.  The following is a reference of scripts and their associated log files.

     Test:                      Script name:            Log File:        
     ********************       *******************     *****************
     Applet Network Test        applet.wtd              applet.log
     DOS Multi-VM Test          dosvm.wtd               dosvm.log
     Enh Net Lg File Test       eremiolg.wtd            eremiolg.log
     Enh Net Sm File Test       eremio.wtd              eremio.log
     FM Test - Drive A          fm_a.wtd                fm_a.log
     FM Test - Drive C          fm_c.wtd                fm_c.log
     FM Test - Network          fm_net.wtd              fm_net.log
     NetBIOS VM Test            netbios.nct             dnetbios.log
     Network Stress Test        stress.wtd              stress.log
     Std Net Lg File Test       sremio.wtd              sremio.log
     Std Net Sm File Test       sremiolg.wtd            sremiolg.log
     Task Swapper Test          stdos.wtd               stdos.log
     WinNet API Test            winnet.wtd              winnet.log
     WinNet Prt API Test        wnetprt.wtd             wnetprt.log
     Win NetBIOS Test           netbios.nct             netosct.log

