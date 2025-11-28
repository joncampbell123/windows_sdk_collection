VERSION 5.00
Begin VB.Form frmQuery 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Query"
   ClientHeight    =   5025
   ClientLeft      =   2430
   ClientTop       =   2595
   ClientWidth     =   7455
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   HelpContextID   =   2016115
   Icon            =   "QUERY.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   ScaleHeight     =   4583.248
   ScaleMode       =   0  'User
   ScaleWidth      =   7358.616
   ShowInTaskbar   =   0   'False
   Begin VB.OptionButton optOrder 
      Caption         =   "Desc"
      Height          =   225
      Index           =   1
      Left            =   6480
      MaskColor       =   &H00000000&
      TabIndex        =   10
      Top             =   1560
      Width           =   855
   End
   Begin VB.OptionButton optOrder 
      Caption         =   "Asc"
      Height          =   221
      Index           =   0
      Left            =   5760
      MaskColor       =   &H00000000&
      TabIndex        =   9
      Top             =   1560
      Value           =   -1  'True
      Width           =   615
   End
   Begin VB.CheckBox chkTopPercent 
      Caption         =   "Top Percent"
      Height          =   255
      Left            =   3840
      MaskColor       =   &H00000000&
      TabIndex        =   15
      Top             =   2880
      Width           =   2175
   End
   Begin VB.TextBox txtTopNValue 
      Height          =   285
      Left            =   3000
      TabIndex        =   14
      Top             =   2880
      Width           =   735
   End
   Begin VB.CommandButton cmdGetValues 
      Caption         =   "List &Possible Values"
      Height          =   315
      Left            =   4560
      MaskColor       =   &H00000000&
      TabIndex        =   5
      Top             =   600
      Width           =   2775
   End
   Begin VB.CommandButton cmdOr 
      Caption         =   "&Or into Criteria"
      Height          =   315
      Left            =   2280
      MaskColor       =   &H00000000&
      TabIndex        =   4
      Top             =   600
      Width           =   2175
   End
   Begin VB.CommandButton cmdAnd 
      Caption         =   "&And into Criteria"
      Height          =   315
      Left            =   120
      MaskColor       =   &H00000000&
      TabIndex        =   3
      Top             =   600
      Width           =   2160
   End
   Begin VB.ComboBox cboValue 
      Height          =   315
      Left            =   4560
      Sorted          =   -1  'True
      TabIndex        =   2
      Text            =   "cValue"
      Top             =   240
      Width           =   2775
   End
   Begin VB.ComboBox cboOperator 
      Height          =   315
      ItemData        =   "QUERY.frx":030A
      Left            =   3120
      List            =   "QUERY.frx":030C
      Style           =   2  'Dropdown List
      TabIndex        =   1
      Top             =   240
      Width           =   1335
   End
   Begin VB.ComboBox cboField 
      Height          =   315
      Left            =   120
      Style           =   2  'Dropdown List
      TabIndex        =   0
      Top             =   240
      Width           =   2895
   End
   Begin VB.CommandButton cmdSaveQDF 
      Caption         =   "Sa&ve"
      Height          =   375
      Left            =   3720
      MaskColor       =   &H00000000&
      TabIndex        =   20
      Top             =   4560
      Width           =   1200
   End
   Begin VB.CommandButton cmdJoin 
      Caption         =   "Set Table &Joins"
      Height          =   255
      Left            =   4560
      MaskColor       =   &H00000000&
      TabIndex        =   12
      Top             =   2160
      Width           =   2775
   End
   Begin VB.ListBox lstJoinFields 
      Height          =   255
      Left            =   4560
      TabIndex        =   13
      Top             =   2400
      Width           =   2775
   End
   Begin VB.CommandButton cmdCopySQL 
      Caption         =   "Cop&y"
      Height          =   375
      Left            =   2520
      MaskColor       =   &H00000000&
      TabIndex        =   19
      Top             =   4560
      Width           =   1200
   End
   Begin VB.ComboBox cboOrderByField 
      Height          =   315
      Left            =   4560
      Style           =   2  'Dropdown List
      TabIndex        =   11
      Top             =   1800
      Width           =   2775
   End
   Begin VB.ComboBox cboGroupByField 
      Height          =   315
      Left            =   4560
      Style           =   2  'Dropdown List
      TabIndex        =   8
      Top             =   1200
      Width           =   2775
   End
   Begin VB.ListBox lstTables 
      Height          =   1425
      Left            =   120
      MultiSelect     =   1  'Simple
      TabIndex        =   6
      Top             =   1200
      Width           =   1815
   End
   Begin VB.CommandButton cmdShowSQL 
      Caption         =   "&Show"
      Height          =   375
      Left            =   1320
      MaskColor       =   &H00000000&
      TabIndex        =   18
      Top             =   4560
      Width           =   1200
   End
   Begin VB.ListBox lstShowFields 
      Height          =   1425
      Left            =   2040
      MultiSelect     =   1  'Simple
      TabIndex        =   7
      Top             =   1200
      Width           =   2295
   End
   Begin VB.CommandButton cmdClose 
      Cancel          =   -1  'True
      Caption         =   "&Close"
      Height          =   375
      Left            =   6120
      MaskColor       =   &H00000000&
      TabIndex        =   22
      Top             =   4560
      Width           =   1200
   End
   Begin VB.CommandButton cmdRunQuery 
      Caption         =   "&Run"
      Height          =   375
      Left            =   120
      MaskColor       =   &H00000000&
      TabIndex        =   17
      Top             =   4560
      Width           =   1200
   End
   Begin VB.CommandButton cmdClear 
      Caption         =   "C&lear"
      Height          =   375
      Left            =   4920
      MaskColor       =   &H00000000&
      TabIndex        =   21
      Top             =   4560
      Width           =   1200
   End
   Begin VB.TextBox txtCriteria 
      Height          =   1215
      Left            =   120
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   16
      Top             =   3240
      Width           =   7215
   End
   Begin VB.Label lblLabels 
      Caption         =   "Top N Value:"
      Height          =   195
      Index           =   7
      Left            =   1440
      TabIndex        =   31
      Top             =   2910
      Width           =   1470
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Operator:"
      Height          =   195
      Index           =   1
      Left            =   3120
      TabIndex        =   30
      Top             =   0
      Width           =   720
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Value:"
      Height          =   195
      Index           =   2
      Left            =   4560
      TabIndex        =   29
      Top             =   0
      Width           =   450
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Field Name:"
      Height          =   195
      Index           =   0
      Left            =   120
      TabIndex        =   28
      Top             =   0
      Width           =   840
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Order By: "
      Height          =   195
      Index           =   6
      Left            =   4560
      TabIndex        =   27
      Top             =   1560
      Width           =   750
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Group By: "
      Height          =   195
      Index           =   5
      Left            =   4560
      TabIndex        =   26
      Top             =   960
      Width           =   765
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Tables: "
      Height          =   195
      Index           =   3
      Left            =   120
      TabIndex        =   25
      Top             =   960
      Width           =   570
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Fields to Show: "
      Height          =   195
      Index           =   4
      Left            =   2040
      TabIndex        =   24
      Top             =   960
      Width           =   1140
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Criteria: "
      Height          =   195
      Index           =   8
      Left            =   120
      TabIndex        =   23
      Top             =   3000
      Width           =   630
   End
End
Attribute VB_Name = "frmQuery"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'>>>>>>>>>>>>>>>>>>>>>>>>
Const FORMCAPTION = "Query Builder"
Const BUTTON1 = "&And into Criteria"
Const BUTTON2 = "&Or into Criteria"
Const BUTTON3 = "List &Possible Values"
Const BUTTON4 = "Set Table &Joins"
Const BUTTON5 = "&Run"
Const BUTTON6 = "&Show"
Const BUTTON7 = "Cop&y"
Const BUTTON8 = "Sa&ve"
Const BUTTON9 = "C&lear"
Const BUTTON10 = "&Close"
Const Label1 = "Field Name:"
Const Label2 = "Operator:"
Const LABEL3 = "Value:"
Const LABEL4 = "Tables:"
Const LABEL5 = "Fields to Show:"
Const LABEL6 = "Group By:"
Const LABEL7 = "Order By:"
Const LABEL8 = "Top N Value:"
Const LABEL9 = "Criteria:"
Const CHECK1 = "Top Percent"
Const MSG1 = "Updating Form Fields"
Const MSG2 = "(none)"
Const MSG3 = "You Must Have at Least 2 Tables Selected!"
Const MSG4 = "Choose Joins"
Const MSG5 = "No Query Entered!"
Const MSG6 = "Building Query"
Const MSG7 = "Running Query"
Const MSG8 = "Enter QueryDef Name:"
'>>>>>>>>>>>>>>>>>>>>>>>>


Dim mbShowSQL As Integer
Dim mbCopySQL As Integer
Dim mbSaveSQL As Integer

Private Sub cmdAnd_Click()
  Dim nFldType As Integer
  Dim sFieldName As String
  Dim sTableName As String

  If Len(cboField.Text) = 0 Then Exit Sub

  sTableName = stSTF((cboField), 0)
  sFieldName = stSTF((cboField), 1)
  nFldType = gdbCurrentDB.TableDefs(StripBrackets(sTableName)).Fields(StripBrackets(sFieldName)).Type
  
  If Len(txtCriteria.Text) > 0 Then
    txtCriteria.Text = txtCriteria.Text & vbCrLf & "And "
  End If
  If nFldType = dbText Or nFldType = dbMemo Or nFldType = dbDate Then
    txtCriteria.Text = txtCriteria.Text & cboField.Text & " " & cboOperator.Text & " '" & cboValue.Text & "'"
  Else
    txtCriteria.Text = txtCriteria.Text & cboField.Text & " " & cboOperator.Text & " " & cboValue.Text
  End If
  cboField.SetFocus
End Sub

Private Sub cboField_Click()
  cboValue.Clear
End Sub

Private Sub cmdClear_Click()
  On Error Resume Next
  Dim i As Integer
  
  For i = 0 To lstTables.ListCount - 1
    lstTables.Selected(i) = False
  Next
  txtCriteria.Text = vbNullString
  txtTopNValue.Text = vbNullString
End Sub

Private Sub cmdClose_Click()
  Unload Me
End Sub

Private Sub cmdCopySQL_Click()
  mbCopySQL = True
  Call cmdRunQuery_Click
  mbCopySQL = False
End Sub

Private Sub cmdSaveQDF_Click()
  mbSaveSQL = True
  Call cmdRunQuery_Click
  mbSaveSQL = False
End Sub

Private Sub lstTables_Click()
  On Error GoTo LTErr

  Dim i As Integer, ii As Integer
  Dim tdf As TableDef
  Dim qdf As QueryDef
  Dim sTmp As String
  Dim fld As Field

  MsgBar MSG1, True
  cboField.Clear
  lstShowFields.Clear
  cboGroupByField.Clear
  cboOrderByField.Clear
  cboValue.Clear

  cboGroupByField.AddItem MSG2
  cboOrderByField.AddItem MSG2

  For ii = 0 To lstTables.ListCount - 1
    If lstTables.Selected(ii) Then
      If lstTables.ItemData(ii) = 0 Then
        'must be a table
        Set tdf = gdbCurrentDB.TableDefs(lstTables.List(ii))
        For Each fld In tdf.Fields
          sTmp = AddBrackets((lstTables.List(ii))) & "." & AddBrackets((fld.Name))
          cboField.AddItem sTmp
          lstShowFields.AddItem sTmp
          cboGroupByField.AddItem sTmp
          cboOrderByField.AddItem sTmp
        Next
      Else
        'must be a querydef
        Set qdf = gdbCurrentDB.QueryDefs(lstTables.List(ii))
        For Each fld In qdf.Fields
          sTmp = AddBrackets((lstTables.List(ii))) & "." & AddBrackets((fld.Name))
          cboField.AddItem sTmp
          lstShowFields.AddItem sTmp
          cboGroupByField.AddItem sTmp
          cboOrderByField.AddItem sTmp
        Next
      End If
    End If
  Next
  If Len(cboField.List(0)) > 0 Then
    cboField.ListIndex = 0
    cboGroupByField.ListIndex = 0
    cboOrderByField.ListIndex = 0
  End If
  MsgBar vbNullString, False
  Exit Sub
  
LTErr:
  ShowError
End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
  If KeyCode = vbKeyF1 And Shift = 0 Then
    DisplayTopic 2016115
  End If
End Sub

Private Sub Form_Load()
  On Local Error GoTo FLErr

  Dim rec As Recordset
  Dim i As Integer

  Me.Caption = FORMCAPTION
  cmdAnd.Caption = BUTTON1
  cmdOr.Caption = BUTTON2
  cmdGetValues.Caption = BUTTON3
  cmdJoin.Caption = BUTTON4
  cmdRunQuery.Caption = BUTTON5
  cmdShowSQL.Caption = BUTTON6
  cmdCopySQL.Caption = BUTTON7
  cmdSaveQDF.Caption = BUTTON8
  cmdClear.Caption = BUTTON9
  cmdClose.Caption = BUTTON10
  lblLabels(0).Caption = Label1
  lblLabels(1).Caption = Label2
  lblLabels(2).Caption = LABEL3
  lblLabels(3).Caption = LABEL4
  lblLabels(4).Caption = LABEL5
  lblLabels(5).Caption = LABEL6
  lblLabels(6).Caption = LABEL7
  lblLabels(7).Caption = LABEL8
  lblLabels(8).Caption = LABEL9
  chkTopPercent.Caption = CHECK1
  
  'Clear listbox
  txtCriteria.Text = vbNullString

  cboOperator.AddItem "="
  cboOperator.AddItem "<>"
  cboOperator.AddItem ">"
  cboOperator.AddItem ">="
  cboOperator.AddItem "<"
  cboOperator.AddItem "<="
  cboOperator.AddItem "Like"
  cboOperator.ListIndex = 0

  'fill the table list
  GetTableList lstTables, False, False, True
  lstTables.ListIndex = 0

  cboValue.Text = vbNullString

  Height = 5520
  Width = 7224
  Left = (frmMDI.Width - Width) / 2
  Top = 0
  Exit Sub

FLErr:
  ShowError
End Sub

Private Sub Form_Resize()
  On Error Resume Next

  If WindowState <> 1 Then
    Me.Height = 5430
    Me.Width = 7575
  End If
End Sub

Private Sub cmdGetValues_Click()
  On Error GoTo GVErr

  Dim rec As Recordset

  MsgBar "Getting Possible Values", True
  Screen.MousePointer = vbHourglass
  Set rec = gdbCurrentDB.OpenRecordset("select Distinct " & StripOwner(cboField) & " from " & stSTF((cboField), 0))
  Do While rec.EOF = False
    If Len(Trim(rec(0))) > 0 Then
      cboValue.AddItem rec(0).Value
    End If
    rec.MoveNext
  Loop
  rec.Close
  cboValue.Text = cboValue.List(0)
  cboValue.SetFocus

  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False
  Exit Sub

GVErr:
  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False
  cboValue.Text = vbNullString
  Exit Sub

End Sub

Private Sub cmdJoin_Click()
  Dim i As Integer
  Dim c As Integer

  For i = 0 To lstTables.ListCount - 1
    If lstTables.Selected(i) Then
      c = c + 1
    End If
  Next
  If c < 2 Then
    Beep
    MsgBox MSG3, 48
  Else
    MsgBar MSG4, False
    frmJoin.Show vbModal
    MsgBar vbNullString, False
  End If
End Sub

Private Sub cmdOr_Click()
  Dim nType As Integer
  Dim sFieldName As String
  Dim sTableName As String

  If Len(cboField.Text) = 0 Then Exit Sub

  sTableName = stSTF((cboField), 0)
  sFieldName = stSTF((cboField), 1)
  nType = gdbCurrentDB.TableDefs(StripBrackets(sTableName)).Fields(StripBrackets(sFieldName)).Type

  If Len(txtCriteria.Text) > 0 Then
    txtCriteria.Text = txtCriteria.Text & vbCrLf & " Or "
  End If
  If nType = dbText Or nType = dbMemo Or nType = dbDate Then
    txtCriteria.Text = txtCriteria.Text & cboField.Text & " " & cboOperator.Text & " '" & cboValue.Text & "'"
  Else
    txtCriteria.Text = txtCriteria.Text & cboField.Text & " " & cboOperator.Text & " " & cboValue.Text
  End If
  cboField.SetFocus

End Sub

Private Sub cmdRunQuery_Click()

  On Error GoTo OKErr

  Dim rsTmp As Recordset
  Dim frmTmp As Form
  Dim fs As String
  Dim ts As String
  Dim i As Integer
  Dim sWhere As String
  Dim sWhere2 As String
  Dim sNewWhere As String
  Dim sTmp As String
  Dim bMatchParen As Integer
  Dim sQueryName As String
  Dim qdfTmp As QueryDef
  Dim sSQLString As String

  If lstShowFields.ListCount = 0 Then
    MsgBox MSG5, vbExclamation
    Exit Sub
  End If

  MsgBar MSG6, True
  If Len(txtCriteria.Text) > 0 Then
    sWhere = "AND " & LTrim(txtCriteria.Text)
    'strip vbcrlfs
    For i = 1 To Len(sWhere)
      If Mid(sWhere, i, 1) = Chr(13) Then
        sTmp = sTmp & " "
      ElseIf Mid(sWhere, i, 1) = Chr(10) Then
        'do nothing
      Else
        sTmp = sTmp + Mid(sWhere, i, 1)
      End If
    Next
    sWhere = sTmp

    sWhere = RTrim(sWhere)

    'Add parens to sWhere
     sWhere2 = sWhere
     Do
       sTmp = stGetToken(sWhere2, " ")
       sTmp = sTmp & " "
        If bMatchParen = False And UCase(sTmp) = "AND " Then
         sNewWhere = sNewWhere + sTmp & "("
         bMatchParen = True
       ElseIf bMatchParen And UCase(sTmp) = "AND " Then
         sNewWhere = sNewWhere & ") " & sTmp & "("
         'bMatchParen = False
       Else
         If UCase(sTmp) = "OR" Or UCase(sTmp) = "IN " Or UCase(sTmp) = "LIKE" Then
           sNewWhere = sNewWhere & " " & sTmp
         Else
           sNewWhere = sNewWhere + sTmp
         End If
       End If

     Loop Until sWhere2 = vbNullString
     sWhere = sNewWhere & ")"

    'Build DynaSet string:
    'Peel off leading AND/OR
    If Mid(sWhere, 2, 2) = "OR" Then
      sWhere = Mid(sWhere, 5, Len(sWhere) - 5)
    Else
      sTmp = stGetToken(sWhere, " ")
    End If

    If Len(sWhere) > 0 Then
      sWhere = " Where " & sWhere
    End If

  End If

  'check for join condition
  If lstJoinFields.ListCount > 0 Then
    If Len(sWhere) = 0 Then
      sWhere = sWhere & " Where "
    Else
      sWhere = sWhere & " And "
    End If
    For i = 0 To lstJoinFields.ListCount - 1
      sWhere = sWhere + lstJoinFields.List(i) & " And "
    Next
    sWhere = Mid(sWhere, 1, Len(sWhere) - 5)
  End If

  'check for group by field
  If cboGroupByField <> MSG2 Then
    sWhere = sWhere & " Group By " & cboGroupByField
  End If

  'check for order by field
  If cboOrderByField <> MSG2 Then
    sWhere = sWhere & " Order By " & cboOrderByField
    If optOrder(1).Value Then
      sWhere = sWhere & " Desc "
    End If
  End If

  'get show field names
  For i% = 0 To lstShowFields.ListCount - 1
    If lstShowFields.Selected(i%) Then
      fs = fs + lstShowFields.List(i%) & ","
    End If
  Next
  If Len(fs) = 0 Then
    For i% = 0 To lstTables.ListCount - 1
      If lstTables.Selected(i%) Then
        fs = fs + AddBrackets((lstTables.List(i%))) & ".*,"
      End If
    Next
    If Len(fs) = 0 Then
      fs = "*"
    Else
      fs = Mid(fs, 1, Len(fs) - 1)     'take off the last ","
    End If
  Else
    fs = Mid(fs, 1, Len(fs) - 1)
  End If

  'get table names
  For i% = 0 To lstTables.ListCount - 1
    If lstTables.Selected(i%) Then
      ts = ts + AddBrackets((lstTables.List(i%))) & ","
    End If
  Next
  ts = Mid(ts, 1, Len(ts) - 1)

  sSQLString = "Select "
  
  'set Top N Value if present
  If Len(txtTopNValue.Text) > 0 Then
    sSQLString = sSQLString & " TOP " & txtTopNValue.Text & " "
    If chkTopPercent.Value = vbChecked Then
      sSQLString = sSQLString & " PERCENT "
    End If
  End If
  
  sSQLString = sSQLString & fs & " From " & ts + sWhere
  
  If mbShowSQL = False And mbCopySQL = False And mbSaveSQL = False Then
    MsgBar MSG7, True
    OpenQuery sSQLString, True
    
  ElseIf mbShowSQL Then
    MsgBar vbNullString, False
    MsgBox sSQLString, 0, "SQL Query"
  
  ElseIf mbCopySQL Then
    frmSQL.txtSQLStatement.Text = sSQLString
  
  ElseIf mbSaveSQL Then
    MsgBar vbNullString, False
    sQueryName = InputBox(MSG8)
    If Len(sQueryName) = 0 Then Exit Sub
  
    'check for a dupe and exit if the user won't overwrite it
    If DupeTableName(sQueryName) Then
      Exit Sub
    End If
    'add the new querydef
    Set qdfTmp = gdbCurrentDB.CreateQueryDef(sQueryName, sSQLString)
    RefreshTables Nothing
  End If

  MsgBar vbNullString, False
  Exit Sub

OKErr:
  If Err = 364 Then Exit Sub   'catch unloaded form
  ShowError
End Sub

Private Sub cmdShowSQL_Click()
  mbShowSQL = True
  Call cmdRunQuery_Click
  mbShowSQL = False
End Sub

Private Function stGetToken(rsLine As String, rsDelim As String) As String
  On Error GoTo GetTokenError
  
  Dim iOpenQuote As Integer
  Dim iCloseQuote As Integer
  Dim iDelim As Integer
  Dim stToken As String

  iOpenQuote = InStr(1, rsLine, """")
  iDelim = InStr(1, rsLine, rsDelim)

  If (iOpenQuote > 0) And (iOpenQuote < iDelim) Then
    iCloseQuote = InStr(iOpenQuote + 1, rsLine, """")
    iDelim = InStr(iCloseQuote + 1, rsLine, rsDelim)
  End If

  If (iDelim% <> 0) Then
    stToken = LTrim(RTrim(Mid(rsLine, 1, iDelim - 1)))
    rsLine = Mid(rsLine, iDelim + 1)
  Else
    stToken = LTrim(RTrim(Mid(rsLine, 1)))
    rsLine = vbNullString
  End If

  If (Len(stToken) > 0) Then
    If (Mid(stToken, 1, 1) = """") Then
      stToken = Mid(stToken, 2)
    End If
    If (Mid(stToken, Len(stToken), 1) = """") Then
      stToken = Mid(stToken, 1, Len(stToken) - 1)
    End If
  End If
  stGetToken = stToken
  Exit Function

GetTokenError:
  Exit Function

End Function

'function to split the table and the field from a tbl.fld pair
Private Function stSTF(rsName As String, rnPart As Integer) As String
  If InStr(InStr(1, rsName, ".") + 1, rsName, ".") > 1 Then
    rsName = StripOwner(rsName)
  End If
  If rnPart = 0 Then
    stSTF = Mid(rsName, 1, InStr(1, rsName, ".") - 1)
  Else
    stSTF = Mid(rsName, InStr(1, rsName, ".") + 1, Len(rsName))
  End If
End Function
