/*************************************************************************
**
**  This is a part of the Microsoft Source Code Samples.
**
**  Copyright (C) 1992-1995 Microsoft Corporation. All rights reserved.
**
**  This source code is only intended as a supplement to Microsoft Development
**  Tools and/or WinHelp documentation.  See these sources for detailed
**  information regarding the Microsoft samples programs.
**
**  Type Library Browser
**
**  browseex.cpp
**
**  Written by Microsoft Product Support Services, Windows Developer Support
**
*************************************************************************/

#include <windows.h>
#include <windowsx.h>
#ifdef WIN16   
  #include <ole2.h>
  #include <compobj.h>    
  #include <dispatch.h> 
  #include <variant.h>
  #include <olenls.h>
  #include <commdlg.h>  
#endif  
#include "resource.h"
#include "browse.h" 
#include "invhelp.h"         

CBrowseApp BrowseApp;         // Application

int APIENTRY WinMain (HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
   WNDCLASS wc;       
   HRESULT hr;

   //  It is recommended that all OLE applications set
   //  their message queue size to 96. This improves the capacity
   //  and performance of OLE's LRPC mechanism.
   int cMsg = 96;                  // Recommend msg queue size for OLE
   while (cMsg && !SetMessageQueue(cMsg))  // take largest size we can get.
       cMsg -= 8;
   if (!cMsg)
       return -1;       
   
   // Register class for the dialog that is the main window.
   wc.style = 0;
   wc.lpfnWndProc = DefDlgProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = DLGWINDOWEXTRA;
   wc.hInstance = hinst;
   wc.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(ID_ICON));
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
   wc.lpszMenuName = NULL;
   wc.lpszClassName = TEXT("DlgClass");
   RegisterClass(&wc);

   hr = OleInitialize(NULL);
   if FAILED(hr)
   {
       MessageBox(NULL, TEXT("Could not Intialize OLE"), TEXT("Error"), MB_ICONEXCLAMATION|MB_OK);
       return 0;
   }
   DialogBox(hinst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)MainDialogFunc); 
   OleUninitialize();
   
   return 0;
}


BOOL CALLBACK MainDialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{  
   HRESULT hr;
   
   switch (msg)
   {  
      case WM_INITDIALOG:
          BrowseApp.Init(hwndDlg);
          return FALSE;
                      
      case WM_COMMAND:
         switch(GET_WM_COMMAND_ID(wParam,lParam))
         {   
             case IDM_EXIT:
             case IDCANCEL:  
                 BrowseApp.Cleanup();                 
                 EndDialog(hwndDlg, IDOK);
                 CoFreeUnusedLibraries();
                 return TRUE;
                 
             case IDM_FILEOPEN:   
                 hr = BrowseApp.BrowseTypeLibrary();
                 if (FAILED(hr))
                     MessageBox(hwndDlg, TEXT("Failed to browse type library"), TEXT("Error"), MB_ICONEXCLAMATION|MB_OK);     
                 return TRUE;
                 
             case IDC_TYPEINFOSLIST:
                 switch (GET_WM_COMMAND_CMD(wParam, lParam))
                 {
                     case LBN_SELCHANGE:
                         hr = BrowseApp.ChangeTypeInfosSelection();    
                         if (FAILED(hr))
                             MessageBox(hwndDlg, TEXT("Failed to get TypeInfo information"), TEXT("Error"), MB_ICONEXCLAMATION|MB_OK);     
                         return TRUE;
                 }
                 break;  
                 
             case IDC_ELEMENTSLIST:
                 switch (GET_WM_COMMAND_CMD(wParam, lParam))
                 {
                     case LBN_SELCHANGE:
                         hr = BrowseApp.ChangeElementsSelection();  
                         if (FAILED(hr))
                             MessageBox(hwndDlg, TEXT("Failed to get Element information"), TEXT("Error"), MB_ICONEXCLAMATION|MB_OK);
                         return TRUE;
                 } 
                 break; 
                 
             case IDC_PARAMETERSLIST:
                 switch (GET_WM_COMMAND_CMD(wParam, lParam))
                 {
                     case LBN_SELCHANGE:
                         hr = BrowseApp.ChangeParametersSelection();
                         if (FAILED(hr))
                             MessageBox(hwndDlg, TEXT("Failed to get Parameter information"), TEXT("Error"), MB_ICONEXCLAMATION|MB_OK);
                         return TRUE;
                 } 
                 break;
                 
             case IDC_ELEM_OPEN_HELPFILE:
                 BrowseApp.OpenElementHelpFile();   
                 return TRUE;    
                                                                                               
         }
         break;
   }
   return FALSE;
}  

/*
 * CBrowseApp::BrowseTypeLibrary
 *
 * Purpose:
 *  Prompts user for type library. Fills the TypeInfo list with the TypeInfos of
 *  the type library.
 *
 */
HRESULT CBrowseApp::BrowseTypeLibrary()
{   
    OPENFILENAME   ofn;
    TCHAR szFileName[128]; 
    BOOL bRes;
    VARIANT vRet;
    EXCEPINFO excepinfo;
    UINT nArgErr;  
    HRESULT hr;
    HCURSOR hcursorOld;
    LPDISPATCH pdispBrowseHelper = NULL;  
    LPDISPATCH pdispTypeLibrary = NULL;
    LPDISPATCH pdispTypeInfos = NULL;      
    TCHAR szTemp[100];
    
    // Prompt user for type library
    szFileName[0] = '\0';
    _fmemset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hwndMain;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile  = sizeof(szFileName);
    ofn.hInstance = m_hinst;
    ofn.lpstrFilter  = TEXT("Type Libraries *.tlb,*.olb\0*.tlb;*.olb\0All Files *.*\0*.*\0\0");
    ofn.nFilterIndex = 1; 
    ofn.Flags= OFN_FILEMUSTEXIST;    
    bRes = GetOpenFileName(&ofn);
    if (!bRes)
        return ResultFromScode(S_FALSE);  
        
    // Empty lists and clear fields.
    EmptyList(GetDlgItem(m_hwndMain, IDC_TYPEINFOSLIST)); 
    EmptyList(GetDlgItem(m_hwndMain, IDC_ELEMENTSLIST));
    EmptyList(GetDlgItem(m_hwndMain, IDC_PARAMETERSLIST));   
    ClearTypeLibStaticFields();
    ClearTypeInfoStaticFields();
    ClearElementStaticFields();
    ClearParamStaticFields(); 
    EnableWindow(GetDlgItem(m_hwndMain, IDC_ELEM_OPEN_HELPFILE), FALSE); 
    
    hcursorOld = SetCursor(LoadCursor(NULL, IDC_WAIT));
    // Create BrowseHelper object
    hr = CreateObject(OLESTR("BrowseHelper.Browser"), &pdispBrowseHelper);   
    if (FAILED(hr))  
    {
        MessageBox(m_hwndMain, TEXT("Failed to create BrowseHelper object"), TEXT("Error"), MB_ICONEXCLAMATION|MB_OK);
        goto error;
    }
                
    // Invoke IBrowseHelper.BrowseTypeLibrary(szFileName). Returns ITypeLibrary.
    hr = Invoke(pdispBrowseHelper, DISPATCH_METHOD, &vRet, &excepinfo, &nArgErr,
                      OLESTR("BrowseTypeLibrary"), TEXT("s"), (LPOLESTR)TO_OLE_STRING(szFileName)); 
    if (FAILED(hr))
        goto error;  
    pdispTypeLibrary = V_DISPATCH(&vRet);  
    
    // Invoke ITypeLibrary.Name. Returns BSTR.
    hr = Invoke(pdispTypeLibrary, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Name"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    SetDlgItemText(m_hwndMain, IDC_TYPELIB_NAME, FROM_OLE_STRING(V_BSTR(&vRet)));
    VariantClear(&vRet);
    
    // Invoke ITypeLibrary.HelpFile. Returns BSTR.
    hr = Invoke(pdispTypeLibrary, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("HelpFile"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    SetDlgItemText(m_hwndMain, IDC_TYPELIB_HELPFILE, FROM_OLE_STRING(V_BSTR(&vRet)));
    if (NULL == V_BSTR(&vRet))
        m_szHelpFile[0] = '\0';
    else lstrcpy(m_szHelpFile, FROM_OLE_STRING(V_BSTR(&vRet))); 
    VariantClear(&vRet); 
    
    // Invoke ITypeLibrary.Documentation. Returns BSTR.
    hr = Invoke(pdispTypeLibrary, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Documentation"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    SetDlgItemText(m_hwndMain, IDC_TYPELIB_DOC, FROM_OLE_STRING(V_BSTR(&vRet)));
    VariantClear(&vRet);
    
    // Invoke ITypeLibrary.LocaleID. Returns VT_I4.
    hr = Invoke(pdispTypeLibrary, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("LocaleID"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    wsprintf(szTemp, TEXT("0x%lx"), V_I4(&vRet)); 
    SetDlgItemText(m_hwndMain, IDC_TYPELIB_LCID, szTemp);
    
    // Invoke ITypeLibrary.MajorVersion. Returns VT_I2.
    hr = Invoke(pdispTypeLibrary, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("MajorVersion"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    wsprintf(szTemp, TEXT("%d"), V_I2(&vRet));    
    // Invoke ITypeLibrary.MinorVersion. Returns VT_I2.
    hr = Invoke(pdispTypeLibrary, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("MinorVersion"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    wsprintf(szTemp, TEXT("%s.%d"), (LPSTR)szTemp, V_I2(&vRet)); 
    SetDlgItemText(m_hwndMain, IDC_TYPELIB_VERSION, szTemp);
    
    // Invoke ITypeLibrary.TypeInfos. Returns ICollection of ITypeInfo.
    hr = Invoke(pdispTypeLibrary, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("TypeInfos"), TEXT("")); 
    if (FAILED(hr))
        goto error;
    pdispTypeInfos = V_DISPATCH(&vRet);
    
    // Load TypeInfo list with the names of the type infos.
    hr = LoadList(pdispTypeInfos, IDC_TYPEINFOSLIST); 
    if (FAILED(hr))
        goto error;  
    
    pdispBrowseHelper->Release(); 
    pdispTypeLibrary->Release();
    pdispTypeInfos->Release();
    SetCursor(hcursorOld);
    return NOERROR;     

error:     
    if (pdispBrowseHelper) pdispBrowseHelper->Release();  
    if (pdispTypeLibrary) pdispTypeLibrary->Release();  
    if (pdispTypeInfos) pdispTypeLibrary->Release(); 
    VariantClear(&vRet); 
    SetCursor(hcursorOld);
    return hr;
}  

/*
 * CBrowseApp::ChangeTypeInfosSelection
 *
 * Purpose:
 *  Called when user changes selection in TypeInfos list box. Information 
 *  about the the selected TypeInfo is dispayed. Elements of the TypeInfo
 *  are placed in the Elements list box.
 *
 */
HRESULT CBrowseApp::ChangeTypeInfosSelection()
{   
    int nCurSel;
    VARIANT vRet;   
    EXCEPINFO excepinfo;
    UINT nArgErr; 
    HRESULT hr;   
    HCURSOR hcursorOld;
    LPDISPATCH pdispTypeInfo;  
    TYPEKIND typekind;   
    TCHAR szTemp[100]; 
    LPDISPATCH pdispElements = NULL;
    LPDISPATCH pdispMembers = NULL;
    LPDISPATCH pdispFunctions = NULL;
    LPDISPATCH pdispProperties = NULL;
    LPDISPATCH pdispMethods = NULL;
    LPDISPATCH pdispInterfaces = NULL;
    LPDISPATCH pdispTypeDesc = NULL;
     
    // Get current selection in TypeInfos list.
    nCurSel = (int)SendDlgItemMessage(m_hwndMain, IDC_TYPEINFOSLIST, LB_GETCURSEL, 0, 0L);
    if (nCurSel == LB_ERR) 
        return NOERROR; 
    // Get IDispatch* of selected TypeInfo.
    pdispTypeInfo = (LPDISPATCH) SendDlgItemMessage(m_hwndMain, IDC_TYPEINFOSLIST, LB_GETITEMDATA,
                (WPARAM)nCurSel, 0L);
    
    // Empty Elements & Parameters lists.
    EmptyList(GetDlgItem(m_hwndMain, IDC_ELEMENTSLIST));
    EmptyList(GetDlgItem(m_hwndMain, IDC_PARAMETERSLIST));     
    ClearElementStaticFields();
    ClearParamStaticFields();

    ClearTypeInfoStaticFields();
    
    hcursorOld = SetCursor(LoadCursor(NULL, IDC_WAIT));
    // Invoke ITypeInformation.HelpContext. Returns VT_I4.
    hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("HelpContext"), TEXT("")); 
    if (FAILED(hr))
        goto error;      
    wsprintf(szTemp, TEXT("%ld"), V_I4(&vRet)); 
    SetDlgItemText(m_hwndMain, IDC_TYPEINFO_HELPCTX, szTemp);
    
    // Invoke ITypeInformation.Documentation. Returns BSTR.
    hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Documentation"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    SetDlgItemText(m_hwndMain, IDC_TYPEINFO_DOC, FROM_OLE_STRING(V_BSTR(&vRet)));
    VariantClear(&vRet);
    
    // Invoke ITypeInformation.TypeInfoKind. Returns TYPEKIND.
    hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("TypeInfoKind"), TEXT("")); 
    if (FAILED(hr))
        goto error; 
    typekind = (TYPEKIND)V_I2(&vRet);  
    
    switch (typekind)
    {
        case TKIND_ENUM:
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_TYPE, TEXT("TKIND_ENUM"));
            // Invoke IEnum.Elements. Returns ICollection of IConstant.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Elements"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispElements = V_DISPATCH(&vRet);   
            // Put the Enumerator constants in the Elements list. 
            hr = LoadList(pdispElements, IDC_ELEMENTSLIST);  
            if (FAILED(hr))
                goto error; 
            pdispElements->Release();     
            break;   
        
        case TKIND_RECORD:        
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_TYPE, TEXT("TKIND_RECORD"));
            // Invoke IStruct.Members. Returns ICollection of IProperty.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Members"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispMembers = V_DISPATCH(&vRet);    
            // Put the Structure memebers in the Elements list.
            hr = LoadList(pdispMembers, IDC_ELEMENTSLIST);   
            if (FAILED(hr))
                goto error; 
            pdispMembers->Release(); 
            break; 
            
        case TKIND_MODULE:
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_TYPE, TEXT("TKIND_MODULE"));
            // Invoke IModule.Functions. Returns ICollection of IFunction.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Functions"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispFunctions = V_DISPATCH(&vRet); 
            // Put the Module functions in the Elements list.
            hr = LoadList(pdispFunctions, IDC_ELEMENTSLIST);         
            if (FAILED(hr))
                goto error; 
            pdispFunctions->Release(); 
            break;   
            
        case TKIND_INTERFACE:
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_TYPE, TEXT("TKIND_INTERFACE"));
            // Invoke IInterface.Functions. Returns ICollection of IFunction.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Functions"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispFunctions = V_DISPATCH(&vRet); 
            // Put the Interface functions in the Elements list.
            hr = LoadList(pdispFunctions, IDC_ELEMENTSLIST);
            if (FAILED(hr))
                goto error; 
            pdispFunctions->Release(); 
            break;  
            
        case TKIND_DISPATCH:
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_TYPE, TEXT("TKIND_DISPATCH"));
            // Invoke Dispinterface.Properties. Returns ICollection of IProperty.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Properties"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispProperties = V_DISPATCH(&vRet); 
            // Invoke IDispinterface.Methods. Returns ICollection of IFunction.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Methods"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispMethods = V_DISPATCH(&vRet);
            // Put the Dispinterface properties and methods in the Elements list.   
            hr = LoadList(pdispProperties, IDC_ELEMENTSLIST);  
            if (FAILED(hr))
                goto error; 
            pdispProperties->Release();  
            hr = LoadList(pdispMethods, IDC_ELEMENTSLIST); 
            if (FAILED(hr))
                goto error; 
            pdispMethods->Release(); 
            break; 
            
        case TKIND_COCLASS:
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_TYPE, TEXT("TKIND_COCLASS"));
            // Invoke ICoClass.Interfaces. Returns ICollection of IInterface & IDispinterface.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Interfaces"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispInterfaces = V_DISPATCH(&vRet);  
            // Put the CoClass interfaces and dispinterfaces in the Elements list.
            hr = LoadList(pdispInterfaces, IDC_ELEMENTSLIST); 
            if (FAILED(hr))
                goto error; 
            pdispInterfaces->Release();  
            break;        
            
        case TKIND_ALIAS:           
            // An alias does not have elements. Display the base type.
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_TYPE, TEXT("TKIND_ALIAS"));
            // Invoke IAlias.BaseType. Returns ITypeDesc.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("BaseType"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispTypeDesc = V_DISPATCH(&vRet);
            szTemp[0] = '\0'; // Must initialize to empty string before calling TypeToString.
            hr = TypeToString(pdispTypeDesc, szTemp);
            if (FAILED(hr))
                goto error; 
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_ALIASTYPE, szTemp);
            pdispTypeDesc->Release();  
            break;
            
        case TKIND_UNION:
            SetDlgItemText(m_hwndMain, IDC_TYPEINFO_TYPE, TEXT("TKIND_UNION"));
            // Invoke IUnion.Members. Returns ICollection of IProperty.
            hr = Invoke(pdispTypeInfo, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Members"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispMembers = V_DISPATCH(&vRet);     
            // Put the Union members in the Elements list.
            hr = LoadList(pdispMembers, IDC_ELEMENTSLIST);   
            if (FAILED(hr))
                goto error; 
            pdispMembers->Release();  
            break;   
    }
    SetCursor(hcursorOld);   
    return NOERROR;
error: 
    if (pdispElements) pdispElements->Release(); 
    if (pdispMembers) pdispMembers->Release(); 
    if (pdispFunctions) pdispFunctions->Release(); 
    if (pdispProperties) pdispProperties->Release(); 
    if (pdispMethods) pdispElements->Release(); 
    if (pdispInterfaces) pdispInterfaces->Release(); 
    if (pdispTypeDesc) pdispTypeDesc->Release();
    VariantClear(&vRet); 
    SetCursor(hcursorOld);
    return hr;
}

/*
 * CBrowseApp::ChangeElementsSelection
 *
 * Purpose:
 *  Called when user changes selection in Elements list box. Information 
 *  about the the selected Element is dispayed. If the element is a function,
 *  the parameters are placed in the the Parameters list box.
 *
 */
HRESULT CBrowseApp::ChangeElementsSelection()
{ 
    int nCurSel;
    VARIANT vRet;   
    EXCEPINFO excepinfo;
    UINT nArgErr; 
    HRESULT hr;     
    int nKind; 
    TYPEKIND typekind;
    LPDISPATCH pdispElement;   
    LPDISPATCH pdispParameters = NULL;
    LPDISPATCH pdispTypeDesc = NULL; 
    TCHAR szTemp[100];
    
    // Disable the help file button.
    EnableWindow(GetDlgItem(m_hwndMain, IDC_ELEM_OPEN_HELPFILE), FALSE); 
    
    // Get current selection in Elements list.
    nCurSel = (int)SendDlgItemMessage(m_hwndMain, IDC_ELEMENTSLIST, LB_GETCURSEL, 0, 0L);
    if (nCurSel == LB_ERR) 
        return NOERROR; 
    // Get IDispatch* of selected Element.
    pdispElement = (LPDISPATCH) SendDlgItemMessage(m_hwndMain, IDC_ELEMENTSLIST, LB_GETITEMDATA,
                (WPARAM)nCurSel, 0L);
    
    // Empty Parameters lists.
    EmptyList(GetDlgItem(m_hwndMain, IDC_PARAMETERSLIST));  
    ClearParamStaticFields();

    ClearElementStaticFields();
    
    // Invoke IFunction/IConstant/IProperty/IInterface/IDispinterface.HelpContext. Returns VT_I4.
    hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("HelpContext"), TEXT("")); 
    if (FAILED(hr))
        goto error;      
    wsprintf(szTemp, TEXT("%ld"), V_I4(&vRet)); 
    SetDlgItemText(m_hwndMain, IDC_ELEM_HELPCTX, szTemp);
    m_lElemHelpCtx = V_I4(&vRet); 
    
    // Invoke IFunction/IConstant/IProperty/IInterface/IDispinterface.Documentation. Returns BSTR.
    hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Documentation"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    SetDlgItemText(m_hwndMain, IDC_ELEM_DOC, FROM_OLE_STRING(V_BSTR(&vRet)));
    VariantClear(&vRet);
    
    // Invoke IFunction/IConstant/IProperty.MemberID. Returns VT_I4.
    hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("MemberID"), TEXT("")); 
    if (FAILED(hr))
    {
        // This maybe a interface/dispinterface element of a coclass   
        hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("TypeInfoKind"), TEXT("")); 
        if (FAILED(hr))
            goto error; 
        typekind = (TYPEKIND)V_I2(&vRet);  
        if (typekind == TKIND_INTERFACE)
            SetDlgItemText(m_hwndMain, IDC_ELEM_TYPE, TEXT("TKIND_INTERFACE"));
        else if (typekind == TKIND_DISPATCH)
            SetDlgItemText(m_hwndMain, IDC_ELEM_TYPE, TEXT("TKIND_DISPATCH"));  
        else SetDlgItemText(m_hwndMain, IDC_ELEM_TYPE, TEXT("Unknown element"));   
        return NOERROR;
    }
    wsprintf(szTemp, TEXT("0x%lx"), V_I4(&vRet));
    SetDlgItemText(m_hwndMain, IDC_ELEM_MEMID, szTemp);
    
    // Invoke IFunction/IConstant/IProperty.Kind property. Returns OBJKIND.
    hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Kind"), TEXT("")); 
    if (FAILED(hr))
        goto error;  
    nKind = V_I2(&vRet);  
    
    switch (nKind)
    {
        case TYPE_FUNCTION: //Function
            // Invoke IFunction.Parameters. Returns ICollection of IParameter..
            hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                        OLESTR("Parameters"), TEXT("")); 
            if (FAILED(hr))
                goto error; 
            pdispParameters = V_DISPATCH(&vRet); 
            hr = LoadList(pdispParameters, IDC_PARAMETERSLIST);   
            if FAILED(hr)
                goto error;
            pdispParameters->Release(); pdispParameters = NULL;
            
            // Invoke IFunction.CallConvention. Returns CALLCONV.
            hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("CallConvention"), TEXT("")); 
            if (FAILED(hr))
                goto error;   
            CallConvToString(V_I2(&vRet), szTemp);
            SetDlgItemText(m_hwndMain, IDC_ELEM_CALLCONV, szTemp); 
            
            // Invoke IFunction.FuncKind. Returns FUNCKIND.
            hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("FuncKind"), TEXT("")); 
            if (FAILED(hr))
                goto error;   
            FuncKindToString(V_I2(&vRet), szTemp);
            SetDlgItemText(m_hwndMain, IDC_ELEM_FUNCKIND, szTemp);
            
            // Invoke IFunction.InvocationKind. Returns INVOKEKIND.
            hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("InvocationKind"), TEXT("")); 
            if (FAILED(hr))
                goto error;   
            InvokeKindToString(V_I2(&vRet), szTemp);
            SetDlgItemText(m_hwndMain, IDC_ELEM_INVOKEKIND, szTemp);   
            
            // Invoke IFunction.ReturnType. Returns ITypeDesc.
            hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("ReturnType"), TEXT("")); 
            if (FAILED(hr))
                goto error;           
            break;    
            
        case TYPE_CONSTANT:
            // Invoke IConstant.Value. Returns VARIANT.
            hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Value"), TEXT("")); 
            if (FAILED(hr))
                goto error;   
            VariantToString(vRet, szTemp);
            SetDlgItemText(m_hwndMain, IDC_ELEM_CONST_VALUE, szTemp);
            // Fall through to default.     
            
        default: 
            // Invoke IConstant/IProperty.Type. Returns ITypeDesc.
            hr = Invoke(pdispElement, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Type"), TEXT("")); 
            if (FAILED(hr))
                goto error;  
            break;
    }   
    // Get string that describes type and display it.
    pdispTypeDesc = V_DISPATCH(&vRet);  
    szTemp[0] = '\0'; // Must initialize to empty string before calling TypeToString.
    hr = TypeToString(pdispTypeDesc, szTemp);
    if (FAILED(hr))
        goto error;
    SetDlgItemText(m_hwndMain, IDC_ELEM_TYPE, szTemp);
    pdispTypeDesc->Release(); 
    
    // Enable the helpfile button if a helpfile and help context is specified.
    if (m_lElemHelpCtx && m_szHelpFile[0])
        EnableWindow(GetDlgItem(m_hwndMain, IDC_ELEM_OPEN_HELPFILE), TRUE);
    return NOERROR;
error: 
    if (pdispParameters) pdispParameters->Release();  
    VariantClear(&vRet);
    return hr; 
}

/*
 * CBrowseApp::ChangeParametersSelection
 *
 * Purpose:
 *  Called when user changes selection in Parameters list box. Information 
 *  about the the selected Parameter is dispayed.
 *
 */
HRESULT CBrowseApp::ChangeParametersSelection()
{   
    int nCurSel;
    VARIANT vRet;   
    EXCEPINFO excepinfo;
    UINT nArgErr; 
    HRESULT hr;
    LPDISPATCH pdispParameter;
    LPDISPATCH pdispTypeDesc = NULL; 
    TCHAR szTemp[100];
     
    // Get current selection in Parameters list.
    nCurSel = (int)SendDlgItemMessage(m_hwndMain, IDC_PARAMETERSLIST, LB_GETCURSEL, 0, 0L);
    if (nCurSel == LB_ERR) 
        return NOERROR; 
    // Get IDispatch* of selected Parameter.
    pdispParameter = (LPDISPATCH) SendDlgItemMessage(m_hwndMain, IDC_PARAMETERSLIST, LB_GETITEMDATA,
                (WPARAM)nCurSel, 0L);  
    
    ClearParamStaticFields();
    
    // Invoke IParameter.IDLFlags. Returns VT_I2.
    hr = Invoke(pdispParameter, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("IDLFlags"), TEXT("")); 
    if (FAILED(hr))
        goto error; 
    IDLFlagsToString(V_I2(&vRet), szTemp); 
    SetDlgItemText(m_hwndMain, IDC_PARAM_INOUT, szTemp);
                    
    // Invoke IParameter.Type. Returns ITypeDesc.
    hr = Invoke(pdispParameter, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Type"), TEXT("")); 
    if (FAILED(hr))
        goto error;
    
   // Get string that describes type and display it.
    pdispTypeDesc = V_DISPATCH(&vRet);  
    szTemp[0] = '\0'; // Must initialize to empty string before calling TypeToString.
    hr = TypeToString(pdispTypeDesc, szTemp);     
    if (FAILED(hr))
        goto error;
    SetDlgItemText(m_hwndMain, IDC_PARAM_TYPE, szTemp);
    pdispTypeDesc->Release();
    return NOERROR;
error: 
    if (pdispTypeDesc) pdispTypeDesc->Release();
    VariantClear(&vRet);
    return hr; 
} 

/*
 * CBrowseApp::OpenElementHelpFile
 *
 * Purpose:
 *  Opens the helpfile for the element.
 *
 */
void CBrowseApp::OpenElementHelpFile()
{
    BOOL b;
    
    b = WinHelp(m_hwndMain, m_szHelpFile, HELP_CONTEXT, m_lElemHelpCtx);
    if (!b)
        MessageBox(m_hwndMain, TEXT("Failed to open help file"), TEXT("Error"), 
            MB_ICONEXCLAMATION|MB_OK);      
}

void CBrowseApp::EmptyList(HWND hwndList)
{
    int nItems, i;
    LPDISPATCH pdispItem;

    nItems = (int)SendMessage(hwndList, LB_GETCOUNT, 0, 0L);
    for (i=0; i<nItems; i++)
    {
        pdispItem = (LPDISPATCH)SendMessage(hwndList, LB_GETITEMDATA, (WPARAM)i, 0L); 
        if (pdispItem)
            pdispItem->Release();
    }
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);
} 

/*
 * CBrowseApp::LoadList  
 *
 * Parameters:
 *  pdispItems IDispatch* of collection of items to be put into the list.
 *  nListID    Identifies the list to be filled.
 *
 * Purpose:
 *  Loads the list with the items in the collection.
 *
 */
HRESULT CBrowseApp::LoadList(LPDISPATCH pdispItems, int nListID)
{   
    VARIANT vRet, v;  
    HRESULT hr;  
    EXCEPINFO excepinfo;
    UINT nArgErr;   
    LPUNKNOWN punkEnum;
    IEnumVARIANT FAR* penum = NULL;  
    LPDISPATCH pdispItem = NULL;   
    int nIndex;
    
    // Get _NewEnum property. Returns enumerator's IUnknown.
    hr = Invoke(pdispItems, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                        OLESTR("_NewEnum"), TEXT("")); 
    if (FAILED(hr))
        goto error;         
    punkEnum = V_UNKNOWN(&vRet);
    hr = punkEnum->QueryInterface(IID_IEnumVARIANT, (LPVOID FAR*)&penum);
    if (FAILED(hr))
        goto error;
    punkEnum->Release();

    VariantInit(&v);
    // Enumerate the Items.
    while (S_OK == penum->Next(1, &v, NULL))
    {
        pdispItem = V_DISPATCH(&v);
        pdispItem->AddRef();
        VariantClear(&v);
        
        // Get Name of Item.
        hr = Invoke(pdispItem, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr,
                    OLESTR("Name"), TEXT("")); 
        if (FAILED(hr))
            goto error;             
        // Add name to ItemsList.
        nIndex = (int)SendDlgItemMessage(m_hwndMain, nListID, LB_ADDSTRING,
            0, (LPARAM)(LPTSTR)FROM_OLE_STRING(V_BSTR(&vRet))); 
        VariantClear(&vRet);          
        // Save IDispatch* of Item in the list.
        SendDlgItemMessage(m_hwndMain, nListID, LB_SETITEMDATA,
            nIndex, (LPARAM)pdispItem);
    }
    penum->Release();
    return NOERROR;     

error:   
    if (penum) penum->Release();  
    if (pdispItem) pdispItem->Release();
    return hr;
}  

void CBrowseApp::Init(HWND hwndMain) 
{   
    HFONT hfontDlg;   
    LOGFONT lfont;
    int nID;
    
    m_hwndMain = hwndMain;
    m_hfont = NULL;
#ifdef WIN16   
    m_hinst = (HINSTANCE)GetWindowWord(hwndMain, GWW_HINSTANCE); 
#else
    m_hinst = (HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE);
#endif     
    EnableWindow(GetDlgItem(m_hwndMain, IDC_ELEM_OPEN_HELPFILE), FALSE);
    
    // Change to the font of the fields to a non-bold font.
    hfontDlg = (HFONT)SendMessage(m_hwndMain, WM_GETFONT, NULL, NULL);  
    if (!hfontDlg)
        return;
    GetObject(hfontDlg, sizeof(LOGFONT), (LPVOID)&lfont);            
    lfont.lfWeight = FW_NORMAL;    
    if (m_hfont = CreateFontIndirect(&lfont)) 
        for (nID=IDC_TYPELIB_FIRST; nID<=IDC_PARAM_LAST; nID++) 
            SendDlgItemMessage(m_hwndMain, nID, WM_SETFONT, (WPARAM)m_hfont, 0L);
        
} 

void CBrowseApp::Cleanup() 
{
    EmptyList(GetDlgItem(m_hwndMain, IDC_TYPEINFOSLIST)); 
    EmptyList(GetDlgItem(m_hwndMain, IDC_ELEMENTSLIST));
    EmptyList(GetDlgItem(m_hwndMain, IDC_PARAMETERSLIST));
    ClearTypeLibStaticFields();
    ClearTypeInfoStaticFields();     
    ClearElementStaticFields();
    ClearParamStaticFields();
    if (m_hfont)
        DeleteObject(m_hfont);  
} 

void CBrowseApp::ClearTypeLibStaticFields()
{   
    int nID;
    
    for (nID=IDC_TYPELIB_FIRST; nID<=IDC_TYPELIB_LAST; nID++)
        SetDlgItemText(m_hwndMain, nID, TEXT(""));
}
void CBrowseApp::ClearTypeInfoStaticFields()
{   
    int nID;
    
    for (nID=IDC_TYPEINFO_FIRST; nID<=IDC_TYPEINFO_LAST; nID++)
        SetDlgItemText(m_hwndMain, nID, TEXT(""));
}     
void CBrowseApp::ClearElementStaticFields()
{ 
    int nID;
    
    for (nID=IDC_ELEM_FIRST; nID<=IDC_ELEM_LAST; nID++)
        SetDlgItemText(m_hwndMain, nID, TEXT(""));
}
void CBrowseApp::ClearParamStaticFields()
{  
    int nID;
    
    for (nID=IDC_PARAM_FIRST; nID<=IDC_PARAM_LAST; nID++)
        SetDlgItemText(m_hwndMain, nID, TEXT(""));
} 

#define CASE_VT(vt)  \
        case vt: \
            lstrcat(pszTypeName, TEXT(#vt)); \
            break;
HRESULT CBrowseApp::TypeToString(LPDISPATCH pdispTypeDesc, LPTSTR pszTypeName)
{  
    VARIANT vRet;
    VARTYPE vartype;  
    HRESULT hr;  
    EXCEPINFO excepinfo;
    UINT nArgErr;  
    LPDISPATCH pdispTypeDesc2; 
    
    // Get Type property  
    hr = Invoke(pdispTypeDesc, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Type"), TEXT("")); 
    if (FAILED(hr))
        goto error;   
    vartype = V_I2(&vRet); 
    
    if (vartype & 0x2000)  // If SafeArray
      lstrcat(pszTypeName, TEXT("SAFEARRAY(")); 
    
    switch (vartype & ~0x7000)
    { 
    CASE_VT(VT_EMPTY)
    CASE_VT(VT_NULL)
    CASE_VT(VT_I2)
    CASE_VT(VT_I4)
    CASE_VT(VT_R4)
    CASE_VT(VT_R8)
    CASE_VT(VT_CY)
    CASE_VT(VT_DATE)
    CASE_VT(VT_BSTR)
    CASE_VT(VT_DISPATCH)
    CASE_VT(VT_ERROR)
    CASE_VT(VT_BOOL)
    CASE_VT(VT_VARIANT)
    CASE_VT(VT_UNKNOWN)

    CASE_VT(VT_I1)
    CASE_VT(VT_UI1)
    CASE_VT(VT_UI2)
    CASE_VT(VT_UI4)
    CASE_VT(VT_I8)
    CASE_VT(VT_UI8)
    CASE_VT(VT_INT)
    CASE_VT(VT_UINT)
    CASE_VT(VT_VOID)
    CASE_VT(VT_HRESULT)
    CASE_VT(VT_SAFEARRAY)  
    CASE_VT(VT_CARRAY)
    CASE_VT(VT_LPSTR)
    CASE_VT(VT_LPWSTR)

    CASE_VT(VT_FILETIME)
    CASE_VT(VT_BLOB)
    CASE_VT(VT_STREAM)
    CASE_VT(VT_STORAGE)
    CASE_VT(VT_STREAMED_OBJECT)
    CASE_VT(VT_STORED_OBJECT)
    CASE_VT(VT_BLOB_OBJECT)
    CASE_VT(VT_CF)
    CASE_VT(VT_CLSID)
   
    case VT_PTR:    
         // Get ITypeDesc.PointerDesc property. Returns ITypeDesc.  
         hr = Invoke(pdispTypeDesc, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("PointerDesc"), TEXT("")); 
         if (FAILED(hr))
             goto error;  
         pdispTypeDesc2 = V_DISPATCH(&vRet);
         TypeToString(pdispTypeDesc2, pszTypeName);
         lstrcat(pszTypeName, TEXT(" *"));
         pdispTypeDesc2->Release();
         break;

    case VT_USERDEFINED:   
         // Get ITypeDesc.UserDefinedDesc property. Returns ITypeInformation 
         hr = Invoke(pdispTypeDesc, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("UserDefinedDesc"), TEXT("")); 
         if (FAILED(hr))
             goto error;  
         pdispTypeDesc2 = V_DISPATCH(&vRet);
         // Get ITypeInformation.Name property. Returns BSTR.  
         hr = Invoke(pdispTypeDesc2, DISPATCH_PROPERTYGET, &vRet, &excepinfo, &nArgErr, 
                OLESTR("Name"), TEXT("")); 
         if (FAILED(hr))
             goto error;  
         lstrcat(pszTypeName, FROM_OLE_STRING(V_BSTR(&vRet))); 
         VariantClear(&vRet);
         pdispTypeDesc2->Release();
         break;
    }    
    
    if (vartype & 0x2000) // If SafeArray
        lstrcat(pszTypeName, TEXT(")"));  
      
    return NOERROR;
error:
    return ERROR;
}   

void CBrowseApp::IDLFlagsToString(int n, LPTSTR psz)
{  
   psz[0] = '\0';
   if (n & IDLFLAG_FIN)
   {
       lstrcpy(psz, TEXT("IN")); 
       if (n & IDLFLAG_FOUT)
           lstrcat(psz, TEXT("|OUT"));
   }
   else if (n & IDLFLAG_FOUT)
       lstrcpy(psz, TEXT("OUT"));    
}

#define CASE_INVOKE(invokekind)  \
        case invokekind: \
            lstrcpy(psz, TEXT(#invokekind)); \
            break;   
void CBrowseApp::CallConvToString(int n, LPTSTR psz)   
{
    CALLCONV c = (CALLCONV)n;
    switch (c)
    {
        CASE_INVOKE(CC_CDECL)
        CASE_INVOKE(CC_PASCAL)
        CASE_INVOKE(CC_MACPASCAL)
        CASE_INVOKE(CC_STDCALL)
        CASE_INVOKE(CC_SYSCALL)
        default:
            lstrcpy(psz, TEXT("Unknown"));          
    }
}

#define CASE_FUNC(funckind)  \
        case funckind: \
            lstrcpy(psz, TEXT(#funckind)); \
            break;   
void CBrowseApp::FuncKindToString(int n, LPTSTR psz)    
{     
    FUNCKIND f = (FUNCKIND)n;
    switch (f)
    {
        CASE_FUNC(FUNC_VIRTUAL)
        CASE_FUNC(FUNC_PUREVIRTUAL)
        CASE_FUNC(FUNC_NONVIRTUAL)
        CASE_FUNC(FUNC_STATIC)
        CASE_FUNC(FUNC_DISPATCH)
        default:
            lstrcpy(psz, TEXT("Unknown"));          
    }
} 

#define CASE_INVOKE(invokekind)  \
        case invokekind: \
            lstrcpy(psz, TEXT(#invokekind)); \
            break;   
void CBrowseApp::InvokeKindToString(int n, LPTSTR psz)  
{  
    INVOKEKIND i = (INVOKEKIND)n;
    switch (i)
    {
        CASE_INVOKE(INVOKE_FUNC)
        CASE_INVOKE(INVOKE_PROPERTYGET)
        CASE_INVOKE(INVOKE_PROPERTYPUT)
        CASE_INVOKE(DISPATCH_PROPERTYPUTREF)
        default:
            lstrcpy(psz, TEXT("Unknown"));          
    }
}
void CBrowseApp::VariantToString(VARIANT v, LPTSTR psz) 
{   
    HRESULT hr;
    VARIANT vTemp;
    
    VariantInit(&vTemp);
    hr = VariantChangeType(&vTemp, &v, 0, VT_BSTR);
    if (FAILED(hr))
        lstrcpy(psz, TEXT("Cannot Display Value")); 
    else lstrcpy(psz, FROM_OLE_STRING(V_BSTR(&vTemp))); 
    VariantClear(&vTemp);
}

#ifdef WIN32

#ifndef UNICODE
char* ConvertToAnsi(OLECHAR FAR* szW)
{
  static char achA[STRCONVERT_MAXLEN]; 
  
  WideCharToMultiByte(CP_ACP, 0, szW, -1, achA, STRCONVERT_MAXLEN, NULL, NULL);  
  return achA; 
} 

OLECHAR* ConvertToUnicode(char FAR* szA)
{
  static OLECHAR achW[STRCONVERT_MAXLEN]; 

  MultiByteToWideChar(CP_ACP, 0, szA, -1, achW, STRCONVERT_MAXLEN);  
  return achW; 
}
#endif

#endif   
