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

        CDADrag* _dragPtr = new CDADrag();

        // Create a ModifiableBehavior which will be used to change the color
        // of the square.
        IDABehaviorPtr _squareColor = e->ModifiableBehavior(e->Red);

        // Instanciate an uninitialized ColorBvr (_squareColor).
        IDAColorPtr temp;
        temp.CreateInstance( L"DirectAnimation.DAColor");

        // Initialize _squareColor.  Let it start out as red, change it to blue,
        // when the square is grabbed, and return to red when the square
        // is released.  The grab and release events are obtained from the
        // getGrabEvent() and getReleaseEvent() from the CDADrag 
        // class.
        //_squareColor->Init(e->Until(e->Red, _dragPtr->getGrabEvent(), 
        //  e->Until(e->Blue, _dragPtr->getReleaseEvent(), _squareColor)));

        // Create cropped square, and apply _squareColor's color behavior to it. 
        _blockImg = e->SolidColorImage((IDAColorPtr)_squareColor)->Crop(e->Point2(0,0),
          e->Point2(0.005,0.005));
    
        _dragPtr->initNotify(_blockImg, e->Origin2, e);

        // Initialize squareClr.  Let it start out as red, change it to blue,
        // when the cube is grabbed, and return to red when the cube
        // is released.  The grab and release events are obtained from the
		    // getGrabEvent() and getReleaseEvent() methods of the CDADrag
		    // class respectively.
        IDAColorPtr squareClr;
        squareClr.CreateInstance( L"DirectAnimation.DAColor");
        squareClr->Init(e->Until(e->Red, _dragPtr->getGrabEvent(), 
			    e->Until(e->Blue, _dragPtr->getReleaseEvent(), squareClr)));

        // Apply squareClr's behavior to the square.
        _squareColor->SwitchTo(squareClr);

        // Get the ImageBvr part of grabImg, by calling the getImagePtr() method
        // of DraggableImage.
        IDAImagePtr pickableBlockImg = _dragPtr->getImagePtr();

        // overlay pickableBlockImg on a black background.
        IDAImagePtr model = e->Overlay(pickableBlockImg, e->SolidColorImage(e->Black));
        
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


HRESULT CDAViewerCtl::GetIUnknown(IUnknown **pUnk) {
    if (!pUnk)
        return E_POINTER;

    if (_vc == NULL)
        return E_NOINTERFACE;

    return _vc->QueryInterface(IID_IUnknown, (LPVOID *)pUnk);
}
