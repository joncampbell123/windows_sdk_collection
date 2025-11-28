VERSION 5.00
Begin VB.Form frmDynaSnap 
   Caption         =   "Dynaset/Snapshot"
   ClientHeight    =   3750
   ClientLeft      =   2730
   ClientTop       =   2610
   ClientWidth     =   5490
   HelpContextID   =   2016125
   Icon            =   "DYNASNAP.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MDIChild        =   -1  'True
   ScaleHeight     =   3733.906
   ScaleMode       =   0  'User
   ScaleWidth      =   5503.698
   ShowInTaskbar   =   0   'False
   Tag             =   "Recordset"
   Begin VB.PictureBox picViewButtons 
      Align           =   1  'Align Top
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   852
      Left            =   0
      ScaleHeight     =   855
      ScaleMode       =   0  'User
      ScaleWidth      =   5487.272
      TabIndex        =   13
      TabStop         =   0   'False
      Top             =   0
      Width           =   5484
      Begin VB.CommandButton cmdMove 
         Caption         =   "&Move"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   345
         Left            =   2730
         MaskColor       =   &H00000000&
         TabIndex        =   7
         Top             =   375
         Width           =   1365
      End
      Begin VB.CommandButton cmdSort 
         Caption         =   "&Sort"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   345
         Left            =   0
         MaskColor       =   &H00000000&
         TabIndex        =   5
         Top             =   372
         Width           =   1365
      End
      Begin VB.CommandButton cmdFilter 
         Caption         =   "F&ilter"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   345
         Left            =   1365
         MaskColor       =   &H00000000&
         TabIndex        =   6
         Top             =   375
         Width           =   1365
      End
      Begin VB.CommandButton cmdClose 
         Cancel          =   -1  'True
         Caption         =   "&Close"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   345
         Left            =   4095
         MaskColor       =   &H00000000&
         TabIndex        =   4
         TabStop         =   0   'False
         Top             =   15
         Width           =   1365
      End
      Begin VB.CommandButton cmdDelete 
         Caption         =   "&Delete"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   345
         Left            =   2730
         MaskColor       =   &H00000000&
         TabIndex        =   3
         Top             =   15
         Width           =   1365
      End
      Begin VB.CommandButton cmdEdit 
         Caption         =   "&Edit"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   345
         Left            =   1365
         MaskColor       =   &H00000000&
         TabIndex        =   2
         Top             =   15
         Width           =   1365
      End
      Begin VB.CommandButton cmdAdd 
         Caption         =   "&Add"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   345
         Left            =   0
         MaskColor       =   &H00000000&
         TabIndex        =   1
         Top             =   20
         Width           =   1365
      End
      Begin VB.CommandButton cmdFind 
         Caption         =   "&Find"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   345
         Left            =   4095
         MaskColor       =   &H00000000&
         TabIndex        =   8
         Top             =   375
         Width           =   1365
      End
   End
   Begin VB.PictureBox picChangeButtons 
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   855
      Left            =   0
      ScaleHeight     =   919.528
      ScaleMode       =   0  'User
      ScaleWidth      =   5719.056
      TabIndex        =   14
      TabStop         =   0   'False
      Top             =   0
      Visible         =   0   'False
      Width           =   5655
      Begin VB.CommandButton cmdUpdate 
         Caption         =   "&Update"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   372
         Left            =   960
         MaskColor       =   &H00000000&
         TabIndex        =   11
         Top             =   48
         Width           =   1212
      End
      Begin VB.CommandButton cmdCancel 
         Caption         =   "&Cancel"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   372
         Left            =   2640
         MaskColor       =   &H00000000&
         TabIndex        =   12
         Top             =   48
         Width           =   1212
      End
   End
   Begin VB.PictureBox picFldHdr 
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   240
      Left            =   0
      ScaleHeight     =   240
      ScaleMode       =   0  'User
      ScaleWidth      =   14948.92
      TabIndex        =   18
      TabStop         =   0   'False
      Top             =   840
      Width           =   14946
      Begin VB.Label lblFieldValue 
         Caption         =   " Value (F4=Zoom)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   1680
         TabIndex        =   20
         Top             =   0
         Width           =   2295
      End
      Begin VB.Label lblFieldHdr 
         Caption         =   "Field Name:"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   252
         Left            =   120
         TabIndex        =   19
         Top             =   0
         Width           =   1212
      End
   End
   Begin VB.PictureBox picMoveButtons 
      Align           =   2  'Align Bottom
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   288
      Left            =   0
      ScaleHeight     =   298.153
      ScaleMode       =   0  'User
      ScaleWidth      =   5493.878
      TabIndex        =   17
      TabStop         =   0   'False
      Top             =   3465
      Width           =   5484
      Begin VB.HScrollBar hsclCurrRow 
         Height          =   255
         Left            =   0
         Max             =   100
         TabIndex        =   9
         Top             =   29
         Width           =   2895
      End
      Begin VB.Label lblStatus 
         Height          =   255
         Left            =   3000
         TabIndex        =   21
         Top             =   38
         Width           =   1695
      End
   End
   Begin VB.VScrollBar vsbScrollBar 
      Height          =   2250
      LargeChange     =   3000
      Left            =   5040
      SmallChange     =   300
      TabIndex        =   10
      Top             =   1080
      Visible         =   0   'False
      Width           =   255
   End
   Begin VB.PictureBox picFields 
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   375
      Left            =   120
      ScaleHeight     =   372
      ScaleMode       =   0  'User
      ScaleWidth      =   4812
      TabIndex        =   15
      TabStop         =   0   'False
      Top             =   1080
      Width           =   4815
      Begin VB.TextBox txtFieldData 
         DataSource      =   "Data1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   288
         Index           =   0
         Left            =   1560
         TabIndex        =   0
         Top             =   0
         Visible         =   0   'False
         Width           =   3252
      End
      Begin VB.Label lblFieldName 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   252
         Index           =   0
         Left            =   0
         TabIndex        =   16
         Top             =   60
         Visible         =   0   'False
         Width           =   1572
      End
   End
End
Attribute VB_Name = "frmDynaSnap"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'>>>>>>>>>>>>>>>>>>>>>>>>
Const BUTTON1 = "&Add"
Const BUTTON2 = "&Edit"
Const BUTTON3 = "&Delete"
Const BUTTON4 = "&Close"
Const BUTTON5 = "&Sort"
Const BUTTON6 = "F&ilter"
Const BUTTON7 = "&Move"
Const BUTTON8 = "&Find"
Const BUTTON9 = "&Cancel"
Const BUTTON10 = "&Update"
Const Label1 = "Field Name:"
Const Label2 = "Value (F4=Zoom)"
Const MSG1 = "Add record"
Const MSG2 = "Enter number of Rows to Move:"
Const MSG3 = "(Use negative value to move backwards)"
Const MSG4 = "Field Length Exceeded, Data Truncated!"
Const MSG5 = "Delete Current Record?"
Const MSG6 = "Edit record"
Const MSG7 = "Enter Filter Expression:"
Const MSG8 = "Setting New Filter"
Const MSG9 = "Enter Search Parameters"
Const MSG10 = "Searching for New Record"
Const MSG11 = "Record Not Found"
Const MSG12 = "Resizing Form"
Const MSG13 = "Enter Sort Column:"
Const MSG14 = "Setting New Sort Order"
'>>>>>>>>>>>>>>>>>>>>>>>>


'form variables
Public mrsFormRecordset As Recordset
Dim msTableName As String      'form recordset table name
Dim mvBookMark As Variant       'form bookmark
Dim mbNotFound As Integer      'used by find function
Dim mbEditFlag As Integer      'edit mode
Dim mbAddNewFlag As Integer    'add mode
Dim mbDataChanged As Integer   'field data dirty flag
Dim mfrmFind As New frmFindForm      'find form instance
Dim mlNumRows As Long          'total rows in recordset

Private Sub cmdAdd_Click()
  On Error GoTo AddErr

  'set the mode
  mrsFormRecordset.AddNew
  lblStatus.Caption = MSG1
  mbAddNewFlag = True
  If mrsFormRecordset.RecordCount > 0 Then
    mvBookMark = mrsFormRecordset.Bookmark
  Else
    mvBookMark = vbNullString
  End If

  picChangeButtons.Visible = True
  picViewButtons.Visible = False
  hsclCurrRow.Enabled = False
  
  ClearDataFields Me, mrsFormRecordset.Fields.Count
  txtFieldData(0).SetFocus
  mbDataChanged = False
  Exit Sub

AddErr:
  ShowError
End Sub

Private Sub cmdCancel_Click()
   On Error Resume Next

   picChangeButtons.Visible = False
   picViewButtons.Visible = True
   hsclCurrRow.Enabled = True

   mbEditFlag = False
   mbAddNewFlag = False
   mrsFormRecordset.CancelUpdate
   DBEngine.Idle dbFreeLocks
   DisplayCurrentRecord Me, mrsFormRecordset, mlNumRows, mbAddNewFlag
   mbDataChanged = False

End Sub

Private Sub cmdMove_Click()
  On Error GoTo MVErr
  
  Dim sBookMark As String
  Dim sRows As String
  Dim lRows As Long
  
  sRows = InputBox(MSG2 & vbCrLf & MSG3)
  If Len(sRows) = 0 Then Exit Sub
  
  lRows = CLng(sRows)
  mrsFormRecordset.Move lRows
  
  'check to see if they moved past the bounds of the recordset
  If mrsFormRecordset.EOF Then
    mrsFormRecordset.MoveLast
  ElseIf mrsFormRecordset.BOF Then
    mrsFormRecordset.MoveFirst
  End If
  
  If hsclCurrRow.Value = mrsFormRecordset.PercentPosition Then Exit Sub
  
  sBookMark = mrsFormRecordset.Bookmark  'save the new position
  'now we need to reposition the scrollbar to reflect the move
  If mlNumRows > 32767 Then
    hsclCurrRow.Value = (mrsFormRecordset.PercentPosition * 32767) / 100 + 1
  ElseIf mlNumRows > 99 Then
    hsclCurrRow.Value = (mrsFormRecordset.PercentPosition * mlNumRows) / 100 + 1
  Else
    hsclCurrRow.Value = mrsFormRecordset.PercentPosition
  End If
  mrsFormRecordset.Bookmark = sBookMark
  
  Exit Sub
  
MVErr:
  ShowError
End Sub



Private Sub hsclCurrRow_Change()
  On Error GoTo SCRErr
  
  Static nPrevVal As Integer
  Dim rsTmp As Recordset

  'based on number of rows, there is different logic needed
  'to set the current position in the recordset
  If mlNumRows > 0 Then
    If mlNumRows > 99 Then   '32767 Then
      'if there are > 32767 we need to use the move methods because
      'the scrollbar is limited to 32767 so if we didn't apply this
      'logic, it would be impossible to get to every record on a
      'small change of the scrollbar
      If hsclCurrRow.Value - nPrevVal = 1 Then
        mrsFormRecordset.MoveNext
        If mrsFormRecordset.EOF Then
          mrsFormRecordset.MoveLast
        End If
      ElseIf hsclCurrRow.Value - nPrevVal = -1 Then
        mrsFormRecordset.MovePrevious
        If mrsFormRecordset.BOF Then
          mrsFormRecordset.MoveFirst
        End If
      Else
        If mlNumRows > 32767 Then
          mrsFormRecordset.PercentPosition = (hsclCurrRow.Value / 32767) * 100 + 0.005
        Else
          mrsFormRecordset.PercentPosition = (hsclCurrRow.Value / mlNumRows) * 100 + 0.005
        End If
      End If
      nPrevVal = hsclCurrRow.Value
    Else
      mrsFormRecordset.PercentPosition = hsclCurrRow.Value
    End If
  End If
  DisplayCurrentRecord Me, mrsFormRecordset, mlNumRows, mbAddNewFlag
  mbDataChanged = False

  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False
  Exit Sub

SCRErr:
  ShowError
End Sub


Private Sub txtFieldData_Change(Index As Integer)
  'just set the flag if data is changed
  'it gets reset to false when a new record is displayed
  mbDataChanged = True
End Sub

Private Sub txtFieldData_KeyDown(Index As Integer, KeyCode As Integer, Shift As Integer)
  If KeyCode = &H73 Then   'F4
    lblFieldName_DblClick Index

  ElseIf KeyCode = 34 And vsbScrollBar.Visible Then
    'pagedown with > 10 fields
    vsbScrollBar.Value = vsbScrollBar.Value - 3000

  ElseIf KeyCode = 33 And vsbScrollBar.Visible Then
    'pageup with > 10 fields
    vsbScrollBar.Value = vsbScrollBar.Value + 3000

  End If
End Sub

Private Sub txtFieldData_KeyPress(Index As Integer, KeyAscii As Integer)
  'only allow return when in edit of add mode
  If mbEditFlag Or mbAddNewFlag Then
    If KeyAscii = 13 Then
      KeyAscii = 0
      SendKeys "{Tab}"
    End If

  'throw away the keystrokes if not in add or edit mode
  ElseIf mbEditFlag = False And mbAddNewFlag = False Then
    KeyAscii = 0
  End If

End Sub

Private Sub txtFieldData_LostFocus(Index As Integer)
  On Error GoTo FldDataErr

  If mbDataChanged Then
    'store the data in the field
    mrsFormRecordset(Index) = txtFieldData(Index)
  End If

  'reset for valid or error condition
  mbDataChanged = False
  Exit Sub

FldDataErr:
  'reset for valid or error condition
  mbDataChanged = False
  ShowError
End Sub

Private Sub lblFieldName_DblClick(Index As Integer)
  On Error GoTo ZoomErr

  If mrsFormRecordset(Index).Type = dbText Or mrsFormRecordset(Index).Type = dbMemo Then
     If mrsFormRecordset(Index).Type = dbText Then
       gsZoomData = txtFieldData(Index).Text
     ElseIf mrsFormRecordset(Index).FieldSize() < gnGETCHUNK_CUTOFF Then
       gsZoomData = txtFieldData(Index).Text
     Else
       'add the rest of the field data with getchunk
       MsgBar "Getting Memo Field Data", True
       Screen.MousePointer = vbHourglass
       gsZoomData = txtFieldData(Index).Text & _
         StripNonAscii(mrsFormRecordset(Index).GetChunk(gnGETCHUNK_CUTOFF, gnMAX_MEMO_SIZE))
       Screen.MousePointer = vbDefault
       MsgBar vbNullString, False
     End If
     frmZoom.Caption = Mid(lblFieldName(Index).Caption, 1, Len(lblFieldName(Index).Caption) - 1)
     If mbAddNewFlag Or mbEditFlag Then
       frmZoom.cmdSave.Visible = True
       frmZoom.cmdCloseNoSave.Visible = True
     Else
       frmZoom.cmdClose.Visible = True
     End If
     If mrsFormRecordset(Index).Type = dbText Then
       frmZoom.txtZoomData.Text = gsZoomData
       frmZoom.Height = 1125
     Else
       frmZoom.txtMemo.Text = gsZoomData
       frmZoom.txtMemo.Visible = True
       frmZoom.txtZoomData.Visible = False
       frmZoom.Height = 2205
     End If

     frmZoom.Show vbModal
     If (mbAddNewFlag Or mbEditFlag) And gsZoomData <> "__CANCELLED__" Then
       If mrsFormRecordset(Index).Type = dbText And Len(gsZoomData) > mrsFormRecordset(Index).Size Then
         Beep
         MsgBox MSG4, 48
         txtFieldData(Index).Text = Mid(gsZoomData, 1, mrsFormRecordset(Index).Size)
       Else
         txtFieldData(Index).Text = gsZoomData
       End If
       mrsFormRecordset(Index) = txtFieldData(Index).Text
       mbDataChanged = False
     End If
  End If
  Exit Sub

ZoomErr:
  ShowError
End Sub

Private Sub cmdClose_Click()
  DBEngine.Idle dbFreeLocks
  Unload Me
End Sub

Private Sub vsbScrollBar_Change()
  Dim nTop As Integer

  nTop = vsbScrollBar.Value
  If (nTop - 1080) Mod gnCTLARRAYHEIGHT = 0 Then
    picFields.Top = nTop
  Else
    picFields.Top = ((nTop - 1080) \ gnCTLARRAYHEIGHT) * gnCTLARRAYHEIGHT + 1080
  End If

End Sub

Private Sub cmdDelete_Click()
  On Error GoTo DelRecErr

  If MsgBox(MSG5, vbYesNo + vbQuestion) = vbYes Then
    mrsFormRecordset.Delete
    If gbTransPending Then gbDBChanged = True
    If mrsFormRecordset.EOF = False Then
      'see if we can move to the next record
      mrsFormRecordset.MoveNext
      If mrsFormRecordset.EOF And (mrsFormRecordset.RecordCount > 0) Then
        'must've been the last record so we can't move next
        mrsFormRecordset.MoveLast
      End If
    End If
    mlNumRows = mlNumRows - 1
    SetScrollBar
    mlNumRows = mrsFormRecordset.RecordCount
    DisplayCurrentRecord Me, mrsFormRecordset, mlNumRows, mbAddNewFlag
    mbDataChanged = False
  End If

  Exit Sub

DelRecErr:
  ShowError
End Sub

Private Sub cmdEdit_Click()
   On Error GoTo EditErr

  Dim nDelay As Long
  Dim nRetryCnt As Integer
  
  Screen.MousePointer = vbHourglass
RetryEdit:
   mrsFormRecordset.Edit
   lblStatus.Caption = MSG6
   mbEditFlag = True
   txtFieldData(0).SetFocus
   mvBookMark = mrsFormRecordset.Bookmark

   picChangeButtons.Visible = True
   picViewButtons.Visible = False
   hsclCurrRow.Enabled = False

   Screen.MousePointer = vbDefault
   Exit Sub

EditErr:
  If Err = 3260 And nRetryCnt < gnMURetryCnt Then
    nRetryCnt = nRetryCnt + 1
    DBEngine.Idle dbFreeLocks
    'Wait gnMUDelay seconds
    nDelay = Timer
    While Timer - nDelay < gnMUDelay
      'do nothing
    Wend
    Resume RetryEdit
  Else
    ShowError
  End If
End Sub

Private Sub cmdFilter_Click()
  On Error GoTo FilterErr

  Dim sBookMark As String
  Dim recRecordset1 As Recordset, recRecordset2 As Recordset
  Dim sFilterStr As String

  If mrsFormRecordset.RecordCount = 0 Then Exit Sub

  sBookMark = mrsFormRecordset.Bookmark        'save the bookmark
  Set recRecordset1 = mrsFormRecordset            'save the recordset
  
  sFilterStr = InputBox(MSG7)
  If Len(sFilterStr) = 0 Then Exit Sub

  Screen.MousePointer = vbHourglass
  MsgBar MSG8, True
  mrsFormRecordset.Filter = sFilterStr
  Set recRecordset2 = mrsFormRecordset.OpenRecordset(mrsFormRecordset.Type) 'establish the filter
  'force population to get an accurate recordcount
  recRecordset2.MoveLast
  recRecordset2.MoveFirst
  Set mrsFormRecordset = recRecordset2            'assign back to original recordset object

  'everything must be okay so redisplay form on 1st record
  mlNumRows = mrsFormRecordset.RecordCount
  SetScrollBar
  hsclCurrRow.Value = 0
  DisplayCurrentRecord Me, mrsFormRecordset, mlNumRows, mbAddNewFlag
  mbDataChanged = False
  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False
  Exit Sub
  
FilterRecover:
  On Error Resume Next
  Set mrsFormRecordset = recRecordset1            're-assign back to original
  mrsFormRecordset.Bookmark = sBookMark           'go back to original record
  Exit Sub

FilterErr:
  ShowError
  Resume FilterRecover
End Sub

Private Sub cmdFind_Click()
  On Error GoTo FindErr
  
  Dim i As Integer
  Dim sBookMark As String
  Dim sTmp As String

  'load the column names into the find form
  If mfrmFind.lstFields.ListCount = 0 Then
    For i = 0 To mrsFormRecordset.Fields.Count - 1
      mfrmFind.lstFields.AddItem Mid(lblFieldName(i).Caption, 1, Len(lblFieldName(i).Caption) - 1)
    Next
  End If

FindStart:

  'reset the flags
  gbFindFailed = False
  gbFromTableView = False
  mbNotFound = False

  MsgBar MSG9, False
  mfrmFind.Show vbModal
  MsgBar MSG10, True
  If gbFindFailed Then    'find cancelled
    GoTo AfterWhile
  End If

  Screen.MousePointer = vbHourglass

  i = mfrmFind.lstFields.ListIndex
  sBookMark = mrsFormRecordset.Bookmark
  'search for the record
  If mrsFormRecordset(i).Type = dbText Or mrsFormRecordset(i).Type = dbMemo Then
    sTmp = AddBrackets((mrsFormRecordset(i).Name)) & " " & gsFindOp & " '" & gsFindExpr & "'"
  ElseIf mrsFormRecordset(i).Type = dbDate Then
    sTmp = AddBrackets((mrsFormRecordset(i).Name)) & " " & gsFindOp & " #" & gsFindExpr & "#"
  Else
    sTmp = AddBrackets((mrsFormRecordset(i).Name)) & gsFindOp & Val(gsFindExpr)
  End If
  Select Case gnFindType
    Case 0
      mrsFormRecordset.FindFirst sTmp
    Case 1
      mrsFormRecordset.FindNext sTmp
    Case 2
      mrsFormRecordset.FindPrevious sTmp
    Case 3
      mrsFormRecordset.FindLast sTmp
  End Select
  mbNotFound = mrsFormRecordset.NoMatch

AfterWhile:

  Screen.MousePointer = vbDefault

  If gbFindFailed Then    'go back to original row
    mrsFormRecordset.Bookmark = sBookMark
  ElseIf mbNotFound Then
    Beep
    MsgBox MSG11, 48
    mrsFormRecordset.Bookmark = sBookMark
    GoTo FindStart
  Else
    sBookMark = mrsFormRecordset.Bookmark  'save the new position
    'now we need to reposition the scrollbar to reflect the move
    If mlNumRows > 99 Then
      hsclCurrRow.Value = (mrsFormRecordset.PercentPosition * mlNumRows) / 100 + 1
    Else
      hsclCurrRow.Value = mrsFormRecordset.PercentPosition
    End If
    mrsFormRecordset.Bookmark = sBookMark
  End If

  DisplayCurrentRecord Me, mrsFormRecordset, mlNumRows, mbAddNewFlag
  mbDataChanged = False

  MsgBar vbNullString, False
  Exit Sub

FindErr:
  Screen.MousePointer = vbDefault
  If Err <> gnEOF_ERR Then
    ShowError
  Else
    mbNotFound = True
    Resume Next
  End If
End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
  If KeyCode = vbKeyF1 And Shift = 0 Then
    DisplayTopic 2016125
  End If
End Sub

Private Sub Form_Load()
  Dim sTmp As String             'temp recordset name string
  Dim nFieldType As Integer      'field type of current field
  Dim i As Integer, j As Integer 'indexes

  On Error GoTo DynasetErr
   
   
  cmdAdd.Caption = BUTTON1
  cmdEdit.Caption = BUTTON2
  cmdDelete.Caption = BUTTON3
  cmdClose.Caption = BUTTON4
  cmdSort.Caption = BUTTON5
  cmdFilter.Caption = BUTTON6
  cmdMove.Caption = BUTTON7
  cmdFind.Caption = BUTTON8
  cmdCancel.Caption = BUTTON9
  cmdUpdate.Caption = BUTTON10
  lblFieldHdr.Caption = Label1
  lblFieldValue.Caption = Label2
  
  'mrsFormRecordset is a public module level variable
  'that must get set prior to 'Show'ing this form
   
   
  'set the locking type (comment out for standalone use)
  If gsDataType = gsMSACCESS And mrsFormRecordset.Type <> dbOpenSnapshot Then
    mrsFormRecordset.LockEdits = gnMULocking
  End If

  'get the row count
  With mrsFormRecordset
    If .RecordCount > 0 Then
      'move last, then first to get recordcount
      .MoveLast
      .MoveFirst
    End If
    mlNumRows = .RecordCount
  End With
  SetScrollBar

  'load the controls on the recordset form
  lblFieldName(0).Visible = True
  txtFieldData(0).Visible = True
  nFieldType = mrsFormRecordset(0).Type
  txtFieldData(0).Width = GetFieldWidth(nFieldType)
  If nFieldType = dbText Then txtFieldData(0).MaxLength = mrsFormRecordset(0).Size
  txtFieldData(0).TabIndex = 0
  For i = 1 To mrsFormRecordset.Fields.Count - 1
    picFields.Height = picFields.Height + gnCTLARRAYHEIGHT
    Load lblFieldName(i)
    lblFieldName(i).Top = lblFieldName(i - 1).Top + gnCTLARRAYHEIGHT
    lblFieldName(i).Visible = True
    Load txtFieldData(i)
    txtFieldData(i).Top = txtFieldData(i - 1).Top + gnCTLARRAYHEIGHT
    txtFieldData(i).Visible = True
    nFieldType = mrsFormRecordset.Fields(i).Type
    txtFieldData(i).Width = GetFieldWidth(nFieldType)
    If nFieldType = dbText Then txtFieldData(i).MaxLength = mrsFormRecordset(i).Size
    txtFieldData(i).TabIndex = i
  Next

  'resize main window
  Me.Width = 5580
  If i <= 10 Then
    Me.Height = ((i + 1) * gnCTLARRAYHEIGHT) + 1600
  Else
    Me.Height = 4368
    Me.Width = Me.Width + 260
    vsbScrollBar.Visible = True
    vsbScrollBar.Min = 1080
    vsbScrollBar.Max = 1080 - (i * gnCTLARRAYHEIGHT) + 2240
  End If

  'display the field names
  For i = 0 To mrsFormRecordset.Fields.Count - 1
    lblFieldName(i).Caption = mrsFormRecordset(i).Name & ":"
  Next

  DisplayCurrentRecord Me, mrsFormRecordset, mlNumRows, mbAddNewFlag
  mbDataChanged = False

  Me.Left = 1000
  Me.Top = 1000
  
  MsgBar vbNullString, False
  Exit Sub

DynasetErr:
  ShowError
  Unload Me
End Sub

Private Sub Form_Resize()
  On Error Resume Next

  Dim nHeight As Integer
  Dim i As Integer
  Dim nTotWidth As Integer
  Const nHeightFactor = 1420

  If WindowState <> 1 Then   'not minimized
    MsgBar MSG12, True
    'make sure the form is lined up on a field
    nHeight = Height
    If (nHeight - nHeightFactor) Mod gnCTLARRAYHEIGHT <> 0 Then
      Me.Height = ((nHeight - nHeightFactor) \ gnCTLARRAYHEIGHT) * gnCTLARRAYHEIGHT + nHeightFactor
    End If
    'resize the status bar
    picMoveButtons.Top = Me.Height - 650
    'resize the scrollbar
    vsbScrollBar.Height = picMoveButtons.Top - (picViewButtons.Top - picFldHdr.Height) - 1320
    vsbScrollBar.Left = Me.Width - 360
    If mrsFormRecordset.Fields.Count > 10 Then
      picFields.Width = Me.Width - 260
      nTotWidth = vsbScrollBar.Left - 20
    Else
      picFields.Width = Me.Width - 20
      nTotWidth = Me.Width - 50
    End If
    picFldHdr.Width = Me.Width - 20
    'widen the fields if possible
    For i = 0 To mrsFormRecordset.Fields.Count - 1
      lblFieldName(i).Width = 0.3 * nTotWidth
      txtFieldData(i).Left = lblFieldName(i).Width + 20
      If mrsFormRecordset(i).Type = dbText Or mrsFormRecordset(i).Type = dbMemo Then
        txtFieldData(i).Width = 0.7 * nTotWidth - 250
      End If
    Next
    lblFieldValue.Left = txtFieldData(0).Left
    hsclCurrRow.Width = picMoveButtons.Width \ 2
    lblStatus.Width = picMoveButtons.Width \ 2
    lblStatus.Left = hsclCurrRow.Width + 10
  End If
  MsgBar vbNullString, False
End Sub

Private Sub Form_Unload(Cancel As Integer)
  On Error Resume Next

  Unload mfrmFind   'get rid of attached find form
  mrsFormRecordset.Close          'close the form recordset
  DBEngine.Idle dbFreeLocks
  MsgBar vbNullString, False
End Sub

Private Sub cmdSort_Click()
  On Error GoTo SortErr

  Dim sBookMark As String
  Dim recRecordset1 As Recordset, recRecordset2 As Recordset
  Dim SortStr As String

  If mrsFormRecordset.RecordCount = 0 Then Exit Sub

  sBookMark = mrsFormRecordset.Bookmark        'save the bookmark
  Set recRecordset1 = mrsFormRecordset            'save the recordset
  
  SortStr = InputBox(MSG13)
  If Len(SortStr) = 0 Then Exit Sub

  Screen.MousePointer = vbHourglass
  MsgBar MSG14, True
  mrsFormRecordset.Sort = SortStr
  'establish the Sort
  Set recRecordset2 = mrsFormRecordset.OpenRecordset(mrsFormRecordset.Type)
  Set mrsFormRecordset = recRecordset2            'assign back to original recordset object

  'everything must be okay so redisplay form on 1st record
  mlNumRows = mrsFormRecordset.RecordCount
  hsclCurrRow.Value = 0
  DisplayCurrentRecord Me, mrsFormRecordset, mlNumRows, mbAddNewFlag
  mbDataChanged = False
  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False
  Exit Sub

SortRecover:
  On Error Resume Next
  Set mrsFormRecordset = recRecordset1            're-assign back to original
  mrsFormRecordset.Bookmark = sBookMark        'go back to original record
  Exit Sub

SortErr:
  ShowError
  Resume SortRecover
End Sub

Private Sub cmdUpdate_Click()
  On Error GoTo UpdateErr

  Dim nDelay As Long
  Dim nRetryCnt As Integer

  Screen.MousePointer = vbHourglass
RetryUpd:
  mrsFormRecordset.Update
  If gbTransPending Then gbDBChanged = True

  If mbAddNewFlag Then
    mlNumRows = mlNumRows + 1
    SetScrollBar
    'move to the new record
    mrsFormRecordset.MoveLast
  End If

  picChangeButtons.Visible = False
  picViewButtons.Visible = True
  hsclCurrRow.Enabled = True
  mbEditFlag = False
  mbAddNewFlag = False
  hsclCurrRow_Change
  DBEngine.Idle dbFreeLocks

  Screen.MousePointer = vbDefault
  Exit Sub

UpdateErr:
  'check for locked error
  If Err = 3260 And nRetryCnt < gnMURetryCnt Then
    nRetryCnt = nRetryCnt + 1
    mrsFormRecordset.Bookmark = mrsFormRecordset.Bookmark   'Cancel the update
    DBEngine.Idle dbFreeLocks
    nDelay = Timer
    'Wait gnMUDelay seconds
    While Timer - nDelay < gnMUDelay
      'do nothing
    Wend
    Resume RetryUpd
  Else
    ShowError
  End If
End Sub

Private Sub SetScrollBar()
  On Error Resume Next
  
  If mlNumRows < 2 Then
    hsclCurrRow.Max = 100
    hsclCurrRow.SmallChange = 1 '00
    hsclCurrRow.LargeChange = 100
  ElseIf mlNumRows > 32767 Then
    hsclCurrRow.Max = 32767
    hsclCurrRow.SmallChange = 1
    hsclCurrRow.LargeChange = 1000
  ElseIf mlNumRows > 99 Then
    hsclCurrRow.Max = mlNumRows
    hsclCurrRow.SmallChange = 1
    hsclCurrRow.LargeChange = mlNumRows \ 20
  Else
    'must be between 2 and 100
    hsclCurrRow.Max = 100
    hsclCurrRow.SmallChange = 100 \ (mlNumRows - 1)
    hsclCurrRow.LargeChange = (100 \ (mlNumRows - 1)) * 10
  End If
  'move off, then back on to fix flashing bar
  txtFieldData(0).SetFocus
  hsclCurrRow.SetFocus
End Sub

