Mid2Strm Sample Readme File
===========================

This sample is related to the mstream sample, and contains a subset of the
functionality of mstream.

Mid2strm converts a .mid file (standard midi file) into a format which can
be easily sent to the the midiStreamXxx APIs.  It takes a .mid file as
input, and produces a .mds file as output.  Note that .mds files are not
a standard Windows file format; for example, they can't be played back by
mplayer.  Your application must play the file back by sending the data to
the midiStreamXxx APIs as shown in the mstream sample.

The midiStreamXxx APIs expect a buffer containing 3 DWORDs per MIDI event:
the event, the time stamp, and the stream ID.  The mid2strm sample will
create a buffer of this format and store it as a .mds file.  Of course,
the file will be larger than the .mid file.  To save space, you can specify
the -c option.  This specifies that stream IDs should not be included in the
file, compressing the file by 1/3.  This assumes that all stream IDs will be
zero; for most applications, this is sufficient.  Note, however, that the
stream ID DWORD will have to be inserted into the buffer before it is sent
to the midiStreamXxx APIs.

Please see the mstream readme file for more information.
