#include "dadrag.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys\timeb.h>
#include <tchar.h>


class CDAViewerCtl {
  
  public:
    // Construction of the control.  We use COM smart pointers in the
    // viewer control, the pointers are released when they're out of the
    // scope, so we don't need (the destructor) to release them.
    CDAViewerCtl();

    // For constructing the model.
    void CreateModel();

    // Return the IUnknown pointer of the control.
    HRESULT GetIUnknown(IUnknown **pUnk);

  private:
    IDAViewerControlPtr _vc;  
};
