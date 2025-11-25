//
// DraggableImage: Creates an image which can be dragged by the mouse
// Dependencies: TagWrapper.java, TaggedNotifier.java
// Copyright (c) 1997, Microsoft Corporation
//
// Used to create an image which you can click on and drag.  This class
// has three public methods for getting data:
//   getImageBvr() returns the new pickable image
//   getGrabEvent() returns a DXMEvent which fires when a drag begins
//   getReleaseEvent() returns a DXMEvent which fires when a drag ends
//
package java_utility;
import com.ms.dxmedia.*;

public class DraggableImage extends Statics implements UntilNotifier {

  public DraggableImage(ImageBvr im, Point2Bvr pt) {

    PickableImage pim = new PickableImage(im);
    _pickEv = andEvent(leftButtonDown, pim.getPickEvent());
    // construct a handler for release events,
    // upon pick events it passes the puck back to this object.
    _releaseHdlr = new ReleaseHandlerImg(_pickEv, this);    
    _draggablePt = (Point2Bvr)
        untilNotify(pt, _pickEv, this).runOnce(); 
	  _releaseEv = leftButtonUp.snapshotEvent(_draggablePt);
    _draggableImg =
        pim.getImageBvr().transform(translate(sub(_draggablePt, origin2)));
  }

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun listBvrs) {

        Point2Bvr currPt = (Point2Bvr) currRunningBvr;

        // pull apart the pair that comes from andEvent and from the
        // pick event pair, ultimately to get at the local mouse 
        PairObject andEventPair = (PairObject) evData;
        
        PairObject    pickPair   = (PairObject)(andEventPair.getSecond());

        Vector2Bvr localMouse = (Vector2Bvr)(pickPair.getSecond());

        Point2Bvr  draggingPt = currPt.transform(translate(localMouse));

        return (Point2Bvr) untilNotify(draggingPt,
                             _releaseEv, 
                               _releaseHdlr);
  }

  DXMEvent _pickEv;
  DXMEvent _releaseEv;
  ImageBvr _draggableImg;
  Point2Bvr _draggablePt;
  ReleaseHandlerImg _releaseHdlr;

  public ImageBvr getImageBvr() { return _draggableImg; }
  public DXMEvent getGrabEvent() { return _pickEv; }
  public DXMEvent getReleaseEvent() { return _releaseEv; } 
  public Point2Bvr getPointBvr() { return _draggablePt; }
}



class ReleaseHandlerImg extends Statics implements UntilNotifier {

  public ReleaseHandlerImg(DXMEvent pickEv, DraggableImage grabHdlr) {

    _pickEv = pickEv;
    _grabHdlr = grabHdlr;
  }

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun listBvrs) {

	  // Releasing.  Freeze the image where it is, and go 
    // back to waiting for a pick event.
    Point2Bvr snappedPt = (Point2Bvr) evData;
    return (Point2Bvr)
          untilNotify(snappedPt, _pickEv, _grabHdlr);
  }

  DXMEvent _pickEv;
  DraggableImage _grabHdlr;
}	