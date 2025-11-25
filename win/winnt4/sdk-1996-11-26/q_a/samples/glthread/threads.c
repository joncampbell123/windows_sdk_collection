//****************************************************************************
// File: glthread.c
//
// Purpose: Contains code to set up the pixel format and initialize OpenGL.  
//          Also contains code to show how to implement two threads in an
//          OpenGL application.
//
// Development Team:
//         Greg Binkerd - Windows Developer Support
//
// Written by Microsoft Windows Developer Support
// Copyright (c) 1995-1996 Microsoft Corporation. All rights reserved.
//****************************************************************************

#include "glthread.h"
#include <math.h>

// Threads that will perform the OpenGL rendering
HANDLE hThreads[2];
// Handle to main window
extern HWND ghWnd;

HPALETTE ghpalOld, ghPalette = (HPALETTE) 0;
GLfloat radius;
RECT    oldrect;

unsigned char threeto8[8] = {
    0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

unsigned char twoto8[4] = {
    0, 0x55, 0xaa, 0xff
};

unsigned char oneto8[2] = {
    0, 255
};

static int defaultOverride[13] = {
    0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

static PALETTEENTRY defaultPalEntry[20] = {
    { 0,   0,   0,    0 },
    { 0x80,0,   0,    0 },
    { 0,   0x80,0,    0 },
    { 0x80,0x80,0,    0 },
    { 0,   0,   0x80, 0 },
    { 0x80,0,   0x80, 0 },
    { 0,   0x80,0x80, 0 },
    { 0xC0,0xC0,0xC0, 0 },

    { 192, 220, 192,  0 },
    { 166, 202, 240,  0 },
    { 255, 251, 240,  0 },
    { 160, 160, 164,  0 },

    { 0x80,0x80,0x80, 0 },
    { 0xFF,0,   0,    0 },
    { 0,   0xFF,0,    0 },
    { 0xFF,0xFF,0,    0 },
    { 0,   0,   0xFF, 0 },
    { 0xFF,0,   0xFF, 0 },
    { 0,   0xFF,0xFF, 0 },
    { 0xFF,0xFF,0xFF, 0 }
};

//****************************************************************************
// Function: ComponentFromIndex
//
//
// Comments: Taken from the "GenGL" OpenGL sample application
//
//****************************************************************************
unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift)
{
    unsigned char val;

    val = (unsigned char) (i >> shift);
    switch (nbits) 
    {
        case 1:
            val &= 0x1;
            return oneto8[val];

        case 2:
            val &= 0x3;
            return twoto8[val];

        case 3:
            val &= 0x7;
            return threeto8[val];

        default:
            return 0;
    }
}

//****************************************************************************
// Function: CreateRGBPalette 
//
//
// Comments: Taken from the "GenGL" OpenGL sample application
//
//****************************************************************************
void CreateRGBPalette(HDC hDC)
{
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE *pPal;
    int n, i;

    n = GetPixelFormat(hDC);
    DescribePixelFormat(hDC, n, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE) {
        n = 1 << pfd.cColorBits;
        pPal = (PLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
               n * sizeof(PALETTEENTRY));
        pPal->palVersion = 0x300;
        pPal->palNumEntries = n;
        for (i=0; i<n; i++) 
        {
            pPal->palPalEntry[i].peRed =
                    ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
            pPal->palPalEntry[i].peGreen =
                    ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
            pPal->palPalEntry[i].peBlue =
                    ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
            pPal->palPalEntry[i].peFlags = 0;
        }

        /* fix up the palette to include the default GDI palette */
        if ((pfd.cColorBits == 8)                           &&
            (pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
            (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
            (pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)) 
        {
            for (i = 1 ; i <= 12 ; i++)
                pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
        }

        ghPalette = CreatePalette(pPal);
        LocalFree(pPal);

        ghpalOld = SelectPalette(hDC, ghPalette, FALSE);
        n = RealizePalette(hDC);
    }
}

//****************************************************************************
// Function: bSetupPixelFormat
//
//
// Comments: Taken from the "GenGL" OpenGL sample application
//
//****************************************************************************
BOOL bSetupPixelFormat(HDC hDC)
{
    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
        1,                              // version number
        PFD_DRAW_TO_WINDOW |            // support window
        PFD_SUPPORT_OPENGL,             // support OpenGL
        PFD_TYPE_RGBA,                  // RGBA type
        24,                             // 24-bit color depth
        0, 0, 0, 0, 0, 0,               // color bits ignored
        0,                              // no alpha buffer
        0,                              // shift bit ignored
        0,                              // no accumulation buffer
        0, 0, 0, 0,                     // accum bits ignored
        32,                             // 32-bit z-buffer  
        0,                              // no stencil buffer
        0,                              // no auxiliary buffer
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
    };
    int pixelformat;

    if ( (pixelformat = ChoosePixelFormat(hDC, &pfd)) == 0 )
    {
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE)
    {
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    CreateRGBPalette(hDC);

    return TRUE;
}


//****************************************************************************
// Function: initialize
//
// Purpose: Called by Windows on app startup.  Initializes everything,
//          and enters a message loop.
//
// Parameters:
//    hWnd     == Handle to window
//
// Returns: none
//
// Comments:
//
// History:  Date       Author        Reason
//           3/20/95     GGB          Created
//****************************************************************************

GLvoid initialize(HWND hWnd)
{
    GLfloat light_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    /*  light_position is NOT default value */
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat maxObjectSize, aspect;
    GLdouble near_plane, far_plane;

    GetClientRect(hWnd, &oldrect);
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth( 70.0 );

    glEnable(GL_DEPTH_TEST);
    
    // Make it so our normal vectors are automatically changed to "unit" length
    glEnable(GL_NORMALIZE);

    glMatrixMode( GL_PROJECTION );
    aspect = (GLfloat) oldrect.right / oldrect.bottom;;
    gluPerspective( 45.0, aspect, 3.0, 70.0 );
    glMatrixMode( GL_MODELVIEW );

    // Set up lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    near_plane = 3.0;
    far_plane = 70.0;
    maxObjectSize = 3.0;
    radius = near_plane + maxObjectSize/2.0;
}


//****************************************************************************
// Function: ThreadFunc1
//
// Purpose:  Thread that draws the 3D wave on the left side of the screen.
//           This thread calls glClear and let's thread "2" proceed after
//           it has cleared the screen.  Then, they both start drawing their
//           own waves at the same time.
//
// Parameters: none
//
// Returns: none
//
// Comments:
//
// History:  Date       Author        Reason
//           3/21/95     GGB          Created
//****************************************************************************

GLvoid ThreadFunc1(void)
{
    HDC   hDC;
    HGLRC hRC;

    // Get a DC for the main window
    hDC = GetDC(ghWnd);
    bSetupPixelFormat(hDC);
    hRC = wglCreateContext( hDC );
    wglMakeCurrent( hDC, hRC );
    initialize(ghWnd);

    // Clear the main window.  Only _one_ thread should be doing this.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // Now, it's safe for the other thread to start drawing.
    ResumeThread(hThreads[1]);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -radius);

    // Define our point of reference
    gluLookAt(0.5,1.0,2.0,0.0,0.2,0.0,0.0,1.0,0.0);
    DrawWave(0.6, 0.7, -2.5);
    glPopMatrix();
    glFinish();

    wglMakeCurrent(NULL, NULL);
    if (hRC)
        wglDeleteContext(hRC);
    if (hDC)
        ReleaseDC(ghWnd, hDC);
}


//****************************************************************************
// Function: ThreadFunc2
//
// Purpose:  Thread that draws the 3D wave on the right side of the screen.
//           This thread waits until thread "1" has cleared the screen and has
//           called ResumeThread(hThreads[1]).  Then, it draws the wave.
//
// Parameters: none
//
// Returns: none
//
// Comments:
//
// History:  Date       Author        Reason
//           3/21/95     GGB          Created
//****************************************************************************

GLvoid ThreadFunc2(void)
{
    HDC   hDC;
    HGLRC hRC;

    // Get a DC for the main window
    hDC = GetDC(ghWnd);
    bSetupPixelFormat(hDC);
    hRC = wglCreateContext( hDC );
    wglMakeCurrent( hDC, hRC );
    initialize(ghWnd);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -radius);

    // Define our point of reference
    gluLookAt(0.5,1.0,2.0,0.0,0.2,0.0,0.0,1.0,0.0);
    DrawWave(0.2, 0.3, 2.5);
    glPopMatrix();
    glFinish();

    wglMakeCurrent(NULL, NULL);
    if (hRC)
        wglDeleteContext(hRC);
    if (hDC)
        ReleaseDC(ghWnd, hDC);
}


//****************************************************************************
// Function: draw_scene
//
// Purpose:  Called when a WM_PAINT message is sent to the main window.  It
//           creates two threads.  One is created suspended so the other
//           can call glClear to clear the rendering context.  Then, the
//           thread that calls glClear will call ResumeThread to let the other
//           thread start drawing.
//
// Parameters: hWnd == main window handle
//
// Returns: none
//
// Comments:
//
// History:  Date       Author        Reason
//           3/22/95     GGB          Created
//****************************************************************************

GLvoid draw_scene(HWND hWnd)
{
    DWORD dwThread1ID;
    DWORD dwThread2ID;
    
    hThreads[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc1,
                               NULL, 0, &dwThread1ID);
    
    // Create this thread suspended so it will wait until the window has been
    // cleared before drawing.
    hThreads[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc2,
                               NULL, CREATE_SUSPENDED,&dwThread2ID);

    // Wait for both threads to finish.
    WaitForMultipleObjects(2, hThreads, TRUE, 10000);
    CloseHandle(hThreads[0]);
    CloseHandle(hThreads[1]);
}


//****************************************************************************
// Function: resize
//
//
// Comments: Taken from the "GenGL" OpenGL sample application
//
//****************************************************************************
GLvoid resize(HWND hWnd)
{
    RECT    rect;

    GetClientRect(hWnd, &rect);

    glViewport(0, 0, rect.right, rect.bottom);

    oldrect.right = rect.right;
    oldrect.bottom = rect.bottom;
}


//****************************************************************************
// Function: DrawWave
//
// Purpose: Called by the two threads to draw a "cos" 3D wave
//
// Parameters:
//    fHeight     == Height of wave
//    fWidth      == Width of wave
//    fHorizon    == Where to place the wave along the "x" axis
//
// Returns: none
//
// Comments:
//
// History:  Date       Author        Reason
//           3/22/95     GGB           Created
//****************************************************************************

GLvoid DrawWave(GLfloat fHeight, GLfloat fWidth, GLfloat fHorizon)
{
    GLfloat x;
    GLfloat z;
    GLfloat step;

    step = 0.11;
    for (z=3.5;z >= -12.0;z-=step) 
    {
        for (x= -2.0 + fHorizon; x <= (2.0 + 0.01 + fHorizon); x += step) 
        {
            GLfloat fNormalX, fNormalY, fNormalZ;
            GLfloat fVert1[3];
            GLfloat fVert2[3];
            GLfloat fVert3[3];
            GLfloat fVert4[3];

            glBegin(GL_POLYGON);
            fVert1[0] = x;
            fVert1[1] = solve(x,z,fHeight,fWidth);
            fVert1[2] = z;
            glVertex3fv(fVert1);
            fVert2[0] = x;
            fVert2[1] = solve(x,z-step,fHeight,fWidth);
            fVert2[2] = z-step;
            glVertex3fv(fVert2);
            fVert3[0] = x+step;
            fVert3[1] = solve(x+step,z-step,fHeight,fWidth);
            fVert3[2] = z-step;
            glVertex3fv(fVert3);
            fVert4[0] = x+step;
            fVert4[1] = solve(x+step,z,fHeight,fWidth);
            fVert4[2] = z;
            glVertex3fv(fVert4);

            // Calculate the vector normal coming out of the 3D polygon.
            CalculateVectorNormal(fVert1, fVert2, fVert3, &fNormalX, &fNormalY, 
                                  &fNormalZ);
            // Set the normal vector for the polygon
            glNormal3f(fNormalX, fNormalY, fNormalZ);
            glEnd();
        }
    }
}


//****************************************************************************
// Function: solve
//
// Purpose: Calculates the "y" value of a "cos" wave given an x and z.
//
// Parameters:
//    x     == value for the "x" coordinate
//    y     == value for the "y" coordinate
//    h     == value for the height of the wave
//    w     == value for the width of the wave
//
// Returns: value for "y"
//
// Comments:
//
// History:  Date       Author        Reason
//           3/22/95     GGB           Created
//****************************************************************************

GLfloat solve(GLfloat x, GLfloat z, GLfloat h, GLfloat w)
{
    GLfloat y;
    y = cos(x/w)*h+cos(z/w)*h;
    return y;
}
    

//****************************************************************************
// Function: CalculateVectorNormal
//
// Purpose: Given three points of a 3D plane, this function will calculate the
//          normal vector of that plane.
//
// Parameters:
//      fVert1[]   == array for 1st point (3 elements are x, y, and z). 
//      fVert2[]   == array for 2nd point (3 elements are x, y, and z). 
//      fVert3[]   == array for 3rd point (3 elements are x, y, and z). 
//
// Returns: 
//      fNormalX   == X vector for the normal vector
//      fNormalY   == Y vector for the normal vector
//      fNormalZ   == Z vector for the normal vector
//
// Comments:
//
// History:  Date       Author        Reason
//           3/22/95     GGB           Created
//****************************************************************************

GLvoid CalculateVectorNormal(GLfloat fVert1[], GLfloat fVert2[], 
                             GLfloat fVert3[], GLfloat *fNormalX, 
                             GLfloat *fNormalY, GLfloat *fNormalZ)
{ 
    GLfloat Qx, Qy, Qz, Px, Py, Pz;

    Qx = fVert2[0]-fVert1[0];
    Qy = fVert2[1]-fVert1[1];
    Qz = fVert2[2]-fVert1[2];
    Px = fVert3[0]-fVert1[0];
    Py = fVert3[1]-fVert1[1];
    Pz = fVert3[2]-fVert1[2];

    *fNormalX = Py*Qz - Pz*Qy;
    *fNormalY = Pz*Qx - Px*Qz;
    *fNormalZ = Px*Qy - Py*Qx;
}
