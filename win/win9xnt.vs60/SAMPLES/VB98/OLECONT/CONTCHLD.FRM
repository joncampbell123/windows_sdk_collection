VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Begin VB.Form ChildForm 
   AutoRedraw      =   -1  'True
   Caption         =   "OLE Object Container"
   ClientHeight    =   3330
   ClientLeft      =   1140
   ClientTop       =   3735
   ClientWidth     =   5550
   LinkTopic       =   "Form1"
   MDIChild        =   -1  'True
   ScaleHeight     =   3330
   ScaleWidth      =   5550
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   315
      Top             =   105
      _ExtentX        =   847
      _ExtentY        =   847
      FilterIndex     =   4096
      FontSize        =   8.40483e-38
   End
   Begin VB.OLE OLE1 
      Height          =   3252
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   5532
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      NegotiatePosition=   1  'Left
      Begin VB.Menu mnuFileNew 
         Caption         =   "&New..."
      End
      Begin VB.Menu mnuSave 
         Caption         =   "&Save As..."
      End
      Begin VB.Menu mnuOpen 
         Caption         =   "&Open"
      End
      Begin VB.Menu sep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
      Begin VB.Menu sep2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuAbout 
         Caption         =   "A&bout..."
      End
   End
   Begin VB.Menu mnuEdit 
      Caption         =   "&Edit"
      Begin VB.Menu mnuCopy 
         Caption         =   "&Copy"
      End
      Begin VB.Menu mnuDelete 
         Caption         =   "&Delete"
      End
      Begin VB.Menu mnuSpecial 
         Caption         =   "Paste &Special..."
      End
      Begin VB.Menu esep2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuUpdate 
         Caption         =   "&Update "
      End
   End
   Begin VB.Menu mnuCloseOLE 
      Caption         =   "&Close Ole Object"
      NegotiatePosition=   3  'Right
   End
   Begin VB.Menu mnuWindow 
      Caption         =   "&Window"
      WindowList      =   -1  'True
      Begin VB.Menu mnuCascade 
         Caption         =   "&Cascade"
      End
      Begin VB.Menu mnuTile 
         Caption         =   "&Tile"
      End
      Begin VB.Menu mnuArrange 
         Caption         =   "&Arrange Icons"
      End
   End
End
Attribute VB_Name = "ChildForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Load()
On Error Resume Next

    OLE1.Move 0, 0
    
    OLE1.Height = Me.Height
    OLE1.Width = Me.Width
          
    OLE1.HostName = "OLE Container Control Demo"
End Sub

Private Sub mnuAbout_Click()
    DisplayInstructions
End Sub

Private Sub mnuArrange_Click()
    ParentForm.Arrange vbArrangeIcons
End Sub

Private Sub mnuCascade_Click()
    ParentForm.Arrange vbCascade
End Sub

Private Sub mnuClose_Click()
    ' Close the OLE container control.
    OLE1.Close
    
End Sub

Private Sub mnuCloseOLE_Click()
    OLE1.Close
End Sub

Private Sub mnuCopy_Click()
    If OLE1.AppIsRunning = True Then
        OLE1.Copy
    Else
        OLE1.AppIsRunning = True
        If OLE1.AppIsRunning Then OLE1.Copy
    End If
    
    If OLE1.OLEType <> vbOLENone Then   ' If the control contains a valid object...
        ' Display the hourglass mouse pointer.
        Screen.MousePointer = 11
        If OLE1.AppIsRunning Then
           OLE1.Copy                    ' Copy the object onto the Clipboard.
        End If
    End If
    Screen.MousePointer = 0
End Sub

Private Sub mnuDelete_Click()
    If OLE1.OLEType <> vbOLENone Then   ' If the OLE container control contains a valid object...
        OLE1.Delete                     ' Delete the object, and then unload the form.
    End If
        Unload Me
End Sub

Private Sub mnuEdit_Click()
    On Error Resume Next
    If Err Then
        MsgBox "No contained object."
    End If
    

    If OLE1.PasteOK Then
        ParentForm.ActiveForm.mnuSpecial.Enabled = True
    Else
        ParentForm.ActiveForm.mnuSpecial.Enabled = False
    End If
End Sub

Private Sub mnuExit_Click()
    End
End Sub

Private Sub mnuFileNew_Click()
    NewObject
End Sub

Private Sub mnuOpen_Click()
    OpenObject
End Sub

Private Sub mnuSave_Click()
    OpenSave ("Save")
End Sub

Private Sub mnuSpecial_Click()
    If OLE1.PasteOK Then
        MDINew = False
        OLE1.PasteSpecialDlg
        Screen.MousePointer = 11
        UpdateCaption
        Screen.MousePointer = 0
    End If
End Sub

Private Sub mnuTile_Click()
    ParentForm.Arrange vbTileHorizontal
End Sub

Private Sub mnuUpdate_Click()
    Screen.MousePointer = 11
    OLE1.Update
    Screen.MousePointer = 0
End Sub

Private Sub Ole1_ObjectMove(Left As Single, Top As Single, Width As Single, Height As Single)
    OLE1.Move OLE1.Left, OLE1.Top, Width, Height
    OLE1.Move Left, Top, OLE1.Width, OLE1.Height
End Sub

Private Sub OLE1_Resize(HeightNew As Single, WidthNew As Single)
    OLE1.SizeMode = vbOLESizeAutoSize
End Sub

Private Sub Ole1_Updated(Code As Integer)
    OLE1.SizeMode = vbOLESizeAutoSize
End Sub

