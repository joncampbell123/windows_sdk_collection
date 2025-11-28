VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "SmallBusiness2"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit
' >> Best viewed in Full Module view. <<
'
' Storage for debug ID number.
Private mlngDebugID As Long
Implements IDebug

' Private Collection object is slightly
'   more robust than a public Collection
'   object, but it has problems.
Private colEmployees As New Collection

' Problem 1: The collection is private,
'   so the Add method has to be part of
'   the SmallBusiness2 class.  Since
'   there may be many kinds of objects in
'   a business, there may be many such
'   methods, such as AddProduct.
'
Public Function AddEmployee(ByVal Name As String, ByVal Salary As Double) As Employee
    Dim empNew As New Employee
    Static intEmpNum As Integer
    ' Using With shortens property references (.ID instead of empNew.ID)
    ' and speeds up execution.
    With empNew
        ' Generate a unique ID for the new employee.
        intEmpNum = intEmpNum + 1
        .ID = "E" & Format$(intEmpNum, "00000")
        .Name = Name
        .Salary = Salary
        ' Add the Employee object reference to the collection, using its
        ' ID property as the key.
        colEmployees.Add empNew, .ID
    End With
    ' The Add method should return a reference to
    '   the new object.
    Set AddEmployee = empNew
End Function

' Problem 1, continued:  There will also
'   be Count functions for all the
'   different business objects, and
'   Delete methods, and so on.  Even
'   with just Employee objects, the
'   SmallBusiness2 class becomes very
'   cluttered.
'
Public Function EmployeeCount() As Long
    EmployeeCount = colEmployees.Count
End Function

Public Sub DeleteEmployee(ByVal Index As Variant)
    colEmployees.Remove Index
End Sub

' Problem 2: The Employees method can't
'   be used with For Each.  You could make
'   it work with For Each by returning a
'   reference to the private collection,
'   but then it's not a private collection
'   any more, and you're right back in
'   the House of Straw.
'
Public Function Employees(ByVal Index As Variant) As Employee
    Set Employees = colEmployees.Item(Index)
End Function

' Problem 3: It's still possible for
'   coding errors, this time within the
'   vastly more complicated SmallBusiness
'   object, to add invalid objects to the
'   collection.
Public Sub Trouble()
    Dim X As New Employee
    ' Because the Collection object accepts
    '   a reference to any object, it's
    '   still possible for an internal
    '   coding error to add an uninitialized
    '   Employee object.
    colEmployees.Add X
End Sub

Private Sub Class_Initialize()
    mlngDebugID = DebugInit(Me)
End Sub

Private Sub Class_Terminate()
    DebugTerm Me
End Sub

' -------- IDebug Implementation --------
'
' IDebug.DebugID gives you a way to tell
' ====== -------    objects apart.  It's
'   required by the DebugInit, DebugTerm,
'   and DebugShow debugging procedures
'   declared in modFriend.
'
Private Property Get IDebug_DebugID() As Long
    IDebug_DebugID = mlngDebugID
End Property


