//
// Reactive Cube: A library module for interactive six sided cubes
// Dependencies: A geoBase containing Panel2.x
// Copyright (c) 1997, Microsoft Corporation
//
// The reactive cube is a constructed from six sides.  Each side of the 
// cube can have a separate texture that can be updated at runtime.  In
// addition, each side of the cube has a modifyable behavior when clicked.  
// The event caused by clicking on the side of the cube can be accessed by the 
// application for use elsewhere.
// 
// Methods of interest:
//   ReactiveCube(ImageBvr[]) - Constructs the cube with the given initial sides
//   setFace(ImageBvr) - Can be used to set the image on a particular side
//   getFaceEvent(int) - Gets the click event for a particular face
//   setEventTransform - Changes the transform used when a face is clicked on
//
package ie4winless_module;
import com.ms.dxmedia.*;
import java.net.*;

public class ReactiveCube extends Statics {	
	public ReactiveCube() {
		for(int i=0; i<6; i++) 
			_faceImgs[i] = new ModifiableBehavior(solidColorImage(white));
	}

	public ReactiveCube(ImageBvr[] faceImgs, SoundBvr clickSnd) {
		if(faceImgs.length<6) {
		for(int i=0; i<6; i++) 
			_faceImgs[i] = new ModifiableBehavior(solidColorImage(white));
			
		}
    else {
		  for(int i=0; i<6; i++) 
			  _faceImgs[i] = new ModifiableBehavior(faceImgs[i]);
    }
    _clickSnd = clickSnd;
	}

	public void setFace(int index, ImageBvr faceImg) { _faceImgs[index].switchTo(faceImg); }
	public DXMEvent getFaceEvent(int index) { return _facePicks[index]; }
  public void setEventTransform(Transform3Bvr evXF) { _evXF = evXF; }
  public SoundBvr getSoundBvr() { return _pickSnd; }

  public GeometryBvr getGeometryBvr(URL mediaBase) {
    URL geoBase = buildURL(mediaBase, "geometry/");     

    GeometryBvr faceGeo = 
      importGeometry(buildURL(geoBase,"panel2.x"));

    GeometryBvr movingFaceGeo = faceGeo.transform(
      (_evXF == null) ? 
      translate(toBvr(0), toBvr(0), mul(toBvr(.5), sin(mul(toBvr(Math.PI/2),localTime)))) :
        _evXF);
     
    GeometryBvr cubeGeo = emptyGeometry;    
    SoundBvr cubeSnd = silence;
    for(int i=0; i<6; i++) {
      PickableGeometry reactiveFaceGeo = new PickableGeometry(faceGeo);
      _facePicks[i] = andEvent(leftButtonDown, reactiveFaceGeo.getPickEvent());
      GeometryBvr liveFaceGeo = GeometryBvr.newUninitBvr();
      liveFaceGeo.init(until( reactiveFaceGeo.getGeometryBvr(), _facePicks[i], 
        until( movingFaceGeo, timer(toBvr(2)), liveFaceGeo )));

      GeometryBvr cubeFaceGeo = emptyGeometry;
      switch(i) {
      case 0:
        cubeFaceGeo = liveFaceGeo.transform(translate(toBvr(0), toBvr(0), toBvr(1)));
        break;
      case 1:
        cubeFaceGeo = liveFaceGeo.transform(
          compose(translate(toBvr(0), toBvr(0), toBvr(-1)), rotate(yVector3, toBvr(Math.PI))));
        break;
      case 2:
        cubeFaceGeo = liveFaceGeo.transform(
          compose(translate(toBvr(1), toBvr(0), toBvr(0)), rotate(yVector3, toBvr(Math.PI/2))));
        break;
      case 3:
        cubeFaceGeo = liveFaceGeo.transform(
          compose(translate(toBvr(-1), toBvr(0), toBvr(0)), rotate(yVector3, toBvr(-Math.PI/2))));
        break;
      case 4:
        cubeFaceGeo = liveFaceGeo.transform(
          compose(translate(toBvr(0), toBvr(1), toBvr(0)), rotate(xVector3, toBvr(-Math.PI/2))));
        break;
      case 5:
        cubeFaceGeo = liveFaceGeo.transform(
          compose(translate(toBvr(0), toBvr(-1), toBvr(0)), rotate(xVector3, toBvr(Math.PI/2))));
        break;
      }

      SoundBvr clickSnd = SoundBvr.newUninitBvr();
      clickSnd.init(until( silence, _facePicks[i], mix( _clickSnd, clickSnd ) ) );
      cubeSnd = mix(cubeSnd, clickSnd);
      cubeGeo = union(cubeGeo, cubeFaceGeo.texture(
        ((ImageBvr)_faceImgs[i].getBvr()).mapToUnitSquare() ));
    }
    _pickSnd = cubeSnd;
    return cubeGeo;
  }

  Transform3Bvr _evXF = null;   
	ModifiableBehavior[] _faceImgs = new ModifiableBehavior[6];
  DXMEvent[] _facePicks = new DXMEvent[6];
  SoundBvr _clickSnd = silence;
  SoundBvr _pickSnd = silence;
}