// <Tutorial Section=1.0 Title="Using AFC Scrollbars">
//
/** This sample shows how to use DirectAnimation with the Applet Foundation<BR>
Classes (AFC). The scrollbar is created by using AFC's UIScrollBar class.<BR>
The speed of the rotation of the geometry is mapped to the position of the <BR>
scrollbar in DirectAnimation.<BR>

It illustrates the following:<BR><BR>
- using DirectAnimation along with AFC.<BR>
- creating a scrollbar in AFC.<BR>
- The use of ModifiableBehaviors.<BR>
**/

import com.ms.dxmedia.*; // All DirectAnimation classes
import com.ms.ui.*;      // The AFC classes
import java.awt.*;       // The event handle classes
import java.net.URL;     // The Java URL classes

// Use AFC's AwtUIApplet to construct the applet.
public class AFCScroll extends AwtUIApplet  {
  public void init()  {
	setLayout(new UIBorderLayout());

	// Get the AFC (scrollbar) part of the Applet.
	UIPanel ctrls = new AFCScrollControls();
    
	// Get the DirectAnimation part of the Applet. 
    Canvas cv = new DXMImg();

	// Position both parts.
	add("North",cv);
    add("South",ctrls);

	// The resize the DirectAnimation part, otherwise it will take over the 
    // entire viewport.
    cv.setSize(getSize().width - 15,getSize().height - 15);

    setBackground(Color.orange);
  }
}

// The scrollbar gets created in the AFCScrollControls Class.  
class AFCScrollControls extends UIPanel  {

  // Create the scrollbar using AFC's UIScrollBar class.
  AFCScrollControls()  {
    setLayout(new UIGridLayout(1, 1));
    _speedBar = new UIScrollBar(0, 0, 30, 3, 1, 15);
    _speedBar.setBackground(Color.orange);
    add(_speedBar);
  }

  // Handle relevant events when the user interacts with the scrollbar.
  public boolean handleEvent(Event e)  {
    switch (e.id)  {
      case Event.SCROLL_LINE_UP:
      case Event.SCROLL_LINE_DOWN:
      case Event.SCROLL_PAGE_UP:
      case Event.SCROLL_PAGE_DOWN:
      case Event.SCROLL_ABSOLUTE:

      // The position of the scrollbar sent to the setSpeed() of the 
      // ImgModel class. 
      ImgModel.setSpeed(_speedBar.getScrollPos());
      return true;
    }
    return false;
  }
  UIScrollBar _speedBar;
}

class DXMImg extends DXMCanvas  {
  DXMImg()  {
    setModel(new ImgModel());
  }
}

// In the ImgModel class the createModel method is where you construct 
// your animation.
class ImgModel extends Model  {
  public void createModel(BvrsToRun bvrs)  {

    // Import the cube.
    URL mediaBase = getImportBase();
    URL geoBase = buildURL(mediaBase, "geometry/");
    GeometryBvr geo = importGeometry(buildURL(geoBase, "cube.x"));

	// Create a ModifiableBehavior (_speed) which will be linked to the 
    // position of the scrollbar.  This behavior will then be used to set 
    // the speed of rotation for the cube.
    _speed = new ModifiableBehavior(toBvr(4));

    NumberBvr spinYRate = (NumberBvr)_speed.getBvr();
    NumberBvr spinZRate = mul(spinYRate, toBvr(1.3));
    geo = geo.transform(compose(rotate(yVector3, integral(spinYRate)),
                                rotate(zVector3, integral(spinZRate))));

    // Create a camera to view the cube.
    CameraBvr cam = perspectiveCamera(toBvr(5), toBvr(2));

	// Display the cube on a blue background.
    setImage(overlay(union(geo, directionalLight).render(cam)
     .transform(scale2(mul(toBvr(20),pixelBvr))),solidColorImage(blue)));
  }

  // This method is used in the AFCScrollControls class to change _speed.
  static void setSpeed(int speed)  {
    _speed.switchTo(toBvr(speed/3.0));
  }

  static ModifiableBehavior _speed;
}

// That's all there is to it.  Happy animating...
// </Tutorial>
