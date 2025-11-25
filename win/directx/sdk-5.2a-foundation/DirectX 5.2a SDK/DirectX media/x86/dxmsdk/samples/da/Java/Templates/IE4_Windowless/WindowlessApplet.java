//
// DirectX Media Windowless Control Template
// Copyright (c) 1997, Microsoft Corporation
//
// <Tutorial Section=1.0 Title="Constructing a Windowless Control">
//
// This template demonstrates how to construct an animation that
// makes use of IE4 windowless controls. It illustrates the following:
// - Using the modelmaker applet
// - Using the dxactrl (DirectAnimation windowless control)	
// - Constructing the model and passing it to the control
//
import com.ms.dxmedia.*;
import ie4winless_module.*;          // This gets ReactiveCube
import java.net.*;

// When designing windowless controls, the Java applet is used purely to construct
// the model. At runtime, hand the finished model over to the windowless 
// control to be displayed.
class WindowlessModel extends Model {
  public void createModel(BvrsToRun blist) {

    // Create any type of animation you like here... it is completely up to you 
    // what goes into the model.

    // </Tutorial>

    URL mediaBase = getImportBase();
    URL geoBase = buildURL(mediaBase, "geometry/"); 
    URL imgBase = buildURL(mediaBase, "image/"); 
    URL sndBase = buildURL(mediaBase, "sound/");

    // Here, the ReactiveCube module is used to create an
    // animated cube which scales up from zero and reacts to the mouse

    // Create a camera
		CameraBvr  Cam = 
			perspectiveCamera(toBvr(1),toBvr(0)).transform(			    
				translate(0,0,40) );						

    // Create some lighting
	  GeometryBvr lightGeo = 
      directionalLight.transform(translate(0,0,4));

    // Import the images used for the faces
    ImageBvr[] faces = {
      importImage(buildURL(imgBase, "hiddenBeachSG.jpg")),
      importImage(buildURL(imgBase, "hiddenBeachSS.jpg")),
      importImage(buildURL(imgBase, "arches.jpg")),
      importImage(buildURL(imgBase, "foliageUtah.jpg")),
      importImage(buildURL(imgBase, "tulipsHol1.jpg")),
      importImage(buildURL(imgBase, "redWoodCar.jpg"))
    };

    // Import a click snd
    SoundBvr clickSnd = importSound(buildURL(sndBase, "fishbuck.mp2"), null);

    // Instantiate a new ReactiveCube
    ReactiveCube cube = new ReactiveCube(faces, clickSnd);

    // Put some rotations on the cube to make it interesting
    GeometryBvr cubeGeo = cube.getGeometryBvr(mediaBase).transform(compose(compose(
      rotate(vector3(1,1,1),localTime),
      rotate(vector3(toBvr(Math.random()),toBvr(Math.random()),toBvr(Math.random())), div(localTime,toBvr(2)))),
      rotate(vector3(-1,1,-1),localTime)));      

    // merge the lighting and cube geometry
    cubeGeo = union(cubeGeo, lightGeo);

    // Make the cube hide or show when clicked:
    // First, declare a number which will go from 0 to 1 in 2 seconds
    NumberBvr grow = slowInSlowOut(0,1,2,1);
    // Second, declare and initialize a recursive number which goes from 0 to 1
    // and back to 0 again on each rightButtonDown event
    NumberBvr size = NumberBvr.newUninitBvr();
    size.init(until( grow, rightButtonDown, until( sub(toBvr(1),grow), rightButtonDown, size )));      
    // Third, have the cube scale up from zero initially and then start the previous behavior
    NumberBvr start = (NumberBvr)until( toBvr(0), timer(toBvr(2)), size );
    // Now apply it to the cube as a scale
    cubeGeo = cubeGeo.transform(scale3(start));

    // <Tutorial Section=1.1>
    // The image is simply the rendered cube without any background. 
    // Since you want to overlay the windowless control with other HTML
    // elements, specifically avoid a solidColor background.
    setImage(cubeGeo.render(Cam));
    setSound(cube.getSoundBvr());
  }
}

// Here is the important part, rather than extending DXMApplet as usual,
// extend ModelMakerApplet. Aside from the name, the construction
// is exactly the same as if you were doing a normal applet.
public class WindowlessApplet extends ModelMakerApplet {

  public void init() {
    // Always call the super classes init first to ensure codeBase is set.
    super.init() ;
    // Now set the model.
    setModel (new WindowlessModel());
  }
}

// </Tutorial>
