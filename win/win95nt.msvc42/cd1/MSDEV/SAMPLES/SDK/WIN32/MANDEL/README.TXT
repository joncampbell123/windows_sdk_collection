Sample: Demonstrating GDI and USER API Functions in Fractals

Summary:

The MANDEL sample demonstrates the Win32 GDI and USER API
functions in the setting of fractals.

More Information:

The Mandelbrot Dream provides the following functions:

  Drawing the Mandelbrot set and the corresponding julia
  set
  Zooming into any of the set
  MDI fractal drawing windows
  Floating Point Math/Fix Point Math
  Shifting color table entries
  Changing palette entries and animating palatte aka color
  cycling
  Loading/Saving bitmap created with special effect
  Changing bitmap color with flood fill
  Boundary tracing and creating a clip region out of it for
  creating special effect
  Enumerate printers for printing
  Load RLE (or convert .bmp files to RLE) for playing in viewer
  Save the RLE in memory to disk.


  Note: The sample makes use of 64-bit integers for its
        fixed point math.



Drawing the Mandelbrot Set and the Corresponding Julia Set

1.To draw the Mandelbrot set, choose the Mandelbrot Set
  menu item from the Create menu to create a MDI child
  window to draw the set in if one has not already been
  created.

2.Then, choose either "use Fix Point math" or "use Floating
  Point math" from the Draw menu to decide if floating
  point or fixed point math is desired for calculation.
  Fix Point is faster, however you lose resolution sooner
  as you zoom in.

3.Also, choose the number of iterations from the Iteration
  menu item and choose Step from the Draw menu.  The higher
  the number of iterations, the more detail is the picture
  but the slower to generate the picture.  The step
  determines whether every scan line is drawn.  The more
  scan lines it has, the better the picture but the slower
  to generate the picture.

4.To start drawing, choose Draw Set from the Draw menu.

5.To draw the Julia set, use the right mouse button to
  select a point in the Mandelbrot set (the drawing surface
  of the Mandlebrot window). A new Julia MDI window will be
  created.  Then choose Draw Set from the Draw menu to
  start drawing.

6.The point selected with the right mouse button determines
  the complex constant to use for the Julia Set.

Zooming Into Any of the Set

1.To zoom in, click, drag and release with the left mouse
  button to describe the zoom in region.

  A new MDI child of the same type as the parent
  (Mandelbrot window or Julia window) will be created.

2.Choose Draw Set from the Draw menu to start drawing.

MDI Fractal Drawing Windows

  Choose either Mandelbrot Set or Julia Set from the Create
  menu for creating a new MDI window for drawing.

  Or, use the left mouse button to describe a zoom in
  region in either a mandelbrot or Julia MDI window for
  creating a new MDI window for drawing.

  Or, click on the Mandelbrot window with the right mouse
  button for creating a Julia MDI window corresponding to
  the mouse click position in the Mandelbrot window.

Floating Point Math/Fix Point Math

  Choose the appropriate menu item ("Floating Point math"
  or "Fix Point math") from the Draw menu.  The Fix Point
  math uses 20.11 fixed point integer arithmetic for
  calculation.

Shifting Color Table Entries

  Choose Shift from the Color menu or hit F10 to shift the
  color table entry.  The picture of the active MDI window
  will be updated.

Changing Palette Entries and Animating Palatte (aka Color
Cycling)

  Choose Cycle from the Color menu or hit F11 to start
  color cycling the picture.

  The menu item will be grayed if the display device does
  not support palette management.  Currently, only the MIPs
  display driver supports that.

Loading/Saving Bitmaps Created With Special Effect

  Choose Load Bitmap from the Bitmap menu to load a bitmap
  into the active child window.  Or, choose Save Bitmap As
  to save the picture in the active MDI child window.

Changing Bitmap Color With Floodfill

  Choose Custom from the Color menu to select a color.
  Then the cursor will be changed to a paint can over the
  active child window.  Click with the left mouse button on
  the picture, the old color under the cursor will be
  changed to the new color.

Boundary Tracing and Creating a Clip Region Out of it For
Creating Special Effect

  From any active Mandelbrot window, choose Set Mandelbrot
  Clip region from the Region menu.  The boundary of the
  escape region will be traced.  The region will then be
  selected as a clip region.
   
  Thus, if you load a bitmap for display, the bitmap will
  only show through the clip region.  The new picture can
  then be saved.

  To remove the clip region, choose Remove Clip Region from
  the Region menu.

  Note, the boundary tracing algorithm may trace out a
  small island of only several pixels and stop.  If that
  happens, you might change the size of the window or
  create another zoom window and trace again.

Enumerate printers for printing

  On start up, the Mandelbrot Dream will enumerate the
  printers and insert the printers into the Print menu.
  Then selecting the printer on the Print menu will print
  the picture in the active MDI window.

Load RLE (or convert .bmp files to RLE) for playing in viewer

  Choose the Viewer item from the Create menu to create a
  viewer window or bring any existing viewer window to the top.
  Select Load Bitmap(s) from the File menu for loading RLE or
  bmp files into the memory from disk.
  Select the Play or Play Continuously item from the Options
  menu for viewing.

  For demonstration of what this functionality can do.
  Load the .\rsc\julia.rle file and select Play Continuously.
  The Julia.rle is a collections of the various julia sets
  along the boundary of escaping and non-escaping points of
  the Mandelbrot Set.

Save the RLE in memory to disk.

  Choose the Viewer item from the Create menu to create a
  viewer window or bring any existing viewer window to the top.
  Select Save Bitmap(s) from the File menu for saving the RLE(s)
  from memory to disk.
