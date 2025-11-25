 Sample: AngleArc Demonstration Program

Summary:

The sample named ANGLE provides a demonstration of how the
new AngleArc API function works. The X, Y, and RADIUS
parameters are all in the world coordinate space. The start
angle and sweep angle are floating-point values and are
interpreted as degrees.

More Information:

This program presents a dialog box stretched across the top
of the window. The user can set the parameters for the
AngleArc API function by changing the values in the entry
fields of this dialog box. A button on the dialog box then
allows the user to immediately see the results of these
values on the arc in the client area. If the values in the
entry field are invalid, the program will write out this
information and not draw the arc. The origin of the viewport
is shifted down in the client area so that it exists at the
upper-left corner of the viewable area.
