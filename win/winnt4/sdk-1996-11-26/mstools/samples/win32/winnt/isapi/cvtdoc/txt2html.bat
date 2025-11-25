rem ----------------------------------------------------------
rem TXT2HTML.BAT
rem
rem Converts text file to HTML using Perl script.
rem Install into HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\W3SVC
rem \Parameters\Conversions as value .txt: txt2html.bat %s
rem
rem Uses Seth Golub's txt2html Perl script (see txt2html.pl)
rem This batch file requires a Perl implementation such as 
rem NT Perl on the Windows NT 3.51 Resource Kit 
rem or on Hip Communications' Web site http://www.perl.hip.com
rem
perl txt2html.pl < %1 > %1.htm
