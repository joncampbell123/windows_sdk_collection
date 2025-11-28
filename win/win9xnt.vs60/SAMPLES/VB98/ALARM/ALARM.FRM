VERSION 5.00
Begin VB.Form AlarmForm 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Alarm Clock"
   ClientHeight    =   780
   ClientLeft      =   5010
   ClientTop       =   4245
   ClientWidth     =   3135
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Icon            =   "ALARM.frx":0000
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   780
   ScaleWidth      =   3135
   StartUpPosition =   2  'CenterScreen
   Begin VB.Timer Timer1 
      Interval        =   500
      Left            =   2640
      Top             =   120
   End
   Begin VB.Label lblTime 
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   2415
   End
End
Attribute VB_Name = "AlarmForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim AlarmTime
Const conMinimized = 1


Private Sub Form_Click()
    AlarmTime = InputBox("Enter alarm time", "VB Alarm", AlarmTime)
    If AlarmTime = "" Then Exit Sub
    If Not IsDate(AlarmTime) Then
        MsgBox "The time you entered was not valid."
    Else                                    ' String returned from InputBox is a valid time,
        AlarmTime = CDate(AlarmTime)        ' so store it as a date/time value in AlarmTime.
    End If
End Sub

Private Sub Form_Load()
    AlarmTime = ""
End Sub

Private Sub Form_Resize()
    If WindowState = conMinimized Then      ' If form is minimized, display the time in a caption.
        SetCaptionTime
    Else
        Caption = "Alarm Clock"
    End If
End Sub

Private Sub SetCaptionTime()
    Caption = Format(Time, "Medium Time")   ' Display time using medium time format.
End Sub

Private Sub Timer1_Timer()
Static AlarmSounded As Integer
    If lblTime.Caption <> CStr(Time) Then
        ' It's now a different second than the one displayed.
        If Time >= AlarmTime And Not AlarmSounded Then
            Beep
            MsgBox "Alarm at " & Time
            AlarmSounded = True
        ElseIf Time < AlarmTime Then
            AlarmSounded = False
        End If
        If WindowState = conMinimized Then
            ' If minimized, then update the form's Caption every minute.
            If Minute(CDate(Caption)) <> Minute(Time) Then SetCaptionTime
        Else
            ' Otherwise, update the label Caption in the form every second.
            lblTime.Caption = Time
        End If
    End If
End Sub

