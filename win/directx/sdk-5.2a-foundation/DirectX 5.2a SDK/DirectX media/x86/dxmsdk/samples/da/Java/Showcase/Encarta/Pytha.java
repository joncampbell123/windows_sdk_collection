// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*; //added to support URL's

public class Pytha extends Statics {

  final static double x = 0.02;

  Pytha(URL imgBase, URL sndBase) {

    _imgBase = imgBase;
    _sndBase = sndBase;

    Point2Bvr ptA = point2(toBvr(x), toBvr(0));
    Point2Bvr ptB = point2(toBvr(0), toBvr(3*x/4));
    Point2Bvr ptC = origin2;

    _resetPythaPtsEv = new AppTriggeredEvent();
    _startPythaEv = new AppTriggeredEvent();

    ImageBvr imgStart = importImage(buildURL(imgBase, "start.gif"), emptyImage, null, null, null);
    ImageBvr imgReset = importImage(buildURL(imgBase, "reset.gif"), emptyImage, null, null, null);
    _tri = new Triangle(ptA, ptB, ptC, this, imgStart, imgReset);
  }

  public ImageBvr img() {
    return _tri._img;
  }

  public SoundBvr snd() {
    return _tri._snd;
  }

  public void cleanup() {
    _tri.cleanup();
	_tri = null;
    _resetPythaPtsEv = null;
	_startPythaEv = null;
    _imgBase = null;
    _sndBase = null;
  }

  Triangle _tri;
  AppTriggeredEvent _resetPythaPtsEv, _startPythaEv;
  static URL _imgBase;
  static URL _sndBase;
}

class Triangle extends Statics {

  Triangle(Point2Bvr ptA, Point2Bvr ptB, Point2Bvr ptC, Pytha pytha,
           ImageBvr imgStart, ImageBvr imgReset) {

    //          C|\
    //           |  \
    //           |    \
    //           |______\
    //          C        A
    // Create 3 draggable objects, one for each point in the triangle.
    Draggable draggableC = new Draggable(ptC, magenta, pytha._resetPythaPtsEv);
    DraggableAngle drAngle = new DraggableAngle(ptA, ptB, draggableC._draggablePt, pytha._resetPythaPtsEv);

    // Create a draggable triangle from the coordinates of the
    // 3 draggable objects.
    Point2Bvr pts[] = {drAngle._draggablePtA,
                       drAngle._draggablePtB,
                       draggableC._draggablePt};
    Draggable draggableTri = new Draggable(myPolygon(pts, blue), pytha._resetPythaPtsEv);

    // Create the anchors and calculate the coordinates of the dragged
    // triangle.
    Vector2Bvr dragVec = sub(draggableTri._draggablePt, origin2);

    ImageBvr anchorA = drAngle._draggableImgA.transform(translate(dragVec));
    ImageBvr anchorB = drAngle._draggableImgB.transform(translate(dragVec));
    ImageBvr anchorC = draggableC._draggableImg.transform(translate(dragVec));

    Point2Bvr dragPtAPre = drAngle._draggablePtA.transform(translate(dragVec));
    Point2Bvr dragPtBPre = drAngle._draggablePtB.transform(translate(dragVec));
    Point2Bvr dragPtC = draggableC._draggablePt.transform(translate(dragVec));

    // Calculate the angle C.
    NumberBvr radianC = angle(sub(dragPtAPre, dragPtC), sub(dragPtBPre, dragPtC));

    // If angle C is negative (when a user drags anchor C to the other side
    // of the hypotenuse), swap ptA and ptB so that
    // ptA, ptC, ptB are always in the clockwise order.  This would ensure
    // the rectangles are properly created.
    Point2Bvr dragPtA = (Point2Bvr)cond(lt(radianC, toBvr(0)),
                                        dragPtBPre,
                                        dragPtAPre);
    Point2Bvr dragPtB = (Point2Bvr)cond(lt(radianC, toBvr(0)),
                                        dragPtAPre,
                                        dragPtBPre);

    //               / \
    //              /C4  \
    //             /       \
    //       (ptB)/          \
    //     ______/ C1      C3/
    //  A3|    A2|\         /
    //    |      |  \      /
    //    |      |    \ C2/
    //    |______|______\/
    //  A4     A1|B2    |B1(ptA)
    //      (ptC)|      |
    //           |      |
    //           |______|
    //            B3     B4
    // Initialize the coordinates we'll need in the proof.
    _ptA1 = dragPtC;
    _ptA2 = dragPtB;
    _ptA3 = add(dragPtB, sub(dragPtC, dragPtB).transform(rotate(toBvr(-Math.PI/2))));
    _ptA4 = add(dragPtC, sub(dragPtB, dragPtC).transform(rotate(toBvr(Math.PI/2))));
    _ptB1 = dragPtA;
    _ptB2 = dragPtC;
    _ptB3 = add(dragPtC, sub(dragPtA, dragPtC).transform(rotate(toBvr(-Math.PI/2))));
    _ptB4 = add(dragPtA, sub(dragPtC, dragPtA).transform(rotate(toBvr(Math.PI/2))));
    _ptC1 = dragPtB;
    _ptC2 = dragPtA;
    _ptC3 = add(dragPtA, sub(dragPtB, dragPtA).transform(rotate(toBvr(-Math.PI/2))));
    _ptC4 = add(dragPtB, sub(dragPtA, dragPtB).transform(rotate(toBvr(Math.PI/2))));

    // _reset is true if the the animated proof is not running, false if it is.
    _reset = true;

    PickableImage resetButton =
      new PickableImage(imgReset.
        transform(translate(mul(toBvr(-45),pixelBvr),
				  mul(toBvr(-130),pixelBvr))));
    PickableImage startButton =
      new PickableImage(imgStart.
        transform(translate(mul(toBvr(45),pixelBvr),
				  mul(toBvr(-130),pixelBvr))));

    // The events that starts and resets the proof.
    _resetEv = andEvent(leftButtonDown, resetButton.getPickEvent());
    //_startEv = andEvent(leftButtonDown, startButton.getPickEvent());
    _startEv = orEvent(andEvent(leftButtonDown, startButton.getPickEvent()),
                       pytha._startPythaEv);

    // imgFixed is the part of image that remains the same whether the proof
    // is running or not.  This includes the triangle and the rectangle C.
    ImageBvr triangle = draggableTri._draggableImg;
    Point2Bvr ptsC[] = {_ptC1, _ptC2, _ptC3, _ptC4};
    ImageBvr imgFixed = overlay(myPolygon(ptsC, cyan), triangle);

    // imgReactive is the part of image that changes when the proof is running.
    _imgReset = imgReset();
    _imgProof = imgProof();

    ImageBvr imgReactive = ImageBvr.newUninitBvr();
    imgReactive.init(until(_imgReset, _startEv,
                           until(_imgProof, _resetEv, imgReactive)));


    // Label the sides A, B, and C for visual aid.
		FontStyleBvr fs = defaultFont.color(black).bold();
      
    ImageBvr strA = stringImage(toBvr("a"), fs).transform(
      compose(translate(sub(add(dragPtC,
        sub(dragPtB, dragPtC).div(toBvr(2))),origin2)),
          scale2(toBvr(1.5))));

    ImageBvr strB = stringImage(toBvr("b"), fs).transform(
      compose(translate(sub(add(dragPtC,
        sub(dragPtA, dragPtC).div(toBvr(2))),origin2)),
          scale2(toBvr(1.5))));

    ImageBvr strC = stringImage(toBvr("c"), fs).transform(
      compose(translate(sub(add(dragPtA,
        sub(dragPtB, dragPtA).div(toBvr(2))),origin2)),
          scale2(toBvr(1.5))));

    // Create a text image for angle C for display.
    ImageBvr strAngleC =
      stringImage(concat(toBvr("Angle C = "),
                        radiansToDegrees(abs(radianC)).toString(toBvr(2))),fs)
        .transform(translate(mul(toBvr(-70), pixelBvr),
                             mul(toBvr(120), pixelBvr)));

    // Composite all the images together.

    ImageBvr imgText =
           overlay(overlay(strAngleC,
                           overlay(anchorA, overlay(anchorB, anchorC))),
                   overlay(strA, overlay(strB, strC)));

    ImageBvr bkgnd = importImage(buildURL(Pytha._imgBase,"epad512.gif"), emptyImage, null, null, null).
                      crop(point2(mul(toBvr(-150),pixelBvr),
											  mul(toBvr(-150),pixelBvr)),
                        point2(mul(toBvr(150),pixelBvr),
												mul(toBvr(150),pixelBvr)));
    _img = overlay(overlay(overlay(startButton.getImageBvr(), resetButton.getImageBvr()), imgText),
                   overlay(overlay(imgReactive, imgFixed), bkgnd));

    // Don't forget the sound.
    PythaSound snd = new PythaSound(_startEv, _resetEv);
    _snd = snd._snd;
  }

  ImageBvr myPolygon(Point2Bvr[] pts, ColorBvr clr) {

    Path2Bvr p = polyline(pts);
    return overlay(p.close().draw(defaultLineStyle),
                   solidColorImage(clr).clip(fillMatte(p)));
  }

  ImageBvr imgReset() {

    Point2Bvr ptsA[] = {_ptA1, _ptA2, _ptA3, _ptA4};
    Point2Bvr ptsB[] = {_ptB1, _ptB2, _ptB3, _ptB4};

    return overlay(myPolygon(ptsA, yellow),
                   myPolygon(ptsB, green));
  }

  NumberBvr smooth0to1() {
    return (NumberBvr)until(mul(localTime, toBvr(0.6)),
                            timer(toBvr(1.5)), toBvr(1));
  }

  Point2Bvr intersect(Point2Bvr p1, Point2Bvr p2, Point2Bvr p3, Point2Bvr p4) {

    NumberBvr yDiff1 = sub(p2.getY(), p1.getY());
    NumberBvr xDiff1 = sub(p2.getX(), p1.getX());
    NumberBvr yDiff2 = sub(p4.getY(), p3.getY());
    NumberBvr xDiff2 = sub(p4.getX(), p3.getX());

    NumberBvr y = div(sub(mul(yDiff2, sub(mul(yDiff1, p1.getX()),
                                          mul(xDiff1, p1.getY()))),
                          mul(yDiff1, sub(mul(yDiff2, p3.getX()),
                                          mul(xDiff2, p3.getY())))),
                      sub(mul(yDiff1, xDiff2), mul(yDiff2, xDiff1)));

    NumberBvr x = div(sub(mul(xDiff2, sub(mul(yDiff1, p1.getX()),
                                          mul(xDiff1, p1.getY()))),
                          mul(xDiff1, sub(mul(yDiff2, p3.getX()),
                                          mul(xDiff2, p3.getY())))),
                      sub(mul(xDiff2, yDiff1), mul(xDiff1, yDiff2)));

    return point2(x, y);
  }

  Point2Bvr rotateAround(Point2Bvr pt, Point2Bvr ptPivot, double degree) {
    return pt.transform(compose(translate(sub(ptPivot, origin2)),
                    compose(rotate(mul(toBvr(degree), smooth0to1())),
                      translate(sub(origin2, ptPivot)))));
  }

  static NumberBvr angle(Vector2Bvr v1, Vector2Bvr v2) {
    NumberBvr len = mul(v2.length(), v1.length());
    NumberBvr dotP = dot(v2, v1);

    // If any one of the vectors is a zero vector, don't do the division,
    // just return 0 angle (cos(0) = 1)
    NumberBvr num = (NumberBvr)cond(eq(len, toBvr(0)),
                                    toBvr(1),
                                    div(dotP, len));

    // Due to the limited precision in the floating point arithmetic, num
    // can be slightly bigger than 1 sometimes.  Set to 1 when it happens.
    num = (NumberBvr)cond(gt(abs(num), toBvr(1)),
                          cond(gt(num, toBvr(1)), toBvr(1), toBvr(-1)),
                          num);

    NumberBvr theta = acos(num);

    BooleanBvr isAntiClock =
      (BooleanBvr)cond(gt(v1.getY(), toBvr(0)),
                       lte(sub(v2.getX(), mul(div(v1.getX(), v1.getY()), v2.getY())), toBvr(0)),
                       cond(lt(v1.getY(), toBvr(0)),
                            gte(sub(v2.getX(), mul(div(v1.getX(), v1.getY()), v2.getY())), toBvr(0)),
                            cond(gte(v1.getX(), toBvr(0)),
                                 gte(v2.getY(), toBvr(0)),
                                 lte(v2.getY(), toBvr(0)))));

    return (NumberBvr)cond(isAntiClock, theta, neg(theta));
  }

  ImageBvr imgProof() {

    Point2Bvr newA1 = intersect(_ptA1, _ptA4, _ptC1, _ptC2);
    Vector2Bvr vecShear1A = sub(newA1, _ptA1);
    Vector2Bvr vecShear1AAnim = (Vector2Bvr)vecShear1A.mul(smooth0to1());
    DXMEvent evRotate1 = timer(toBvr(2));
    DXMEvent evShear2 = timer(toBvr(2));
    NumberBvr theta = angle(sub(_ptA1, _ptA2), sub(_ptC2, _ptC1));
    NumberBvr len = mul(sin(theta), sub(_ptA2, _ptA1).length());
    Vector2Bvr norm = sub(_ptC3, _ptC2).normalize();
    Vector2Bvr vShear2 = norm.mul(len);

    Vector2Bvr vecShear2A = (Vector2Bvr)until(zeroVector2,
                                              evShear2,
                                              vShear2.mul(smooth0to1()));

    Point2Bvr animA1 = (Point2Bvr)until(add(_ptA1, vecShear1AAnim),
                                        evRotate1,
                                        rotateAround(add(_ptA1, vecShear1A), _ptA2, Math.PI/2));
    Point2Bvr animA3 = (Point2Bvr)until(_ptA3,
                                        evRotate1,
                                        add(rotateAround(_ptA3, _ptA2, Math.PI/2), vecShear2A));
    Point2Bvr animA4 = (Point2Bvr)until(add(_ptA4, vecShear1AAnim),
                                        evRotate1,
                                        add(rotateAround(add(_ptA4, vecShear1A), _ptA2, Math.PI/2),
                                            vecShear2A));

    Point2Bvr newB2 = intersect(_ptB2, _ptB3, _ptC1, _ptC2);
    Vector2Bvr vecShear1B = sub(newB2, _ptB2);
    Vector2Bvr vecShear1BAnim = (Vector2Bvr)until(zeroVector2,
                                  timer(toBvr(6)),
                                  vecShear1B.mul(smooth0to1()));

    DXMEvent evRotate2 = timer(toBvr(8));
    Vector2Bvr vecShear2B = (Vector2Bvr)until(zeroVector2,
                                              evShear2,
                                              vShear2.mul(smooth0to1()));

    Point2Bvr animB2 = (Point2Bvr)until(add(_ptB2, vecShear1BAnim),
                                        evRotate2,
                                        rotateAround(add(_ptB2, vecShear1B), _ptB1, -Math.PI/2));
    Point2Bvr animB3 = (Point2Bvr)until(add(_ptB3, vecShear1BAnim),
                                        evRotate2,
                                        add(rotateAround(add(_ptB3, vecShear1B), _ptB1, -Math.PI/2), vecShear2B));

    Point2Bvr animB4 = (Point2Bvr)until(_ptB4,
                                        evRotate2,
                                        add(rotateAround(_ptB4, _ptB1,  -Math.PI/2), vecShear2B));

    Point2Bvr ptsA[] = {animA1, _ptA2, animA3, animA4};
    Point2Bvr ptsB[] = {_ptB1, animB2, animB3, animB4};

    return overlay(myPolygon(ptsB, green).opacity(toBvr(0.5)),
                   myPolygon(ptsA, yellow).opacity(toBvr(0.5)));
  }

  public void cleanup() {
    _resetEv = null;
	_startEv = null;
    _ptA1 = _ptA2 = _ptA3 = _ptA4= null;
    _ptB1 = _ptB2 = _ptB3 = _ptB4= null;
    _ptC1 = _ptC2 = _ptC3 = _ptC4= null;
    _img = _imgProof = _imgReset = null;
    _snd = rotSnd = null;
  }


  static DXMEvent _resetEv, _startEv;
  Point2Bvr _ptA1, _ptA2, _ptA3, _ptA4;
  Point2Bvr _ptB1, _ptB2, _ptB3, _ptB4;
  Point2Bvr _ptC1, _ptC2, _ptC3, _ptC4;
  ImageBvr _img, _imgProof, _imgReset;
  boolean _reset;
  SoundBvr _snd, rotSnd;
}

class DraggableNotifier extends Statics implements UntilNotifier {
  DraggableNotifier(Draggable dra) {
    _dra = dra;
    _releaseEv = leftButtonUp;
  }

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun blst) {

    //System.out.println("draggableNotifier _picked");
    Point2Bvr currPt = (Point2Bvr)currRunningBvr;

    if (_picked == false) {

      // pull apart the pair that comes from andEvent and from the
      // pick event pair, ultimately to get at the local mouse 
      // coord behavior.
      //System.out.println("draggableNotifier _picked"+evData.getClass());
      PairObject andEventPair  = (PairObject)evData;
      PairObject pickPair   = (PairObject)(andEventPair.getSecond());
      Vector2Bvr localMouse = (Vector2Bvr)(pickPair.getSecond());

      Point2Bvr  draggingPt =
                   currPt.transform(translate(localMouse));

      // set the state to saying that we've been picked.
      _picked = true;

      return untilNotify(draggingPt,
                         _releaseEv.snapshotEvent(draggingPt),
                         this);

    } else {

      //System.out.println("draggableNotifier _released"+evData.getClass());
      // Releasing.  Freeze the image where it is, and go 
      // back to waiting for a pick event.
      Point2Bvr snappedPt = (Point2Bvr) evData;

      // set the state to saying we're waiting to be picked again.
      _picked = false;

      return _dra.tracker(snappedPt);
    }
  }
  public void cleanup() {
    _releaseEv = null;
    _dra.cleanup();
	_dra = null;
  }


  boolean _picked = false;
  DXMEvent _releaseEv;
  Draggable _dra;
}

class Draggable extends Statics implements UntilNotifier {
  public Draggable(Point2Bvr pt, ColorBvr clr, DXMEvent resetEv) {
    ImageBvr im = solidColorImage(clr).crop(point2(-0.001, -0.001),
                                            point2(0.001, 0.001));

    init(im, pt, resetEv);
  }

  public Draggable(ImageBvr im,  DXMEvent resetEv) {
    init(im, origin2, resetEv);
  }

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun blst) {

    //System.out.println("draggable reset");
    return tracker(_pt);
  }

  Point2Bvr tracker(Point2Bvr pt) {
    //System.out.println("tracker"+_notifier);
    return (Point2Bvr)untilEx(pt, orEvent(_pickEv.notifyEvent(_notifier),
                                        _resetEv.notifyEvent(this)));
  }

  void init(ImageBvr im, Point2Bvr pt, DXMEvent resetEv) {

    _pt = pt;
    PickableImage pim = new PickableImage(im);
    _pickEv = andEvent(leftButtonDown, pim.getPickEvent());
    _resetEv = resetEv;

    _notifier = new DraggableNotifier(this);
    //System.out.println("_notifier = "+_notifier.getClass());
    // Wait for the first pick.
    _draggablePt = (Point2Bvr)tracker(pt).runOnce();
    _draggableImg = pim.getImageBvr().transform(translate(sub(_draggablePt, origin2)));
  }

  public void cleanup() {
    _notifier.cleanup();
    _notifier = null;
    _pickEv = _resetEv = null;
    _draggableImg = null;
    _draggablePt = _pt = null;
  }


  DraggableNotifier _notifier;
  DXMEvent          _pickEv, _resetEv;
  ImageBvr          _draggableImg;
  Point2Bvr         _draggablePt, _pt;
}

class DraggableAngleNotifier extends Statics implements UntilNotifier {
  DraggableAngleNotifier(DraggableAngle dra, Point2Bvr pt, int evType) {
    _pt = pt;
    _evType = evType;
    _dra = dra;
    _releaseEv = leftButtonUp;
  }

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun blst) {

    int iDragging, iTagAlong;

    if (_evType == DraggableAngle.DRAGGING_PTA) {
      iDragging = 0;
      iTagAlong = 1;
    } else {
      iDragging = 1;
      iTagAlong = 0;
    }

    if (_picked == false) {

      Point2Bvr currPt = (Point2Bvr)((TupleBvr)currRunningBvr).nth(iDragging);
      Point2Bvr tagAlongPt = (Point2Bvr)((TupleBvr)currRunningBvr).nth(iTagAlong);

      // set the state to saying that we've been picked.
      _picked = true;

      // pull apart the pair that comes from andEvent and from the
      // pick event pair, ultimately to get at the local mouse
      // coord behavior.
      PairObject    pickEventPair  = (PairObject)evData;
      PairObject    pickPair   = (PairObject)(pickEventPair.getSecond());
      Vector2Bvr localMouse = (Vector2Bvr)(pickPair.getSecond());
      Point2Bvr draggingPt = currPt.transform(translate(localMouse));

      NumberBvr theta = Triangle.angle(sub(currPt, _pt),
                                       sub(draggingPt, _pt));

      Transform2Bvr xf = compose(translate(sub(_pt, origin2)),
                                 compose(rotate(theta),
                                         translate(sub(origin2, _pt))));

      tagAlongPt = tagAlongPt.transform(xf);

      return untilNotify(pairBvr(draggingPt, tagAlongPt),
                         _releaseEv.snapshotEvent(pairBvr(draggingPt, tagAlongPt)),
                         this);
    } else {

      //System.out.println("draggableAngle notifier _released");
      // Releasing.  Freeze the image where it is, and go
      // back to waiting for a pick event.
      Point2Bvr snappedDraggingPt = (Point2Bvr) ((TupleBvr) evData).nth(iDragging);
      Point2Bvr snappedTagAlongPt = (Point2Bvr) ((TupleBvr) evData).nth(iTagAlong);

      // set the state to saying we're waiting to be picked again.
      _picked = false;

      return _dra.tracker(snappedDraggingPt, snappedTagAlongPt);
    }
  }

  public void cleanup() {
    _releaseEv = null;
    _dra.cleanup();
	_dra =  null;
    _pt = null;
  }

  DXMEvent _releaseEv;
  boolean _picked = false;
  DraggableAngle _dra;
  Point2Bvr _pt;
  int _evType;
}

class DraggableAngle extends Statics implements UntilNotifier {

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun blst) {

    return tracker(_ptA, _ptB);
  }

  // Create two draggable objects at the given locations - ptA, and ptB.
  // When either of the two objects is dragged upon, the other will move along
  // so that the angle created by the two vectors, ptA - ptC and ptB - ptC,
  // remain the same.

  TupleBvr tracker(Point2Bvr pt1, Point2Bvr pt2) {
    return (TupleBvr)untilEx(pairBvr(pt1, pt2),
                             orEvent(orEvent(_evA.notifyEvent(_nA),
                                             _evB.notifyEvent(_nB)),
                                             _resetEv.notifyEvent(this)));
  }

  DraggableAngle(Point2Bvr ptA, Point2Bvr ptB, Point2Bvr ptC, DXMEvent resetEv) {

    _ptA = ptA;
    _ptB = ptB;
    PickableImage pimA = init(ptA, red);
    PickableImage pimB = init(ptB, red);

    _evA = andEvent(leftButtonDown, pimA.getPickEvent());
    _evB = andEvent(leftButtonDown, pimB.getPickEvent());
    _resetEv = resetEv;

    _nA = new DraggableAngleNotifier(this, ptC, DRAGGING_PTA);
    _nB = new DraggableAngleNotifier(this, ptC, DRAGGING_PTB);
    TupleBvr pair = tracker(ptA, ptB);

    _draggablePtA = (Point2Bvr)pair.nth(0).runOnce();
    _draggablePtB = (Point2Bvr)pair.nth(1).runOnce();

    _draggableImgA = pimA.getImageBvr().transform(translate(sub(_draggablePtA, origin2)));
    _draggableImgB = pimB.getImageBvr().transform(translate(sub(_draggablePtB, origin2)));
  }

  PickableImage init(Point2Bvr pt, ColorBvr clr) {
    ImageBvr im = solidColorImage(clr).crop(point2(-0.001,-0.001),
                                            point2(0.001,0.001));

    return new PickableImage(im);
  }

  public void cleanup() {
    _evA = _evB = _resetEv = null;
    _nA.cleanup();
	_nB.cleanup();
	_nA = _nB = null;
    _draggablePtA = _draggablePtB = _ptA = _ptB = null;
    _draggableImgA = _draggableImgB = null;
  }


  DXMEvent _evA, _evB, _resetEv;
  DraggableAngleNotifier _nA, _nB;
  Point2Bvr _draggablePtA, _draggablePtB, _ptA, _ptB;
  ImageBvr _draggableImgA, _draggableImgB;
  final static int DRAGGING_PTA = 1, DRAGGING_PTB = 2;
}

class PythaSound extends Statics {
  PythaSound(DXMEvent evPlay, DXMEvent evStop) {

    _proofSnd = proofSnd();

    _snd = SoundBvr.newUninitBvr();
    _snd.init(until(silence, evPlay,
                    until(_proofSnd, evStop, _snd)));
  }

  SoundBvr proofSnd() {

    SoundBvr rotSnd = importSound(buildURL(Pytha._sndBase,"inflate.mp2"), null, silence, null, null, null).rate(toBvr(0.3));
    SoundBvr shearSnd = importSound(buildURL(Pytha._sndBase,"zoomin.mp2"), null, silence, null, null, null).rate(toBvr(0.3));

    SoundBvr snd = (SoundBvr)until(shearSnd, timer(toBvr(2)),
                               until(rotSnd, timer(toBvr(2)),
                                 until(shearSnd, timer(toBvr(2)),
                                   until(shearSnd, timer(toBvr(2)),
                                     until(rotSnd, timer(toBvr(2)), shearSnd)))));
    return snd;
  }
  public void cleanup() {
    _snd = null;
    _proofSnd = null;
  }

  SoundBvr _snd;
  static SoundBvr _proofSnd;    //!!!hackhack
}
