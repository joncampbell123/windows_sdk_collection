VERSION 5.00
Object = "{20C62CAE-15DA-101B-B9A8-444553540000}#1.1#0"; "msmapi32.Ocx"
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Object = "{CDE57A40-8B86-11D0-B3C6-00A0C90AEA82}#1.0#0"; "MSDatGrd.Ocx"
Begin VB.Form frmMain 
   Caption         =   "VB Mail"
   ClientHeight    =   6675
   ClientLeft      =   1410
   ClientTop       =   1605
   ClientWidth     =   9540
   LinkTopic       =   "Form1"
   NegotiateMenus  =   0   'False
   ScaleHeight     =   6675
   ScaleWidth      =   9540
   Begin MSComCtlLib.Toolbar tbrMail 
      Align           =   1  'Align Top
      Height          =   630
      Left            =   0
      TabIndex        =   2
      Top             =   0
      Width           =   9540
      _ExtentX        =   16828
      _ExtentY        =   1111
      ButtonWidth     =   1931
      ButtonHeight    =   953
      Appearance      =   1
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   8
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Log On"
            Key             =   "LogOn"
            Description     =   "Log on"
            Object.ToolTipText     =   "Log On"
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Caption         =   "Log Off"
            Key             =   "logOff"
            Description     =   "Log off"
            Object.ToolTipText     =   "Log Off"
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Style           =   4
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Caption         =   "Fetch"
            Key             =   "fetch"
            Description     =   "Fetch unread messages"
            Object.ToolTipText     =   "Fetch unread messages"
         EndProperty
         BeginProperty Button5 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Caption         =   "Address Book"
            Key             =   "address"
            Description     =   "Show Address Book"
            Style           =   5
            BeginProperty ButtonMenus {66833FEC-8583-11D1-B16A-00C0F0283628} 
               NumButtonMenus  =   2
               BeginProperty ButtonMenu1 {66833FEE-8583-11D1-B16A-00C0F0283628} 
                  Enabled         =   0   'False
                  Key             =   "global"
                  Text            =   "Global"
               EndProperty
               BeginProperty ButtonMenu2 {66833FEE-8583-11D1-B16A-00C0F0283628} 
                  Enabled         =   0   'False
                  Key             =   "recepient"
                  Text            =   "Recepient"
               EndProperty
            EndProperty
         EndProperty
         BeginProperty Button6 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Style           =   4
         EndProperty
         BeginProperty Button7 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Caption         =   "Compose"
            Key             =   "compose"
            Description     =   "Compose messages"
            Object.ToolTipText     =   "Compose new message"
         EndProperty
         BeginProperty Button8 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
      EndProperty
   End
   Begin MSComCtlLib.ImageList imlMail 
      Left            =   240
      Top             =   5160
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   32
      ImageHeight     =   32
      MaskColor       =   12632256
      _Version        =   393216
   End
   Begin MSDataGridLib.DataGrid grdMess 
      Height          =   3780
      Left            =   60
      TabIndex        =   1
      Top             =   1155
      Width           =   9435
      _ExtentX        =   16642
      _ExtentY        =   6668
      _Version        =   393216
      AllowUpdate     =   0   'False
      HeadLines       =   1
      RowHeight       =   15
      BeginProperty HeadFont {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ColumnCount     =   2
      BeginProperty Column00 
         DataField       =   ""
         Caption         =   ""
         BeginProperty DataFormat {6D835690-900B-11D0-9484-00A0C91110ED} 
            Type            =   0
            Format          =   ""
            HaveTrueFalseNull=   0
            FirstDayOfWeek  =   0
            FirstWeekOfYear =   0
            LCID            =   1024
         EndProperty
      EndProperty
      BeginProperty Column01 
         DataField       =   ""
         Caption         =   ""
         BeginProperty DataFormat {6D835690-900B-11D0-9484-00A0C91110ED} 
            Type            =   0
            Format          =   ""
            HaveTrueFalseNull=   0
            FirstDayOfWeek  =   0
            FirstWeekOfYear =   0
            LCID            =   1024
         EndProperty
      EndProperty
      SplitCount      =   1
      BeginProperty Split0 
         MarqueeStyle    =   1
         BeginProperty Column00 
         EndProperty
         BeginProperty Column01 
         EndProperty
      EndProperty
   End
   Begin MSComCtlLib.StatusBar sbrMapi 
      Align           =   2  'Align Bottom
      Height          =   255
      Left            =   0
      TabIndex        =   0
      Top             =   6420
      Width           =   9540
      _ExtentX        =   16828
      _ExtentY        =   450
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
         EndProperty
      EndProperty
   End
   Begin MSMAPI.MAPIMessages mapMess 
      Left            =   615
      Top             =   615
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      AddressEditFieldCount=   1
      AddressModifiable=   0   'False
      AddressResolveUI=   0   'False
      FetchSorted     =   0   'False
      FetchUnreadOnly =   0   'False
   End
   Begin MSMAPI.MAPISession mapSess 
      Left            =   -45
      Top             =   570
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      DownloadMail    =   -1  'True
      LogonUI         =   -1  'True
      NewSession      =   0   'False
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuMail 
      Caption         =   "&Mail"
      Begin VB.Menu mnuLogOn 
         Caption         =   "Log O&n"
      End
      Begin VB.Menu mnuLogOff 
         Caption         =   "Log O&ff"
         Enabled         =   0   'False
      End
   End
   Begin VB.Menu mnuTools 
      Caption         =   "&Tools"
      Enabled         =   0   'False
      Begin VB.Menu mnuCheck 
         Caption         =   "&Check for New Mail"
         Enabled         =   0   'False
         Shortcut        =   {F5}
      End
      Begin VB.Menu mnuAddress 
         Caption         =   "Address Book"
         Enabled         =   0   'False
         Shortcut        =   ^{F1}
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private bNewSession As Boolean ' Flag to signal logon status.
' Set the gbIgnoreEvent to True whenever you are
' about to do an operation that causes the DataGrid
' control's BeforeColEdit event to occur. When True,
' the event will exit without further execution.
Private gbIgnoreEvent As Boolean
Private WithEvents rsUnread As ADODB.Recordset
Attribute rsUnread.VB_VarHelpID = -1
Private gbGridConfigured As Boolean ' Flag to configure DataGrid
Private gbRSalreadyPopulated As Boolean ' Flag to signal RS populated


Private Sub Form_Load()
    ' Configure StatusBar.
    sbrMapi.Panels(1).Key = "SessID"
    sbrMapi.Panels.Add , "MsgCnt" ' Add Panel
End Sub
Private Sub tbrMail_ButtonClick(ByVal Button As MSComCtlLib.Button)
    ' Use Select Case statement to determine which button
    ' was clicked, then react appropriately.
    On Error GoTo ButtonErr
    
    Select Case Button.Key
    Case "LogOn"
    ' Log and fetch unread messages. CheckRS checks
    ' if the ADORecordset has already been populated,
    ' and populates it if needed. DoGrid configures the
    ' grid and populates with the recordset.
        If LogOn = True Then
            FetchUnreadOnly
            CheckRS
            DoGrid
        Else
            Exit Sub
        End If
    Case "logOff" ' Log off
        LogOff
    Case "fetch"
        FetchUnreadOnly
        ' Check to see if the ADO Recordset is
        ' already populated. Then populate grid.
        CheckRS
        DoGrid
    Case "compose" ' Create a new message.
        ComposeMessage
    Case "address"
        Debug.Print "something else"
    Case Else
        Debug.Print Button.Key
    End Select
    Exit Sub
ButtonErr:
    Debug.Print Err.Number, Err.Description
    Resume Next
End Sub

Private Sub grdMess_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If mapSess.SessionID = 0 Or mapMess.MsgCount = 0 Then Exit Sub
    grdMess.Row = grdMess.RowContaining(Y)
    If gbIgnoreEvent Then Exit Sub
  
    On Error GoTo RowERR
    mapMess.MsgIndex = grdMess.Columns(0).Value ' Set MsgIndex.
    
    Load frmRead ' Load the form.
    ' Read the message and headers and put it in
    ' the appropriate textboxes.
    With frmRead
        .lblFrom = mapMess.MsgOrigDisplayName
        .lblCC = GetList(mapCcList)
        .lblTo = GetList(mapToList)
        .lblSubject = mapMess.MsgSubject
        .txtRead = mapMess.MsgNoteText
    End With
    grdMess.Columns("Read").Text = "X"
    
    frmRead.Show vbModal
    
    Exit Sub
RowERR:
    Debug.Print Err.Number, Err.Description
    Resume Next


End Sub




Private Sub CheckRS()
    ' Check a flag, gbRSalreadyPopulated before
    ' continuing. Clear the recordset before
    ' populating it again.
    If gbRSalreadyPopulated Then
        ClearRS
        PopulateRS
    Else
        PopulateRS
        gbRSalreadyPopulated = True
    End If
End Sub

Private Sub DoGrid()
    ' Check a flag, gbGridConfigured, before
    ' continuing. If already configured, clear it.
    If gbGridConfigured Then
        gbIgnoreEvent = True
        grdMess.HoldFields ' Retain grid configuration.
        Set grdMess.DataSource = rsUnread
        gbIgnoreEvent = False
    Else
        ConfigureGrid
    End If

End Sub

Private Sub ComposeMessage()
    On Error GoTo ComposeErr
    Dim strMessage As String
    ' Use the Compose method and then invoke the
    ' Send method. When the optional argument
    ' is set to True, the underlying mail system's
    ' form is used. Otherwise, you must create your
    ' own.
    mapMess.Compose
    mapMess.Send True
    
    Exit Sub
ComposeErr:
    Debug.Print Err.Number, Err.Description
    Resume Next
End Sub

Private Sub CreateRS()
    ' Create ADO recordset and add fields. Each field becomes a
    ' column in the DataGrid control.
    
    Set rsUnread = New ADODB.Recordset
    With rsUnread.Fields
        .Append "ID", adSmallInt
        .Append "Read", adBSTR
        .Append "Date Received", adDate
        .Append "From", adBSTR
        .Append "Subject", adBSTR
    End With
    rsUnread.Open
End Sub


Private Sub FetchUnreadOnly()
    With mapMess
         ' Fetch unread messages only, then display number
         ' of unread messages in Statusbar.
        .FetchUnreadOnly = True
        .Fetch
        sbrMapi.Panels("MsgCnt").Text = .MsgCount & " Messages"
    End With
End Sub

Private Sub ClearRS()
    ' Clear the recordset of all rows.
    If rsUnread.RecordCount = 0 Then Exit Sub
    Dim i As Integer
    gbIgnoreEvent = True
    
    rsUnread.MoveFirst
    For i = 1 To rsUnread.RecordCount
        rsUnread.Delete adAffectCurrent
        DoEvents
    Next i
    gbIgnoreEvent = False

End Sub
Private Sub PopulateRS()
    gbIgnoreEvent = True ' Flag to prevent RowColChanged event from processing.
    Dim i As Integer
    For i = 0 To mapMess.MsgCount - 1
        mapMess.MsgIndex = i
        rsUnread.AddNew
        rsUnread!ID = i
        rsUnread![date received] = mapMess.MsgDateReceived
        rsUnread!From = mapMess.MsgOrigDisplayName
        rsUnread!subject = mapMess.MsgSubject
    Next i
    gbIgnoreEvent = False ' Reset flag.
    
End Sub
Private Sub ConfigureGrid()
    ' Set the width of the grid columns before
    ' setting the DataSource to the recordset.
    
    gbIgnoreEvent = True
    With grdMess
        Set .DataSource = rsUnread ' Fires event.
        .Columns("ID").Width = 0   ' Hide ID column.
        .Columns("Read").Width = 500
        .Columns("Date Received").Width = 900
        .Columns("From").Width = 2000
        .Columns("Subject").Width = 5000
    End With
    Dim fmtdate As StdDataFormat
    
    ' Use the Format object to format the
    ' date column.
    Set fmtdate = New StdDataFormat
    With fmtdate
        .Type = fmtCustom
        .Format = "Short Date"
     End With
    Set grdMess. _
    Columns("Date Received").DataFormat = fmtdate
    
    gbIgnoreEvent = False
    gbGridConfigured = True ' Set flag so we know
    ' we don't have to do this again.
End Sub


Private Function LogOn() As Boolean
    ' Create Recordset Object named rsUnread
    CreateRS

    ' If a session is already started,
    ' exit sub.
    If mapSess.NewSession Then
        MsgBox "Session already established"
        Exit Function
    End If
    
    On Error GoTo errLogInFail
    With mapSess
        ' Set DownLoadMail to False to prevent immediate download.
        .DownLoadMail = False
        .LogonUI = True ' Use the underlying email system's logon UI.
        .SignOn ' Signon method.
        ' If successful, return True
        LogOn = True
        ' Set NewSession to True and set0
        ' variable flag to true
        .NewSession = True
        bNewSession = .NewSession
        mapMess.SessionID = .SessionID ' You must set this before continuing.
        sbrMapi.Panels("SessID") = "ID = " & .SessionID ' Just so you can see the SessionID.
    End With
    ' Enabled and disable buttons.
    ToggleButtonEnabled
    Exit Function
    
errLogInFail:
    Debug.Print Err.Number, Err.Description
    If Err.Number = 32003 Then
        MsgBox "Canceled Login"
        LogOn = False
    End If
    Exit Function
End Function
Private Sub LogOff()
    ' Logoff the MapSessions control.
    With mapSess
        .SignOff ' Close the session.
        .NewSession = False ' Flag for new session.
        bNewSession = .NewSession ' Reset flag.
    End With
    ' Disable and enable buttons.
    ToggleButtonEnabled
    
    rsUnread.Close ' Close ADO recordset and set variable to Nothing.
    Set rsUnread = Nothing
    gbRSalreadyPopulated = False
    Unload frmRead ' Unload the form.
    grdMess.ClearFields ' Clear the grid.
End Sub

Private Sub ToggleButtonEnabled()
    ' Toggle Enabled property of various buttons.
    With tbrMail
        .Buttons("LogOn").Enabled = Abs(.Buttons("LogOn").Enabled) - 1
        .Buttons("logOff").Enabled = Abs(.Buttons("logOff").Enabled) - 1
        .Buttons("fetch").Enabled = Abs(.Buttons("fetch").Enabled) - 1
        .Buttons("compose").Enabled = Abs(.Buttons("compose").Enabled) - 1
        .Buttons("address").Enabled = Abs(.Buttons("address").Enabled) - 1
        .Buttons("address").ButtonMenus(1).Enabled = Abs(.Buttons("address").ButtonMenus(1).Enabled) - 1
        .Buttons("address").ButtonMenus(2).Enabled = Abs(.Buttons("address").ButtonMenus(2).Enabled) - 1
        
    End With
    
    ' Toggle menu enabled.
    mnuLogOn.Enabled = Abs(mnuLogOn.Enabled) - 1
    mnuLogOff.Enabled = Abs(mnuLogOff.Enabled) - 1
    mnuTools.Enabled = Abs(mnuTools.Enabled) - 1
    mnuCheck.Enabled = Abs(mnuCheck.Enabled) - 1
    mnuAddress.Enabled = Abs(mnuAddress.Enabled) - 1

End Sub

Private Function GetList(ListType As Integer) As String
    ' The function just gets all the recipients
    ' of a message and contatenates them.
    Dim i As Integer
    Dim strList As String
    For i = 0 To mapMess.RecipCount - 1
        mapMess.RecipIndex = i
        If mapMess.RecipType = ListType Then
            strList = strList & mapMess.RecipDisplayName & "; "
        End If
    Next i
    If strList = "" Then
        GetList = ""
        Exit Function
    End If
    ' Strip semicolon from last recipient name.
    GetList = Left(strList, Len(strList) - 2)

End Function


Private Sub mnuAddress_Click()
    ' Display the Addressbook.
    On Error GoTo AddressErr
    mapMess.Show True
    Exit Sub
AddressErr:
    Debug.Print Err.Number, Err.Description
    Resume Next
End Sub

Private Sub mnuCheck_Click()
    FetchUnreadOnly ' Fetch unread messages.
    CheckRS ' Check Recordset and fill.
    DoGrid ' Set Grid's DataSource to recordset.
    
End Sub

Private Sub mnuExit_Click()
    ' Sign off if not done yet, then unload form.
    If mapSess.SessionID <> 0 Then mapSess.SignOff
    Unload Me
End Sub

Private Sub mnuLogOff_Click()
    LogOff
End Sub

Private Sub mnuLogOn_Click()
    If LogOn = True Then
        FetchUnreadOnly
        CheckRS
        DoGrid
    Else
        Exit Sub
    End If
End Sub

Private Sub tbrMail_ButtonMenuClick(ByVal ButtonMenu As MSComCtlLib.ButtonMenu)
    On Error GoTo btnClickErr
    Select Case ButtonMenu.Key
    Case "global"
        mapMess.Show False
    Case "recepient"
        mapMess.Show True
    End Select
    Exit Sub
btnClickErr:
    If Err.Number = mapUserAbort Then
        Resume Next
    Else
        MsgBox Err.Number & ": " & Err.Description
    End If
    
End Sub
