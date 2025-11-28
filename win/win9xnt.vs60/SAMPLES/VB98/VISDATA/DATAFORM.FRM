VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "ComDlg32.ocx"
Begin VB.Form frmDataControl 
   Caption         =   "Data Control"
   ClientHeight    =   2160
   ClientLeft      =   1230
   ClientTop       =   3075
   ClientWidth     =   5775
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   HelpContextID   =   2016122
   Icon            =   "DATAFORM.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form2"
   LockControls    =   -1  'True
   MDIChild        =   -1  'True
   ScaleHeight     =   2160
   ScaleWidth      =   5775
   ShowInTaskbar   =   0   'False
   Tag             =   "Recordset"
   Begin VB.PictureBox picButtons 
      Align           =   1  'Align Top
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
      Height          =   600
      Left            =   0
      ScaleHeight     =   600
      ScaleWidth      =   5775
      TabIndex        =   0
      Top             =   0
      Width           =   5772
      Begin VB.CommandButton cmdCancelAdd 
         Caption         =   "C&ancel"
         Height          =   330
         Left            =   0
         MaskColor       =   &H00000000&
         TabIndex        =   13
         Top             =   0
         Visible         =   0   'False
         Width           =   960
      End
      Begin VB.CommandButton cmdRefresh 
         Caption         =   "&Refresh"
         Height          =   330
         Left            =   3840
         MaskColor       =   &H00000000&
         TabIndex        =   12
         Top             =   0
         Width           =   960
      End
      Begin VB.CommandButton cmdFind 
         Caption         =   "&Find"
         Height          =   330
         Left            =   2880
         MaskColor       =   &H00000000&
         TabIndex        =   5
         Top             =   0
         Width           =   960
      End
      Begin VB.CommandButton cmdClose 
         Cancel          =   -1  'True
         Caption         =   "&Close"
         Height          =   330
         Left            =   4800
         MaskColor       =   &H00000000&
         TabIndex        =   4
         Top             =   0
         Width           =   960
      End
      Begin VB.CommandButton cmdDelete 
         Caption         =   "&Delete"
         Height          =   330
         Left            =   1920
         MaskColor       =   &H00000000&
         TabIndex        =   3
         Top             =   0
         Width           =   960
      End
      Begin VB.CommandButton cmdAdd 
         Caption         =   "&Add"
         Height          =   330
         Left            =   0
         MaskColor       =   &H00000000&
         TabIndex        =   2
         Top             =   0
         Width           =   960
      End
      Begin VB.CommandButton cmdUpdate 
         Caption         =   "&Update"
         Height          =   330
         Left            =   960
         MaskColor       =   &H00000000&
         TabIndex        =   1
         Top             =   0
         Width           =   960
      End
      Begin VB.Label lblFieldHeader 
         Caption         =   "Field Name:"
         Height          =   255
         Left            =   120
         TabIndex        =   15
         Top             =   360
         Width           =   1215
      End
      Begin VB.Label lblFieldValue 
         Caption         =   " Value:"
         Height          =   255
         Left            =   1680
         TabIndex        =   14
         Top             =   360
         Width           =   2655
      End
   End
   Begin VB.Data datDataCtl 
      Align           =   2  'Align Bottom
      Connect         =   "Access"
      DatabaseName    =   ""
      DefaultCursorType=   0  'DefaultCursor
      DefaultType     =   2  'UseODBC
      Exclusive       =   0   'False
      Height          =   345
      Left            =   0
      Options         =   0
      ReadOnly        =   0   'False
      RecordsetType   =   1  'Dynaset
      RecordSource    =   ""
      Tag             =   "OLE"
      Top             =   1815
      Width           =   5772
   End
   Begin VB.VScrollBar vsbScrollBar 
      Height          =   2440
      LargeChange     =   3000
      Left            =   7665
      SmallChange     =   300
      TabIndex        =   11
      Top             =   630
      Visible         =   0   'False
      Width           =   255
   End
   Begin VB.PictureBox picFields 
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
      Height          =   1065
      Left            =   0
      ScaleHeight     =   1056.479
      ScaleMode       =   0  'User
      ScaleWidth      =   7600.264
      TabIndex        =   6
      TabStop         =   0   'False
      Top             =   600
      Width           =   7605
      Begin VB.TextBox txtFieldData 
         DataSource      =   "datDataCtl"
         ForeColor       =   &H00000000&
         Height          =   285
         Index           =   0
         Left            =   1665
         TabIndex        =   9
         Top             =   0
         Visible         =   0   'False
         Width           =   3255
      End
      Begin VB.CheckBox chkFieldData 
         DataSource      =   "datDataCtl"
         Height          =   282
         Index           =   0
         Left            =   1680
         MaskColor       =   &H00000000&
         TabIndex        =   8
         Top             =   735
         Visible         =   0   'False
         Width           =   3270
      End
      Begin VB.PictureBox picFieldData 
         BackColor       =   &H80000005&
         DataSource      =   "datDataCtl"
         Height          =   285
         Index           =   0
         Left            =   1680
         ScaleHeight     =   225
         ScaleWidth      =   3210
         TabIndex        =   7
         Top             =   360
         Visible         =   0   'False
         Width           =   3270
      End
      Begin VB.OLE oleFieldData 
         BackColor       =   &H00FFFFFF&
         DataSource      =   "datDataCtl"
         DisplayType     =   1  'Icon
         Height          =   300
         Index           =   0
         Left            =   1680
         TabIndex        =   16
         Top             =   360
         Visible         =   0   'False
         Width           =   3255
      End
      Begin VB.Label lblFieldName 
         ForeColor       =   &H00000000&
         Height          =   195
         Index           =   0
         Left            =   105
         TabIndex        =   10
         Top             =   0
         Visible         =   0   'False
         Width           =   300
      End
   End
   Begin MSComDlg.CommonDialog dlgCMD1 
      Left            =   2415
      Top             =   1755
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
      FilterIndex     =   1019
      FontSize        =   1.74012e-39
   End
End
Attribute VB_Name = "frmDataControl"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'>>>>>>>>>>>>>>>>>>>>>>>>
Const BUTTON1 = "&Cancel"
Const BUTTON2 = "&Update"
Const BUTTON3 = "&Delete"
Const BUTTON4 = "&Find"
Const BUTTON5 = "&Refresh"
Const BUTTON6 = "&Close"
Const BUTTON7 = "&Add"
Const Label1 = "Field Name:"
Const Label2 = "Value:"
Const MSG1 = "New Record"
Const MSG2 = "Select a Picture File to Load"
Const MSG3 = "Data error event hit Err:"
Const MSG4 = "Save New Record?"
Const MSG5 = "Commit Changes?"
Const MSG6 = "Commit Changes before Closing?"
Const MSG7 = "Delete Current Record?"
Const MSG8 = "Enter Seek Value:"
Const MSG9 = "Enter Search Expression:"
Const MSG10 = " Rows"
Const MSG11 = " Rows (Forward Only Recordset)"
Const MSG12 = "  [Not Updatable]"
Const MSG13 = "Function not available for paramaterized query!"
'>>>>>>>>>>>>>>>>>>>>>>>>

'============================================================================
' This is a fairly generic form that can be used in most cases with any
' table or querydef.
'============================================================================

Dim maFldArr() As Object
Public mrsFormRecordset As Recordset
Public mbIsParameterized As Boolean
Dim mvBookMark As Variant        'form bookmark
Dim mnNumFields As Integer       'number of fields
Dim mlNumRows As Long            'recordcount for recordset
Dim mbJustUsedFind As Boolean    'flag for find function
Dim mbResizing As Boolean        'flag to avoid resize recursion
Dim mbCancel As Boolean          'flag to cancel an addnew
Dim mnFieldTop As Integer        'top field position

Const mnMSGBOX_TYPE = vbYesNo + vbQuestion
Const mnCTLARRAYHEIGHT = 340

Private Sub cmdAdd_Click()
  On Error GoTo AddErr

  datDataCtl.Recordset.AddNew
  datDataCtl.Caption = MSG1
  cmdCancelAdd.Visible = True
  cmdAdd.Visible = False
  If datDataCtl.Recordset.RecordCount <> 0 Then
    mvBookMark = datDataCtl.Recordset.Bookmark
    maFldArr(0).SetFocus
  End If

  Exit Sub
  
AddErr:
  ShowErrMsg

End Sub

Private Sub cmdCancelAdd_Click()
  On Error Resume Next

  mbCancel = True
  'go back to the previous current record
  If Len(mvBookMark) > 0 Then
    datDataCtl.Recordset.Bookmark = mvBookMark
  End If

End Sub

'----------------------------------------------------------
'this sub loads the Data Control property sheet
'comment it out for standalone use
'----------------------------------------------------------
Sub datDataCtl_MouseUp(BUTTON As Integer, Shift As Integer, x As Single, y As Single)
  On Error GoTo DCPErr

  Dim i As Integer
  Dim recClone As Recordset
  Dim sTmpRS As String
  Dim sTmpDB As String
  Dim sTmpTag As String
  
  If BUTTON = 2 Then
    If mbIsParameterized Then
      MsgBox MSG13, vbInformation
      Exit Sub
    End If
  
    Screen.MousePointer = vbHourglass
    sTmpRS = datDataCtl.RecordSource
    sTmpDB = datDataCtl.DatabaseName
    sTmpTag = datDataCtl.Tag
    Set gDataCtlObj = datDataCtl
    frmDataCtlProp.Show vbModal
    If Not gDataCtlObj Is Nothing Then
      'check for a changed recordset, db or bound binary type
      'and clear the bound fields if one was changed
      If (sTmpRS <> gDataCtlObj.RecordSource Or sTmpDB <> gDataCtlObj.DatabaseName) _
         Or gDataCtlObj.Tag <> sTmpTag Then
        'clear the field names and unbind the controls
        For i = 0 To mnNumFields - 1
          lblFieldName(i).Caption = vbNullString
          maFldArr(i).DataField = vbNullString
          maFldArr(i).Visible = False
        Next
      End If
      datDataCtl.Refresh
      If (sTmpRS <> gDataCtlObj.RecordSource Or sTmpDB <> gDataCtlObj.DatabaseName) _
         Or gDataCtlObj.Tag <> sTmpTag Then
        Set recClone = datDataCtl.Recordset.Clone()
        If recClone.BOF = False And (datDataCtl.Options And dbForwardOnly) = 0 Then
          recClone.MoveLast
          mlNumRows = recClone.RecordCount
        Else
          mlNumRows = 0
        End If
        recClone.Close
        LoadFields
        SetRecNum
      Else
        'need to refresh the local recordset copy for
        'other operations that use it such as property access
        Set mrsFormRecordset = datDataCtl.Recordset
        'if it is a forward only recordset, then we need to call SetRecNum
        'to display the correct data control caption
        If (datDataCtl.Options And dbForwardOnly) = dbForwardOnly Then
          SetRecNum
        End If
      End If
      gbSettingDataCtl = False
      If gDataCtlObj.Tag <> sTmpTag Then
        Form_Resize    'needed to set the left prop on newly loaded ctls
      End If
    End If
    
  End If
  Exit Sub
  
DCPErr:
  ShowError
  Unload Me
End Sub

Private Sub Form_Unload(Cancel As Integer)
  DBEngine.Idle dbFreeLocks
End Sub

Private Sub oleFieldData_MouseUp(Index As Integer, BUTTON As Integer, Shift As Integer, x As Single, y As Single)
  If BUTTON <> 2 Then Exit Sub
  'this will display the dialog that allows
  'entry in to the ole field
  oleFieldData(Index).InsertObjDlg
End Sub

Private Sub txtFieldData_KeyPress(Index As Integer, KeyAscii As Integer)
  'go to next field on an enter keypress
  If KeyAscii = 13 Then
    KeyAscii = 0
    SendKeys "{Tab}"
  End If
End Sub

Private Sub picFieldData_Click(Index As Integer)
  'this toggles the size of a picture control
  'so it mat be viewed or compressed
  If picFieldData(Index).Height <= 280 Then
    picFieldData(Index).AutoSize = True
  Else
    picFieldData(Index).AutoSize = False
    picFieldData(Index).Height = 280
  End If
End Sub

Private Sub picFieldData_DblClick(Index As Integer)
  On Error GoTo PicErr

  With dlgCMD1
    .Filter = "Bitmaps (*.bmp)|*.bmp|Icons (*.ico)|*.ico|Metafiles (*.wmf)|*.wmf|All Files (*.*)|*.*"
    .DialogTitle = MSG2
    .FilterIndex = 1
    .ShowOpen
  
    If Len(.FileName) > 0 Then
      picFieldData(Index).Picture = LoadPicture(.FileName)
    End If
  End With

  Exit Sub
  
PicErr:
  ShowErrMsg
  Exit Sub

End Sub

Private Sub cmdClose_Click()
  On Error Resume Next

  Unload Me
End Sub

Private Sub vsbScrollBar_Change()
  Dim nCurrVal As Integer

  nCurrVal = vsbScrollBar
  If (nCurrVal - mnFieldTop) Mod mnCTLARRAYHEIGHT = 0 Then
    picFields.Top = nCurrVal
  Else
    picFields.Top = ((nCurrVal - mnFieldTop) \ mnCTLARRAYHEIGHT) * mnCTLARRAYHEIGHT + mnFieldTop
  End If

End Sub

Private Sub datDataCtl_Error(DataErr As Integer, Response As Integer)
  If DataErr = 481 Then  'throw away bad picture error
    Response = vbDataErrContinue
  Else
    MsgBox MSG3 & Error(DataErr)
  End If
End Sub

Private Sub datDataCtl_RePosition()
  On Error GoTo RepErr

  Dim sBookMark As String
  Dim recClone As Recordset
  
  'if we are resetting the data control, we need to skip this sub
  If gbSettingDataCtl Then Exit Sub
  
  'if the recordset is empty and we are not already in AddNew mode
  'we need to call the cmdAdd_Click routine to execute an AddNew
  If (datDataCtl.Recordset.RecordCount = 0) And _
     (datDataCtl.EditMode <> dbEditAdd) And _
      datDataCtl.Recordset.Updatable Then
    Call cmdAdd_Click
    Exit Sub
  End If

  SetRecNum
  
  Exit Sub
  
RepErr:
  ShowErrMsg
  Exit Sub

End Sub

Private Sub datDataCtl_Validate(Action As Integer, Save As Integer)
  On Error GoTo ValErr

  If mbCancel Then
    Save = False
    mbCancel = False
    Exit Sub
  End If

  'first check for a move from an addnew or edit record
  If Action < 5 Then
    If Save Then       'data changed
      If datDataCtl.EditMode = dbEditAdd Then
        If MsgBox(MSG4, mnMSGBOX_TYPE) = vbYes Then
          mlNumRows = mlNumRows + 1
        Else
          Save = False
        End If
      Else
        If MsgBox(MSG5, mnMSGBOX_TYPE) <> vbYes Then
          Save = False        'loose changes
        End If
      End If
    End If
  End If

  Select Case Action
    Case vbDataActionMoveFirst
      'do nothing
    Case vbDataActionMovePrevious
      'do nothing
    Case vbDataActionMoveNext
      'do nothing
    Case vbDataActionMoveLast
      'do nothing
    Case vbDataActionAddNew
      'do nothing
    Case vbDataActionUpdate
       'moved to the cmdUpdate_click event code
    Case vbDataActionDelete
      'do nothing
    Case vbDataActionFind
      'set the flag for use in the reposition event
      mbJustUsedFind = True
    Case vbDataActionBookmark
      'do nothing"
    Case vbDataActionClose, vbDataActionUnload
      If Save Then
        If MsgBox(MSG6, mnMSGBOX_TYPE) <> vbYes Then
          Save = False
        End If
      End If

  End Select

  Exit Sub

ValErr:
  ShowErrMsg
  Exit Sub
  
End Sub

Private Sub cmdDelete_Click()
  On Error GoTo DELErr

  If MsgBox(MSG7, mnMSGBOX_TYPE) = vbYes Then
    datDataCtl.Recordset.Delete
    mlNumRows = mlNumRows - 1
    datDataCtl.Recordset.MoveNext
    'move off the EOF condition if it occurs
    If datDataCtl.Recordset.RecordCount > 0 Then datDataCtl.Recordset.MoveLast
    maFldArr(0).SetFocus
  End If

  Exit Sub

DELErr:
  ShowErrMsg
  Exit Sub

End Sub

Private Sub cmdFind_Click()
  On Error GoTo FindErr

  Dim sBookMark As String
  Dim sFindStr As String

  If datDataCtl.Recordset.Type = dbOpenTable Then
    sFindStr = InputBox(MSG8)
  Else
    sFindStr = InputBox(MSG9)
  End If
  If Len(sFindStr) = 0 Then Exit Sub

  If datDataCtl.Recordset.RecordCount > 0 Then
    sBookMark = datDataCtl.Recordset.Bookmark
  End If

  If datDataCtl.Recordset.Type = dbOpenTable Then
    datDataCtl.Recordset.Seek "=", sFindStr
  Else
    datDataCtl.Recordset.FindFirst sFindStr
  End If

  'return to old record if no match was found
  If datDataCtl.Recordset.NoMatch And Len(sBookMark) > 0 Then
    datDataCtl.Recordset.Bookmark = sBookMark
  End If

  maFldArr(0).SetFocus
  Exit Sub

FindErr:
  ShowErrMsg
  maFldArr(0).SetFocus
  Exit Sub

End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
  If KeyCode = vbKeyF1 And Shift = 0 Then
    DisplayTopic 2016122
  End If
End Sub

Private Sub Form_Load()
  On Error GoTo LoadErr

  cmdCancelAdd.Caption = BUTTON1
  cmdUpdate.Caption = BUTTON2
  cmdDelete.Caption = BUTTON3
  cmdFind.Caption = BUTTON4
  cmdRefresh.Caption = BUTTON5
  cmdClose.Caption = BUTTON6
  cmdAdd.Caption = BUTTON7
  lblFieldHeader.Caption = Label1
  lblFieldValue.Caption = Label2
  
  'mrsFormRecordset is a public module level variable
  'that must get set prior to 'Show'ing this form
  
  
  With mrsFormRecordset
    If .Type = dbOpenTable Then
      'need to set the index
      If gdbCurrentDB.TableDefs(.Name).Indexes.Count > 0 Then
        .Index = gdbCurrentDB.TableDefs(.Name).Indexes(0).Name
      End If
    End If
    If .RecordCount > 0 Then
      'move next, then previous to get recordcount
      .MoveLast
      .MoveFirst
    End If
  End With
  Set datDataCtl.Recordset = mrsFormRecordset
  
  Me.Width = 5868
  LoadFields
  Me.Show
  maFldArr(0).SetFocus
  Exit Sub

LoadErr:
  ShowErrMsg
  Unload Me
End Sub

Private Sub Form_Resize()
  On Error Resume Next

  If gbSettingDataCtl Then Exit Sub
  If mbResizing Then Exit Sub

  Dim nHeight As Integer
  Dim i As Integer
  Dim nTotalWidth As Integer

  mbResizing = True
  If Me.WindowState <> 1 And lblFieldName(0).Visible Then 'not minimized
    'make sure the form is lined up on a field
    nHeight = Me.Height
    If (nHeight - 1320) Mod mnCTLARRAYHEIGHT <> 0 Then
      Me.Height = ((nHeight - 1280) \ mnCTLARRAYHEIGHT) * mnCTLARRAYHEIGHT + 1280
    End If
    'resize the status bar
    datDataCtl.Top = Me.Height - 650
    'resize the scrollbar
    vsbScrollBar.Height = Me.Height - 1300
    vsbScrollBar.Left = Me.Width - 360
    If mrsFormRecordset.Fields.Count > 10 Then
      picFields.Width = Me.Width - 260
      nTotalWidth = vsbScrollBar.Left - 20
    Else
      picFields.Width = Me.Width - 20
      nTotalWidth = Me.Width - 50
    End If
    picButtons.Width = Me.Width - 20
    'widen the fields if possible
    For i = 0 To mrsFormRecordset.Fields.Count - 1
      lblFieldName(i).Width = 0.3 * nTotalWidth - 100
      maFldArr(i).Left = lblFieldName(i).Width + 200
      If datDataCtl.Recordset.Fields(i).Type > 9 Then
        maFldArr(i).Width = 0.7 * nTotalWidth - 270
      End If
    Next
    lblFieldValue.Left = maFldArr(0).Left

  End If

  mbResizing = False

End Sub

Private Function GetFieldWidth(rnType As Integer)
  'determines the form control width
  'based on the field type
  Select Case rnType
    Case dbBoolean
      GetFieldWidth = 850
    Case dbByte
      GetFieldWidth = 650
    Case dbInteger
      GetFieldWidth = 900
    Case dbLong
      GetFieldWidth = 1100
    Case dbCurrency
      GetFieldWidth = 1800
    Case dbSingle
      GetFieldWidth = 1800
    Case dbDouble
      GetFieldWidth = 2200
    Case dbDate
      GetFieldWidth = 2000
    Case dbText
      GetFieldWidth = 3250
    Case dbMemo
      GetFieldWidth = 3250
    Case Else
      GetFieldWidth = 3250
  End Select

End Function

Private Sub LoadFields()
   Dim recTmp As Recordset
   Dim nFldType As Integer
   Dim i As Integer

   On Error GoTo LoadFieldsErr

   Set mrsFormRecordset = datDataCtl.Recordset
   Set recTmp = mrsFormRecordset

   'load the controls on the recordset form
   mnNumFields = recTmp.Fields.Count
   ReDim maFldArr(mnNumFields) As Object
   lblFieldName(0).Visible = True
   nFldType = recTmp.Fields(0).Type
   If nFldType = dbBoolean Then
     Set maFldArr(0) = chkFieldData(0)
   ElseIf nFldType = dbLongBinary Then
     If datDataCtl.Tag = "OLE" Then
       Set maFldArr(0) = oleFieldData(0)
     Else
       Set maFldArr(0) = picFieldData(0)
     End If
   Else
     Set maFldArr(0) = txtFieldData(0)
   End If
   maFldArr(0).Visible = True
   maFldArr(0).Top = 0
   maFldArr(0).Width = GetFieldWidth(nFldType)
   If nFldType = dbText Then maFldArr(0).MaxLength = recTmp.Fields(0).Size

   maFldArr(0).TabIndex = 0
   On Error Resume Next
   For i = 1 To recTmp.Fields.Count - 1
     picFields.Height = picFields.Height + mnCTLARRAYHEIGHT
     Load lblFieldName(i)
     lblFieldName(i).Top = lblFieldName(i - 1).Top + mnCTLARRAYHEIGHT
     lblFieldName(i).Visible = True
     nFldType = recTmp.Fields(i).Type
     If nFldType = dbBoolean Then
       Load chkFieldData(i)
       Set maFldArr(i) = chkFieldData(i)
     ElseIf nFldType = dbLongBinary Then
       If datDataCtl.Tag = "OLE" Then
         Load oleFieldData(i)
         Set maFldArr(i) = oleFieldData(i)
       Else
         Load picFieldData(i)
         Set maFldArr(i) = picFieldData(i)
       End If
     Else
       Load txtFieldData(i)
       Set maFldArr(i) = txtFieldData(i)
     End If
     maFldArr(i).Top = maFldArr(i - 1).Top + mnCTLARRAYHEIGHT
     maFldArr(i).Visible = True
     maFldArr(i).Width = GetFieldWidth(nFldType)
     maFldArr(i).TabIndex = i
     If nFldType = dbText Then maFldArr(i).MaxLength = recTmp.Fields(i).Size
   Next

   On Error GoTo LoadFieldsErr

   'resize main window
   picFields.Top = picButtons.Top + picButtons.Height
   mnFieldTop = picFields.Top
   vsbScrollBar.Value = mnFieldTop
   If i <= 10 Then
     Height = i * mnCTLARRAYHEIGHT + 1500
     vsbScrollBar.Visible = False
   Else
     Height = 4500
     Width = Width + 260
     vsbScrollBar.Visible = True
     vsbScrollBar.Min = mnFieldTop
     vsbScrollBar.Max = mnFieldTop - (i * mnCTLARRAYHEIGHT) + 3000
   End If

   'display the field names
   For i = 0 To recTmp.Fields.Count - 1
     lblFieldName(i).Caption = recTmp.Fields(i).Name & ":"
   Next
   
   'bind the controls
   On Error Resume Next   'bind even if table is empty
   For i = 0 To recTmp.Fields.Count - 1
     maFldArr(i).DataField = recTmp.Fields(i).Name
   Next

   Exit Sub

LoadFieldsErr:
   ShowErrMsg
   Exit Sub

End Sub

Private Sub cmdRefresh_Click()
  On Error GoTo RefErr

  If mbIsParameterized Then
    MsgBox MSG13, vbInformation
    Exit Sub
  End If
  
  datDataCtl.Refresh
  Exit Sub

RefErr:
  ShowErrMsg
End Sub

Private Sub SetRecNum()
  On Error GoTo SRErr
  
  Dim sCurrStat As String
  Dim lCurrRec As Long
  Dim bNoInd As Integer
  
  'get the current record count
  mlNumRows = datDataCtl.Recordset.RecordCount
  
  If datDataCtl.EditMode <> dbEditAdd Then
    If datDataCtl.Recordset.BOF Then
      sCurrStat = "(BOF)/" & mlNumRows
    ElseIf datDataCtl.Recordset.EOF Then
      sCurrStat = "(EOF)/" & mlNumRows
    Else
      'check to see if a table w/ 0 indexes is in use
      If datDataCtl.Recordset.Type = dbOpenTable Then
        If datDataCtl.Database(datDataCtl.RecordSource).Indexes.Count = 0 Then
          bNoInd = True
        End If
      End If
      'if there are no indexes on a table or the recset if ForwardOnly
      'then the PercentPosition is unavailable
      If bNoInd Then
        sCurrStat = mlNumRows & MSG10
      ElseIf (datDataCtl.Options And dbForwardOnly) = dbForwardOnly Then
        sCurrStat = mlNumRows & MSG11
      Else
        lCurrRec = (mlNumRows * (datDataCtl.Recordset.PercentPosition * 0.01)) + 1
        sCurrStat = lCurrRec & "/" & mlNumRows
      End If
    End If
    If datDataCtl.Recordset.Updatable = False Then
      sCurrStat = sCurrStat & MSG12
      cmdAdd.Enabled = False
      cmdCancelAdd.Enabled = False
      cmdUpdate.Enabled = False
      cmdDelete.Enabled = False
    Else
      cmdAdd.Enabled = True
      cmdCancelAdd.Enabled = True
      cmdUpdate.Enabled = True
      cmdDelete.Enabled = True
    End If
    datDataCtl.Caption = sCurrStat
  End If

  'reset buttons if needed
  If datDataCtl.EditMode <> dbEditAdd Then
    cmdCancelAdd.Visible = False
    cmdAdd.Visible = True
  End If

  Exit Sub
SRErr:
  If Err <> 3021 Then
    ShowErrMsg
  End If
  Exit Sub

End Sub

Private Sub cmdUpdate_Click()
  On Error GoTo UpdErr

  Dim bAddFlag As Integer

  Dim nDelay As Long
  Dim nRetryCnt As Integer

  bAddFlag = datDataCtl.EditMode
  
  If datDataCtl.EditMode = dbEditAdd Then
    If MsgBox(MSG4, mnMSGBOX_TYPE) = vbYes Then
      Screen.MousePointer = vbHourglass
RetryUpd1:
      datDataCtl.UpdateRecord
      mlNumRows = mlNumRows + 1
    End If
  Else
    If MsgBox(MSG5, mnMSGBOX_TYPE) = vbYes Then
      Screen.MousePointer = vbHourglass
RetryUpd2:
      datDataCtl.UpdateRecord
    End If
  End If

  If bAddFlag = dbEditAdd Then
    mrsFormRecordset.MoveLast
  End If

  DBEngine.Idle dbFreeLocks
  Screen.MousePointer = vbDefault
  Exit Sub

UpdErr:
  If Err = 3260 And nRetryCnt < gnMURetryCnt Then
    nRetryCnt = nRetryCnt + 1
    datDataCtl.Recordset.Bookmark = datDataCtl.Recordset.Bookmark   'Cancel the update
    DBEngine.Idle dbFreeLocks
    nDelay = Timer
    'Wait gnMUDelay seconds
    While Timer - nDelay < gnMUDelay
      'do nothing
    Wend
    If datDataCtl.EditMode = dbEditAdd Then
      Resume RetryUpd1
    Else
      Resume RetryUpd2
    End If
  Else
    Screen.MousePointer = vbDefault
    ShowErrMsg
    Exit Sub
  End If

End Sub

Private Sub ShowErrMsg()
  MsgBox "Error:" & Err & " " & Error
End Sub
