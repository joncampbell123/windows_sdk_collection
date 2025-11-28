VERSION 5.00
Object = "{648A5603-2C6E-101B-82B6-000000000014}#1.1#0"; "MSCOMM32.OCX"
Begin VB.Form DIALER 
   Caption         =   "MSComm Phone Dialer"
   ClientHeight    =   1545
   ClientLeft      =   4005
   ClientTop       =   3270
   ClientWidth     =   4275
   LinkTopic       =   "Form2"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1545
   ScaleWidth      =   4275
   WhatsThisHelp   =   -1  'True
   Begin MSCommLib.MSComm MSComm1 
      Left            =   0
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      DTREnable       =   -1  'True
   End
   Begin VB.CommandButton CancelButton 
      Caption         =   "Cancel"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   348
      Left            =   1680
      TabIndex        =   3
      Top             =   885
      Width           =   852
   End
   Begin VB.CommandButton QuitButton 
      Cancel          =   -1  'True
      Caption         =   "Quit"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   348
      Left            =   2640
      TabIndex        =   1
      Top             =   885
      Width           =   852
   End
   Begin VB.CommandButton DialButton 
      Caption         =   "Dial"
      Default         =   -1  'True
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   348
      Left            =   720
      TabIndex        =   0
      Top             =   885
      Width           =   852
   End
   Begin VB.Label Status 
      BorderStyle     =   1  'Fixed Single
      Caption         =   "To dial a number, click the Dial button"
      Height          =   255
      Left            =   720
      TabIndex        =   2
      Top             =   360
      Width           =   2775
   End
End
Attribute VB_Name = "DIALER"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'--------------------------------------------------------
'   DIALER.FRM
'   Copyright (c) 1994 Crescent Software, Inc.
'   by Carl Franklin
'
'   Updated by Anton de Jong
'
'   Demonstrates how to dial phone numbers with a modem.
'
'   For this program to work, your telephone and
'   modem must be connected to the same phone line.
'--------------------------------------------------------
Option Explicit

DefInt A-Z

' This flag is set when the user chooses Cancel.
Dim CancelFlag

Private Sub CancelButton_Click()
    ' CancelFlag tells the Dial procedure to exit.
    CancelFlag = True
    CancelButton.Enabled = False
End Sub

Private Sub Dial(Number$)
    Dim DialString$, FromModem$, dummy

    ' AT is the Hayes compatible ATTENTION command and is required to send commands to the modem.
    ' DT means "Dial Tone." The Dial command uses touch tones, as opposed to pulse (DP = Dial Pulse).
    ' Numbers$ is the phone number being dialed.
    ' A semicolon tells the modem to return to command mode after dialing (important).
    ' A carriage return, vbCr, is required when sending commands to the modem.
    DialString$ = "ATDT" + Number$ + ";" + vbCr

    ' Communications port settings.
    ' Assuming that a mouse is attached to COM1, CommPort is set to 2
    MSComm1.CommPort = 2
    MSComm1.Settings = "9600,N,8,1"
    
    ' Open the communications port.
    On Error Resume Next
    MSComm1.PortOpen = True
    If Err Then
       MsgBox "COM2: not available. Change the CommPort property to another port."
       Exit Sub
    End If
    
    ' Flush the input buffer.
    MSComm1.InBufferCount = 0
    
    ' Dial the number.
    MSComm1.Output = DialString$
    
    ' Wait for "OK" to come back from the modem.
    Do
       dummy = DoEvents()
       ' If there is data in the buffer, then read it.
       If MSComm1.InBufferCount Then
          FromModem$ = FromModem$ + MSComm1.Input
          ' Check for "OK".
          If InStr(FromModem$, "OK") Then
             ' Notify the user to pick up the phone.
             Beep
             MsgBox "Please pick up the phone and either press Enter or click OK"
             Exit Do
          End If
       End If
        
       ' Did the user choose Cancel?
       If CancelFlag Then
          CancelFlag = False
          Exit Do
       End If
    Loop
    
    ' Disconnect the modem.
    MSComm1.Output = "ATH" + vbCr
    
    ' Close the port.
    MSComm1.PortOpen = False
End Sub

Private Sub DialButton_Click()
    Dim Number$, Temp$
    
    DialButton.Enabled = False
    QuitButton.Enabled = False
    CancelButton.Enabled = True
    
    ' Get the number to dial.
    Number$ = InputBox$("Enter phone number:", Number$)
        If Number$ = "" Then Exit Sub
    Temp$ = Status
    Status = "Dialing - " + Number$
    
    ' Dial the selected phone number.
    Dial Number$

    DialButton.Enabled = True
    QuitButton.Enabled = True
    CancelButton.Enabled = False

    Status = Temp$
End Sub

Private Sub Form_Load()
    ' Setting InputLen to 0 tells MSComm to read the entire
    ' contents of the input buffer when the Input property
    ' is used.
    MSComm1.InputLen = 0
    
End Sub

Private Sub QuitButton_Click()
    End
End Sub

