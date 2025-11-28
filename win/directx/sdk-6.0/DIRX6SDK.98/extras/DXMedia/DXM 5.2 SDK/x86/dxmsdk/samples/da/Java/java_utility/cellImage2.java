//
// cellImage2: A vertical cell image cropper
// Copyright (c) 1997, Microsoft Corporation
//
// Used to import and crop images which contain a single row 
// of vertical cells.  The showIndex method can be used to show a 
// partcular frame, or the loopStrip method can be used to cycle through
// all the images at a particular rate, thus providing an animation.
//
package utility;
import com.ms.dxmedia.*;
import java.net.*;

public class cellImage2 extends Statics {

  public cellImage2(java.net.URL url, int frameCount) 
	{           
	  super();
    _BaseImg = importImage(url);
    _FrameCount = toBvr(frameCount);
  }
	
  // showIndex crops a single indexed image out of the horizontal strip 
  public ImageBvr showIndex(NumberBvr index) {
    Bbox2Bvr frame = _BaseImg.boundingBox();
    NumberBvr CellWidth = 
            div(sub(frame.getMax().getX(),frame.getMin().getX()),
                    _FrameCount);
    NumberBvr cellMinX = add(frame.getMin().getX(), mul(CellWidth, index));
    NumberBvr cellMaxX = add(cellMinX, CellWidth);
    ImageBvr cellImg =
            _BaseImg.crop(
                    point2(cellMinX, frame.getMin().getY()),
                    point2(cellMaxX, frame.getMax().getY()));              
    ImageBvr cntrImg =
            cellImg.transform(
                    translate(sub(neg(cellMinX),div(CellWidth,toBvr(2))), toBvr(0)));
    return cntrImg;
  }

  // loopStrip cycles through a the cells at the indicated speed
  public ImageBvr loopStrip(NumberBvr speed) {
    return showIndex(mod(floor(mul(localTime,speed)),_FrameCount));
  }

  public ImageBvr _BaseImg;
  public NumberBvr _FrameCount;
}


