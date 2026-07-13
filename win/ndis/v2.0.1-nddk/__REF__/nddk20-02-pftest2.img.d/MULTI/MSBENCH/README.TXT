
	Usage of multinode performance tests - 10 step approach
	------------------------------------------------------

	1. Install the multi dir on the server on which you are going to run
	   the multinode performance tests. It has following subdirs:
		    * msbench : all test scripts and files
		    * Copyfils: Files for copyfile tests
		    * Loadfils: Files for loadfile tests

	2. Set an environmental variable machine=nn (nn = 1..6) on all the
	   workstations participating in the performance tests. 
	   (If more than 6 workstations are used then please modify m.cmd and
	    report.tmp suitably. Please see the next step)

	3. Edit report.tmp file in the \multi\msbench directory to include 
		* Server configuration
		* Workstation configuration 
		* Product specific information (as a title)
		* Number of workstations for which the performance report 
		  is generated.

	4. Set up a share for the path where the "multi" dir is residing on the
	   server (e.g. Net share ddr=d:\ ).

	5. Make connections from all the workstations to the server 
	   (e.g Net use t: \\Serv\ddr).

	6. Change to the redirected drive and go in the multi\msbench dir.
	   t:
	   cd multi\msbench

	7. type one of the following commands on the first machine i.e. 
	   with machine = 1:
	   for DOS
		DOS <drive name> <output file name>
		    e.g DOS t HPFS386
	   for OS/2
		OS2 <drive name> <output file name>
		    e.g OS2 t HPFS386
	
	8. When the first workstation stops and displays "start other machines" 
	   type the same command on all the other workstations. When all the
	   other workstations start looping in "goto hold" and are waiting for 
	   the first workstation to generate a SYNC message, start the first 
	   workstation by pressing any key on it. 
				
	9. The multinode performance tests would run by themselves. After 
	   having completed one cycle of all the tests for the given number of 
	   workstations the server will omit the workstation with maximum 
	   machine number(e.g. 6) and start a new test cycle. This process will
	   continue till the first workstation (machine = 1) completes the 
	   test cycle.  Reports will be generated in 
	   	 \multi\outut\<output file name>.rpt and
	     		      <output file name>.xls (excel charts)
			      e.g t:\multi\output\HPFS386.rpt	
			          t:\multi\output\HPFS386.xls	

	10. Report any failures during this process or be happy with the 
	    performance results you get!. Failure could occur either during
	   the performance tests execution phase or report generation phase. 
	   
-------------------------------------------------------------------------------
