VERSION 5.00
Begin VB.Form frmMain 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Entity/Proxy Code Generator"
   ClientHeight    =   6615
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   8775
   Icon            =   "frmMain.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   6615
   ScaleWidth      =   8775
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtVB 
      Height          =   2535
      Left            =   0
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   9
      Top             =   840
      Width           =   8775
   End
   Begin VB.TextBox txtSP 
      Height          =   2895
      Left            =   0
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   1
      Top             =   3720
      Width           =   8775
   End
   Begin VB.ComboBox cmbPrimaryKey 
      Height          =   315
      Left            =   5160
      Style           =   2  'Dropdown List
      TabIndex        =   8
      Top             =   480
      Width           =   2415
   End
   Begin VB.CommandButton btnGenerate 
      Caption         =   "Generate"
      Height          =   285
      Left            =   7680
      TabIndex        =   6
      Top             =   120
      Width           =   975
   End
   Begin VB.CommandButton btnRefresh 
      Caption         =   "Refresh"
      Height          =   285
      Left            =   3480
      TabIndex        =   5
      Top             =   120
      Width           =   975
   End
   Begin VB.ComboBox cmbTable 
      Height          =   315
      Left            =   5160
      Style           =   2  'Dropdown List
      TabIndex        =   3
      Top             =   120
      Width           =   2415
   End
   Begin VB.TextBox txtDSN 
      Height          =   285
      Left            =   1080
      TabIndex        =   0
      Text            =   "DSN=Northwind;UID=sa;PWD="
      Top             =   120
      Width           =   2295
   End
   Begin VB.CheckBox chkCombine 
      Caption         =   "Combine Insert and Update"
      Height          =   375
      Left            =   1800
      TabIndex        =   12
      Top             =   3400
      Width           =   2655
   End
   Begin VB.CheckBox chkView 
      Caption         =   "Views"
      Height          =   375
      Left            =   2520
      TabIndex        =   13
      Top             =   480
      Width           =   855
   End
   Begin VB.Label Label5 
      Caption         =   "Visual Basic Code:"
      Height          =   255
      Left            =   120
      TabIndex        =   11
      Top             =   600
      Width           =   1575
   End
   Begin VB.Label Label4 
      Caption         =   "Stored Procedures:"
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   3480
      Width           =   1575
   End
   Begin VB.Label Label3 
      Alignment       =   1  'Right Justify
      Caption         =   "Primary Key:"
      Height          =   255
      Left            =   3720
      TabIndex        =   7
      Top             =   525
      Width           =   1335
   End
   Begin VB.Label Label2 
      Alignment       =   1  'Right Justify
      Caption         =   "Data Source:"
      Height          =   375
      Left            =   -360
      TabIndex        =   4
      Top             =   180
      Width           =   1335
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Caption         =   "Table:"
      Height          =   255
      Left            =   3840
      TabIndex        =   2
      Top             =   180
      Width           =   1215
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Const MAX_FIELD_DATA_SIZE = 2048 ' max length of field value buffer (number of WCHARs)
Const FIELDNAME_MAXSIZE = 128   ' max size of a field name

Dim m_table As ADOX.table
Dim m_cat As New ADOX.Catalog

Private Function RemoveSpaces(sSpaces As String) As String
    Dim sNoSpaces As String
    Dim nChar As Integer
    
    For nChar = 1 To Len(sSpaces)
        If (Mid$(sSpaces, nChar, 1) <> " ") Then
            sNoSpaces = sNoSpaces & Mid$(sSpaces, nChar, 1)
        End If
    Next nChar
    RemoveSpaces = sNoSpaces
End Function

Private Function InitValue(col As ADOX.Column, bSQL As Boolean) As String
    Dim sType As String
    Select Case col.Type
        Case Is = adInteger
            If bSQL Then
                sType = "0"
            Else
                sType = "CLng(0)"
            End If
        Case Is = adBSTR
            sType = """"""
        Case Is = adWChar
            sType = """"""
        Case Is = adVarWChar
            sType = """"""
        Case Is = adVarChar
            sType = """"""
        Case Is = adLongVarWChar
            sType = """"""
        Case Is = adLongVarChar
            sType = """"""
        Case Is = adDBTimeStamp
            sType = "Now"
        Case Is = adSmallInt
            sType = "0"
        Case Is = adTinyInt
            sType = "0"
        Case Is = adUnsignedTinyInt
            sType = "0"
        Case Is = adCurrency
            If bSQL Then
                sType = "0"
            Else
                sType = "CDec(0)"
            End If
        Case Is = adChar
            sType = """"""
        Case Is = adBoolean
            sType = "False"
        Case Is = adNumeric
            sType = "0"
        Case Is = adDecimal
            If bSQL Then
                sType = "0"
            Else
                sType = "CDec(0)"
            End If
        Case Is = adSingle
            If bSQL Then
                sType = "0"
            Else
                sType = "CDec(0)"
            End If
        Case Is = adDouble
            If bSQL Then
                sType = "0"
            Else
                sType = "CDec(0)"
            End If
        Case Else
            sType = "**UNKNOWN**"
            MsgBox "An unsupported data type is used in this table, generation will continue.", vbCritical + vbOKOnly, "Unsupported Data Type for field: " & col.Name
    End Select
    If (col.DefinedSize > (MAX_FIELD_DATA_SIZE - 1)) Then
        MsgBox "The default column length has been exeeded. Entity Proxy has a default limit of " & MAX_FIELD_DATA_SIZE & " bytes per field.", vbCritical + vbOKOnly, "Data Type for field: " & col.Name & ", Length: " & col.DefinedSize
    End If
    
    If (Len(col.Name) > (FIELDNAME_MAXSIZE - 1)) Then
        MsgBox "The default column name length has been exeeded. Entity Proxy has a limit of " & FIELDNAME_MAXSIZE & " characters per field and stored procedure name.", vbCritical + vbOKOnly, "Data Type for field: " & col.Name & ", Name Length: " & Len(col.Name)
    End If
    
    InitValue = sType
End Function


Private Function SQLType(col As ADOX.Column) As String
    Dim sType As String
    Select Case col.Type
        Case Is = adInteger
            sType = "int"
        Case Is = adBSTR
            sType = "varchar(" & col.DefinedSize & ")"
        Case Is = adWChar
            sType = "nvarchar(" & col.DefinedSize & ")"
        Case Is = adVarWChar
            sType = "nvarchar(" & col.DefinedSize & ")"
        Case Is = adVarChar
            sType = "varchar(" & col.DefinedSize & ")"
        Case Is = adLongVarWChar
            sType = "ntext"
        Case Is = adLongVarChar
            sType = "text"
        Case Is = adDBTimeStamp
            sType = "datetime"
        Case Is = adSmallInt
            sType = "smallint"
        Case Is = adTinyInt
            sType = "tinyint"
        Case Is = adUnsignedTinyInt
            sType = "tinyint"
        Case Is = adCurrency
            sType = "money"
        Case Is = adChar
            sType = "char(" & col.DefinedSize & ")"
        Case Is = adBoolean
            sType = "bit"
        Case Is = adNumeric
            sType = "numeric"
        Case Is = adDecimal
            sType = "decimal"
        Case Is = adSingle
            sType = "real"
        Case Else
            sType = "**UNKNOWN**"
'            MsgBox "An unsupported data type is used in this table, generation will continue.", vbCritical + vbOKOnly, "Unsupported Data Type for field: " & col.Name
    End Select
    SQLType = sType
End Function

Private Function GenerateCreateNewVB() As String
    Dim sCode As String
    Dim col As ADOX.Column
    Dim nCol As Long
    
    sCode = ""
    sCode = sCode & "Private Function CreateNew" & RemoveSpaces(m_table.Name) & "(entity As IEntityData) As Long" & vbCrLf
    sCode = sCode & "' Create a new static entity" & vbCrLf
    sCode = sCode & vbTab & "Set entity = New DataServices.Entity" & vbCrLf
    sCode = sCode & "' Initialize the entity with the appropriate number of fields" & vbCrLf
    sCode = sCode & vbTab & "entity.Initialize " & m_table.Columns.Count & vbCrLf
    sCode = sCode & vbCrLf
    sCode = sCode & "' Add a placeholder to the entity (for each field) ready for the data" & vbCrLf
    nCol = 1
    For Each col In m_table.Columns
        sCode = sCode & vbTab & "entity.Insert " & nCol & ", """ & col.Name & """, " & InitValue(col, False) & vbCrLf
        nCol = nCol + 1
    Next col
    sCode = sCode & vbCrLf
    sCode = sCode & "' Intially the entity is marked as being in Add mode." & vbCrLf
    sCode = sCode & vbTab & "entity.EditMode = colEditAdd" & vbCrLf
    sCode = sCode & "' Assign the names of the entity's stored procedures." & vbCrLf
    sCode = sCode & vbTab & "entity.ReadStoredProcedure = ""sp" & RemoveSpaces(m_table.Name) & "Load""" & vbCrLf
    sCode = sCode & vbTab & "entity.UpdateStoredProcedure = ""sp" & RemoveSpaces(m_table.Name) & "Update""" & vbCrLf
    sCode = sCode & vbTab & "entity.DeleteStoredProcedure = ""sp" & RemoveSpaces(m_table.Name) & "Delete""" & vbCrLf
    If (chkCombine.Value = 0) Then
        sCode = sCode & vbTab & "entity.InsertStoredProcedure = ""sp" & RemoveSpaces(m_table.Name) & "Insert""" & vbCrLf
    End If
    sCode = sCode & vbTab & "entity.KeyField = """ & cmbPrimaryKey.Text & """" & vbCrLf
    sCode = sCode & vbTab & "CreateNew" & RemoveSpaces(m_table.Name) & " = 1" & vbCrLf
    sCode = sCode & "End Function" & vbCrLf
    GenerateCreateNewVB = sCode
End Function

Private Function GenerateLoadSP() As String
    Dim sCode As String
    Dim col As ADOX.Column
    
    sCode = ""
    sCode = sCode & "Create Procedure sp" & RemoveSpaces(m_table.Name) & "Load" & vbCrLf
    sCode = sCode & "(" & vbCrLf
    sCode = sCode & vbTab & "@" & cmbPrimaryKey.Text & " " & SQLType(m_table.Columns(cmbPrimaryKey.Text)) & vbCrLf
    sCode = sCode & ")" & vbCrLf
    sCode = sCode & "As" & vbCrLf
    sCode = sCode & "SELECT "
    For Each col In m_table.Columns
            sCode = sCode & col.Name & ", "
    Next col
    sCode = Left$(sCode, Len(sCode) - 2) & vbCrLf
    sCode = sCode & "FROM [" & m_table.Name & "] WHERE " & cmbPrimaryKey.Text & " = " & "@" & cmbPrimaryKey.Text & vbCrLf
    sCode = sCode & "Return" & vbCrLf
    GenerateLoadSP = sCode
    
End Function

Private Function GenerateDeleteSP() As String
    Dim sCode As String
    Dim col As ADOX.Column
    
    sCode = ""
    sCode = sCode & "Create Procedure sp" & RemoveSpaces(m_table.Name) & "Delete" & vbCrLf
    sCode = sCode & "(" & vbCrLf
    sCode = sCode & vbTab & "@" & cmbPrimaryKey.Text & " " & SQLType(m_table.Columns(cmbPrimaryKey.Text)) & vbCrLf
    sCode = sCode & ")" & vbCrLf
    sCode = sCode & "As" & vbCrLf
    sCode = sCode & "DELETE from [" & m_table.Name & "] WHERE " & cmbPrimaryKey.Text & " = " & "@" & cmbPrimaryKey.Text & vbCrLf
    sCode = sCode & "Return" & vbCrLf
    GenerateDeleteSP = sCode
    
End Function

Private Function GenerateCombinedUpdateSP() As String
    Dim sCode As String
    Dim col As ADOX.Column
    
    sCode = ""
    
    sCode = sCode & "CREATE Procedure sp" & RemoveSpaces(m_table.Name) & "Update" & vbCrLf
    sCode = sCode & "(" & vbCrLf
    
    For Each col In m_table.Columns
        sCode = sCode & vbTab & "@" & col.Name & " " & SQLType(col) & ", " & vbCrLf
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    
    Set col = m_table.Columns(cmbPrimaryKey.Text)
    sCode = sCode & ")" & vbCrLf
    sCode = sCode & "As" & vbCrLf
    sCode = sCode & "SET NOCOUNT ON" & vbCrLf
    sCode = sCode & vbTab & "if (@" & cmbPrimaryKey.Text & " = " & InitValue(col, True) & ")" & vbCrLf
    sCode = sCode & vbTab & vbTab & "begin" & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "INSERT INTO [" & m_table.Name & "]" & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "(" & vbCrLf
    For Each col In m_table.Columns
        If (col.Name <> cmbPrimaryKey.Text) Then
            sCode = sCode & vbTab & vbTab & vbTab & vbTab & col.Name & ", " & vbCrLf
        End If
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & ")" & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "VALUES" & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "(" & vbCrLf
    For Each col In m_table.Columns
        If (col.Name <> cmbPrimaryKey.Text) Then
            sCode = sCode & vbTab & vbTab & vbTab & vbTab & "@" & col.Name & ", " & vbCrLf
        End If
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & ")" & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "SELECT @@identity" & vbCrLf
    sCode = sCode & vbTab & vbTab & "end" & vbCrLf
    sCode = sCode & vbTab & "else" & vbCrLf
    sCode = sCode & vbTab & vbTab & "begin" & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "UPDATE [" & m_table.Name & "]" & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "SET " & vbCrLf
    For Each col In m_table.Columns
        If (col.Name <> cmbPrimaryKey.Text) Then
            sCode = sCode & vbTab & vbTab & vbTab & vbTab & col.Name & " = @" & col.Name & ", " & vbCrLf
        End If
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "WHERE " & cmbPrimaryKey.Text & " = @" & cmbPrimaryKey.Text & vbCrLf
    sCode = sCode & vbTab & vbTab & vbTab & "SELECT @" & cmbPrimaryKey.Text & vbCrLf
    sCode = sCode & vbTab & vbTab & "end" & vbCrLf
    sCode = sCode & vbTab & "/* endif */" & vbCrLf
    sCode = sCode & "Return" & vbCrLf
    
    GenerateCombinedUpdateSP = sCode
End Function

Private Function GenerateUpdateSP() As String
    Dim sCode As String
    Dim col As ADOX.Column
    
    sCode = ""
    
    sCode = sCode & "CREATE Procedure sp" & RemoveSpaces(m_table.Name) & "Update" & vbCrLf
    sCode = sCode & "(" & vbCrLf
    
    For Each col In m_table.Columns
        sCode = sCode & vbTab & "@" & col.Name & " " & SQLType(col) & ", " & vbCrLf
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    
    Set col = m_table.Columns(cmbPrimaryKey.Text)
    sCode = sCode & ")" & vbCrLf
    sCode = sCode & "As" & vbCrLf
    sCode = sCode & "SET NOCOUNT ON" & vbCrLf
    sCode = sCode & vbTab & "UPDATE [" & m_table.Name & "]" & vbCrLf
    sCode = sCode & vbTab & "SET " & vbCrLf
    For Each col In m_table.Columns
        If (col.Name <> cmbPrimaryKey.Text) Then
            sCode = sCode & vbTab & vbTab & col.Name & " = @" & col.Name & ", " & vbCrLf
        End If
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    sCode = sCode & vbTab & "WHERE " & cmbPrimaryKey.Text & " = @" & cmbPrimaryKey.Text & vbCrLf
    sCode = sCode & vbTab & "SELECT @" & cmbPrimaryKey.Text & vbCrLf
    sCode = sCode & "Return" & vbCrLf
    
    GenerateUpdateSP = sCode
End Function

Private Function GenerateInsertSP() As String
    Dim sCode As String
    Dim col As ADOX.Column
    
    sCode = ""
    
    sCode = sCode & "CREATE Procedure sp" & RemoveSpaces(m_table.Name) & "Insert" & vbCrLf
    sCode = sCode & "(" & vbCrLf
    
    For Each col In m_table.Columns
        sCode = sCode & vbTab & "@" & col.Name & " " & SQLType(col) & ", " & vbCrLf
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    
    Set col = m_table.Columns(cmbPrimaryKey.Text)
    sCode = sCode & ")" & vbCrLf
    sCode = sCode & "As" & vbCrLf
    sCode = sCode & "SET NOCOUNT ON" & vbCrLf
    sCode = sCode & vbTab & "INSERT INTO [" & m_table.Name & "]" & vbCrLf
    sCode = sCode & vbTab & "(" & vbCrLf
    For Each col In m_table.Columns
        If (col.Name <> cmbPrimaryKey.Text) Then
            sCode = sCode & vbTab & vbTab & col.Name & ", " & vbCrLf
        End If
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    sCode = sCode & vbTab & ")" & vbCrLf
    sCode = sCode & vbTab & "VALUES" & vbCrLf
    sCode = sCode & vbTab & "(" & vbCrLf
    For Each col In m_table.Columns
        If (col.Name <> cmbPrimaryKey.Text) Then
            sCode = sCode & vbTab & vbTab & "@" & col.Name & ", " & vbCrLf
        End If
    Next col
    sCode = Left$(sCode, Len(sCode) - 4) & vbCrLf
    sCode = sCode & vbTab & ")" & vbCrLf
    sCode = sCode & vbTab & "SELECT @@identity" & vbCrLf
    sCode = sCode & "Return" & vbCrLf
    
    GenerateInsertSP = sCode
End Function


Private Sub btnGenerate_Click()
    If (Not m_table Is Nothing And Not m_cat Is Nothing) Then
        txtVB.Text = GenerateCreateNewVB
        
        txtSP.Text = GenerateLoadSP
        txtSP.Text = txtSP & vbCrLf & vbCrLf
        txtSP.Text = txtSP & GenerateDeleteSP
        txtSP.Text = txtSP & vbCrLf & vbCrLf
        If (chkCombine.Value = 1) Then
            txtSP.Text = txtSP & GenerateCombinedUpdateSP
        Else
            txtSP.Text = txtSP & GenerateUpdateSP
            txtSP.Text = txtSP & vbCrLf & vbCrLf
            txtSP.Text = txtSP & GenerateInsertSP
        End If
    Else
        MsgBox "You must first open a datasource. Enter the datasource and click 'Refresh', then select a table and primary key.", vbExclamation + vbOK, "Please select a data source"
    End If
End Sub

Private Sub btnRefresh_Click()
    Dim table As ADOX.table
    Set m_cat = New ADOX.Catalog
    On Error GoTo Refresh_Error

    m_cat.ActiveConnection = txtDSN.Text
    cmbTable.Clear
    For Each table In m_cat.Tables
        If (chkView.Value = 1) Then
            If (table.Type = "TABLE" Or table.Type = "VIEW") Then
                cmbTable.AddItem (table.Name)
            End If
        Else
            If (table.Type = "TABLE") Then
                cmbTable.AddItem (table.Name)
            End If
        End If
    Next table
    cmbTable.Text = cmbTable.List(0)
    Exit Sub
Refresh_Error:
    MsgBox Err.Description, vbCritical, "Critical Error"
End Sub

Private Sub cmbTable_Click()
    Dim col As ADOX.Column
    
    Set m_table = m_cat.Tables(cmbTable.Text)
    cmbPrimaryKey.Clear
    For Each col In m_table.Columns
        cmbPrimaryKey.AddItem (col.Name)
    Next col
    If (cmbPrimaryKey.List(0) <> "") Then
        cmbPrimaryKey.Text = cmbPrimaryKey.List(0)
    End If
End Sub
