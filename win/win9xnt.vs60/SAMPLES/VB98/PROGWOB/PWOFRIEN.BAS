Attribute VB_Name = "modFriends"
Option Explicit
' Constants for debug functions.
Global Const DEBUGTOKEN_DebugID = 1
Global Const DEBUGTOKEN_ClassName = 2

' The user-defined type used by the
'   Friend member demo.
Public Type udtDEMO
    intA As Integer
    lngB As Long
    strC As String
End Type

' timeGetTime is used by the Implements
' -----------       demo, and by the
'   object lifetime debug code in this
'   module.
Declare Function timeGetTime Lib "winmm.dll" () As Long

' Storage for the global collection
'   for debugging object lifetimes.
'   Used by DebugInit, DebugTerm,
'   and DebugShow procedures (below).
Private mcolDebug As New Collection

'         DEBUGGING PROCEDURES
'
' DebugInit
' DebugTerm
' DebugShow
'
' All objects implement the IDebug
'   interface, and support it by
'   calling DebugInit(Me) in their
'   Initialize events, and DebugTerm Me
'   in their Terminate events.  You can
'   use DebugShow in the Immediate window
'   to list an active object, all active
'   objects, or all objects of a class.
'
' All of the objects in this project are
'   set up to use these functions.
'
' -------------------------------------
' DebugInit is called by each object,
' ---------     in its Initialize event.
'   DebugInit adds a debug string for the
'   object to the global collection, and
'   returns a unique DebugID for the
'   object. The method optionally shows
'   the debug string in the Immediate
'   window (default is True).
'
Public Function DebugInit(ByVal obj As Object, _
        Optional ByVal ShowImmediate As Boolean = True) As Long
    Dim lngDebugID As Long
    Dim strDebug As String
    
    ' Get a unique ID number.
    lngDebugID = GetDebugID
    ' The debug string kept for each
    '   object shows the DebugID, the
    '   class name of the object, and
    '   the time it was initialized
    '   (number of seconds since the
    '   first debug object was created,
    '   expressed as a Double, with the
    '   milliseconds as the fractional
    '   part).
    strDebug = lngDebugID & " " _
        & TypeName(obj) _
        & " (created at " & DebugTime & ")"
    '
    ' Add the string to the collection,
    '   using the unique ID as a key.
    mcolDebug.Add strDebug, CStr(lngDebugID)
    '
    ' The default is to show the debug
    '   string in the Immediate window.
    If ShowImmediate Then Debug.Print strDebug
    '
    ' Return the DebugID.  The object must
    '   store this as part of the
    '   implementation of IDebug.
    DebugInit = lngDebugID
End Function

' DebugTerm is called by each object,
' ---------     in its Terminate event.
'   DebugTerm removes the object's
'   debug string from the global
'   collection, and optionally (default
'   is True) shows the debug string in
'   the Immediate window.
'
Public Sub DebugTerm(ByVal obj As Object, _
        Optional ByVal ShowImmediate As Boolean = True)
    
    Dim idbg As IDebug
    
    On Error Resume Next
    '
    ' Get a reference to the object's
    '   IDebug interface.
    Set idbg = obj
    If Err.Number <> 0 Then
        MsgBox TypeName(obj) & " doesn't implement IDebug; can't record termination.", , "DebugTerm"
        Exit Sub
    End If
    '
    ' The default is to show the debug
    '   string in the Immediate window.
    If ShowImmediate Then Debug.Print _
        mcolDebug(CStr(idbg.DebugID)) _
        & " (Term at " & DebugTime & ")"
    '
    ' Remove the string from the
    '   collection.
    mcolDebug.Remove CStr(idbg.DebugID)
End Sub

' DebugShow displays the debug string(s)
' ---------     for the entire list of
'   active objects, for all active objects
'   of a class, or for a particular object.
'   Call DebugShow from the Immediate
'   window with no argument (lists all),
'   a class name (lists all of that class),
'   an object reference (lists that
'   object), or the DebugID of an object
'   (lists that object).
'
Public Sub DebugShow(Optional ByVal What As Variant)
    Dim vnt As Variant
    Dim idbg As IDebug
    
    On Error GoTo NoShow
    ' If no argument is supplied, display
    '   all active objects.  (It would be
    '   useful to have an optional second
    '   parameter Filename that would let
    '   you dump this to a file; or perhaps
    '   it should dump to the Clipboard.)
    If IsMissing(What) Then
        What = "<All>"
        For Each vnt In mcolDebug
            Debug.Print vnt
        Next
    '
    ' If an object is supplied, use its
    '   DebugID to look up its debug
    '   string.
    ElseIf IsObject(What) Then
        On Error Resume Next
        '
        ' Get a reference to the object's
        '   IDebug interface.
        Set idbg = What
        If Err.Number <> 0 Then
            MsgBox TypeName(What) & " doesn't implement IDebug; can't show debug record.", , "DebugShow"
            Exit Sub
        End If
        '
        Debug.Print mcolDebug(CStr(idbg.DebugID))
    '
    ' If a number is supplied, assume it's
    '   a DebugID and use it to look up
    '   the string.
    ElseIf IsNumeric(What) Then
        Debug.Print mcolDebug(CStr(What))
    '
    ' If it's not a number, assume it's
    '   a string containing the class
    '   name; display all objects with
    '   that class name.
    Else
        For Each vnt In mcolDebug
            If What = GetDebugToken(vnt, DEBUGTOKEN_ClassName) Then
                Debug.Print vnt
            End If
        Next
    End If
    Exit Sub
    
NoShow:
    If IsObject(What) Then
        MsgBox "Unable to display information.  Is this object set up for debugging?", , "DebugShow"
    Else
        MsgBox "Unable to display information for " _
            & What & ".  Is this object set up for debugging?", , "DebugShow"
    End If
End Sub

' GetDebugString returns an object's
' --------------    string from the global
'   collection.
'
Public Function GetDebugString(ByVal obj As Object) As String
    Dim idbg As IDebug
    
    On Error Resume Next
    '
    ' Get a reference to the object's
    '   IDebug interface.
    Set idbg = obj
    GetDebugString = mcolDebug(CStr(idbg.DebugID))
End Function

' GetDebugID is used to assign each object
' ----------    a unique ID number, for
'   debugging purposes.
Public Function GetDebugID() As Long
    Static lngLastID As Long
    lngLastID = lngLastID + 1
    GetDebugID = lngLastID
End Function

' GetDebugToken parses the debug string
' -------------     for an object and
'   returns the requested token.  Tokens
'   are separated by single spaces.
'   (1) DebugID
'   (2) class name
'
' There are other tokens, but they're
'   kind of a jumble.
'
Public Function GetDebugToken( _
        ByVal DebugString As String, _
        ByVal TokenNumber As Integer) As String

    Dim inx1 As Long
    Dim inx2 As Long
    Dim ct As Integer
    
    If TokenNumber <= 0 Then
        Err.Raise vbObjectError + 1060, , _
            "Bad token number in GetDebugToken"
    Else
        inx2 = 1
        For ct = 1 To TokenNumber
            inx1 = inx2
            inx2 = InStr(inx1, DebugString, " ")
            If inx2 = 0 Then Exit For
        Next
        If inx2 = 0 Then
            GetDebugToken = ""
        Else
            GetDebugToken = Mid$(DebugString, inx1 + 1, inx2 - inx1)
        End If
    End If
End Function
        
' DebugTime uses the timeGetTime API to
' ---------     get milliseconds since
'   the computer was booted.  This is
'   converted to a Double containing the
'   number of seconds since the first
'   debug object was created (s.mmm),
'   using the first time this function
'   was called as the base time.  (This
'   makes the time values more useful
'   than the raw number of milliseconds
'   since the last boot, which (1) tends
'   to be a very large number, and (2) can
'   be negative, as explained below.)
'
Public Function DebugTime() As Double
    Static timeBase As Double
    Dim timeCurrent As Double
    
    If timeBase = 0 Then
        ' Initialize the base time.  (The
        '   loop allows for the fact that
        '   the time returned by timeGetTime
        '   can pass through zero again, if
        '   the computer is left running
        '   long enough.)
        Do While timeBase = 0
            timeBase = timeGetTime
        Loop
        '
        ' The value returned by timeGetTime
        '   can be negative (see note
        '   below) if the computer has
        '   been running long enough.
        '   Correct for this.
        If timeBase < 0 Then
            timeBase = timeBase + 4294967296#
        End If
    End If
    '
    timeCurrent = timeGetTime
    '
    ' Correct for negative value, if
    '   necessary.
    If timeCurrent < 0 Then
        timeCurrent = timeCurrent + 4294967296#
    End If
    '
    ' Handle the case where timeGetTime
    '   rolls over to zero.
    If timeCurrent < timeBase Then
        DebugTime = (timeCurrent + 4294967296# - timeBase) / 1000#
    Else
        DebugTime = (timeCurrent - timeBase) / 1000#
    End If
End Function
' ----------- timeGetTime -----------
' The number of milliseconds since
'   last boot is an unsigned four-byte
'   binary integer, which means it can
'   get bigger than a Long can hold.
'   When it passes the largest positive
'   number a Long can hold, 2147483647,
'   it appears to Basic as if the
'   number has 'rolled over' and gone
'   negative.  Once it has rolled over,
'   it continues increasing -- moving
'   from the largest negative number a
'   Long can hold up to zero, and then
'   into positive numbers again.
'
' This creates a 'sawtooth' pattern,
'   and it works just fine for time
'   differences (which is what
'   DebugTime is calculating), except
'   for that awkward moment when the
'   rollover happens.
'
' DebugTime solves this problem by
'   putting the number into a larger
'   container -- a Double.  If the
'   number is negative, it can be
'   turned into the number it should
'   have been by adding 4294967296.
