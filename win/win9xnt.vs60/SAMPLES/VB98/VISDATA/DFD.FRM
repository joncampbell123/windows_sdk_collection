VERSION 5.00
Begin VB.Form frmDFD 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Data Form Designer"
   ClientHeight    =   3705
   ClientLeft      =   1155
   ClientTop       =   2505
   ClientWidth     =   6930
   HelpContextID   =   2018517
   Icon            =   "DFD.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3705
   ScaleWidth      =   6930
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.CommandButton cmdDown 
      Height          =   540
      Left            =   6285
      Picture         =   "DFD.frx":030A
      Style           =   1  'Graphical
      TabIndex        =   16
      Top             =   2295
      UseMaskColor    =   -1  'True
      Width           =   540
   End
   Begin VB.CommandButton cmdUp 
      Height          =   540
      Left            =   6285
      Picture         =   "DFD.frx":0614
      Style           =   1  'Graphical
      TabIndex        =   15
      Top             =   1710
      UseMaskColor    =   -1  'True
      Width           =   540
   End
   Begin VB.CommandButton cmdMoveFields 
      Caption         =   "<"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Index           =   3
      Left            =   2910
      MaskColor       =   &H00000000&
      TabIndex        =   7
      Top             =   2745
      Width           =   495
   End
   Begin VB.CommandButton cmdMoveFields 
      Caption         =   "<<"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Index           =   2
      Left            =   2910
      MaskColor       =   &H00000000&
      TabIndex        =   6
      Top             =   2295
      Width           =   495
   End
   Begin VB.CommandButton cmdMoveFields 
      Caption         =   ">>"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Index           =   1
      Left            =   2910
      MaskColor       =   &H00000000&
      TabIndex        =   5
      Top             =   1845
      Width           =   495
   End
   Begin VB.CommandButton cmdMoveFields 
      Caption         =   ">"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Index           =   0
      Left            =   2910
      MaskColor       =   &H00000000&
      TabIndex        =   4
      Top             =   1395
      Width           =   495
   End
   Begin VB.ListBox lstSelected 
      DragIcon        =   "DFD.frx":091E
      Height          =   1620
      Left            =   3510
      TabIndex        =   3
      Top             =   1440
      Width           =   2685
   End
   Begin VB.CommandButton cmdBuildForm 
      Caption         =   "&Build the Form"
      Height          =   375
      Left            =   3330
      MaskColor       =   &H00000000&
      TabIndex        =   8
      Top             =   3225
      Width           =   1695
   End
   Begin VB.ComboBox cboRecordSource 
      Height          =   315
      Left            =   1680
      TabIndex        =   1
      Top             =   480
      Width           =   5010
   End
   Begin VB.ListBox lstAll 
      DragIcon        =   "DFD.frx":0C28
      Height          =   1620
      Left            =   120
      TabIndex        =   2
      Top             =   1440
      Width           =   2685
   End
   Begin VB.TextBox txtFormName 
      Height          =   285
      Left            =   2760
      TabIndex        =   0
      Top             =   120
      Width           =   1830
   End
   Begin VB.CommandButton cmdClose 
      Cancel          =   -1  'True
      Caption         =   "&Close"
      Height          =   375
      Left            =   5115
      MaskColor       =   &H00000000&
      TabIndex        =   9
      Top             =   3225
      Width           =   1695
   End
   Begin VB.Line Line1 
      BorderWidth     =   3
      X1              =   120
      X2              =   6780
      Y1              =   1080
      Y2              =   1080
   End
   Begin VB.Label lblLabels 
      Alignment       =   2  'Center
      Caption         =   "Select a Table/QueryDef from the list or enter a SQL statement."
      Height          =   195
      Index           =   2
      Left            =   1680
      TabIndex        =   14
      Top             =   840
      Width           =   5010
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Included Fields: "
      Height          =   195
      Index           =   4
      Left            =   3510
      TabIndex        =   13
      Top             =   1200
      Width           =   1170
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "RecordSource: "
      Height          =   195
      Index           =   1
      Left            =   105
      TabIndex        =   12
      Top             =   540
      Width           =   1110
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Available Fields: "
      Height          =   195
      Index           =   3
      Left            =   120
      TabIndex        =   11
      Top             =   1200
      Width           =   1200
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Form Name (w/o Extension): "
      Height          =   195
      Index           =   0
      Left            =   120
      TabIndex        =   10
      Top             =   120
      Width           =   2100
   End
End
Attribute VB_Name = "frmDFD"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'>>>>>>>>>>>>>>>>>>>>>>>>
Const FORMCAPTION = "Data Form Designer"
Const BUTTON1 = "&Build the Form"
Const BUTTON2 = "&Close"
Const Label1 = "Form Name (w/o Extension):"
Const Label2 = "RecordSource:"
Const LABEL3 = "Select a Table/QueryDef from the list or enter a SQL statement."
Const LABEL4 = "Available Fields:"
Const LABEL5 = "Included Fields:"
Const MSG1 = "Form Name cannot be blank!"
Const MSG2 = "You must enter a RecordSource!"
Const MSG3 = "You must include some Columns!"
Const CTLNAME1 = "&Add"
Const CTLNAME2 = "&Delete"
Const CTLNAME3 = "&Refresh"
Const CTLNAME4 = "&Update"
Const CTLNAME5 = "&Close"
'>>>>>>>>>>>>>>>>>>>>>>>>

Dim mrecRS As Recordset


Private Sub cboRecordSource_Change()
  Set mrecRS = Nothing
  lstAll.Clear
  lstSelected.Clear
End Sub

Private Sub cboRecordSource_Click()
  Call cboRecordSource_LostFocus
End Sub

Private Sub cboRecordSource_LostFocus()
  On Error GoTo RSErr
  
  Dim i As Integer
  Dim fld As Field
  
  If Len(cboRecordSource.Text) = 0 Then Exit Sub
  
  Screen.MousePointer = 11
  If mrecRS Is Nothing Then
    Set mrecRS = gdbCurrentDB.OpenRecordset(cboRecordSource.Text)
    For Each fld In mrecRS.Fields
      lstAll.AddItem fld.Name
    Next
  ElseIf mrecRS.Name <> cboRecordSource.Text Then
    lstAll.Clear
    lstSelected.Clear
    Set mrecRS = gdbCurrentDB.OpenRecordset(cboRecordSource.Text)
    For Each fld In mrecRS.Fields
      lstAll.AddItem fld.Name
    Next
  End If
  If lstAll.ListCount > 0 Then lstAll.ListIndex = 0
  
  Screen.MousePointer = 0
  Exit Sub
  
RSErr:
  Screen.MousePointer = 0
  MsgBox Error$
End Sub

Sub cmdBuildForm_Click()
  If Len(txtFormName.Text) = 0 Then
    MsgBox MSG1, 16
    txtFormName.SetFocus
    Exit Sub
  End If
  
  If Len(cboRecordSource.Text) = 0 Then
    MsgBox MSG2, 16
    Exit Sub
  End If
    
  If lstSelected.ListCount = 0 Then
    MsgBox MSG3, 16
    Exit Sub
  End If
    
  BuildForm
End Sub

Sub cmdClose_Click()
  Unload Me
End Sub

Private Sub cmdDown_Click()
  On Error Resume Next
  Dim nItem As Integer
  
  With lstSelected
    If .ListIndex < 0 Then Exit Sub
    nItem = .ListIndex
    If nItem = .ListCount - 1 Then Exit Sub 'can't move last item down
    'move item down
    .AddItem .Text, nItem + 2
    'remove old item
    .RemoveItem nItem
    'select the item that was just moved
    .Selected(nItem + 1) = True
  End With
End Sub

Private Sub cmdMoveFields_Click(Index As Integer)
  Dim i As Integer
  Select Case Index
    Case 0
      If lstAll.ListIndex < 0 Then Exit Sub
      lstSelected.AddItem lstAll.Text
      i = lstAll.ListIndex
      lstAll.RemoveItem i
      If lstAll.ListCount > 0 Then
        If i > lstAll.ListCount - 1 Then
          lstAll.ListIndex = i - 1
        Else
          lstAll.ListIndex = i
        End If
      End If
      lstSelected.ListIndex = lstSelected.NewIndex
    Case 1
      If lstAll.ListCount = 0 Then Exit Sub
      For i = 0 To lstAll.ListCount - 1
        lstSelected.AddItem lstAll.List(i)
      Next
      lstAll.Clear
      lstSelected.ListIndex = 0
    Case 2
      If lstSelected.ListCount = 0 Then Exit Sub
      For i = 0 To lstSelected.ListCount - 1
        lstAll.AddItem lstSelected.List(i)
      Next
      lstSelected.Clear
      lstAll.ListIndex = lstAll.NewIndex
    Case 3
      If lstSelected.ListIndex < 0 Then Exit Sub
      lstAll.AddItem lstSelected.Text
      i = lstSelected.ListIndex
      lstSelected.RemoveItem i
      
      lstAll.ListIndex = lstAll.NewIndex
      If lstSelected.ListCount > 0 Then
        If i > lstSelected.ListCount - 1 Then
          lstSelected.ListIndex = i - 1
        Else
          lstSelected.ListIndex = i
        End If
      End If
  End Select
End Sub

Private Sub cmdUp_Click()
  On Error Resume Next
  Dim nItem As Integer
  
  With lstSelected
    If .ListIndex < 0 Then Exit Sub
    nItem = .ListIndex
    If nItem = 0 Then Exit Sub  'can't move 1st item up
    'move item up
    .AddItem .Text, nItem - 1
    'remove old item
    .RemoveItem nItem + 1
    'select the item that was just moved
    .Selected(nItem - 1) = True
  End With
End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
  If KeyCode = vbKeyF1 And Shift = 0 Then
    DisplayTopic 2018517
  End If
End Sub

Sub Form_Load()
  Me.Caption = FORMCAPTION
  cmdBuildForm.Caption = BUTTON1
  cmdClose.Caption = BUTTON2
  lblLabels(0).Caption = Label1
  lblLabels(1).Caption = Label2
  lblLabels(2).Caption = LABEL3
  lblLabels(3).Caption = LABEL4
  lblLabels(4).Caption = LABEL5
  
  GetTableList cboRecordSource, True, False, True
End Sub

Private Sub lstAll_DblClick()
  cmdMoveFields_Click 0
End Sub

Private Sub lstSelected_DblClick()
  cmdMoveFields_Click 3
End Sub

Sub BuildForm()
  On Error GoTo BuildErr
    
  Dim i As Integer
  Dim sTmp As String
  Dim nNumFlds As Integer
  Dim frmNewForm As VBComponent
  Dim ctlNewControl As VBControl
  Dim nButtonTop As Integer
  Dim bOLEFields As Boolean
  
  nNumFlds = lstSelected.ListCount
    
  'create the new form
  Set frmNewForm = gVDClass.VBInstance.ActiveVBProject.VBComponents.Add(vbext_ct_VBForm)
  
  'form height = 320 * numflds + 1260 for buttons and data control
  'form width = 5640
  With frmNewForm
    .Properties!Appearance = 1
    .Properties!Caption = Left(mrecRS.Name, 32)
    .Properties!Height = 1115 + (nNumFlds * 320)
    .Properties!Left = 1050
    .Properties!Name = "frm" & txtFormName.Text
    .Properties!Width = 5640
  End With
   
  'labels.left = 120, .width = 1815, .height = 255
  'fields.left = 2040, .width = 3375, .height = 285
  For i = 0 To nNumFlds - 1
    sTmp = lstSelected.List(i)
    Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("Label", Nothing)
    With ctlNewControl
      .Properties!Appearance = 1
      .Properties!Caption = sTmp & ":"
      .Properties!Height = 255
      .Properties!Index = i
      .Properties!Left = 120
      .Properties!Name = "lblLabels"
      .Properties!Top = (i * 320) + 60
      .Properties!Width = 1815
    End With
    If mrecRS.Fields(sTmp).Type = 1 Then
      'true/false field
      Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("CheckBox", Nothing)
      With ctlNewControl
        .Properties!Appearance = 1
        .Properties!Caption = ""
        .Properties!Height = 285
        .Properties!Left = 2040
        .Properties!Name = "chkFields"
        .Properties!Top = (i * 320) + 40
        .Properties!Width = 3375
        .Properties!DataSource = "Data1"
        .Properties!DataField = sTmp
      End With
    ElseIf mrecRS.Fields(sTmp).Type = 11 Then
      'picture field
      bOLEFields = True
      Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("OLE", Nothing)
      With ctlNewControl
        .Properties!Height = 285
        .Properties!Left = 2040
        .Properties!Name = "oleFields"
        .Properties!OLETypeAllowed = 1
        .Properties!Top = (i * 320) + 40
        .Properties!Width = 3375
        .Properties!DataSource = "Data1"
        .Properties!DataField = sTmp
        If .Properties("Index") = -1 Then
          .Properties("Index") = 0
        End If
      End With
      SendKeys "{Esc}"
    Else
      Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("TextBox", Nothing)
      With ctlNewControl
        .Properties!Appearance = 1
        .Properties!Left = 2040
        .Properties!Name = "txtFields"
        .Properties!Text = ""
        If mrecRS.Fields(sTmp).Type < 10 Then
          'numeric or date
          .Properties!Width = 1935
        Else
          'string or memo
          .Properties!Width = 3375
        End If
        .Properties!DataSource = "Data1"
        .Properties!DataField = sTmp
        If mrecRS.Fields(sTmp).Type = 10 Then
          .Properties!Height = 285
          .Properties!Top = (i * 320) + 40
          .Properties!MaxLength = mrecRS.Fields(sTmp).Size
        ElseIf mrecRS.Fields(sTmp).Type = 12 Then
          .Properties!Height = 310
          .Properties!Top = (i * 320) + 30
          .Properties!MultiLine = True
          .Properties!ScrollBars = 2
        Else
          .Properties!Height = 285
          .Properties!Top = (i * 320) + 40
        End If
      End With
    End If
  Next
  nButtonTop = ctlNewControl.Properties!Top + 340
  
  'add the data control and buttons
  Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("Data", Nothing)
  With ctlNewControl
    .Properties!Appearance = 1
    .Properties!Align = 2
    .Properties!Caption = ""
    If gsDataType <> gsSQLDB Then
      'only set for local dbs
      .Properties!DatabaseName = gdbCurrentDB.Name
    End If
    .Properties!Connect = gdbCurrentDB.Connect
    .Properties!RecordSource = cboRecordSource.Text
  End With
  Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("CommandButton", Nothing)
  With ctlNewControl
    .Properties!Appearance = 1
    .Properties!Caption = CTLNAME1
    .Properties!Height = 300
    .Properties!Left = 120
    .Properties!Name = "cmdAdd"
    .Properties!Top = nButtonTop
    .Properties!Width = 975
  End With
  Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("CommandButton", Nothing)
  With ctlNewControl
    .Properties!Appearance = 1
    .Properties!Caption = CTLNAME2
    .Properties!Height = 300
    .Properties!Left = 1200
    .Properties!Name = "cmdDelete"
    .Properties!Top = nButtonTop
    .Properties!Width = 975
  End With
  Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("CommandButton", Nothing)
  With ctlNewControl
    .Properties!Appearance = 1
    .Properties!Caption = CTLNAME3
    .Properties!Height = 300
    .Properties!Left = 2280
    .Properties!Name = "cmdRefresh"
    .Properties!Top = nButtonTop
    .Properties!Width = 975
  End With
  Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("CommandButton", Nothing)
  With ctlNewControl
    .Properties!Appearance = 1
    .Properties!Caption = CTLNAME4
    .Properties!Height = 300
    .Properties!Left = 3360
    .Properties!Name = "cmdUpdate"
    .Properties!Top = nButtonTop
    .Properties!Width = 975
  End With
  Set ctlNewControl = frmNewForm.Designer.ContainedVBControls.Add("CommandButton", Nothing)
  With ctlNewControl
    .Properties!Appearance = 1
    .Properties!Caption = CTLNAME5
    .Properties!Height = 300
    .Properties!Left = 4440
    .Properties!Name = "cmdClose"
    .Properties!Top = nButtonTop
    .Properties!Width = 975
  End With
  
  'add the code to the form
  frmNewForm.CodeModule.AddFromString BuildFrmCode(bOLEFields)
  
  'set the form back to defaults
  txtFormName.Text = ""
  cboRecordSource.Text = ""
  'try to set focus back to the form
  Me.SetFocus
  txtFormName.SetFocus
  Exit Sub
  
BuildErr:
  MsgBox Err.Description
End Sub

Function BuildFrmCode(bOLEFields As Boolean) As String
  Dim sCode As String
  Dim i As Integer
  
  sCode = "Private Sub cmdAdd_Click()"
  sCode = sCode & vbCrLf & "  Data1.Recordset.AddNew"
  sCode = sCode & vbCrLf & "End Sub"
  sCode = sCode & vbCrLf
  sCode = sCode & vbCrLf & "Private Sub cmdDelete_Click()"
  sCode = sCode & vbCrLf & "  'this may produce an error if you delete the last"
  sCode = sCode & vbCrLf & "  'record or the only record in the recordset"
  sCode = sCode & vbCrLf & "  Data1.Recordset.Delete"
  sCode = sCode & vbCrLf & "  Data1.Recordset.MoveNext"
  sCode = sCode & vbCrLf & "End Sub"
  sCode = sCode & vbCrLf
  sCode = sCode & vbCrLf & "Private Sub cmdRefresh_Click()"
  sCode = sCode & vbCrLf & "  'this is really only needed for multi user apps"
  sCode = sCode & vbCrLf & "  Data1.Refresh"
  sCode = sCode & vbCrLf & "End Sub"
  sCode = sCode & vbCrLf
  sCode = sCode & vbCrLf & "Private Sub cmdUpdate_Click()"
  sCode = sCode & vbCrLf & "  Data1.UpdateRecord"
  sCode = sCode & vbCrLf & "  Data1.Recordset.Bookmark = Data1.Recordset.LastModified"
  sCode = sCode & vbCrLf & "End Sub"
  sCode = sCode & vbCrLf
  sCode = sCode & vbCrLf & "Private Sub cmdClose_Click()"
  sCode = sCode & vbCrLf & "  Unload Me"
  sCode = sCode & vbCrLf & "End Sub"
  sCode = sCode & vbCrLf
  sCode = sCode & vbCrLf & "Private Sub Data1_Error(DataErr As Integer, Response As Integer)"
  sCode = sCode & vbCrLf & "  'This is where you would put error handling code"
  sCode = sCode & vbCrLf & "  'If you want to ignore errors, comment out the next line"
  sCode = sCode & vbCrLf & "  'If you want to trap them, add code here to handle them"
  sCode = sCode & vbCrLf & "  MsgBox ""Data error event hit err:"" & Error$(DataErr)"
  sCode = sCode & vbCrLf & "  Response = 0  'throw away the error"
  sCode = sCode & vbCrLf & "End Sub"
  sCode = sCode & vbCrLf
  sCode = sCode & vbCrLf & "Private Sub Data1_Reposition()"
  sCode = sCode & vbCrLf & "  Screen.MousePointer = vbDefault"
  sCode = sCode & vbCrLf & "  On Error Resume Next"
  sCode = sCode & vbCrLf & "  'This will display the current record position"
  sCode = sCode & vbCrLf & "  'for dynasets and snapshots"
  sCode = sCode & vbCrLf & "  Data1.Caption = ""Record: "" & (Data1.Recordset.AbsolutePosition + 1)"
  sCode = sCode & vbCrLf & "  'for the table object you must set the index property when"
  sCode = sCode & vbCrLf & "  'the recordset gets created and use the following line"
  sCode = sCode & vbCrLf & "  'Data1.Caption = ""Record: "" & (Data1.Recordset.RecordCount * (Data1.Recordset.PercentPosition * 0.01)) + 1"
  sCode = sCode & vbCrLf & "End Sub"
  sCode = sCode & vbCrLf
  sCode = sCode & vbCrLf & "Private Sub Data1_Validate(Action As Integer, Save As Integer)"
  sCode = sCode & vbCrLf & "  'This is where you put validation code"
  sCode = sCode & vbCrLf & "  'This event gets called when the following actions occur"
  sCode = sCode & vbCrLf & "  Select Case Action"
  sCode = sCode & vbCrLf & "    Case vbDataActionMoveFirst"
  sCode = sCode & vbCrLf & "    Case vbDataActionMovePrevious"
  sCode = sCode & vbCrLf & "    Case vbDataActionMoveNext"
  sCode = sCode & vbCrLf & "    Case vbDataActionMoveLast"
  sCode = sCode & vbCrLf & "    Case vbDataActionAddNew"
  sCode = sCode & vbCrLf & "    Case vbDataActionUpdate"
  sCode = sCode & vbCrLf & "    Case vbDataActionDelete"
  sCode = sCode & vbCrLf & "    Case vbDataActionFind"
  sCode = sCode & vbCrLf & "    Case vbDataActionBookMark"
  sCode = sCode & vbCrLf & "    Case vbDataActionClose"
  sCode = sCode & vbCrLf & "  End Select"
  sCode = sCode & vbCrLf & "  Screen.MousePointer = vbHourglass"
  sCode = sCode & vbCrLf & "End Sub"
  sCode = sCode & vbCrLf
  
  'write the code for the bound OLE client control(s)
  If bOLEFields Then
    sCode = sCode & vbCrLf & "Private Sub oleFields_DblClick(Index As Integer)"
    sCode = sCode & vbCrLf & "  'this is the way to get data into an empty ole control"
    sCode = sCode & vbCrLf & "  'and have it saved back to the table"
    sCode = sCode & vbCrLf & "  oleFields(Index).InsertObjDlg"
    sCode = sCode & vbCrLf & "End Sub" & vbCrLf
  End If

  BuildFrmCode = sCode
End Function

