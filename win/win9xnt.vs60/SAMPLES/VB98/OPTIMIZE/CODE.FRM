VERSION 5.00
Begin VB.Form Code 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Code Optimizations"
   ClientHeight    =   5220
   ClientLeft      =   870
   ClientTop       =   1515
   ClientWidth     =   6975
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H80000008&
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   5220
   ScaleWidth      =   6975
   ShowInTaskbar   =   0   'False
   Tag             =   "Real"
   WhatsThisHelp   =   -1  'True
   Begin VB.TextBox txtLoopCount 
      Height          =   320
      Left            =   5600
      TabIndex        =   8
      Text            =   "txtLoopCount"
      Top             =   3765
      Width           =   1300
   End
   Begin VB.CommandButton cmdExplicit 
      Caption         =   "Explict Property"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   450
      Left            =   120
      TabIndex        =   2
      Top             =   2280
      Width           =   1935
   End
   Begin VB.CommandButton cmdVariable 
      Caption         =   "Variable Test"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   450
      Left            =   120
      TabIndex        =   1
      Top             =   960
      Width           =   1935
   End
   Begin VB.CommandButton cmdVariant 
      Caption         =   "Variant Counter"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   450
      Left            =   120
      TabIndex        =   4
      Top             =   4190
      Width           =   1935
   End
   Begin VB.CommandButton cmdInteger 
      Caption         =   "Integer Counter"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   450
      Left            =   120
      TabIndex        =   5
      Top             =   4680
      Width           =   1935
   End
   Begin VB.TextBox txtValue 
      Height          =   320
      Left            =   5600
      TabIndex        =   7
      Text            =   "txtValue"
      Top             =   1800
      Width           =   1300
   End
   Begin VB.CommandButton cmdValue 
      Caption         =   "Value of the Control"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   450
      Left            =   120
      TabIndex        =   3
      Top             =   2780
      Width           =   1935
   End
   Begin VB.TextBox txtCounter 
      Height          =   320
      Left            =   5600
      TabIndex        =   6
      Text            =   "txtCounter"
      Top             =   60
      Width           =   1300
   End
   Begin VB.CommandButton cmdProperty 
      Caption         =   "Property Test"
      Default         =   -1  'True
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   450
      Left            =   120
      TabIndex        =   0
      Top             =   480
      Width           =   1935
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00808080&
      Index           =   3
      X1              =   -15
      X2              =   7320
      Y1              =   3465
      Y2              =   3465
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00FFFFFF&
      BorderWidth     =   2
      Index           =   2
      X1              =   0
      X2              =   7335
      Y1              =   3480
      Y2              =   3465
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00808080&
      Index           =   1
      X1              =   0
      X2              =   7335
      Y1              =   1680
      Y2              =   1680
   End
   Begin VB.Label lblTitle 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      BackStyle       =   0  'Transparent
      Caption         =   "Integer vs. Variant Variables"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00800000&
      Height          =   300
      Index           =   0
      Left            =   120
      TabIndex        =   21
      Top             =   3750
      Width           =   3675
   End
   Begin VB.Label lblTitle 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      BackStyle       =   0  'Transparent
      Caption         =   "Explicit property vs. Value of the Control"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00800000&
      Height          =   300
      Index           =   2
      Left            =   120
      TabIndex        =   20
      Top             =   1800
      Width           =   5550
   End
   Begin VB.Label lblTitle 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      BackStyle       =   0  'Transparent
      Caption         =   "Variables vs. Property Access"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00800000&
      Height          =   300
      Index           =   1
      Left            =   120
      TabIndex        =   19
      Top             =   120
      Width           =   3855
   End
   Begin VB.Label lblVariant 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "00.00 secs"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   250
      Left            =   5600
      TabIndex        =   18
      Top             =   4290
      Width           =   1800
   End
   Begin VB.Label lblInteger 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "00.00 secs"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   250
      Left            =   5600
      TabIndex        =   17
      Top             =   4785
      Width           =   1800
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Use the explicit Text property:"
      ForeColor       =   &H80000008&
      Height          =   250
      Index           =   3
      Left            =   2145
      TabIndex        =   16
      Top             =   2385
      Width           =   2700
   End
   Begin VB.Label lblExplicit 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "00.00 secs"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   250
      Left            =   5600
      TabIndex        =   15
      Top             =   2385
      Width           =   1300
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Use value of the control:"
      ForeColor       =   &H80000008&
      Height          =   250
      Index           =   2
      Left            =   2145
      TabIndex        =   14
      Top             =   2880
      Width           =   2700
   End
   Begin VB.Label lblValue 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "00.00 secs"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   250
      Left            =   5600
      TabIndex        =   13
      Top             =   2880
      Width           =   1300
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00FFFFFF&
      BorderWidth     =   2
      Index           =   0
      X1              =   15
      X2              =   7350
      Y1              =   1695
      Y2              =   1680
   End
   Begin VB.Label lblVariable 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "00.00 secs"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   250
      Left            =   5600
      TabIndex        =   12
      Top             =   1080
      Width           =   1300
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Using a Variable in the loop:"
      ForeColor       =   &H80000008&
      Height          =   250
      Index           =   1
      Left            =   2145
      TabIndex        =   11
      Top             =   1080
      Width           =   3100
   End
   Begin VB.Label lblProperty 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "00.00 secs"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   250
      Left            =   5600
      TabIndex        =   10
      Top             =   585
      Width           =   1300
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Using the Text property in the loop:"
      ForeColor       =   &H80000008&
      Height          =   250
      Index           =   0
      Left            =   2145
      TabIndex        =   9
      Top             =   585
      Width           =   3105
   End
End
Attribute VB_Name = "Code"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub cmdExplicit_Click()
'Use the explicit property (txtValue.Text) reference
'Sub cmdValue_Click is slightly faster
    Limit% = Val(txtValue)
    Screen.MousePointer = 11
    start = Timer
    For x% = 1 To Limit%
        foo$ = txtValue.TEXT
    Next x%
    Finish = Timer
    Screen.MousePointer = 0
    lblExplicit = Format$(Finish - start, "##.##") & " secs."
End Sub

Private Sub cmdInteger_Click()
'Use an Integer as the Loop Counter
'faster than Sub cmdVariant_Click
Dim intCounter1 As Integer, intCounter2 As Integer, intCounter3 As Integer
    Screen.MousePointer = 11
    start = Timer
    For intCounter1 = 1 To 100
        For intCounter2 = 1 To intCounter1
            For intCounter3 = 1 To intCounter2
                VAR = intCounter3 * 100 + intCounter2 * 10 + intCounter1
            Next intCounter3
        Next intCounter2
    Next intCounter1
    Finish = Timer
    Screen.MousePointer = 0
    lblInteger = Format$(Finish - start, "##.##") & " secs."
End Sub

Private Sub cmdProperty_Click()
'In each iteration, reference the Text Property
'Sub cmdVariable_Click is faster
    Limit% = Val(txtCounter.TEXT)
    Screen.MousePointer = 11
    
    start = Timer
    
    For x% = 1 To Limit%
        foo$ = Left(txtCounter.TEXT, Len(txtCounter.TEXT) \ 2)
    Next x%
    
    Finish = Timer
    
    Screen.MousePointer = 0
    lblProperty = Format$(Finish - start, "##.##") & " secs."
End Sub

Private Sub cmdValue_Click()
'Use the Value of the control by omitting the explicit Text property
'slightly faster than Sub cmdExplicit_Click
    Limit% = Val(txtValue)
    Screen.MousePointer = 11
    start = Timer
    For x% = 1 To Limit%
        foo$ = txtValue
    Next x%
    Finish = Timer
    Screen.MousePointer = 0
    lblValue = Format$(Finish - start, "##.##") & " secs."
End Sub

Private Sub cmdVariable_Click()
'Use a Variable (Temp$) instead of a Property Access
'faster than Sub cmdProperty_Click
    Limit% = Val(txtCounter.TEXT)
    Temp$ = txtCounter.TEXT
    Screen.MousePointer = 11
    
    start = Timer
    
    For x% = 1 To Limit%
        foo$ = Left(Temp$, Len(Temp$) \ 2)
    Next x%
    
    Finish = Timer
    
    Screen.MousePointer = 0
    lblVariable = Format$(Finish - start, "##.##") & " secs."
End Sub
Private Sub cmdVariant_Click()
'Use a Variant as the Loop Counter.
'Sub cmdInteger_Click is faster since it uses a Integer as the Loop Counter
    
    Screen.MousePointer = 11
    start = Timer
    
    For varCounter1 = 1 To txtLoopCount.TEXT
        For varCounter2 = 1 To varCounter1
            For varCounter3 = 1 To varCounter2
                VAR = varCounter3 * 100 + varCounter2 * 10 + varCounter1
            Next varCounter3
        Next varCounter2
    Next varCounter1
    
    Finish = Timer
    Screen.MousePointer = 0
    lblVariant = Format$(Finish - start, "##.##") & " secs."
End Sub

Private Sub Form_Load()
    PosForm Me
    
    ' Set default counter values
    txtCounter = 1000
    txtValue = 1000
    txtLoopCount = 100

End Sub

