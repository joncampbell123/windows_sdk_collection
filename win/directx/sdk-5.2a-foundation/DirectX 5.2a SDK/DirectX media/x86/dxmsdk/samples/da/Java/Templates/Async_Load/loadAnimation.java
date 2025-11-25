// <Tutorial Section=2.0 Title="Section 2: Stand-In Image">
//
// Define an opening animation used for latency masking during
// media files download.
//
// </Tutorial>
//

import com.ms.dxmedia.*;

// <Tutorial Section=2.1>
class loadAnimation extends Statics {
  public ImageBvr getImage(NumberBvr progress) {

    // There are two primary images, introImg and radarImg, in this animation.
    // The introImg composes of a few text images and a progress indicator.
    String[] intro = {"Direct", "X", "Media", "for Animation", "loading...please wait"},
             info = {"Integrated", "2D", "3D", "and", "Audio", "for", "Java"};
    double[] size = {3, 7, 3, 2, 1},
             startPos = {-500,50,  0,500,  500,50,  6,-10,  0,-50},
             endPos = {-98,50,  0,50,  100,50,  6,-10,  0,-50},
             trigger = {.5, 1.5, 2.5, 4, 5};
    ImageBvr[] introImgs = new ImageBvr[intro.length];

    // Create an array of text images for the introImg.
    for(int i=0; i<intro.length; i++) {
      Point2Bvr startPt = point2(mul(toBvr(startPos[2*i]),pixelBvr), mul(toBvr(startPos[2*i+1]),pixelBvr)),
                endPt = point2(mul(toBvr(endPos[2*i]),pixelBvr), mul(toBvr(endPos[2*i+1]),pixelBvr));
      
      FontStyleBvr fs = defaultFont.family(toBvr("Arial")).color(blue).bold();

      ImageBvr textIm = stringImage(toBvr(intro[i]), fs)
        .transform(scale2(size[i]));

      introImgs[i] = move(textIm, startPt, endPt, 1, trigger[i]);
    }

    introImgs[3] = introImgs[3].opacity(ramp(trigger[3],.3));
    introImgs[4] = introImgs[4].opacity(mul(ramp(trigger[4],1), sin(mul(toBvr(4),localTime))));

    // Compose the text images together and overlay them on a black background.
    ImageBvr introImg = solidColorImage(black);
    for(int i=0; i<intro.length; i++)
      introImg = overlay(introImgs[i],introImg);

    // Add a progress status indicator to the introImg.
    Vector2Bvr statPos = vector2(toBvr(0),mul(toBvr(-80),pixelBvr));

    FontStyleBvr fs = defaultFont.family(toBvr("Arial")).color(green);
    ImageBvr statusImg = stringImage(concat(mul(progress,toBvr(100))
                        .toString(toBvr(2)), toBvr("% complete") ), fs)
        .opacity(ramp(trigger[4],1));

    statusImg = statusImg.transform(compose(translate(statPos),scale2(.8)));
    introImg = overlay(statusImg, introImg);

    // The radarImg has two parts.  The infoImg is composed of a few text
    // images clipped to a path of a rotating pie.  The lineImg draws the
    // the rotating edge of the pie.
    ImageBvr infoImg = emptyImage;
    for(int i=0; i<info.length; i++) {
      infoImg = overlay(radiate(info[i], (2*Math.PI*i/info.length)+.3), infoImg);
    }
    infoImg = overlay(infoImg, solidColorImage(black));
    Path2Bvr origin = ray(point2(1,0));
    Path2Bvr pie = concat(origin,
                          arc(toBvr(0), mod(localTime, toBvr(2*Math.PI)),
                              toBvr(1), toBvr(1)));
    infoImg = infoImg.clip(fillMatte(pie));

    // Create a line of length 0.05 meter then rotate it at the same rate
    // the pie is rotating.
    Path2Bvr ln = ray(point2(.05,0));
    ImageBvr lineImg = ln.draw(defaultLineStyle.width(toBvr(.08*cm)).color(white)),
             radarImg = overlay(lineImg.transform(rotate(localTime)), infoImg);

    // Delay the animation to create a more interesting effect.
    radarImg = (ImageBvr)until(emptyImage, timer(toBvr(6)), radarImg);
    introImg = overlay(radarImg, introImg);
    return (ImageBvr)until(solidColorImage(black), timer(toBvr(1)), introImg);
  }

  // This function creates an image from the given text string.  It then
  // moves the image to the position defined by the given radians.
  ImageBvr radiate(String text, double radians) {
    // Rotate the vector (0.04, 0) by the given radians, that's where we'll
    // translate the image to.
    Vector2Bvr vec = vector2(.04,0).transform(rotate(toBvr(radians)));
    FontStyleBvr fs = defaultFont.family(toBvr("Arial")).color(red).bold();
    ImageBvr img = stringImage(toBvr(text), fs)
      .transform(scale2(1.3));

    return img.transform(translate(vec));
  }

  // Move the given image from the start position to the end position
  // after second at the given rate.
  ImageBvr move(ImageBvr img, Point2Bvr start, Point2Bvr end, double rate, double trigger) {
    Vector2Bvr startPos = sub(start, origin2),
    path     = add(startPos, sub(end,start).mul(ramp(0,rate)) );
    path = (Vector2Bvr) until(startPos, timer(toBvr(trigger)), path);
    return img.transform(translate(path));
  }

  // Before trigger seconds, return 0.
  // After trigger seconds, return localTime*rate.
  // When localTime*rate > 1, return 1.
  NumberBvr ramp(double trigger, double rate) {
    NumberBvr rampVal = mul(localTime,toBvr(rate));
    return (NumberBvr) cond(lte(localTime,toBvr(trigger)), toBvr(0),
            cond(lt(rampVal,toBvr(1)), rampVal, toBvr(1)));
  }
} 

// </Tutorial>
