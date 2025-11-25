This directory contains tests to help you ensure that your WAN
drivers work well with RAS.  

Directory structure
-------------------

x86     DIR     Directory containing x86 test applications
mips    DIR     Directory containing MIPS test applications
alpha   DIR     Directory containing ALPHA test applications
*.bat           various tests for verifying driver stability.  Work
		with appropriate binaries in x86, mips, and alpha
		directories
*.txt           files containing instructions for the various tests.


Other Tests
-----------

1.      Run different i/o tests simultaneously.  eg. run FTP
        test while running file i/o tests to Windows NT and NetWare
        servers at the same time.

2.      Ensure that your driver works with the callback feature 
        in RAS

3.      Run some off-the-shelf network applications over your 
        RAS connection.  For example, use MS-Mail, Schedule+, 
        and other network applications over a RAS 
        connection established with your WAN adapter.

4.      If possible, test interoperability with other hardware
        vendors' equipment.  For example, if you have an ISDN
        driver, test it connecting to another vendor's ISDN card.

5.      If your WAN adapter has multiple ports, you should test
        it with the maximum number of simultaneous connections
        possible.  eg. if you have an 8 port ISDN adapter, you
        should test 8 simultaneous connections to your adapter.

6.      Install more than one WAN adapter on the Windows NT Server
        to ensure that your driver works well with this configuration

7.      Test your driver on Multiprocessor systems in addition to
        single processor Intel, MIPS, and DEC ALPHA systems.


