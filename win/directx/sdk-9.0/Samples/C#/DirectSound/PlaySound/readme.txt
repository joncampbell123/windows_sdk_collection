//-----------------------------------------------------------------------------
// 
// Sample Name: PlaySound Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The PlaySound sample shows how to play a wave file in a DirectSound 
  secondary buffer.
    
Path
====
  Source:     DXSDK\Samples\C#\DirectSound\PlaySound
  Executable: DXSDK\Samples\C#\DirectSound\Bin

User's Guide
============
  Load a wave file by clicking Sound File. Select Loop Sound if you want 
  it to play repeatedly. Click Play.

Programming Notes
=================
  The basic tasks write an application that supports DirectSound are as follows:
 
  * Set up DirectSound: 
     1. Instantiate the Device.
     2. Call Device.SetCooperativeLevel to set the cooperative level.
       
  * Load a wav file into a DirectSound SecondaryBuffer:
     1. Call the constructor that takes a stream or file name and a device if standard
        control is needed over the file, or if specific control is needed, call the
        constructor that takes a stream or file name, a WaveFormat class, and a device.

  * Play or stop the DirectSound buffer:
     1. To play the buffer call DirectSoundBuffer.Play.
     2. To stop the buffer call DirectSoundBuffer.Stop.
     
  * To check to see if the sound stopped:
     It may be useful to tell if a DirectSound buffer stopped playing.  An easy 
     way to do this would be to set a timer to trigger every so often.  When the 
     timer message is sent, call DirectSoundBuffer.GetStatus to see if the 
     BufferStatus.Playing flag is set.  If it is not, then the sound has stopped.   
