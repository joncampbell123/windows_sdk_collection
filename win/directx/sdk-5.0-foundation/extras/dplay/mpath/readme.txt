                           README.TXT
               Mplayer DirectPlay SDK Version 1.0
                     ----------------------

Welcome to the Mplayer DirectPlay SDK. This SDK is designed to allow
rapid testing and deployment of DirectPlay games on the Mplayer Internet
Game Service. Mpath Interactive also provides a whole host of services
and technologies through the Mpath Foundation to enable you to build and
deploy your own internet games and services.

Joining The Mplayer Developer Program
-------------------------------------

The first step in developing DirectPlay games to run on the Mplayer
Internet Game Service is to join Mplayer’s Developer Program. The
program is designed to keep you informed of the latest developments
at Mplayer and to discuss the guidelines required to deploy your
product on Mplayer. To join the Mplayer Developer Program, contact 
Mplayer at:

Direct:		1-415-429-3901  (Roy Harvey)
Company:	1-415-429-3900
Fax:		1-415-429-3911

Email:		dplay-support@mpath.com

Web:		http://www.mpath.com
		http://www.mplayer.com

Snail mail:	Mpath Interactive, Inc.
		665 Clyde Avenue
		Mountain View, CA 94043 USA


INSTALLING THE MPLAYER DIRECTPLAY SDK
-------------------------------------

In order to use the Mplayer DirectPlay SDK, you must install the
DirectX SDK (version 3.0 or later) available from Microsoft. You
should then install the Mplayer client software by running the
self-extracting archive MPCLIENT.EXE and finally the Mplayer
DirectPlay SDK by running the self-extracting archive MPDPSDK.EXE.

UNINSTALLING THE MPLAYER DIRECTPLAY SDK
---------------------------------------

If you want to uninstall the SDK,
  1. Delete all files and subdirectories of \mpdpsdk
  2. To remove start menu shortcuts, delete all files from
     \windows\Start Menu\Programs\Mplayer DirectPlay SDK
  3. From the Start Menu, select Run. Run the regedit program. Select
     HKEY_LOCAL_MACHINE\Software\Mpath\Mplayer\MANIFEST and delete
     the game manifest entries associated with game id's 551-553
     and any other games that you have registered yourself while
     developing for the SDK.

UNINSTALLING MPLAYER
--------------------

If you want to uninstall the Mplayer consumer product, you may go to
the Control Panel | Add/Remove Programs, and select Mplayer. 

Some older versions of Mplayer may not completely uninstall with this
method. To manually uninstall the Mplayer consume product, you may do
the following: 

  1. Delete all files and subdirectories of \program files\mplayer
  2. From the Start Menu, select Run. Run the regedit program. Select
     HKEY_LOCAL_MACHINE\Software\Mpath\Mplayer
  3. Select Edit | Delete from the menu.


