//
// DraggableGeometry: Creates a geometry which can be dragged by the mouse
// Dependencies: TagWrapper.java, TaggedNotifier.java
// Copyright (c) 1997, Microsoft Corporation
//
// Used to create an image which you can click on and drag.  This class
// has three public methods for getting data:
//   getGeometryBvr() returns the new pickable geometry
//   getGrabEvent() returns a DXMEvent which fires when a drag begins
//   getReleaseEvent() returns a DXMEvent which fires when a drag ends
//
package java_utility;
import com.ms.dxmedia.*;

public class DraggableGeometry extends Statics implements UntilNotifier {

  public DraggableGeometry(GeometryBvr geo, Point3Bvr pt) {
	  _releaseEv = new AppTriggeredEvent();
    PickableGeometry pgeo = new PickableGeometry(geo);
    _pickEv = andEvent(leftButtonDown, pgeo.getPickEvent());
    // construct a handler for release events,
    // upon pick events it passes the puck back to this object.
    _releaseHdlr = new ReleaseHandlerGeo(_pickEv, this);     
    _draggablePt = (Point3Bvr)
        untilNotify(pt, _pickEv, this).runOnce(); 
	  _releaseEv = leftButtonUp.snapshotEvent(_draggablePt);
    _draggableGeo =
        pgeo.getGeometryBvr().transform(translate(sub(_draggablePt, origin3)));
  }

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun listBvrs) {

    Point3Bvr currPt = (Point3Bvr) currRunningBvr;

    // pull apart the pair that comes from andEvent and from the
    // pick event pair, ultimately to get at the displacement vector 
    PairObject andEventPair = (PairObject) evData;
        
    PairObject    pickPair   = (PairObject)(andEventPair.getSecond());

    Vector3Bvr localMouse = (Vector3Bvr)(pickPair.getSecond());

    Point3Bvr  draggingPt = currPt.transform(translate(localMouse));

    return (Point3Bvr) untilNotify(draggingPt,
                             _releaseEv, 
                               _releaseHdlr);
  }

  DXMEvent _pickEv;
  DXMEvent _releaseEv;
  GeometryBvr _draggableGeo;
  Point3Bvr _draggablePt;
  ReleaseHandlerGeo _releaseHdlr;

  public GeometryBvr getGeometryBvr() { return _draggableGeo; }
  public DXMEvent getGrabEvent() { return _pickEv; }
  public DXMEvent getReleaseEvent() { return _releaseEv; } 
  public Point3Bvr getPointBvr() { return _draggablePt; }
}
  
class ReleaseHandlerGeo extends Statics implements UntilNotifier {

  public ReleaseHandlerGeo(DXMEvent pickEv, DraggableGeometry grabHdlr) {

    _pickEv = pickEv;
    _grabHdlr = grabHdlr;
  }

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun listBvrs) {

	  // Releasing.  Freeze the geometry where it is, and go 
    // back to waiting for a pick event.
    Point3Bvr snappedPt = (Point3Bvr) evData;
    return (Point3Bvr)
          untilNotify(snappedPt, _pickEv, _grabHdlr);
  }

  DXMEvent _pickEv;
  DraggableGeometry _grabHdlr;
}	

