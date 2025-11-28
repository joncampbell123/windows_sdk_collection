'----------------------------------------------------------------------------
' File: dxdiag.vb
'
' Desc: Sample app to read info from dxdiagn.dll by enumeration
'
' Copyright (c) Microsoft Corp. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports Microsoft.DirectX.Diagnostics


Namespace DxDiagOutput
    Class DxDiagDisplay

        Overloads Shared Sub Main(ByVal args() As String)
            Try
                ' Just start our recursive loop with our root container.  Don't worry
                ' about checking Whql
                OutputDiagData(Nothing, New Container(False))
            Catch
                ' Something bad happened
            End Try
        End Sub 'Main


        '/ <summary>
        '/ Recursivly print the properties the root node and all its child to the console window
        '/ </summary>
        '/ <param name="parent">A string to display to show the root node of this data</param>
        '/ <param name="root">The actual container for this data.</param>
        Shared Sub OutputDiagData(ByVal parent As String, ByVal root As Container)
            Try
                Dim pd As PropertyData
                For Each pd In root.Properties
                    ' Just display the data
                    Console.WriteLine("{0}.{1} = {2}", parent, pd.Name, pd.Data)
                Next pd
            Catch
            End Try
            
            Try
                Dim cd As ContainerData
                For Each cd In root.Containers
                    ' Recurse all the internal nodes
                    If (parent Is Nothing) Then
                        OutputDiagData(cd.Name, cd.Container)
                    Else
                        OutputDiagData(parent + "." + cd.Name, cd.Container)
                    End If
                Next cd
            Catch
            End Try
                
            ' We are done with this container, we can dispose it.
            root.Dispose()
        End Sub 'OutputDiagData
    End Class 'DxDiagDisplay
End Namespace 'DxDiagOutput