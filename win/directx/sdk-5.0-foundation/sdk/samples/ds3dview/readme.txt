Reality Lab / DirectSound 3D Viewer Sample

DS3DVIEW.EXE

This sample is a simple extension of the 3D object viewer sample that shipped
with DirectX 2.  It allows you to attach sounds to objects and move them around
a bit in 3D.

Viewer Notes:
-------------
The initial scene has a light source in the top right. To see this the view
can be moved backwards and forwards using the T and R keys. Additional objects
can be loaded from the file menu, and new light sources can be added from the
lights menu.  Objects can be rotated with the left mouse button and dragged
with the right.

The last object that was rotated or dragged is the current selection.

The device quality can be altered using the Renderer menu. Using this menu the
type of device used can also be switched from ramp to RGB. When using the RGB
device the colour of lights in the scene can be changed in the same way as you
change the colour of objects. In 256 colour mode it is advisable to dither an
RGB device for best results.



Additional keyboard controls

T               Forwards.
R               Back.
Z               Move current selection forwards.
X               Move current selection back.
G               Gouraud shade
F               Flat Shade
D               Dither toggle.
Delete  Delete the current selection.


Sound Notes:
------------

To attach a sound to an object, select the object and then select the "Attach Sound"
command from the Sound menu.  You can then select a WAV file to load.  To hear the
sound, select "Play Sound Once" or "Play Sound Looping."  To hear some neat effects,
try "Orbit" which will orbit the object/sound around the listener, or "Bullet,"
which will shoot the object/sound past the listener's head.

Properties for the sound or the listener can also be modified using the appropriate
menu commands and dialogs.


Current Known Issues for DX 3, Beta 1:
--------------------------------------
*  Double-cones are not currently implemented right -- the outer cone is simply set
   to be the same as the inside cone, which is always valid.
