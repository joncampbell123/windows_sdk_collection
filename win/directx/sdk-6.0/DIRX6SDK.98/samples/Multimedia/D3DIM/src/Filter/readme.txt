//-----------------------------------------------------------------------------
// 
// Sample Name: Filter Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The Filter sample program demonstrates the texture filtering techniques that 
  Direct3D supports. Direct3D texture filtering techniques enable applications 
  to achieve a greater realism in the appearance of rendered primitives. For 
  more information, see Texture Filtering. 

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\Filter

  Executable: None.

User's Guide
============
  When the program begins, it displays two rectangular primitives with textures 
  on them.

  In addition to the usual commands listed in the About box when you press F1, 
  this program has a main menu. 

  The File menu contains choices for pausing and resuming the program, changing 
  the Direct3D device, and exiting the program. You can also pause and resume 
  the application by pressing the ENTER key on your keyboard.

  The Left Pane menu controls the texture filtering methods that the program 
  uses for magnification and minification of the 3-D primitive on the left side 
  of the screen. It also has options for edge antialiasing and anisotropic 
  texture filtering. You may have to enable to software reference rasterizer to 
  view the effects of these options. To enable the reference rasterizer, run 
  Mssdk\Samples\Multimedia\D3dim\Bin\Enablerefrast.reg.

  The Right Pane menu enables you to set the filtering methods that the program 
  uses to perform magnification and minification when it renders the primitive 
  on the right side of the screen.

Programming Notes
=================
  This program demonstrates nearest point sampling, linear filtering, and 
  anisotropic texture filtering. It illustrates how to enable and disable 
  anisotropy. In addition, this sample shows how your program can set Direct3D 
  to perform edge antialiasing.

