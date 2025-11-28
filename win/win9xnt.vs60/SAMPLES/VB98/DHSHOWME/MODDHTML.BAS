Attribute VB_Name = "modDHTML"
'The following code allows you to use the Web browser's property bag
'   to persist information across different DHTML pages.
Public objWebBrowser As WebBrowser

'PutProperty: Store information in the Property bag by calling this
'             function.  The required inputs are the named Property
'             and the value of the property you would like to store.
'
Public Sub PutProperty(strName As String, vntValue As Variant)
    
    'Check whether we have an instance of the browser.
    If objWebBrowser Is Nothing Then Set objWebBrowser = New WebBrowser
    
    'Call the browser's PutProperty method to store the value.
    objWebBrowser.PutProperty strName, vntValue

End Sub

'GetProperty: Retrieve information from the Property bag by calling this
'             function.  The required input is the named Property,
'             and the return value of the function is the current value
'             of the property.
'
Public Function GetProperty(strName As String) As Variant
    
    'Check whether we have an instance of the browser.
    If objWebBrowser Is Nothing Then Set objWebBrowser = New WebBrowser
    
    'Call the browser's GetProperty method to retrieve the value.
    GetProperty = objWebBrowser.GetProperty(strName)

End Function
