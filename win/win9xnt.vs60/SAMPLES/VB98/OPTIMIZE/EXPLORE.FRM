VERSION 5.00
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.Form frmExplore 
   Caption         =   "Optimizing VB 6.0"
   ClientHeight    =   5856
   ClientLeft      =   1776
   ClientTop       =   1956
   ClientWidth     =   3420
   ClipControls    =   0   'False
   Icon            =   "Explore.frx":0000
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   5856
   ScaleWidth      =   3420
   WhatsThisHelp   =   -1  'True
   Begin MSComCtlLib.TreeView tvExample 
      Height          =   5130
      Left            =   -30
      TabIndex        =   0
      Top             =   450
      Width           =   3450
      _ExtentX        =   6096
      _ExtentY        =   9038
      _Version        =   393216
      Indentation     =   529
      LabelEdit       =   1
      LineStyle       =   1
      Style           =   7
      ImageList       =   "ilExplore"
      BorderStyle     =   1
      Appearance      =   1
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   7.8
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin MSComCtlLib.StatusBar StatusBar1 
      Align           =   2  'Align Bottom
      Height          =   288
      Left            =   0
      TabIndex        =   1
      Top             =   5568
      Width           =   3420
      _ExtentX        =   6033
      _ExtentY        =   508
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   3
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   2558
            MinWidth        =   2558
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   5
            AutoSize        =   2
            Object.Width           =   1588
            MinWidth        =   1587
            TextSave        =   "4:04 PM"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   6
            AutoSize        =   2
            TextSave        =   "2/18/98"
         EndProperty
      EndProperty
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   7.8
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin MSComCtlLib.ImageList ilExplore 
      Left            =   2715
      Top             =   600
      _ExtentX        =   995
      _ExtentY        =   995
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   16711935
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   7
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "Explore.frx":030A
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "Explore.frx":0624
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "Explore.frx":0736
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "Explore.frx":0848
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "Explore.frx":095A
            Key             =   ""
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "Explore.frx":0A6C
            Key             =   ""
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "Explore.frx":0B7E
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00808080&
      Index           =   0
      X1              =   0
      X2              =   9195
      Y1              =   15
      Y2              =   15
   End
   Begin VB.Label lblInfoWnd 
      BorderStyle     =   1  'Fixed Single
      Caption         =   "Optimizing Code Examples"
      Height          =   315
      Index           =   0
      Left            =   0
      TabIndex        =   2
      Top             =   105
      Width           =   3400
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00FFFFFF&
      BorderWidth     =   2
      Index           =   1
      X1              =   0
      X2              =   9195
      Y1              =   30
      Y2              =   30
   End
   Begin VB.Menu mnu_File 
      Caption         =   "&File"
      Begin VB.Menu mnu_FileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnu_View 
      Caption         =   "&View"
   End
   Begin VB.Menu mnu_Tools 
      Caption         =   "&Tools"
      Begin VB.Menu mnu_ToolsCloseAll 
         Caption         =   "&Close All Windows"
      End
   End
   Begin VB.Menu mnu_Help 
      Caption         =   "&Help"
      Begin VB.Menu mnu_HelpAbout 
         Caption         =   "&About..."
      End
   End
End
Attribute VB_Name = "frmExplore"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim frmlastForm As Form
Private Sub Form_Load()
    
  Me.Move 0, (Screen.Height - Me.Height) / 2
  Me.Show
  DoEvents

End Sub


Private Sub Form_Paint()
  Debug.Print "frmExplore Painted"
End Sub

Private Sub Form_Resize()
  Dim lfrmScaleWidth As Long
  Dim lRetVal As Long
  
  On Error Resume Next
  ' Disable automatic screen updating
  'lRetVal = SetRedraw(Me.hWnd, 0)
  
  lfrmScaleWidth = frmExplore.ScaleWidth
  
  tvExample.Move 0, 495, lfrmScaleWidth, frmExplore.ScaleHeight - tvExample.Top - StatusBar1.Height - 20
  'tvExample.Width = lfrmScaleWidth
  'tvExample.Height = frmExplore.ScaleHeight - tvExample.Top - StatusBar1.Height - 20
  
  lblInfoWnd(0).Width = lfrmScaleWidth
  Line1(0).X2 = lfrmScaleWidth
  Line1(1).X2 = lfrmScaleWidth
  On Error GoTo 0
  
  ' Enable automatic screen updating
  'lRetVal = SetRedraw(Me.hWnd, 1)

End Sub
Private Sub Form_Unload(Cancel As Integer)
  Dim iFormCount As Integer

  For iFormCount = Forms.Count - 1 To 0 Step -1
    Unload Forms(iFormCount)
  Next iFormCount

End Sub


Private Sub mnu_FileExit_Click()
  Unload Me
End Sub


Private Sub mnu_HelpAbout_Click()
  frmAbout.Show
End Sub


Private Sub mnu_ToolsCloseAll_Click()
  Dim iFormCount As Integer

  For iFormCount = Forms.Count - 1 To 1 Step -1
    Unload Forms(iFormCount)
  Next iFormCount
End Sub

Private Sub tvExample_NodeClick(ByVal Node As Node)
  Dim lRetVal As Long
  
  Select Case Node.Text
    Case "Algorithms"
      Algorithms.Show
      Algorithms.SetFocus
    Case "Paint Picture"
      frmPaintPic.Show
      frmPaintPic.SetFocus
    Case "Graphics"
      Graphics.Show
      Graphics.SetFocus
    Case "String Manipulation"
      Strings.Show
      Strings.SetFocus
    Case "Code Optimizations"
      Code.Show
      Code.SetFocus
    Case "Numeric Types"
      frmNumbers.Show
      frmNumbers.SetFocus
    
    Case "Picture vs. Image"
      Pictures.Show
      Pictures.SetFocus
    Case "Destroy Form Objects"
      frmShow2.Show
      frmShow2.SetFocus
    Case "Splash Screen"
      frmSplashDemo.Show
      frmSplashDemo.SetFocus
    Case "Collections"
      frmCollection.Show
      frmCollection.SetFocus
    Case Else
      MsgBox "I don't know that form type.", vbQuestion, "Show Example Error"
    End Select
  DoEvents
End Sub


