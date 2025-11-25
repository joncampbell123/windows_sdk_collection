/************************************************************************************************************

  Direct3DRM and DirectSound 3D interface designed for the Viewer Sample Application

  (c) 1996 Microsoft Corporation

************************************************************************************************************/

#ifndef ___DS3DVIEWER_INTERNAL
#define ___DS3DVIEWER_INTERNAL

// "Internal" structure definitions and other include information used by
// the DS3DVIEW application.  We put this stuff here so other modules can
// access it without great pain.

// Info for the 3D3RM  (RL info)
typedef struct _AppInfo
{   
        // The parent frame 
        LPDIRECT3DRMFRAME scene;
        // The frame for the camera (a child of the scene)
        LPDIRECT3DRMFRAME camera;
        
        // Device for Windows
    LPDIRECT3DRMDEVICE dev;
        // Defines how the 3D scene is rendered into the 2D window
    LPDIRECT3DRMVIEWPORT view;
        
        // Defines which color model (RGB/Ramp) is used for rendering - used when creating a device
        //  from the RL api
    D3DRMCOLORMODEL model;

        // Whether or not the window is minimized (modified by the resizing handler)
        BOOL bMinimized;
} AppInfo;



/*
** Error Checking code
*/
BOOL D3DRM_SUCCEED(HRESULT result, int force_critical = -1, char* info = NULL);
BOOL DS3D_SUCCEED(HRESULT result, int force_critical = -1, char* info = NULL);


/****** DirectSound3D Helper Stuff ************************************/

// Simple thing to copy vectors to each other independant of type
#define RLV_TO_DSV(in,out) out.x = in.x;\
                                                   out.y = in.y;\
                                                   out.z = in.z;


#endif



