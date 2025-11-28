//-----------------------------------------------------------------------------
// 
// Sample Name: FDFilter Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The FDFilter program demonstrates how to use DirectSound to implement full 
  duplex audio and a filter.

  A microphone or other audio input is required.

Path
====
  Source: \Mssdk\Samples\Multimedia\DSound\Src\FDfilter

  Executable: \Mssdk\Samples\Multimedia\DSound\Bin

User's Guide
============
  When you start the program, it presents you with a dialog box that contains 
  pull-down lists of the sound input and output devices attached to your 
  computer. Select the appropriate devices. Usually the defaults are the most 
  appropriate. 

  You next see lists of sampling rates for the input and output buffers. Select 
  an output format, then an input format. You can change these formats later 
  from the Settings menu.

  The program then shows a dialog box that enables you to select the filter. 
  The available options are None (pass-through), which just plays back the 
  input sound as is, or Gargle, which distorts the input sound into a gargling 
  noise. Click Enable to begin capturing and playing sound.

Programming Notes
=================
  FDFilter is designed to be a simple example of how one might go about 
  implementing full-duplex audio. It is designed primarily to show application 
  developers the proper sequence of steps for dealing with the sound devices 
  installed on the system for using full-duplex audio with waveIn as an input 
  source and DirectSound as the output device.

  For more information, see the Readme.txt file in the Fdfilter directory.

