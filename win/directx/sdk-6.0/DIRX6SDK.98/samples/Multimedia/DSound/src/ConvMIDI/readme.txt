//-----------------------------------------------------------------------------
// 
// Sample Name: ConvMIDI Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  ConvMIDI is a console application that converts a MIDI file into a format 
  that can easily be sent to the Win32 midiStream* functions. It is not a 
  DirectSound application.

Path
====
  Source: \Mssdk\Samples\Multimedia\DSound\Src\ConvMIDI

  Executable: \Mssdk\Samples\Multimedia\DSound\Bin

User's Guide
============
  The program accepts the name of a source file and a destination file on the 
  command line. The full name of each file must be given, and the destination 
  file should have the .mds extension. Optionally you can include the -c 
  argument in order to eliminate stream IDs from the file, thus reducing its 
  size. 

  For example:
  convmidi -c canyon.mid canyon.mds

Programming Notes
=================
  You need to write your own code to read the .mds file and send the data to 
  the midiStream* functions. The use of these functions is demonstrated in the 
  MStream sample.

  The midiStream* functions expect a buffer containing 3 DWORDs per MIDI event: 
  the event, the time stamp, and the stream ID. The ConvMIDI application stores 
  the data in this format. Of course, the output file will be larger than the 
  MIDI file. To save space, you can specify the -c option, which specifies that 
  stream IDs should not be included, reducing the file's size by a third. This 
  assumes that all stream IDs will be zero, and for most applications this is 
  sufficient. Note, however, that the stream ID DWORD will have to be inserted 
  into the buffer before it is sent to the midiStream* functions

