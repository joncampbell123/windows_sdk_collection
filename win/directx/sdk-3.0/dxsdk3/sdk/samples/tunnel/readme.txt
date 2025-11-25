Tunnel
Direct3D Immediate Mode Sample
Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.

Camera moves through a tunnel generated around a spline.  Only a small
section of the tunnel is placed in the execute buffer at a time.
After each frame, the buffer is locked and the tunnel vertices updated
to include the next section.

Triflags are set so the segments of the tunnel are drawn as quads
rather than triangles when rendered in wireframe.
