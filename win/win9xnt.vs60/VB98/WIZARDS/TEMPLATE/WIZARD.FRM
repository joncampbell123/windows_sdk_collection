VERSION 5.00
Begin VB.Form frmWizard 
   Appearance      =   0  'Flat
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "??? Wizard"
   ClientHeight    =   5055
   ClientLeft      =   1965
   ClientTop       =   1815
   ClientWidth     =   7155
   ControlBox      =   0   'False
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Icon            =   "Wizard.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5055
   ScaleWidth      =   7155
   Tag             =   "10"
   Begin VB.Frame fraStep 
      BorderStyle     =   0  'None
      Caption         =   "Introduction Screen"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4425
      Index           =   0
      Left            =   -10000
      TabIndex        =   6
      Tag             =   "1000"
      Top             =   0
      Width           =   7155
      Begin VB.CheckBox chkShowIntro 
         Caption         =   "chkShowIntro"
         Height          =   315
         Left            =   2700
         MaskColor       =   &H00000000&
         TabIndex        =   18
         Tag             =   "1002"
         Top             =   3000
         Width           =   3810
      End
      Begin VB.Label lblStep 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "lblStep"
         ForeColor       =   &H80000008&
         Height          =   1470
         Index           =   0
         Left            =   2700
         TabIndex        =   7
         Tag             =   "1001"
         Top             =   210
         Width           =   3960
      End
      Begin VB.Image imgStep 
         BorderStyle     =   1  'Fixed Single
         Height          =   1995
         Index           =   0
         Left            =   210
         Picture         =   "Wizard.frx":0442
         Stretch         =   -1  'True
         Top             =   210
         Width           =   2070
      End
   End
   Begin VB.Frame fraStep 
      BorderStyle     =   0  'None
      Caption         =   "Step 1"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4425
      Index           =   1
      Left            =   -10000
      TabIndex        =   8
      Tag             =   "2000"
      Top             =   0
      Width           =   7155
      Begin VB.Label lblStep 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "lblStep"
         ForeColor       =   &H80000008&
         Height          =   1470
         Index           =   1
         Left            =   2700
         TabIndex        =   9
         Tag             =   "2001"
         Top             =   210
         Width           =   3960
      End
      Begin VB.Image imgStep 
         BorderStyle     =   1  'Fixed Single
         Height          =   1995
         Index           =   1
         Left            =   210
         Picture         =   "Wizard.frx":4D18
         Stretch         =   -1  'True
         Top             =   210
         Width           =   2070
      End
   End
   Begin VB.Frame fraStep 
      BorderStyle     =   0  'None
      Caption         =   "Step 2"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4425
      Index           =   2
      Left            =   -10000
      TabIndex        =   10
      Tag             =   "2002"
      Top             =   0
      Width           =   7155
      Begin VB.Label lblStep 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "lblStep"
         ForeColor       =   &H80000008&
         Height          =   1470
         Index           =   2
         Left            =   2700
         TabIndex        =   11
         Tag             =   "2003"
         Top             =   210
         Width           =   3960
      End
      Begin VB.Image imgStep 
         BorderStyle     =   1  'Fixed Single
         Height          =   1995
         Index           =   2
         Left            =   210
         Picture         =   "Wizard.frx":95EE
         Stretch         =   -1  'True
         Top             =   210
         Width           =   2070
      End
   End
   Begin VB.Frame fraStep 
      BorderStyle     =   0  'None
      Caption         =   "Step 3"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4425
      Index           =   3
      Left            =   -10000
      TabIndex        =   12
      Tag             =   "2004"
      Top             =   0
      Width           =   7155
      Begin VB.Label lblStep 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "lblStep"
         ForeColor       =   &H80000008&
         Height          =   1470
         Index           =   3
         Left            =   2700
         TabIndex        =   13
         Tag             =   "2005"
         Top             =   210
         Width           =   3960
      End
      Begin VB.Image imgStep 
         BorderStyle     =   1  'Fixed Single
         Height          =   1995
         Index           =   3
         Left            =   210
         Picture         =   "Wizard.frx":DEC4
         Stretch         =   -1  'True
         Top             =   210
         Width           =   2070
      End
   End
   Begin VB.Frame fraStep 
      BorderStyle     =   0  'None
      Caption         =   "Step 4"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4425
      Index           =   4
      Left            =   -10000
      TabIndex        =   14
      Tag             =   "2006"
      Top             =   0
      Width           =   7155
      Begin VB.Label lblStep 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "lblStep"
         ForeColor       =   &H80000008&
         Height          =   1470
         Index           =   4
         Left            =   2700
         TabIndex        =   15
         Tag             =   "2007"
         Top             =   210
         Width           =   3960
      End
      Begin VB.Image imgStep 
         BorderStyle     =   1  'Fixed Single
         Height          =   1995
         Index           =   4
         Left            =   210
         Picture         =   "Wizard.frx":1279A
         Stretch         =   -1  'True
         Top             =   210
         Width           =   2070
      End
   End
   Begin VB.Frame fraStep 
      BorderStyle     =   0  'None
      Caption         =   "Finished!"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4425
      Index           =   5
      Left            =   -10000
      TabIndex        =   16
      Tag             =   "3000"
      Top             =   0
      Width           =   7155
      Begin VB.CheckBox chkSaveSettings 
         Caption         =   "chkSaveSettings"
         Height          =   552
         Left            =   3210
         MaskColor       =   &H00000000&
         TabIndex        =   19
         Tag             =   "3003"
         Top             =   1650
         Width           =   3552
      End
      Begin VB.Label lblStep 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "lblStep"
         ForeColor       =   &H80000008&
         Height          =   1470
         Index           =   5
         Left            =   3210
         TabIndex        =   17
         Tag             =   "3001"
         Top             =   210
         Width           =   3960
      End
      Begin VB.Image imgStep 
         BorderStyle     =   1  'Fixed Single
         Height          =   3075
         Index           =   5
         Left            =   210
         Picture         =   "Wizard.frx":17070
         Stretch         =   -1  'True
         Top             =   210
         Width           =   2430
      End
   End
   Begin VB.PictureBox picNav 
      Align           =   2  'Align Bottom
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   570
      Left            =   0
      ScaleHeight     =   570
      ScaleWidth      =   7155
      TabIndex        =   0
      Top             =   4485
      Width           =   7155
      Begin VB.CommandButton cmdNav 
         Caption         =   "&Finish"
         Height          =   312
         Index           =   4
         Left            =   5910
         MaskColor       =   &H00000000&
         TabIndex        =   5
         Tag             =   "104"
         Top             =   120
         Width           =   1092
      End
      Begin VB.CommandButton cmdNav 
         Caption         =   "&Next >"
         Height          =   312
         Index           =   3
         Left            =   4545
         MaskColor       =   &H00000000&
         TabIndex        =   4
         Tag             =   "103"
         Top             =   120
         Width           =   1092
      End
      Begin VB.CommandButton cmdNav 
         Caption         =   "< &Back"
         Height          =   312
         Index           =   2
         Left            =   3435
         MaskColor       =   &H00000000&
         TabIndex        =   3
         Tag             =   "102"
         Top             =   120
         Width           =   1092
      End
      Begin VB.CommandButton cmdNav 
         Cancel          =   -1  'True
         Caption         =   "Cancel"
         Height          =   312
         Index           =   1
         Left            =   2250
         MaskColor       =   &H00000000&
         TabIndex        =   2
         Tag             =   "101"
         Top             =   120
         Width           =   1092
      End
      Begin VB.CommandButton cmdNav 
         Caption         =   "Help"
         Height          =   312
         Index           =   0
         Left            =   108
         MaskColor       =   &H00000000&
         TabIndex        =   1
         Tag             =   "100"
         Top             =   120
         Width           =   1092
      End
      Begin VB.Line Line1 
         BorderColor     =   &H00808080&
         Index           =   1
         X1              =   108
         X2              =   7012
         Y1              =   0
         Y2              =   0
      End
      Begin VB.Line Line1 
         BorderColor     =   &H00FFFFFF&
         Index           =   0
         X1              =   108
         X2              =   7012
         Y1              =   24
         Y2              =   24
      End
   End
End
Attribute VB_Name = "frmWizard"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Const NUM_STEPS = 6

Const RES_ERROR_MSG = 30000

'BASE VALUE FOR HELP FILE FOR THIS WIZARD:
Const HELP_BASE = 1000
Const HELP_FILE = "MYWIZARD.HLP"

Const BTN_HELP = 0
Const BTN_CANCEL = 1
Const BTN_BACK = 2
Const BTN_NEXT = 3
Const BTN_FINISH = 4

Const STEP_INTRO = 0
Const STEP_1 = 1
Const STEP_2 = 2
Const STEP_3 = 3
Const STEP_4 = 4
Const STEP_FINISH = 5

Const DIR_NONE = 0
Const DIR_BACK = 1
Const DIR_NEXT = 2

Const FRM_TITLE = "Blank Wizard"
Const INTRO_KEY = "IntroductionScreen"
Const SHOW_INTRO = "ShowIntro"
Const TOPIC_TEXT = "<TOPIC_TEXT>"

'module level vars
Dim mnCurStep       As Integer
Dim mbHelpStarted   As Boolean

Public VBInst       As VBIDE.VBE
Dim mbFinishOK      As Boolean

Private Sub chkShowIntro_Click()
    If chkShowIntro.Value Then
        SaveSetting APP_CATEGORY, WIZARD_NAME, INTRO_KEY, SHOW_INTRO
    Else
        SaveSetting APP_CATEGORY, WIZARD_NAME, INTRO_KEY, vbNullString
    End If
End Sub

Private Sub cmdNav_Click(Index As Integer)
    Dim nAltStep As Integer
    Dim lHelpTopic As Long
    Dim rc As Long
    
    Select Case Index
        Case BTN_HELP
            mbHelpStarted = True
            lHelpTopic = HELP_BASE + 10 * (1 + mnCurStep)
            rc = WinHelp(Me.hwnd, HELP_FILE, HELP_CONTEXT, lHelpTopic)
        
        Case BTN_CANCEL
            Unload Me
          
        Case BTN_BACK
            'place special cases here to jump
            'to alternate steps
            nAltStep = mnCurStep - 1
            SetStep nAltStep, DIR_BACK
          
        Case BTN_NEXT
            'place special cases here to jump
            'to alternate steps
            nAltStep = mnCurStep + 1
            SetStep nAltStep, DIR_NEXT
          
        Case BTN_FINISH
            'wizard creation code goes here
      
            Unload Me
            
            If GetSetting(APP_CATEGORY, WIZARD_NAME, CONFIRM_KEY, vbNullString) = vbNullString Then
                frmConfirm.Show vbModal
            End If
        
    End Select
End Sub

Private Sub Form_KeyDown(KeyCode As Integer, Shift As Integer)
    If KeyCode = vbKeyF1 Then
        cmdNav_Click BTN_HELP
    End If
End Sub

Private Sub Form_Load()
    Dim i As Integer
    'init all vars
    mbFinishOK = False
    
    For i = 0 To NUM_STEPS - 1
      fraStep(i).Left = -10000
    Next
    
    'Load All string info for Form
    LoadResStrings Me
    
    'Determine 1st Step:
    If GetSetting(APP_CATEGORY, WIZARD_NAME, INTRO_KEY, vbNullString) = SHOW_INTRO Then
        chkShowIntro.Value = vbChecked
        SetStep 1, DIR_NEXT
    Else
        SetStep 0, DIR_NONE
    End If
End Sub

Private Sub SetStep(nStep As Integer, nDirection As Integer)
  
    Select Case nStep
        Case STEP_INTRO
      
        Case STEP_1
      
        Case STEP_2
        
        Case STEP_3
      
        Case STEP_4
            mbFinishOK = False
      
        Case STEP_FINISH
            mbFinishOK = True
        
    End Select
    
    'move to new step
    fraStep(mnCurStep).Enabled = False
    fraStep(nStep).Left = 0
    If nStep <> mnCurStep Then
        fraStep(mnCurStep).Left = -10000
    End If
    fraStep(nStep).Enabled = True
  
    SetCaption nStep
    SetNavBtns nStep
  
End Sub

Private Sub SetNavBtns(nStep As Integer)
    mnCurStep = nStep
    
    If mnCurStep = 0 Then
        cmdNav(BTN_BACK).Enabled = False
        cmdNav(BTN_NEXT).Enabled = True
    ElseIf mnCurStep = NUM_STEPS - 1 Then
        cmdNav(BTN_NEXT).Enabled = False
        cmdNav(BTN_BACK).Enabled = True
    Else
        cmdNav(BTN_BACK).Enabled = True
        cmdNav(BTN_NEXT).Enabled = True
    End If
    
    If mbFinishOK Then
        cmdNav(BTN_FINISH).Enabled = True
    Else
        cmdNav(BTN_FINISH).Enabled = False
    End If
End Sub

Private Sub SetCaption(nStep As Integer)
    On Error Resume Next

    Me.Caption = FRM_TITLE & " - " & LoadResString(fraStep(nStep).Tag)

End Sub

'=========================================================
'this sub displays an error message when the user has
'not entered enough data to continue
'=========================================================
Sub IncompleteData(nIndex As Integer)
    On Error Resume Next
    Dim sTmp As String
      
    'get the base error message
    sTmp = LoadResString(RES_ERROR_MSG)
    'get the specific message
    sTmp = sTmp & vbCrLf & LoadResString(RES_ERROR_MSG + nIndex)
    Beep
    MsgBox sTmp, vbInformation
End Sub

Private Sub Form_Unload(Cancel As Integer)
    On Error Resume Next
    Dim rc As Long
    'see if we need to save the settings
    If chkSaveSettings.Value = vbChecked Then
      
'        SaveSetting APP_CATEGORY, WIZARD_NAME, "OptionName", Option Value
      
    End If
  
    If mbHelpStarted Then rc = WinHelp(Me.hwnd, HELP_FILE, HELP_QUIT, 0)
End Sub

