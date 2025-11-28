Attribute VB_Name = "Module2"
'*** Standard module with procedures for working with   ***
'*** files. Part of the SDI Notepad sample application. ***
'**********************************************************
Option Explicit

Sub FileOpenProc()
    Dim intRetVal
    Dim intResponse As Integer
    Dim strOpenFileName As String
    
    ' If the file has changed, save it
    If FState.Dirty = True Then
        intResponse = FileSave
        If intResponse = False Then Exit Sub
    End If
    On Error Resume Next
    
    frmSDI.CMDialog1.Filename = ""
    frmSDI.CMDialog1.ShowOpen
    If Err <> 32755 Then    ' User chose Cancel.
        strOpenFileName = frmSDI.CMDialog1.Filename
        ' If the file is larger than 65K, it can't
        ' be opened, so cancel the operation.
        If FileLen(strOpenFileName) > 65000 Then
            MsgBox "The file is too large to open."
            Exit Sub
        End If
        
        OpenFile (strOpenFileName)
        UpdateFileMenu (strOpenFileName)
    End If
End Sub

Function GetFileName(Filename As Variant)
    ' Display a Save As dialog box and return a filename.
    ' If the user chooses Cancel, return an empty string.
    On Error Resume Next
    frmSDI.CMDialog1.Filename = Filename
    frmSDI.CMDialog1.ShowSave
    If Err <> 32755 Then    ' User chose Cancel.
        GetFileName = frmSDI.CMDialog1.Filename
    Else
        GetFileName = ""
    End If
End Function

Function OnRecentFilesList(Filename) As Integer
    Dim i         ' Counter variable.

    For i = 1 To 4
        If frmSDI.mnuRecentFile(i).Caption = Filename Then
            OnRecentFilesList = True
            Exit Function
        End If
    Next i
    OnRecentFilesList = False
End Function

Sub OpenFile(Filename)
    Dim fIndex As Integer
    
    On Error Resume Next
    ' Open the selected file.
    Open Filename For Input As #1
    If Err Then
        MsgBox "Can't open file: " + Filename
        Exit Sub
    End If
    ' Change the mouse pointer to an hourglass.
    Screen.MousePointer = 11
    
    ' Change the form's caption and display the new text.
    frmSDI.Caption = "SDI NotePad - " & UCase(Filename)
    frmSDI.txtNote.Text = StrConv(InputB(LOF(1), 1), vbUnicode)
    FState.Dirty = False
    Close #1
    ' Reset the mouse pointer.
    Screen.MousePointer = 0
End Sub

Sub SaveFileAs(Filename)
    On Error Resume Next
    Dim strContents As String

    ' Open the file.
    Open Filename For Output As #1
    ' Place the contents of the notepad into a variable.
    strContents = frmSDI.txtNote.Text
    ' Display the hourglass mouse pointer.
    Screen.MousePointer = 11
    ' Write the variable contents to a saved file.
    Print #1, strContents
    Close #1
    ' Reset the mouse pointer.
    Screen.MousePointer = 0
    ' Set the form's caption.
    If Err Then
        MsgBox Error, 48, App.Title
    Else
        frmSDI.Caption = "SDI NotePad - " & Filename
        ' Reset the dirty flag.
        FState.Dirty = False
    End If
End Sub

Sub UpdateFileMenu(Filename)
        Dim intRetVal As Integer
        ' Check if the open filename is already in the File menu control array.
        intRetVal = OnRecentFilesList(Filename)
        If Not intRetVal Then
            ' Write open filename to the registry.
            WriteRecentFiles (Filename)
        End If
        ' Update the list of the most recently opened files in the File menu control array.
        GetRecentFiles
End Sub

