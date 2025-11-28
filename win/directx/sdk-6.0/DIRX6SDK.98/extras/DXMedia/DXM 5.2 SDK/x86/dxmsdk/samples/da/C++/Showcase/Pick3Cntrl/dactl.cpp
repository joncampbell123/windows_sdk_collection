#include "dactl.h"

void dump_com_error( _com_error &e )
{
    char buf[2048];

    sprintf(buf, _T( "Oops - hit an error!\n\tCode = %08lx\n\tCode meaning = %s\n" ),
            e.Error(), e.ErrorMessage());
    OutputDebugString(buf);
}

// If this is placed in the scope of the smart pointers, they must be
// explicitly Release(d) before CoUninitialize() is called.  If any reference
// count is non-zero, a protection fault will occur.

CDAViewerCtl::CDAViewerCtl()
    :_vc(NULL)
{
    try   {
        _vc.CreateInstance(__uuidof(DAViewerControlWindowed));
    } catch( _com_error &e ) {
        dump_com_error( e );
    }
}

void CDAViewerCtl::CreateModel() {
    try {
        // Create the statics object
        IDAStaticsPtr e;
        e = _vc->GetMeterLibrary();

        // Import Media (geometries, images and in this case).  
        // The GetCurrentDirectory() is used as a starting
        // point for relative file importing.
        TCHAR szMediaBase[_MAX_PATH];
        TCHAR szImg[_MAX_PATH];
        TCHAR szGeo[_MAX_PATH];
        TCHAR szSnd[_MAX_PATH];

        GetModuleFileName(GetModuleHandle("Pick3Cntrl.exe"),
          szMediaBase,sizeof(szMediaBase));
        char *pos = strrchr( szMediaBase, (int)'\\' );
        int result = pos - szMediaBase + 1;
        szMediaBase[result]= NULL;

        _tcscat(szMediaBase,_T("../../../../../media/"));

        _tcscpy(szImg,szMediaBase);
        _tcscpy(szGeo,szMediaBase);
        _tcscpy(szSnd,szMediaBase);

        _tcscat(szImg,_T("image/"));
        _tcscat(szGeo,_T("geometry/"));
        _tcscat(szSnd,_T("sound/"));

        // Define constants
        SIZE = e->Scale3Uniform(0.25);
        PICKEVENTL = e->LeftButtonDown;
        PICKEVENTR = e->RightButtonDown;
        SPEED = e->DANumber(0.07);
        PI = 3.14159265359;

        // import background.
        IDAImagePtr stillSky = e->ImportImage(_bstr_t(szImg) + _bstr_t("cldtile.jpg"));

        IDAPoint2Ptr maxSky = stillSky->BoundingBox->Max;
        IDAImagePtr tiledSky = stillSky->Tile();
        IDAImagePtr movingSky = tiledSky->Transform(e->Translate2Anim(e->Mul(e->LocalTime,
          e->Div(maxSky->X,e->DANumber(8))), e->Mul(e->LocalTime,e->Div(maxSky->X,e->DANumber(16)))));

        // Import the geometries.
        IDAGeometryPtr rawCube = 
        e->ImportGeometry( _bstr_t(szGeo) + _bstr_t( "cube.x" ) );
        rawCube = rawCube->Transform(SIZE);

        IDAGeometryPtr rawCylinder = 
        e->ImportGeometry(_bstr_t(szGeo) + _bstr_t("cylinder.x"));
        rawCylinder = rawCylinder->Transform(SIZE);

        IDAGeometryPtr rawCone = 
        e->ImportGeometry(_bstr_t(szGeo) + _bstr_t("cone.x"));
        rawCone = rawCone->Transform(SIZE);

        // Make the geometries pickable.
        IDAGeometryPtr cone1 = activate(rawCone, e->Green, e);

        IDAGeometryPtr cube1 = activate(rawCube, e->Magenta, e);

        IDAGeometryPtr cube2 = activate(rawCube, e->ColorHslAnim(e->Div(e->LocalTime,
          e->DANumber(8)), e->DANumber(1), e->DANumber(0.5)), e);

        IDAGeometryPtr cylinder = activate(rawCylinder, e->ColorRgb(0.8,0.4,0.4), e);

        // Construct the final geometry, scale and rotate it.
        IDAGeometryPtr multigeo = e->UnionGeometry(cone1->Transform(e->Translate3(0,1,0)),
          e->UnionGeometry(cube1->Transform(e->Translate3(0,0,1)),
            e->UnionGeometry(cube2->Transform(e->Translate3(0,0,-1)),cylinder)));

        IDAGeometryPtr geo = multigeo->Transform(e->Scale3Anim(e->Add(e->Abs(e->Sin(e->Mul(e->LocalTime,
          e->DANumber(0.2)))),e->DANumber(0.5)),e->Add(e->Abs(e->Sin(e->Mul(e->LocalTime,
            e->DANumber(0.26)))),e->DANumber(0.5)),e->Add(e->Abs(e->Sin(e->Mul(e->LocalTime,
              e->DANumber(0.14)))),e->DANumber(0.5))));

        IDATransform3Ptr transform1 = e->Rotate3Anim(e->ZVector3,
          e->Mul(e->DANumber(0.07), e->Mul(e->LocalTime, e->DANumber(1.9))));

        IDATransform3Ptr transform2 = e->Rotate3Anim(e->YVector3,
          e->Mul(e->DANumber(0.07), e->Mul(e->LocalTime, e->DANumber(PI))));

        IDAImagePtr movingGeoImg = geometryImage(geo->Transform(e->Compose3(e->Rotate3Anim(e->ZVector3,
          e->Mul(e->DANumber(0.07),e->Mul(e->LocalTime,e->DANumber(1.9)))), e->Rotate3Anim(e->YVector3,
            e->Mul(e->DANumber(0.07),e->Mul(e->LocalTime,e->DANumber(PI)))))), e);

        IDAFontStylePtr fs = e->DefaultFont->Color(e->Black);
        IDAImagePtr titleIm = e->StringImage("Left Click On An Object", 
          fs)->Transform(e->Translate2(0,0.03));

        IDAImagePtr model = e->Overlay( titleIm, e->Overlay( movingGeoImg, movingSky ) );
        
        // And set the model's image to this image.
        _vc->PutImage( model );

        // Set the cap for the frame rate.  If we don't do this, DA
        // will hog the cpu and the mouse and keyboard won't be very
        // responsive.  If you're running in full screen mode, you may
        // want to remove this line to get better frame rate.
        _vc->put_UpdateInterval(0.2);

        // Start the model on the view.  The WndProc will
        // generate the frames.
        _vc->Start();

    } catch( _com_error &e ) {
        dump_com_error( e );
    }
}

IDAGeometryPtr CDAViewerCtl::activate(IDAGeometryPtr unpickedGeo, 
                                      IDAColorPtr col, IDAStaticsPtr e)  {
  IDAPickableResultPtr pickGeo = unpickedGeo->Pickable();
  IDAEventPtr pickEvent = e->AndEvent(PICKEVENTL, pickGeo->PickEvent);

  IDANumberPtr numcyc;
  numcyc.CreateInstance( L"DirectAnimation.DANumber");
  numcyc->Init(e->Until(e->DANumber(0),pickEvent,
    e->Until(e->DANumber(1), pickEvent, numcyc)));

  IDAColorPtr colcyc;
  colcyc.CreateInstance( L"DirectAnimation.DAColor");
  colcyc->Init(e->Until(e->White, pickEvent, e->Until(col, pickEvent, colcyc)));

  IDATransform3Ptr xf = e->Rotate3Anim(e->XVector3, e->Integral(numcyc));

  return pickGeo->Geometry->DiffuseColor(colcyc)->Transform(xf);

}

IDAImagePtr CDAViewerCtl::geometryImage(IDAGeometryPtr geo, IDAStaticsPtr e)  {
  IDANumberPtr scaleFactor = e->DANumber(0.02);

  IDATransform3Ptr perspTransform;
  perspTransform.CreateInstance( L"DirectAnimation.DATransform3");
  perspTransform->Init(e->Until(e->Compose3(e->Rotate3Anim(e->XVector3,
    e->Mul(SPEED,e->LocalTime)),e->Translate3(0, 0, 0.2)),PICKEVENTR, 
      e->Until(e->Rotate3Anim(e->XVector3, e->Mul(SPEED,e->LocalTime)),
        PICKEVENTR, perspTransform)));

  IDAGeometryPtr myLight = 
    e->UnionGeometry(e->DirectionalLight->Transform(perspTransform), e->DirectionalLight);

  IDACameraPtr perspectiveCam = (e->PerspectiveCamera(1,0))->Transform(e->Compose3(e->Rotate3Anim(e->XVector3,
    e->Mul(SPEED,e->LocalTime)),e->Translate3(0,0,0.2)));

  IDACameraPtr parallelCam = (e->ParallelCamera(1))->Transform(e->Rotate3Anim(e->XVector3,
    e->Mul(SPEED,e->LocalTime)));

  IDACameraPtr camera;
  camera.CreateInstance( L"DirectAnimation.DACamera");
  camera->Init(e->Until(perspectiveCam, PICKEVENTR,
    e->Until(parallelCam, PICKEVENTR, camera)));

  // Display text which tells the user what camera is currently being used.
  IDAStringPtr camText;
  camText.CreateInstance( L"DirectAnimation.DAString");
  camText->Init(e->Until(e->DAString("Perspective - Right Click to Switch"), PICKEVENTR,
    e->Until(e->DAString("Parallel - Right Click to Switch"),PICKEVENTR, camText)));

  IDAFontStylePtr fs = e->DefaultFont->Color(e->Red);
  IDAImagePtr camIm  = e->StringImageAnim(camText, fs);
  camIm = camIm->Transform(e->Translate2(0, -0.03));

  return e->Overlay(camIm, e->UnionGeometry(geo->Transform(e->Scale3UniformAnim(scaleFactor)),
    myLight)->Render(camera));
}

HRESULT CDAViewerCtl::GetIUnknown(IUnknown **pUnk) {
    if (!pUnk)
        return E_POINTER;

    if (_vc == NULL)
        return E_NOINTERFACE;

    return _vc->QueryInterface(IID_IUnknown, (LPVOID *)pUnk);
}
