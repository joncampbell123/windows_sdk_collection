Enhanced Metafile Editor


SUMMARY
=======

The MFEDIT sample demonstrates the enhanced metafile functions.

The metafile editor provides the following features:

 1. Playback and recording of GDI calls.
 2. Embedding bitmaps and enhanced metafiles into another enhanced metafile 
    with transformations.
 3. Hit-testing against enhanced metafile records.
 4. Random-access playback.
 5. Playback of metafile records one-by-one.
 6. Selective recording of existing enhanced metafile records into a new 
    enhanced metafile.
 7. Drawing with pens, text, beziers, lines, ellipses, and rectangles, 
    embedding bitmaps, and embedding enhanced metafiles.

MORE INFORMATION
================

1. Playback and Recording of GDI Calls

To playback an existing enhanced metafile, choose Load Metafile from the 
File menu or click on the Eject button to bring up the Open File dialog box 
and select the appropriate file. Then click on the Play button to play it on 
the drawing surface.

To record, click on the Record button and draw on the drawing surface with 
the graphic tools provided. When you are done, click on the Stop button.

The GDI calls are recorded as c:\metafx.emf where x is 0, 1, 2, 3, etc. If 
you want to save the metafile with a different name, choose Record Metafile 
As from the File menu. The new filename will be used as the root for all 
metafiles recorded; 0, 1, 2, etc, will be appended to the root name.

The default drawing tool is "pen". To select a different drawing tool, click 
on the desired tool button in the Control Panel.

2. Embedding Bitmaps and Enhanced Metafiles Into Another Enhanced Metafile
   with Transformations

Click the Record button. Select the Bitmap or Metafile tool and then embed 
the currently loaded bitmap or metafile as described in section 7 below. 
When you are done, click the Stop button.

3. Hit-Testing Against Enhanced Metafile Records

Playback an enhanced metafile by clicking on the Playback button. Then 
choose Hit Test from the Edit menu. The cursor will be changed to a cross 
when the mouse pointer is over the drawing surface.

Then, click on the graphic object played back in the drawing surface. The 
editor will search through the metafile record after record to find the 
record that corresponds to the object based on the mouse position. The 
search provides a visual cue by changing the graphic objects to red as it 
goes until it hits the corresponding object. If there is a hit, the record 
number is displayed on the Control Panel, a beep is issued, and a hit 
message is displayed on the bottom.

When you are done, remove the check from the Hit Test menu item. 

4. Random Access Playback

Click on the number button in the Control Panel to playback a particular 
record. To access a non single-digit record, click on the 10+ button an 
appropriate number of times, and then the appropriate number button, to 
bring the sum to the record desired.

5. Playback of Metafile Records One-by-One

Click on the Fast Forward button to play the metafile records one at a time.

6. Selective Recording of Existing Enhanced Metafile Records into a New
   Enhanced Metafile.

Click on the Record button and the appropriate number button for playing 
back selected metafile records in the drawing surface. The playback records 
are recorded into the new metafile. When you are done, click on the Stop 
button.

7. Drawing With Pens, Text, Beziers, Lines, Ellipses, and Rectangles,
   Embedding Bitmaps, and Embedding Enhanced Metafiles

The default pen is black. To change the pen color, simply choose the Pen 
menu item from the Options menu.

The default brush used by the Fill Rectangle and Fill Ellipse routines is 
black. To change the brush color, simply choose Brush from the Option menu.

The "Text" tool uses the default system font. To change the font and font 
attributes, simply choose Font from the Options menu.

The "Bezier" tool takes four points initially and three thereafter. To draw 
a bezier curve, simply select the "Bezier" tool and click on the drawing 
surface three or four times to place the control points.

To embed a currently loaded bitmap, simply select the "Bitmap" tool and 
click three points on the drawing surface to describe the destination of 
where you would like the bitmap to go. The editor will do the proper 
transform to the bitmap and embed the bitmap to the drawing.

To load a bitmap, simply choose Load Bitmap from the File menu and do the 
selection. The "Bitmap" tool optionally takes a mask bitmap. The mask bitmap 
has to be a monochrome bitmap.

To load a mask bitmap, simply choose Load Mask Bitmap from the File menu to 
do the selection. Select a color bitmap as the mask has the effect of 
resetting the mask to none.

To embed a currently loaded enhanced metafile, simply select the "Embed 
Enhanced Metafile" tool and click three points on the drawing surface to 
describe the destination of where you would like the enhanced metafile to 
go. The editor will do the proper transformation on the metafile and embed 
it to the drawing.
