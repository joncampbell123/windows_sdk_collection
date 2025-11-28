VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "Sales"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit
Dim sngPubRevenue() As Single     ' Array of monthly revenue amounts
Dim sngAuthorRoyalty() As Single  ' Array of monthly royalty amounts
Dim sngBookPrice() As Single        ' Array of monthly book prices

Public Function GetAuthors() As ADODB.Recordset
'This routine returns the complete list of known authors.  It is provided as a service to clients
'so that they do not need to know how or where to get the data themselves.  As such, it behaves
'as a "Data Service".  Normally, Data Services would be grouped separately from Business
'Services to help avoid development and maintenance dependencies.
    Dim strSQL As String
    
    strSQL = "SELECT Authors.Author FROM Authors"
    Set rsAuthors = New ADODB.Recordset
    With rsAuthors
      .CursorLocation = adUseClient
      .Open strSQL, gCN, adOpenStatic, adLockReadOnly
    End With
   Set GetAuthors = rsAuthors
End Function

Public Function GetTitles(ByVal strSQL As String) As ADODB.Recordset
   ' Get titles using a query passed from the client.
   Set rsTitles = New ADODB.Recordset
   With rsTitles
      .CursorLocation = adUseClient
      .Open strSQL, gCN, adOpenStatic, adLockReadOnly
   End With
   Set GetTitles = rsTitles
End Function
Public Function GetBookPages(ByVal strSQL As String) As ADODB.Recordset
   ' Get book pages using a query passed from the client.
   Set rsBookPages = New ADODB.Recordset
   With rsBookPages
      .CursorLocation = adUseClient
      .Open strSQL, gCN, adOpenStatic, adLockReadOnly
   End With
   Set GetBookPages = rsBookPages

End Function
Public Function GetRsCOGS(ByVal strSQL As String) As ADODB.Recordset
   ' Get Cost Of Goods using a query passed from the client.
   Set rsCOGS = New ADODB.Recordset
   With rsCOGS
      .CursorLocation = adUseClient
      .Open strSQL, gCN, adOpenStatic, adLockReadOnly
   End With
   Set GetRsCOGS = rsCOGS
End Function

Public Function GetRevenue(intSalesModel As Integer, _
                              curCostPerUnit As Currency, _
                              curAdvCost As Currency, _
                              intSalesPeriod As Integer, _
                              lngUnitsPerMonth As Long, _
                              bolIsDiscount As Boolean, _
                              strBookTitle As String) As Variant

  Dim i As Integer
  Dim iOldBound As Integer
  Dim iNewBound As Integer

  gintSalesModel = intSalesModel
  gcurCostPerUnit = curCostPerUnit
  gcurAdvertisingCost = curAdvCost
  gintSalesPeriod = intSalesPeriod
  glngUnitsPerMonth = lngUnitsPerMonth
  
  If GetPubRevenue(strBookTitle) = False Then
      ServerMsg Error$ & " - " & Str$(Err), vbOKOnly, "GetChartData Error"
    GetRevenue = 0
    Exit Function
  
  End If

  If GetAuthorRoyalty() = False Then
      ServerMsg Error$ & " - " & Str$(Err), vbOKOnly, "GetChartData Error"
    GetRevenue = 0
    Exit Function

  End If

  iOldBound = UBound(sngPubRevenue)

  For i = 0 To iOldBound
    sngPubRevenue(i, 1) = sngAuthorRoyalty(i)
  
  Next i

  GetRevenue = sngPubRevenue()

End Function

Public Function GetAuthorRoyalty() As Boolean
Dim i As Integer

Dim cGrossMonthlySalary As Currency
Dim cTaxAmount As Currency
Dim cTotalRevenue As Currency

'Create reference to Tax Class
Dim objTax As New Taxes

frmBookSales.lblStatus(1).Caption = "Request Author Royalty..."

ReDim sngAuthorRoyalty(gintSalesPeriod)

  For i = 0 To (gintSalesPeriod - 1)
    cGrossMonthlySalary = sngPubRevenue(i, 0) * gRoyalty
    sngAuthorRoyalty(i) = cGrossMonthlySalary - _
                                    objTax.CalcNationalIncomeTax(cGrossMonthlySalary) - _
                                    objTax.CalcSalesTax(cGrossMonthlySalary, 0)

  Next i

  ' Delete Class Reference
  Set objTax = Nothing
  
  frmBookSales.lblStatus(1).Caption = "Calculating Author Royalty..."
  GetAuthorRoyalty = True

End Function

Public Function GetPubRevenue(strTitle As String) As Variant
Dim sn As ADODB.Recordset

Dim strSQL As String
Dim i As Integer
Dim Price As Currency

'Create Class References
Dim objModel As New Model

Static strOldTitle As String
Static cUnitPrice As Currency

frmBookSales.lblStatus(0).Caption = "Request Publisher Revenue..."
frmBookSales.lblStatus(1).Caption = "Calculating Publisher Revenue..."

On Error GoTo GetRevenueError

If strTitle <> strOldTitle Then
  frmBookSales.lblStatus(1).Caption = "Fetching row " & strTitle & "..."
  strSQL = "SELECT Titles.Price " & _
                "FROM Titles " & _
                "WHERE ((Titles.Title=" & Chr$(34) & strTitle & Chr$(34) & "));"
    ' Initialize the ADODB Recordset variable, and open it using the
    ' global connection object gCN.
  Set sn = New ADODB.Recordset
  sn.Open strSQL, gCN, adOpenForwardOnly, adLockReadOnly
  'Set sn = gdb.OpenRecordset(strSQL, dbOpenSnapshot)
  cUnitPrice = sn.Fields("Price")
  
Else
  frmBookSales.lblStatus(1).Caption = "Using last values..."
  
End If

ReDim sngPubRevenue(gintSalesPeriod - 1, 1)
ReDim sngBookPrice(gintSalesPeriod - 1)

For i = 0 To gintSalesPeriod - 1
   sngPubRevenue(i, 0) = cUnitPrice * _
                          objModel.intGetMonthSales(i, _
                          gintSalesPeriod, _
                          gintSalesModel)
Next i

'Delete Class Reference
Set objModel = Nothing

frmBookSales.lblStatus(1).Caption = "Sending publisher revenue to client..."
GetPubRevenue = True

' Don't try to close the object if we never created the snapshot.
' sn is never defined when strTitle = strOldTitle.
  If strTitle <> strOldTitle Then
    sn.Close
    Set sn = Nothing
  End If

  strOldTitle = strTitle

  Exit Function
  If IsObject(sn) Then sn.Close
  Set sn = Nothing

GetRevenueError:
  frmBookSales.lblStatus(1).Caption = Error$ & " - " & Str$(Err)
  GetPubRevenue = False
  
End Function

Private Sub Class_Initialize()
' Class initialized when instantiated by Client.

On Error GoTo InitErr
    
  If gintInstanceCount = 0 Then
    frmBookSales.Show
    gintInstanceCount = 0
  ' Create global ADODB Connnection, set ConnnectionString, and open.
    LoadDB
    frmBookSales.lblStatus(1).Caption = "opening " & gDBName & "..."
  
    frmBookSales.lblStatus(1).Caption = "Awaiting command..."
  End If
    
  gintInstanceCount = gintInstanceCount + 1
  frmBookSales.lblInstanceCount.Caption = Format$(gintInstanceCount)

  Exit Sub

InitExit:
    Screen.MousePointer = vbDefault
Exit Sub
  
InitErr:
  
  frmBookSales.lblStatus(1).Caption = Error$ & " - " & Str$(Err)
  
  If Err.Number <> 0 Then ' another error
      ServerMsg Error$ & " - " & Str$(Err), vbCritical, "BookSale Server Startup Error"
      End
  End If
  
  Resume InitExit
  
End Sub
Private Sub LoadDB()
  ' Load the booksale.mdb. If the file isn't in the
  ' stated directory, present the CommonDialog control
  ' to let the end user find the file.
   
   gDBName = "booksale.mdb"
   On Error GoTo LoadDBError
   Set gCN = New ADODB.Connection ' Global Connection object.

   ' Set ConnectionString for Connection Object.
   gCN.ConnectionString = "Provider=Microsoft.Jet.OLEDB.3.51;Data Source=" & _
   Left(App.Path, Len(App.Path) - Len("client")) & gDBName
   gCN.Open ' Open the connection.
  
  Exit Sub
LoadDBError:
   Select Case Err.Number
   Case -2147467259
      ' Can't find file, so use CommonDialog to find it.
      gCN.ConnectionString = "Provider=Microsoft.Jet.OLEDB.3.51;Data Source=" & _
      GetBooksale
      Resume
   Case Else ' Other unknown errors
       MsgBox Err.Number & ": " & Err.Description
   End Select
End Sub

Private Function GetBooksale() As String
   ' Returns the path of the booksale.mdb.
   ' If the booksale.mdb isn't at the default location,
   ' present a CommonDialog control to the end user, and
   ' prompt to find the database.
    On Error GoTo ErrHandler

   With frmBookSales.dlgFindDB
      .DialogTitle = "Please find Booksale.mdb"
      .InitDir = App.Path
      .FileName = gDBName
      .Filter = "Access (*.mdb)| *.mdb"
      .CancelError = True 'Causes an error if user clicks on cancel
      .ShowOpen
   End With
    
   Do While UCase(Right(Trim(frmBookSales.dlgFindDB.FileName), Len("booksale.mdb"))) <> "BOOKSALE.MDB"
         MsgBox "File Name is not equal to BOOKSALE.MDB"
         frmBookSales.dlgFindDB.ShowOpen
   Loop
   
   GetBooksale = frmBookSales.dlgFindDB.FileName
   Exit Function
ErrHandler:
    If Err = 32755 Then 'Cancel caused an error
      End
    End If
End Function

Private Sub Class_Terminate()
  gintInstanceCount = gintInstanceCount - 1
  frmBookSales.lblInstanceCount.Caption = Format$(gintInstanceCount)
  
  If gintInstanceCount <= 0 Then
    Unload frmBookSales
  End If
End Sub


