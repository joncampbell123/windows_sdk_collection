// <Tutorial Section=1.0 Title="Draggable Geometry">
//
/** This applet shows a red cube that can be dragged around the applet.<BR>
Dragging actually consists of three parts: grabbing, dragging and releasing.<BR>
Grabbing occurs when the left mouse button is pressed and held down over the<BR>
red cube. Dragging happens when the mouse moves within the applet. During<BR>
this process, the color of the cube changes to blue.  Releasing happens when<BR>
the left mouse button is released, and the cube is dropped in that position,<BR>
with its color returning to red.<BR>

It illustrates the following:<BR><BR>
- making a imported cube draggable.<BR>
- getting events that trigger when the cube is grabbed and released.<BR>
- changing the color of the cube when it is being dragged.<BR>
- returning the cube to its original color when it is released.<BR>
**/



import com.ms.dxmedia.*; // All DirectAnimation classes
import java_utility.*;  // the DraggableImage class
import java.net.*; // the Java URL classes

public class GeometryDrag extends DXMApplet {
  public void init() {
    super.init() ;
    setModel (new GeometryPickTestModel());
  }
}

// In the GeometryPickTestModel class the createModel method is where you construct 
// your animation.
class GeometryPickTestModel extends Model {
  public void createModel(BvrsToRun listBvrs) {

		// Import the cube rotate it and scale it down.
    URL mediaBase = getImportBase();
    URL geoBase = buildURL(mediaBase,"geometry/");
    GeometryBvr geo = importGeometry(buildURL(geoBase,"cube.x"));	
    geo = geo.transform(compose(
            rotate(vector3(1,1,1), localTime),
            scale3(1*cm) ));
		
    // Make geo draggable by creating a DraggableGeometry class
    // object (grabGeo). 
    grabGeo = new DraggableGeometry(geo, origin3);

    // Initialize clr.  Let it start out as red, change it to blue,
    // when the cube is grabbed, and return to red when the cube
    // is released.  The grab and release events are obtained from the
		// getGrabEvent() and getReleaseEvent() methods of the DraggableGeometry
		// class respectively.
	  ColorBvr clr = ColorBvr.newUninitBvr();
	  clr.init(until(red, grabGeo.getGrabEvent(), 
			until(blue, grabGeo.getReleaseEvent(), clr)));

		// Get the GeometryBvr part of grabGeo, by calling the getGeometryBvr() 
		// method of DraggableGeometry.
    GeometryBvr pickableGeo = grabGeo.getGeometryBvr();  
		
    // Apply clr to pickableGeo.   
    pickableGeo = pickableGeo.diffuseColor(clr);


    pickableGeo = union(pickableGeo, directionalLight);
        
    CameraBvr cam = perspectiveCamera(toBvr(10*cm),toBvr(5*cm));
		
		// overlay the rendered on a black background.
    setImage(overlay(pickableGeo.render(cam), solidColorImage(black)));                
  }
  public void cleanup() {
    super.cleanup();
	grabGeo = null;
  }
  DraggableGeometry grabGeo;
}

// That's all there is to it.  Happy animating...
// </Tutorial>

