// This applet creates and plays a sequence of animations about
// the Encarta 2000 encyclopedia.  It uses the following features in the
// DirectX Media.
// 1) 2D/3D animation used as a texture onto 3D objects
// 2) 2D and 3D interplay
// 3) interaction through texture
// 4) audeo mixing and spacialization

import com.ms.dxmedia.*;
import java.net.*; //added to support URL's

class EncartaModel extends Model {
  //BACKGROUND SWITCH
  static final boolean useGradientBkgnd = true;

  public void createModel(BvrsToRun blst) {
    mediaBase = getImportBase();
    imgBase = buildURL(mediaBase,"image/");
    midiBase = buildURL(mediaBase,"midi/");
    sndBase = buildURL(mediaBase,"sound/");
    geoBase = buildURL(mediaBase,"geometry/");
    movieBase = buildURL(mediaBase,"movie/");

    // Import sounds.
    SoundBvr Wsh = importSound(buildURL(midiBase,"Wsh2.mid"), null);
    _illusSnd = new ModifiableBehavior(silence);
    Behavior sndBvr[] = {importSound(buildURL(midiBase,"1-open2.mid"), null),
                         Wsh, Wsh,
                         importSound(buildURL(midiBase,"boxSpinS2.mid"), null),
                         Wsh, Wsh, Wsh, Wsh,
                         importSound(buildURL(midiBase,"hit.mid"), null),
                         importSound(buildURL(midiBase,"finding2.mid"), null),
                         _illusSnd.getBvr()
                        };

    // Initialize data members.
    init(blst);

    // The restart event restart the entire animation sequence.
    DXMEvent restartEv = keyDown('r');

    // Put all the animated image hehaviors we want to play and the events
    // to switch to the next behavior in two arrays so we can cycle through
    // them with MyCycler class.
    Behavior imgBvr[] = {imgEncarta(), imgEncyclopedia(),
                         imgScaleUpEncarta(), imgAdd2000(),
                         boxRollIn(), boxAndText(),
                         ultimateTool(),
                         illustrations()};

    DXMEvent imgEv[] = {timer(bvr1), timer(toBvr(1.2)),
                        timer(toBvr(2.5)), timer(toBvr(4)),
                        timer(toBvr(3)), timer(toBvr(5.5)),
                        timer(toBvr(5)),
                        restartEv};
/*
    DXMEvent imgEv[] = {timer(bvr0), timer(bvr0),
                        timer(bvr0), timer(bvr0),
                        timer(bvr0), timer(bvr0),
                        timer(bvr0),
                        restartEv};
*/
    MyCycler imgCyl = new MyCycler(imgBvr, imgEv);

    NumberBvr num = toBvr(0.4);
    ImageBvr bkgnd;
    if (useGradientBkgnd) {
      bkgnd = gradientSquare(colorRgb(sub(bkgndR, num), bkgndG, sub(bkgndB, num)),
                             colorRgb(sub(bkgndR, num), bkgndG, bkgndB),
                             colorRgb(sub(bkgndR, num), sub(bkgndG, num), bkgndB),
                             colorRgb(bkgndR, bkgndG, sub(bkgndB, num))).
                             transform(scale2(viewSize));
    } else {
      bkgnd = solidColorImage(colorRgb(bkgndR, bkgndG, bkgndB));
    }

    // Overlay the animation on a gradient filled image, and set the result as
    // the model's image behavior.
    setImage(overlay((ImageBvr)imgCyl.getBvr(), bkgnd));

    DXMEvent sndEv[] = {timer(toBvr(4.7)),                  // intro
                        timer(bvr2), timer(bvr2),   // 2000 fade in/out
                        timer(toBvr(3)),                    //boxspin
                        timer(toBvr(0.7)), timer(toBvr(0.7)),   // wooshes
                        timer(toBvr(0.7)),
                        timer(toBvr(3.4)),                  // ultimate tool
                        timer(toBvr(1.6)),                 // finding
                        timer(toBvr(5.4)),
                        restartEv};

    MyCycler sndCyl = new MyCycler(sndBvr, sndEv);

    setSound((SoundBvr)sndCyl.getBvr());
  }

  void init(BvrsToRun blst) {
    // Initialize data members.  The behaviors declared as data members
    // are those that get used by different member functions.  By reusing the
    // same instance of behaviors across multiple functions, we can reduce
    // the number of behaviors created by the system.

    // We want to dynamically texture the back face of the box in the program,
    // so we'll import two geometries for the box.  One for the back of the box
    // - panel2.x, the other for the rest of the box - box.x, an elongated
    // cube without the back face.
    _panel = mapToCenteredUnitBox(importGeometry(buildURL(getImportBase(),"geometry/panel2.x")));//, emptyGeometry, null, null, null));
    _panel = _panel.transform(rotate(zVector3, toBvr(Math.PI)));
    _boxFront = mapToCenteredUnitBox(importGeometry(buildURL(getImportBase(),"geometry/box.x")));//, emptyGeometry, null, null, null));
    //_boxFront = _boxFront.transform(rotate(yVector3, toBvr(Math.PI)));

    // Scale the panel to the width and height of the box, rotate it by
    // 180 degree so it's facing back, then move it to the position where
    // the back of the box should be.
    Point3Bvr maxPt = _boxFront.boundingBox().getMax();
    _boxZ = maxPt.getZ();
    Transform3Bvr xfBack = compose(translate(bvr0, bvr0, neg(_boxZ)),
                             compose(rotate(yVector3, toBvr(Math.PI)),
                                   scale(mul(bvr2, maxPt.getX()),
                                         mul(bvr2, maxPt.getY()), bvr1)));

    _boxBack = _panel.transform(xfBack);

    // Apply texture to the back face and put the two geometries together.
    ImageBvr textureImg = importImage(buildURL(
                                      getImportBase(),"image/boxback_movie.gif"),
                                      emptyImage, null, null, null);
    
    _boxGeo = union(_boxFront.texture(importImage(buildURL(getImportBase(),"image/box_top.gif"))), _boxBack.texture(textureImg.mapToUnitSquare()));

    // The image for the text "Encyclopedia".
    FontStyleBvr fs1 = defaultFont.italic().color(txtClr);
      
    _imgEncyc = stringImage(toBvr("Encyclopedia"), fs1)
                        .transform(scale2(1.7));

    // Import the images we'll use later.
    _boxImg = importImage(buildURL(getImportBase(),"image/box_boxart.gif"), emptyImage, null, null, null);

    _box3DImg = importImage(buildURL(
                            getImportBase(),"image/box3D_movie.gif"),
                            emptyImage, null, null, null);

    _box3DImg = _box3DImg.transform(translate(mul(toBvr(-57),pixelBvr),
                        mul(toBvr(-1),pixelBvr)));
    ImageBvr closeImg = importImage(buildURL(imgBase,"minimize.gif"), emptyImage, null, null, null);

    _closeImg = new PickableImage(closeImg.transform(
                        translate(mul(toBvr(137),pixelBvr),
        mul(toBvr(137),pixelBvr))));
    _closeEv = andEvent(leftButtonDown, _closeImg.getPickEvent());

    NumberBvr halfViewSize = div(viewSize, bvr2);
    _viewerLowerLeft = point2(neg(halfViewSize), neg(halfViewSize));
    _viewerUpperRight = point2(halfViewSize, halfViewSize);

    Pytha pytha = new Pytha(imgBase, sndBase);
    _startPythaEv = pytha._startPythaEv;
    _resetPythaPtsEv = pytha._resetPythaPtsEv;
    initIllustration(PYTHA_ID, pytha.img(), pytha.snd(), timer(bvr1));

    EncartaMovie movie = new EncartaMovie(movieBase,imgBase);
    initIllustration(MOVIE_ID, movie._img1, silence, timer(toBvr(7)));

    MiniSolar solar = new MiniSolar(imgBase, sndBase, geoBase);
    initIllustration(SOLAR_ID, solar._img, solar._snd, timer(toBvr(13)));

    waterfallmodel waterfall = new waterfallmodel(mediaBase);
    waterfall.createModel(blst);

    // Waterfall is 600x320 and our view size is 300x300.  I'd have used
    // the boundingbox instead, but the tree branches in waterfall extend
    // beyond the background image.
    ImageBvr waterfallImg = overlay(waterfall.getImage().transform(scale2(toBvr(0.5))),
                                    solidColorImage(colorRgb(toBvr(0.19), toBvr(0.5), toBvr(0.87))));
    SoundBvr waterfallSnd = waterfall.getSound().gain(toBvr(0.8));
    initIllustration(WATER_ID, waterfallImg, waterfallSnd, timer(toBvr(19)));

  }

  void initIllustration(int index, ImageBvr img,
                               SoundBvr snd, DXMEvent startEv) {

    _animImg[index] = overlay(_closeImg.getImageBvr(), img);
    _snd[index] = snd;
    _startEv[index] = startEv;
  }

  // The letters ENCARTA, with changing text colors, drop into the window
  // one by one.
  ImageBvr imgEncarta() {
    // Create 4 arrays, one for the letters, one for the positions of
    // the letters they drop from, one for the positions they drop to,
    // and one for the images for the letters.
    String[] letters = {"E", "N", "C", "A", "R", "T", "A"};
    int len = letters.length;
    Vector2Bvr[] froms = new Vector2Bvr[len];
    Vector2Bvr[] tos  = new Vector2Bvr[len];
    ImageBvr[] imgs = new ImageBvr[len];

    // The y positions for all letters are the same.
    NumberBvr yFrom = mul(toBvr(150),pixelBvr);
    NumberBvr yTo = mul(toBvr(40),pixelBvr);

    // We'll initialize the arrays as well as create two additional images
    // in the for loop.  One of the images is the static image after
    // all the letters are dropped, the other is the image with the
    // letters dropping.
    _imgEncarta = emptyImage;
    _imgAnimEncarta = emptyImage;

    // The index to the middle of the array.
    int iMid = len/2;

    for(int i = 0; i < len; i++) {

      // Initialize the positions for the letter.
      NumberBvr x = mul(toBvr(40*(i-iMid)),pixelBvr);
      froms[i] = vector2(x, yFrom);
      tos[i] = vector2(x, yTo);

      // Initialize the text image for the letter.
      ColorBvr animClr = colorHsl(add(toBvr(0.5),
                                mul(smooth0to1(mul(toBvr(0.8), localTime)),
        toBvr(0.2))), toBvr(0.8),toBvr(0.4));
      FontStyleBvr fs1 = defaultFont.bold().color(animClr);
      FontStyleBvr fs2 = defaultFont.bold().color(txtClr);
      
      imgs[i] = stringImage(toBvr(letters[i]), fs1)
                          .transform(scale2(toBvr(3)));

                        StringBvr txt = toBvr(letters[i]);

      //TextBvr txt = simpleText(toBvr(letters[i])).bold();
      //imgs[i] = txt.color(animClr).
      //                    render().transform(scale2(toBvr(3)));

      // Accumulate the static image.
      ImageBvr imgStaticClr = stringImage(txt, fs2)
                          .transform(scale2(toBvr(3)));

      //ImageBvr imgStaticClr = txt.color(txtClr).
      //                         render().transform(scale2(toBvr(3)));
      _imgEncarta = overlay(imgStaticClr.transform(translate(tos[i])), _imgEncarta);

      // Accumulate the animating image.
      _imgAnimEncarta =
        overlay(imgStaticClr.transform(compose(translate(tos[i]),
                                               scale(add(bvr1, sin(mul(bvr2, localTime))),
                                                     add(bvr1, mul(toBvr(0.5), sin(mul(bvr2, localTime))))))),
                _imgAnimEncarta);
    }

    // Slide the array of the images we just created.
    return slide(imgs, froms, tos, mul(localTime, toBvr(4)), 0.06);
  }

  // Show the "Encyclopedia" text by gradually changing the crop window.
  ImageBvr imgEncyclopedia() {
    return overlay(showFromLeft(_imgEncyc, mul(localTime, bvr1)),
                   _imgEncarta);
  }

  // Scale up the "Encarta" text up a little then down to its original size.
  ImageBvr imgScaleUpEncarta() {
    ImageBvr imgEncarta = (ImageBvr)until(_imgAnimEncarta,
                                          timer(toBvr(Math.PI/2)),
                                          _imgEncarta);

    return overlay(_imgEncyc, imgEncarta);
  }

  ImageBvr imgAdd2000() {
    ColorBvr clr = colorHsl(bkgndClr.getHue(),
                            bkgndClr.getSaturation(), toBvr(0.95));
    FontStyleBvr fs1 = defaultFont.bold().color(clr);
      
    ImageBvr img2000 = stringImage(toBvr("2000"), fs1)
            .transform(compose(translate(0.015,0.0005),scale2(5)));


    // ImageBvr img2000 = simpleText(toBvr("2000")).bold().
    //                      color(clr).render().transform(compose(translate(0.015,0.0005),
    //                                         scale2(toBvr(5))));
    ImageBvr imgEncarta = _imgEncarta;

    ImageBvr imgIn = overlay(imgEncarta, overlay(_imgEncyc, img2000));

    NumberBvr xPos = mul(localTime, toBvr(0.06));
    ImageBvr imgOut = overlay(imgEncarta.transform(
                              translate(xPos, bvr0)),
                        overlay(_imgEncyc.transform(
                                translate(neg(xPos), bvr0)),
                                img2000.transform(
                                translate(xPos, bvr0))));

    return (ImageBvr)until(imgIn,
                           timer(bvr2),
                           imgOut);
  }

  static GeometryBvr mapToCenteredUnitBox(GeometryBvr geo) {
    Bbox3Bvr bbox = geo.boundingBox();
    Point3Bvr min = bbox.getMin();
    Point3Bvr max = bbox.getMax();

    Vector3Bvr extent = sub(max, min);
    Point3Bvr center = add(min, extent.div(bvr2));

    NumberBvr maxExtent = findMax(extent.getX(),
                                  findMax(extent.getY(),
                                          extent.getZ()));

    // Move the object to the origin, resize, and move to the unit box.

    Transform3Bvr normalizingXform =
                     compose(scale3(div(bvr1, maxExtent)),
                             translate(sub(origin3, center)));

    return geo.transform(normalizingXform);
  }

  ImageBvr boxRollIn() {
    Transform3Bvr xf = compose(scale3(mul(toBvr(410),pixelBvr)),
                          compose(scale3(clampMax(div(mul(toBvr(1.5), localTime),
                                                      toBvr(Math.PI*1.2)),
                                                  bvr1)),
                          compose(translate(bvr0, bvr0, neg(_boxZ)),
                          compose(rotate(xVector3, add(toBvr(Math.PI*-1.2),
                                                       clampMax(mul(localTime, toBvr(1.5)),
                                                                toBvr(Math.PI*1.2)))),
                                  rotate(yVector3, sub(toBvr(Math.PI*1.2*1.8),
                                                       clampMax(mul(localTime, toBvr(1.8*1.5)),
                                                                toBvr(Math.PI*1.2*1.8))))))));

    // Move the projection point closer to maintain the perspective.
    // Translate the camera twice as far so the 3D object is not clipped out.
    _camera = perspectiveCamera(toBvr(0.15), bvr0).
                         transform(translate(bvr0, bvr0, toBvr(0.15)));

    ImageBvr tumbling3D = union(_boxGeo, union(directionalLight, ambientLight)).
                                transform(xf).render(_camera);

    return (ImageBvr)until(tumbling3D, timer(toBvr(Math.PI*1.2/1.5)),
                           _boxImg);
  }

  ImageBvr scaleUpImg(ImageBvr img, Transform2Bvr xf, double startTime) {
    return (ImageBvr)until(emptyImage,
                           timer(toBvr(startTime)),
                           img.transform(compose(xf, scale2(clampMax(
                                         mul(localTime, toBvr(1.2)), bvr1)))));
  }

  ImageBvr rotatingImage(ImageBvr img, Vector2Bvr vec) {
      Transform2Bvr xf =
        compose(translate(vec.mul(clampMin(sub(bvr1, mul(toBvr(0.5), localTime)),
                                           bvr0)).
                              transform(rotate(mul(toBvr(4), localTime)))),
                scale2(clampMin(sub(bvr1, mul(toBvr(0.5), localTime)),
                                        bvr0)));


      return img.transform(xf);
    }

  ImageBvr boxAndText() {
    FontStyleBvr fs1 = defaultFont.bold().italic()
                        .color(colorHsl(toBvr(0.6), toBvr(0.8), toBvr(0.4)));
    FontStyleBvr fs2 = defaultFont.bold().italic()
                        .color(colorHsl(toBvr(0.8), toBvr(0.8), toBvr(0.4)));
    FontStyleBvr fs3 = defaultFont.bold().italic()
                        .color(colorHsl(toBvr(0.9), toBvr(0.8), toBvr(0.4)));
    FontStyleBvr fs4 = defaultFont.bold().italic()
                        .color(colorHsl(bvr0, toBvr(0.8), toBvr(0.4)));
      
    ImageBvr imgExp = stringImage(toBvr("Exploring"), fs1)
                        .transform(scale2(bvr2));

    ImageBvr imgSearch = stringImage(toBvr("Searching"), fs2)
                        .transform(scale2(bvr2));

    ImageBvr imgBrowse = stringImage(toBvr("Browsing"), fs3)
                        .transform(scale2(bvr2));

    ImageBvr imgLearn = stringImage(toBvr("Learning"), fs4)
                        .transform(scale2(bvr2));

    // ImageBvr imgExp = simpleText(toBvr("Exploring")).bold().italic().
    //                    color(colorHsl(toBvr(0.6), toBvr(0.8), toBvr(0.4))).render().
    //                        transform(scale2(bvr2));

    // ImageBvr imgSearch = simpleText(toBvr("Searching")).bold().italic().
    //                       color(colorHsl(toBvr(0.8), toBvr(0.8), toBvr(0.4))).render().
    //                       transform(scale2(bvr2));

    //ImageBvr imgBrowse = simpleText(toBvr("Browsing")).bold().italic().
    //                       color(colorHsl(toBvr(0.9), toBvr(0.8), toBvr(0.4))).render().
    //                      transform(scale2(bvr2));

    //ImageBvr imgLearn = simpleText(toBvr("Learning")).bold().italic().
    //                        color(colorHsl(bvr0, toBvr(0.8), toBvr(0.4))).render().
    //                        transform(scale2(bvr2));

    Vector2Bvr vecExp = vector2(toBvr(-0.01), toBvr(0.015));
    Vector2Bvr vecSearch = vector2(toBvr(0.01), toBvr(0.005));
    Vector2Bvr vecBrowse = vector2(toBvr(-0.01), toBvr(-0.005));
    Vector2Bvr vecLearn = vector2(toBvr(0.01), toBvr(-0.015));

    ImageBvr imgText =
          overlay(scaleUpImg(imgExp, translate(vecExp), 0),
            overlay(scaleUpImg(imgSearch, translate(vecSearch), 0.7),
              overlay(scaleUpImg(imgBrowse, translate(vecBrowse), 1.4),
                      scaleUpImg(imgLearn, translate(vecLearn), 2.1))));

    ImageBvr rotatingTxt =
          overlay(rotatingImage(imgExp, vecExp),
            overlay(rotatingImage(imgSearch, vecSearch),
              overlay(rotatingImage(imgBrowse, vecBrowse),
                      rotatingImage(imgLearn, vecLearn))));

    ImageBvr txt = (ImageBvr)until(imgText,
                                   timer(toBvr(3.6)),
                                   rotatingTxt);

    Transform2Bvr xfBkgnd =
        (Transform2Bvr)until(identityTransform2,
                             timer(toBvr(3.6)),
                             scale2(sub(bvr1, div(localTime, toBvr(1.8)))));

    ImageBvr boxBkgnd =
        _boxImg.opacity(clampMin(sub(bvr1, localTime), toBvr(0.5))).
              transform(xfBkgnd);

    return overlay(txt, boxBkgnd);
  }

  ImageBvr ultimateTool() {
    FontStyleBvr fs1 = defaultFont.bold().color(txtClr);

    FontStyleBvr fs2 = defaultFont.bold().color(red);
      
    ImageBvr imgTool = stringImage(toBvr("The ultimate tool"), fs1)
                        .transform(scale(bvr2, toBvr(3)));

    ImageBvr imgFor = stringImage(toBvr("for"), fs1)
                        .transform(compose(translate(bvr0, toBvr(-0.02)),
                                  scale(bvr2, toBvr(3))));

    ImageBvr imgDis = stringImage(toBvr("discovering"), fs2)
                        .transform(compose(translate(bvr0, toBvr(-0.04)),scale2(bvr2)));

    ImageBvr imgInfo = stringImage(toBvr("information"), fs2)
                        .transform(compose(translate(bvr0, toBvr(-0.05)),scale2(bvr2)));

    //ImageBvr imgTool = simpleText(toBvr("The ultimate tool")).bold().color(txtClr).render().
    //            transform(scale(bvr2, toBvr(3)));
    //ImageBvr imgFor = simpleText(toBvr("for")).bold().color(txtClr).render().
    //            transform(compose(translate(bvr0, toBvr(-0.02)),
    //                              scale(bvr2, toBvr(3))));
    //ImageBvr imgDis = simpleText(toBvr("discovering")).bold().color(red).render().
    //           transform(compose(translate(bvr0, toBvr(-0.04)),
    //                             scale2(bvr2)));
    //ImageBvr imgInfo = simpleText(toBvr("information")).bold().color(red).render().
    //           transform(compose(translate(bvr0, toBvr(-0.05)),
    //                             scale2(bvr2)));

    ImageBvr slide1[] = {overlay(imgTool, imgFor)};
    ImageBvr slide2[] = {overlay(imgDis, imgInfo)};
    ImageBvr slide3[] = {_boxImg};
    Vector2Bvr vec1From[] = {vector2(bvr0, toBvr(0.025))};
    Vector2Bvr vec1To[] = {vector2(0.1, 0.025)};
    Vector2Bvr vec2To[] = {vector2(-0.1, 0.025)};
    Vector2Bvr vec3From[] = {vector2(bvr0, toBvr(-0.05))};
    Vector2Bvr vec3To[] = {zeroVector2};
    ImageBvr img4 = overlay(slide(slide1, vec1From, vec1To, localTime, 0),
                      overlay(slide(slide2, vec1From, vec2To, localTime, 0),
                        slide(slide3, vec3From, vec3To, localTime, 0)));

    ImageBvr slide5[] = {overlay(overlay(imgTool, imgFor),
                                 overlay(imgDis, imgInfo))};
    ImageBvr img3 = (ImageBvr)until(slide(slide5,
                                          vec3To, vec1From,  localTime, 0),
                                    timer(toBvr(1.8)),
                                    img4);

    ImageBvr img2 = (ImageBvr)until(overlay(imgTool, imgFor),
                                    timer(toBvr(0.8)), img3);
    return (ImageBvr)until(imgTool, timer(toBvr(0.8)), img2);
  }

  ImageBvr imageTexture(Transform3Bvr xf, ImageBvr img) {
    GeometryBvr geo = _panel.transform(xf).
                        texture(img.crop(_viewerLowerLeft, _viewerUpperRight).
                          mapToUnitSquare());

    return union(union(directionalLight, ambientLight),
                 geo).render(_camera);
  }

  ImageBvr showIllustration(int index,
                              NumberBvr sizeOnBox, Vector3Bvr vec)
  {
    Transform3Bvr xfBox =
      compose(scale3(mul(toBvr(410),pixelBvr)),
        compose(translate(toBvr(-0.3), bvr0, sub(toBvr(-0.5), _boxZ)),
          compose(rotate(yVector3, toBvr(Math.PI/3.0)),
            compose(translate(bvr0, bvr0, _boxZ),
              compose(translate(vec),
                      scale3(sizeOnBox))))));

    double boxSize = ((Double) sizeOnBox.extract()).doubleValue();

    // take 1 sec to translate, 300/205-sizeOnBox sec to scale,
    // 1 sec to rotate, 1 sec to move to center
    Transform3Bvr xfToView =
      compose(scale3(mul(toBvr(410),pixelBvr)),
        compose(translate(add(toBvr(-0.3),
                            clampMax(mul(localTime, toBvr(0.3)), toBvr(0.3))),
                        bvr0,
                        add(sub(toBvr(-0.5), _boxZ),
                            clampMax(mul(localTime, toBvr(0.5)), toBvr(0.5)))),
          compose(rotate(yVector3, mul(toBvr(Math.PI/3.0),
                                       sub(bvr1, clampMax(localTime, bvr1)))),
            compose(translate(bvr0, bvr0, _boxZ),
              compose(translate(add(vec, vec.mul(clampMin(neg(localTime), toBvr(-1))))),
                      scale3(clampMax(add(sizeOnBox,
                                          mul(localTime, toBvr(1.0/(300.0/205.0-boxSize)))), toBvr(300.0/205.0)))
                      )))));

    // take 300/205-sizeOnBox to scale (~1.26 sec in the worst case),
    // 1 sec to translate
    Transform3Bvr xfToBox =
      compose(scale3(mul(toBvr(410),pixelBvr)),
        compose(translate(add(toBvr(-0.3),
                            sub(toBvr(0.3), clampMax(mul(localTime, toBvr(0.3)), toBvr(0.3)))),
                        bvr0,
                        add(sub(toBvr(-0.5), _boxZ),
                            sub(toBvr(0.5), clampMax(mul(localTime, toBvr(0.5)), toBvr(0.5))))),
          compose(rotate(yVector3, mul(toBvr(Math.PI/3.0),
                                       clampMax(localTime, bvr1))),
            compose(translate(bvr0, bvr0, _boxZ),
              compose(translate(vec.mul(clampMax(localTime, bvr1))),
                      scale3(clampMin(sub(toBvr(300.0/205.0), localTime), sizeOnBox))
                                  )))));

    ImageBvr img = ImageBvr.newUninitBvr();
    img.init(until(_animImg[index], _pickEv[index], img));
    ImageBvr imgRunOnce = (ImageBvr)img.runOnce();

    // Overlay the image fly forward to view on detectableEmptyImage so other
    // illustrations can't be picked.
    ImageBvr textureToView = overlay(imageTexture(xfToView, imgRunOnce),
                                     detectableEmptyImage);
    ImageBvr textureToBox = imageTexture(xfToBox, imgRunOnce);

    FlyForwardNotifier flyForward = new FlyForwardNotifier(index, textureToView,
                                      imgRunOnce, textureToBox, _snd[index],
                                      _pickEv[index], _closeEv, _startPythaEv,
                                      _resetPythaPtsEv, _illusSnd);

    return (ImageBvr)untilEx(emptyImage,
                             orEvent(_startEv[index].notifyEvent(flyForward),
                                     leftButtonDown.notifyEvent(new PickedNotifier(_pickEv[index], flyForward))));

/* COVER_2D
    Transform3Bvr xfCover =
      compose(scale3(mul(toBvr(410), pixelBvr)),
            compose(translate(bvr0, bvr0, _boxZ),
              compose(translate(vec),
                      scale3(sizeOnBox))));

    ImageBvr img2D = imageTexture(xfCover, imgRunOnce);
    return img2D;
*/

/* COVER_3D
    ImageBvr img3D = imageTexture(xfToBox, imgRunOnce);
    return img3D;
*/
  }

  ImageBvr illustrations() {

    NumberBvr size[] = {toBvr(0.23), toBvr(0.27), toBvr(0.29), toBvr(0.2)};
    Vector3Bvr moveTo[] = {
                           vector3(toBvr(-0.23), toBvr(0.05), bvr0),
                           vector3(toBvr(0.07), toBvr(0.1), bvr0),
                           vector3(toBvr(-0.1), toBvr(-0.2), bvr0),
                           vector3(toBvr(0.2), toBvr(-0.1), bvr0)};

    Point2Bvr cropBox[] = {
                            point2(mul(toBvr(25),pixelBvr),
                                   mul(toBvr(42),pixelBvr)),
                            point2(mul(toBvr(23),pixelBvr), 
                                   mul(toBvr(43),pixelBvr)),
                            point2(mul(toBvr(30),pixelBvr),
                                   mul(toBvr(48),pixelBvr)),
                            point2(mul(toBvr(16),pixelBvr),
                                   mul(toBvr(32),pixelBvr))};

    Vector2Bvr moveTo2[] = {
                            vector2(mul(toBvr(-78),pixelBvr),
                                    mul(toBvr(-10),pixelBvr)),
                            vector2(mul(toBvr(-46),pixelBvr),
                                    mul(toBvr(-4),pixelBvr)),
                            vector2(mul(toBvr(-67),pixelBvr),
                                    mul(toBvr(-59),pixelBvr)),
                            vector2(mul(toBvr(-28),pixelBvr),
                                    mul(toBvr(-31),pixelBvr))};

    ImageBvr pimgAll = emptyImage;
    for (int i = 0; i < NUM_ILLUSTRATIONS; i++) {
      PickableImage pimg = new PickableImage(detectableEmptyImage);
      _pickEv[i] = andEvent(leftButtonDown, pimg.getPickEvent());

      pimgAll = overlay(pimg.getImageBvr().crop(origin2, cropBox[i]).
                          transform(translate(moveTo2[i])),
                        pimgAll);

      ImageBvr illusImg = showIllustration(i, size[i], moveTo[i]);
      _illusImg[i] = (ImageBvr)illusImg.runOnce();
    }

    DXMEvent to3DEv = timer(toBvr(2.3));
    ImageBvr iconImg = (ImageBvr)until(emptyImage,
                            to3DEv,
                            overlay(overlay(overlay(_illusImg[3], _illusImg[2]), _illusImg[1]), _illusImg[0]));
    // The transform for turning the box so the back of the box is facing
    // the front.
    // takes PI/3 seconds
    Transform3Bvr xfTurning = compose(scale3(mul(toBvr(410),pixelBvr)),
                          compose(translate(bvr0, bvr0, neg(_boxZ)),
                          // from 0 to pi/4, stays at pi/4 for a while, goes back to 0

                          //compose(rotate(xVector3,
                          //               clampMin(mul(sin(mul(localTime, toBvr(3))),
                          //                            toBvr(Math.PI/4.0)),
                          //                        bvr0)),

                          // time              value
                          // 0 - 0.3           0
                          // 0.3 - 0.3+pi/3    0 - pi
                                  rotate(yVector3, clampMax(clampMin(mul(localTime, toBvr(3)),
                                                                     bvr0),
                                                                toBvr(Math.PI)))));

    // The transform to move the box to the side of the view.
    // take PI/3 seconds
    Transform3Bvr xfMoveAside = compose(scale3(mul(toBvr(410),pixelBvr)),
                          compose(translate(clampMin(neg(mul(localTime, toBvr(0.3))), toBvr(-0.3)),
                                            bvr0,
                                            sub(neg(_boxZ), clampMax(mul(localTime, toBvr(0.5)), toBvr(0.5)))),
                                  rotate(yVector3, add(clampMax(localTime, toBvr(Math.PI/3.0)),
                                                       toBvr(Math.PI)))));

    // Move the projection point closer to maintain the perspective.
    // Translate the camera twice as far so the 3D object is not clipped out.

    Transform3Bvr xfBox = (Transform3Bvr)until(xfTurning, timer(toBvr(1.1)),
                                               xfMoveAside);

    GeometryBvr xfedBox = _boxGeo.transform(xfBox);

    ImageBvr showBoxBack = union(xfedBox,
                                 union(directionalLight, ambientLight)).
                                 render(_camera);

    showBoxBack = (ImageBvr)until(showBoxBack, to3DEv, _box3DImg);

    ImageBvr text = (ImageBvr)until(emptyImage,
                                    to3DEv,
                                    instructionalTxt());

    return (ImageBvr)overlay(overlay(iconImg, pimgAll),
                             overlay(showBoxBack, text)).runOnce();

/* COVER_2D
    ImageBvr backcover = importImage(buildURL(
                                     getImportBase(),"../../../../../Medialib/image/demosafe/boxback_blank.gif"),
                                     emptyImage, null, null, null);
    return overlay(iconImg, backcover);
*/

/* COVER_3D
    ImageBvr backcover = importImage(buildURL(
                          getImportBase(),"../../../../../Medialib/image/demosafe/box3D_blank.gif"),
                          emptyImage, null, null, null);
    backcover = backcover.transform(translate(mul(toBvr(-57),pixelBvr),
                        mul(toBvr(-1),pixelBvr)));
    return overlay(iconImg, backcover);
*/
  }

  static NumberBvr smooth0to1(NumberBvr rate) {
    return (NumberBvr)until(sub(toBvr(0.5), div(cos(mul(rate, toBvr(Math.PI))), bvr2)),
                            timer(bvr1), bvr1);

  }

  ImageBvr txtHelper(String str) {
    FontStyleBvr fs1 = defaultFont.bold().color(txtClr);
      
    return stringImage(toBvr(str), fs1);

  }

  ImageBvr instructionalTxt() {

    ImageBvr txt1 = txtHelper("Click on the");
    ImageBvr txt2 = txtHelper("pictures on the");
    ImageBvr txt3 = txtHelper("back of the box");
    ImageBvr txt4 = txtHelper("to play with the");
    ImageBvr txt5 = txtHelper("illustrations!");
    NumberBvr minX = txt1.boundingBox().getMin().getX();
    NumberBvr y1 = txt1.boundingBox().getMax().getY();
    NumberBvr x2 = txt2.boundingBox().getMin().getX();
    NumberBvr x3 = txt3.boundingBox().getMin().getX();
    NumberBvr x4 = txt4.boundingBox().getMin().getX();
    NumberBvr x5 = txt5.boundingBox().getMin().getX();

    ImageBvr txtAll =
      overlay(txt1.transform(translate(bvr0, mul(y1, toBvr(6)))),
        overlay(txt2.transform(translate(sub(minX, x2), mul(y1, toBvr(3)))),
        overlay(txt3.transform(translate(sub(minX, x3), bvr0)),
        overlay(txt4.transform(translate(sub(minX, x4), mul(y1, toBvr(-3)))),
                txt5.transform(translate(sub(minX, x5), mul(y1, toBvr(-6))))))));

    NumberBvr maxX = txtAll.boundingBox().getMax().getX();

    NumberBvr decreasingNum = mul(localTime, toBvr(-0.005));
    NumberBvr movingX = NumberBvr.newUninitBvr();
    movingX.init(until(decreasingNum,
                       predicate(lt(decreasingNum,
                                    sub(mul(bvr2, _viewerLowerLeft.getX()),
                                        sub(maxX, minX)))),
                       movingX));

    Transform2Bvr xf = translate(add(sub(_viewerUpperRight.getX(), minX),
                                     movingX),
                                 bvr0);

    //Transform2Bvr xf = translate(toBvr(65*pixel), bvr0);

    return txtAll.transform(xf);
  }

  static ImageBvr slide(ImageBvr[] imgs, Vector2Bvr[] froms,
                        Vector2Bvr[] tos, NumberBvr rate, double timeNext) {

    ImageBvr accumImg = emptyImage;

    for(int i = 0; i < imgs.length; i++) {
      Transform2Bvr xf =
        translate(add(froms[i],
                      sub(tos[i],froms[i]).mul(clampMax(rate, bvr1))));

      ImageBvr newImg;
      if (i == 0) {

        // special case the first image so it doesn't flash
        newImg = imgs[i].transform(xf);

      } else {

        newImg = (ImageBvr)until(emptyImage,
                                 timer(toBvr(i*timeNext)),
                                 imgs[i].transform(xf));
      }

      accumImg = overlay(newImg, accumImg);
    }

    return accumImg;
  }

  static ImageBvr showFromLeft(ImageBvr img, NumberBvr rate) {
    Bbox2Bvr bb = img.boundingBox();
    Point2Bvr min = bb.getMin();
    NumberBvr minX = min.getX();
    NumberBvr maxX = bb.getMax().getX();
    NumberBvr maxY = bb.getMax().getY();

    ImageBvr croppedImg =
        img.crop(min, point2(add(minX, mul(sub(maxX, minX),
                                             clampMax(rate, bvr1))),
                               maxY));

    return croppedImg;
  }

  static NumberBvr clampMax(NumberBvr val, NumberBvr max) {
      return (NumberBvr)cond(gt(val, max), max, val);
  }

  static NumberBvr clampMin(NumberBvr val, NumberBvr min) {
      return (NumberBvr)cond(lt(val, min), min, val);
  }

  static NumberBvr findMax(NumberBvr a, NumberBvr b) {
      return (NumberBvr)cond(gt(a, b), a, b);
  }
    public void cleanup() {
    super.cleanup();
    viewSize = null;
    _boxImg = null;
	_box3DImg = null;
    _boxGeo= null;
	_boxFront= null; 
	_boxBack= null;
    _panel= null;
    _boxZ= null;
    _camera= null;
    _restartEv= null;
	_closeEv= null;
    _illusSnd= null;

    _viewerUpperRight= null;
    _viewerLowerLeft= null;
    _animImg = null;
    _pickEv = null;
    _pickEvData= null;
    _startEv = null;
    _snd = null;
    _imgEncarta= null;
	_imgEncyc= null;
	_imgAnimEncarta= null;
    _allIllusImg= null;
    _closeImg= null;
    _startPythaEv= null;
	_resetPythaPtsEv= null;
    _illusImg = null;

    mediaBase= null;
    imgBase= null;
    sndBase= null;
    midiBase= null;
    geoBase= null;
    movieBase= null;
  }


  static ColorBvr txtClr = colorHsl(toBvr(0.7), toBvr(0.8), toBvr(0.4));
  static NumberBvr bkgndR = toBvr(0.708);
  static NumberBvr bkgndG = toBvr(0.886);
  static NumberBvr bkgndB = toBvr(0.969);
  static ColorBvr bkgndClr = colorRgb(bkgndR, bkgndG, bkgndB);

  static final int NUM_ILLUSTRATIONS = 4;
  static final int PYTHA_ID = 0, MOVIE_ID = 1, SOLAR_ID = 2, WATER_ID = 3;
  static NumberBvr bvr0 = toBvr(0);
  static NumberBvr bvr1 = toBvr(1);
  static NumberBvr bvr2 = toBvr(2);

  NumberBvr viewSize = mul(toBvr(300),pixelBvr);
  ImageBvr _boxImg, _box3DImg;
  GeometryBvr _boxGeo, _boxFront, _boxBack;
  GeometryBvr _panel;
  NumberBvr _boxZ;
  CameraBvr _camera;
  DXMEvent _restartEv, _closeEv;
  ModifiableBehavior _illusSnd;

  Point2Bvr _viewerUpperRight;
  Point2Bvr _viewerLowerLeft;
  ImageBvr _animImg[] = new ImageBvr[NUM_ILLUSTRATIONS];
  DXMEvent _pickEv[] = new DXMEvent [NUM_ILLUSTRATIONS];
  DXMEvent _pickEvData[] = new DXMEvent [NUM_ILLUSTRATIONS];
  DXMEvent _startEv[] = new DXMEvent [NUM_ILLUSTRATIONS];
  SoundBvr _snd[] = new SoundBvr[NUM_ILLUSTRATIONS];
  ImageBvr _imgEncarta, _imgEncyc, _imgAnimEncarta;
  ImageBvr _allIllusImg;
  PickableImage _closeImg;
  AppTriggeredEvent _startPythaEv, _resetPythaPtsEv;
  ImageBvr _illusImg[] = new ImageBvr[NUM_ILLUSTRATIONS];

  URL mediaBase;
  URL imgBase;
  URL sndBase;
  URL midiBase;
  URL geoBase;
  URL movieBase;

}

public class EncartaApplet extends DXMApplet {
  public void init() {
    super.init();
	_model = new EncartaModel();
    setModel(_model);
  }

  public void destroy() {
    super.destroy();
    _model = null;
  }
  private EncartaModel _model;

}

class MyCycler extends Statics implements UntilNotifier {
  MyCycler(Behavior abvr[], DXMEvent aev[]) {
    _count = 0;
    _len = abvr.length;
    _abvr = abvr;
    _aev = aev;

    _bvr = untilNotify(abvr[0], aev[0], this);
  }

  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

    _count++;
    int i = _count % _len;
    return untilNotify(_abvr[i], _aev[i], this);
  }

  Behavior getBvr() {
    return _bvr;
  }

  public void cleanup() {
    _bvr = null;
    _abvr = null;
    _aev = null;
  }


  int _count;
  int _len;
  Behavior _bvr;
  Behavior _abvr[];
  DXMEvent _aev[];
  
}

// <Tutorial Section=1.0 Title="Section1: Using video in your applet">

// This section will guide you through the process of playing a video clip
// in your applet.  You'll also learn how to pause, fast forward, and rewind
// your video with the substituteTime function.

class EncartaMovie extends Statics implements UntilNotifier {

  EncartaMovie(URL movieBase, URL imgBase) {
    // We import movie files the same way we import static images.
    // We'll use a solid black background behind the movie image.
    SoundBvr [] sndArr = { null } ;
    ImageBvr [] ImArr = { null } ;
        URL _movieBase = movieBase;
        URL _imgBase = imgBase;

    //!!! len returns -1 now, work around by guessing how long the video is.
    //System.out.println("len = "+len);
    NumberBvr len;
    DXMEvent[] movieLoaded = {null};
    len = importMovie(buildURL(_movieBase,"movie.avi"), ImArr, sndArr,
      emptyImage, silence, movieLoaded, null, null);

    ImageBvr avi = ImArr[0];
    _snd = sndArr[0];
    
    ImageBvr movieBkgnd = solidColorImage(black);

    // We then import the bitmap for the playback controls.
    // We put the bitmap at the pixel position of (12, -135).

    NumberBvr yPos = mul(toBvr(-135),pixelBvr);
    ImageBvr imgPlayback = importImage(buildURL(_imgBase,"playback.gif"), emptyImage, null, null, null).
                             transform(translate(mul(toBvr(12),pixelBvr), yPos));

    // Since there are four controls - pause, restart, fast forward, and
    // rewind - in the playback bitmap, we'll lay four detectableEmptyImage
    // on top of the bitmap so we know which one is picked.
    // The pause, fast forward, and rewind buttons are 24x20 in size.
    // The pause, restart, rewind, and fast forward buttons are 25, 70, 25,
    // 24 pixels wide respectively.
    Point2Bvr buttonLL = point2(mul(toBvr(-12),pixelBvr),
                        mul(toBvr(-10),pixelBvr));
    Point2Bvr buttonUR = point2(mul(toBvr(12),pixelBvr),
                        mul(toBvr(10),pixelBvr));
    PickableImage pauseButton =
      new PickableImage(detectableEmptyImage.crop(buttonLL, buttonUR).
        transform(translate(mul(toBvr(-48),pixelBvr), yPos)));
    PickableImage rwButton =
      new PickableImage(detectableEmptyImage.crop(buttonLL, buttonUR).
        transform(translate(mul(toBvr(48),pixelBvr), yPos)));
    PickableImage ffButton =
      new PickableImage(detectableEmptyImage.crop(buttonLL, buttonUR).
        transform(translate(mul(toBvr(72),pixelBvr), yPos)));
    PickableImage restartButton =
      new PickableImage(detectableEmptyImage.crop(
        point2(mul(toBvr(-35),pixelBvr), mul(toBvr(-10),pixelBvr)),
        point2(mul(toBvr(35),pixelBvr), mul(toBvr(10),pixelBvr))).
        transform(translate(toBvr(0), yPos)));

    // Lay the pickable empty images we just created on top of the bitmap.
    ImageBvr controls = overlay(overlay(pauseButton.getImageBvr(),
                                        restartButton.getImageBvr()),
                                overlay(overlay(rwButton.getImageBvr(),
                                                ffButton.getImageBvr()),
                                        imgPlayback));

    // Define the events that will change the rate the video is played back.
    DXMEvent pauseEv = andEvent(leftButtonDown, pauseButton.getPickEvent());
    DXMEvent restartEv = andEvent(leftButtonDown, restartButton.getPickEvent());
    DXMEvent ffEv = andEvent(leftButtonDown, ffButton.getPickEvent());
    DXMEvent rwEv = andEvent(leftButtonDown, rwButton.getPickEvent());

    // To distinguish which event occurs in the notify() function, which you'll
    // see later, we need to attach an event ID to each event.
    DXMEvent pauseDataEv = andEvent(pauseEv, pauseEv.attachData(new Integer(PAUSE_ID)));
    DXMEvent ffDataEv = andEvent(ffEv, ffEv.attachData(new Integer(FF_ID)));
    DXMEvent rwDataEv = andEvent(rwEv, rwEv.attachData(new Integer(RW_ID)));
    DXMEvent restartDataEv = andEvent(restartEv, restartEv.attachData(new Integer(RESTART_ID)));

    // The rate of the play back is changed when any of the events defined
    // above occurs.  So we or these events together here.
    _rateChangedEv = orEvent(orEvent(pauseDataEv, ffDataEv),
                             orEvent(rwDataEv, restartDataEv));

    // Define the rate of play back here.  The normal rate is that we'll
    // play 1 second of video in 1 second.  The fast forward rate is
    // to play 3 seconds of video in 1 second.  The rewind rate is
    // to go back 3 seconds of video in 1 second.  The pause rate is
    // to stop at the current frame and advance by 0 in the video position.
    _normalRate = toBvr(1);
    _ffRate = toBvr(3);
    _rwRate = toBvr(-3);
    _pauseRate = toBvr(0);

    // Default to normal rate until any rate change event occurs.
    NumberBvr rate = (NumberBvr)untilNotify(_normalRate, _rateChangedEv, this);

    // When the restart event occurs, we want to restart the video from
    // position 0, so the moviePos is a reactive recursive behavior here.
    // We add the current playback rate to the current position by doing
    // the integral(rate).
    NumberBvr moviePos = NumberBvr.newUninitBvr();
    moviePos.init(until(mod(integral(rate), len), restartEv, moviePos));

    // Lay the controls over the video and the background, and save it
    // for our clients' access.
    _img1 = overlay(overlay(controls,
                           (ImageBvr)avi.substituteTime(moviePos)), movieBkgnd);
                           //COVER_2D and COVER_3D
                           //(ImageBvr)avi.substituteTime(toBvr(0))), movieBkgnd);

  }

  // This function is called every time _rateChangedEv occurs.
  // Find out the event ID and return the corresponding playback rate.
  // Clicking on the pause button resets the states for the other buttons.
  // Clicking on the pause button the second time will undo the pause.
  // Ditto for other buttons except the restart button which always restart
  // the video when it's clicked on.
  public Behavior notify(Object evData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

    PairObject andEventPair  = (PairObject)evData;
    int     evID = ((Integer)andEventPair.getSecond()).intValue();

    switch (evID) {
      case PAUSE_ID:
      {
        if (_paused == true) {
          _paused = false;
          return untilNotify(_normalRate, _rateChangedEv, this);
        } else {
          _paused = true;
          _ff = false;
          _rw = false;
          return untilNotify(_pauseRate, _rateChangedEv, this);
        }
      }
      case FF_ID:
        if (_ff == true) {
          _ff = false;
          return untilNotify(_normalRate, _rateChangedEv, this);
        } else {
          _ff = true;
          _paused = false;
          _rw = false;
          return untilNotify(_ffRate, _rateChangedEv, this);
        }

      case RW_ID:
        if (_rw == true) {
          _rw = false;
          return untilNotify(_normalRate, _rateChangedEv, this);
        } else {
          _rw = true;
          _ff = false;
          _paused = false;
          return untilNotify(_rwRate, _rateChangedEv, this);
        }
      case RESTART_ID:
      default:
        _rw = false;
        _ff = false;
        _paused = false;
        return untilNotify(_normalRate, _rateChangedEv, this);
    }
  }

  static void resetState() {
    _paused = false;
    _ff = false;
    _rw = false;
  }

  public void cleanup() {
    _img1 =null;
     _snd=null;
    _rateChangedEv=null;
    _pauseRate=null;
	_ffRate=null;
	_rwRate=null;
	_normalRate=null;
  }

  ImageBvr _img1;
  SoundBvr _snd;
  DXMEvent _rateChangedEv;
  NumberBvr _pauseRate, _ffRate, _rwRate, _normalRate;
  static boolean _paused = false, _ff = false, _rw = false;


  final int PAUSE_ID = 1, FF_ID = 2, RW_ID = 3, RESTART_ID = 4;

}
// </Tutorial>

class PickedNotifier extends Statics implements UntilNotifier {
  PickedNotifier(DXMEvent ev, FlyForwardNotifier flyForward) {
    _ev = ev;
    _flyForward = flyForward;
  }

  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

    //System.out.println("user picked");
    _flyForward.autonomous = false;
    return untilNotify(currentRunningBvr, _ev, _flyForward);
  }

  public void cleanup() {
    _ev = null;
    _flyForward.cleanup();
	_flyForward = null;
  }

  DXMEvent _ev;
  FlyForwardNotifier _flyForward;

}

class FlyForwardNotifier extends Statics implements UntilNotifier {
  FlyForwardNotifier(int evID, ImageBvr imgFlyForward, ImageBvr imgAnim,
                     ImageBvr imgShrinkToBox, SoundBvr snd, DXMEvent pickEv,
                     DXMEvent closeEv, AppTriggeredEvent startPythaEv,
                     AppTriggeredEvent resetPythaPtsEv, ModifiableBehavior sndSwitcher) {
    _myID = evID;
    _bvr = imgFlyForward;
    _snd = snd;
    _sndSwitcher = sndSwitcher;
    _resetPythaPtsEv = resetPythaPtsEv;
    WaitToBePickedNotifier waitToBePicked = new WaitToBePickedNotifier(emptyImage, pickEv, sndSwitcher, this);
    ShrinkToBoxNotifier shrinkToBox = new ShrinkToBoxNotifier(imgShrinkToBox, waitToBePicked);
    _startAnim = new StartAnimationNotifier(evID, imgAnim, closeEv, startPythaEv, shrinkToBox, this);
  }

  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

    if (_myID == EncartaModel.SOLAR_ID) {
      MiniSolar._resetSolarEv.trigger();
    } else if (_myID == EncartaModel.MOVIE_ID) {
      EncartaMovie.resetState();
    } else if (_myID == EncartaModel.PYTHA_ID) {
      _resetPythaPtsEv.trigger();
    }

    _sndSwitcher.switchTo(_snd);
    return untilNotify(_bvr, timer(toBvr(2)), _startAnim);
  }

  public void cleanup() {
    _snd =null;
    _bvr =null;
    _startAnim.cleanup();
	_startAnim = null;
    _sndSwitcher =null;
    _resetPythaPtsEv =null;
  }

  boolean autonomous = true;
  SoundBvr _snd;
  int _myID;
  Behavior _bvr;
  StartAnimationNotifier _startAnim;
  ModifiableBehavior _sndSwitcher;
  AppTriggeredEvent _resetPythaPtsEv;
}

class StartAnimationNotifier extends Statics implements UntilNotifier {
  StartAnimationNotifier(int evID, Behavior bvr, DXMEvent closeEv,
                         AppTriggeredEvent startPythaEv, ShrinkToBoxNotifier shrinkToBox,
                         FlyForwardNotifier flyForward) {
    _myID = evID;
    _bvr = bvr;
    _autoCloseEv = timer(toBvr(2.5));
    _closeEv = closeEv;
    _shrinkToBox = shrinkToBox;
    _flyForward = flyForward;
    _startPythaEv = startPythaEv;
  }

  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

    //System.out.println("autonomous = "+_flyForward.autonomous);
    if (_flyForward.autonomous && (_myID == EncartaModel.PYTHA_ID)) {
      _startPythaEv.trigger();
    }

    DXMEvent autoCloseUntilPickedEv = (DXMEvent)until(_autoCloseEv, leftButtonDown,
                                                      _closeEv);
    DXMEvent ev = _flyForward.autonomous ? orEvent(autoCloseUntilPickedEv, _closeEv)
                                         : _closeEv;

    _flyForward.autonomous = false;
    return untilNotify(_bvr, ev, _shrinkToBox);
  }

  public void cleanup() {
    _flyForward.cleanup();
	_flyForward = null;
    _bvr = null;
    _shrinkToBox.cleanup();
	_shrinkToBox = null;
    _autoCloseEv = null; 
	_closeEv = null; 
    _startPythaEv = null;
  }


  FlyForwardNotifier _flyForward;
  int _myID;
  Behavior _bvr;
  ShrinkToBoxNotifier _shrinkToBox;
  DXMEvent _autoCloseEv, _closeEv;
  AppTriggeredEvent _startPythaEv;
}

class ShrinkToBoxNotifier extends Statics implements UntilNotifier {
  ShrinkToBoxNotifier(Behavior bvr, WaitToBePickedNotifier waitToBePicked) {
    _bvr = bvr;
    _waitToBePicked = waitToBePicked;
  }

  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

    return untilNotify(_bvr, timer(toBvr(1.25)), _waitToBePicked);
  }

  public void cleanup() {
    _bvr = null;
    _waitToBePicked.cleanup();
    _waitToBePicked = null;
  }

  Behavior _bvr;
  WaitToBePickedNotifier _waitToBePicked;
}

class WaitToBePickedNotifier extends Statics implements UntilNotifier {
  WaitToBePickedNotifier(Behavior bvr, DXMEvent pickEv, ModifiableBehavior sndSwitcher,
                         FlyForwardNotifier flyForward) {
    _bvr = bvr;
    _pickEv = pickEv;
    _flyForward = flyForward;
    _sndSwitcher = sndSwitcher;
  }

  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

    _sndSwitcher.switchTo(silence);
    return untilNotify(_bvr, _pickEv, _flyForward);
  }

  public void cleanup() {
    _bvr = null;
    _pickEv = null;
    _flyForward.cleanup();
    _flyForward = null;
    _sndSwitcher = null;
  }

  Behavior _bvr;
  DXMEvent _pickEv;
  FlyForwardNotifier _flyForward;
  ModifiableBehavior _sndSwitcher;
}
