VERSION 5.00
Begin VB.Form frmCogs 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "COGS Options"
   ClientHeight    =   2550
   ClientLeft      =   705
   ClientTop       =   3975
   ClientWidth     =   6900
   ClipControls    =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2550
   ScaleWidth      =   6900
   StartUpPosition =   2  'CenterScreen
   WhatsThisHelp   =   -1  'True
   Begin VB.CommandButton Command1 
      Caption         =   "&Save"
      Default         =   -1  'True
      Height          =   420
      Index           =   0
      Left            =   5550
      TabIndex        =   13
      Top             =   1650
      Width           =   1215
   End
   Begin VB.CommandButton Command1 
      Caption         =   "&Help"
      Height          =   420
      Index           =   1
      Left            =   5550
      TabIndex        =   12
      Top             =   705
      Width           =   1215
   End
   Begin VB.CommandButton Command1 
      Caption         =   "&Cancel"
      Height          =   420
      Index           =   2
      Left            =   5550
      TabIndex        =   11
      Top             =   210
      Width           =   1215
   End
   Begin VB.Frame Frame3 
      Caption         =   "Paper Type"
      Height          =   2340
      Left            =   1965
      TabIndex        =   6
      Top             =   120
      Width           =   1395
      Begin VB.OptionButton optPaperGrade 
         Caption         =   "Grade 2"
         Height          =   285
         Index           =   1
         Left            =   150
         TabIndex        =   10
         Top             =   690
         Width           =   1215
      End
      Begin VB.OptionButton optPaperGrade 
         Caption         =   "Grade 3"
         Height          =   285
         Index           =   2
         Left            =   150
         TabIndex        =   9
         Top             =   1050
         Width           =   1215
      End
      Begin VB.OptionButton optPaperGrade 
         Caption         =   "Grade 4"
         Height          =   285
         Index           =   3
         Left            =   150
         TabIndex        =   8
         Top             =   1410
         Width           =   1215
      End
      Begin VB.OptionButton optPaperGrade 
         Caption         =   "Grade 1"
         Height          =   285
         Index           =   0
         Left            =   150
         TabIndex        =   7
         Top             =   330
         Value           =   -1  'True
         Width           =   1215
      End
   End
   Begin VB.Frame Frame2 
      Caption         =   "Picture Colors"
      Height          =   1125
      Left            =   105
      TabIndex        =   3
      Top             =   1335
      Width           =   1680
      Begin VB.OptionButton optPicColor 
         Caption         =   "Color"
         Height          =   285
         Index           =   1
         Left            =   120
         TabIndex        =   5
         Top             =   690
         Width           =   1500
      End
      Begin VB.OptionButton optPicColor 
         Caption         =   "Black && White"
         Height          =   285
         Index           =   0
         Left            =   120
         TabIndex        =   4
         Top             =   345
         Value           =   -1  'True
         Width           =   1500
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Binding Type"
      Height          =   1125
      Left            =   105
      TabIndex        =   0
      Top             =   120
      Width           =   1680
      Begin VB.OptionButton optBinding 
         Caption         =   "Paper Back"
         Height          =   285
         Index           =   1
         Left            =   140
         TabIndex        =   2
         Top             =   600
         Width           =   1215
      End
      Begin VB.OptionButton optBinding 
         Caption         =   "Hard Cover"
         Height          =   285
         Index           =   0
         Left            =   140
         TabIndex        =   1
         Top             =   300
         Value           =   -1  'True
         Width           =   1215
      End
   End
   Begin VB.Frame FrameConverted 
      BackColor       =   &H00C0C0C0&
      Height          =   2355
      Left            =   3465
      TabIndex        =   14
      Top             =   90
      Width           =   1905
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "+"
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   10
         Left            =   1725
         TabIndex        =   32
         Top             =   1455
         Width           =   90
      End
      Begin VB.Label lblCost 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   6
         Left            =   1125
         TabIndex        =   31
         Top             =   1755
         Width           =   585
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         Caption         =   "Unit Cost"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00800000&
         Height          =   255
         Index           =   9
         Left            =   60
         TabIndex        =   30
         Top             =   1755
         Width           =   1005
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "___________________"
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   8
         Left            =   105
         TabIndex        =   29
         Top             =   1515
         Width           =   1710
      End
      Begin VB.Label lblCost 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   5
         Left            =   1125
         TabIndex        =   28
         Top             =   1455
         Width           =   585
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         Caption         =   "Binding Type"
         ForeColor       =   &H00800000&
         Height          =   255
         Index           =   7
         Left            =   60
         TabIndex        =   27
         Top             =   1455
         Width           =   1005
      End
      Begin VB.Label lblCost 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   4
         Left            =   1125
         TabIndex        =   26
         Top             =   1245
         Width           =   585
      End
      Begin VB.Label lblCost 
         BackStyle       =   0  'Transparent
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   3
         Left            =   1125
         TabIndex        =   25
         Top             =   960
         Width           =   555
      End
      Begin VB.Label lblCost 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   2
         Left            =   1125
         TabIndex        =   24
         Top             =   750
         Width           =   585
      End
      Begin VB.Label lblCost 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   1
         Left            =   1125
         TabIndex        =   23
         Top             =   480
         Width           =   585
      End
      Begin VB.Label lblCost 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   0
         Left            =   1125
         TabIndex        =   22
         Top             =   270
         Width           =   585
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "___________________"
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   6
         Left            =   105
         TabIndex        =   21
         Top             =   1020
         Width           =   1710
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "*"
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   5
         Left            =   1725
         TabIndex        =   20
         Top             =   960
         Width           =   60
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         Caption         =   "Pages"
         ForeColor       =   &H00800000&
         Height          =   255
         Index           =   4
         Left            =   450
         TabIndex        =   19
         Top             =   960
         Width           =   615
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "+"
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   2
         Left            =   1725
         TabIndex        =   17
         Top             =   480
         Width           =   90
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         Caption         =   "Paper Grade"
         ForeColor       =   &H00800000&
         Height          =   255
         Index           =   1
         Left            =   60
         TabIndex        =   16
         Top             =   480
         Width           =   1005
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         Caption         =   "Picture Colors"
         ForeColor       =   &H00800000&
         Height          =   255
         Index           =   0
         Left            =   60
         TabIndex        =   15
         Top             =   270
         Width           =   1005
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "___________________"
         ForeColor       =   &H00800000&
         Height          =   195
         Index           =   3
         Left            =   105
         TabIndex        =   18
         Top             =   540
         Width           =   1710
      End
   End
End
Attribute VB_Name = "frmCogs"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim lRetVal As Long

' These variables store the current COGS values selected by the user.
Dim cBindingCost As Currency
Dim cPicture As Currency
Dim cPaperGrade As Currency
Dim lngNumPages As Long

Dim strBinding As String
Dim strPicture As String
Dim strPaper As String

Dim acurBindingCost(1) As Currency
Dim acurPictureCost(1) As Currency
Dim acurPaperCost(3) As Currency
Dim acurCogs(5) As Currency
Private Sub optPicColor_Click(Index As Integer)
If Index = 0 Then
  ' Black & white
  strPicture = "BW"
Else
  ' Color
  strPicture = "Color"
End If
  
acurCogs(0) = acurPictureCost(Index)
lblCost(0).Caption = Format$(acurCogs(0), "#.0000")

CalcUnitCost

End Sub

Private Sub Command1_Click(Index As Integer)
  Dim sHelpString As String
  
  Select Case Index
    Case 0  ' Save
      frmRevenue.txtRevParm(0).Text = strBinding & ", " & strPicture & ", " & strPaper
      frmRevenue.txtRevParm(0).Tag = lblCost(6).Caption
      Unload Me
      
    Case 1  ' Help
      sHelpString = "Select the desired Binding Type, Picture Type, and Paper Quality." & vbCrLf & _
                    "Click <Save> to save these values and return to the Book Revenue form." & vbCrLf & _
                    "Click <Cancel> to discard values and return to the Book Revenue form."
                    
      lRetVal = MsgBox(prompt:=sHelpString, Title:="COGS Help", Buttons:=vbInformation)
      
    Case 2  'Close
      Unload Me
      
 End Select
      
End Sub
Sub CalcUnitCost()
  Dim curTemp As Currency
    
  curTemp = acurCogs(0) + acurCogs(1)
  lblCost(2).Caption = Format$(curTemp, "#.0000")
  
  curTemp = curTemp * lngNumPages
  lblCost(4).Caption = Format$(curTemp, "#.0000")
  
  curTemp = curTemp + acurCogs(5)
  lblCost(6).Caption = Format$(curTemp, "#.0000")
  
End Sub
Function GetCOGS() As Integer
  Dim fld As Field
  Dim strSQL As String
  Dim rsCOGS As ADODB.Recordset
  On Error GoTo GetCOGSError
  
  goStatusPanel.Text = "Fetching COGS numbers..."
  strSQL = "SELECT * FROM COGS"
           
  Set rsCOGS = gobjServer.GetRsCOGS(strSQL)
  
  
  
  acurBindingCost(0) = rsCOGS.Fields("HardCover")
  acurBindingCost(1) = rsCOGS.Fields("PaperBack")
  acurPictureCost(0) = rsCOGS.Fields("BlackWhite")
  acurPictureCost(1) = rsCOGS.Fields("Color")
  acurPaperCost(0) = rsCOGS.Fields("Grade1")
  acurPaperCost(1) = rsCOGS.Fields("Grade2")
  acurPaperCost(2) = rsCOGS.Fields("Grade3")
  acurPaperCost(3) = rsCOGS.Fields("Grade4")

  rsCOGS.Close
  
  GetCOGS = True
  Exit Function
  
GetCOGSError:
  If IsObject(gSn) Then gSn.Close
  GetCOGS = False
    
End Function

Public Function GetBookPages(strTitle As String) As Long
   Dim strSQL As String
   Dim rsBookPages As ADODB.Recordset
   Static strOldTitle As String
   Static lngPages As Long

   On Error GoTo GetBookPagesError

   If strTitle <> strOldTitle Then
     frmRevenue.sbrRev.Panels(1).Text = "Retrieving page count for " & strTitle & "..."
     strSQL = "SELECT Titles.Pages " & _
              "FROM Titles " & _
              "WHERE ((Titles.Title=" & Chr$(34) & Trim$(strTitle) & Chr$(34) & "));"
  
      
  Set rsBookPages = gobjServer.GetBookPages(strSQL)
  
  lngPages = rsBookPages.Fields("Pages")
  strOldTitle = strTitle
  rsBookPages.Close
  
Else
  goStatusPanel.Text = "Using cached pagecount."

End If

GetBookPages = lngPages

Exit Function

GetBookPagesError:
  If IsObject(rsBookPages) Then rsBookPages.Close
  GetBookPages = False
  MsgBox Error$ & Str$(Err), vbCritical, "GetBookPages Error"

End Function

Private Sub Form_Load()
  Me.Left = (Screen.Width - Me.Width) / 2
  Me.Top = (Screen.Height - Me.Height) / 2
  Me.Show
  DoEvents
  
  lngNumPages = GetBookPages(frmRevenue.cboBooks.Text)
  lRetVal = GetCOGS
  
  lblCost(0).Caption = Format$(acurPictureCost(0), "#.0000")
  acurCogs(0) = acurPictureCost(0)
  
  lblCost(1).Caption = Format$(acurPaperCost(0), "#.0000")
  acurCogs(1) = acurPaperCost(0)
  
  lblCost(3).Caption = Str$(lngNumPages)
  
  lblCost(5).Caption = Format$(acurBindingCost(0), "#.0000")
  acurCogs(5) = acurBindingCost(0)
  
  strBinding = "Hard"
  strPicture = "BW"
  strPaper = "1"
  CalcUnitCost
  DoEvents
  
End Sub

Private Sub optBinding_Click(Index As Integer)
  If Index = 0 Then
    strBinding = "Hard"
    
  Else
    strBinding = "Paper"
    
  End If
  
  acurCogs(5) = acurBindingCost(Index)
  lblCost(5).Caption = Format$(acurCogs(5), "#.0000")
  
  If optPaperGrade(0).Value Then
    optPaperGrade_Click (0)
  
  ElseIf optPaperGrade(0).Value Then
    optPaperGrade_Click (1)
    
  ElseIf optPaperGrade(2).Value Then
    optPaperGrade_Click (2)
    
  Else
    optPaperGrade_Click (3)
    
  End If

  CalcUnitCost
  
End Sub

Private Sub optPaperGrade_Click(Index As Integer)
  
  Select Case Index
    Case 0
      strPaper = "1"
      
    Case 1
      strPaper = "2"
      
    Case 2
      strPaper = "3"

    Case 3
      strPaper = "4"
      
  End Select
 
  
  If optBinding(1).Value = True Then
    acurCogs(1) = acurPaperCost(Index) * 0.5
      
  Else
    acurCogs(1) = acurPaperCost(Index)
    
  End If
  
  lblCost(1).Caption = Format$(acurCogs(1), "#.0000")
  CalcUnitCost
  
End Sub


