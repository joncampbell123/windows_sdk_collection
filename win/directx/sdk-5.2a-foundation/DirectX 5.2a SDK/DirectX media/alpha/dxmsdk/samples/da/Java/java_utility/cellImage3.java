//
// cellImage3: An arbitrary frame indexed cell image cropper
// Copyright (c) 1997, Microsoft Corporation
//
// Used to import and crop images which contain multiple rows of
// different sized cells. The constructor takes the image and the 
// array of upper left hand offsets, as well as the array of extents. 
// These are used to determine where to crop a particular cell frame.
// The showIndex method can be used to show a partcular frame, or the 
// loopStrip method can be used to cycle through all the images at a 
// particular rate, thus providing an animation.
//
package utility;
import com.ms.dxmedia.*;
import java.net.*;

public class cellImage3 extends Statics {

	public cellImage3(URL pathname, Point2Bvr[] Offsets, Point2Bvr[] Extents) {           
		super();
		_Offsets = new ArrayBvr(Offsets);
		_Extents = new ArrayBvr(Extents);
        _BaseImg = importImage(pathname);
  }
	
  // showIndex crops a single indexed image out 
  public ImageBvr showIndex(NumberBvr index) {				
		Point2Bvr mainUR = _BaseImg.boundingBox().getMax();		
		Point2Bvr Offset = (Point2Bvr)_Offsets.nth(index);
		Point2Bvr Extent = (Point2Bvr)_Extents.nth(index);
		
		Vector2Bvr centerDelta = 
			vector2(
				sub(_BaseImg.boundingBox().getMax().getX(),
					mul(add(Offset.getX(), div(Extent.getX(),toBvr(2))),pixelBvr) ),
				mul(toBvr(-1),
					sub( _BaseImg.boundingBox().getMax().getY(),
						 mul(add(Offset.getY(), div(Extent.getY(),toBvr(2))),pixelBvr) ))					
			);

		ImageBvr centerImg = _BaseImg.transform(translate(centerDelta));

		Point2Bvr UR = point2( mul(mul(toBvr(.5),Extent.getX()), pixelBvr ),
								 mul(mul(toBvr(.5),Extent.getY()), pixelBvr ) );
		Point2Bvr LL = point2( mul(mul(toBvr(-.5),Extent.getX()),pixelBvr ), 
								 mul(mul(toBvr(-.5),Extent.getY()),pixelBvr ) );
		
        ImageBvr frameImg =
			centerImg.crop(LL, UR);              

        return frameImg;
	}

  // loopStrip cycles through a spcific range of cells at the indicated speed (in frames/sec)
  public ImageBvr loopStrip(NumberBvr minIndex, NumberBvr maxIndex, NumberBvr speed) {
		NumberBvr relativeIndex = mod( floor(mul(localTime,speed)),
									   add(sub(maxIndex,minIndex),toBvr(1)) );
		return showIndex(add(minIndex,relativeIndex));
  }

  protected ImageBvr _BaseImg;
  protected ArrayBvr _Extents;
	protected ArrayBvr _Offsets;
}


