VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "ComDlg32.OCX"
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.Form frmTreeview 
   Caption         =   "TreeView and ListView"
   ClientHeight    =   5655
   ClientLeft      =   735
   ClientTop       =   2640
   ClientWidth     =   9660
   Icon            =   "treeview.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   5655
   ScaleWidth      =   9660
   Begin VB.ComboBox cmbView 
      Height          =   315
      Left            =   6960
      TabIndex        =   4
      Text            =   "Combo1"
      Top             =   0
      Width           =   2415
   End
   Begin VB.CommandButton cmdLoad 
      Caption         =   "Load"
      Height          =   270
      Left            =   2190
      TabIndex        =   1
      Top             =   4935
      Visible         =   0   'False
      Width           =   1050
   End
   Begin MSComCtlLib.StatusBar sbrDB 
      Align           =   2  'Align Bottom
      Height          =   255
      Left            =   0
      TabIndex        =   5
      Top             =   5400
      Width           =   9660
      _ExtentX        =   17039
      _ExtentY        =   450
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
         EndProperty
      EndProperty
   End
   Begin MSComCtlLib.ProgressBar prgLoad 
      Height          =   210
      Left            =   255
      TabIndex        =   3
      Top             =   375
      Visible         =   0   'False
      Width           =   9090
      _ExtentX        =   16034
      _ExtentY        =   370
      _Version        =   393216
      Appearance      =   1
   End
   Begin MSComCtlLib.ListView lvwDB 
      Height          =   4215
      Left            =   3480
      TabIndex        =   2
      Top             =   615
      Width           =   5880
      _ExtentX        =   10372
      _ExtentY        =   7435
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   393216
      Icons           =   "imlIcons"
      SmallIcons      =   "imlSmallIcons"
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      Appearance      =   1
      NumItems        =   0
   End
   Begin MSComDlg.CommonDialog dlgDialog 
      Left            =   1605
      Top             =   4830
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
      FilterIndex     =   474
      FontSize        =   8.01821e-38
   End
   Begin MSComCtlLib.TreeView tvwDB 
      Height          =   4215
      Left            =   255
      TabIndex        =   0
      Top             =   615
      Width           =   3120
      _ExtentX        =   5503
      _ExtentY        =   7435
      _Version        =   393216
      LineStyle       =   1
      Style           =   7
      ImageList       =   "imlSmallIcons"
      Appearance      =   1
   End
   Begin MSComCtlLib.ImageList imlIcons 
      Left            =   645
      Top             =   4800
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   32
      ImageHeight     =   32
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   1
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "treeview.frx":0442
            Key             =   "book"
         EndProperty
      EndProperty
   End
   Begin MSComCtlLib.ImageList imlSmallIcons 
      Left            =   45
      Top             =   4815
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   13
      ImageHeight     =   13
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   6
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "treeview.frx":075C
            Key             =   "closed"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "treeview.frx":08CE
            Key             =   "cylinder"
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "treeview.frx":0A40
            Key             =   "leaf"
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "treeview.frx":0BB2
            Key             =   "open"
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "treeview.frx":0D24
            Key             =   "smlBook"
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "treeview.frx":0FD6
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuLoad 
         Caption         =   "&Load"
      End
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
   End
End
Attribute VB_Name = "frmTreeview"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private mNode As node ' Module-level Node variable.
Private mItem As ListItem ' Module-level ListItem variable.
Private EventFlag As Integer ' To signal which event has occurred.
Private mCurrentIndex As Integer ' Flag to assure this node wasn't already clicked.
Private mStatusBarStyle As Integer ' Switches Statusbar style.
Private cn As ADODB.Connection ' We use only one active connection.
Attribute cn.VB_VarHelpID = -1

Const PUBLISHER = 1 ' For EventFlag, Signals Publisher colmunheader objects.
Const TITLE = 2 ' EventFlag, signals Title in ListView
Private Sub cmdLoad_Click()
    Dim intCounter As Integer ' Counter to set Progressbar.Value
    Dim intIndex ' Variable for index of current node.
    ' Set the ADODB Connection object's ConnectionString and open it.
    
    ' Create an ADODB Recordset object variable.
    Dim rsPublishers As New ADODB.Recordset
    ' Open the recordset.
    With rsPublishers
        .Open "SELECT PubID, [Company Name] FROM Publishers", cn, adOpenStatic, adLockOptimistic
        ' Move To last record to get a RecordCount, then move back.
        .MoveLast
        .MoveFirst
    End With
    ' Set ProgressBar Max, and make it visible.
    With prgLoad
        .Max = rsPublishers.RecordCount
        .Visible = True
    End With
    
    ' While the record is not the last record, add a ListItem object.
    ' Use the Name field for the ListItem object's text.
    Do While Not rsPublishers.EOF
        intCounter = intCounter + 1
        prgLoad.Value = intCounter ' Update ProgressBar.
        
        ' Add a Node to the TreeView, and set its properties.
        Set mNode = tvwDB.Nodes.Add(1, tvwChild, rsPublishers!pubID & " ID", CStr(rsPublishers![Company name]), "closed")
        mNode.Tag = "Publisher" ' Identifies the table.
        
        ' Set the variable intIndex to the Index property of the
        ' newly created Node. Use this variable to add child
        ' Node objects to the present Node.
        intIndex = mNode.Index
        
        rsPublishers.MoveNext   ' Move to next Publishers record.
    Loop
    ' Hide Progressbar
    prgLoad.Visible = False
    ' Set Statusbar style to normal.
    sbrDB.Style = sbrNormal
    ' Sort the Publishers nodes.
    tvwDB.Nodes(1).Sorted = True
    ' Expand top node.
    tvwDB.Nodes(1).Expanded = True
    
End Sub

Private Sub cmbView_Click()
    ' Set the ListView.View property.
    lvwDB.View = cmbView.ListIndex
End Sub

Private Function FindBiblio() As String
    On Error GoTo ErrHandler

    ' Configure cmdDialog in case the Biblio.mdb can't be found.
    With dlgDialog
        .DialogTitle = "Can't Find Biblio.mdb"
        .Filter = "(*.MDB)|*.mdb"
    End With

    'Causes an error if user clicks on cancel
    dlgDialog.CancelError = True
    dlgDialog.ShowOpen
        
    Do While UCase(Right(Trim(dlgDialog.FileName), 10)) <> "BIBLIO.MDB"
       MsgBox "File Name is not equal to BIBLIO.MDB"
       dlgDialog.ShowOpen
    Loop
    
    FindBiblio = dlgDialog.FileName
    Exit Function
ErrHandler:
    If Err = 32755 Then
      End
    End If
End Function


Private Sub Form_Load()
    
    ' Open the global Connection object first.
    On Error GoTo errFind
    Set cn = New ADODB.Connection
    ' The ConnectionString contains the path of the database. If the
    ' Biblio.mdb is not on your machine, you can find it on the MSDN
    ' CD
    With cn
        .ConnectionString = "Provider=Microsoft.Jet.OLEDB.3.51;Data Source=" & _
        "C:\Program Files\Microsoft Visual Studio\VB98\Biblio.mdb"
        .Open
    End With
    
    ' Configure cmbView control.
    With cmbView
        .AddItem "Icon View"         ' 0
        .AddItem "SmallIcon View"    ' 1
        .AddItem "List View"         ' 2
        .AddItem "Report View"       ' 3
        .ListIndex = 3
    End With

    ' Configure ListView control.
    lvwDB.View = lvwReport
        
    ' Configure TreeView
    With tvwDB
        .Sorted = True
        Set mNode = .Nodes.Add()
        .LabelEdit = False
        .LineStyle = tvwRootLines
    End With
    
    With mNode ' Add first node.
        .Text = "Publishers"
        .Tag = "Biblio"
        .Image = "closed"
    End With
    frmTreeview.Show
    
    mnuLoad_Click
    Exit Sub
 
    ' If the Biblio database can't be found, open the
    ' common dialog control to let the user find it.
errFind:
    
    If Err = -2147467259 Then
        Set cn = Nothing
        Set cn = New ADODB.Connection
        cn.ConnectionString = "Provider=Microsoft.Jet.OLEDB.3.51;Data Source=" & FindBiblio
        cn.Open
        Resume Next
    ElseIf Err <> 0 Then ' another error
        MsgBox "Unexpected Error: " & Err.Description
        End
    End If
End Sub

Private Sub lvwDB_ColumnClick(ByVal ColumnHeader As ColumnHeader)
    lvwDB.SortKey = ColumnHeader.Index - 1
    ' Set Sorted to True to sort the list.
    lvwDB.Sorted = True
End Sub

Private Sub lvwDB_ItemClick(ByVal Item As ListItem)
    GetData Item.Key
 End Sub
Private Sub GetData(ISBN As String)
    ' The global EventFlag indicates how the Statusbar is being used.
    
    If EventFlag <> TITLE Then
        sbrDB.Panels.Clear
        Dim pnlX As Panel
        Set pnlX = sbrDB.Panels.Add(, "ISBN")
        pnlX.AutoSize = sbrContents
        Set pnlX = sbrDB.Panels.Add(, "author")
        pnlX.AutoSize = sbrContents
        Set pnlX = sbrDB.Panels.Add(, "year")
        pnlX.Width = 1000
        Set pnlX = sbrDB.Panels.Add(, "description")
        pnlX.AutoSize = sbrContents
    End If
        
    ' Open an ADODB recordset to get data for Statusbar.
    Dim rsTitles As New ADODB.Recordset
    Dim strQ As String
    strQ = "SELECT Authors.Author, Titles.ISBN, Titles.[Year Published], " & _
    "Titles.Description FROM Authors INNER JOIN (Titles INNER JOIN " & _
    "[Title Author] ON " & _
    "Titles.ISBN = [Title Author].ISBN) ON Authors.Au_ID = " & _
    "[Title Author].Au_ID WHERE Titles.ISBN='" & ISBN & " '"

    ' Open recordset.
    rsTitles.Open strQ, cn, adOpenStatic, adLockOptimistic
    
    ' Populate StatusBar panels with info.
    sbrDB.Panels("author").Text = rsTitles!author
    sbrDB.Panels("ISBN").Text = rsTitles!ISBN
    If Not IsNull(rsTitles![Year Published]) Then
        sbrDB.Panels("year").Text = rsTitles![Year Published]
    Else
        sbrDB.Panels("year").Text = "n/a"
    End If
    If Not IsNull(rsTitles!Description) Then
        sbrDB.Panels("description").Text = rsTitles!Description
    Else
        sbrDB.Panels("description").Text = "n/a"
    End If
    If Not rsTitles.EOF Then rsTitles.MoveNext
    ' Add other author names.
    Do Until rsTitles.EOF
        
        If Not IsNull(rsTitles!author) Then
            sbrDB.Panels("author").Text = sbrDB.Panels("author").Text & _
            " & " & rsTitles!author
        End If
        rsTitles.MoveNext
    Loop
    ' Set EventFlag so Panels don't have to be recreated.
    EventFlag = TITLE
End Sub


Private Sub mnuExit_Click()
    Unload Me
End Sub

Private Sub mnuLoad_Click()
    Static Loaded As Boolean
    If Loaded = True Then
        Exit Sub
    Else
        cmdLoad_Click
        Loaded = Abs(Loaded - 1)
        mnuLoad.Enabled = False
    End If
End Sub

Private Sub tvwDB_Collapse(ByVal node As node)
    ' Only nodes that are folders can be collapsed.
    If node.Tag = "Publisher" Or node.Index = 1 Then node.Image = "closed"
End Sub

Private Sub tvwDB_Expand(ByVal node As node)
    ' Only the top node, and publisher nodes can be expanded.
    If node.Tag = "Publisher" Or node.Index = 1 Then
        node.Image = "open"
        node.Sorted = True
    End If
    If node.Tag = "Publisher" And EventFlag <> _
    PUBLISHER Then MakeColumns
    ' If the Tag is "Publisher" and the mItemCurrentIndex
    ' index isn't the same as the Node.key, then
    ' invoke the GetTitles function.
    If node.Tag = "Publisher" And mCurrentIndex <> Val(node.Key) _
    Then GetTitles node, Val(node.Key)
    
    If node.Tag = "Publisher" Then PopStatus node

    node.Sorted = True

End Sub

Private Sub MakeColumns()
    ' Clear the ColumnHeaders collection.
    lvwDB.ColumnHeaders.Clear
    ' Add four ColumnHeaders.
    lvwDB.ColumnHeaders.Add , , "Title", 2800
    lvwDB.ColumnHeaders.Add , , "Author"
    lvwDB.ColumnHeaders.Add , , "Year", 800
    lvwDB.ColumnHeaders.Add , , "ISBN"
    
    ' Set the EventFlag variable so this doesn't get done again and again.
    EventFlag = PUBLISHER
End Sub
Private Sub AddListItemsOnly(pubID)
    Dim rsTitles As New ADODB.Recordset
    Dim newNode As node
    Dim strQ As String
    strQ = "SELECT Titles.Title, Authors.Author, Titles.ISBN, " & _
    "Titles.[Year Published] FROM Authors INNER JOIN " & _
    "(Titles INNER JOIN [Title Author] " & _
    "ON Titles.ISBN = [Title Author].ISBN) ON Authors.Au_ID = " & _
    "[Title Author].Au_ID WHERE Titles.PubID=" & pubID
    
    lvwDB.ListItems.Clear
    With rsTitles
        .Open strQ, cn, adOpenStatic, adLockReadOnly, adCmdText
        .MoveLast
        .MoveFirst
        prgLoad.Max = .RecordCount + 1
    End With
    
    ' Show Progress bar
    prgLoad.Visible = True

    Dim intCounter As Integer
    ' Create a child node.
    

    ' Add a corresponding ListItem.
    AddListItem mItem, rsTitles
    
    rsTitles.MoveNext
    ' Go through the rest of the recordset. If the next record
    ' is a duplicate, then just add the author's name.
    ' Otherwise, add a new Node and ListItem.
    Do Until rsTitles.EOF
        intCounter = intCounter + 1 ' For the ProgressBar.
        prgLoad.Value = intCounter  ' Update progress.

        If mItem.Key = rsTitles!ISBN Then ' Duplicate
            ' Add the author to the list of authors.
            mItem.ListSubItems(1).Text = _
            mItem.ListSubItems(1).Text & _
            " & " & rsTitles!author
        Else
            AddListItem mItem, rsTitles
        End If
        rsTitles.MoveNext
    Loop
    prgLoad.Visible = False
    mCurrentIndex = pubID
End Sub

Private Function GetTitles(ByRef ParentNode As node, pubID) As Boolean
    Dim rsTitles As New ADODB.Recordset
    Dim newNode As node ' For new Node.
    Dim strQ As String
    Dim intCounter As Integer ' For ProgressBar value.
    
    ' Check that the node isn't already populated. If it is, then
    ' add only the ListItem objects to the ListView and exit.
    If ParentNode.Children Then
        AddListItemsOnly pubID
        Exit Function
    End If
    
    ' If ListView is already populated, clear it.
    lvwDB.ListItems.Clear
    
    ' SQL Query that retrieves all the fields needed.
    strQ = "SELECT Titles.Title, Authors.Author, Titles.ISBN, " & _
    "Titles.[Year Published] FROM Authors INNER JOIN " & _
    "(Titles INNER JOIN [Title Author] " & _
    "ON Titles.ISBN = [Title Author].ISBN) ON Authors.Au_ID = " & _
    "[Title Author].Au_ID WHERE Titles.PubID=" & pubID
    
    ' Open the recordset. Exit if no results.
    With rsTitles
        .Open strQ, cn, adOpenStatic, adLockReadOnly, adCmdText
        If .BOF Then
            ' If no results, return a false and exit
            GetTitles = False
            Exit Function
        End If
        .MoveLast
        .MoveFirst
        prgLoad.Max = .RecordCount + 1
    End With
    
    ' Show Progress bar
    prgLoad.Visible = True
    
    On Error GoTo childErr
    ' Add a first node.
    AddNode newNode, ParentNode, rsTitles
    ' Add a corresponding ListItem.
    AddListItem mItem, rsTitles
    
    rsTitles.MoveNext
    
    ' Go through the rest of the recordset. If the next record
    ' is a duplicate, then just add the author's name.
    ' Otherwise, add a new Node and ListItem.
    Do Until rsTitles.EOF
        intCounter = intCounter + 1 ' For the ProgressBar.
        prgLoad.Value = intCounter  ' Update progress.

        ' Check the Key against the current ISDN. If they are the same
        ' then the record only differs by containing a different
        ' author. So add the author to the current list.
        If newNode.Key = rsTitles!ISBN Then
            ' Add the author to the list of authors.
            mItem.ListSubItems("author").Text = _
            mItem.ListSubItems("author").Text & _
            " & " & rsTitles!author
        Else ' Add a new Node and ListItem
            AddNode newNode, ParentNode, rsTitles
            AddListItem mItem, rsTitles
        End If
        rsTitles.MoveNext
    Loop
    GetTitles = True ' return true for success
    
    prgLoad.Visible = False
    mCurrentIndex = pubID
    Exit Function
childErr:
        Debug.Print Err.Number, Err.Description
        
        Debug.Print rsTitles!ISBN
        Resume Next
    
    Exit Function
End Function
Private Sub AddNode(ByRef newNode As node, ByRef ParentNode As node, ByRef rs As ADODB.Recordset)
    ' Add a new node. The newNode and ParentNode are both needed.
    Set newNode = tvwDB.Nodes.Add(ParentNode, _
    tvwChild, rs!ISBN, rs!TITLE, "smlBook")
    newNode.Tag = "book"
End Sub
Private Sub AddListItem(ByRef xItem As ListItem, ByRef xRec As ADODB.Recordset)
    ' Add a ListItem setting its text, icon and small icon. Then
    ' add three ListSubItems setting the Key and Text of each.
    Set xItem = lvwDB.ListItems.Add(Key:=xRec!ISBN, _
    Text:=xRec!TITLE, Icon:="book", SmallIcon:="smlBook")

    xItem.ListSubItems.Add Key:="author", Text:=xRec!author
    If Not IsNull(xRec![Year Published]) Then
        xItem.ListSubItems.Add Key:="year", Text:=xRec![Year Published]
    End If
    xItem.ListSubItems.Add Key:="isbn", Text:=xRec!ISBN
End Sub

Private Sub tvwDB_NodeClick(ByVal node As node)
    ' Check the Tag for "Publisher" and EventFlag
    ' variable to see if the ColumnHeaders
    ' have already been created. If not, then
    ' invoke the MakeColumns procedure.
    If node.Tag = "Publisher" And EventFlag <> _
    PUBLISHER Then MakeColumns
    ' If the Tag is "Publisher" and the mItemCurrentIndex
    ' index isn't the same as the Node.key, then
    ' invoke the GetTitles function, which populates the Node.
    If node.Tag = "Publisher" And mCurrentIndex <> Val(node.Key) _
    Then GetTitles node, Val(node.Key)
    
    If node.Tag = "Publisher" Then PopStatus node
    node.Sorted = True
        
    ' If the node's Tag is "book" then make sure the clicked
    ' book is visible in the ListView by using the EnsureVisible method.
    If node.Tag = "book" Then
        Dim liBook As ListItem
        Set liBook = lvwDB.FindItem(node.Text)
        liBook.EnsureVisible
    End If
    
End Sub
 
Private Sub PopStatus(node As node)
    ' Simply change the StatusBar to reflect current values.
    With sbrDB
        .Panels.Clear
        .Panels.Add , "name", node.Text
        .Panels.Add , "number", node.Children & " titles"
        .Panels(1).AutoSize = sbrContents
        .Panels(2).AutoSize = sbrSpring
    End With
End Sub
