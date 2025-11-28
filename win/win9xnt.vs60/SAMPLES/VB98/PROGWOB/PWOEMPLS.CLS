VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Employees"
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

' Private collection that does all the
'   work.
Private colEmployees As New Collection

' NewEnum method enables the use of For
' -------       Each with the Employees
'   collection class.  For an
'   explanation, see "Creating Your Own
'   Collection Class: The House of Bricks"
'   in Books Online.
'
' For NewEnum to work properly, its
'   procedure ID must be set to -4.  To see
'   this, select Procedure Attributes
'   from the Tools menu.  In the Name box,
'   select Item, then click the Advanced
'   button.  You can see that the Procedure
'   ID has been set to -4.  ("Hide this
'   member" is checked, also, so that the
'   NewEnum method is hidden in the Object
'   Browser.)
Public Function NewEnum() As IUnknown
Attribute NewEnum.VB_UserMemId = -4
Attribute NewEnum.VB_MemberFlags = "40"
    ' Delegate to the private Collection
    '   object's _NewEnum method.
    Set NewEnum = colEmployees.[_NewEnum]
End Function

' Add method creates a new Employee in
' ---       the collection.
'
Public Function Add(ByVal Name As String, _
                ByVal Salary As Double) As Employee
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
    ' It is good practice to return a reference to the new member.
    Set Add = empNew
End Function

' To see how the Item method was made the
'   default method for the Employees
'   collection, select Procedure Attributes
'   from the Tools menu.  In the Name box,
'   select Item, then click the Advanced
'   button.  You can see that (Default)
'   has been selected in the Procedure ID
'   box.
Public Function Item(ByVal Index As Variant) As Employee
    Set Item = colEmployees.Item(Index)
End Function

Public Function Count() As Long
    Count = colEmployees.Count
End Function

Public Sub Delete(ByVal Index As Variant)
    colEmployees.Remove Index
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


