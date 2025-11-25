// <Tutorial Section=1.0 Title="Draggable Image">
//
/** This applet shows a red square that can be dragged around the applet.<BR>
Dragging actually consists of three parts: grabbing, dragging and releasing<BR>
Grabbing occurs when the left mouse button is pressed and held down over the<BR>
red square. Dragging happens when the mouse moves within the applet. During<BR>
this process, the color of the square changes to blue.  Releasing happens when<BR>
the left mouse button is released, and the square is dropped in that position,<BR>
with its color returning to red.<BR>

It illustrates the following:<BR><BR>
- making a square image draggable.<BR>
- getting events that trigger when the image is grabbed and released.<BR>
- changing the color of the square when it is being dragged.<BR>
- returning the square to its original color when it is released.<BR>
**/


import com.ms.dxmedia.*;    // All DirectAnimation classes
import java_utility.*;           // the DraggableImage class

public class ImageDrag extends DXMApplet  {
  public void init() {
    super.init() ;
    setModel (new ImagePickTestModel());
  }
}

// In the ImagePickTestModel class the createModel method is where you construct 
// your animation.
class ImagePickTestModel extends Model  {
  public void createModel(BvrsToRun listBvrs)  {

    // Create an uninitialized ColorBvr (clr).
    ColorBvr clr = ColorBvr.newUninitBvr();

    // Create cropped square, and apply clr's color behavior to it. 
    ImageBvr blockImg = solidColorImage(clr).
      crop(point2(toBvr(0), toBvr(0)),
        point2(toBvr(0.005), toBvr(0.005)));

    // Make blockImg draggable by creating a DraggableImage class
    // object (grabImg). 
    DraggableImage grabImg = new DraggableImage(blockImg, origin2);
    
    // Initialize clr.  Let it start out as red, change it to blue,
    // when the square is grabbed, and return to red when the square
    // is released.  The grab and release events are obtained from the
		// getGrabEvent() and getReleaseEvent() methods of the DraggableImage
		// class respectively.
    clr.init(until(red, grabImg.getGrabEvent(), 
      until(blue, grabImg.getReleaseEvent(), clr)));

    // Get the ImageBvr part of grabImg, by calling the getImageBvr() method
    // of DraggableImage.
    ImageBvr pickableBlockImg = grabImg.getImageBvr();

    // overlay pickableBlockImg on a black background.
    setImage(overlay(pickableBlockImg, solidColorImage(black)));
  }
}

// That's all there is to it.  Happy animating...
// </Tutorial>
