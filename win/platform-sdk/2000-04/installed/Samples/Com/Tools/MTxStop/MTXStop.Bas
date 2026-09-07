Attribute VB_Name = "Module1"
Sub main()
    ' This is mtxstop.exe
    
    Dim cat As New COMAdminCatalog
    Dim catcoll As COMAdminCatalogCollection
    
    Set catcoll = cat.GetCollection("Applications")
    catcoll.Populate
    For i = 0 To catcoll.Count - 1
        If catcoll.Item(i).Value("Activation") = COMAdminActivationLocal Then
            If catcoll.Item(i).Key <> "{02D4B3F1-FD88-11D1-960D-00805FC79235}" Then
                cat.ShutdownApplication catcoll.Item(i).Key
            End If
        End If
    Next i
    ' Need to shutdown the system app last.
    cat.ShutdownApplication "{02D4B3F1-FD88-11D1-960D-00805FC79235}"
            
End Sub
