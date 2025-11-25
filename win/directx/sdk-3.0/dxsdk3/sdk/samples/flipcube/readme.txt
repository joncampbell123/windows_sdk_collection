Flipcube
Direct3D Immediate Mode Sample
Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.

A cube is created and spun through mouse and keyboard input.  The
world matrix is changed in response to mouse and keyboard input and
set before rendering each frame.  Keyboard and mouse cues:

        F11 - closer
        F12 - further
        left-mouse drag - spin with mouse movement
        right-mouse click - resume spinning

Vertices are not shared because the normals are different for each
side.  Each side is covered by a separate material.
DLIGHTSTATE_MATERIAL must be set and D3DPROCESSVERTICES_TRANSFORMLIGHT
must be called for each side of the cube.
