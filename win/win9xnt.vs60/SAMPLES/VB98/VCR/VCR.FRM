VERSION 5.00
Begin VB.Form frmVCR 
   BackColor       =   &H00000000&
   Caption         =   "VBTV"
   ClientHeight    =   7125
   ClientLeft      =   1170
   ClientTop       =   1470
   ClientWidth     =   7110
   FillStyle       =   0  'Solid
   Icon            =   "vcr.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   7125
   ScaleWidth      =   7110
   Begin VB.CommandButton cmdSet 
      Caption         =   "Set"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   360
      Left            =   4560
      MaskColor       =   &H00000000&
      TabIndex        =   13
      ToolTipText     =   "Set the Timer for Recording"
      Top             =   6000
      Width           =   855
   End
   Begin VB.Timer tmr2 
      Enabled         =   0   'False
      Left            =   6240
      Top             =   3240
   End
   Begin VB.Timer tmr1 
      Interval        =   65535
      Left            =   6240
      Top             =   2640
   End
   Begin VB.CommandButton cmdDown 
      Caption         =   "Down"
      Height          =   325
      Left            =   6120
      MaskColor       =   &H00000000&
      TabIndex        =   11
      ToolTipText     =   "Channel selector"
      Top             =   1440
      Width           =   735
   End
   Begin VB.CommandButton cmdUp 
      Caption         =   "Up"
      Height          =   325
      Left            =   6120
      MaskColor       =   &H00000000&
      TabIndex        =   10
      ToolTipText     =   "Channel selector"
      Top             =   1080
      Width           =   735
   End
   Begin VB.CommandButton cmdExit 
      Cancel          =   -1  'True
      Caption         =   "Eject"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   360
      Left            =   5760
      MaskColor       =   &H00000000&
      TabIndex        =   7
      ToolTipText     =   "Exit VBTV"
      Top             =   6000
      Width           =   1095
   End
   Begin VB.CommandButton cmdPause 
      Caption         =   "ll"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   360
      Left            =   1005
      MaskColor       =   &H00000000&
      TabIndex        =   6
      ToolTipText     =   "Pause"
      Top             =   6000
      Width           =   615
   End
   Begin VB.CommandButton cmdRec 
      Caption         =   "Rec"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   360
      Left            =   1650
      MaskColor       =   &H00000000&
      TabIndex        =   5
      ToolTipText     =   "Record"
      Top             =   6000
      Width           =   615
   End
   Begin VB.CommandButton cmdForward 
      Caption         =   ">>"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   360
      Left            =   2955
      MaskColor       =   &H00000000&
      TabIndex        =   4
      ToolTipText     =   "Fast Forward"
      Top             =   6000
      Width           =   615
   End
   Begin VB.CommandButton cmdRewind 
      Caption         =   "<<"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   360
      Left            =   2310
      MaskColor       =   &H00000000&
      TabIndex        =   3
      ToolTipText     =   "Rewind"
      Top             =   6000
      Width           =   615
   End
   Begin VB.CommandButton cmdStop 
      Caption         =   "Stop"
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
      Height          =   360
      Left            =   3600
      MaskColor       =   &H00000000&
      TabIndex        =   2
      ToolTipText     =   "Stop"
      Top             =   6000
      Width           =   615
   End
   Begin VB.PictureBox picTV 
      AutoRedraw      =   -1  'True
      BackColor       =   &H00C0C0C0&
      BorderStyle     =   0  'None
      FillStyle       =   2  'Horizontal Line
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   18
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   4215
      Left            =   360
      ScaleHeight     =   4215
      ScaleWidth      =   5535
      TabIndex        =   1
      Top             =   240
      Width           =   5535
   End
   Begin VB.CommandButton cmdPlay 
      Caption         =   ">"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   360
      Left            =   360
      MaskColor       =   &H00000000&
      TabIndex        =   0
      ToolTipText     =   "Play"
      Top             =   6000
      Width           =   615
   End
   Begin VB.Line Line2 
      BorderColor     =   &H00808080&
      X1              =   0
      X2              =   7080
      Y1              =   4660
      Y2              =   4660
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00FFFFFF&
      X1              =   0
      X2              =   7080
      Y1              =   4700
      Y2              =   4700
   End
   Begin VB.Shape shpPlay 
      FillColor       =   &H0000FF00&
      FillStyle       =   0  'Solid
      Height          =   105
      Left            =   615
      Shape           =   3  'Circle
      Top             =   5835
      Visible         =   0   'False
      Width           =   105
   End
   Begin VB.Shape shpForward 
      FillColor       =   &H0000FF00&
      FillStyle       =   0  'Solid
      Height          =   105
      Left            =   3210
      Shape           =   3  'Circle
      Top             =   5835
      Visible         =   0   'False
      Width           =   105
   End
   Begin VB.Shape shpRewind 
      FillColor       =   &H0000FF00&
      FillStyle       =   0  'Solid
      Height          =   105
      Left            =   2565
      Shape           =   3  'Circle
      Top             =   5835
      Visible         =   0   'False
      Width           =   105
   End
   Begin VB.Shape shpRec 
      FillColor       =   &H000000FF&
      FillStyle       =   0  'Solid
      Height          =   105
      Left            =   1905
      Shape           =   3  'Circle
      Top             =   5835
      Visible         =   0   'False
      Width           =   105
   End
   Begin VB.Shape shpPause 
      FillColor       =   &H0000FF00&
      FillStyle       =   0  'Solid
      Height          =   105
      Left            =   1260
      Shape           =   3  'Circle
      Top             =   5835
      Visible         =   0   'False
      Width           =   105
   End
   Begin VB.Image img2 
      Height          =   1155
      Left            =   1680
      Picture         =   "vcr.frx":0442
      Top             =   6720
      Visible         =   0   'False
      Width           =   1155
   End
   Begin VB.Image img1 
      Height          =   1155
      Left            =   240
      Picture         =   "vcr.frx":10CC
      Top             =   6720
      Visible         =   0   'False
      Width           =   1155
   End
   Begin VB.Label lblTime 
      Alignment       =   2  'Center
      BackColor       =   &H00000000&
      BorderStyle     =   1  'Fixed Single
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   18
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H000000FF&
      Height          =   615
      Left            =   480
      TabIndex        =   12
      Top             =   4920
      Width           =   1935
   End
   Begin VB.Label lblBrand 
      BackStyle       =   0  'Transparent
      Caption         =   "Microsoft Visual Basic VCR - Version 1.0"
      ForeColor       =   &H00FFFFFF&
      Height          =   375
      Left            =   3360
      TabIndex        =   9
      Top             =   5160
      Width           =   2895
   End
   Begin VB.Image imgTapeSlot 
      BorderStyle     =   1  'Fixed Single
      Height          =   735
      Left            =   2640
      Top             =   4920
      Width           =   4215
   End
   Begin VB.Label lblChannel 
      Alignment       =   2  'Center
      BackStyle       =   0  'Transparent
      BorderStyle     =   1  'Fixed Single
      Caption         =   "3"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   24
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H0000FF00&
      Height          =   615
      Left            =   6120
      TabIndex        =   8
      ToolTipText     =   "Channel display"
      Top             =   240
      Width           =   735
   End
End
Attribute VB_Name = "frmVCR"
Attribute VB_Base = "0{FF90640B-E9E1-11CF-84BA-00AA00C007F0}"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'**********************************************
' Purpose:  Main form for the VCR sample
' application. Emulates a video cassette
' recorder using Visual Basic objects.
'**********************************************
Option Explicit

' Create an instance of the Tape class
Dim Tape As New clsTape

Dim vntChannel As Variant   'Channel number

' Constants for QBColor function
Const vcrBlack = 0
Const vcrGreen = 2
Const vcrCyan = 3
Const vcrRed = 4
Const vcrMagenta = 5
Const vcrYellow = 6
Const vcrWhite = 7
Const vcrGray = 8
Const vcrLightBlue = 9
Const vcrLightGreen = 10
Const vcrLightCyan = 11
Const vcrLightRed = 12
Const vcrLightMagenta = 13




Private Sub cmdDown_Click()
    ' if in range, set the channel number
    If vntChannel > 2 Then
        vntChannel = vntChannel - 1
    Else
        vntChannel = 13
    End If
    
    ' assign the channel variable to the display
    lblChannel.Caption = vntChannel
End Sub

Private Sub cmdExit_Click()
    ' unload the form, release the reference
    Unload Me
    Set frmVCR = Nothing
End Sub

Private Sub cmdForward_Click()
    ' call the function to save the old channel
    SaveChannel vntChannel
    ' must be on channel 3 to play a tape
    vntChannel = 3
    lblChannel.Caption = vntChannel
    ' Set the properties of the Tape class
    Tape.Forward = True
    Tape.Speed = 50
    ' Start the timer
    tmr2.Enabled = True
    tmr2.Interval = Tape.Speed
    ' Call the function to update the controls
    ButtonManager frmVCR.cmdForward
End Sub

Private Sub cmdPause_Click()
    ' Stop the timer
    tmr2.Enabled = False
    ' Call the function to update the controls
    ButtonManager frmVCR.cmdPause
End Sub

Private Sub cmdPlay_Click()
    ' call the function to save the old channel
    SaveChannel vntChannel
    ' must be on channel 3 to play a tape
    vntChannel = 3
    lblChannel.Caption = vntChannel
    ' Set the properties of the Tape class
    Tape.Forward = True
    Tape.Speed = 300
    ' Start the timer
    tmr2.Enabled = True
    tmr2.Interval = Tape.Speed
    ' Call the function to update the controls
    ButtonManager frmVCR.cmdPlay
End Sub

Private Sub cmdRec_Click()
    Dim strStatus As String     'Display text
    
    ' call the function to save the old channel
    SaveChannel vntChannel
    ' Clear the display
    picTV.Cls
    ' Diplay the status
    strStatus = "Recording: Channel " & vntChannel
    picTV.Print strStatus
    strStatus = lblTime.Caption
    picTV.Print strStatus
    ' Call the function to update the controls
    ButtonManager frmVCR.cmdRec
End Sub

Private Sub cmdRewind_Click()
    ' call the function to save the old channel
    SaveChannel vntChannel
    ' must be on channel 3 to play a tape
    vntChannel = 3
    lblChannel.Caption = vntChannel
    ' Set the properties of the Tape class
    Tape.Forward = False
    Tape.Speed = 50
    ' Start the timer
    tmr2.Enabled = True
    tmr2.Interval = Tape.Speed
    ' Call the function to update the controls
    ButtonManager frmVCR.cmdRewind
End Sub

Private Sub cmdSet_Click()
    ' show the user entry form modally
    frmSetTime.Show vbModal
End Sub

Private Sub cmdStop_Click()
    Dim intChannel As Integer   'Channel number
    
    ' Stop the timer
    tmr2.Enabled = False
    ' Call the function to update the controls
    ButtonManager frmVCR.cmdStop
    ' Clear the display
    picTV.Cls
    ' restore the old channel
    intChannel = SaveChannel(0)
    vntChannel = intChannel
    lblChannel.Caption = vntChannel
End Sub

Private Sub cmdUp_Click()
    ' if in range, set the channel number
    If vntChannel < 13 Then
        vntChannel = vntChannel + 1
    Else
        vntChannel = 2
    End If
    
     ' assign the channel variable to the display
     lblChannel.Caption = vntChannel
End Sub

Private Sub Form_Load()
    ' Show the current time
    lblTime.Caption = Format((Now), "h:mm AM/PM")
    ' Set the height of the form
    frmVCR.Height = 6990
    img1.Visible = True
    ' Display the form
    Me.Show
    ' set the initial channel
    vntChannel = 3
    lblChannel.Caption = vntChannel
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' Release the references
    Set Tape = Nothing
    Set Recorder = Nothing
    Set frmVCR = Nothing
End Sub

Private Sub lblChannel_Change()
    ' Change the display color based on channel
    Select Case vntChannel
        Case 2
            picTV.BackColor = QBColor(vcrGreen)
        Case 3
            picTV.BackColor = QBColor(vcrWhite)
        Case 4
            picTV.BackColor = QBColor(vcrRed)
        Case 5
            picTV.BackColor = QBColor(vcrMagenta)
        Case 6
            picTV.BackColor = QBColor(vcrYellow)
        Case 7
            picTV.BackColor = QBColor(vcrCyan)
        Case 8
            picTV.BackColor = QBColor(vcrGray)
        Case 9
            picTV.BackColor = QBColor(vcrLightBlue)
        Case 10
            picTV.BackColor = QBColor(vcrLightGreen)
        Case 11
            picTV.BackColor = QBColor(vcrLightCyan)
        Case 12
            picTV.BackColor = QBColor(vcrLightRed)
        Case 13
            picTV.BackColor = QBColor(vcrLightMagenta)
    End Select
    
    ' Clear the display
    picTV.Cls
    ' Display the channel & time
    picTV.Print "Channel: " & vntChannel
    picTV.Print lblTime.Caption
End Sub

Private Sub tmr1_Timer()
    ' Update the time display
    lblTime.Caption = Format((Now), "h:mm AM/PM")
    ' If the Recorder property is turned on
    If Recorder.Enabled = True Then
        ' If it's time to record
        If Recorder.StartRecording = lblTime.Caption Then
            ' Start "recording"
            vntChannel = Recorder.Channel
            lblChannel.Caption = vntChannel
            ' Activate the Record button
            cmdRec.Value = True
            ' clear the property in the Recorder class
            Recorder.StartRecording = Empty
        End If
    Else
        ' If it's time to stop recording
        If Recorder.StopRecording = lblTime.Caption Then
            ' Activate the Stop button
            cmdStop.Value = True
            ' clear the property in the Recorder class
            Recorder.StopRecording = Empty
        End If
    End If
End Sub


Private Sub tmr2_Timer()
    Dim intWidth As Integer     'Width value
    Dim intLeft As Integer      'Left value
    Dim objImage As Control     'Image control
    
    ' Get the width of the display
    intWidth = picTV.Width
    ' Call the method in the Tape class
    ' to "play" the tape.
    Tape.Animate intWidth
    
    ' Retrieve the Left property from the class
    intLeft = Tape.Left
    ' Show either the first or second image
    If img1.Visible = True Then
        img1.Visible = False
        Set objImage = img2
    Else
        img1.Visible = True
        Set objImage = img1
    End If
    
    ' Clear the display
    picTV.Cls
    ' Show the new image in the new location
    picTV.PaintPicture objImage.Picture, intLeft, 1200
End Sub



