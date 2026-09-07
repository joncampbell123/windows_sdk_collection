VERSION 5.00
Begin VB.Form frmMain 
   Caption         =   "Test Entity Proxy DataServices"
   ClientHeight    =   960
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4365
   LinkTopic       =   "Form1"
   ScaleHeight     =   960
   ScaleWidth      =   4365
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdNewOrder 
      Caption         =   "Add New Order"
      Height          =   375
      Left            =   1560
      TabIndex        =   0
      Top             =   240
      Width           =   1335
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private Sub cmdNewOrder_Click()
    Dim proxy As New DataServices.proxy
    Dim customer As IEntityData
    Dim order As IEntityData
    Dim orderitem As IEntityData
    Dim product As IEntityData
    
On Error GoTo cmdNewOrder_Click_Error

    proxy.ManualConstruct "DSN=Northwind"
    
    Set customer = proxy.EntityLoad("spCustomersLoad", "spCustomersUpdate", "spCustomersDelete", "cpCustomerInsert", "CustomerID", ChooseCustomer(CLng(Rnd * 90) + 1))
    Set product = proxy.EntityLoad("spProductsLoad", "spProductsUpdate", "spProductsDelete", "spProductsInsert", "ProductID", CLng(Rnd * 76) + 1)
       
    CreateNewOrders order
    
    order!CustomerID = customer!CustomerID
    order!EmployeeID = 3
    order!ShipVia = 1
    order!Freight = 17.52
    order!ShipName = customer!ContactName
    order!ShipAddress = customer!Address
    order!ShipCity = customer!City
    order!ShipRegion = customer!Region
    order!ShipPostalCode = customer!PostalCode
    order!ShipCountry = customer!Country
    
    CreateNewOrderDetails orderitem
    
    orderitem!OrderID = proxy.EntityUpdate(order)
    orderitem!ProductID = product!ProductID
    orderitem!UnitPrice = product!UnitPrice
    orderitem!Quantity = 1
    orderitem!Discount = 0
    
    proxy.EntityUpdate orderitem
    
    Exit Sub
cmdNewOrder_Click_Error:
    MsgBox " An Error occured: " & Err.Description
End Sub
