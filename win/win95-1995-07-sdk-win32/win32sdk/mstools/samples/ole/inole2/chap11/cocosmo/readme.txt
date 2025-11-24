To build a version of Component Cosmo that uses POLY11.DLL, copy
the sources from CHAP08\COCOSMO and modify the function
CCosmoDoc::FInit in DOCUMENT.CPP to use CLSID_Polyline11 instead
of CLSID_Polyline6 in the CoCreateInstance call:

BOOL CCosmoDoc::FInit(PDOCUMENTINIT pDI)
    {
    RECT            rc;
    HRESULT         hr;
    FORMATETC       fe;
    LPDATAOBJECT    pIDataObject;

    //Change the stringtable range to our customization.
    pDI->idsMin=IDS_DOCUMENTMIN;
    pDI->idsMax=IDS_DOCUMENTMAX;

    //Do default initialization
    if (!CDocument::FInit(pDI))
        return FALSE;

    //Create the Polyline Object via COMPOBJ.DLL functions.
    hr=CoCreateInstance(CLSID_Polyline11, NULL, CLSCTX_INPROC_SERVER
        , IID_IPolyline6, (PPVOID)&m_pPL);


    ...
    }


Nothing else in CoCosmo needs to change to use the revised Polyline.
Note that CoCosmo still uses the IPolyline6 interface on the Polyline
object.  For all intents and purposes, nothing should change at all
in the execution of CoCosmo, demonstrating how interfaces do not
interfere with each other:  CoCosmo ignores the compound document
interfaces.
