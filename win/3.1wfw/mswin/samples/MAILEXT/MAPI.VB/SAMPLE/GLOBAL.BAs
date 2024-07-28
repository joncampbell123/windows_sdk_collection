'Global Const TRUE = -1
'Global Const FALSE = 0

'****************************************************************************'
'                                                                            '
' Visual Basic declaration for the MAPI functions.                           '
' This file can be loaded into the global module.                            '
'                                                                            '                                                                           '
'****************************************************************************'


'***************************************************
'   MAPI Message holds information about a message
'***************************************************

Type MAPIMessage
    Reserved As Long
    Subject As String
    NoteText As String
    MessageType As String
    DateReceived As String
    ConversationID As String
    Flags As Long
    RecipCount As Long
    FileCount As Long
End Type


'************************************************
'   MAPIRecip holds information about a message
'   originator or recipient
'************************************************

Type MapiRecip
    Reserved As Long
    RecipClass As Long
    Name As String
    Address As String
    EIDSize As Long
    EntryID As String
End Type


'******************************************************
'   MapiFile holds information about file attachments
'******************************************************

Type MapiFile
    Reserved As Long
    Flags As Long
    Position As Long
    PathName As String
    FileName As String
    FileType As String
End Type


'***************************
'   FUNCTION Declarations
'***************************

Declare Function MAPILogon Lib "MAPI.DLL" (ByVal UIParam&, ByVal User$, ByVal Password$, ByVal Flags&, ByVal Reserved&, Session&) As Long
Declare Function MAPILogoff Lib "MAPI.DLL" (ByVal Session&, ByVal UIParam&, ByVal Flags&, ByVal Reserved&) As Long
Declare Function BMAPIReadMail Lib "MAPI.DLL" (lMsg&, nRecipients&, nFiles&, ByVal Session&, ByVal UIParam&, MessageID$, ByVal Flag&, ByVal Reserved&) As Long
Declare Function BMAPIGetReadMail Lib "MAPI.DLL" (ByVal lMsg&, Message As MAPIMessage, Recip As MapiRecip, File As MapiFile, Originator As MapiRecip) As Long
Declare Function MAPIFindNext Lib "MAPI.DLL" Alias "BMAPIFindNext" (ByVal Session&, ByVal UIParam&, MsgType$, SeedMsgID$, ByVal Flag&, ByVal Reserved&, MsgId$) As Long
Declare Function MAPISendDocuments Lib "MAPI.DLL" (ByVal UIParam&, ByVal DelimStr$, ByVal FilePaths$, ByVal FileNames$, ByVal Reserved&) As Long
Declare Function MAPIDeleteMail Lib "MAPI.DLL" (ByVal Session&, ByVal UIParam&, ByVal MsgId$, ByVal Flags&, ByVal Reserved&) As Long
Declare Function MAPISendMail Lib "MAPI.DLL" Alias "BMAPISendMail" (ByVal Session&, ByVal UIParam&, Message As MAPIMessage, Recipient As MapiRecip, File As MapiFile, ByVal Flags&, ByVal Reserved&) As Long
Declare Function MAPISaveMail Lib "MAPI.DLL" Alias "BMAPISaveMail" (ByVal Session&, ByVal UIParam&, Message As MAPIMessage, Recipient As MapiRecip, File As MapiFile, ByVal Flags&, ByVal Reserved&, MsgId$) As Long
Declare Function BMAPIAddress Lib "MAPI.DLL" (lInfo&, ByVal Session&, ByVal UIParam&, Caption$, ByVal nEditFields&, Label$, nRecipients&, Recip As MapiRecip, ByVal Flags&, ByVal Reserved&) As Long
Declare Function BMAPIGetAddress Lib "MAPI.DLL" (ByVal lInfo&, ByVal nRecipients&, Recipients As MapiRecip) As Long
Declare Function MapiDetails Lib "MAPI.DLL" Alias "BMAPIDetails" (ByVal Session&, ByVal UIParam&, Recipient As MapiRecip, ByVal Flags&, ByVal Reserved&) As Long
Declare Function MapiResolveName Lib "MAPI.DLL" Alias "BMAPIResolveName" (ByVal Session&, ByVal UIParam&, ByVal UserName$, ByVal Flags&, ByVal Reserved&, Recipient As MapiRecip) As Long



'**************************
'   CONSTANT Declarations
'**************************
'

Global Const SUCCESS_SUCCESS = 0
Global Const MAPI_USER_ABORT = 1
Global Const MAPI_E_FAILURE = 2
Global Const MAPI_E_LOGIN_FAILURE = 3
Global Const MAPI_E_DISK_FULL = 4
Global Const MAPI_E_INSUFFICIENT_MEMORY = 5
Global Const MAPI_E_BLK_TOO_SMALL = 6
Global Const MAPI_E_TOO_MANY_SESSIONS = 8
Global Const MAPI_E_TOO_MANY_FILES = 9
Global Const MAPI_E_TOO_MANY_RECIPIENTS = 10
Global Const MAPI_E_ATTACHMENT_NOT_FOUND = 11
Global Const MAPI_E_ATTACHMENT_OPEN_FAILURE = 12
Global Const MAPI_E_ATTACHMENT_WRITE_FAILURE = 13
Global Const MAPI_E_UNKNOWN_RECIPIENT = 14
Global Const MAPI_E_BAD_RECIPTYPE = 15
Global Const MAPI_E_NO_MESSAGES = 16
Global Const MAPI_E_INVALID_MESSAGE = 17
Global Const MAPI_E_TEXT_TOO_LARGE = 18
Global Const MAPI_E_INVALID_SESSION = 19
Global Const MAPI_E_TYPE_NOT_SUPPORTED = 20
Global Const MAPI_E_AMBIGUOUS_RECIPIENT = 21

Global Const MAPI_ORIG = 0
Global Const MAPI_TO = 1
Global Const MAPI_CC = 2
Global Const MAPI_BCC = 3


'***********************
'   FLAG Declarations
'***********************

Global Const MAPI_LOGON_UI = &H1
Global Const MAPI_NEW_SESSION = &H2
Global Const MAPI_DIALOG = &H8
Global Const MAPI_UNREAD_ONLY = &H20
Global Const MAPI_ENVELOPE_ONLY = &H40
Global Const MAPI_PEEK = &H80
Global Const MAPI_GUARANTEE_FIFO = &H100
Global Const MAPI_BODY_AS_FILE = &H200
Global Const MAPI_AB_NOMODIFY = &H400
Global Const MAPI_SUPPRESS_ATTACH = &H800
Global Const MAPI_FORCE_DOWNLOAD = &H1000

Global Const MAPI_OLE = &H1
Global Const MAPI_OLE_STATIC = &H2


'*****************************************
' Global variables used by MapiDemo only
'*****************************************

Global MapiSession&
Global rc As Long
Global MsgId$
Global Recips() As MapiRecip, Files() As MapiFile
Global PopupErrors As Integer

