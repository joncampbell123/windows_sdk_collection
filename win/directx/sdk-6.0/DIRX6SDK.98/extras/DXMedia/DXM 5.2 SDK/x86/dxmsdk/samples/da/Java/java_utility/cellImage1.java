//
// cellImage1: A horizontal cell image cropper
// Copyright (c) 1997, Microsoft Corporation
//
// Used to import and crop images which contain a single row 
// of horizontal cells.  The showIndex method can be used to show a 
// partcular frame, or the loopStrip method can be used to cycle through
// all the images at a particular rate, thus providing an animation.
//
package java_utility;
import com.ms.dxmedia.*;
import java.net.*; 

// <Tutorial Section=3.1>
// So how does cellImage1 work?  The contructor takes an image and frame count
// and imports that image.
public class cellImage1 extends Statics  {
  public cellImage1(java.net.URL url, int frameCount)  {           
    super();
    _BaseImg = importImage(url);
    _FrameCount = toBvr(frameCount);
  }
        
  // To get a particular frame of the cellImage, the author can make a call to
        // the showIndex method.  This will return a properly cropped image.  The
        // following code calculates the minimum and maximum points of the indexed
        // image, crops the entire image, then centers the new image.
  public ImageBvr showIndex(NumberBvr index)  {
    NumberBvr actualIndex = sub(sub(_FrameCount, toBvr(1)), index);

    Bbox2Bvr frame = _BaseImg.boundingBox();

    NumberBvr cellHeight = 
      div(sub(frame.getMax().getY(),frame.getMin().getY()),
        _FrameCount);

    NumberBvr cellMin = add(frame.getMin().getY(), mul(cellHeight, actualIndex));
    NumberBvr cellMax = add(cellMin, cellHeight);
    ImageBvr cellImg =
    _BaseImg.crop(
      point2(frame.getMin().getX(), cellMin),
      point2(frame.getMax().getX(), cellMax));    
                
    ImageBvr cntrImg =
      cellImg.transform(
        translate(toBvr(0), sub(neg(cellMin),div(cellHeight,toBvr(2)))));

    return cntrImg;
  }

  // The loopStrip is a handy method that cycles through all the cells at the
        // indicated speed (in frames per second).
  public ImageBvr loopStrip(NumberBvr speed) {
    return showIndex(
      mod(floor(mul(localTime,speed)),_FrameCount));
  }

  public ImageBvr _BaseImg;
  public NumberBvr _FrameCount;
}
// </Tutorial>

