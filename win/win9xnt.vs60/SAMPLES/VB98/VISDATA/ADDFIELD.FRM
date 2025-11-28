VERSION 5.00
Begin VB.Form frmAddField 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Add Field"
   ClientHeight    =   3735
   ClientLeft      =   2490
   ClientTop       =   2865
   ClientWidth     =   6120
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   HelpContextID   =   2016117
   Icon            =   "ADDFIELD.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3297.239
   ScaleMode       =   0  'User
   ScaleWidth      =   6054.501
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.OptionButton optVariable 
      Caption         =   "VariableField"
      Height          =   255
      Left            =   240
      MaskColor       =   &H00000000&
      TabIndex        =   4
      Top             =   2160
      Value           =   -1  'True
      Width           =   2379
   End
   Begin VB.OptionButton optFixedField 
      Caption         =   "FixedField"
      Height          =   255
      Left            =   240
      MaskColor       =   &H00000000&
      TabIndex        =   3
      Top             =   1920
      Width           =   2379
   End
   Begin VB.CheckBox chkAutoInc 
      Caption         =   "AutoIncrField"
      Height          =   255
      Left            =   240
      MaskColor       =   &H00000000&
      TabIndex        =   5
      TabStop         =   0   'False
      Top             =   2640
      Width           =   2379
   End
   Begin VB.CheckBox chkAllowZeroLen 
      Caption         =   "AllowZeroLength"
      Height          =   255
      Left            =   240
      MaskColor       =   &H00000000&
      TabIndex        =   6
      Top             =   3000
      Width           =   2379
   End
   Begin VB.TextBox txtFieldName 
      Height          =   285
      Left            =   120
      TabIndex        =   0
      Top             =   360
      Width           =   2895
   End
   Begin VB.ComboBox cboFieldType 
      Height          =   315
      ItemData        =   "ADDFIELD.frx":030A
      Left            =   120
      List            =   "ADDFIELD.frx":030C
      Style           =   2  'Dropdown List
      TabIndex        =   1
      Top             =   960
      Width           =   1695
   End
   Begin VB.TextBox txtFieldSize 
      Height          =   285
      Left            =   120
      TabIndex        =   2
      Top             =   1560
      Width           =   1335
   End
   Begin VB.TextBox txtOrdinalPos 
      Height          =   285
      Left            =   3120
      TabIndex        =   8
      Top             =   360
      Width           =   1335
   End
   Begin VB.TextBox txtValidationText 
      Height          =   285
      Left            =   3120
      TabIndex        =   9
      Top             =   960
      Width           =   2895
   End
   Begin VB.TextBox txtValidationRule 
      Height          =   285
      Left            =   3120
      TabIndex        =   10
      Top             =   1680
      Width           =   2895
   End
   Begin VB.TextBox txtDefaultValue 
      Height          =   285
      Left            =   3120
      TabIndex        =   11
      Top             =   2280
      Width           =   2895
   End
   Begin VB.CheckBox chkRequired 
      Caption         =   "Required"
      Height          =   255
      Left            =   240
      MaskColor       =   &H00000000&
      TabIndex        =   7
      Top             =   3360
      Width           =   2379
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "&OK"
      Default         =   -1  'True
      Enabled         =   0   'False
      Height          =   375
      Left            =   3480
      MaskColor       =   &H00000000&
      TabIndex        =   12
      Top             =   2760
      Width           =   2175
   End
   Begin VB.CommandButton cmdClose 
      Cancel          =   -1  'True
      Caption         =   "&Close"
      Height          =   375
      Left            =   3480
      MaskColor       =   &H00000000&
      TabIndex        =   13
      Top             =   3240
      Width           =   2175
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   " Name: "
      Height          =   195
      Index           =   0
      Left            =   120
      TabIndex        =   20
      Top             =   120
      Width           =   555
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   " Type: "
      Height          =   195
      Index           =   2
      Left            =   120
      TabIndex        =   19
      Top             =   720
      Width           =   510
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   " Size: "
      Height          =   195
      Index           =   3
      Left            =   120
      TabIndex        =   18
      Top             =   1320
      Width           =   435
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "OrdinalPosition: "
      Height          =   195
      Index           =   4
      Left            =   3120
      TabIndex        =   17
      Top             =   120
      Width           =   1170
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "ValidationText: "
      Height          =   195
      Index           =   5
      Left            =   3120
      TabIndex        =   16
      Top             =   720
      Width           =   1125
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "ValidationRule: "
      Height          =   195
      Index           =   6
      Left            =   3120
      TabIndex        =   15
      Top             =   1320
      Width           =   1110
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "DefaultValue: "
      Height          =   195
      Index           =   7
      Left            =   3120
      TabIndex        =   14
      Top             =   2040
      Width           =   1020
   End
End
Attribute VB_Name = "frmAddField"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'>>>>>>>>>>>>>>>>>>>>>>>>
Const FORMCAPTION = "Add Field"
Const BUTTON1 = "&OK"
Const BUTTON2 = "&Close"
Const MSG1 = " Already exists!"
'>>>>>>>>>>>>>>>>>>>>>>>>

Private Sub cmdClose_Click()
  Unload Me
End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
  If KeyCode = vbKeyF1 And Shift = 0 Then
    DisplayTopic 2016117
  End If
End Sub

Sub Form_Load()
  Me.Caption = FORMCAPTION
  cmdOK.Caption = BUTTON1
  cmdClose.Caption = BUTTON2

  cboFieldType.AddItem "Boolean"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbBoolean
  cboFieldType.AddItem "Byte"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbByte
  cboFieldType.AddItem "Integer"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbInteger
  cboFieldType.AddItem "Long"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbLong
  cboFieldType.AddItem "Currency"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbCurrency
  cboFieldType.AddItem "Single"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbSingle
  cboFieldType.AddItem "Double"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbDouble
  cboFieldType.AddItem "Date/Time"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbDate
  cboFieldType.AddItem "Text"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbText
  cboFieldType.AddItem "Binary"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbLongBinary
  cboFieldType.AddItem "Memo"
  cboFieldType.ItemData(cboFieldType.NewIndex) = dbMemo
  
  SetDefaults
  'need to disable controls that don't apply
  'to non Microsoft Access tables
  If gsDataType <> gsMSACCESS Then
    optFixedField.Enabled = False
    chkAutoInc.Enabled = False
    optVariable.Enabled = False
    txtValidationText.Enabled = False
    txtValidationRule.Enabled = False
    txtDefaultValue.Enabled = False
    chkRequired.Enabled = False
    chkAllowZeroLen.Enabled = False
  End If
  
End Sub

Private Sub txtFieldName_Change()
  'activate the ok button only if the
  'name field has something in it
  cmdOK.Enabled = (Len(txtFieldName.Text) > 0)
End Sub

Private Sub cboFieldType_Click()
  Dim nFldType As Integer
  
  'call function to set size and type of field
  txtFieldSize.Text = SetFldProperties(cboFieldType.ItemData(cboFieldType.ListIndex))
  txtFieldSize.Enabled = False

  nFldType = cboFieldType.ItemData(cboFieldType.ListIndex)
  
  'enable appropriate controls for each field type
  If gsDataType <> gsMSACCESS Then
    If nFldType = dbText Then
      'allow entry of field length
      txtFieldSize.Enabled = True
      'default field size from Access UI
      txtFieldSize.Text = "50"
    End If
    'only do the stuff below for MDB dbs
    Exit Sub
  End If
  
  If nFldType = dbText Then
    'allow entry of field length
    txtFieldSize.Enabled = True
    'default field size from Access UI
    txtFieldSize.Text = "50"
    'avaiable for memo and text
    chkAllowZeroLen.Enabled = True
    'avaiable for text only
    optVariable.Enabled = True
    optFixedField.Enabled = True
    'disable these controls
    chkAutoInc.Enabled = False
    chkAutoInc.Value = vbUnchecked
  ElseIf nFldType = dbMemo Then
    'avaiable for memo and text
    chkAllowZeroLen.Enabled = True
    'disable these controls
    optVariable.Enabled = False
    optFixedField.Enabled = False
    chkAutoInc.Enabled = False
    'set the value to 0
    optVariable.Value = False
    optFixedField.Value = False
    chkAutoInc.Value = vbUnchecked
  ElseIf nFldType = dbLong Then
    'enable this one for counter type fields
    chkAutoInc.Enabled = True
    'disable these controls
    chkAllowZeroLen.Enabled = False
    optVariable.Enabled = False
    optFixedField.Enabled = False
    'set the value to 0
    chkAllowZeroLen.Value = vbUnchecked
    optVariable.Value = False
    optFixedField.Value = False
  Else
    'disable these for all other types
    chkAllowZeroLen.Enabled = False
    optVariable.Enabled = False
    optFixedField.Enabled = False
    chkAutoInc.Enabled = False
    'set the value to 0
    chkAllowZeroLen.Value = vbUnchecked
    optVariable.Value = False
    optFixedField.Value = False
    chkAutoInc.Value = vbUnchecked
  End If

End Sub

Private Sub cmdOK_Click()
  On Error GoTo OkayErr

  Dim fld As Field     'local field structure
  Dim i As Integer

  'get a fresh field object
  Set fld = gtdfTableDef.CreateField()
  'fill the field structure
  With fld
    .Name = txtFieldName.Text
    .Type = cboFieldType.ItemData(cboFieldType.ListIndex)
    .Size = txtFieldSize.Text
    If Len(txtOrdinalPos.Text) > 0 Then .OrdinalPosition = txtOrdinalPos.Text
    
    If gsDataType = gsMSACCESS Then
      .Required = IIf(chkRequired.Value = vbChecked, -1, 0)
      If .Type = dbText Then
        'this only applies to text
        .AllowZeroLength = IIf(chkAllowZeroLen.Value = vbChecked, -1, 0)
      End If
      If optFixedField.Value Then
        .Attributes = .Attributes Or dbFixedField
      End If
      If .Type = dbLong Then
        'only applies to long type
        If chkAutoInc.Value = vbChecked Then
          .Attributes = .Attributes Or dbAutoIncrField
        End If
      End If
      If optVariable.Value Then
        .Attributes = .Attributes Or dbVariableField
      End If
      .ValidationText = txtValidationText.Text
      .ValidationRule = txtValidationRule.Text
      .DefaultValue = txtDefaultValue.Text
    End If
  End With
  
  'check for a dupe
  If ObjectExists(gtdfTableDef.Fields, fld.Name) Then
    MsgBox "'" & fld.Name & "'" & MSG1
    txtFieldName.SelStart = 0
    txtFieldName.SelLength = Len(txtFieldName.Text)
    txtFieldName.SetFocus
    Exit Sub
  End If
  
  'try to append the field
  gtdfTableDef.Fields.Append fld
  
  'must've been successful, so...
  'add the item to the list
  frmTblStruct.lstFields.AddItem txtFieldName
  'make the new item active
  frmTblStruct.lstFields.ListIndex = frmTblStruct.lstFields.NewIndex

  'enable the add table button if needed
  If frmTblStruct.cmdAddTable.Visible Then
    frmTblStruct.cmdAddTable.Enabled = True
  End If
  
  'clear the name and allow entry of another
  SetDefaults
  txtFieldName.SetFocus
  Exit Sub

OkayErr:
  ShowError
End Sub

Private Sub SetDefaults()
  txtFieldName.Text = vbNullString
  If gsDataType = gsMSACCESS Then
    optFixedField.Value = False
    chkAutoInc.Value = vbUnchecked
    optVariable.Value = True
    chkRequired.Value = vbUnchecked
    chkAllowZeroLen.Value = vbChecked
  Else
    optFixedField.Value = False
    chkAutoInc.Value = 2
    optVariable.Value = False
    chkRequired.Value = 2
    chkAllowZeroLen.Value = 2
  End If
  cboFieldType.ListIndex = 8             'default to text
  txtFieldSize.Text = 50                 'default to 50
  txtValidationText.Text = vbNullString
  txtValidationRule.Text = vbNullString
  txtDefaultValue.Text = vbNullString
End Sub
