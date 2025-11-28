//-----------------------------------------------------------------------------
// 
// Sample Name: Flip2D Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  This sample program demonstrates DirectDraw animation using surface flipping.

Path
====
  Source: Mssdk\Samples\Multimedia\DDraw\Src\Flip2d

  Executable: Mssdk\Samples\Multimedia\DDraw\Bin

User's Guide
============
  The Flip2D program displays and animates a cube. You can change the screen 
  mode by pressing the F10 key and selecting a resolution and color depth value 
  from the Modes menu. Use the Cube menu or the keyboard shortcuts to increase 
  (F7) or decrease (F8) the displayed size of the cube, or to switch to GDI 
  drawing on the primary surface (F9).

Programming Notes
=================
  Normally the cube is rendered on the back surface and then the surface is 
  flipped to the front. You can compare the results with those from rendering 
  directly on to the front surface by choosing GDI drawing.

  The palette that the Flip2D program uses for 8-bpp modes is red, green and 
  blue wash. There isn't enough room in the palette for yellow, orange, and 
  purple. In 8-bpp modes the Flip2D sample applicationmaps yellow, orange and 
  purple to red. If you select DrawWithGDI from the Cube menu, yellow, orange, 
  purple are drawn without very many shades. This is because the program is 
  using system colors, and there are only a few shades of yellow, orange, and 
  purple available in the system palette.

