CompuServe MAPI 1 Services for Windows NT and Chicago  (Pre-Beta 2) 10/5/94



******************************
What's New? (since Pre-Beta 1)
******************************


1.  Installation of the CompuServe Services is now much easier using the
	MAPI profile wizard.  Simply use the wizard, and it will ask
	you for all the information needed to automatically configure
	the CompuServe providers.

2.  Support for Microsoft's Telephone API (TAPI) has been added to this 
	release.

3.  The iconic window that allowed access to immediate connect options and
	driver settings dialogs has been replaced in this release with
	a more integrated MAPI 1 style:

	--> Use the "Deliver Mail Now" pulldown menu item to both send and
		receive CompuServe Mail messages.

	--> Use the MAPI configuration facility to access driver settings.
		CFGITP32.EXE, (for Chicago, located in the control panel)

4.  The address book now supports adding new entries.  (Full read/write access)

5.  18 different template dialogs have been added, allowing for easy entry
	of addressing formats such as X.400, INTERNET, cc:Mail, etc.,
	for use by CompuServe Mail.

6.  Binary FILE attachments are now recognized and sent.  (You no longer need 
	to specify "Use TNEF format" for binary file transmission.)
	


*****************
EASY INSTALLATION
*****************


To add CompuServe services to a MAPI profile, add the following options
to your profile using the MAPI profile Wizard:


                          CompuServe Mail
                          CompuServe Address Book  


The wizard will ask for the information needed to properly configure
the CompuServe providers.



****************************
Tips for using this release:
****************************


- The address type of our MAPI provider is:  CompuServe

	If you enter new CUSTOM address entries into the Microsoft 
	Personal Address Book for delivery through CompuServe Mail, 
	make sure you specify the address type above.  (Entries added 
	into the CompuServe Address Book are automatically designated to
	be for CompuServe Mail.)


- The option to connect at "Exit of Mail" only pertains to sending messages,
	that is, mail will only be sent when the mail client is exited when
	this option is used.


- By default, the driver is set up to delete retrieved mail from the host
	computer.  If this is not the desired behavior, you may easily 
	change the behavior by editing the driver settings using the
	MAPI configuration tools, CFGITP32.EXE  (For Chicago, look in
	the control panel for the Info Center Configuration icon)


- If you are unable to dial the modem and/or establish a connection, 
	use the following checklist:


	- If you are using TAPI to dial the modem, check that you
		have properly set up the modem settings under
		Chicago's control panel.  (NOTE:  TAPI is only available
		with the Chicago version.)

	- When using TAPI, make sure that you have selcted TAPI as the
		CONNECTOR type in the CompuServe session settings
		dialog.

	- Connect scripts are present (*.SCR files), and
		located in the \SCRIPTS subdirectory, underneath
		your CompuServe parent directory.
		(Usually, scripts are located in "C:\CSERVE\SCRIPTS").

	- The communications settings dialog contains the
		proper information for baud rate, comm port, phone number, 
		etc.  
		
		*****
		NOTE:  When using TAPI, the baud rate, dial type, and 
		*****  modem strings in the CompuServe Session Settings 
		       dialog are not used.  Instead, the settings 
		       registered with TAPI for your modem configuration 
		       are used.

	- The MODEM initialization string matches your modem
		hardware's requirements.  (Refer to your modem owner's
		manual for more information.)

	- The CompuServe Parent directory points to the proper
		location so that the script files are found.

	- The phone number supplied supports the baud rate that you
		are using.

	- Make sure that the setting for "Show Progress Window" is 
		turned on.


-  A typical map of the installed CompuServe Files look like this:

	c:\cserve 	   (CIS.INI)
                |- support (ADDRBOOK.DAT)
                |- scripts (*.SCR, MODEM.DB)

	c:\chicago
	         |- system (CSMAP*.DLL, MAPIABM.DLL,CCT200.DLL,CSMAIL.HLP)




*************************************************
Features tentatively planned for future releases:
*************************************************

	- More message options for transport

	- True multi-part mail transmission, allowing for "what you
		see is what you SENT" message delivery.  

	- Streamlined transmission throughput.

	- Improved deferred message submission mechanism and non-delivery
		report (NDR) integration with MAPI

	- OLE and embedded message support, without TNEF



******************************
Known Behaviors of Pre-Beta 2:
******************************

- Currently, the driver will place outgoing messages into the "sent mail"
	folder prior to the physical upload process to CompuServe.  This
	is intentional.  Note that the mail is not physically uploaded until
	the "connect at time" settings dictate, or an immediate send operation
	is performed.  This mechanism will be modified in a future release
	to file the message into "sent mail" after the actual transmission.


- Currently, the MAPI form of Non-Delivery Reports (NDRs) are not generated.  
	Instead, you may use the copy of the outgoing message filed into the 
	"sent mail" folder if you wish to modify its contents and resend.
	MAPI NDRs are also slated for a future release.



************************
Reporting Problems, etc.
************************

If you encounter any "unexpected behaviors", please GO MAPIBETA on CompuServe
and complete the Report Form.  This will allow us to respond to reports as 
quickly as possible by gathering all the neccessary information from the 
beggining.

Any updates or fixes to our service provider may also be found in this area.



*********************
Questions?  Comments?
*********************

Please send any question or comments on this release to:

	CIS:MAPIBETA 
	
	or 

	INTERNET:MAPIBETA@CIS.COMPUSERVE.COM


Please specify "Pre-Beta 2" in your message.


