//-----------------------------------------------------------------------------
// 
// Sample Name: ShadowVol2 Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The ShadowVol2 sample demonstrates how to create and use stencil buffers to 
  implement shadow volumes, which are used to cast shadows on arbitrarily 
  complex objects. It is an extension of the ShadowVol Sample.

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\ShadowVol2

  Executable: Mssdk\Samples\Multimedia\D3dim\Bin

User's Guide
============
  This sample will run only on devices that support stencil buffers.

  Press F1 to see available commands.

  The following options are available on the Shadow Modes menu:
  	Draw Shadows: Check this option to enable shadow rendering.

  	Show Shadow Volumes: Instead of shadows, draw the shadow volumes used to 
  compute them.

  	Draw Shadow Volume Caps: If this is turned off, "extra" shadows may be 
  visible where the far caps of the directional-light cylindrical shadow 
  volumes happen to be visible.

  	Show Quarter Viewpoint: Show scene from a different angle. 

  	1 Bit Stencil Mode: Use different algorithm that uses only 1 bit of stencil 
  buffer, where overlapping shadows are not allowed.  If the device only 
  supports 1-bit stencil, you will not be allowed to switch out of this mode.

  	Z Order Shadow Vols: In 1-Bit Mode, shadow volumes must be rendered 
  front-to-back, so rendering may be incorrect unless this option is checked.

Programming Notes
=================
  Shadow volumes are a technique for casting shadows onto arbitrary non-planar 
  surfaces. The effect is achieved by constructing a shadow volume with respect 
  to the light source and the shadow caster. In this example, the light source 
  is a directional light whose direction circles about points on the plane, and 
  the shadow volume is computed by projecting the vertices of the shadow caster 
  onto a plane perpendicular to the light, finding the 2D convex hull of these 
  points in the plane, and extruding the 2D hull in the light direction to form 
  the 3D shadow volume.  The shadow volume must extend far enough so that it 
  covers any geometry that will be in shadow.  This particular shadow volume 
  computation requires that the shadow caster be a convex object.

  The rendering proceeds as follows. First the geometry is rendered as normal, 
  then the shadow volume is rendered without writing to the z or color buffer 
  (alpha blending is used here to avoid writes to the color buffer). Every 
  place the shadow volume appears is marked in the stencil buffer. Next, the 
  cull order is reversed and the backfaces of the shadow volume are rendered, 
  this time unmarking all the pixels that are covered in the stencil buffer. 
  These have passed the z-test, and thus are visible behind the back of the 
  shadow volume, so they are not in shadow. The pixels still marked are those 
  that lie inside the front and back bounds of the shadow volume and are thus 
  in shadow. These pixels are blended with a large black rectangle that covers 
  the viewport, generating the shadow.

  

