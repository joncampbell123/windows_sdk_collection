VERSION 2.00
Begin Form InkFrm 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Storage and Retrival of Ink"
   ClientHeight    =   5640
   ClientLeft      =   300
   ClientTop       =   615
   ClientWidth     =   9015
   ControlBox      =   0   'False
   Height          =   6045
   Left            =   240
   LinkMode        =   1  'Source
   LinkTopic       =   "Form1"
   ScaleHeight     =   5640
   ScaleWidth      =   9015
   Top             =   270
   Width           =   9135
   Begin VIedit IEdit1 
      EraserColor     =   &H00FFFFFF&
      EraserWidth     =   5
      Height          =   1575
      Left            =   240
      TabIndex        =   12
      Top             =   1200
      Width           =   6615
   End
   Begin PictureBox Picture1 
      BackColor       =   &H00C0C0C0&
      Height          =   855
      Index           =   0
      Left            =   120
      ScaleHeight     =   825
      ScaleWidth      =   8745
      TabIndex        =   6
      TabStop         =   0   'False
      Top             =   120
      Width           =   8775
      Begin Label Label1 
         Alignment       =   2  'Center
         BackColor       =   &H00C0C0C0&
         Caption         =   "Ink can saved and restored from memory or a file.  The InkDataString property of an IEdit control is used to manipulate the ink."
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   12
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   615
         Index           =   0
         Left            =   120
         TabIndex        =   7
         Top             =   120
         Width           =   8535
      End
   End
   Begin CommandButton EraseInk 
      Caption         =   "Clear Ink"
      Height          =   1575
      Left            =   7080
      TabIndex        =   0
      Top             =   1200
      Width           =   1575
   End
   Begin PictureBox Picture4 
      BackColor       =   &H00C0C0C0&
      Height          =   855
      Left            =   300
      ScaleHeight     =   825
      ScaleWidth      =   8325
      TabIndex        =   8
      TabStop         =   0   'False
      Top             =   2880
      Width           =   8355
      Begin CommandButton SaveMem 
         Caption         =   "Save Ink in Memory"
         Height          =   555
         Left            =   3060
         TabIndex        =   1
         Top             =   120
         Width           =   2355
      End
      Begin CommandButton RestoreMem 
         Caption         =   "Restore Ink from Memory"
         Height          =   555
         Left            =   5640
         TabIndex        =   2
         Top             =   120
         Width           =   2355
      End
      Begin Label Label1 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Scribble on the IEdit above and then try saving and restoring the ink."
         ForeColor       =   &H00000000&
         Height          =   615
         Index           =   1
         Left            =   240
         TabIndex        =   10
         Top             =   120
         Width           =   2535
      End
   End
   Begin PictureBox Picture5 
      BackColor       =   &H00C0C0C0&
      Height          =   1095
      Left            =   300
      ScaleHeight     =   1065
      ScaleWidth      =   8325
      TabIndex        =   9
      TabStop         =   0   'False
      Top             =   3840
      Width           =   8355
      Begin CommandButton SaveDisk 
         Caption         =   "Save Ink to Disk"
         Height          =   555
         Left            =   3060
         TabIndex        =   3
         Top             =   240
         Width           =   2355
      End
      Begin CommandButton RestoreDisk 
         Caption         =   "Restore Ink from Disk"
         Height          =   555
         Left            =   5640
         TabIndex        =   4
         Top             =   240
         Width           =   2355
      End
      Begin Label Label1 
         BackColor       =   &H00C0C0C0&
         Caption         =   "The ink will be saved to file INK.OUT in the default directory.  It is automatically saved in compressed format."
         ForeColor       =   &H00000000&
         Height          =   855
         Index           =   2
         Left            =   240
         TabIndex        =   11
         Top             =   120
         Width           =   2535
      End
   End
   Begin CommandButton Command5 
      Caption         =   "Return to Main Menu"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   435
      Left            =   120
      TabIndex        =   5
      Top             =   5100
      Width           =   8715
   End
End



Dim szGlobalInkData$

Sub Command5_Click ()
    MainFrm.Show
    InkFrm.Hide
End Sub

Sub EraseInk_Click ()
    IEdit1.EraseInk = True
End Sub

Sub RestoreDisk_Click ()
    Dim lSize As Long       'Size of Ink as a long
    Dim szBuffer$           'Buffer to hold ink when we get it from disk

    ' --- Open the File and Get the data

    On Error GoTo FileErr1
    Open "ink.out" For Binary Access Read As #1
    Get #1, , lSize
    szBuffer$ = String$(lSize, 0)
    Get #1, , szBuffer$
    Close #1

    IEdit1.InkDataString = szBuffer$  ' Now assign ink to Control
    Exit Sub

FileErr1:
    MsgBox "FileErr: " + Error$
    Exit Sub
End Sub

Sub RestoreMem_Click ()
    On Error Resume Next
    IEdit1.InkDataString = szGlobalInkData$
End Sub

Sub SaveDisk_Click ()
    Dim lSize As Long     'Size of the Ink
    Dim szBuffer$         'Buffer to hold the ink temporarily before writing to disk

    szBuffer$ = IEdit1.InkDataString
    lSize = Len(szBuffer$)

    Rem --- The Buffer can now be saved to a file

    On Error Resume Next
    Kill "ink.out"                  'Erase the old file
    On Error GoTo FileErr
    Open "ink.out" For Binary Access Write As #1
    Put #1, , lSize                 'Save Size Information
    Put #1, , szBuffer$             'Save Ink
    Close #1

    MsgBox "Saved" + Str$(lSize) + " bytes of data to 'ink.out' file", 64
    Exit Sub

FileErr:
    MsgBox "FileErr: " + Error$
    Exit Sub
End Sub

Sub SaveMem_Click ()
    szGlobalInkData$ = IEdit1.InkDataString
End Sub
