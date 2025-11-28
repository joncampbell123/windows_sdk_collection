VERSION 5.00
Begin VB.Form Strings 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Strings & Files"
   ClientHeight    =   4650
   ClientLeft      =   1575
   ClientTop       =   2100
   ClientWidth     =   7215
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
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4650
   ScaleWidth      =   7215
   ShowInTaskbar   =   0   'False
   Tag             =   "Real"
   Begin VB.CommandButton cmdString 
      Caption         =   "Build outside Loop"
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
      Index           =   1
      Left            =   120
      TabIndex        =   1
      Top             =   1320
      Width           =   1900
   End
   Begin VB.TextBox txtCount2 
      Height          =   280
      Left            =   5700
      TabIndex        =   5
      Text            =   "1000"
      Top             =   2520
      Width           =   1400
   End
   Begin VB.CommandButton cmdFiles 
      Caption         =   "Binary"
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
      Index           =   1
      Left            =   120
      TabIndex        =   3
      Top             =   3840
      Width           =   1900
   End
   Begin VB.CommandButton cmdFiles 
      Caption         =   "Random"
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
      Index           =   0
      Left            =   120
      TabIndex        =   2
      Top             =   3120
      Width           =   1900
   End
   Begin VB.CommandButton cmdString 
      Caption         =   "Build in Loop"
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
      Index           =   0
      Left            =   120
      TabIndex        =   0
      Top             =   660
      Width           =   1900
   End
   Begin VB.TextBox txtCounter 
      Height          =   280
      Left            =   5700
      TabIndex        =   4
      Text            =   "txtCounter"
      Top             =   120
      Width           =   1400
   End
   Begin VB.Label lblFiles 
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
      ForeColor       =   &H00800000&
      Height          =   280
      Index           =   1
      Left            =   5700
      TabIndex        =   8
      Top             =   3840
      Width           =   1400
   End
   Begin VB.Label lblFiles 
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
      ForeColor       =   &H00800000&
      Height          =   280
      Index           =   0
      Left            =   5700
      TabIndex        =   10
      Top             =   3120
      Width           =   1400
   End
   Begin VB.Label Label2 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Read && Write records using Binary I/O"
      ForeColor       =   &H80000008&
      Height          =   450
      Left            =   2150
      TabIndex        =   13
      Top             =   3840
      Width           =   3100
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Read && Write records using Random access"
      ForeColor       =   &H80000008&
      Height          =   450
      Index           =   4
      Left            =   2150
      TabIndex        =   15
      Top             =   3120
      Width           =   3100
   End
   Begin VB.Label lblTitle 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "File I/O"
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
      TabIndex        =   14
      Top             =   2400
      Width           =   1875
   End
   Begin VB.Line Line2 
      BorderColor     =   &H00808080&
      X1              =   0
      X2              =   7335
      Y1              =   2100
      Y2              =   2100
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Copy the strings to a temp Variable, then copy temp to new Variable"
      ForeColor       =   &H80000008&
      Height          =   450
      Index           =   3
      Left            =   2145
      TabIndex        =   12
      Top             =   1320
      Width           =   3105
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Explicitly copy strings to the new Variable"
      ForeColor       =   &H80000008&
      Height          =   450
      Index           =   2
      Left            =   2150
      TabIndex        =   11
      Top             =   660
      Width           =   3100
   End
   Begin VB.Label lblString 
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
      ForeColor       =   &H00800000&
      Height          =   280
      Index           =   0
      Left            =   5700
      TabIndex        =   9
      Top             =   660
      Width           =   1400
   End
   Begin VB.Label lblString 
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
      ForeColor       =   &H00800000&
      Height          =   280
      Index           =   1
      Left            =   5700
      TabIndex        =   7
      Top             =   1320
      Width           =   1400
   End
   Begin VB.Label lblTitle 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Strings"
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
      TabIndex        =   6
      Top             =   60
      Width           =   1875
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00FFFFFF&
      BorderWidth     =   2
      Index           =   0
      X1              =   0
      X2              =   7335
      Y1              =   2115
      Y2              =   2115
   End
End
Attribute VB_Name = "Strings"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub cmdFiles_Click(Index As Integer)
'Use Binary instead of Random File I/O"
    On Error Resume Next
    Dim allrecs() As BookAuthor
    ReDim allrecs(txtCount2)
    Me.MousePointer = 11
    For x = 1 To txtCount2
        allrecs(x).ID = x
        allrecs(x).Name = "John Doe - Generic Person"
    Next x
    Kill App.Path & "\" & "files.out"
    If Index = 0 Then
        Open App.Path & "\" & "files.out" For Random As #1
    Else
        Open App.Path & "\" & "files.out" For Binary As #1
    End If
    start# = Timer
    For x = 1 To txtCount2
       Put #1, , allrecs(x)
    Next x
    Seek #1, 1
    For x = 1 To txtCount2
        Get #1, , allrecs(x)
    Next x
    Finish# = Timer
    lblFiles(Index) = Format(Finish# - start#, "##.##") & " secs."
    Close #1
    Me.MousePointer = 0
End Sub

Private Sub cmdString_Click(Index As Integer)
    strfirst$ = "Really long String. "
    strsecond$ = "Next really long string. "
    strthird$ = "I mean this is going to be a really long one!"
    MousePointer = 11
    If Index = 0 Then
        start# = Timer
        For x = 1 To txtCounter
            result$ = strfirst$ & strsecond$ & strthird$
        Next x
        Finish# = Timer
    Else
        Temp$ = strfirst$ & strsecond$ & strthird$
        start# = Timer
        For x = 1 To txtCounter
            result$ = Temp$
        Next x
        Finish# = Timer
    End If
    lblString(Index) = Format$(Finish# - start#, "##.##") & " secs."
    MousePointer = 0
End Sub

Private Sub Form_Load()

  PosForm Me

  txtCounter = 10000
  txtCount2 = 1000

End Sub

