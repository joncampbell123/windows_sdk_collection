import com.ms.dxmedia.*;
import java.util.*;
import java.net.*;
import Piece;


public class ChessModel extends Model {

  // here are a bunch of constants we'll use:
  // the size of each square on the board, and the location of square 0,0
  protected NumberBvr squareSize = toBvr(1.0/8.0);
  protected Vector3Bvr pieceOrigin = vector3(toBvr(-3.5/8),toBvr(0),toBvr(3.5/8));
 
  //static URL mybase = null;
  // the geometry for the pieces (initialized in SetupBoard())
  protected GeometryBvr knight = null;
  protected GeometryBvr queen = null;
  protected GeometryBvr rook = null;
  protected GeometryBvr bishop = null;
  protected GeometryBvr king = null;
  protected GeometryBvr pawn = null;
  protected SoundBvr    moveSnd = null;
  protected SoundBvr    captureSnd = null;
  // end of constants

  // The geoArray keeps track of which geometry is at each board location
  // The switcherArray keeps track of the switchers needed to tell these
  // geometries to move around
  protected GeometryBvr[][] m_geoArray = new GeometryBvr[8][8];
  protected ModifiableBehavior[][] m_switcherArray = new ModifiableBehavior[8][8];

  // Same information is needed for captured geometries
  protected Vector m_capturedGeos = new Vector(); 
  protected Vector m_capturedSws = new Vector();
  protected int[] m_capturedCounts = new int[2];

  // Switcher requires to get an updating behavior of the canvas size.
  protected ModifiableBehavior m_sizeSw = null;

  public LabelImage m_resultLabel = null;

  public void createModel(BvrsToRun b2run) {

    //mybase = getImportBase();

    m_resultLabel = new LabelImage();
    // here we setup the size Behavior to give us the size of the canvas. Every time
    // SetSize() is called the switcher is used to change the behavior.

    m_sizeSw = new ModifiableBehavior(vector2(mul(toBvr(320),pixelBvr),
			mul(toBvr(240),pixelBvr)));
    Vector2Bvr size = (Vector2Bvr) m_sizeSw.getBvr();
    
     
    // This sets up the arrow buttons to rotate the board
    ButtonPanel bp = new ButtonPanel();
    ImageBvr buttons = bp.getImg();
    Vector2Bvr rots = bp.getRots();
    
    // calculate the vector to put the buttonpanel in the upper right corner
    Vector2Bvr buttonXform = add(size.div(toBvr(2)),
					      sub(origin2,buttons.boundingBox().getMax()));
    buttonXform = vector2(mul(buttonXform.getX(),toBvr(-1)),
			mul(buttonXform.getY(),toBvr(-1)));
		buttons = buttons.transform(translate(buttonXform)); //problem with making the buttons work.

    // set up the buttons for zooming in and out
    ZoomPanel zp = new ZoomPanel();
    ImageBvr zoomButtons = zp.getImg();

		NumberBvr zoomNum = (NumberBvr)zp.getZoom().runOnce();

    //Transform2Bvr imgScale = scale2(zp.getZoom());

		Transform3Bvr zoomScale = scale(zoomNum, zoomNum,toBvr(1));

    // calculate the translate to put it in the lower right corner
    Vector2Bvr zbtranslate = add(size.div(2),
				 sub(origin2,zoomButtons.boundingBox().getMax()));
    zbtranslate = vector2(zbtranslate.getX(), mul(zbtranslate.getY(),toBvr(-1)));
    zoomButtons = zoomButtons.transform(translate(zbtranslate));
    

    // setup the camera according to a pretty first position, and the rotation from the 
    // button panel and scale the image with the info from the zoomPanel.
    Transform3Bvr cameraXform = compose(
					rotate(yVector3, neg(rots.getX())),
					compose(rotate(xVector3, add(toBvr(-Math.PI/6),rots.getY())),
					  compose(zoomScale,translate(toBvr(0),toBvr(0),toBvr(2.5)))));
		
    CameraBvr camera= perspectiveCamera(toBvr(1),toBvr(0)).transform(compose(
									     cameraXform,scale(toBvr(6),toBvr(6),toBvr(1))));

    MicrophoneBvr mic = defaultMicrophone.transform(cameraXform);
		
    // setup the initial board geometry
    GeometryBvr geo = SetupBoard();

		//geo = geo.transform(zoomScale);

    GeometryBvr headlight = directionalLight.lightColor(colorRgb(toBvr(.6),toBvr(.6),toBvr(.6))).
      transform(cameraXform);
    geo = union(geo,headlight);

    ImageBvr img = geo.render(camera);
    SoundBvr snd = geo.render(mic);

    // scale the image with the info from the zoomPanel
    //img = img.transform(imgScale);

    // overlay the rendered image with the buttons
    img = overlay(zoomButtons,overlay(buttons,img));

    // make a pretty background color
    ImageBvr model = overlay(m_resultLabel.getBvr(), 
			     overlay(img, solidColorImage(colorRgb(toBvr(.2),toBvr(.27),toBvr(.3)))));

    setImage(model);
    setSound(snd);
  }


  // this updates size via the size Switcher when the model changes size.
  public void SetSize(int width, int height){
    if (m_sizeSw != null) {
      Vector2Bvr size = vector2(mul(toBvr(width),pixelBvr),
				mul(toBvr(height),pixelBvr));
      m_sizeSw.switchTo(size);
    }
  }


  protected GeometryBvr SetupBoard() {

    // import all the geometry we're going to use
    NumberBvr scale = toBvr(1.0/18.0);

    try {

      knight = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/knight.x")).transform(scale3(scale));
      knight = knight.transform(translate(0.28,0.0,0.38));
      queen = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/queen.x")).transform(scale3(scale));
      queen = queen.transform(translate(-0.05,0,-0.40));
      rook = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/rook.x")).transform(scale3(scale));
      rook = rook.transform(translate(0.38,0,0.38));      
      bishop = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/bishop.x")).transform(scale3(scale));
      bishop = bishop.transform(translate(0.17,0,-0.40));      
      king = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/king.x")).transform(scale3(scale));
      king = king.transform(translate(0.05,0,-0.40));
      pawn = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/pawn.x")).transform(scale3(scale));
      pawn = pawn.transform(translate(-0.05,0,-0.28));

      NumberBvr foo[] = new NumberBvr[2];

      moveSnd = importSound(new URL("file:///c:/program files/da_pgn_viewer/media/zoomin2.wav"),foo);
      captureSnd = importSound(new URL("file:///c:/program files/da_pgn_viewer/media/bangblow.wav"),foo);

			    } catch (MalformedURLException e) {
      System.err.println("malformed URL!!!!!!!!!!!");
    }

    // set the middle part of the board to empty.
    for (int i = 2; i < 6; i++) {
      for (int j = 0; j < 8; j++) {
        m_switcherArray[i][j] = null;
        m_geoArray[i][j] = null;
      }
    }

    // put pieces on the rest of the board.
    setPiece(rook, 0, 0, Piece.WHITE);
    setPiece(knight, 1, 0, Piece.WHITE);
    setPiece(bishop, 2, 0, Piece.WHITE);
    setPiece(queen, 3, 0, Piece.WHITE);
    setPiece(king, 4, 0, Piece.WHITE);
    setPiece(bishop, 5, 0, Piece.WHITE);
    setPiece(knight, 6, 0, Piece.WHITE);
    setPiece(rook, 7, 0, Piece.WHITE);

    setPiece(pawn, 0, 1, Piece.WHITE);
    setPiece(pawn, 1, 1, Piece.WHITE);
    setPiece(pawn, 2, 1, Piece.WHITE);
    setPiece(pawn, 3, 1, Piece.WHITE);
    setPiece(pawn, 4, 1, Piece.WHITE);
    setPiece(pawn, 5, 1, Piece.WHITE);
    setPiece(pawn, 6, 1, Piece.WHITE);
    setPiece(pawn, 7, 1, Piece.WHITE);

    setPiece(pawn, 0, 6, Piece.BLACK);
    setPiece(pawn, 1, 6, Piece.BLACK);
    setPiece(pawn, 2, 6, Piece.BLACK);
    setPiece(pawn, 3, 6, Piece.BLACK);
    setPiece(pawn, 4, 6, Piece.BLACK);
    setPiece(pawn, 5, 6, Piece.BLACK);
    setPiece(pawn, 6, 6, Piece.BLACK);
    setPiece(pawn, 7, 6, Piece.BLACK);

    setPiece(rook, 0, 7, Piece.BLACK);
    setPiece(knight, 1, 7, Piece.BLACK);
    setPiece(bishop, 2, 7, Piece.BLACK);
    setPiece(queen, 3, 7, Piece.BLACK);
    setPiece(king, 4, 7, Piece.BLACK);
    setPiece(bishop, 5, 7, Piece.BLACK);
    setPiece(knight, 6, 7, Piece.BLACK);
    setPiece(rook, 7, 7, Piece.BLACK);


    // setup the board geometry
    GeometryBvr boardGeo = null;
    GeometryBvr whiteSquares = null;
    GeometryBvr blackSquares = null;
    GeometryBvr finalBoard = null;
    try {
      whiteSquares = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/white_sqrs.x"));
      blackSquares = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/blk_sqrs.x"));
      boardGeo = importGeometry(new URL("file:///c:/program files/da_pgn_viewer/media/board.x"));//.transform(
			  //scale3(toBvr(1.0/15.0)));
      finalBoard = union(blackSquares,union(whiteSquares,boardGeo));
      finalBoard = finalBoard.transform(scale3(toBvr(1.0/16.0)));
    
    
	//										compose(rotate(xVector3, toBvr(-Math.PI/2)), scale3(toBvr(1.0/80.0))));
    } catch (MalformedURLException e) {
      System.err.println("another bad URL!!!");
    }

    GeometryBvr lights = union(union(directionalLight.
				     lightColor(colorRgb(toBvr(.5),toBvr(.5),toBvr(.5))).
				     transform(compose(rotate(xVector3,toBvr(-Math.PI/4)),
						       rotate(yVector3,toBvr(0))) 
					       ),
      
				     directionalLight.lightColor(colorRgb(toBvr(.5),toBvr(.5),toBvr(.5))).
				     transform(compose(
						       rotate(xVector3,toBvr(-.75*Math.PI)),
						       rotate(yVector3,toBvr(0))
						       ))),
      
			       ambientLight.lightColor(colorRgb(toBvr(.1),toBvr(.1),toBvr(.1))));

    GeometryBvr allGeo = union(finalBoard, lights);

    // union the board geometry with the geometry of all the pieces. We get the geometry for the
    // pieces from the switchers so they can be changed by the switcher.
    for (int i = 0; i < 8; i++){
      for (int j = 0; j < 8; j++){
	if (m_switcherArray[i][j] != null) {
	  allGeo = union(allGeo, (GeometryBvr)m_switcherArray[i][j].getBvr());
	}
      }
    }
    return allGeo;

  }

  protected void setPiece(GeometryBvr geo, int x, int y, int color) {
    
    GeometryBvr cgeo = null;
    
    // color and rotate the piece appropriate to the player
    if (color == Piece.WHITE) {
      cgeo = geo.diffuseColor( colorRgb(toBvr(0.7),toBvr(0.7),toBvr(0.7)));/*.
									       specularColor(colorRgb(toBvr(.9),toBvr(.9),toBvr(.9))).
									       specularExponent(toBvr(10)); */      
      cgeo = cgeo.transform(rotate(yVector3,toBvr(Math.PI)));
    } else {
      cgeo = geo.diffuseColor( colorRgb(toBvr(0.6),toBvr(0),toBvr(0.3)));/*.
									   specularColor(colorRgb(toBvr(.5),toBvr(.5),toBvr(.5))).
									   specularExponent(toBvr(10));       */
    }

    // put the piece on the board
    m_switcherArray[x][y] = new ModifiableBehavior(cgeo.transform(LocationToTransform(x,y)));
    m_geoArray[x][y] = cgeo;

  }

  // returns a vector that will translate a piece to a given spot on the board
  protected Vector3Bvr LocationToTranslate(int x, int y) {
    return add(pieceOrigin, vector3(toBvr(x),toBvr(0),toBvr(-y)).mul(squareSize));
  }
  // same thing but returns a transform3Bvr
  protected Transform3Bvr LocationToTransform(int x, int y) {
    return translate(LocationToTranslate(x,y));
  }

  // handy utility function, returns a nice number
  public static NumberBvr Smooth0to1(int duration) {
    NumberBvr rising = sub (toBvr(.5), 
			    div(cos(mul(div(localTime,toBvr(duration)), toBvr(Math.PI))),toBvr(2)));
    NumberBvr done = toBvr(1);

    DXMEvent ev = timer(toBvr(duration));

    return (NumberBvr)(until(rising, ev, done));
  }

  // returns a transform that will slide a piece from one location to another
  protected Transform3Bvr SlidingTransform(int xfrom, int yfrom, int xto, int yto,int duration) {
    Vector3Bvr fromVec = LocationToTranslate(xfrom,yfrom);
    Vector3Bvr toVec = LocationToTranslate(xto,yto);
    Vector3Bvr moveVec = sub(toVec, fromVec);
    return  translate(add(moveVec.mul(Smooth0to1(duration)),LocationToTranslate(xfrom,yfrom)));
  }

  //  animates a piece moving from one location to another
  public void MovePiece(int xfrom, int yfrom, int xto, int yto) {
		
    GeometryBvr newGeo = union(m_geoArray[xfrom][yfrom],soundSource(moveSnd));
    newGeo = newGeo.transform(SlidingTransform(xfrom,yfrom,xto,yto,3));

    m_switcherArray[xto][yto] = m_switcherArray[xfrom][yfrom];
    m_switcherArray[xfrom][yfrom] = null;
    m_geoArray[xto][yto] = m_geoArray[xfrom][yfrom];
    m_geoArray[xfrom][yfrom] = null;
    m_switcherArray[xto][yto].switchTo(newGeo);
  }

  public void CapturePiece(int x, int y, int player) {

    //keep track of the fact that its been captured
    m_capturedSws.addElement(m_switcherArray[x][y]);
    m_capturedGeos.addElement(m_geoArray[x][y]);

    // calculate where on the board to put the captured piece. We want two columns along side 
    // the board
    int xTo = m_capturedCounts[player]/10;
    int yTo = m_capturedCounts[player]%10;

    if (player == Piece.WHITE) {
      xTo += 9;
      yTo -= 1;
    } else {
      xTo = -2 -xTo;
      yTo = 9 -yTo;
    }

    // calculate the transform to make it fly and twist in agony
    int duration = 4;
    Transform3Bvr slide = SlidingTransform(x,y,xTo,yTo,duration);

    // here is the height of the toss
    NumberBvr height = sub(mul(sin(mul(localTime,toBvr(Math.PI/duration))),toBvr(.35)),
			   mul(div(localTime,toBvr(duration)),squareSize));

    Transform3Bvr rot = rotate(vector3(toBvr(1),toBvr(0.2),toBvr(.5)), mul(localTime,toBvr(Math.PI*2/duration)));

    // this toss just puts a piece straight up int the air and then back down.
    Transform3Bvr toss = (Transform3Bvr)until(compose(translate(toBvr(0),height,toBvr(0)),rot),
					      timer(toBvr(duration)),translate(toBvr(0),mul(toBvr(-1),squareSize),toBvr(0)));
 
    // add the slide to move the piece off the board
    Transform3Bvr capturedXform =compose(slide,toss);

    // tell the piece to execute the move with sound
    m_switcherArray[x][y].switchTo(union(m_geoArray[x][y],soundSource(captureSnd)).transform(capturedXform));

    // take the piece off the board
    m_geoArray[x][y] = null;
    m_switcherArray[x][y] = null;
    m_capturedCounts[player]++;
    
  }


  public void RevivePiece(int x, int y, int player) {

    // move the last captured piece of color player back onto the board
    m_switcherArray[x][y] = (ModifiableBehavior)m_capturedSws.lastElement();
    m_capturedSws.removeElementAt(m_capturedSws.size() -1);
    m_geoArray[x][y] = (GeometryBvr)m_capturedGeos.lastElement();
    m_capturedGeos.removeElementAt(m_capturedGeos.size() -1);

    m_switcherArray[x][y].switchTo(m_geoArray[x][y].transform(LocationToTransform(x,y)));
    m_capturedCounts[player]--;
  }

  public void PromotePiece(int x, int y, int type) {
    // change the geometry of a specific piece
    switch (type) {
    case Piece.QUEEN:
      m_geoArray[x][y] = queen;
      break;
    case Piece.PAWN:
      m_geoArray[x][y] = pawn;
      break;
    case Piece.ROOK:
      m_geoArray[x][y] = rook;
      break;
    case Piece.KNIGHT:
      m_geoArray[x][y] = knight;
      break;
    case Piece.BISHOP:
      m_geoArray[x][y] = bishop;
      break;
    case Piece.KING:
      m_geoArray[x][y] = king;
      break;
    }
    m_switcherArray[x][y].switchTo(m_geoArray[x][y].transform(LocationToTransform(x,y)));      
  }
} // end ChessModel



// class: ButtonPanel
// this class generates the arrow buttons for the upper right hand corner of the screen. 
// we will get from this an image to put onscreen, and a vector2 that is the amount to 
// rotate the board on the X and Y axis.

class ButtonPanel extends Statics implements UntilNotifier {

  DXMEvent m_arrowPick = null;
  Vector2Bvr m_rots = null;
  PickableImage m_pimg =  null;

  public ButtonPanel() {

    // setup the pickable image
    try {
      m_pimg = new PickableImage(importImage(new URL("file:///c:/program files/da_pgn_viewer/media/arrows.gif")));
    }catch (MalformedURLException e) {
      System.err.println("AWWWWWWWWWW!");
    }
    m_arrowPick = m_pimg.getPickEvent();

    // speeds is zero until we get clicked on...
    Vector2Bvr speeds = (Vector2Bvr)untilNotify(zeroVector2, 
						andEvent(leftButtonDown,m_arrowPick), this);

		Vector2Bvr actualZoomSpeed = Vector2Bvr.newUninitBvr();
		  actualZoomSpeed.init(until(
				speeds,orEvent(notEvent(m_arrowPick),leftButtonUp),
			    actualZoomSpeed));


    // total amount to rotate is the integral of how long we've been holding
    // the button down.
    m_rots = integral(actualZoomSpeed);
    m_rots.runOnce();
  }

  public ImageBvr getImg() {
    return m_pimg.getImageBvr();
  }
  public Vector2Bvr getRots() {
    return m_rots;
  }

  // utility function
  public static NumberBvr sign(NumberBvr b) {
    return (NumberBvr)cond(gte(b, toBvr(0)), toBvr(1), toBvr(-1));
  }
  
  public static NumberBvr min(NumberBvr a, NumberBvr b) {
    return (NumberBvr)cond(lte(a,b),a,b);
  }
  public static NumberBvr max(NumberBvr a, NumberBvr b) {
    return (NumberBvr)cond(gte(a,b),a,b);
  }

  // called when we're picked on
  public Behavior notify(Object eventData, Behavior prev, 
			 BvrsToRun b2r) {
    // cast the event data for easier access
    PairObject andEv = (PairObject)eventData;
    
    PairObject pickData = (PairObject)andEv.getSecond();

    Point2Bvr pickPt= (Point2Bvr)pickData.getFirst();
    Vector2Bvr pickVec = (Vector2Bvr)pickData.getSecond();

    // the Pt picked on the image 
    pickPt = add(pickPt,pickVec);

    // turn on/off latspeed/longspeed depending on which quadrant of the image we're in
    NumberBvr latSpeed = (NumberBvr)cond (gt(abs(pickPt.getX()), abs(pickPt.getY())), 
					  sign(pickPt.getX()),toBvr(0));

    NumberBvr longSpeed = (NumberBvr)cond (gt(abs(pickPt.getX()), abs(pickPt.getY())), 
					   toBvr(0), sign(pickPt.getY()));

    longSpeed.runOnce();

    NumberBvr cappedLongSpeed =(NumberBvr) cond(gte(m_rots.getY(), toBvr(Math.PI/2.0)),min(longSpeed,toBvr(0)),longSpeed);

    Vector2Bvr speeds = vector2(latSpeed,cappedLongSpeed).div(toBvr(3));

		return speeds;

    // return the speeds we just calculated, until we either let go of the mouse button 
    // or move the mouse of the picture, then become a zeroVector that will run through this 
    // notifier when clicked on.
    //return until(speeds, orEvent(notEvent(m_arrowPick),leftButtonUp), 
		// untilNotify(zeroVector2, andEvent(leftButtonDown,m_arrowPick), this));
  }
}


   
// class: ZoomPanel
// Very similar to  the ButtonPanel, except here we only have to detect if we are on
// the top or bottom half of the image.

class ZoomPanel extends Statics implements UntilNotifier {
  DXMEvent m_zoomPick = null;

  NumberBvr m_zoom = null;
  PickableImage m_pimg =  null;

  public ZoomPanel() {
    // set up the pickable image
    try {
		  ImageBvr img = importImage(new URL("file:///c:/program files/da_pgn_viewer/media/zoom.bmp"));

      m_pimg = new PickableImage(img);
    }catch (MalformedURLException e) {
      System.err.println("AWWW!");
    }
    m_zoomPick = m_pimg.getPickEvent();

    // set zoomspeed to be zero until we get clicked on
    NumberBvr zoomSpeed = (NumberBvr)untilNotify(toBvr(0), 
						 andEvent(leftButtonDown,m_zoomPick), this);

		NumberBvr actualZoomSpeed = NumberBvr.newUninitBvr();
		  actualZoomSpeed.init(until(
				zoomSpeed,orEvent(notEvent(m_zoomPick),leftButtonUp),
			    actualZoomSpeed));

    // integrate the zoom speed and scale it to look nice.
    m_zoom = add(div(integral(actualZoomSpeed),toBvr(4)),toBvr(1));

  }

  public ImageBvr getImg() {
    return m_pimg.getImageBvr();
  }
  public NumberBvr getZoom() {
    return m_zoom;
  }

	public static NumberBvr sign(NumberBvr b) {
    return (NumberBvr)cond(gte(b, toBvr(0)), toBvr(-1), toBvr(1));
  }

  public Behavior notify(Object eventData, Behavior prev, 
			 BvrsToRun b2r) {
    // get at the event data
    PairObject andEv = (PairObject)eventData;
    PairObject pickData = (PairObject)andEv.getSecond();

		Point2Bvr pickPt= (Point2Bvr)pickData.getFirst();
    Vector2Bvr pickVec = (Vector2Bvr)pickData.getSecond();

    // pickPt is the pt that the mouse is currently on in image coordinates
    pickPt = add(pickPt,pickVec);

    // are we on the top or bottom of the image
    NumberBvr zoomSpeed = sign(pickPt.getY());
 		
		return zoomSpeed;
  }

}


class LabelImage extends Statics {
  ModifiableBehavior sw = null;

  LabelImage() {
    sw = new ModifiableBehavior(emptyImage);
  }
  
  ImageBvr getBvr() {
    return (ImageBvr)sw.getBvr();
  }


  void switchText(String text) {
		FontStyleBvr fs = defaultFont.bold().color(white);
    if (text == null) {
      sw.switchTo(emptyImage);
    } else {
      ImageBvr textIm = textImage(text,fs).transform(scale2(2));
      Bbox2Bvr bbox = textIm.boundingBox();

      ImageBvr background = solidColorImage(black)
				.crop(bbox.getMin(), bbox.getMax()).opacity(.5).
        transform(scale(2,1));
      sw.switchTo(overlay(textIm,background));
    }

  }
}
