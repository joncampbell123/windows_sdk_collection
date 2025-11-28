VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.0#0"; "COMDLG32.OCX"
Begin VB.Form frmSDI 
   Caption         =   "SDI NotePad - Untitled"
   ClientHeight    =   5370
   ClientLeft      =   315
   ClientTop       =   1470
   ClientWidth     =   6690
   LinkTopic       =   "Form1"
   ScaleHeight     =   5370
   ScaleWidth      =   6690
   Begin VB.TextBox txtNote 
      Height          =   3975
      HideSelection   =   0   'False
      Left            =   0
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   0
      Top             =   480
      Width           =   5655
   End
   Begin VB.PictureBox picToolbar 
      Align           =   1  'Align Top
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   405
      Left            =   0
      ScaleHeight     =   345
      ScaleWidth      =   6630
      TabIndex        =   1
      Top             =   0
      Width           =   6690
      Begin MSComDlg.CommonDialog CMDialog1 
         Left            =   1995
         Top             =   0
         _ExtentX        =   847
         _ExtentY        =   847
         CancelError     =   -1  'True
         DefaultExt      =   "TXT"
         Filter          =   "Text Files (*.txt)|*.txt|All Files (*.*)|*.*"
         FilterIndex     =   473
         FontSize        =   7.98198e-38
      End
      Begin VB.Image imgFileNewButton 
         Height          =   330
         Left            =   0
         Picture         =   "frmsdi.frx":0000
         ToolTipText     =   "New File"
         Top             =   0
         Width           =   360
      End
      Begin VB.Image imgFileOpenButton 
         Height          =   330
         Left            =   360
         Picture         =   "frmsdi.frx":018A
         ToolTipText     =   "Open File"
         Top             =   0
         Width           =   360
      End
      Begin VB.Image imgCutButton 
         Height          =   330
         Left            =   840
         Picture         =   "frmsdi.frx":0314
         ToolTipText     =   "Cut"
         Top             =   0
         Width           =   375
      End
      Begin VB.Image imgCopyButton 
         Height          =   330
         Left            =   1200
         Picture         =   "frmsdi.frx":04F6
         ToolTipText     =   "Copy"
         Top             =   0
         Width           =   375
      End
      Begin VB.Image imgPasteButton 
         Height          =   330
         Left            =   1560
         Picture         =   "frmsdi.frx":06D8
         ToolTipText     =   "Paste"
         Top             =   0
         Width           =   375
      End
      Begin VB.Image imgFileNewButtonDn 
         Height          =   330
         Left            =   2040
         Picture         =   "frmsdi.frx":08BA
         Top             =   0
         Visible         =   0   'False
         Width           =   375
      End
      Begin VB.Image imgFileNewButtonUp 
         Height          =   330
         Left            =   2400
         Picture         =   "frmsdi.frx":0A9C
         Top             =   0
         Visible         =   0   'False
         Width           =   360
      End
      Begin VB.Image imgFileOpenButtonUp 
         Height          =   330
         Left            =   3120
         Picture         =   "frmsdi.frx":0C26
         Top             =   0
         Visible         =   0   'False
         Width           =   360
      End
      Begin VB.Image imgFileOpenButtonDn 
         Height          =   330
         Left            =   2760
         Picture         =   "frmsdi.frx":0DB0
         Top             =   0
         Visible         =   0   'False
         Width           =   360
      End
      Begin VB.Image imgCutButtonUp 
         Height          =   330
         Left            =   3480
         Picture         =   "frmsdi.frx":0F3A
         Top             =   0
         Visible         =   0   'False
         Width           =   375
      End
      Begin VB.Image imgCutButtonDn 
         Height          =   330
         Left            =   3840
         Picture         =   "frmsdi.frx":111C
         Top             =   0
         Visible         =   0   'False
         Width           =   375
      End
      Begin VB.Image imgCopyButtonUp 
         Height          =   330
         Left            =   4560
         Picture         =   "frmsdi.frx":12FE
         Top             =   0
         Visible         =   0   'False
         Width           =   375
      End
      Begin VB.Image imgCopyButtonDn 
         Height          =   330
         Left            =   4200
         Picture         =   "frmsdi.frx":14E0
         Top             =   0
         Visible         =   0   'False
         Width           =   375
      End
      Begin VB.Image imgPasteButtonDn 
         Height          =   330
         Left            =   4920
         Picture         =   "frmsdi.frx":16C2
         Top             =   0
         Visible         =   0   'False
         Width           =   375
      End
      Begin VB.Image imgPasteButtonUp 
         Height          =   330
         Left            =   5280
         Picture         =   "frmsdi.frx":18A4
         Top             =   0
         Visible         =   0   'False
         Width           =   375
      End
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileNew 
         Caption         =   "&New"
      End
      Begin VB.Menu mnuFileOpen 
         Caption         =   "&Open"
      End
      Begin VB.Menu mnuFileSave 
         Caption         =   "&Save"
      End
      Begin VB.Menu mnuFileSaveAs 
         Caption         =   "Save &As"
      End
      Begin VB.Menu mnuFSep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "E&xit"
      End
      Begin VB.Menu mnuRecentFile 
         Caption         =   "-"
         Index           =   0
         Visible         =   0   'False
      End
      Begin VB.Menu mnuRecentFile 
         Caption         =   "RecentFile1"
         Index           =   1
         Visible         =   0   'False
      End
      Begin VB.Menu mnuRecentFile 
         Caption         =   "RecentFile2"
         Index           =   2
         Visible         =   0   'False
      End
      Begin VB.Menu mnuRecentFile 
         Caption         =   "RecentFile3"
         Index           =   3
         Visible         =   0   'False
      End
      Begin VB.Menu mnuRecentFile 
         Caption         =   "RecentFile4"
         Index           =   4
         Visible         =   0   'False
      End
      Begin VB.Menu mnuRecentFile 
         Caption         =   "RecentFile5"
         Index           =   5
         Visible         =   0   'False
      End
   End
   Begin VB.Menu mnuEdit 
      Caption         =   "&Edit"
      Begin VB.Menu mnuEditCut 
         Caption         =   "Cu&t"
         Shortcut        =   ^X
      End
      Begin VB.Menu mnuEditCopy 
         Caption         =   "&Copy"
         Shortcut        =   ^C
      End
      Begin VB.Menu mnuEditPaste 
         Caption         =   "&Paste"
         Shortcut        =   ^V
      End
      Begin VB.Menu mnuEditDelete 
         Caption         =   "De&lete"
         Shortcut        =   {DEL}
      End
      Begin VB.Menu mnuEditSep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuEditSelectAll 
         Caption         =   "Select &All"
      End
      Begin VB.Menu mnuEditTime 
         Caption         =   "Time / &Date"
      End
   End
   Begin VB.Menu mnuSearch 
      Caption         =   "&Search"
      Begin VB.Menu mnuSearchFind 
         Caption         =   "&Find"
      End
      Begin VB.Menu mnuSearchFindNext 
         Caption         =   "Find &Next"
         Shortcut        =   {F3}
      End
   End
   Begin VB.Menu mnuOptions 
      Caption         =   "&Options"
      Begin VB.Menu mnuOptionsToolbar 
         Caption         =   "&Toolbar"
      End
      Begin VB.Menu mnuFonts 
         Caption         =   "&Fonts"
         Begin VB.Menu mnuFontName 
            Caption         =   "FontName"
            Index           =   0
         End
      End
      Begin VB.Menu mnuOptionsLaunch 
         Caption         =   "&Launch New Instance"
      End
   End
End
Attribute VB_Name = "frmSDI"
Attribute VB_Base = "0{8EC00D4C-E9E4-11CF-84BA-00AA00C007F0}"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'*** Main form for the SDI Notepad sample application  ***
'**********************************************************
Option Explicit

Private Sub Form_Load()
    Dim i As Integer        ' Counter variable.
    
    ' Application starts here (Load event of Startup form).
    Show
    ' Always set the working directory to the directory containing the application.
    ChDir App.Path
    FState.Dirty = False
    ' Read System registry and set the recent menu file list control array appropriately.
    GetRecentFiles
    ' Set public variable gFindDirection which determines which direction
    ' the FindIt function will search in.
    gFindDirection = 1
        
    ' Assign the name of the first font to a font
    ' menu entry, then loop through the fonts
    ' collection, adding them to the menu
    mnuFontName(0).Caption = Screen.Fonts(0)
    For i = 1 To Screen.FontCount - 1
        Load mnuFontName(i)
        mnuFontName(0).Caption = Screen.Fonts(i)
    Next
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
    Dim strMsg As String
    Dim strFilename As String
    Dim intResponse As Integer

    ' Check to see if the text has been changed.
    If FState.Dirty Then
        strFilename = Me.Caption
        strMsg = "The text in [" & strFilename & "] has changed."
        strMsg = strMsg & vbCrLf
        strMsg = strMsg & "Do you want to save the changes?"
        intResponse = MsgBox(strMsg, 51, frmSDI.Caption)
        Select Case intResponse
            Case 6      ' User chose Yes.
                If Left(Me.Caption, 8) = "Untitled" Then
                    ' The file hasn't been saved yet.
                    strFilename = "untitled.txt"
                    ' Get the strFilename, and then call the save procedure, GetstrFilename.
                    strFilename = GetFileName(strFilename)
                Else
                    ' The form's Caption contains the name of the open file.
                    strFilename = Me.Caption
                End If
                ' Call the save procedure. If strFilename = Empty, then
                ' the user chose Cancel in the Save As dialog box; otherwise,
                ' save the file.
                If strFilename <> "" Then
                    SaveFileAs strFilename
                End If
            Case 7      ' User chose No. Unload the file.
                Cancel = False
            Case 2      ' User chose Cancel. Cancel the unload.
                Cancel = True
        End Select
    End If
End Sub

Private Sub Form_Resize()
    ' Call the resize procedure
    ResizeNote
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' Call the recent file list procedure
    GetRecentFiles
End Sub

Private Sub imgCopyButton_Click()
    ' Refresh the image.
    imgCopyButton.Refresh
    ' Call the copy procedure
    EditCopyProc
End Sub

Private Sub imgCopyButton_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the down state.
    imgCopyButton.Picture = imgCopyButtonDn.Picture
End Sub

Private Sub imgCopyButton_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' If the button is pressed, display the up bitmap when the
    ' mouse is dragged outside the button's area; otherwise
    ' display the down bitmap.
    Select Case Button
    Case 1
        If X <= 0 Or X > imgCopyButton.Width Or Y < 0 Or Y > imgCopyButton.Height Then
            imgCopyButton.Picture = imgCopyButtonUp.Picture
        Else
            imgCopyButton.Picture = imgCopyButtonDn.Picture
        End If
    End Select
End Sub

Private Sub imgCopyButton_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the up state.
    imgCopyButton.Picture = imgCopyButtonUp.Picture
End Sub

Private Sub imgCutButton_Click()
    ' Refresh the image.
    imgCutButton.Refresh
    ' Call the cut procedure
    EditCutProc
End Sub

Private Sub imgCutButton_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the down state.
    imgCutButton.Picture = imgCutButtonDn.Picture
End Sub

Private Sub imgCutButton_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' If the button is pressed, display the up bitmap when the
    ' mouse is dragged outside the button's area; otherwise,
    ' display the down bitmap.
    Select Case Button
    Case 1
        If X <= 0 Or X > imgCutButton.Width Or Y < 0 Or Y > imgCutButton.Height Then
            imgCutButton.Picture = imgCutButtonUp.Picture
        Else
            imgCutButton.Picture = imgCutButtonDn.Picture
        End If
    End Select
End Sub

Private Sub imgCutButton_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the up state.
    imgCutButton.Picture = imgCutButtonUp.Picture
End Sub

Private Sub imgFileNewButton_Click()
    ' Refresh the image.
    imgFileNewButton.Refresh
    ' Call the new file procedure
    FileNew
End Sub

Private Sub imgFileNewButton_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the down state.
    imgFileNewButton.Picture = imgFileNewButtonDn.Picture
End Sub

Private Sub imgFileNewButton_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' If the button is pressed, display the up bitmap when the
    ' mouse is dragged outside the button's area; otherwise,
    ' display the down bitmap.
    Select Case Button
    Case 1
        If X <= 0 Or X > imgFileNewButton.Width Or Y < 0 Or Y > imgFileNewButton.Height Then
            imgFileNewButton.Picture = imgFileNewButtonUp.Picture
        Else
            imgFileNewButton.Picture = imgFileNewButtonDn.Picture
        End If
    End Select
End Sub

Private Sub imgFileNewButton_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the up state.
    imgFileNewButton.Picture = imgFileNewButtonUp.Picture
End Sub

Private Sub imgFileOpenButton_Click()
    ' Refresh the image.
    imgFileOpenButton.Refresh
    ' Call the file open procedure
    FileOpenProc
End Sub

Private Sub imgFileOpenButton_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the down state.
    imgFileOpenButton.Picture = imgFileOpenButtonDn.Picture
End Sub

Private Sub imgFileOpenButton_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' If the button is pressed, display the up bitmap when the
    ' mouse is dragged outside the button's area; otherwise,
    ' display the down bitmap.
    Select Case Button
    Case 1
        If X <= 0 Or X > imgFileOpenButton.Width Or Y < 0 Or Y > imgFileOpenButton.Height Then
            imgFileOpenButton.Picture = imgFileOpenButtonUp.Picture
        Else
            imgFileOpenButton.Picture = imgFileOpenButtonDn.Picture
        End If
    End Select
End Sub

Private Sub imgFileOpenButton_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the up state.
    imgFileOpenButton.Picture = imgFileOpenButtonUp.Picture

End Sub

Private Sub imgPasteButton_Click()
    ' Refresh the image.
    imgPasteButton.Refresh
    ' Call the paste procedure
    EditPasteProc
End Sub

Private Sub imgPasteButton_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the down state.
    imgPasteButton.Picture = imgPasteButtonDn.Picture
End Sub

Private Sub imgPasteButton_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' If the button is pressed, display the up bitmap when the
    ' mouse is dragged outside the button's area; otherwise,
    ' display the down bitmap.
    Select Case Button
    Case 1
        If X <= 0 Or X > imgPasteButton.Width Or Y < 0 Or Y > imgPasteButton.Height Then
            imgPasteButton.Picture = imgPasteButtonUp.Picture
        Else
            imgPasteButton.Picture = imgPasteButtonDn.Picture
        End If
    End Select
End Sub

Private Sub imgPasteButton_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' Show the picture for the up state.
    imgPasteButton.Picture = imgPasteButtonUp.Picture
End Sub

Private Sub mnuEditCopy_Click()
    ' Call the copy procedure
    EditCopyProc
End Sub

Private Sub mnuEditCut_Click()
    ' Call the cut procedure
    EditCutProc
End Sub

Private Sub mnuEditDelete_Click()
' If the mouse pointer is not at the end of the notepad...
    If txtNote.SelStart <> Len(Screen.ActiveControl.Text) Then
        ' If nothing is selected, extend the selection by one.
        If txtNote.SelLength = 0 Then
            txtNote.SelLength = 1
            ' If the mouse pointer is on a blank line, extend the selection by two.
            If Asc(txtNote.SelText) = 13 Then
                txtNote.SelLength = 2
            End If
        End If
        ' Delete the selected text.
        txtNote.SelText = ""
    End If
End Sub

Private Sub mnuEditPaste_Click()
    ' Call the paste procedure.
    EditPasteProc
End Sub

Private Sub mnuEditSelectAll_Click()
    ' Use SelStart & SelLength to select the text.
    txtNote.SelStart = 0
    txtNote.SelLength = Len(txtNote.Text)
End Sub

Private Sub mnuEditTime_Click()
    ' Insert the current time and date.
    txtNote.SelText = Now
End Sub

Private Sub mnuFileExit_Click()
    ' End the application.
    Unload Me
End Sub

Public Sub mnuFileNew_Click()
    ' Call the new form procedure
    FileNew
End Sub

Private Sub mnuFileOpen_Click()
    ' Call the file open procedure.
    FileOpenProc
End Sub

Private Sub mnuFileSave_Click()
    'Call the file save procedure
    FileSave
End Sub

Private Sub mnuFileSaveAs_Click()
    Dim strSaveFileName As String
    Dim strDefaultName As String
    
    ' Assign the form caption to the variable.
    strDefaultName = Right$(Me.Caption, Len(Me.Caption) - 14)
    If Me.Caption = "SDI NotePad - Untitled" Then
        ' The file hasn't been saved yet.
        ' Get the filename, and then call the save procedure, strSaveFileName.
        
        strSaveFileName = GetFileName("Untitled.txt")
        If strSaveFileName <> "" Then SaveFileAs (strSaveFileName)
        ' Update the list of recently opened files in the File menu control array.
        UpdateFileMenu (strSaveFileName)
    Else
        ' The form's Caption contains the name of the open file.
        strSaveFileName = GetFileName(strDefaultName)
        If strSaveFileName <> "" Then SaveFileAs (strSaveFileName)
        ' Update the list of recently opened files in the File menu control array.
        UpdateFileMenu (strSaveFileName)
    End If
End Sub

Private Sub mnuFontName_Click(Index As Integer)
    ' Assign the selected font to the textbox fontname property.
    txtNote.FontName = mnuFontName(Index).Caption
End Sub

Private Sub mnuOptions_Click()
    ' Toggle the Checked property to match the .Visible property.
    mnuOptionsToolbar.Checked = picToolbar.Visible
End Sub

Private Sub mnuOptionsLaunch_Click()
    Dim strApp As String
    
    ' Shell a new instance of the notepad.
    strApp = App.Path & "\" & App.EXEName
    Shell strApp, 1
End Sub

Private Sub mnuOptionsToolbar_Click()
    ' Toggle the visible property of the toolbar
    picToolbar.Visible = Not picToolbar.Visible
    ' Change the check to match the current state
    mnuOptionsToolbar.Checked = picToolbar.Visible
    ' Call the resize procedure
    ResizeNote
End Sub

Private Sub mnuRecentFile_Click(Index As Integer)
    ' Call the file open procedure, passing a
    ' reference to the selected file name
    OpenFile (mnuRecentFile(Index).Caption)
    ' Update the list of recently opened files in the File menu control array.
    GetRecentFiles
End Sub

Private Sub mnuSearchFind_Click()
    ' If there is text in the textbox, assign it to
    ' the textbox on the Find form, otherwise assign
    ' the last findtext value.
    If txtNote.SelText <> "" Then
        frmFind.txtFind.Text = txtNote.SelText
    Else
        frmFind.txtFind.Text = gFindString
    End If
    ' Set the public variable to start at the beginning.
    gFirstTime = True
    ' Set the case checkbox to match the public variable
    If (gFindCase) Then
        frmFind.chkCase = 1
    End If
    ' Display the Find form.
    frmFind.Show vbModal
End Sub

Private Sub mnuSearchFindNext_Click()
    ' If the public variable isn't empty, call the
    ' find procedure, otherwise call the find menu
    If Len(gFindString) > 0 Then
        FindIt
    Else
        mnuSearchFind_Click
    End If
End Sub

Private Sub txtNote_Change()
    ' Set the public variable to show that text has changed.
    FState.Dirty = True
End Sub
