// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;

class TileModel extends Model {

  // For constructing a time-varying range.  This function
  // generates a time varying number bvr provided min and max
  // values, and a rate (number of cycles per second).
  public static NumberBvr myRange(double min, double max, double rate) {
      
      // a = (1 + sin(time * rate)) * 0.5
      NumberBvr a = mul(add(toBvr(1), sin(mul(toBvr(rate), localTime))),
                        toBvr(0.5));

      // b = (max - min) * a + min
      NumberBvr b = add(mul(toBvr(max-min), a),
                        toBvr(min));

      return b;
  }

  public void createModel(BvrsToRun blst) {

    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase,"image/");
      ////// Top Image
      
      // Bring in a bitmap image
      ImageBvr head =
          importImage(buildURL(imgBase,"shingle.jpg"));

      // Construct a new image by overlaying atop an empty image that
      // has an infinite bounding box.  This allows subsequent crops
      // to retain the "see-through-ness" that we're trying to
      // achieve. 
      head = overlay(head, detectableEmptyImage);

      // Construct four time varying values representing a rectangle
      // whose position is changing, and whose size is changing.  x,
      // y, width, and height.
      NumberBvr xPos = myRange(-0.03, 0.01, 0.8);
      NumberBvr yPos = myRange(-0.03, 0.01, 0.4);
      NumberBvr width = myRange(0.01, 0.07, 1.5);
      NumberBvr height = myRange(0.01, 0.07, 1.5);

      // combine these numbers into points at the corners of a
      // rectangle. 
      Point2Bvr hMin = point2(xPos, yPos);
      Point2Bvr hMax = point2(add(xPos, width), add(yPos, height));

      // Apply this rectangle as the cropping rectangle
      ImageBvr  croppedHead = head.crop(hMin, hMax);

      // Infinitely tile the result.  
      ImageBvr  headTiles = croppedHead.tile();

      // Build up a time-varying transform...
      Transform2Bvr xf1 = translate(mul(toBvr(0.03), sin(localTime)),
                                    mul(toBvr(0.03), cos(mul(localTime,
                                                            toBvr(0.3)))));

      // ... and apply it to the head tiles to move the whole tiling
      // around.   This completes our top image.
      ImageBvr topIm = headTiles.transform(xf1);


      ////// Bottom Image

      // build up a time-varying transform to apply to the bottom image.
      Transform2Bvr xf2 = translate(mul(toBvr(0.01), sin(localTime)),
                                    mul(toBvr(0.01), cos(localTime)));

      // Construct two time varying colors that cycle through the
      // hues. 
      ColorBvr col1 = colorHsl(mul(localTime, toBvr(0.5)),
                               toBvr(0.5),
                               toBvr(0.5));
      
      ColorBvr col2 = colorHsl(mul(localTime, toBvr(0.43)),
                               toBvr(0.5),
                               toBvr(0.5));

      // Use these to construct a unit-sized gradient image with black
      // and white at two corners, and these time-varying colors at
      // the other two corners.
      ImageBvr gradImg = gradientSquare(col1, black, col2, white);

      // Now scale this way down.
      gradImg = gradImg.transform(scale2(0.065));

      // Create the bottom image by transforming the gradient image
      // around. 
      ImageBvr botIm = gradImg.transform(xf2);

      ////// Entire model

      // Finally, overlay the top over the bottom over a solid white
      // image. 
      ImageBvr model = overlay(topIm,
                               overlay(botIm,
                                       solidColorImage(white)));

      // And set the model's image to this image.
      setImage(model);
  }
}

public class Tile extends DXMApplet {
    public Tile() { setModel(new TileModel()) ; }
}
