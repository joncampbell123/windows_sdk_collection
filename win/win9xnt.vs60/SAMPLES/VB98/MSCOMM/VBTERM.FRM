VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.0#0"; "comdlg32.ocx"
Object = "{648A5603-2C6E-101B-82B6-000000000014}#1.0#0"; "mscomm32.ocx"
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#1.0#0"; "comctl32.ocx"
Begin VB.Form frmTerminal 
   Caption         =   "Visual Basic Terminal"
   ClientHeight    =   4935
   ClientLeft      =   2940
   ClientTop       =   2055
   ClientWidth     =   7155
   ForeColor       =   &H00000000&
   Icon            =   "vbterm.frx":0000
   LinkMode        =   1  'Source
   LinkTopic       =   "Form1"
   ScaleHeight     =   4935
   ScaleWidth      =   7155
   Begin VB.Timer Timer2 
      Enabled         =   0   'False
      Interval        =   2000
      Left            =   210
      Top             =   3645
   End
   Begin VB.TextBox txtTerm 
      Height          =   3690
      Left            =   1245
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   3
      Top             =   1140
      Width           =   5790
   End
   Begin MSComCtlLib.Toolbar tbrToolBar 
      Align           =   1  'Align Top
      Height          =   390
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   7155
      _ExtentX        =   12621
      _ExtentY        =   688
      ButtonWidth     =   609
      ButtonHeight    =   582
      ImageList       =   "ImageList1"
      BeginProperty Buttons {0713E452-850A-101B-AFC0-4210102A8DA7} 
         NumButtons      =   10
         BeginProperty Button1 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Style           =   3
            Value           =   1
            MixedState      =   -1  'True
         EndProperty
         BeginProperty Button2 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Key             =   "OpenLogFile"
            Description     =   "Open Log File..."
            Object.ToolTipText     =   "Open Log File..."
            ImageIndex      =   1
         EndProperty
         BeginProperty Button3 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Enabled         =   0   'False
            Key             =   "CloseLogFile"
            Description     =   "Close Log File"
            Object.ToolTipText     =   "Close Log File"
            ImageIndex      =   2
         EndProperty
         BeginProperty Button4 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Style           =   3
            Value           =   1
            MixedState      =   -1  'True
         EndProperty
         BeginProperty Button5 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Key             =   "DialPhoneNumber"
            Description     =   "Dial Phone Number..."
            Object.ToolTipText     =   "Dial Phone Number..."
            ImageIndex      =   3
         EndProperty
         BeginProperty Button6 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Enabled         =   0   'False
            Key             =   "HangUpPhone"
            Description     =   "Hang Up Phone"
            Object.ToolTipText     =   "Hang Up Phone"
            ImageIndex      =   4
         EndProperty
         BeginProperty Button7 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Style           =   3
            Value           =   1
            MixedState      =   -1  'True
         EndProperty
         BeginProperty Button8 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Key             =   "Properties"
            Description     =   "Properties..."
            Object.ToolTipText     =   "Properties..."
            ImageIndex      =   5
         EndProperty
         BeginProperty Button9 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Style           =   3
            Value           =   1
            MixedState      =   -1  'True
         EndProperty
         BeginProperty Button10 {0713F354-850A-101B-AFC0-4210102A8DA7} 
            Enabled         =   0   'False
            Key             =   "TransmitTextFile"
            Description     =   "Transmit Text File..."
            Object.ToolTipText     =   "Transmit Text File..."
            ImageIndex      =   6
         EndProperty
      EndProperty
      MouseIcon       =   "vbterm.frx":030A
      Begin VB.Frame Frame1 
         BorderStyle     =   0  'None
         Caption         =   "Frame1"
         Height          =   240
         Left            =   4000
         TabIndex        =   2
         Top             =   75
         Width           =   240
         Begin VB.Image imgConnected 
            Height          =   240
            Left            =   0
            Picture         =   "vbterm.frx":0326
            Stretch         =   -1  'True
            ToolTipText     =   "Toggles Port"
            Top             =   0
            Width           =   240
         End
         Begin VB.Image imgNotConnected 
            Height          =   240
            Left            =   0
            Picture         =   "vbterm.frx":0470
            Stretch         =   -1  'True
            ToolTipText     =   "Toggles Port"
            Top             =   0
            Width           =   240
         End
      End
   End
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   1000
      Left            =   165
      Top             =   1815
   End
   Begin MSCommLib.MSComm MSComm1 
      Left            =   45
      Top             =   510
      _ExtentX        =   1005
      _ExtentY        =   1005
      DTREnable       =   -1  'True
      NullDiscard     =   -1  'True
      RThreshold      =   1
      RTSEnable       =   -1  'True
      SThreshold      =   1
      InputMode       =   1
   End
   Begin MSComDlg.CommonDialog OpenLog 
      Left            =   105
      Top             =   1170
      _ExtentX        =   847
      _ExtentY        =   847
      DefaultExt      =   "LOG"
      FileName        =   "Open Communications Log File"
      Filter          =   "Log File (*.log)|*.log;"
      FilterIndex     =   501
      FontSize        =   9.02458e-38
   End
   Begin MSComCtlLib.StatusBar sbrStatus 
      Align           =   2  'Align Bottom
      Height          =   315
      Left            =   0
      TabIndex        =   0
      Top             =   4620
      Width           =   7155
      _ExtentX        =   12621
      _ExtentY        =   556
      BeginProperty Panels {0713E89E-850A-101B-AFC0-4210102A8DA7} 
         NumPanels       =   3
         BeginProperty Panel1 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   2
            Object.Width           =   2540
            MinWidth        =   2540
            Text            =   "Status:"
            TextSave        =   "Status:"
            Key             =   "Status"
            Object.ToolTipText     =   "Communications Port Status"
         EndProperty
         BeginProperty Panel2 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   1
            Object.Width           =   8310
            MinWidth        =   2
            Text            =   "Settings:"
            TextSave        =   "Settings:"
            Key             =   "Settings"
            Object.ToolTipText     =   "Communications Port Settings"
         EndProperty
         BeginProperty Panel3 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   2
            Object.Width           =   1244
            MinWidth        =   1244
            Key             =   "ConnectTime"
            Object.ToolTipText     =   "Connect Time"
         EndProperty
      EndProperty
      MouseIcon       =   "vbterm.frx":05BA
   End
   Begin MSComCtlLib.ImageList ImageList1 
      Left            =   165
      Top             =   2445
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      BeginProperty Images {0713E8C2-850A-101B-AFC0-4210102A8DA7} 
         NumListImages   =   6
         BeginProperty ListImage1 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbterm.frx":05D6
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbterm.frx":08F0
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbterm.frx":0C0A
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbterm.frx":0F24
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbterm.frx":123E
            Key             =   ""
         EndProperty
         BeginProperty ListImage6 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbterm.frx":1558
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuOpenLog 
         Caption         =   "&Open Log File..."
      End
      Begin VB.Menu mnuCloseLog 
         Caption         =   "&Close Log File"
         Enabled         =   0   'False
      End
      Begin VB.Menu M3 
         Caption         =   "-"
      End
      Begin VB.Menu mnuSendText 
         Caption         =   "&Transmit Text File..."
         Enabled         =   0   'False
      End
      Begin VB.Menu Bar2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuPort 
      Caption         =   "&CommPort"
      Begin VB.Menu mnuOpen 
         Caption         =   "Port &Open"
      End
      Begin VB.Menu MBar1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuProperties 
         Caption         =   "Properties..."
      End
   End
   Begin VB.Menu mnuMSComm 
      Caption         =   "&MSComm"
      Begin VB.Menu mnuInputLen 
         Caption         =   "&InputLen..."
      End
      Begin VB.Menu mnuRThreshold 
         Caption         =   "&RThreshold..."
      End
      Begin VB.Menu mnuSThreshold 
         Caption         =   "&SThreshold..."
      End
      Begin VB.Menu mnuParRep 
         Caption         =   "P&arityReplace..."
      End
      Begin VB.Menu mnuDTREnable 
         Caption         =   "&DTREnable"
      End
      Begin VB.Menu Bar3 
         Caption         =   "-"
      End
      Begin VB.Menu mnuHCD 
         Caption         =   "&CDHolding..."
      End
      Begin VB.Menu mnuHCTS 
         Caption         =   "CTSH&olding..."
      End
      Begin VB.Menu mnuHDSR 
         Caption         =   "DSRHo&lding..."
      End
   End
   Begin VB.Menu mnuCall 
      Caption         =   "C&all"
      Begin VB.Menu mnuDial 
         Caption         =   "&Dial Phone Number..."
      End
      Begin VB.Menu mnuHangUp 
         Caption         =   "&Hang Up Phone"
         Enabled         =   0   'False
      End
   End
End
Attribute VB_Name = "frmTerminal"
Attribute VB_Base = "0{3E2D207C-D67B-11CF-9BF3-00AA002FFD8F}"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'--------------------------------------------------
' VBTerm - This is a demonstration program for the MSComm
' communications ActiveX control.
'
' Copyright (c) 1994, Crescent Software, Inc.
' by Don Malin and Carl Franklin.
'
' Updated by Mike Maddox
'--------------------------------------------------
Option Explicit
                        
Dim Ret As Integer      ' Scratch integer.
Dim Temp As String      ' Scratch string.
Dim hLogFile As Integer ' Handle of open log file.
Dim StartTime As Date   ' Stores starting time for port timer

Private Sub Form_Load()
    Dim CommPort As String, Handshaking As String, Settings As String
        
    On Error Resume Next
    
    ' Set the default color for the terminal
    txtTerm.SelLength = Len(txtTerm)
    txtTerm.SelText = ""
    txtTerm.ForeColor = vbBlue
       
    ' Set Title
    App.Title = "Visual Basic Terminal"
    
    ' Set up status indicator light
    imgNotConnected.ZOrder
       
    ' Center Form
    frmTerminal.Move (Screen.Width - Width) / 2, (Screen.Height - Height) / 2
    
    ' Load Registry Settings
    
    Settings = GetSetting(App.Title, "Properties", "Settings", "") ' frmTerminal.MSComm1.Settings]\
    If Settings <> "" Then
        MSComm1.Settings = Settings
        If Err Then
            MsgBox Error$, 48
            Exit Sub
        End If
    End If
    
    CommPort = GetSetting(App.Title, "Properties", "CommPort", "") ' frmTerminal.MSComm1.CommPort
    If CommPort <> "" Then MSComm1.CommPort = CommPort
    
    Handshaking = GetSetting(App.Title, "Properties", "Handshaking", "") 'frmTerminal.MSComm1.Handshaking
    If Handshaking <> "" Then
        MSComm1.Handshaking = Handshaking
        If Err Then
            MsgBox Error$, 48
            Exit Sub
        End If
    End If
    
    Echo = GetSetting(App.Title, "Properties", "Echo", "") ' Echo
    On Error GoTo 0

End Sub

Private Sub Form_Resize()
   ' Resize the Term (display) control
   txtTerm.Move 0, tbrToolBar.Height, frmTerminal.ScaleWidth, frmTerminal.ScaleHeight - sbrStatus.Height - tbrToolBar.Height
   
   ' Position the status indicator light
   Frame1.Left = ScaleWidth - Frame1.Width * 1.5
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim Counter As Long

    If MSComm1.PortOpen Then
       ' Wait 10 seconds for data to be transmitted.
       Counter = Timer + 10
       Do While MSComm1.OutBufferCount
          Ret = DoEvents()
          If Timer > Counter Then
             Select Case MsgBox("Data cannot be sent", 34)
                ' Cancel.
                Case 3
                   Cancel = True
                   Exit Sub
                ' Retry.
                Case 4
                   Counter = Timer + 10
                ' Ignore.
                Case 5
                   Exit Do
             End Select
          End If
       Loop

       MSComm1.PortOpen = 0
    End If

    ' If the log file is open, flush and close it.
    If hLogFile Then mnuCloseLog_Click
    End
End Sub

Private Sub imgConnected_Click()
    ' Call the mnuOpen_Click routine to toggle connect and disconnect
    Call mnuOpen_Click
End Sub

Private Sub imgNotConnected_Click()
    ' Call the mnuOpen_Click routine to toggle connect and disconnect
    Call mnuOpen_Click
End Sub

Private Sub mnuCloseLog_Click()
    ' Close the log file.
    Close hLogFile
    hLogFile = 0
    mnuOpenLog.Enabled = True
    tbrToolBar.Buttons("OpenLogFile").Enabled = True
    mnuCloseLog.Enabled = False
    tbrToolBar.Buttons("CloseLogFile").Enabled = False
    frmTerminal.Caption = "Visual Basic Terminal"
End Sub

Private Sub mnuDial_Click()
    On Local Error Resume Next
    Static Num As String
    
    Num = "1-206-936-6735" ' This is the MSDN phone number
    
    ' Get a number from the user.
    Num = InputBox$("Enter Phone Number:", "Dial Number", Num)
    If Num = "" Then Exit Sub
    
    ' Open the port if it isn't already open.
    If Not MSComm1.PortOpen Then
       mnuOpen_Click
       If Err Then Exit Sub
    End If
      
    ' Enable hang up button and menu item
    mnuHangUp.Enabled = True
    tbrToolBar.Buttons("HangUpPhone").Enabled = True
              
    ' Dial the number.
    MSComm1.Output = "ATDT" & Num & vbCrLf
    
    ' Start the port timer
    StartTiming
End Sub

' Toggle the DTREnabled property.
Private Sub mnuDTREnable_Click()
    ' Toggle DTREnable property
    MSComm1.DTREnable = Not MSComm1.DTREnable
    mnuDTREnable.Checked = MSComm1.DTREnable
End Sub


Private Sub mnuFileExit_Click()
    ' Use Form_Unload since it has code to check for unsent data and an open log file.
    Form_Unload Ret
End Sub



' Toggle the DTREnable property to hang up the line.
Private Sub mnuHangup_Click()
    On Error Resume Next
    
    MSComm1.Output = "ATH"      ' Send hangup string
    Ret = MSComm1.DTREnable     ' Save the current setting.
    MSComm1.DTREnable = True    ' Turn DTR on.
    MSComm1.DTREnable = False   ' Turn DTR off.
    MSComm1.DTREnable = Ret     ' Restore the old setting.
    mnuHangUp.Enabled = False
    tbrToolBar.Buttons("HangUpPhone").Enabled = False
    
    ' If port is actually still open, then close it
    If MSComm1.PortOpen Then MSComm1.PortOpen = False
    
    ' Notify user of error
    If Err Then MsgBox Error$, 48
    
    mnuSendText.Enabled = False
    tbrToolBar.Buttons("TransmitTextFile").Enabled = False
    mnuHangUp.Enabled = False
    tbrToolBar.Buttons("HangUpPhone").Enabled = False
    mnuDial.Enabled = True
    tbrToolBar.Buttons("DialPhoneNumber").Enabled = True
    sbrStatus.Panels("Settings").Text = "Settings: "
    
    ' Turn off indicator light and uncheck open menu
    mnuOpen.Checked = False
    imgNotConnected.ZOrder
            
    ' Stop the port timer
    StopTiming
    sbrStatus.Panels("Status").Text = "Status: "
    On Error GoTo 0
End Sub

' Display the value of the CDHolding property.
Private Sub mnuHCD_Click()
    If MSComm1.CDHolding Then
        Temp = "True"
    Else
        Temp = "False"
    End If
    MsgBox "CDHolding = " + Temp
End Sub

' Display the value of the CTSHolding property.
Private Sub mnuHCTS_Click()
    If MSComm1.CTSHolding Then
        Temp = "True"
    Else
        Temp = "False"
    End If
    MsgBox "CTSHolding = " + Temp
End Sub

' Display the value of the DSRHolding property.
Private Sub mnuHDSR_Click()
    If MSComm1.DSRHolding Then
        Temp = "True"
    Else
        Temp = "False"
    End If
    MsgBox "DSRHolding = " + Temp
End Sub

' This procedure sets the InputLen property, which determines how
' many bytes of data are read each time Input is used
' to retreive data from the input buffer.
' Setting InputLen to 0 specifies that
' the entire contents of the buffer should be read.
Private Sub mnuInputLen_Click()
    On Error Resume Next

    Temp = InputBox$("Enter New InputLen:", "InputLen", Str$(MSComm1.InputLen))
    If Len(Temp) Then
        MSComm1.InputLen = Val(Temp)
        If Err Then MsgBox Error$, 48
    End If
End Sub

Private Sub mnuProperties_Click()
  ' Show the CommPort properties form
  frmProperties.Show vbModal
  
End Sub

' Toggles the state of the port (open or closed).
Private Sub mnuOpen_Click()
    On Error Resume Next
    Dim OpenFlag

    MSComm1.PortOpen = Not MSComm1.PortOpen
    If Err Then MsgBox Error$, 48
    
    OpenFlag = MSComm1.PortOpen
    
    mnuOpen.Checked = OpenFlag
    mnuSendText.Enabled = OpenFlag
    tbrToolBar.Buttons("TransmitTextFile").Enabled = OpenFlag
        
    If MSComm1.PortOpen Then
        ' Enable dial button and menu item
        mnuDial.Enabled = True
        tbrToolBar.Buttons("DialPhoneNumber").Enabled = True
        
        ' Enable hang up button and menu item
        mnuHangUp.Enabled = True
        tbrToolBar.Buttons("HangUpPhone").Enabled = True
        
        imgConnected.ZOrder
        sbrStatus.Panels("Settings").Text = "Settings: " & MSComm1.Settings
        StartTiming
    Else
        ' Enable dial button and menu item
        mnuDial.Enabled = True
        tbrToolBar.Buttons("DialPhoneNumber").Enabled = True
        
        ' Disable hang up button and menu item
        mnuHangUp.Enabled = False
        tbrToolBar.Buttons("HangUpPhone").Enabled = False
        
        imgNotConnected.ZOrder
        sbrStatus.Panels("Settings").Text = "Settings: "
        StopTiming
    End If
    
End Sub

Private Sub mnuOpenLog_Click()
   Dim replace
   On Error Resume Next
   OpenLog.Flags = cdlOFNHideReadOnly Or cdlOFNExplorer
   OpenLog.CancelError = True
      
   ' Get the log filename from the user.
   OpenLog.DialogTitle = "Open Communications Log File"
   OpenLog.Filter = "Log Files (*.LOG)|*.log|All Files (*.*)|*.*"
   
   Do
      OpenLog.Filename = ""
      OpenLog.ShowOpen
      If Err = cdlCancel Then Exit Sub
      Temp = OpenLog.Filename

      ' If the file already exists, ask if the user wants to overwrite the file or add to it.
      Ret = Len(Dir$(Temp))
      If Err Then
         MsgBox Error$, 48
         Exit Sub
      End If
      If Ret Then
         replace = MsgBox("Replace existing file - " + Temp + "?", 35)
      Else
         replace = 0
      End If
   Loop While replace = 2

   ' User clicked the Yes button, so delete the file.
   If replace = 6 Then
      Kill Temp
      If Err Then
         MsgBox Error$, 48
         Exit Sub
      End If
   End If

   ' Open the log file.
   hLogFile = FreeFile
   Open Temp For Binary Access Write As hLogFile
   If Err Then
      MsgBox Error$, 48
      Close hLogFile
      hLogFile = 0
      Exit Sub
   Else
      ' Go to the end of the file so that new data can be appended.
      Seek hLogFile, LOF(hLogFile) + 1
   End If

   frmTerminal.Caption = "Visual Basic Terminal - " + OpenLog.FileTitle
   mnuOpenLog.Enabled = False
   tbrToolBar.Buttons("OpenLogFile").Enabled = False
   mnuCloseLog.Enabled = True
   tbrToolBar.Buttons("CloseLogFile").Enabled = True
End Sub

' This procedure sets the ParityReplace property, which holds the
' character that will replace any incorrect characters
' that are received because of a parity error.
Private Sub mnuParRep_Click()
    On Error Resume Next

    Temp = InputBox$("Enter Replace Character", "ParityReplace", frmTerminal.MSComm1.ParityReplace)
    frmTerminal.MSComm1.ParityReplace = Left$(Temp, 1)
    If Err Then MsgBox Error$, 48
End Sub

' This procedure sets the RThreshold property, which determines
' how many bytes can arrive at the receive buffer before the OnComm
' event is triggered and the CommEvent property is set to comEvReceive.
Private Sub mnuRThreshold_Click()
    On Error Resume Next
    
    Temp = InputBox$("Enter New RThreshold:", "RThreshold", Str$(MSComm1.RThreshold))
    If Len(Temp) Then
        MSComm1.RThreshold = Val(Temp)
        If Err Then MsgBox Error$, 48
    End If

End Sub




' The OnComm event is used for trapping communications events and errors.
Private Static Sub MSComm1_OnComm()
    Dim EVMsg$
    Dim ERMsg$
    
    ' Branch according to the CommEvent property.
    Select Case MSComm1.CommEvent
        ' Event messages.
        Case comEvReceive
            Dim Buffer As Variant
            Buffer = MSComm1.Input
            Debug.Print "Receive - " & StrConv(Buffer, vbUnicode)
            ShowData txtTerm, (StrConv(Buffer, vbUnicode))
        Case comEvSend
        Case comEvCTS
            EVMsg$ = "Change in CTS Detected"
        Case comEvDSR
            EVMsg$ = "Change in DSR Detected"
        Case comEvCD
            EVMsg$ = "Change in CD Detected"
        Case comEvRing
            EVMsg$ = "The Phone is Ringing"
        Case comEvEOF
            EVMsg$ = "End of File Detected"

        ' Error messages.
        Case comBreak
            ERMsg$ = "Break Received"
        Case comCDTO
            ERMsg$ = "Carrier Detect Timeout"
        Case comCTSTO
            ERMsg$ = "CTS Timeout"
        Case comDCB
            ERMsg$ = "Error retrieving DCB"
        Case comDSRTO
            ERMsg$ = "DSR Timeout"
        Case comFrame
            ERMsg$ = "Framing Error"
        Case comOverrun
            ERMsg$ = "Overrun Error"
        Case comRxOver
            ERMsg$ = "Receive Buffer Overflow"
        Case comRxParity
            ERMsg$ = "Parity Error"
        Case comTxFull
            ERMsg$ = "Transmit Buffer Full"
        Case Else
            ERMsg$ = "Unknown error or event"
    End Select
    
    If Len(EVMsg$) Then
        ' Display event messages in the status bar.
        sbrStatus.Panels("Status").Text = "Status: " & EVMsg$
                
        ' Enable timer so that the message in the status bar
        ' is cleared after 2 seconds
        Timer2.Enabled = True
        
    ElseIf Len(ERMsg$) Then
        ' Display event messages in the status bar.
        sbrStatus.Panels("Status").Text = "Status: " & ERMsg$
        
        ' Display error messages in an alert message box.
        Beep
        Ret = MsgBox(ERMsg$, 1, "Click Cancel to quit, OK to ignore.")
        
        ' If the user clicks Cancel (2)...
        If Ret = 2 Then
            MSComm1.PortOpen = False    ' Close the port and quit.
        End If
        
        ' Enable timer so that the message in the status bar
        ' is cleared after 2 seconds
        Timer2.Enabled = True
    End If
End Sub

Private Sub mnuSendText_Click()
   Dim hSend, BSize, LF&
   
   On Error Resume Next
   
   mnuSendText.Enabled = False
   tbrToolBar.Buttons("TransmitTextFile").Enabled = False
   
   ' Get the text filename from the user.
   OpenLog.DialogTitle = "Send Text File"
   OpenLog.Filter = "Text Files (*.TXT)|*.txt|All Files (*.*)|*.*"
   Do
      OpenLog.CancelError = True
      OpenLog.Filename = ""
      OpenLog.ShowOpen
      If Err = cdlCancel Then
        mnuSendText.Enabled = True
        tbrToolBar.Buttons("TransmitTextFile").Enabled = True
        Exit Sub
      End If
      Temp = OpenLog.Filename

      ' If the file doesn't exist, go back.
      Ret = Len(Dir$(Temp))
      If Err Then
         MsgBox Error$, 48
         mnuSendText.Enabled = True
         tbrToolBar.Buttons("TransmitTextFile").Enabled = True
         Exit Sub
      End If
      If Ret Then
         Exit Do
      Else
         MsgBox Temp + " not found!", 48
      End If
   Loop

   ' Open the log file.
   hSend = FreeFile
   Open Temp For Binary Access Read As hSend
   If Err Then
      MsgBox Error$, 48
   Else
      ' Display the Cancel dialog box.
      CancelSend = False
      frmCancelSend.Label1.Caption = "Transmitting Text File - " + Temp
      frmCancelSend.Show
      
      ' Read the file in blocks the size of the transmit buffer.
      BSize = MSComm1.OutBufferSize
      LF& = LOF(hSend)
      Do Until EOF(hSend) Or CancelSend
         ' Don't read too much at the end.
         If LF& - Loc(hSend) <= BSize Then
            BSize = LF& - Loc(hSend) + 1
         End If
      
         ' Read a block of data.
         Temp = Space$(BSize)
         Get hSend, , Temp
      
         ' Transmit the block.
         MSComm1.Output = Temp
         If Err Then
            MsgBox Error$, 48
            Exit Do
         End If
      
         ' Wait for all the data to be sent.
         Do
            Ret = DoEvents()
         Loop Until MSComm1.OutBufferCount = 0 Or CancelSend
      Loop
   End If
   
   Close hSend
   mnuSendText.Enabled = True
   tbrToolBar.Buttons("TransmitTextFile").Enabled = True
   CancelSend = True
   frmCancelSend.Hide
End Sub


' This procedure sets the SThreshold property, which determines
' how many characters (at most) have to be waiting
' in the output buffer before the CommEvent property
' is set to comEvSend and the OnComm event is triggered.
Private Sub mnuSThreshold_Click()
    On Error Resume Next
    
    Temp = InputBox$("Enter New SThreshold Value", "SThreshold", Str$(MSComm1.SThreshold))
    If Len(Temp) Then
        MSComm1.SThreshold = Val(Temp)
        If Err Then MsgBox Error$, 48
    End If
End Sub

' This procedure adds data to the Term control's Text property.
' It also filters control characters, such as BACKSPACE,
' carriage return, and line feeds, and writes data to
' an open log file.
' BACKSPACE characters delete the character to the left,
' either in the Text property, or the passed string.
' Line feed characters are appended to all carriage
' returns.  The size of the Term control's Text
' property is also monitored so that it never
' exceeds MAXTERMSIZE characters.
Private Static Sub ShowData(Term As Control, Data As String)
    On Error GoTo Handler
    Const MAXTERMSIZE = 16000
    Dim TermSize As Long, i
    
    ' Make sure the existing text doesn't get too large.
    TermSize = Len(Term.Text)
    If TermSize > MAXTERMSIZE Then
       Term.Text = Mid$(Term.Text, 4097)
       TermSize = Len(Term.Text)
    End If

    ' Point to the end of Term's data.
    Term.SelStart = TermSize

    ' Filter/handle BACKSPACE characters.
    Do
       i = InStr(Data, Chr$(8))
       If i Then
          If i = 1 Then
             Term.SelStart = TermSize - 1
             Term.SelLength = 1
             Data = Mid$(Data, i + 1)
          Else
             Data = Left$(Data, i - 2) & Mid$(Data, i + 1)
          End If
       End If
    Loop While i

    ' Eliminate line feeds.
    Do
       i = InStr(Data, Chr$(10))
       If i Then
          Data = Left$(Data, i - 1) & Mid$(Data, i + 1)
       End If
    Loop While i

    ' Make sure all carriage returns have a line feed.
    i = 1
    Do
       i = InStr(i, Data, Chr$(13))
       If i Then
          Data = Left$(Data, i) & Chr$(10) & Mid$(Data, i + 1)
          i = i + 1
       End If
    Loop While i

    ' Add the filtered data to the SelText property.
    Term.SelText = Data
  
    ' Log data to file if requested.
    If hLogFile Then
       i = 2
       Do
          Err = 0
          Put hLogFile, , Data
          If Err Then
             i = MsgBox(Error$, 21)
             If i = 2 Then
                mnuCloseLog_Click
             End If
          End If
       Loop While i <> 2
    End If
    Term.SelStart = Len(Term.Text)
Exit Sub

Handler:
    MsgBox Error$
    Resume Next
End Sub

Private Sub Timer2_Timer()
sbrStatus.Panels("Status").Text = "Status: "
Timer2.Enabled = False

End Sub

' Keystrokes trapped here are sent to the MSComm
' control where they are echoed back via the
' OnComm (comEvReceive) event, and displayed
' with the ShowData procedure.
Private Sub txtTerm_KeyPress(KeyAscii As Integer)
    ' If the port is opened...
    If MSComm1.PortOpen Then
        ' Send the keystroke to the port.
        MSComm1.Output = Chr$(KeyAscii)
        
        ' Unless Echo is on, there is no need to
        ' let the text control display the key.
        ' A modem usually echos back a character
        If Not Echo Then
            ' Place position at end of terminal
            txtTerm.SelStart = Len(txtTerm)
            KeyAscii = 0
        End If
    End If
     
End Sub




Private Sub tbrToolBar_ButtonClick(ByVal Button As MSComCtlLib.Button)
Select Case Button.Key
Case "OpenLogFile"
    Call mnuOpenLog_Click
Case "CloseLogFile"
    Call mnuCloseLog_Click
Case "DialPhoneNumber"
    Call mnuDial_Click
Case "HangUpPhone"
    Call mnuHangup_Click
Case "Properties"
    Call mnuProperties_Click
Case "TransmitTextFile"
    Call mnuSendText_Click
End Select
End Sub

Private Sub Timer1_Timer()
    ' Display the Connect Time
    sbrStatus.Panels("ConnectTime").Text = Format(Now - StartTime, "hh:nn:ss") & " "
End Sub
' Call this function to start the Connect Time timer
Private Sub StartTiming()
    StartTime = Now
    Timer1.Enabled = True
End Sub
' Call this function to stop timing
Private Sub StopTiming()
    Timer1.Enabled = False
    sbrStatus.Panels("ConnectTime").Text = ""
End Sub
