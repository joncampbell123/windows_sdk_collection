Sample: Enhanced Metafile Editor

Summary:

This sample demonstrates the Win32 Enhanced Metafile API
functions.

More Information:

The MFEDIT metafile editor provides the following
functionalities:

  Playback and recording of GDI calls
  Embedding bitmap and enhanced metafile into another
  enhanced metafile with transformation
  Hit-testing against enhanced metafile records
  Random access playback
  Playback metafile records one-by-one
  Selective recording of existing enhanced metafile records
  into a new enhanced metafile
  Drawing with pen, text, bezier, line, ellipse, rectangle
  and embedding bitmap and enhanced metafile tools


Playback and Recording of GDI calls

  To playback an existing enhanced metafile, choose Load
  Metafile from the File menu or click on the Eject button
  to bring up the open file dialog and select the
  appropriate file.  Then click on the Play button to play
  it on the drawing surface.

  To record, click on the Record button and draw on the
  drawing surface with the graphic tools provided.  When
  done, click on the Stop button.

  The GDI calls will be recorded as c:\metafx.emf where x
  is 0, 1, 2, 3, etc.  If you want to save the metafile
  with a different name, choose Record Metafile As from the
  File menu.
  
  The new filename will be used as the root for all
  metafile recorded; 0, 1, 2, etc will be appended to the
  root name.

  The default drawing tool is "pen".  To select a different
  drawing tool, simply click on the desired tool button in
  the control panel.

Embedding Bitmap and Enhanced Metafile Into Another Enhanced
Metafile with Transformation

  Click the Record button.  Select the Bitmap or Metafile
  tool and then embed the currently loaded bitmap or
  metafile as described in 7 below.  When done, click the
  Stop button.

Hit-Testing Against Enhanced Metafile Records

  Playback an enhanced metafile by clicking on the Playback
  button. Then choose Hit Test from the Edit menu.  The
  cursor will be changed to a cross when the mouse pointer
  is over the drawing surface.

  Then click on the graphic object played back in the
  drawing surface.  The editor will search through the
  metafile record after record to find the record that
  corresponds to the object based on the mouse position.
  The search provides a visual cue by changing the graphic
  objects to red as it goes until it hits the corresponding
  object.  If there is a hit, the record number will be
  displayed on the control panel, an beep will be heard and
  a hit message displayed on the bottom.

  When done, uncheck the Hit Test menu item.

Random Access Playback

  Click on the number button in the control panel to
  playback a particular record.  To access a non single
  digit record, click on the 10+ button an appropriate
  number of times and then the appropriate number button to
  bring the sum to the record desired.

Playback Metafile Records One-by-One

  Click on the Fast Forward button to play the metafile
  record one at a time.

Selective Recording of Existing Enhanced Metafile Records
Into a New Enhanced Metafile

  Click on the Record button and the appropriate number
  button for playing back selective metafile records in the
  drawing surface.  The playback records will be recorded
  into the new metafile.  When done, click on the Stop
  button.

Drawing With Pen, Text, Bezier, Line, Ellipse, Rectangle and
Embedding Bitmap and Enhanced Metafile Tools

  The default pen is black.  To change the pen color,
  simply choose the Pen menu item from the Options menu to
  select a color.

  The default brush used by the Fill Rectangle and Fill
  Ellipse routines is black.  To change the brush color,
  simply choose Brush from the Option menu to select a
  color.

  The "Text" tool uses the default system font.  To change
  the font, simply choose Font from the Option menu to
  change to a different font and font attributes.

  The "Bezier" tool takes four points initially and three
  thereafter. To draw a bezier curve, simple select the
  "Bezier" tool and click on the drawing surface three or
  four times to place the control points.

  To embed a currently loaded bitmap, simply select the
  "Bitmap" tool and click three points on the drawing
  surface to describe the destination of where you would
  like the bitmap to go.  The editor will do the proper
  transform to the bitmap and embed the bitmap to the
  drawing.

  To load a bitmap, simply choose Load Bitmap from the File
  menu and do the selection.  The "Bitmap" tool optionally
  takes a mask bitmap.  The mask bitmap has to be a
  monochrome bitmap.

  To load a mask bitmap, simply choose Load Mask Bitmap
  from the File menu to do the selection.  Select a color
  bitmap as the mask has the effect of reseting the mask to
  none.

  To embed a currently loaded enhanced metafile, simply
  select the "Embed Enhanced Metafile" tool and click three
  points on the drawing surface to describe the destination
  of where you would like the enhanced metafile to go.  The
  editor will do the proper transformation on the metafile
  and embed it to the drawing.

