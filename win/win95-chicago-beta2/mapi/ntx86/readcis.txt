Instructions to install demo version of
CompuServe MAPI 1 Services for Windows NT and Chicago 


Since a full fledged installation utility is not included with this demo 
release version, it is important that these few steps be followed.  

	       ********************************

If you encounter any "unexpected behaviors" please GO MAPIBETA on CompuServe
and complete the Report Form.  This will allow us to respond to reports as 
quickly as possible by gathering all the neccessary information from the 
beggining.

If you have questions or comments:

Send them to   CIS:MAPIBETA or INTERNET:MAPIBETA@CIS.COMPUSERVE.COM


******************
INSTALLATION STEPS
******************


1. If you do not currently have a CompuServe Product such as DOSCIM, WINCIM, 
	or CSNAV that has the CompuServe connect scripts, you may obtain the 
	script files by running the self-extracting pkzip file CSERVE.EXE 
	with the -D parameter in the directory where you wish to install the 
	script files.  If you choose to run the program in the C:\CSERVE 
	directory, you will obtain a directory tree structure that looks like 
	this:

	C:\CSERVE
                |- SUPPORT
                |- SCRIPTS

	Be sure to run 	CSERVE -D to extract the files into the subdirectory
	tree structure shown above.


2.  If running CHICAGO, copy all DLL files and the CSMAIL.HLP file 
	    into the SYSTEM directory.

    If running NT, copy CSMAIL.HLP into the SYSTEM32 directory, then
	    copy all DLL files into the directory containing the MSMAIL
	    executables, and ensure this directory is included in your
	    environment's PATH.

    Files:
	
	CSMAPA.DLL  CSMAPX.DLL  MAPIABM.DLL  CCT200.DLL  CSMAIL.HLP


3.  If you chose a directory other than C:\CSERVE to install script files, 
	add the following line under the [CompuServe Mail] section in your 
	MSMAIL.INI:

		      CompuServe-Parent-Dir=X:\MYDIR\

	(where X:\MYDIR\ stands for the path to the CompuServe parent 
	  directory you are using)


	  NOTE:  Be sure to include the ending backslash.


4  Using the MAPI configuration utility called CFGITP32.EXE, (or the 
	Messaging Profiles option in CHICAGO's control panel), 
	configure the provider such that you use the CompuServe address book 
	data file called ADDRBOOK.DAT, usually found in 
	C:\CSERVE\SUPPORT\ADDRBOOK.DAT.  
	

	NOTE:  To configure the "CompuServe MAPI Services", use the 
	       "Profile Settings" push button, not the "Edit" push button.


	The CSERVE.EXE self-extracting zip file contains a preseeded address 
	book containing 1 address.  An example SERVICES.INI is included 
	which you may use or incorporate into the file located in your 
	windows SYSTEM directory (SYSTEM32 for NT) for use by the MAPI 
	config. utility.

	The MAPI Services neccessary for the CompuServe MAPI driver are: (**)

	CompuServe Address Book
	CompuServe Transport
	MS Personal Information Store
	MS Personal Address Book
	

	By default, when you create a new mail profile, you will obtain the
	MS Personal Information Store and the MS Personal Adress Book as a
	part of that profile.  If you wish to add both the CompuServe 
	transport and address book services, you may choose the single 
	option called:
	
	"CompuServe MAPI Services"



	(**) These may be added one by one if you have to manually merge the 
	SERVICES.INI provided with the master, and if CompuServe MAPI Services
	are no longer the default.


5.  If you do not have an existing CompuServe settings file (CIS.INI) file 
	present in your CompuServe parent directory, you will be prompted to 
	enter Communications settings when you first run the MSMAIL client.  
	The help facility may be used to answer any questions regarding the 
	proper use of the communications settings dialog, as you complete it.

	Driver functions are available by clicking on the CompuServe iconic
	window. (This will be found in the toolbar if using Chicago).


6.  Also when you first start up the MSMAIL client, you may be asked to 
	supply the locations of various files, if you have not already done
	so through the MAPI configuration utility in step #4 above.  The 
	CompuServe Address Book file is normally found in 
	C:\CSERVE\SUPPORT\ADDRBOOK.DAT

	When asked to supply the location of the Microsoft private address 
	book data file and the Microsoft local message store file, you may
	not have these files on your machine if this is the very first time
	that you have run MSMAIL.  If that is the case, you may use the
	dialog presented to CREATE the files by supplying the file and path
	name of your choice.


7.  When you first run the Mail Client with the CompuServe profile,
	you will be prompted to choose an active session to use for 
	connecting to CompuServe.  Depending on your system, this window
	may initially be covered up by the MS client parent window.

	If you wish to always use the active session, you may do so by
	editing the file MSMAIL.INI, located in the Windows parent directory.
	Under the section called [CompuServe Mail], add the following line:

	Use-Active-Session=1


**************************************************
Tips for using this pre-beta release MAPI 1 Driver
**************************************************


- The address type of our MAPI provider is:  CompuServe

	If you enter new address entries into the private address book,
	make sure you specify the address type above.


- The option to connect at "Exit of Mail" only pertains to sending messages,
	that is, mail will only be sent when the mail client is exited when
	this option is used.


- If the CompuServe driver icon window remains on-screen after the mail
	client has exited, this may be because the mail SPOOLER program is 
	still loaded in memory.  To remove the icon, run the SPOOLER 
	application by itself, then close the SPOOLER.


- By default, the driver is set up to delete retrieved mail from the host
	computer.  If this is not the desired behavior, you may easily 
	change the behavior by accessing the Settings item from the 
	CompuServe MAPI driver window.


- If you are unable to dial the modem and/or establish a connection, 
	use the following checklist:


	- Connect scripts are present (*.SCR files), and
		located in the C:\CSERVE\SCRIPTS directory.

	- The communications settings dialog contains the
		proper information for baud rate, comm port, phone number, 
		etc.

	- The MODEM initialization string matches your modem
		hardware's requirements.  (Refer to your modem owner's
		manual for more information.)

	- The CompuServe Parent directory points to the proper
		location so that the above files are found.

	- The phone number supplied supports the baud rate supplied.

	- Make sure that the setting for "Show Progress Window" is 
		turned on.


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


- The current implementation of the CompuServe Address Book services is
	for accessing a prior existing ADDRBOOK.DAT file in read-only mode.
	This is useful for developers who already have such a file
	from either DOSCIM, WINCIM, or CSNAV products.  If you do not
	already have an ADDRBOOK.DAT file, the CSERVE.EXE self-extracting
	file contains an example one with a single address.
	Future versions are planned to allow both read and write access.


-  A typical map of the installed CompuServe Files look like this:

	c:\cserve 	   (CIS.INI)
                |- support (ADDRBOOK.DAT)
                |- scripts (*.SCR, MODEM.DB)

	c:\chicago
	         |- system (CSMAP*.DLL, MAPIABM.DLL,CCT200.DLL,CSMAIL.HLP)


************************************************
Features tentatively planned for future releases
************************************************

	- More message options for transport

	- True multi-part mail transmission, allowing for "what you
		see is what you SENT" message delivery.  

	- Template dialogs for entering custom address formats, such as
		for internet or postal mail.

	- Read/write access to CompuServe address book (currently read only).

	- Streamlined transmission throughput.

	- Improved deferred message submission mechanism and non-delivery
		report (NDR) integration with MAPI


*********************************
Known Limitations of this Release
*********************************

	At this time, there is a known inconsistency with the handling of
attachments within messages.  This may cause attachments that you create 
within a message to not be visible to the CompuServe provider at upload
time.  This problem will NOT occur if you send the message using the
MS-Binary (TNEF) format.

	To alleviate this symptom, we recommend that you send messages with 
attachment(s) using the TNEF option.  To send a message using TNEF, you must 
use the "Send Options" under the File pulldown menu in the window used while
composing the message.  Checking on the option labelled 
"Send as MS-Binary (TNEF)" will cause attachments to be sent properly.

	NOTE:  The receiver must also be using MS Mail and a service
	       provider that recognizes the MS-Binary (TNEF) file format 
               for them to be able to view the message's contents.


	       ********************************

If you encounter any "unexpected behaviors" please GO MAPIBETA on CompuServe
and complete the Report Form.  This will allow us to respond to reports as 
quickly as possible by gathering all the neccessary information from the 
beggining.

Any updates or fixes to our service provider may also be found in this
area.

If you have questions or comments:

Send them to   CIS:MAPIBETA or INTERNET:MAPIBETA@CIS.COMPUSERVE.COM

