README.TXT for MDB viewer

This file describes what the MDB viewer is, how to build
the MDB viewer, what it does, and how to use it.
MDB stands for Message Data Base. The terms
MDB, Message Data Base, Store, and Message Store all mean the 
same thing.

*********** What is the MDB Viewer ? *********************

The MDB Viewer is a MFC windows application that allows 
viewing and interactive testing of any MAPI 1.0 compliant message
store.  It allows viewing of properties of message store objects,
table values, folder heirarchies, and messages/attachments contained
in these folders.  It also allows interactive testing of many of the
message store object, folder object, message object, and attachment 
object functions available to all MAPI 1.0 message stores.

The MDB viewer is not the primary method of testing being done at Microsoft
for the MAPI 1.0 product, but serves as a tool for interactive testing, 
and complements the existing testing methods and tools.


*********** How do you build the MDB viewer ? ************

What we want to do is build the table viewer dll first, and 
then the mdb viewer exe.  
The 16 bit table viewer dll is called tblvu.dll
The 32 bit table viewer dll is called tblvu32.dll
The 16 bit mdb viewer exe is called mdbvu.exe
The 32 bit mdb viewer exe is called mdbvu32.exe         
          
To build 32 bit debug MDB viewer:

1.  $(MAPI)\samples\src\tablevu\nmake clean               // this cleans up directory of old
                                                          //   build's object files etc.
2.  $(MAPI)\samples\src\tablevu\nmake                     // this builds table viewer DLL  
                                                          //  tblvu32.dll to link with the MDB viewer
3.  $(MAPI)\samples\src\mdbview\nmake clean               // this cleans up directory of old
                                                          //   build's object files etc.
4.  $(MAPI)\samples\src\mdbview\nmake                     // this builds mdbvu32.exe
                          
To build 16 bit debug MDB viewer:

1.  $(MAPI)\samples\src\tablevu\nmake clean               // this cleans up directory of old
                                                          //   build's object files etc.
2.  $(MAPI)\samples\src\tablevu\nmake PRODUCT=WIN16DLLMFC // this builds table viewer DLL  
                                                          // tblvu.dll to link with the MDB viewer
3.  $(MAPI)\samples\src\mdbview\nmake clean               // this cleans up directory of old
                                                          //   build's object files etc.
4.  $(MAPI)\samples\src\mdbview\nmake PRODUCT=WIN16EXE    // this builds mdbvu.exe

To build 32 bit ship MDB viewer:

1.  $(MAPI)\samples\src\tablevu\nmake clean               

2.  $(MAPI)\samples\src\tablevu\nmake VERSION=SHIP
                                                        
3.  $(MAPI)\samples\src\mdbview\nmake clean             
                                                        
4.  $(MAPI)\samples\src\mdbview\nmake VERSION=SHIP      

To build 16 bit ship MDB viewer:

1.  $(MAPI)\samples\src\tablevu\nmake clean                                                                     

2.  $(MAPI)\samples\src\tablevu\nmake PRODUCT=WIN16DLLMFC VERSION=SHIP
                                                          
3.  $(MAPI)\samples\src\mdbview\nmake clean             
                                                        
4.  $(MAPI)\samples\src\mdbview\nmake PRODUCT=WIN16EXE VERSION=SHIP   


*********** What functionality does the MDB viewer have ? ************

****** The message store viewer can view properties of the following Message 
store objects:

Folders
Messages
Attachments

****** Each dialog shows what logically exists underneath/within its object.
Ex. A folder shows subfolders and messages within the current folder
and a message shows attachments and recipients

****** You can view properties as you choose(HEX, DECIMAL, or STRING format
for PropTags, and with/without IDS/TAGS/DATA) based upon checkboxes and radio
buttons for property display

****** Furthermore it can view table interfaces and do all table operations on 
all the following table interfaces in MAPI 1.0 message stores:

Recipient Tables        (recipients in message)
Attachment Tables       (attachments in message)
Status Tables           (status of MAPI and Spooler)
Message Stores Table    (message stores in MAPI)
Contents Tables         (messages in folder)
Heirarchy Tables        (subfolders in folder)

****** Folder, Message and Attachment dialogs are modeless, and therefore you
can minimize all the objects on your desktop and directly compare 2 folders, etc.

****** The MDB viewer also can do the following interactive functions within the 
store:

lpFolder->CreateFolder()
lpFolder->CopyFolder() 
lpFolder->DeleteFolder()
lpFolder->GetHeirarchyTable() 
lpFolder->GetContentsTable()  
lpFolder->SetProps()          
lpFolder->DeleteProps()
lpFolder->CreateMessage()
lpFolder->CopyMessage()
lpFolder->DeleteMessage()
lpFolder->SaveChanges()

lpMessage->SetReadFlag()
lpMessage->SaveChanges()
lpMessage->SetProps()
lpMessage->DeleteProps()
lpMessage->CreateAttach()
lpMessage->DeleteAttach()
lpMessage->GetRecipientTable()
lpMessage->ModifyRecipients()
lpMessage->SubmitMessage()

lpAttach->SaveChanges()
lpAttach->SetProps()
lpAttach->DeleteProps()

lpMDB->GetReceiveFolder()
lpMDB->SetReceiveFolder()
lpMDB->AbortSubmit()

****** In addition the viewer calls the following functions internally(without
your ability to alter the parameters) :

MAPILogon()
lpSession->Logoff()
lpSession->OpenAddressBook()
lpSession->OpenMessageStore()
lpSession->GetMsgStoresTable()
lpSession->Release()

lpMDB->OpenEntry()
lpMDB->StoreLogoff()

lpFolder->Release()
lpFolder->OpenEntry()

lpMessage->OpenAttach()
lpMessage->Release()

lpAttach->Release()

****** LIMITATIONS :

Currently the message store viewer only opens the message store denoted 
as the default store in the Message Stores table.  Ability to open other 
stores will be put into future version of the MDB viewer.  For now, use the
profile editor to configure the store you want to open as the default store.

DeleteProps() can only delete one property at a time.

CopyMessages() can only copy one message at a time.

Addressing for ModifyRecipients goes through the MAPI common address dialog only.

The application currently does not use MAPI Notification, and therefore, sometimes in
order to get a current refresh of listboxes with Messages and folders, you must click on
the listbox.

*********** How to use the MDB viewer **********************************

The MDB viewer is a windows application, and is fairly intuitive, but there are a
few things that can be tricky and I will explain how to use them here.

When you are in a dialog, let's say in a folder dialog, the context of all the operations
performed is in the Folder you are currently looking at(upper left listbox with foldername
describes where you are; the dialog title bar also describes your current location). 

Ex. if I do a CreateFolder from the Root folder, the new folder dialog will appear on top of our current
root folder dialog, and this folder will be added to the list of subfolders in the HeirarchyTable
of the root folder....so the subfolder is created underneath our current location.

For the interactive buttons to become active you have to be in the correct context...you have to
have a message selected in the messages listbox to do a DeleteMessage().

Double click on listbox items or select an item in a listbox and select a button to open this object.

CopyMessages() and CopyFolder() require a bit of explaining.  Each of these needs a destination folder.
Therefore I put in a mechanism for determining where the destination folder will be for the next call
to CopyMessages() and/or CopyFolder().  To select the next destination folder for either of these
calls, open a folder you want to be the destination folder and double click on the current folder listbox
which is the listbox in the upper left of the folder dialog with only one entry in it.  This will bring up
a message box describing that this will be the next destination folder for any CopyMessages() or CopyFolder()
Then go to the source folder and select CopyMessages() or CopyFolder().....you get the picture.





