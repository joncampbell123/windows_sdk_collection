Triangle
Direct3D Immediate Mode Sample
Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.

One triangle on a textured background.  The triangle is created by
specifying the screen coordinates for the vertices using TLVERTEXs.

The top left vertex, the first vertex in the triangle data, has a
diffuse color of blue.  Switching between flat and gouraud shading
changes the effect this vertex has on the lighting across the
triangle.  When flat shading, the first vertex specifies the lighting
for the entire triangle. When gouraud shading, the lighting is
interpolated across the triangle.  Lighting is different in the two
software renderers.  The RAMP driver does not modulate the texture's
color with the material color.

The rhw value of the top right vertex is larger than the other
two; it is closer to the eye.  Turning perspective correction on and
off changes the interpolation of the texture across the triangle.
