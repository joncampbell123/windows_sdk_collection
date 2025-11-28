'----------------------------------------------------------------------------
' File: HostInfo.cs
'
' Desc: Contains the HostInfo collection item class. Objects of this class
'       are used to store information about detected hosts.
'
' Copyright (c) Microsoft Corp. All rights reserved.
'----------------------------------------------------------------------------- 
Imports System
Imports Microsoft.DirectX.DirectPlay


Namespace Tut09_Client
    '/ <summary>
    '/ This class contains display and address information for a detected host.
    '/ </summary>
    Public Class HostInfo
        Public GuidInstance As Guid ' Instance Guid for the session
        Public HostAddress As Address ' DirectPlay address of the session host  
        Public SessionName As String
        ' Display name for the session
        '/ <summary>
        '/ Used by the system collection class
        '/ </summary>
        Public Overloads Function Equals(ByVal obj As Object) As Boolean
            Dim node As HostInfo = CType(obj, HostInfo)
            Return GuidInstance.Equals(node.GuidInstance)
        End Function 'Equals

        '/ <summary>
        '/ Used by the system collection class
        '/ </summary>
        Public Overrides Function GetHashCode() As Integer
            Return GuidInstance.GetHashCode()
        End Function 'GetHashCode


        '/ <summary>
        '/ Used by the system collection class
        '/ </summary>
        Public Overrides Function ToString() As String
            Dim displayString As String = IIf(Not (SessionName Is Nothing), SessionName, "<unnamed>")
            displayString += " (" + HostAddress.GetComponentString("hostname")
            displayString += ":" + HostAddress.GetComponentInteger("port").ToString() + ")"

            Return displayString
        End Function 'ToString
    End Class 'HostInfo
End Namespace 'Tut09_Client