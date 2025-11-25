
                                   MSTREAM
 
                        Design Description and Notes

   The MSTREAM sample application is an example of how you can use the new
midiStream API that's built into Windows 95 to play MIDI data with low
latency and low processor overhead.

   The basic idea behind the implementation is to start the application and
initialize the user interface.  After that's done, the following events occur.

When a user selects and opens a file
------------------------------------
Open a MIDI output device (the Microsoft MIDI Mapper in this case)
Open the file using buffered I/O and fill our buffers with data in MIDI stream
   format
Prepare and queue the buffers using midiOutPrepareHeader and midiStreamOut
Wait until the user decides to do something else

When PLAY is selected
---------------------
Call midiStreamRestart to un-pause the device and begin playback
As buffers are returned, the callback function fills them with more data and
sends them back to be played by calling midiStreamOut() again.  IT IS
IMPORTANT THAT YOU NOTE THAT IT IS NOT NECESSARY FOR YOU TO UNPREPARE AND
PREPARE BUFFERS EVERY TIME THEY ARE RETURNED.  This is only a waste of time.
All you have to do is send them back into the subsystem with midiStreamOut().
The API documentation is a bit confusing on this point.

When PAUSE is selected
----------------------
Call midiStreamPause() and wait for a PLAY or PAUSE to call midiStreamRestart().
Note that pausing may make your audio sound kind of funny, since all notes are
turned off and some note on events may be lost when playback is restarted.

When STOP is selected
---------------------
Call midiStreamStop().  Since the callback function is in another thread, we
use a Win32 synchronization object, an event to block the main thread until
the callback thread has received all buffers.  When this happens, we know it's
okay to go ahead and call midiStreamReset() and then to go ahead and free all
our buffers.  Currently, the device is also closed using midiStreamClose().
Then, if we are resetting the file to playback starting at its beginning point
the next time PLAY is hit, reopen the file by calling StreamBufferSetup(),
which is the workhorse function for opening a file and initializing the
the converter and the buffers.

Note that the reason we must close and reopen the device is due to an apparent
bug in the Multimedia System which causes undesireable playback once a device
has been stopped.  If you disable the code for closing the device, then the rest
of the code will automatically know it does not have to reopen the device.
However, the following may occur: After the first instance of playback followed
by a midiStreamStop() and a midiStreamReset(), there will be a pause equal in
length to the first playback period before playback begins once more with a call
to midiStreamRestart().

If Looped is selected
---------------------
Notice that the converter has a little function in it called RewindConverter()
which is called by ConvertToBuffer() if the bLooped variable is TRUE.  This
function resets the track state structures and performs most of the steps
originally executed in ConvertInit() with the notable exception of opening the
file and reading in file and track header data.  It simply resets the tracks
to their initial state.


More on the buffering scheme
----------------------------
Note that there is an OUT_BUFFER_SIZE and a BUFFER_TIME_LENGTH.  The idea is
that the converter counts ticks and calculates when it has put at least
BUFFER_TIME_LENGTH milliseconds worth of events in the buffer.  At this point,
it returns to the caller with a "full" buffer.  At the very worst, there will
be as many events as can fit into OUT_BUFFER_SIZE bytes.  Imagine that you have
to MIDI files loaded into memory somehow and you want to switch between them in
a hurry to correspond to some action the player performed like switching rooms.
All you have to do is start filling the next buffer with new data (assuming
they streams use similar patch sets, time division settings, etc.) and after
(NUM_STREAM_BUFFERS-1)*BUFFER_TIME_LENGTH milliseconds, the music will switch
over automatically.  This theory is sort of illustrated by the tempo trackbar
control in this sample.  This control sets a flag which forces the converter
code to start a new buffer, with the first event being a new tempo setting.
The tempo setting is calculated as a relative increase with respect to the
last real tempo event from the file.  Of course, to implement the scheme
mentioned above requires some modification to the converter code so that it
will work with multiple MIDI files.

Since the buffering scheme uses very small buffers, it is currently rather
sensitive to heavy activity which may prevent it from completing processing
in time.  This can be solved by increasing the NUM_STREAM_BUFFERS constant,
but you must make a trade-off between latency and playback stability.


Known problems and possible improvements:
-----------------------------------------
It is more desireable to enumerate all possible MIDI output devices and then
   either allow the user to use a specific device, or choose one which has
   desireable capabilities.  It is not recommended that you ship a product
   which is hard-coded to use the MIDI Mapper only.  For more on enumerating
   MIDI output devices, see the MIDIPLYR sample application which is part of
   the Win32 SDK.

Instead of using the BUFFER_TIME_LENGTH, it would be possible to handle the
   time signature META event in MIDI files and calculate the length of a
   measure of music.  Then you could change buffers at the end of each measure,
   which would probably yield a smoother sounding transition.  It may even be
   possible to define system-exclusive events or other such extensions to the
   MIDI converter code designed to provide your application with extra data
   about when to switch between buffers or do other processing, though it is
   not necessarily recommended that you modify the MIDI file format spec.  For
   more information on that spec, contact the International MIDI Association.

You may wish to modify the way a change in tempo is handled, or remove this
   code entirely.  Right now, there is a chunk of code in the convert function
   AddEventToStreamBuffer() which detects tempo events and stores the new tempo.
   There is also code which will react to the tempo slider by calculating a new
   tempo, truncating the current buffer, and starting the next buffer with a
   tempo event reflecting the new desired tempo. It may be more desireable to
   force any tempo changes which are not encoded in the file originally to take
   effect only on buffer boundaries, instead of always creating a buffer
   boundary.  Proceeding under the above context of buffers equal in length to
   measures of music, it may make more sense to only change tempo between each
   measure.  You can also send tempo change messages using the midiOutShortMsg()
   function, similar to the way SetAllChannelVolumes() behaves.

The volume control is a channel-wide, percentage-based control which relies on
   a cache of volumes for each channel.  As the converter encounters a volume
   change message, it flags it for a callback.  This causes the MidiProc() to
   receive notification when that event is reached.  MidiProc() then grabs a
   copy of the new volume event and sends a MIDI short message to the proper
   channel which reflects the current slider position.  In other words, the
   code saves the "full" or "raw" value and then modifies it so the volume
   trackbar represents a percentage of that volume.  Though it is not shown
   here, this scheme could be broken down to allow for individual volume
   control also.  This idea could also be expanded to include the LSB volume
   controller(39), which is not handled here.

Further, by duplicating the volume code described above and making slight
   modifications to the converter (to detect other events), it is possible to
   handle pan, balance, or other controller messages using the exact same idea.

Having said the above, it should be noted that attaching the volume change code
   to a trackbar is for illustration purposes only.  The implementation shown
   and described works best for isolated volume events, like when your player
   moves away from the sound source and you need to update volume.  It should
   not really be used for real-time scrolling because the method tends to flood
   the MIDI output device with short messages, which interferes severly with
   playback.

BUG: If you are using an internal MIDI device which uses the OPL chipset, you
   should be aware of a bug which seems to occur in most of these drivers.  If
   a volume channel message is sent to these drivers, they will not reflect the
   change until a note on/off event occurs.  This means long sustaining notes
   will not reduce in volume.
