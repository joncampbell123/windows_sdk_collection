//-----------------------------------------------------------------------------
// 
// Sample Name: D3DFrame Library
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The D3D framework is a set of C++ classes that were used to create the 
  Direct3D Immediate Mode sample programs. It is not a part of the Direct3D 
  API. The framework was created and used to provide consistency and clarity of 
  presentation for the Direct3D samples. The framework classes may or may not 
  be appropriate for use in your applications.

Path
====
  Source: Mssdk\Samples\Multimedia\D3dim\Src\D3DFrame

  Executable: None.

User's Guide
============
  D3DFrame compiles as a static linker library, which is used to build the 
  remainder of the Direct3D Immediate Mode samples.

Programming Notes
=================
  The framework consists of classes that enumerate the DirectDraw drivers, 
  Direct3D devices, and display modes available to each device. The sample 
  programs use them to initialize and run Direct3D. The classes also help 
  provide a consistent user interface for the set of sample programs. In 
  addition, the framework includes a set of classes for loading and managing 
  textures.

  The Direct3D framework also contains numerous macros and functions for 
  debugging, for manipulating Direct3D objects, and for doing math operations 
  common in Direct3D programming. 

