Immortal Klowns Sample Game
---------------------------

To build Immortal Klowns, do an NMAKE in this directory.

Be sure that the proper environment variables have been set up for the
windows include files and lib files.

When the make is done, just type IKLOWNS in the current directory.  Make sure
that you DO NOT run it from the debug or retail subdirectories - if you do,
you will get an error because IKLOWNS can't find all the resources it needs
to run.

To play using TCP/IP over the Internet, the people who are joining the
game must enter the IP address of the machine that hosted the game.
You can find out the IP address of your machine by running "winipcfg".
If you have a net card and a modem installed, you will need to make sure
you read the IP address for the modem connected to the Internet. The
modem will be listed as "PPP Adapter".  DirectPlay will not work through
proxies and firewalls.

Also see the DPLAUNCH sample.


