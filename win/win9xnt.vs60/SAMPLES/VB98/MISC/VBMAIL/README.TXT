			VBMAIL Sample

The VBMail sample uses the MAPISessions and MAPIMessages controls in tandem with an underlying mail application, such as Microsoft Exchange or Microsoft Outlook, to view and compose messages. The sample also uses the ActiveX Data Objects Recordset Library 2.0 to create a stand-alone recordset. That recordset is filled with unread messages supplied by the MAPIMessages control. 

Files			Description

frmLogOn.frm		Log on and view unread messages.
frmRead.frm		The form used to read a message.
prjMapi.vbp		Project file.

To Run
The DataGrid control is used to view the unread messages. Clicking on a message lets you read the message. Other functions include displaying the underlying mail system's address book, and composing a new message.

While reading a message, you can reply, reply to all, or forward the message.
