DSShow3D -- DirectSound3D Sample
================================


THIS SAMPLE IS STILL UNDER DEVELOPMENT, PLEASE SEE THE "KNOWN ISSUES" SECTION

This program is an improved version of the DirectSound Show (DSSHOW) sample
that shipped in previous DirectX SDKs.  It features a more compact interface,
support for 3D sounds and new buffer options, the ability to open countless
sounds (i.e. more than 8, which was dsshow's limit), and a new UI that takes
advantage of your entire desktop for placing (or minimizing) windows.

Here are some things to try:

* Right click on a volume, frequency, or pan control to select some common
  settings.

* Frequency control will use the full range reported by IDirectSound::GetCaps()

* Open a bunch of files :)

* Try using the various focus settings in the File|Open Box



KNOWN ISSUES:
-------------
* No text controls for 3D position

* Hopefully the client window will get a Direct3D visual representation of the
  sound world you're playing with, but this isn't certain

* Windows all open at the same position -- there will be a cascade option, and
  new windows will cascade by default

* 3D functionality not complete for buffer controls -- missing cone orientation,
  min distance, max distance

* 3D Listener controls not present -- on the way

* Can't move progress indicator yet

* Doesn't stream large files automagically (yet)

* Progress updating is kind of rough (perhaps not frequent enough)

* Setting frequency to 0 Hz by quickly dragging doesn't behave properly

* App may not properly respond to user log off while running
