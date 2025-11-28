Attribute VB_Name = "modVisData"
'------------------------------------------------------------
' VISDATA.BAS
' support functions for the Visual Data sample application
'
' General Information: This app is intended to demonstrate
'   and exercise all of the functionality available in the
'   DAO (Data Access Objects) in Visual Basic 5.0.
'
'------------------------------------------------------------

Option Explicit
'>>>>>>>>>>>>>>>>>>>>>>>>
Const MSG1 = "Execute Commit or Rollback First."
Const MSG2 = "Closing Recordsets"
Const MSG3 = "Table already exists, delete it?"
Const MSG4 = "Enter New Table Name:"
Const MSG5 = "Ready"
Const MSG6 = ", please wait..."
Const MSG7 = "Refreshing Table List"
Const MSG8 = "Number: "
Const MSG9 = "Display the Data Access Errors Collection?"
Const MSG10 = "Can't Open a Table Object on an Attached Table, Use Dynaset?"
Const MSG11 = "Opening Attached Table as Dynaset"
Const MSG12 = "Opening Attached Table as Snapshot"
Const MSG13 = "Opening Full Table"
Const MSG14 = "Opening Single Table Dynaset"
Const MSG15 = "Opening Single Table Snapshot"
Const MSG16 = "Opening PassThru Snapshot"
Const MSG17 = "Is this a SQLPassThrough Query?"
Const MSG18 = "Enter Connect property value:"
Const MSG19 = "Can't Open a Table Object from a QueryDef, Use Dynaset?"
Const MSG20 = "Opening Query Snapshot"
Const MSG21 = "Opening Query Dynaset"
Const MSG22 = "SQL Statement"
Const MSG23 = "Execute "
Const MSG24 = " Query?"
Const MSG25 = "Executing Query"
Const MSG26 = "  [Not Updatable]"
Const MSG27 = "Table already exists, Delete it?"
Const MSG28 = "QueryDef already exists, Delete it?"
Const MSG29 = "Enter Value for Parameter:"
Const MSG30 = "There are no current data access errors!"
Const MSG31 = "Can't show Errors at this time!"
Const MSG32 = "Data has been changed, Commit it?"
Const MSG33 = "RollBack All changes?"
Const MSG34 = "Can't Close with Transactions Pending!"
Const MSG35 = "You must Close First!"
Const MSG36 = "Open Microsoft Access Database"
Const MSG37 = "Open Dbase Database"
Const MSG38 = "Open FoxPro Database"
Const MSG39 = "Open Paradox Database"
Const MSG40 = "Open Excel File"
Const MSG41 = "Open Btrieve Database"
Const MSG42 = "Open Text Database"
Const MSG43 = "Opening Database"
Const MSG44 = "NOTE: Use of Attached Tables is the Recommended Method"
Const MSG45 = "Repairing "
Const MSG46 = "Attempt to Repair it?"
Const MSG47 = "Enter Directory Name for New ISAM Database:"
Const MSG48 = "Select Microsoft Access Database to Compact"
Const MSG49 = "Microsoft Access MDBs (*.mdb)|*.mdb"
Const MSG50 = "|All Files (*.*)|*.*"
Const MSG51 = "Select Microsoft Access Database to Compact to"
Const MSG52 = "Encrypt Compacted Database?"
Const MSG53 = "Compacting "
Const MSG54 = "Open Newly Compacted Database?"
Const MSG55 = "Select Microsoft Access Database to Create"
Const MSG56 = "Exporting Table: "
Const MSG57 = "Export "
Const MSG58 = "in "
Const MSG59 = "Creating Indexes:"
Const MSG60 = "Successfully Exported:"
Const MSG61 = "Successfully Exported SQL Statement."
Const MSG62 = "Table already exists - overwrite?"
Const MSG63 = "Importing Table: "
Const MSG64 = "Successfully Imported:"
Const MSG65 = "Invalid Directory Name!"
'>>>>>>>>>>>>>>>>>>>>>>>>


'api declarations
Declare Function SQLAllocEnv% Lib "ODBC32.DLL" (env&)
Declare Function SQLDataSources% Lib "ODBC32.DLL" (ByVal henv&, ByVal fdir%, ByVal szDSN$, ByVal cbDSNMax%, pcbDSN%, ByVal szDesc$, ByVal cbDescMax%, pcbDesc%)

'Public object variables
Public gVDClass      As New VisDataClass
Public gnodDBNode    As Node        'current database node in treeview
Public gnodDBNode2   As Node        'backup of current database node in treeview
Public gwsMainWS     As Workspace   'main workspace object
Public gdbCurrentDB  As Database    'main database object
Public gbDBOpenFlag  As Integer     'flag to know if a db is open
Public gPropObject   As Object      'object to show properties on
Public gDataCtlObj   As Object      'Public data control object
Public gtdfTableDef  As TableDef    'Public tabledef used by frmTblStruct
Public gnFormType    As Integer     'form type chosen on main form
                                    '0 = data control
                                    '1 = no data control
                                    '2 = grid control
Public gnRSType      As Integer     'recordset type chosen on main form
                                    '0 = table
                                    '1 = dynaset
                                    '2 = snapshot

'Public database variables
Public gsDataType       As String   'data backend = connect string
                                    'for everything accept Access
Public gsDBName         As String   'current database name
Public gsODBCDatasource As String   'Public odbc values
Public gsODBCDatabase   As String   '       "
Public gsODBCUserName   As String   '       "
Public gsODBCPassword   As String   '       "
Public gsODBCDriver     As String   '       "
Public gsODBCServer     As String   '       "
Public gsTblName        As String   '
Public glQueryTimeout   As Long     '
Public glLoginTimeout   As Long     '
Public gsTableDynaFilter As String  '
Public gnReadOnly       As Integer  'database readonly flag

'other Public vars
Public gsZoomData       As String   'pass info to the zoom form

'multi user variables
Public gnMURetryCnt     As Integer
Public gnMUDelay        As Integer
Public gnMULocking      As Integer  'flag for pessimistic or optimistic locking

'Public find values used to pass info between
'the dynaset form and find dialog
Public gbFindFailed     As Boolean
Public gsFindExpr       As String
Public gsFindOp         As String
Public gsFindField      As String
Public gnFindType       As Integer
Public gbFromTableView  As Boolean

'Public seek values used to pass info between
'the table form and find dialog
Public gsSeekOperator   As String
Public gsSeekValue      As String

'Public flags
Public gbDBChanged      As Boolean   '
Public gbTransPending   As Boolean   'used for transaction management
Public gbFromSQL        As Boolean   'source of sql statement was SQL form
Public gbAddTableFlag   As Boolean   'new or design designator
Public gbSettingDataCtl As Boolean   'used to reset data control props

'Public vars used in the Import Export Code
Public gImpDB As Database
Public gExpDB As Database
Public gExpTable As String

'data backend types used as the connect string
Public Const gsMSACCESS = "Microsoft Access"
Public Const gsDBASEIII = "Dbase III;"
Public Const gsDBASEIV = "Dbase IV;"
Public Const gsDBASE5 = "Dbase 5.0;"
Public Const gsFOXPRO20 = "FoxPro 2.0;"
Public Const gsFOXPRO25 = "FoxPro 2.5;"
Public Const gsFOXPRO26 = "FoxPro 2.6;"
Public Const gsFOXPRO30 = "FoxPro 3.0;"
Public Const gsPARADOX3X = "Paradox 3.X;"
Public Const gsPARADOX4X = "Paradox 4.X;"
Public Const gsPARADOX5X = "Paradox 5.X;"
Public Const gsBTRIEVE = "Btrieve;"
Public Const gsEXCEL30 = "Excel 3.0;"
Public Const gsEXCEL40 = "Excel 4.0;"
Public Const gsEXCEL50 = "Excel 5.0;"
Public Const gsTEXTFILES = "Text;"
Public Const gsSQLDB = "ODBC;"

'import/export data types
Public gnDataType As gnDataTypes
Public Enum gnDataTypes
  gnDT_NONE = -1
  gnDT_MSACCESS = 0
  gnDT_DBASEIV = 1
  gnDT_DBASEIII = 2
  gnDT_FOXPRO26 = 3
  gnDT_FOXPRO25 = 4
  gnDT_FOXPRO20 = 5
  gnDT_PARADOX4X = 6
  gnDT_PARADOX3X = 7
  gnDT_BTRIEVE = 8
  gnDT_EXCEL50 = 9
  gnDT_EXCEL40 = 10
  gnDT_EXCEL30 = 11
  gnDT_TEXTFILE = 12
  gnDT_SQLDB = 13
End Enum

'Public constants
Public Const APPNAME = "VisData6"
Public Const gsDEFAULT_DRIVER = "SQL Server"  'used for registerdatabase
Public Const gnEOF_ERR = 626                  '
Public Const gnFTBLS = 0                      '
Public Const gnFFLDS = 1                      '
Public Const gnFINDX = 2                      '
Public Const gnMAX_GRID_ROWS = 31999          '
Public Const gnMAX_MEMO_SIZE = 20000          '
Public Const gnGETCHUNK_CUTOFF = 50           '

Public Const gnFORM_DATACTL = 0               '
Public Const gnFORM_NODATACTL = 1             '
Public Const gnFORM_DATAGRID = 2              '

Public Const gnRS_TABLE = vbRSTypeTable
Public Const gnRS_DYNASET = vbRSTypeDynaset
Public Const gnRS_SNAPSHOT = vbRSTypeSnapShot
Public Const gnRS_PASSTHRU = 8

Public Const gnCTLARRAYHEIGHT = 340&          '
Public Const gnSCREEN = 0                     'used to center forms on screen
Public Const gnMDIFORM = 1                    'used to center forms on frmMDI

Public Const TABLE_STR = "Table"
Public Const ATTACHED_STR = "Attached"
Public Const QUERY_STR = "Query"
Public Const FIELD_STR = "Field"
Public Const FIELDS_STR = "Fields"
Public Const INDEX_STR = "Index"
Public Const INDEXES_STR = "Indexes"
Public Const PROPERTY_STR = "Property"
Public Const PROPERTIES_STR = "Properties"

Public Const APP_CATEGORY = "Microsoft Visual Basic AddIns"

Private Declare Function LoadLibraryEx Lib "kernel32" Alias "LoadLibraryExA" (ByVal lpLibFileName As String, ByVal hFile As Long, ByVal dwFlags As Long) As Long
Private Declare Function LoadStringA Lib "user32" (ByVal hInstance As Long, ByVal wID As Long, ByVal lpBuffer As String, ByVal nBufferMax As Long) As Long
Private Declare Function FreeLibrary Lib "kernel32" (ByVal hLibModule As Long) As Long

'LoadLibrary constants
Public Const LOAD_LIBRARY_AS_DATAFILE As Long = &H2

'GetLocaleInfo constants
Public Const LOCALE_SLANGUAGE = &H2
Public Const LOCALE_SABBREVLANGNAME = &H3

Private m_HelpServices As VsHelpServices.VsHelpServices
Private m_lcid  As Long
Private m_sHelpFile As String


Sub Main()
  frmMDI.Show
End Sub


'------------------------------------------------------------
'this function returns the type of querydef
'for the item selected in the querydefs
'list on the frmTables form
'------------------------------------------------------------
Function ActionQueryType(qdf As QueryDef) As String
  
  'check to see if it is an action query
  If (qdf.Type And dbQAction) = 0 Then
    ActionQueryType = vbNullString
    Exit Function
  End If
  
  'must be an action query type
  Select Case qdf.Type
    Case dbQCrosstab
      ActionQueryType = "Cross Tab"
    Case dbQDelete
      ActionQueryType = "Delete"
    Case dbQUpdate
      ActionQueryType = "Update"
    Case dbQAppend
      ActionQueryType = "Append"
    Case dbQMakeTable
      ActionQueryType = "Make Table"
    Case dbQDDL
      ActionQueryType = "DDL"
    Case dbQSQLPassThrough
      ActionQueryType = "SQLPassThrough"
    Case dbQSetOperation
      ActionQueryType = "Set Operation"
    Case dbQSPTBulk
      ActionQueryType = "SPT Bulk"
    Case Else
      ActionQueryType = vbNullString
  End Select

End Function

'------------------------------------------------------------
'this functions adds [] to object names that might need
'them because they have spaces in them
'------------------------------------------------------------
Function AddBrackets(rObjName As String) As String
  'add brackets to object names w/ spaces in them
  If InStr(rObjName, " ") > 0 And Mid(rObjName, 1, 1) <> "[" Then
    AddBrackets = "[" & rObjName & "]"
  Else
    AddBrackets = rObjName
  End If
End Function

'------------------------------------------------------------
'this function checks to see if a transaction is pending
'and displays a message is one is
'------------------------------------------------------------
Function CheckTransPending(MSG As String) As Integer

  If gbTransPending Then
    MsgBox MSG & vbCrLf & MSG1, 48
    CheckTransPending = True
  Else
    CheckTransPending = False
  End If

End Function

'------------------------------------------------------------
'clear out the data fields on the table and dynasnap forms
'------------------------------------------------------------
Sub ClearDataFields(frm As Form, nCnt As Integer)
  Dim i As Integer

  'clear out the fields on the main form
  For i = 0 To nCnt - 1
    frm.txtFieldData(i).Text = vbNullString
  Next
End Sub

'------------------------------------------------------------
'this sub closes all frmDynaSnap, frmTableObj and frmDataGrid
'forms by looking for forms with a Tag set to "Recordset"
'------------------------------------------------------------
Sub CloseAllRecordsets()
  Dim i As Integer

  MsgBar MSG2, True
  While i < Forms.Count
    If Forms(i).Tag = "Recordset" Then
      Unload Forms(i)
    Else
      i = i + 1
    End If
  Wend
  MsgBar vbNullString, False

End Sub

'------------------------------------------------------------
'this function copies data from one table to another
'from the frmCopyStruct form
'It demonstrates the use of transactions to speed up this
'type of operation
'------------------------------------------------------------
Function CopyData(rFromDB As Database, rToDB As Database, rFromName As String, rToName As String) As Integer
  On Error GoTo CopyErr

  Dim recRecordset1 As Recordset, recRecordset2 As Recordset
  Dim i As Integer
  Dim nRC As Integer
  Dim fld As Field

  'open both recordsets
  Set recRecordset1 = rFromDB.OpenRecordset(rFromName)
  Set recRecordset2 = rToDB.OpenRecordset(rToName)
  gwsMainWS.BeginTrans
  While recRecordset1.EOF = False
    recRecordset2.AddNew
    'this loop copies the data from each field to
    'the new table
'    For Each fld In recRecordset1.Fields
    For i = 0 To recRecordset1.Fields.Count - 1
      Set fld = recRecordset1.Fields(i)
      recRecordset2(fld.Name).Value = fld.Value
    Next
    recRecordset2.Update
    recRecordset1.MoveNext
    nRC = nRC + 1
    'this test will commit transactions every 1000 records
    If nRC = 1000 Then
      gwsMainWS.CommitTrans
      gwsMainWS.BeginTrans
      nRC = 0
    End If
  Wend
  gwsMainWS.CommitTrans

  CopyData = True
  Exit Function

CopyErr:
  gwsMainWS.Rollback
  ShowError
  CopyData = False
End Function

'------------------------------------------------------------
'this function copies the structure of one table to
'a new table in the same or different database
'------------------------------------------------------------
Function CopyStruct(vFromDB As Database, vToDB As Database, vFromName As String, vToName As String, bCreateIndex As Integer) As Integer
  On Error GoTo CSErr

  Dim i As Integer
  Dim tblTableDefObj As TableDef
  Dim fldFieldObj As Field
  Dim indIndexObj As Index
  Dim tdf As TableDef
  Dim fld As Field
  Dim idx As Index
  
  'search to see if table exists
NameSearch:
'  For Each tdf In vToDB.Tabledefs
  For i = 0 To vToDB.TableDefs.Count - 1
    Set tdf = vToDB.TableDefs(i)
    If UCase(tdf.Name) = UCase(vToName) Then
      If MsgBox(MSG3, 4) = vbYes Then
         vToDB.TableDefs.Delete tdf.Name
      Else
         vToName = InputBox(MSG4)
         If Len(vToName) = 0 Then
           Exit Function
         Else
           GoTo NameSearch
         End If
      End If
      Exit For
    End If
  Next
  
  Set tblTableDefObj = gdbCurrentDB.CreateTableDef()
    
  'strip off owner if needed
  tblTableDefObj.Name = StripOwner(vToName)

  'create the fields
'  For Each fld In vFromDB.Tabledefs(vFromName).Fields
  For i = 0 To vFromDB.TableDefs(vFromName).Fields.Count - 1
    Set fld = vFromDB.TableDefs(vFromName).Fields(i)
    Set fldFieldObj = vFromDB.TableDefs(vFromName).CreateField(fld.Name, fld.Type, fld.Size)
    tblTableDefObj.Fields.Append fldFieldObj
  Next

  'create the indexes
  If bCreateIndex <> False Then
'    For Each idx In vFromDB.Tabledefs(vFromName).Indexes
    For i = 0 To vFromDB.TableDefs(vFromName).Indexes.Count - 1
      Set idx = vFromDB.TableDefs(vFromName).Indexes(i)
      Set indIndexObj = vFromDB.TableDefs(vFromName).CreateIndex(idx.Name)
      With indIndexObj
        indIndexObj.Fields = idx.Fields
        indIndexObj.Unique = idx.Unique
        If gsDataType <> gsSQLDB Then
          indIndexObj.Primary = idx.Primary
        End If
      End With
      tblTableDefObj.Indexes.Append indIndexObj
    Next
  End If

  'append the new table
  vToDB.TableDefs.Append tblTableDefObj

  CopyStruct = True
  Exit Function

CSErr:
  ShowError
  CopyStruct = False
End Function

'------------------------------------------------------------
'this function fills a list or combo box with the
'tables (and querydefs) from the Tables form
'ItemData is set to 0 for a tabledef and 1 for a querydef
'------------------------------------------------------------
Sub GetTableList(rctl As Control, rbIncludeQDFs As Integer, rbIncludeSys As Integer, rbStripConnect As Integer)
  On Error GoTo FTLErr
  
  Dim i As Integer
  Dim sTmp As String
  Dim tbl As TableDef
  Dim qdf As QueryDef
  
  'add the tabledefs
  For Each tbl In gdbCurrentDB.TableDefs
    sTmp = tbl.Name
    If rbIncludeSys Then
      rctl.AddItem sTmp
      rctl.ItemData(rctl.NewIndex) = 0
    Else
      If (gdbCurrentDB.TableDefs(sTmp).Attributes And dbSystemObject) = 0 Then
        rctl.AddItem sTmp
        rctl.ItemData(rctl.NewIndex) = 0
      End If
    End If
  Next
  
  'add the querydefs
  If rbIncludeQDFs Then
    For Each qdf In gdbCurrentDB.QueryDefs
      rctl.AddItem qdf.Name
      rctl.ItemData(rctl.NewIndex) = 1
    Next
  End If
  
  Exit Sub
  
FTLErr:
  ShowError
End Sub

'------------------------------------------------------------
'this function returns the numeric field type
'for the passed in string
'------------------------------------------------------------
Function GetFieldType(rFldType As String) As Integer
  'return field length
  If rFldType = "Text" Then
    GetFieldType = dbText
  Else
    Select Case rFldType
      Case "Counter"
        GetFieldType = dbLong
      Case "Boolean"
        GetFieldType = dbBoolean
      Case "Byte"
        GetFieldType = dbByte
      Case "Integer"
        GetFieldType = dbInteger
      Case "Long"
        GetFieldType = dbLong
      Case "Currency"
        GetFieldType = dbCurrency
      Case "Single"
        GetFieldType = dbSingle
      Case "Double"
        GetFieldType = dbDouble
      Case "Date/Time"
        GetFieldType = dbDate
      Case "Binary"
        GetFieldType = dbLongBinary
      Case "Memo"
        GetFieldType = dbMemo
    End Select
  End If

End Function

'------------------------------------------------------------
'this function returns an appropriate field width for the
'field type passed in to be used for the control width on
'frmDynaSnap and frmTableObj forms
'------------------------------------------------------------
Function GetFieldWidth(rType As Integer)
  Select Case rType
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
    Case dbLongBinary
      GetFieldWidth = 3250
    Case dbMemo
      GetFieldWidth = 3250
    Case Else
      GetFieldWidth = 3250
  End Select

End Function

'------------------------------------------------------------
'this function returns the Registry setting for the
'passed in item and section
'------------------------------------------------------------
Function GetRegistryString(ByVal vsItem As String, ByVal vsDefault As String) As String
  GetRegistryString = GetSetting(APP_CATEGORY, APPNAME, vsItem, vsDefault)
End Function

'------------------------------------------------------------
'this sub hides the menus and toolbar that only apply
'when a database is open
'------------------------------------------------------------
Sub HideDBTools()
  frmMDI.mnuDBClose.Enabled = False
  frmMDI.mnuDBImpExp.Enabled = False
  frmMDI.mnuUtil.Enabled = False
  frmMDI.mnuUBar1.Visible = False
  frmMDI.mnuUAttachments.Visible = False
  frmMDI.mnuUGroupsUsers.Visible = False
  frmMDI.mnuUSystemDB.Visible = False
  frmMDI.tlbToolBar.Buttons("BeginTrans").Enabled = False
  frmMDI.tlbToolBar.Buttons("Commit").Enabled = False
  frmMDI.tlbToolBar.Buttons("Rollback").Enabled = False
End Sub

'------------------------------------------------------------
'this sub displays the passed in message in the status
'bar on the bottom of the MDI form
'------------------------------------------------------------
Sub MsgBar(rsMsg As String, rPauseFlag As Integer)
  If Len(rsMsg) = 0 Then
    Screen.MousePointer = vbDefault
    frmMDI.stsStatusBar.Panels(1).Text = MSG5
  Else
    If rPauseFlag Then
      frmMDI.stsStatusBar.Panels(1).Text = rsMsg & MSG6
    Else
      frmMDI.stsStatusBar.Panels(1).Text = rsMsg
    End If
  End If
  frmMDI.stsStatusBar.Refresh
End Sub

'==================================================
' Routine: ObjectExists
'
' Purpose: Determine whether or not a member exists
'          same as MemberExists except that the 1st arg is declared
'          as an object to allow passing in collections such as
'          VBComponents, VBProjects, etc.
' Arguments:
'   pColl: Name of Collection to check in
'   sMemName: Name(key) of member to check for
' Outputs:
'   True: member exists in collection
'   False: member does not exist in the collection
' Maintenance: J$
'==================================================
Function ObjectExists(pColl As Object, sMemName As String) As Boolean
Dim pObj As Object
  
  On Error Resume Next
  Err = 0
  Set pObj = pColl(sMemName)
  ObjectExists = (Err = 0)
End Function


'------------------------------------------------------------
'this sub refreshs any table list passed in as an object
'------------------------------------------------------------
Sub RefreshTables(rListObject As Object)
  On Error GoTo TRefErr

  Dim tdf As TableDef
  Dim qdf As QueryDef
  Dim sTmp As String
    
  Dim i As Integer
    
  MsgBar MSG7, True
  Screen.MousePointer = vbHourglass

  'if this is called to refresh the database
  'window, bypass the old method of
  'filling a listbox with the table names
  If rListObject Is Nothing Then GoTo LoadTreeView

  rListObject.Clear
  If frmMDI.mnuPAllowSys.Checked Then
    'list all tables
    For Each tdf In gdbCurrentDB.TableDefs
      If (tdf.Attributes And dbAttachedTable) = dbAttachedTable Then
        If Left(tdf.Connect, 1) = ";" Then
          'must be a Microsoft Access attached table
          rListObject.AddItem tdf.Name & " -> Microsoft Access"
        Else
          'must be an ISAM attached table
          rListObject.AddItem tdf.Name & " -> " & Left(tdf.Connect, InStr(tdf.Connect, ";") - 1)
        End If
      ElseIf (tdf.Attributes And dbAttachedODBC) = dbAttachedODBC Then
        rListObject.AddItem tdf.Name & " -> ODBC"
      Else
        rListObject.AddItem tdf.Name
      End If
    Next
  Else
    'don't list system tables
    For Each tdf In gdbCurrentDB.TableDefs
      If (tdf.Attributes And dbSystemObject) = 0 Then
        If (tdf.Attributes And dbAttachedTable) = dbAttachedTable Then
          If Left(tdf.Connect, 1) = ";" Then
            'must be a Microsoft Access attached table
            rListObject.AddItem tdf.Name & " -> Microsoft Access"
          Else
            'must be an ISAM attached table
            rListObject.AddItem tdf.Name & " -> " & Left(tdf.Connect, InStr(tdf.Connect, ";") - 1)
          End If
        ElseIf (tdf.Attributes And dbAttachedODBC) = dbAttachedODBC Then
          rListObject.AddItem tdf.Name & " -> ODBC"
        Else
          rListObject.AddItem tdf.Name
        End If
      End If
    Next
  End If
  'select the 1st item if there is any
  If rListObject.ListCount > 0 Then
    rListObject.ListIndex = 0
  End If
  
LoadTreeView:
  frmDatabase.LoadDatabase
  
  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False
  Exit Sub

TRefErr:
  ShowError
End Sub

'------------------------------------------------------------
'this function returns the size of the field type
'passed in for use on the frmAddField form
'------------------------------------------------------------
Function SetFldProperties(rnType As Integer) As Integer
  'return field length
  Select Case rnType
    Case dbBoolean
      SetFldProperties = 1
    Case dbByte
      SetFldProperties = 1
    Case dbInteger
      SetFldProperties = 2
    Case dbLong
      SetFldProperties = 4
    Case dbCurrency
      SetFldProperties = 8
    Case dbSingle
      SetFldProperties = 4
    Case dbDouble
      SetFldProperties = 8
    Case dbDate
      SetFldProperties = 8
    Case dbText
      SetFldProperties = 50
    Case dbLongBinary
      SetFldProperties = 0
    Case dbMemo
      SetFldProperties = 0
  End Select
End Function

'------------------------------------------------------------
'this sub shows the menus and toolbar that only apply
'when a database is open
'------------------------------------------------------------
Sub ShowDBTools()
  Dim sTmp As String

  frmMDI.mnuDBClose.Enabled = True
  frmMDI.mnuDBImpExp.Enabled = True
  frmMDI.mnuUtil.Enabled = True
  frmMDI.tlbToolBar.Buttons("BeginTrans").Enabled = True
  frmMDI.tlbToolBar.Buttons("Commit").Enabled = False
  frmMDI.tlbToolBar.Buttons("Rollback").Enabled = False
  frmMDI.tlbToolBar.Refresh

  'set general items that apply/don't apply to MDBs
  If gsDataType = gsMSACCESS Then
    frmMDI.mnuUBar1.Visible = True
    frmMDI.mnuUAttachments.Visible = True
    frmMDI.mnuUGroupsUsers.Visible = True
    frmMDI.mnuUSystemDB.Visible = True
    frmSQL.cmdSaveQueryDef.Visible = True
    frmMDI.mnuDBPURename.Visible = True
  Else
    frmSQL.cmdSaveQueryDef.Visible = False
    frmMDI.mnuDBPURename.Visible = False
  End If

  'set ODBC specific items
  If gsDataType = gsSQLDB Then
    If gnRSType = gnRS_TABLE Then
      frmMDI.tlbToolBar.Buttons("Dynaset").Value = tbrPressed
      gnRSType = gnRS_DYNASET
    End If
    frmMDI.tlbToolBar.Buttons("PassThrough").Visible = True
    frmMDI.tlbToolBar.Buttons("Table").Visible = False
  Else
    If gnRSType = gnRS_PASSTHRU Then
      frmMDI.tlbToolBar.Buttons("Dynaset").Value = tbrPressed
      gnRSType = gnRS_DYNASET
    End If
    frmMDI.tlbToolBar.Buttons("PassThrough").Visible = False
    frmMDI.tlbToolBar.Buttons("Table").Visible = True
  End If
  frmMDI.tlbToolBar.Refresh
  'show the 2 main child forms
  frmDatabase.Show
  frmSQL.Show
End Sub

'------------------------------------------------------------
'this sub displays the error message with it's Err code
'and prompts to show the Errors collection if it
'is a data access type error
'------------------------------------------------------------
Sub ShowError()
  Dim sTmp As String

  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False

  sTmp = "The following Error occurred:" & vbCrLf & vbCrLf
  'add the error string
  sTmp = sTmp & Err.Description & vbCrLf
  'add the error number
  sTmp = sTmp & MSG8 & Err
  
  Beep
  'check to see if the error is from the db errors collection
  If DBEngine.Errors.Count > 0 Then
    If DBEngine.Errors(0).Number = Err Then
      'add the prompt to display the errors collection
      sTmp = sTmp & vbCrLf & vbCrLf & MSG9
      'beep and show the error
      If MsgBox(sTmp, vbYesNo + vbQuestion) = vbYes Then
        RefreshErrors
      End If
    Else
      MsgBox sTmp
    End If
  Else
    MsgBox sTmp
  End If

End Sub

'------------------------------------------------------------
'this function strips the attached table connect string off
'------------------------------------------------------------
Function StripConnect(rsTblName As String) As String
  If InStr(rsTblName, "->") > 0 Then
    StripConnect = Left(rsTblName, InStr(rsTblName, "->") - 2)
  Else
    StripConnect = rsTblName
  End If

End Function

'------------------------------------------------------------
'this function strips the [] off of data objects
'------------------------------------------------------------
Function StripBrackets(rsObjName As String) As String
  'add brackets to object names w/ spaces in them
  If Mid(rsObjName, 1, 1) = "[" Then
    StripBrackets = Mid(rsObjName, 2, Len(rsObjName) - 2)
  Else
    StripBrackets = rsObjName
  End If

End Function

'------------------------------------------------------------
'this function strips the file name from a path\file string
'------------------------------------------------------------
Function StripFileName(rsFileName As String) As String
  On Error Resume Next
  Dim i As Integer

  For i = Len(rsFileName) To 1 Step -1
    If Mid(rsFileName, i, 1) = "\" Then
      Exit For
    End If
  Next

  StripFileName = Mid(rsFileName, 1, i - 1)

End Function

'------------------------------------------------------------
'this function strips the non ACSII chars off memo field
'data before displaying it (not sure this is always needed)
'------------------------------------------------------------
Function StripNonAscii(rvntVal As Variant) As String
  Dim i As Integer
  Dim sTmp As String

  'stubbed out to enable DBCS chars
  StripNonAscii = rvntVal
  Exit Function

  For i = 1 To Len(rvntVal)
    If Asc(Mid(rvntVal, i, 1)) < 32 Or Asc(Mid(rvntVal, i, 1)) > 126 Then
      sTmp = sTmp & " "
    Else
      sTmp = sTmp & Mid(rvntVal, i, 1)
    End If
  Next

  StripNonAscii = sTmp

End Function

'------------------------------------------------------------
'strips the owner off of ODBC table names
'------------------------------------------------------------
Function StripOwner(rsTblName As String) As String

  If InStr(rsTblName, ".") > 0 Then
    rsTblName = Mid(rsTblName, InStr(rsTblName, ".") + 1, Len(rsTblName))
  End If
  StripOwner = rsTblName

End Function

'------------------------------------------------------------
'returns the true or false string
'------------------------------------------------------------
Function stTrueFalse(rvntTF As Variant) As String
  If rvntTF Then
    stTrueFalse = "True"
  Else
    stTrueFalse = "False"
  End If
End Function

'------------------------------------------------------------
'returns "" if a field is Null
'------------------------------------------------------------
Function vFieldVal(rvntFieldVal As Variant) As Variant
  If IsNull(rvntFieldVal) Then
    vFieldVal = vbNullString
  Else
    vFieldVal = CStr(rvntFieldVal)
  End If
End Function

'------------------------------------------------------------
'loads all saved Registry settings for VisData
'------------------------------------------------------------
Sub LoadRegistrySettings()
  On Error Resume Next
  
  Dim sTmp As String
  Dim x As Integer

  glQueryTimeout = Val(GetRegistryString("QueryTimeout", "5"))
  glLoginTimeout = Val(GetRegistryString("LoginTimeout", "20"))
  
  
  frmMDI.mnuPOpenOnStartup.Checked = Val(GetRegistryString("OpenOnStartup", "0"))
  frmMDI.mnuPAllowSys.Checked = Val(GetRegistryString("AllowSys", "0"))

  'get the most recently used databases
  For x = 1 To 8
    sTmp = GetRegistryString("MRUDatabase" & x, "")
    If Len(sTmp) > 0 Then
      frmMDI.mnuBarMRU.Visible = True
      frmMDI.mnuDBMRU(x).Caption = "&" & x & " " & sTmp
      frmMDI.mnuDBMRU(x).Visible = True
      sTmp = GetRegistryString("MRUConnect" & x, "")
      frmMDI.mnuDBMRU(x).Tag = sTmp
    End If
  Next

  'get the last used database out of the Registry
  gsDataType = GetRegistryString("DataType", vbNullString)
  gsDBName = GetRegistryString("DatabaseName", vbNullString)
  gsODBCDatasource = GetRegistryString("ODBCDatasource", vbNullString)
  gsODBCDatabase = GetRegistryString("ODBCDatabase", vbNullString)
  gsODBCUserName = GetRegistryString("ODBCUserName", vbNullString)
  gsODBCPassword = GetRegistryString("ODBCPassword", vbNullString)
  gsODBCDriver = GetRegistryString("ODBCDriver", vbNullString)
  gsODBCServer = GetRegistryString("ODBCServer", vbNullString)

  sTmp = GetRegistryString("ViewMode", CStr(gnFORM_NODATACTL))
  Select Case Val(sTmp)
    Case gnFORM_NODATACTL
      gnFormType = gnFORM_NODATACTL
    Case gnFORM_DATACTL
      gnFormType = gnFORM_DATACTL
    Case gnFORM_DATAGRID
      gnFormType = gnFORM_DATAGRID
  End Select
  sTmp = GetRegistryString("RecordsetType", CStr(vbRSTypeDynaset))
  Select Case Val(sTmp)
    Case vbRSTypeTable
      gnRSType = gnRS_TABLE
    Case vbRSTypeDynaset
      gnRSType = gnRS_DYNASET
    Case vbRSTypeSnapShot
      gnRSType = gnRS_SNAPSHOT
    Case gnRS_PASSTHRU
      gnRSType = gnRS_PASSTHRU
  End Select
  
  DoEvents
  Select Case gnFormType
    Case gnFORM_NODATACTL
      frmMDI.tlbToolBar.Buttons("NoDataControl").Value = tbrPressed
    Case gnFORM_DATACTL
      frmMDI.tlbToolBar.Buttons("DataControl").Value = tbrPressed
    Case gnFORM_DATAGRID
      frmMDI.tlbToolBar.Buttons("DBGrid").Value = tbrPressed
  End Select
  Select Case gnRSType
    Case vbRSTypeDynaset
      frmMDI.tlbToolBar.Buttons("Dynaset").Value = tbrPressed
    Case vbRSTypeSnapShot
      frmMDI.tlbToolBar.Buttons("Snapshot").Value = tbrPressed
    Case vbRSTypeTable
      frmMDI.tlbToolBar.Buttons("Table").Value = tbrPressed
    Case gnRS_PASSTHRU
      frmMDI.tlbToolBar.Buttons("PassThrough").Value = tbrPressed
  End Select
  
End Sub

'------------------------------------------------------------
'saves current VisData values to the registry
'------------------------------------------------------------
Sub SaveRegistrySettings()
  On Error Resume Next

  Dim i As Integer
  
  SaveSetting APP_CATEGORY, APPNAME, "DataType", gsDataType
  SaveSetting APP_CATEGORY, APPNAME, "DatabaseName", gsDBName
  SaveSetting APP_CATEGORY, APPNAME, "ODBCDatasource", gsODBCDatasource
  SaveSetting APP_CATEGORY, APPNAME, "ODBCDatabase", gsODBCDatabase
  SaveSetting APP_CATEGORY, APPNAME, "ODBCUserName", gsODBCUserName
  SaveSetting APP_CATEGORY, APPNAME, "ODBCPassword", gsODBCPassword
  SaveSetting APP_CATEGORY, APPNAME, "ODBCDriver", gsODBCDriver
  SaveSetting APP_CATEGORY, APPNAME, "ODBCServer", gsODBCServer
  SaveSetting APP_CATEGORY, APPNAME, "QueryTimeout", glQueryTimeout
  SaveSetting APP_CATEGORY, APPNAME, "LoginTimeout", glLoginTimeout
  DBEngine.LoginTimeout = glLoginTimeout
  SaveSetting APP_CATEGORY, APPNAME, "ViewMode", gnFormType
  SaveSetting APP_CATEGORY, APPNAME, "RecordsetType", gnRSType
  
  SaveSetting APP_CATEGORY, APPNAME, "OpenOnStartup", IIf(frmMDI.mnuPOpenOnStartup.Checked, "-1", "0")
  SaveSetting APP_CATEGORY, APPNAME, "AllowSys", IIf(frmMDI.mnuPAllowSys.Checked, "-1", "0")

  For i = 1 To 8
    If frmMDI.mnuDBMRU(i).Visible Then
      SaveSetting APP_CATEGORY, APPNAME, "MRUDatabase" & i, Mid(frmMDI.mnuDBMRU(i).Caption, 4, Len(frmMDI.mnuDBMRU(i).Caption))
      SaveSetting APP_CATEGORY, APPNAME, "MRUConnect" & i, frmMDI.mnuDBMRU(i).Tag
    Else
      SaveSetting APP_CATEGORY, APPNAME, "MRUDatabase" & i, ""
      SaveSetting APP_CATEGORY, APPNAME, "MRUConnect" & i, ""
    End If
  Next

  SaveSetting APP_CATEGORY, APPNAME, "WindowState", frmMDI.WindowState
  If frmMDI.WindowState = vbNormal Then
    SaveSetting APP_CATEGORY, APPNAME, "WindowTop", frmMDI.Top
    SaveSetting APP_CATEGORY, APPNAME, "WindowLeft", frmMDI.Left
    SaveSetting APP_CATEGORY, APPNAME, "WindowWidth", frmMDI.Width
    SaveSetting APP_CATEGORY, APPNAME, "WindowHeight", frmMDI.Height
  End If
  SaveSetting APP_CATEGORY, APPNAME, "ViewMode", gnFormType
  SaveSetting APP_CATEGORY, APPNAME, "RecordsetType", gnRSType

End Sub

'------------------------------------------------------------
'this sub will open the appropriate data type form and
'display the appropriate msg in the status bar based on
'user selected options on the main MDI form
'------------------------------------------------------------
Sub OpenTable(rName As String)
  On Error GoTo OpenTableErr

  Dim rsTmp As Recordset
  Dim rsADOTmp As ADODB.Recordset
  Dim conADOConn As ADODB.Connection
  Dim sTmp As String
  Dim nAttach As Integer
  Dim frmTmp As Form
  
  If gsDataType = gsMSACCESS Then   'look for attached tables if it's an MDB
    If (gdbCurrentDB.TableDefs(rName).Attributes And dbAttachedTable) = dbAttachedTable Then
      nAttach = 1
    ElseIf (gdbCurrentDB.TableDefs(rName).Attributes And dbAttachedODBC) = dbAttachedODBC Then
      nAttach = 2
    End If
    If nAttach > 0 And gnRSType = gnRS_TABLE Then
      Beep
      If MsgBox(MSG10, vbYesNo + vbQuestion) = vbYes Then
        frmMDI.tlbToolBar.Buttons("Dynaset").Value = tbrPressed         'reset to dynaset
      Else
        Exit Sub
      End If
    End If
  End If
  
  If nAttach > 0 Then
    If gnRSType = gnRS_DYNASET Then
      sTmp = MSG11
    ElseIf gnRSType = gnRS_SNAPSHOT Then
      sTmp = MSG12
    End If
  Else
    If gnRSType = gnRS_TABLE Then
      sTmp = MSG13
    ElseIf gnRSType = gnRS_DYNASET Then
      sTmp = MSG14
    ElseIf gnRSType = gnRS_SNAPSHOT Then
      sTmp = MSG15
    ElseIf gnRSType = gnRS_PASSTHRU Then
      sTmp = MSG16
    End If
  End If
  
  MsgBar sTmp, True
  
  Screen.MousePointer = vbHourglass
  If gnRSType = gnRS_TABLE Then
    Set rsTmp = gdbCurrentDB.OpenRecordset(rName, dbOpenTable)
    sTmp = "Table:"
  ElseIf gnRSType = gnRS_DYNASET Then
    Set rsTmp = gdbCurrentDB.OpenRecordset(rName, dbOpenDynaset)
    sTmp = "Dynaset:"
  ElseIf gnRSType = gnRS_SNAPSHOT Then
    Set rsTmp = gdbCurrentDB.OpenRecordset(rName, dbOpenSnapshot)
    sTmp = "Snapshot:"
  ElseIf gnRSType = gnRS_PASSTHRU Then
    Set rsTmp = gdbCurrentDB.OpenRecordset(rName, dbOpenSnapshot, dbSQLPassThrough)
    sTmp = "Passthrough Snapshot:"
  End If
  Screen.MousePointer = vbDefault
  
  If gnFormType = gnFORM_NODATACTL Then
    If gnRSType = gnRS_TABLE Then
      Set frmTmp = New frmTableObj
      sTmp = "Table:"
    Else
      Set frmTmp = New frmDynaSnap
    End If
    Set frmTmp.mrsFormRecordset = rsTmp
  ElseIf gnFormType = gnFORM_DATACTL Then
    Set frmTmp = New frmDataControl
    Set frmTmp.mrsFormRecordset = rsTmp
  ElseIf gnFormType = gnFORM_DATAGRID Then
    Set frmTmp = New frmDataGrid
    'need to convert the recordset to an ADO recordset
    Set conADOConn = New ADODB.Connection
    With conADOConn
      If Len(gsODBCDatasource) = 0 Then
        If gsDataType = gsMSACCESS Then
          .ConnectionString = "PROVIDER=Microsoft.Jet.OLEDB.3.51;Data Source=" & gsDBName
        Else
          .ConnectionString = "Provider=MSDASQL;Data Source=" & gsDBName
        End If
      Else
        .ConnectionString = "PROVIDER=MSDASQL;" & Mid$(gdbCurrentDB.Connect, 6)
      End If
      .Open
    End With
    Set rsADOTmp = New ADODB.Recordset
    With rsADOTmp
      .Open rsTmp.Name, conADOConn, adOpenStatic, adLockOptimistic, adCmdTable
    End With
    Set frmTmp.mrsFormRecordset = rsADOTmp
  End If
  
  frmTmp.Caption = sTmp & rName
  frmTmp.Show

  MsgBar vbNullString, False
  
  Exit Sub
OpenTableErr:
  ShowError
End Sub

'------------------------------------------------------------
'opens a QueryDef with the user selected form type
'------------------------------------------------------------
Sub OpenQuery(rName As String, bTemp As Boolean)
  On Error GoTo OpenQueryErr

  Dim sTmp As String
  Dim rsTmp As Recordset
  Dim rsADOTmp As ADODB.Recordset
  Dim conADOConn As ADODB.Connection
  Dim qdfTmp As QueryDef
  Dim sQueryType As String
  Dim frmTmp As Form
  Dim nDoIt As Integer
  Dim bReturnsRows As Boolean
  Dim bTriedAlready As Boolean

  If bTemp Then
    Set qdfTmp = gdbCurrentDB.CreateQueryDef("", rName)
    If MsgBox(MSG17, vbYesNo + vbQuestion + vbDefaultButton2) = vbYes Then
      sTmp = InputBox(MSG18)
      If Len(sTmp) > 0 Then
        qdfTmp.Connect = sTmp
      End If
    End If
    'assume it is non row returning to begin with
    bReturnsRows = False
  Else
    Set qdfTmp = gdbCurrentDB.QueryDefs(rName)
    sQueryType = ActionQueryType(qdfTmp)
    If qdfTmp.Type <> dbQSQLPassThrough Then
      'not a sql pass through so we need to set ReturnsRecords
      If qdfTmp.Type = 0 Or qdfTmp.Type = dbQCrosstab Then
        bReturnsRows = True
      Else
        bReturnsRows = False
      End If
    Else
      'get it from the qdf if it is passthrough
      bReturnsRows = qdfTmp.ReturnsRecords
    End If
  End If
  
  If bReturnsRows And (gnRSType = gnRS_TABLE) Then
    Beep
    If MsgBox(MSG19, vbYesNo + vbQuestion) = vbYes Then
      frmMDI.tlbToolBar.Buttons("Dynaset").Value = tbrPressed         'reset to recordset
    Else
      Exit Sub
    End If
  End If
  
  
  If bReturnsRows Then
    SetQDFParams qdfTmp
MakeDynaset:
    Screen.MousePointer = vbHourglass
    If qdfTmp.Type = dbQSQLPassThrough Then
      MsgBar MSG16, True
      Set rsTmp = qdfTmp.OpenRecordset(dbOpenSnapshot, dbSQLPassThrough)
    ElseIf gnRSType = gnRS_SNAPSHOT Then
      MsgBar MSG20, True
      Set rsTmp = qdfTmp.OpenRecordset(dbOpenSnapshot)
    Else
      MsgBar MSG21, True
      Set rsTmp = qdfTmp.OpenRecordset(dbOpenDynaset)
    End If
    Screen.MousePointer = vbDefault
    
    If gnFormType = gnFORM_NODATACTL Then
      Set frmTmp = New frmDynaSnap
      Set frmTmp.mrsFormRecordset = rsTmp
    ElseIf gnFormType = gnFORM_DATACTL Then
      Set frmTmp = New frmDataControl
      If qdfTmp.Parameters.Count > 0 Then
        frmTmp.mbIsParameterized = True
      End If
      Set frmTmp.mrsFormRecordset = rsTmp
    ElseIf gnFormType = gnFORM_DATAGRID Then
      Set frmTmp = New frmDataGrid
      'need to convert the recordset to an ADO recordset
      Set conADOConn = New ADODB.Connection
      With conADOConn
        If Len(gsODBCDatasource) = 0 Then
          If gsDataType = gsMSACCESS Then
            .ConnectionString = "PROVIDER=Microsoft.Jet.OLEDB.3.51;Data Source=" & gsDBName
          Else
            .ConnectionString = "Provider=MSDASQL;Data Source=" & gsDBName
          End If
        Else
          .ConnectionString = "PROVIDER=MSDASQL;" & Mid$(gdbCurrentDB.Connect, 6)
        End If
        .Open
      End With
      Set rsADOTmp = New ADODB.Recordset
      With rsADOTmp
        .Open rName, conADOConn, adOpenStatic, adLockOptimistic
      End With
      Set frmTmp.mrsFormRecordset = rsADOTmp
    End If
    
    If Len(qdfTmp.SQL) > 50 Then
      frmTmp.Caption = MSG22
    Else
      frmTmp.Caption = qdfTmp.SQL
    End If
    frmTmp.Show
    
  Else
    Screen.MousePointer = vbDefault
    If Len(sQueryType) > 0 Then
      nDoIt = MsgBox(MSG23 & sQueryType & MSG24, vbYesNo + vbQuestion)
    Else
      'no name so just try to execute it
      nDoIt = vbYes
    End If
    If nDoIt = vbYes Then
      SetQDFParams qdfTmp
      Screen.MousePointer = vbHourglass
      MsgBar MSG25, True
      qdfTmp.Execute
      If gdbCurrentDB.RecordsAffected > 0 Then
        If gbTransPending Then gbDBChanged = True
      End If
    End If
  End If
  
  MsgBar vbNullString, False
  
  Exit Sub
OpenQueryErr:
  If Err = 3065 Or Err = 3078 And (Not bTriedAlready) Then
    bTriedAlready = True
    'row returning so try to create recordset
    Resume MakeDynaset
  ElseIf Not bTriedAlready Then
    ShowError
  End If
  Screen.MousePointer = vbHourglass
  MsgBar vbNullString, False
End Sub

'------------------------------------------------------------
'this sub display all field data in the current row
'on the table and dynasnap forms
'------------------------------------------------------------
Sub DisplayCurrentRecord(frm As Object, rec As Recordset, lCnt As Long, bNew As Integer)
  Dim i As Integer
  Dim sCurrStat As String
  Dim lCurrRec As Long
  Dim bNoInd As Integer

  On Error GoTo DCRErr

  Screen.MousePointer = vbHourglass

  sCurrStat = "Row "
   
  'check to see if a table w/ 0 indexes is in use
  If rec.Type = dbOpenTable Then
    If gdbCurrentDB(rec.Name).Indexes.Count = 0 Then
      bNoInd = True
    End If
  End If
   
  'check for an empty recordset
  If rec.RecordCount > 0 Then
    lCurrRec = (lCnt * (rec.PercentPosition * 0.01)) + 1
  End If
     
  'check BOF/EOF flag so we know if we
  'are sitting on a valid record
  If bNew Then
    If bNoInd Then
      sCurrStat = lCnt & " Rows"
    Else
      sCurrStat = lCurrRec & "/" & lCnt
    End If
  Else
    If rec.BOF Then
      sCurrStat = "(BOF)/" & lCnt
      ClearDataFields frm, rec.Fields.Count
    ElseIf rec.EOF Then
      sCurrStat = "(EOF)/" & lCnt
      ClearDataFields frm, rec.Fields.Count
    Else
      If bNoInd Then
        sCurrStat = lCnt & " Rows"
      Else
        sCurrStat = lCurrRec & "/" & lCnt
      End If
      'place the data in the form fields
      For i = 0 To rec.Fields.Count - 1
        If rec(i).Type = dbMemo Then
          If rec(i).FieldSize() < gnGETCHUNK_CUTOFF Then
            frm.txtFieldData(i).Text = StripNonAscii(vFieldVal(rec(i)))
          Else
            frm.txtFieldData(i).Text = StripNonAscii(vFieldVal(rec(i).GetChunk(0, gnGETCHUNK_CUTOFF)))
          End If
        ElseIf rec(i).Type = dbText Then
          frm.txtFieldData(i).Text = vFieldVal(rec(i))
        Else
          frm.txtFieldData(i).Text = vFieldVal(rec(i))
        End If
      Next
    End If
  End If
  If rec.Updatable = False Then sCurrStat = sCurrStat & MSG26
  frm.lblStatus.Caption = sCurrStat
  Screen.MousePointer = vbDefault
  Exit Sub

DCRErr:
  ShowError
  Resume Next    'so we can try and display as much data as possible
End Sub

'------------------------------------------------------------
'this function checks to see if the passed in name exists
'in either the Tabledefs or Querydefs collection
'it found, it prompts to delete it and returns false
'if the user selects to delete it or true if not
'if not found, it returns false
'------------------------------------------------------------
Function DupeTableName(rName As String) As Integer
  On Error GoTo DTNErr

  Dim tdf As TableDef
  Dim qdf As QueryDef
  Dim i As Integer

  For Each tdf In gdbCurrentDB.TableDefs
    If UCase(tdf.Name) = UCase(rName) Then
      If MsgBox(MSG27, vbYesNo + vbQuestion) = vbYes Then
        gdbCurrentDB.TableDefs.Delete rName
        DupeTableName = False
      Else
        DupeTableName = True
      End If
      Exit Function
    End If
  Next

  If gsDataType = gsMSACCESS Then
    For Each qdf In gdbCurrentDB.QueryDefs
      If UCase(qdf.Name) = UCase(rName) Then
        If MsgBox(MSG28, vbYesNo + vbQuestion) = vbYes Then
          gdbCurrentDB.QueryDefs.Delete rName
          DupeTableName = False
        Else
          DupeTableName = True
        End If
        Exit Function
      End If
    Next
  End If

  DupeTableName = False
  Exit Function

DTNErr:
  ShowError
  DupeTableName = False
End Function

'------------------------------------------------------------
'this sub unloads all forms except for the
'SQL, Tables and MDI form
'------------------------------------------------------------
Sub UnloadAllForms()
  On Error Resume Next
  
  Dim i As Integer
  
  'close all forms except for the Tables and SQL forms
  For i = Forms.Count - 1 To 1 Step -1
    Unload Forms(i)
  Next
End Sub

'------------------------------------------------------------
'this sub walks the parameters collection in a parameterized
'query and prompts the user for a value for each parameter
'------------------------------------------------------------
Sub SetQDFParams(rqdf As QueryDef)
  On Error GoTo SPErr
  
  Dim prm As Parameter
  Dim sTmp As String
  
  For Each prm In rqdf.Parameters
    'get the value from the user
    sTmp = InputBox(MSG29, "'" & prm.Name & "':")
    'store the value
    prm.Value = CVar(sTmp)
  Next
  
  Exit Sub
    
SPErr:
  ShowError
End Sub

'------------------------------------------------------------
'this sub refreshs the Error form with the latest Errors
'------------------------------------------------------------
Sub RefreshErrors()
  On Error GoTo RErr
  
  Dim errObj As Error
  Dim i As Integer

  If DBEngine.Errors.Count = 0 Then
    MsgBox MSG30, 48
    Unload frmErrors
    Exit Sub
  End If

  frmErrors.Show
  frmErrors.lstErrors.Clear
  For i = 0 To DBEngine.Errors.Count - 1
    Set errObj = DBEngine.Errors(i)
    frmErrors.lstErrors.AddItem errObj.Number & vbTab & errObj.Source & vbTab & errObj.Description
  Next
  frmErrors.SetFocus

  Exit Sub
  
RErr:
  MsgBox MSG31, 48
  Unload frmErrors
  Exit Sub
End Sub

'------------------------------------------------------------
'this sub adds the just opened database to the most recently
'used list in the File menu
'------------------------------------------------------------
Sub AddMRU()
  On Error GoTo AMErr

  Dim i As Integer, j As Integer

  '1st look to see if it alread exists and swap it if it does
  For i = 1 To 8
    If UCase(Mid(frmMDI.mnuDBMRU(i).Caption, 4, Len(frmMDI.mnuDBMRU(i).Caption))) = UCase(gsDBName) Then
      For j = i To 2 Step -1
        frmMDI.mnuDBMRU(j).Caption = "&" & j & " " & Mid(frmMDI.mnuDBMRU(j - 1).Caption, 4, Len(frmMDI.mnuDBMRU(j - 1).Caption))
        frmMDI.mnuDBMRU(j).Tag = frmMDI.mnuDBMRU(j - 1).Tag
      Next
      GoTo Finish
    End If
  Next

  'wasn't there so move everything down one
  For i = 7 To 1 Step -1
    frmMDI.mnuDBMRU(i + 1).Caption = "&" & i + 1 & " " & Mid(frmMDI.mnuDBMRU(i).Caption, 4, Len(frmMDI.mnuDBMRU(i).Caption))
    frmMDI.mnuDBMRU(i + 1).Tag = frmMDI.mnuDBMRU(i).Tag
  Next

Finish:
  frmMDI.mnuDBMRU(1).Caption = "&1 " & gsDBName
  If Len(gdbCurrentDB.Connect) = 0 Then
    'handle the Access case where there is no connect string
    frmMDI.mnuDBMRU(1).Tag = gsMSACCESS
  Else
    frmMDI.mnuDBMRU(1).Tag = gdbCurrentDB.Connect
  End If
  frmMDI.mnuBarMRU.Visible = True
  For i = 1 To 8
    If Len(Mid(frmMDI.mnuDBMRU(i).Caption, 4, Len(frmMDI.mnuDBMRU(i).Caption))) > 0 Then
      frmMDI.mnuDBMRU(i).Visible = True
    End If
  Next

  Exit Sub

AMErr:
  ShowError
End Sub

'------------------------------------------------------------
'this sub breaks out the parts of a ODBC connect string
'and assigns them to the Public ODBC variables
'------------------------------------------------------------
Sub GetODBCConnectParts(rsConnect As String)
  On Error Resume Next
  
  Dim i As Integer
  Dim sTmp As String
  
  'process the connect string just in case the
  'values came from the ODBC dialogs
  If InStr(rsConnect, "=") Then
    i = 1
    While i <= Len(rsConnect) + 1
      If Mid(rsConnect, i, 1) = ";" Or i = Len(rsConnect) + 1 Then
        If Len(sTmp) > 0 And InStr(sTmp, "=") > 0 Then
          Select Case Mid(sTmp, 1, InStr(1, sTmp, "=") - 1)
            Case "DSN"
              gsODBCDatasource = Mid(sTmp, InStr(1, sTmp, "=") + 1, Len(sTmp))
            Case "DATABASE"
              gsODBCDatabase = Mid(sTmp, InStr(1, sTmp, "=") + 1, Len(sTmp))
            Case "DBQ"
              gsODBCDatabase = Mid(sTmp, InStr(1, sTmp, "=") + 1, Len(sTmp))
            Case "UID"
              gsODBCUserName = Mid(sTmp, InStr(1, sTmp, "=") + 1, Len(sTmp))
            Case "PWD"
              gsODBCPassword = Mid(sTmp, InStr(1, sTmp, "=") + 1, Len(sTmp))
            Case "Driver"
              gsODBCDriver = Mid(sTmp, InStr(1, sTmp, "=") + 1, Len(sTmp))
            Case "Server"
              gsODBCServer = Mid(sTmp, InStr(1, sTmp, "=") + 1, Len(sTmp))
             Case Else
              'nothing
          End Select
        End If
        sTmp = vbNullString
      Else
        sTmp = sTmp + Mid(rsConnect, i, 1)
      End If
      i = i + 1
    Wend
  End If
End Sub

'------------------------------------------------------------
'this is a generic sub that adds the name of each item
'in a collection to the passed in control
'------------------------------------------------------------
Sub ListItemNames(rcCollection As Object, rnCtl As Control, bClearList As Integer)
  On Error GoTo LINErr
  
  Dim objTmp As Object
  Dim i As Integer
  
  If bClearList Then
    rnCtl.Clear
  End If
  
  For Each objTmp In rcCollection
    rnCtl.AddItem objTmp.Name
  Next

  Exit Sub
  
LINErr:
  ShowError
End Sub

'------------------------------------------------------------
'this sub closes the current DB and performs any cleanup
'and resetting of controls, menus, etc.
'------------------------------------------------------------
Sub CloseCurrentDB()
  On Error GoTo DBCloseErr

  If gdbCurrentDB Is Nothing Then Exit Sub
    
  If gbDBChanged Then
    If MsgBox(MSG32, vbYesNo + vbQuestion) = vbYes Then
      gwsMainWS.CommitTrans
      gbDBChanged = False
    Else
      If MsgBox(MSG33, vbYesNo + vbQuestion) = vbYes Then
        gwsMainWS.Rollback
        gbDBChanged = False
      Else
        Beep
        MsgBox MSG34, 48
        Exit Sub
      End If
    End If
  End If

  frmMDI.Caption = "VisData"
  
  HideDBTools

  gbDBOpenFlag = False
  gbTransPending = False
  gsDBName = vbNullString
  gnReadOnly = False
  
  gdbCurrentDB.Close
  Set gdbCurrentDB = Nothing
  UnloadAllForms

  Exit Sub

DBCloseErr:
  ShowError
End Sub

'------------------------------------------------------------
'------------------------------------------------------------
Sub OpenLocalDB(bSilent As Boolean)
  On Error GoTo OpenError

  Dim sConnect As String
  Dim sDatabaseName As String
  Dim dbTemp As Database
  Dim sTmp As String

  sDatabaseName = gsDBName
  
  If Not bSilent Then
    Select Case gsDataType
      Case gsMSACCESS
        frmMDI.dlgCMD1.Filter = MSG49 & MSG50
        frmMDI.dlgCMD1.DialogTitle = MSG36
      Case gsDBASEIII, gsDBASEIV, gsDBASE5
        frmMDI.dlgCMD1.Filter = "Dbase DBs (*.dbf)|*.dbf" & MSG50
        frmMDI.dlgCMD1.DialogTitle = MSG37
      Case gsFOXPRO20, gsFOXPRO25, gsFOXPRO26, gsFOXPRO30
        frmMDI.dlgCMD1.Filter = "FoxPro DBs (*.dbf)|*.dbf" & MSG50
        frmMDI.dlgCMD1.DialogTitle = MSG38
      Case gsPARADOX3X, gsPARADOX4X, gsPARADOX5X
        frmMDI.dlgCMD1.Filter = "Paradox DBs (*.db)|*.db" & MSG50
        frmMDI.dlgCMD1.DialogTitle = MSG39
      Case gsEXCEL50
        frmMDI.dlgCMD1.Filter = "Excel Files (*.xls)|*.xls" & MSG50
        frmMDI.dlgCMD1.DialogTitle = MSG40
      Case gsBTRIEVE
        frmMDI.dlgCMD1.Filter = "Btrieve DBs (FILE.DDF)|FILE.DDF" & MSG50
        frmMDI.dlgCMD1.DialogTitle = MSG41
      Case gsTEXTFILES
        frmMDI.dlgCMD1.Filter = "Text Files (*.txt)|*.txt" & MSG50
        frmMDI.dlgCMD1.DialogTitle = MSG42
    End Select

    frmMDI.dlgCMD1.FilterIndex = 1
    frmMDI.dlgCMD1.FileName = gsDBName  '""
    frmMDI.dlgCMD1.CancelError = True
    frmMDI.dlgCMD1.Flags = FileOpenConstants.cdlOFNFileMustExist
    frmMDI.dlgCMD1.ShowOpen

    If Len(frmMDI.dlgCMD1.FileName) > 0 Then
      gsDBName = frmMDI.dlgCMD1.FileName
    Else
      Exit Sub
    End If
  Else
    gsDBName = sDatabaseName
  End If
  
  If Len(gsDBName) = 0 Then
    MsgBar vbNullString, False
    Exit Sub
  End If

  MsgBar MSG43, True
  Screen.MousePointer = vbHourglass

  'set the connect string
  If gsDataType = gsMSACCESS Then
    sConnect = vbNullString
  Else
    sConnect = gsDataType
  End If
  
  'set the database name for non Microsoft Access and Btrieve dbs that
  'came from the Common Dialog
  If gsDataType <> gsMSACCESS And gsDataType <> gsBTRIEVE And _
     gsDataType <> gsEXCEL50 And (Not bSilent) Then
    'need to strip off filename for these dbs
    sDatabaseName = StripFileName(gsDBName)
    gsDBName = sDatabaseName
  Else
    sDatabaseName = gsDBName
  End If

  gsODBCDatasource = vbNullString  'reset it

  GoTo OneMoreTry
  
GetPWD:
  Dim frmPWD As New frmDBPWD
  frmPWD.Show vbModal
  If Len(frmPWD.PWD) > 0 Then
    sConnect = ";pwd=" & frmPWD.PWD
    Unload frmPWD
    Set frmPWD = Nothing
    MsgBar MSG43, True
    Screen.MousePointer = vbHourglass
  Else
    'they cancelled the pwd dialog so we need to exit
    Unload frmPWD
    Set frmPWD = Nothing
    Exit Sub
  End If

OneMoreTry:
  If (frmMDI.dlgCMD1.Flags And FileOpenConstants.cdlOFNReadOnly) = FileOpenConstants.cdlOFNReadOnly Then
    gnReadOnly = True
  Else
    gnReadOnly = False
  End If
  Set dbTemp = gwsMainWS.OpenDatabase(sDatabaseName, False, gnReadOnly, sConnect)
  If gbDBOpenFlag Then
    'save the db name
    sTmp = gsDBName
    'restore it
    CloseCurrentDB
    gsDBName = sTmp
    If gbDBOpenFlag Then
      Beep
      MsgBox MSG35, 48
      Exit Sub
    End If
  End If

  'success
  frmMDI.Caption = "VisData:" & sDatabaseName
  Set gdbCurrentDB = dbTemp
  gbDBOpenFlag = True
  ShowDBTools
  RefreshTables Nothing
  gdbCurrentDB.QueryTimeout = glQueryTimeout

  AddMRU
  If gsDataType <> gsMSACCESS Then
    MsgBar MSG44, False
  End If
  Screen.MousePointer = vbDefault

  Exit Sub

AttemptRepair:
  Screen.MousePointer = vbHourglass
  MsgBar MSG45 & gsDBName, True
  DBEngine.RepairDatabase gsDBName
  Screen.MousePointer = vbDefault
  GoTo OneMoreTry

OpenError:
  Screen.MousePointer = vbDefault
  If Err = 3049 Then
    If MsgBox(Err.Description & vbCrLf & vbCrLf & MSG46, 4 + 48) = vbYes Then
      Resume AttemptRepair
    End If
  ElseIf Err = 3031 Then
    'password protected database
    Resume GetPWD
  End If
  gbDBOpenFlag = False
  gsDBName = vbNullString
  gsDataType = vbNullString
  gsODBCDatabase = vbNullString
  gsODBCUserName = vbNullString
  gsODBCPassword = vbNullString
  gsODBCDriver = vbNullString
  gsODBCServer = vbNullString
  If Err <> 32755 And Err <> 3049 Then   'check for common dialog cancelled
    ShowError
  End If
End Sub

'------------------------------------------------------------
'this sub is used to create a new directory for one
'of the local ISAM data types
'------------------------------------------------------------
Sub NewLocalISAM()
   On Error GoTo NewISAMErr

   Dim sNewName As String
   Dim d As Database

GetNewDirName:
   sNewName = InputBox(MSG47, , sNewName)
   If Len(sNewName) = 0 Then Exit Sub

   If Mid(sNewName, Len(sNewName), 1) <> "\" Then sNewName = sNewName & "\"

   MkDir Mid(sNewName, 1, Len(sNewName) - 1)

   gsDBName = sNewName
   OpenLocalDB True

   If gbDBOpenFlag Then
     ShowDBTools
     RefreshTables Nothing
   End If

  Exit Sub

NewISAMErr:
  If Err = 75 Then Resume Next  'catch the case where dir exists
  If Err = 76 Then
    MsgBox MSG65, vbExclamation
    'now try again
    Resume GetNewDirName
  End If
  ShowError
End Sub

'------------------------------------------------------------
'this sub is called from the compact menu options
'on the main MDI form
'------------------------------------------------------------
Sub CompactDB(rnCompactVersion As Integer)
  On Error GoTo CompactAccErr

  Dim sOldName As String
  Dim sNewName As String
  Dim sNewName2 As String
  Dim nEncrypt As Integer

  'get file name to compact
  frmMDI.dlgCMD1.Filter = MSG49
  frmMDI.dlgCMD1.DialogTitle = MSG48
  frmMDI.dlgCMD1.FilterIndex = 1
  frmMDI.dlgCMD1.Flags = FileOpenConstants.cdlOFNHideReadOnly
  frmMDI.dlgCMD1.ShowOpen
  If Len(frmMDI.dlgCMD1.FileName) > 0 Then
    sOldName = frmMDI.dlgCMD1.FileName
  Else
    Exit Sub
  End If

  'get file name to compact to
  frmMDI.dlgCMD1.DialogTitle = MSG51
  frmMDI.dlgCMD1.FilterIndex = 1
  frmMDI.dlgCMD1.FileName = vbNullString
  frmMDI.dlgCMD1.CancelError = True
  frmMDI.dlgCMD1.Flags = FileOpenConstants.cdlOFNOverwritePrompt + FileOpenConstants.cdlOFNHideReadOnly
  frmMDI.dlgCMD1.ShowSave
  If Len(frmMDI.dlgCMD1.FileName) > 0 Then
    sNewName = frmMDI.dlgCMD1.FileName
    If Dir(sNewName) <> vbNullString And sOldName <> sNewName Then
      Kill sNewName
    End If
  Else
    Exit Sub
  End If

  If MsgBox(MSG52, vbYesNo + vbQuestion) = vbYes Then
    nEncrypt = dbEncrypt
  Else
    nEncrypt = dbDecrypt
  End If

  Screen.MousePointer = vbHourglass
  MsgBar MSG53 & sOldName & " -> " & sNewName, True
  'if they want to overwrite the same file, we need to create a new MDB
  'and rename after the compact is successful
  If sOldName = sNewName Then
    sNewName2 = sNewName 'save the new name
    sNewName = Left(sNewName, Len(sNewName) - 1) & "N"
  End If
  
  DBEngine.CompactDatabase sOldName, sNewName, dbLangGeneral, rnCompactVersion + nEncrypt
  
  'check for an overwrite of the original mdb
  If VBA.Right(sNewName, 1) = "N" Then
    Kill sNewName2             'nuke the old one
    Name sNewName As sNewName2 'rename the new one to the original name
    sNewName = sNewName2       'reset to the correct name
  End If
  
  MsgBar vbNullString, False
  Screen.MousePointer = vbDefault

  If MsgBox(MSG54, vbYesNo + vbQuestion) = vbYes Then
    If gbDBOpenFlag Then
      CloseCurrentDB
    End If
    gsDataType = gsMSACCESS
    gsDBName = sNewName
    OpenLocalDB True
  End If

  If gbDBOpenFlag Then
    ShowDBTools
    RefreshTables Nothing
  End If

  Exit Sub

CompactAccErr:
  If Err <> 32755 Then         'user cancelled
    ShowError
  End If
End Sub

'------------------------------------------------------------
'this sub does some cleanup and shuts down VisData
'------------------------------------------------------------
Sub ShutDownVisData()
  On Error Resume Next

  Dim nRet As Integer

  'save all the current Registry settings
  SaveRegistrySettings

  If gbDBChanged Then
    If MsgBox(MSG32, vbYesNo + vbQuestion) = vbYes Then
      gwsMainWS.CommitTrans
    End If
  End If

  UnloadAllForms
  gdbCurrentDB.Close
  'close the help file
  ReleaseHelp
  
  End

End Sub
Sub NewMDB(rnVersion As Integer)
  On Error GoTo NewAccErr

  Dim sNewName As String
  Dim db As Database

  'get file name to compact to
  frmMDI.dlgCMD1.DialogTitle = MSG55
  frmMDI.dlgCMD1.FilterIndex = 1
  frmMDI.dlgCMD1.Filter = MSG49
  frmMDI.dlgCMD1.FileName = vbNullString
  frmMDI.dlgCMD1.CancelError = True
  frmMDI.dlgCMD1.Flags = FileOpenConstants.cdlOFNOverwritePrompt + FileOpenConstants.cdlOFNHideReadOnly
  frmMDI.dlgCMD1.ShowSave
  If Len(frmMDI.dlgCMD1.FileName) > 0 Then
    sNewName = frmMDI.dlgCMD1.FileName
    If InStr(sNewName, ".") = 0 Then
      'add an extension if the user didn't supply one
      sNewName = sNewName & ".MDB"
    End If
    If Dir(sNewName) <> vbNullString Then
      Kill sNewName
    End If
  Else
    Exit Sub
  End If
  If Len(sNewName) = 0 Then Exit Sub

  Set db = CreateDatabase(sNewName, dbLangGeneral, rnVersion)
  db.Close

  gsDataType = gsMSACCESS
  gsDBName = sNewName
  OpenLocalDB True
  Exit Sub

NewAccErr:
  If Err <> 32755 Then         'user cancelled
    ShowError
  End If
End Sub

Sub Export(rsFromTbl As String, rsToDB As String)

  On Error GoTo ExpErr

  Dim sConnect As String
  Dim sNewTblName As String
  Dim sDBName As String
  Dim nErrState As Integer
  Dim idxFrom As Index
  Dim idxTo As Index
  Dim sSQL As String              'local copy of sql string
  Dim sField As String
  Dim sFrom As String
  Dim sTmp As String
  Dim i As Integer

  If gnDataType = gnDT_SQLDB Then
    Set gExpDB = gwsMainWS.OpenDatabase(vbNullString, 0, 0, "odbc;")
    If gExpDB Is Nothing Then Exit Sub
  End If

  MsgBar MSG56 & "'" & rsFromTbl & "'", True

  nErrState = 1
  Select Case gnDataType
    Case gnDT_MSACCESS
      sConnect = "[;database=" & rsToDB & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(rsToDB)
    Case gnDT_PARADOX3X
      sDBName = StripFileName(rsToDB)
      sConnect = "[Paradox 3.X;database=" & StripFileName(rsToDB) & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(sDBName, 0, 0, gsPARADOX3X)
    Case gnDT_PARADOX4X
      sDBName = StripFileName(rsToDB)
      sConnect = "[Paradox 4.X;database=" & StripFileName(rsToDB) & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(sDBName, 0, 0, gsPARADOX4X)
    Case gnDT_FOXPRO26
      sDBName = StripFileName(rsToDB)
      sConnect = "[FoxPro 2.6;database=" & StripFileName(rsToDB) & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(sDBName, 0, 0, gsFOXPRO26)
    Case gnDT_FOXPRO25
      sDBName = StripFileName(rsToDB)
      sConnect = "[FoxPro 2.5;database=" & StripFileName(rsToDB) & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(sDBName, 0, 0, gsFOXPRO25)
    Case gnDT_FOXPRO20
      sDBName = StripFileName(rsToDB)
      sConnect = "[FoxPro 2.0;database=" & StripFileName(rsToDB) & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(sDBName, 0, 0, gsFOXPRO20)
    Case gnDT_DBASEIV
      sDBName = StripFileName(rsToDB)
      sConnect = "[dBase IV;database=" & StripFileName(rsToDB) & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(sDBName, 0, 0, gsDBASEIV)
    Case gnDT_DBASEIII
      sDBName = StripFileName(rsToDB)
      sConnect = "[dBase III;database=" & StripFileName(rsToDB) & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(sDBName, 0, 0, gsDBASEIII)
    Case gnDT_BTRIEVE
      sConnect = "[Btrieve;database=" & rsToDB & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(rsToDB, 0, 0, gsBTRIEVE)
    Case gnDT_EXCEL50, gnDT_EXCEL40, gnDT_EXCEL30
      sConnect = "[Excel 5.0;database=" & rsToDB & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(rsToDB, 0, 0, gsEXCEL50)
    Case gnDT_SQLDB
      sConnect = "[" & gExpDB.Connect & "]."
    Case gnDT_TEXTFILE
      sDBName = StripFileName(rsToDB)
      sConnect = "[Text;database=" & StripFileName(rsToDB) & "]."
      Set gExpDB = gwsMainWS.OpenDatabase(sDBName, 0, 0, gsTEXTFILES)
  End Select
  If gnDataType = gnDT_MSACCESS Or gnDataType = gnDT_BTRIEVE Or _
     gnDataType = gnDT_SQLDB Or gnDataType = gnDT_EXCEL50 Or _
     gnDataType = gnDT_EXCEL40 Or gnDataType = gnDT_EXCEL30 Then
    With frmExpName
      .Label1.Caption = MSG57 & rsFromTbl & " ->"
      .Label2.Caption = MSG58 & rsToDB
      .txtTable.Text = rsFromTbl
    End With
    frmExpName.Show vbModal
      
    If Len(gExpTable) = 0 Then
      MsgBar vbNullString, False
      Exit Sub
    Else
      sNewTblName = gExpTable
    End If
  Else
    'get the table part of the file name
    'strip off the path
    For i = Len(rsToDB) To 1 Step -1
      If Mid(rsToDB, i, 1) = "\" Then
        Exit For
      End If
    Next
    sTmp = Mid(rsToDB, i + 1, Len(rsToDB))
    'strip off the extension
    For i = 1 To Len(sTmp)
      If Mid(sTmp, i, 1) = "." Then
        Exit For
      End If
    Next
    sNewTblName = Left(sTmp, i - 1)
  End If
  Screen.MousePointer = vbHourglass
  If Len(rsFromTbl) > 0 Then
    gdbCurrentDB.Execute "select * into " & sConnect & StripOwner(sNewTblName) & " from " & StripOwner(rsFromTbl)

    If gnDataType <> gnDT_TEXTFILE Then
      nErrState = 2
      MsgBar MSG59 & " '" & sNewTblName & "'", True
      gExpDB.TableDefs.Refresh
      For Each idxFrom In gdbCurrentDB.TableDefs(rsFromTbl).Indexes
        Set idxTo = gExpDB.TableDefs(sNewTblName).CreateIndex(idxFrom.Name)
        With idxTo
          .Fields = idxFrom.Fields
          .Unique = idxFrom.Unique
          If gnDataType <> gnDT_SQLDB And gsDataType <> "ODBC" Then
            .Primary = idxFrom.Primary
          End If
        End With
        gExpDB.TableDefs(sNewTblName).Indexes.Append idxTo
      Next
    End If
    MsgBar vbNullString, False
    Screen.MousePointer = vbDefault
    MsgBox MSG60 & " '" & rsFromTbl & "'", 64
  Else
    sSQL = frmSQL.txtSQLStatement.Text
    sField = Mid(sSQL, 8, InStr(8, UCase(sSQL), "FROM") - 9)
    sFrom = " " & Mid(sSQL, InStr(UCase(sSQL), "FROM"), Len(sSQL))
    gdbCurrentDB.Execute "select " & sField & " into " & sConnect & sNewTblName & sFrom

    Screen.MousePointer = vbDefault
    MsgBar vbNullString, False
    MsgBox MSG61, 64
  End If

  Exit Sub

ExpErr:
  If Err = 3010 Then      'table exists
    If MsgBox(MSG62, 32 + 1 + 256) = 1 Then
      gExpDB.TableDefs.Delete sNewTblName
      Resume
    Else
      Screen.MousePointer = vbDefault
      MsgBar vbNullString, False
      Exit Sub
    End If
  End If
 
  'nuke the new table if the indexes couldn't be created
  If nErrState = 2 Then
    gExpDB.TableDefs.Delete sNewTblName
  End If
  ShowError
End Sub

Sub Import(rsImpTblName As String)
  On Error GoTo ImpErr

  Dim sOldTblName As String, sNewTblName As String, sConnect As String
  Dim idxFrom As Index
  Dim idxTo As Index
  Dim nErrState As Integer
  Dim i As Integer

  sOldTblName = MakeTableName(rsImpTblName, False)
  sNewTblName = MakeTableName(rsImpTblName, True)

  Screen.MousePointer = vbHourglass
  MsgBar MSG63 & "'" & sNewTblName & "'", True

  nErrState = 1
  Select Case gnDataType
    Case gnDT_MSACCESS
      sConnect = "[;database=" & gImpDB.Name & "]."
    Case gnDT_PARADOX3X
      sConnect = "[Paradox 3.X;database=" & StripFileName(rsImpTblName) & "]."
      Set gImpDB = gwsMainWS.OpenDatabase(StripFileName(rsImpTblName), 0, 0, gsPARADOX3X)
    Case gnDT_PARADOX4X
      sConnect = "[Paradox 4.X;database=" & StripFileName(rsImpTblName) & "]."
      Set gImpDB = gwsMainWS.OpenDatabase(StripFileName(rsImpTblName), 0, 0, gsPARADOX4X)
    Case gnDT_FOXPRO26
      sConnect = "[FoxPro 2.6;database=" & StripFileName(rsImpTblName) & "]."
      Set gImpDB = gwsMainWS.OpenDatabase(StripFileName(rsImpTblName), 0, 0, gsFOXPRO26)
    Case gnDT_FOXPRO25
      sConnect = "[FoxPro 2.5;database=" & StripFileName(rsImpTblName) & "]."
      Set gImpDB = gwsMainWS.OpenDatabase(StripFileName(rsImpTblName), 0, 0, gsFOXPRO25)
    Case gnDT_FOXPRO20
      sConnect = "[FoxPro 2.0;database=" & StripFileName(rsImpTblName) & "]."
      Set gImpDB = gwsMainWS.OpenDatabase(StripFileName(rsImpTblName), 0, 0, gsFOXPRO20)
    Case gnDT_DBASEIV
      sConnect = "[dBase IV;database=" & StripFileName(rsImpTblName) & "]."
      Set gImpDB = gwsMainWS.OpenDatabase(StripFileName(rsImpTblName), 0, 0, gsDBASEIV)
    Case gnDT_DBASEIII
      sConnect = "[dBase III;database=" & StripFileName(rsImpTblName) & "]."
      Set gImpDB = gwsMainWS.OpenDatabase(StripFileName(rsImpTblName), 0, 0, gsDBASEIII)
    Case gnDT_BTRIEVE
      sConnect = "[Btrieve;database=" & gImpDB.Name & "]."
    Case gnDT_EXCEL50, gnDT_EXCEL40, gnDT_EXCEL30
      sConnect = "[Excel 5.0;database=" & gImpDB.Name & "]."
    Case gnDT_SQLDB
      sConnect = "[" & gImpDB.Connect & "]."
    Case gnDT_TEXTFILE
      sConnect = "[Text;database=" & StripFileName(rsImpTblName) & "]."
      Set gImpDB = gwsMainWS.OpenDatabase(StripFileName(rsImpTblName), 0, 0, gsTEXTFILES)
  End Select
  gdbCurrentDB.Execute "select * into " & sNewTblName & " from " & sConnect & sOldTblName

  If gnDataType <> gnDT_TEXTFILE And gnDataType <> gnDT_EXCEL50 And _
     gnDataType <> gnDT_EXCEL40 And gnDataType <> gnDT_EXCEL30 Then
    nErrState = 2
    MsgBar gdbCurrentDB.RecordsAffected & " Rows Imported, Creating Indexes for '" & sNewTblName & "'", True
    gdbCurrentDB.TableDefs.Refresh
    For Each idxFrom In gImpDB.TableDefs(sOldTblName).Indexes
      Set idxTo = gdbCurrentDB.TableDefs(sNewTblName).CreateIndex(idxFrom.Name)
      With idxTo
        .Fields = idxFrom.Fields
        .Unique = idxFrom.Unique
        If gnDataType <> gnDT_SQLDB And gsDataType <> gsSQLDB Then
          .Primary = idxFrom.Primary
        End If
      End With
      gdbCurrentDB.TableDefs(sNewTblName).Indexes.Append idxTo
    Next
  End If
    
  frmImpExp.lstTables.AddItem sNewTblName
'  frmTables.lstTables.AddItem sNewTblName
  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False
  MsgBox MSG64 & "'" & sNewTblName & "'.", 64

  Exit Sub

NukeNewTbl:
  On Error Resume Next  'just in case it fails
  gdbCurrentDB.TableDefs.Delete sNewTblName
  ShowError
  Exit Sub
 
ImpErr:
  'nuke the new table if the indexes couldn't be created
  If nErrState = 2 Then
    Resume NukeNewTbl
  End If
  ShowError
End Sub

Function MakeTableName(fname As String, newname As Integer) As String
  On Error Resume Next
  Dim i As Integer, t As Integer
  Dim tmp As String

  If gnDataType = gnDT_SQLDB And newname Then
    i = InStr(1, fname, ".")
    If i > 0 Then
      tmp = Mid(fname, 1, i - 1) & "_" & Mid(fname, i + 1, Len(fname))
    End If
  ElseIf InStr(fname, "\") > 0 Then
    'strip off path
    For i = Len(fname) To 1 Step -1
      If Mid(fname, i, 1) = "\" Then
        Exit For
      End If
    Next
    tmp = Mid(fname, i + 1, Len(fname))
    i = InStr(1, tmp, ".")
    If i > 0 Then
      tmp = Mid(tmp, 1, i - 1)
    End If
  Else
    tmp = fname
  End If

  If newname Then
    If DupeTableName(tmp) Then
      t = 1
      While DupeTableName(tmp + CStr(t))
        t = t + 1
      Wend
      tmp = tmp + CStr(t)
    End If
  End If

  MakeTableName = tmp

End Function


'------------------------------------------------------------
' Purpose:      set the locale ID for HTML help resources
' Param lcid:   Locale ID in which it is to be displayed
'------------------------------------------------------------
Public Sub setHelpLocaleID(ByVal LCID As Long)
'------------------------------------------------------------
    m_lcid = LCID
End Sub

'------------------------------------------------------------
' Purpose:      set the filename of the help topic
' Param sName:  filename of help topic
'------------------------------------------------------------
Public Sub setHelpFile(ByVal sName As String)
'------------------------------------------------------------
    m_sHelpFile = sName
End Sub

'------------------------------------------------------------
' Public Sub DisplayTopic(ByVal toc As Long)
' Purpose:      displays the html topic identified by toc. Assumes helpfile has already been set
' Param sHelpFile: chm file you are looking up
' Param toc:    topic id to be displayed
' Param lcid:   Locale ID in which it is to be displayed
'------------------------------------------------------------
Public Sub DisplayTopic(ByVal toc As Long)
'------------------------------------------------------------
On Error GoTo errorHandle

    Debug.Assert Len(m_sHelpFile) > 0
    
    If Not initialiseHelp() Then Exit Sub
        
    m_HelpServices.DisplayTopicFromIdentifier m_sHelpFile, toc, VHS_Localize
        
errorHandle:
    ' html help throws up its own error if it can't display help
        
End Sub

'------------------------------------------------------------
' Public Sub KeywordSearch(sKey as string, lcid as long)
' Purpose:      performs a keyword search on the entire (installed) msdn.
' Param sKey:   keyword to be searched for
' Param lcid:   Locale ID in which it is to be displayed
'------------------------------------------------------------
Public Sub KeywordSearch(ByVal sKey As String)
'------------------------------------------------------------
On Error GoTo errorHandle

    If Not initialiseHelp() Then Exit Sub
    
    m_HelpServices.KeywordSearch sKey, 0, 0
    
errorHandle:
    ' html help throws up its own error if it can't display help
    
End Sub

'------------------------------------------------------------
' Public Sub ReleaseHelp
' Purpose:      cleans up m_HelpServices
'------------------------------------------------------------
Public Sub ReleaseHelp()
'------------------------------------------------------------
    Set m_HelpServices = Nothing
End Sub

'------------------------------------------------------------
' initialse the HTML help system
'------------------------------------------------------------
Private Function initialiseHelp() As Boolean
On Error GoTo errorHandle
    Dim helpInit As IVsHelpInit
    
    If m_HelpServices Is Nothing Then
        Set m_HelpServices = New VsHelpServices.VsHelpServices
        Set helpInit = m_HelpServices
        helpInit.LoadUIResources m_lcid
        DoEvents
    End If
    
    initialiseHelp = True

errorHandle:
    If Err <> 0 Then
        initialiseHelp = False
        MsgBox Err.Description
    End If
End Function

Public Function GetVbIdeLocale() As Long
'------------------------------------------------------------
    Dim hInstance   As Long
    Dim sLocale     As String
    Dim rc          As Long
    Const LOCALERESID = 2001
'------------------------------------------------------------
    hInstance = LoadLibraryEx("VB6IDE.DLL", 0&, LOAD_LIBRARY_AS_DATAFILE)
    If (hInstance <> 0) Then
        sLocale = String$(20, vbNullChar)
        rc = LoadStringA(hInstance, LOCALERESID, sLocale, Len(sLocale))
        If (rc > 0) Then
            GetVbIdeLocale = Val("&H" & sLocale)
        End If
        FreeLibrary hInstance
    End If
'------------------------------------------------------------
End Function


