# Clean and Restore utility
# designed to read a log file and remove garbage characters
# that might cause log processing apps problems (i.e. eof, etc.)
# also now removes the Browser String and Referer string that
# the filter adds to the logs...so that we get back to the
# original iis log format...
#
# This utility assumes that the log is filled with ONLY
# W3SVC entries.  When it has finished, you will have three
# copies of the log file.  
# 
# <logname>.clean   new iis format w/o any "garbage" chars
# <logname>.dirty   new iis format w/o any changes
# <logname>         original iis format  w/o UserAgent and Referer
#
#
# author: microsoft
# last update: 2/26/96

	die "You must provide a file name.\n" unless $ARGV[0]
	$szFileName = $ARGV[0]; #get filename and or path
    	$szFileName =~ tr/a-z/A-Z/; #uppercase the filename
    	$szOldLogName = $szFileName . ".dirty"; #rename the original file to filename.dirty
	$szOldCleanLog = $szFileName . ".clean";
    rename($szFileName, $szOldLogName); #finish the rename
    
    open (LOGFILE, "$szOldLogName") || die "failed to open $szOldLogName"; #open file for read
   
	binmode(LOGFILE); #change to binary mode so we don't choke on a bogus eof

    open (CLEAN , "+>> $szOldCleanLog") || die "failed to open $szOldCleanLog for write operation";

	select(CLEAN);
	$| = 1;
	binmode(CLEAN);

    open (NEWLOG, "+>> $szFileName") || die "failed to open $szFileName for write operation"; #open new log

	select(NEWLOG);
	$| = 1;
    	binmode(NEWLOG); #binary mode
    
    print "Working on $szFileName...\n ...writing cleaned log. Old log will be renamed to .dirty\n";
    while ($szLogLine = <LOGFILE>) #read until we reach end
    {
		$tmptotal++;
    		if ($tmptotal >= 1000) {
        	print STDOUT ".";
        	$total += $tmptotal;
        	$tmptotal = 0;
    		}
	
        $szLogLine =~ tr/\0-\011/ /; #strip all the bogus characters
	$szLogLine =~ tr/\013-\014/ /; #note the numbers are in octal
	$szLogLine =~ tr/\016-\037/ /;
	$szLogLine =~ tr/\251-\376/ /; 

	print CLEAN "$szLogLine";

	($machine, $user, $date, $time, $x, $y, $host, $ms, $size, $cbsent,
      $retcode, $w32code, $verb, $object, $Browser, $Refer, $inet) = split( /\054/, $szLogLine, 17);
	
    ###Statistics Here###
    #at this point you have the UserAgent in $Browser and the
    #Referer in $Refer giving you the ability to generate stats based on 
    #these entries.

      $szNewLogLine = "$machine" . "," . "$user" . "," . "$date" . "," . "$time" . "," . "$x" . "," . "$y" . "," . "$host" . "," . "$ms" . "," . "$size" . "," . "$cbsent" . "," . "$retcode" . "," . "$w32code" . "," . "$verb" . "," . "$object" . "," . "$inet";
        print NEWLOG "$szNewLogLine"; #output cleaned data
    }
	print NEWLOG "\032"; #write eof to newlog since we stripped it off
	print CLEAN "\032";
    close (NEWLOG); #close the two files
    close (LOGFILE);
    close (CLEAN);

