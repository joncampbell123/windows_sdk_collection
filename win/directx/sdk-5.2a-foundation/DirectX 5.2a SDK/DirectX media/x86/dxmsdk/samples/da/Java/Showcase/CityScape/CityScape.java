// CityScape.java - Seattle CityScape
// DirectAnimation Java Demo 
//
// Copyright (c) 1997, Microsoft Corporation


import com.ms.dxmedia.*;        // gets DirectAnimation environment
import java.net.*;              // gets URL support
import java.applet.*;           // gets getAppletContext
import java_utility.*;          // gets the Utility Functions
import cityscape_module.*;      // gets carHornSynth


class CityScapeModel extends Model  {
  public void createModel(BvrsToRun bvt)  {
    mediaBase = getImportBase();
    imageBase = buildURL(mediaBase, "image/");
    geometryBase = buildURL(mediaBase, "geometry/");
    soundBase = buildURL(mediaBase, "sound/");

		_radioSnd = importSound(buildURL(soundBase,"radiotow.mp2"),null).loop().gain(0.7);
    _copterSnd = importSound(buildURL(soundBase,"copter2.mp2"),null).loop();

    //GET CITY BACKGROUND 
    ImageBvr city_bg = importImage(buildURL(imageBase,"daypal2w.gif"));
            
    //PUT CITY TOGETHER 
    TupleBvr cars = carImgAndSnd();
    TupleBvr girl_kite = kiteAndGirl();
    TupleBvr fish = fish();
    TupleBvr radio = radio();
    TupleBvr train = trainOn();
    TupleBvr city_links = city();   

    ImageBvr i1 = solidColorImage(colorHsl(add(mul(sin(mul(localTime,toBvr(0.4))),
      toBvr(0.3)),toBvr(0.4)),toBvr(0.8),toBvr(0.5)));
    ImageBvr i2 = i1.crop(point2(mul(toBvr(0),pixelBvr),mul(toBvr(-19),pixelBvr)),
      point2(mul(toBvr(43),pixelBvr),mul(toBvr(0),pixelBvr)));

    Transform2Bvr comp = compose(xfPixel(
			point2(mul(toBvr(411),pixelBvr),mul(toBvr(361),pixelBvr))
        ,vector2(mul(toBvr(84),pixelBvr),mul(toBvr(90),pixelBvr))),
			    translate(mul(toBvr(-23),pixelBvr),mul(toBvr(30),pixelBvr)));

    ImageBvr front4ClrCycle = i2.transform(comp);
            
    ImageBvr bridge = importImage(buildURL(imageBase,"dbridge.gif"))
      .transform(xfPixel(point2(toBvr(0),mul(toBvr(264),pixelBvr))
        ,vector2(mul(toBvr(640),pixelBvr),mul(toBvr(93),pixelBvr))));

    ImageBvr back = importImage(buildURL(imageBase,"dback.gif"))
      .transform(xfPixel(point2(mul(toBvr(192),pixelBvr),mul(toBvr(46),pixelBvr))
        ,vector2(mul(toBvr(440),pixelBvr),mul(toBvr(306),pixelBvr))));

    ImageBvr middle = importImage(buildURL(imageBase,"dmiddle.gif"))
      .transform(translate(mul(toBvr((424-320)),pixelBvr),
			  mul(toBvr((240-328)),pixelBvr)));

    ImageBvr front = importImage(buildURL(imageBase,"dfront.gif"))
      .transform(translate(mul(toBvr((372-320)),pixelBvr),
			  mul(toBvr((240-415)),pixelBvr)));
            
    TupleBvr ski = makePicker(point2(mul(toBvr(84),pixelBvr),
			mul(toBvr(175),pixelBvr)),vector2(mul(toBvr(58),pixelBvr),
			  mul(toBvr(40),pixelBvr)),_posAbove,toBvr("Ski Report"),
				  toBvr("http://www.skireport.com"));

    ImageBvr city = overlay((ImageBvr)city_links.nth(0)
      ,overlay((ImageBvr)girl_kite.nth(0),overlay(flags()
        ,overlay((ImageBvr)cars.nth(0),overlay((ImageBvr)fish.nth(0)
          ,overlay(front,overlay(front4ClrCycle,overlay(middle
            ,overlay(back,overlay(bridge,(ImageBvr)ski.nth(0)))))))))));

    // PUT COPTER TOGETHER
    TupleBvr copterOff = (TupleBvr)tripleBvr(city,silence,toBvr(0)).runOnce();

    DXMEvent copterEv1 = predicate(gt(localTime,
      add(toBvr(30),mul(toBvr(Math.random()),toBvr(10)))));

    MontageBvr montage_1  = imageMontage((ImageBvr)copterOff.nth(0)
      ,(NumberBvr)copterOff.nth(2));
    MontageBvr montage_2 = imageMontage((ImageBvr)copterOn().nth(0)
      ,(NumberBvr)copterOn().nth(2));

    MontageBvr copterMontage_1 = MontageBvr.newUninitBvr();
    copterMontage_1.init(until(montage_1,orEvent(copterEv1,callCopterEv)
      ,until(montage_2,timer(38),
			  copterMontage_1)));

    MontageBvr copterMontage = union(copterMontage_1,montage_1);

    SoundBvr copterSnd = SoundBvr.newUninitBvr();
      copterSnd.init(until((SoundBvr)copterOff.nth(1)
	      ,orEvent(copterEv1,callCopterEv),until((SoundBvr)copterOn().nth(1)
	        ,timer(toBvr(38)),copterSnd)));

    //PUT ALL IMAGES TOGETHER
    setImage(overlay((ImageBvr)radio.nth(0)
	  ,overlay(copterMontage.render(),overlay((ImageBvr)train.nth(0)
	    ,overlay(clouds(),city_bg)))));

    //PUT ALL SOUNDS TOGETHER
    setSound(mix((SoundBvr)city_links.nth(1)
      ,mix((SoundBvr)radio.nth(1),mix((SoundBvr)girl_kite.nth(1)
        ,mix(copterSnd,mix((SoundBvr)cars.nth(1),mix((SoundBvr)fish.nth(1)
					,mix((SoundBvr)ski.nth(1)
						,mix((SoundBvr)train.nth(1),citySynth())))))))));
	}
	
	public void appLink(AppletContext appGet)  {
    getAppContext = appGet;
  }

  private Transform2Bvr xfPixel(Point2Bvr upperleft, Vector2Bvr size)  {
    Point2Bvr center = add(upperleft, size.div(2));
      return translate(sub(center.getX(),mul(toBvr(320),pixelBvr))
        ,sub(mul(toBvr(240),pixelBvr),center.getY()));
	}

  private Transform2Bvr xfPt(Point2Bvr pt, Vector2Bvr size)  {
    Vector2Bvr center = size.div(2);
    return translate(sub(pt.getX(),center.getX())
      ,sub(center.getY(),pt.getY()));
	}

  private ImageBvr crop2(NumberBvr x1, NumberBvr y1, NumberBvr x2,
    NumberBvr y2, ImageBvr img)  {
    BooleanBvr cond1 = gte(x1,x2);
    BooleanBvr cond2 = gte(y1,y2);
    BooleanBvr cond = or(cond1,cond2);
    return (ImageBvr)cond(cond, emptyImage, 
      img.crop(point2(x1,y1), point2(x2,y2)));
	}

  private Transform3Bvr cameraXf()  {
    return compose(lookAtFrom(origin3,
      point3(0,50,120),yVector3),scale(12,12,1));
  }

  private TupleBvr fish()  {
    DXMEvent evNext = timer(toBvr(0.1));

    SoundBvr bucket = importSound(buildURL(soundBase,"fishbuck.mp2"),null);

    Transform2Bvr xf = xfPixel(point2(mul(toBvr(61),pixelBvr),
			mul(toBvr(362),pixelBvr)),vector2(mul(toBvr(88),pixelBvr),
			  mul(toBvr(92),pixelBvr)));

    PickableImage fish1_1 = new PickableImage(importImage(buildURL(imageBase,"fish1.gif")).transform(xf));
    TupleBvr fish1 = pairBvr(fish1_1.getImageBvr(),silence);
    DXMEvent evPick1 = andEvent(leftButtonDown,fish1_1.getPickEvent());
        
    TupleBvr fish2 = pairBvr(importImage(buildURL(imageBase,"fish2.gif")).transform(xf),silence);
    TupleBvr fish3 = pairBvr(importImage(buildURL(imageBase,"fish3.gif")).transform(xf),silence);
    TupleBvr fish4 = pairBvr(importImage(buildURL(imageBase,"fish4.gif")).transform(xf),silence);
    TupleBvr fish5 = pairBvr(importImage(buildURL(imageBase,"fish5.gif")).transform(xf),silence);
    TupleBvr fish6 = pairBvr(importImage(buildURL(imageBase,"fish6.gif")).transform(xf),silence);
    TupleBvr fish7 = pairBvr(importImage(buildURL(imageBase,"fish7.gif")).transform(xf),silence);
    TupleBvr fish8 = pairBvr(importImage(buildURL(imageBase,"fish8.gif")).transform(xf),silence);
    TupleBvr fish9 = pairBvr(importImage(buildURL(imageBase,"fish9.gif")).transform(xf),silence);
                
    TupleBvr fish10 = pairBvr(importImage(buildURL(imageBase,"fish10.gif")).transform(xf),bucket);

    TupleBvr gotFish = TupleBvr.newUninitBvr(pairBvr(emptyImage,silence));
      gotFish.init(until(fish2,evNext,until(fish3,evNext,until(fish4,evNext,
        until(fish5,evNext,until(fish6,evNext,until(fish7,evNext,
          until(fish8,evNext,until(fish9,evNext,fish10)))))))));

    TupleBvr fishClick = TupleBvr.newUninitBvr(pairBvr(emptyImage,silence));
      fishClick.init(until(fish1,evPick1,until(gotFish,timer(toBvr(1.66)),fishClick)));
    
    return fishClick;
	}

  private ImageBvr clouds()  {
    ImageBvr cloudsF = importImage(buildURL(imageBase,"cloudsf.gif"))
       .transform(translate(mul(toBvr((321-320)),pixelBvr), 
          mul(toBvr((240-82)),pixelBvr)));

    ImageBvr cloudsN = importImage(buildURL(imageBase,"cloudsn.gif"))
			.opacity(add(abs(sin(mul(localTime,toBvr(0.05)))),toBvr(0.2)))
        .transform(translate(mul(toBvr((321-320)),pixelBvr),
          mul(toBvr((240-82)),pixelBvr)));

    Bbox2Bvr cloudsNBB = cloudsN.boundingBox();

    NumberBvr cloudPosNear = mod(mul(localTime,toBvr(0.0004)),
      sub(cloudsNBB.getMax().getX(),cloudsNBB.getMin().getX()));

    NumberBvr cloudPosFar = mod(mul(localTime,toBvr(0.0001)),
      sub(cloudsNBB.getMax().getX(),cloudsNBB.getMin().getX()));

    ImageBvr cloud_1 = crop2(add(cloudsNBB.getMin().getX(),cloudPosNear),
      cloudsNBB.getMin().getY(),cloudsNBB.getMax().getX(),
        cloudsNBB.getMax().getY(),cloudsN)
				  .transform(translate(neg(cloudPosNear),toBvr(0)));

    ImageBvr cloud_2 = crop2(cloudsNBB.getMin().getX(),
      cloudsNBB.getMin().getY(),add(cloudsNBB.getMin().getX(),cloudPosNear),
          cloudsNBB.getMax().getY(),cloudsN)
            .transform(translate(sub(sub(cloudsNBB.getMax().getX(),
              cloudsNBB.getMin().getX()),cloudPosNear),toBvr(0)));

    ImageBvr cloud_3 = crop2(add(cloudsNBB.getMin().getX(),cloudPosFar),
      cloudsNBB.getMin().getY(),cloudsNBB.getMax().getX(),
        cloudsNBB.getMax().getY(),cloudsF)
          .transform(translate(neg(cloudPosFar),toBvr(0)));

    ImageBvr cloud_4 = crop2(cloudsNBB.getMin().getX(),
      cloudsNBB.getMin().getY(),add(cloudsNBB.getMin().getX(),cloudPosFar),
        cloudsNBB.getMax().getY(),cloudsF)
				  .transform(translate(sub(sub(cloudsNBB.getMax().getX(),
            cloudsNBB.getMin().getX()),cloudPosFar),toBvr(0)));

    return overlay(cloud_4,overlay(cloud_3,overlay(cloud_2,cloud_1)));
	}

  private SoundBvr citySynth()  {
    SoundBvr citydrone = importSound(buildURL(soundBase,"citydron.mp2"),null).loop();
    SoundBvr citydrone1 = citydrone.gain(0.5).rate(0.7);
		SoundBvr citydrone2 = citydrone.gain(0.5).rate(0.9);
		
    carHornSynth carhorn = new carHornSynth();

    return mix(carhorn.getSound(soundBase),mix(citydrone1,citydrone2));
	}

  private ImageBvr flags()  {
    ImageBvr flag1 = importImage(buildURL(imageBase,"dflag1.gif"));
    ImageBvr flag2 = importImage(buildURL(imageBase,"dflag2.gif"));

    DXMEvent evNext = timer(toBvr(0.5));

    ImageBvr flag = ImageBvr.newUninitBvr();
    flag.init(until(flag1,evNext,until(flag2,evNext,flag)));
    Vector2Bvr vectorFlag = vector2(mul(toBvr(20),pixelBvr),
      mul(toBvr(8),pixelBvr));

    ImageBvr flag_1 = flag.transform(xfPixel(
			point2(mul(toBvr(530),pixelBvr),mul(toBvr(288),pixelBvr)),vectorFlag));
    ImageBvr flag_2 = flag.transform(xfPixel(
      point2(mul(toBvr(557),pixelBvr),mul(toBvr(281),pixelBvr)),vectorFlag));
    ImageBvr flag_3 = flag.transform(xfPixel(
			point2(mul(toBvr(582),pixelBvr),mul(toBvr(279),pixelBvr)),vectorFlag));
    ImageBvr flag_4 = flag.transform(xfPixel(
			point2(mul(toBvr(608),pixelBvr),mul(toBvr(280),pixelBvr)),vectorFlag));
    ImageBvr flag_5 = flag.transform(xfPixel(
			point2(mul(toBvr(545),pixelBvr),mul(toBvr(316),pixelBvr)),vectorFlag));
    ImageBvr flag_6 = flag.transform(xfPixel(
			point2(mul(toBvr(578),pixelBvr),mul(toBvr(324),pixelBvr)),vectorFlag));
    ImageBvr flag_7 = flag.transform(xfPixel(
			point2(mul(toBvr(611),pixelBvr),mul(toBvr(329),pixelBvr)),vectorFlag));

    return overlay(flag_1,overlay(flag_2,overlay(flag_3,
      overlay(flag_4,overlay(flag_5,overlay(flag_6,flag_7))))));
	}

  private TupleBvr carImgAndSnd()  {
    PickableImage evPick2 = new PickableImage(importImage(buildURL(imageBase,"car2.gif")));
    DXMEvent ev2 = andEvent(leftButtonDown, evPick2.getPickEvent());
                
    Transform2Bvr xlate2 = xfPixel(
      point2(mul(toBvr(465),pixelBvr),mul(toBvr(395),pixelBvr)), 
        vector2(mul(toBvr(62),pixelBvr),mul(toBvr(67),pixelBvr)));
                
    NumberBvr s1 = add(toBvr(1),mul(abs(sin(mul(floor(mul(globalTime,toBvr(12))),
      toBvr(Math.PI/2)))),toBvr(0.01)));
                
    Transform2Bvr scaleOff2 = Transform2Bvr.newUninitBvr();
      scaleOff2.init(until(identityTransform2,ev2,until(scale2(s1),ev2,scaleOff2)));

    ImageBvr redcar = evPick2.getImageBvr().transform(xlate2).transform(scaleOff2);

    PickableImage evPick3 = new PickableImage(importImage(buildURL(imageBase,"car3.gif")));
    DXMEvent ev3 = andEvent(leftButtonDown, evPick3.getPickEvent());

    Transform2Bvr xlate3 = xfPixel(
      point2(mul(toBvr(315),pixelBvr),mul(toBvr(432),pixelBvr)), 
        vector2(mul(toBvr(93),pixelBvr),mul(toBvr(42),pixelBvr)));

    Transform2Bvr scaleOff3 = Transform2Bvr.newUninitBvr();
      scaleOff3.init(until(identityTransform2,ev3,until(scale2(s1),
        ev3,scaleOff3)));
                
    ImageBvr bluecar = evPick3.getImageBvr().transform(xlate3)
      .transform(scaleOff3);
  
    SoundBvr alarm = importSound(buildURL(soundBase,"caralarm.mp2"),null).loop();
    SoundBvr redAlarm = alarm.gain(0.9);      
    SoundBvr blueAlarm = alarm.rate(0.5);     
    SoundBvr snd2Off = SoundBvr.newUninitBvr();
    snd2Off.init(until(silence,(DXMEvent)ev2,until(redAlarm,ev2,snd2Off)));
    SoundBvr snd3Off = SoundBvr.newUninitBvr();
    snd3Off.init(until(silence,ev3,until(blueAlarm,ev3,snd3Off)));

    SoundBvr carSound = mix(snd2Off,snd3Off);

    return pairBvr(overlay(redcar,bluecar),carSound);       
	}

  private TupleBvr radio()  {
    TupleBvr pickableImg = makePicker(point2(mul(toBvr(254),pixelBvr),
      mul(toBvr(11),pixelBvr)),vector2(mul(toBvr(72),pixelBvr),
        mul(toBvr(66),pixelBvr)),_posSide,toBvr("PBS Online"), 
          toBvr("http://www.pbs.org"));

    Transform2Bvr xlate = translate(mul(toBvr((289-320)),pixelBvr),
			mul(toBvr((240-54)),pixelBvr));
    Transform2Bvr xf = scale2(add(mul(add(cos(add(toBvr(Math.PI),
      mul(localTime,toBvr(3)))),toBvr(1)),toBvr(0.2)),toBvr(0.5)));
          
    return pairBvr(overlay((ImageBvr)pickableImg.nth(0),
      importImage(buildURL(imageBase,"radio3b.gif"))
        .transform(compose(xlate,xf))),(SoundBvr)pickableImg.nth(1));
	}
        
  private TupleBvr copterOn()  {
    Point3Bvr[] points =  {
			point3(300,50,-30),
			point3(30,50,100),
			point3(70,50,-50),
			point3(0,50,-150),
			point3(-130,50,-100),
			point3(-80,-10,30),
			point3(100,-10,50),
			point3(60,50,-80),
			point3(200,50,-100),
			point3(300,50,-150),
			point3(300,50,-30)
		};
		
		NumberBvr[] knots =  {
      toBvr(0), toBvr(0), toBvr(0), toBvr(1), toBvr(2), 
      toBvr(3), toBvr(4), toBvr(5), toBvr(6), toBvr(7),
      toBvr(8), toBvr(8), toBvr(8)
		};

    Point3Bvr spline = bSpline(3,knots,points,null,localTime);

    NumberBvr current = div(localTime,toBvr(5));

    Point3Bvr position = (Point3Bvr)spline.substituteTime(current);

    Vector3Bvr direction = derivative(position);

    Transform3Bvr comp = lookAtFrom(add(position,direction),position,yVector3);

    GeometryBvr movingCopter = centeredCopter().transform(comp);

    Transform3Bvr temp1 = rotate(yVector3,toBvr(-Math.PI/2));

    Transform3Bvr temp2 = rotate(yVector3,toBvr(Math.PI/6));

    GeometryBvr temp3 = directionalLight
			.lightColor(colorRgb(0.7,0.7,0.7))
			.transform(temp2);

    GeometryBvr temp4 = directionalLight
      .transform(temp1);

    GeometryBvr temp5 = union(temp4,temp3);

    GeometryBvr img_2 = union(movingCopter,temp5);

    ImageBvr img = img_2
      .render(perspectiveCamera(toBvr(1),toBvr(0))
      .transform(cameraXf()));

    Bbox3Bvr movcopBB = movingCopter.boundingBox();

    NumberBvr zPos = movcopBB.getMax().getZ();
    Point3Bvr posViewer = point3(-10,50,120);
    NumberBvr dist = distance(position, posViewer); 

    return tripleBvr(img,copterSynth(dist, direction, position),zPos);
	}

  private GeometryBvr centeredCopter()  {
    GeometryBvr copterb = importGeometry(buildURL(geometryBase,"copter8b.x"));
      copterb = copterb.transform(rotate(yVector3, toBvr(Math.PI)));
    GeometryBvr copterp = importGeometry(buildURL(geometryBase,"copter8p.x"));

    Bbox3Bvr copBB = copterp.boundingBox();

    Vector3Bvr halfExtent = sub(copBB.getMax(),copBB.getMin()).mul(0.5);
    Point3Bvr center = add(copBB.getMin(),halfExtent);

    Transform3Bvr comp = compose(translate(sub(center,origin3))
      ,compose(rotate(yVector3,localTime),translate(sub(origin3,center))));

    GeometryBvr copter_1 = copterp.transform(comp);

    GeometryBvr copter_2 = copterb
			.transform(rotate(yVector3,toBvr(Math.PI)));

    return union(copter_1,copter_2);
	}

  private SoundBvr copterSynth(NumberBvr dist, Vector3Bvr direction, 
		Point3Bvr position)  {
    NumberBvr zMax = toBvr(270);
    NumberBvr zMin = toBvr(130);
    NumberBvr zCenter = mul(add(zMax,zMin),toBvr(0.5));
    NumberBvr zExtent = sub(zMax,zMin);
              
    NumberBvr gn = add(toBvr(1.15),mul(div(sub(zCenter,dist),zExtent),toBvr(0.2)));
    NumberBvr rt = add(toBvr(1),mul(div(direction.getY(),toBvr(35)),toBvr(0.2)));
    NumberBvr pn = div(position.getX(),toBvr(700));
  
    return _copterSnd.loop().rate(rt).gain(gn).pan(pn);
	}

  private TupleBvr  centeredKite()  {
    GeometryBvr kite_1 = importGeometry(buildURL(geometryBase,"kite.x"));
    kite_1 = kite_1.transform(rotate(yVector3, toBvr(Math.PI)));

    Bbox3Bvr kiteBB = kite_1.boundingBox();

    Vector3Bvr halfExtent = sub(kiteBB.getMax(),kiteBB.getMin()).mul(0.5);
    Point3Bvr center = add(kiteBB.getMin(),halfExtent);

    GeometryBvr cube = importGeometry(buildURL(geometryBase,"cube.x"))
      .transform(translate(toBvr(0),toBvr(0),mul(neg(halfExtent.getZ()),toBvr(0.5))));

    Transform3Bvr kiteRotate = 
      compose(rotate(zVector3,toBvr(0.25)),compose(rotate(xVector3,toBvr(-1.45)),
        compose(rotate(yVector3,toBvr(3.10)),translate(sub(origin3,center)))));

    GeometryBvr kite = kite_1.transform(kiteRotate);

		return pairBvr(kite,cube);
	}

  public TupleBvr kiteAndGirl()  {
    Vector2Bvr szGirl = vector2(mul(toBvr(45),pixelBvr),
      mul(toBvr(34),pixelBvr));

    Transform2Bvr xfGirl = xfPixel(point2(mul(toBvr(168),pixelBvr)
      ,mul(toBvr(339),pixelBvr)),szGirl);
        
    Point2Bvr minPt = point2(neg(div(szGirl.getX(),toBvr(2))),
      neg(div(szGirl.getY(),toBvr(2))));

    Point2Bvr maxPt = point2(div(szGirl.getX(),toBvr(2)),
      div(szGirl.getY(),toBvr(2)));

    PickableImage pickImg = new PickableImage(
      detectableEmptyImage.crop(minPt,maxPt).transform(xfGirl));

    DXMEvent evGirlClicked = andEvent(pickImg.getPickEvent(),leftButtonDown);

    Vector3Bvr[] points1 =  {
      vector3(-30,0,0),
      vector3(-10,-20,20),
      vector3(10,20,-20),
      vector3(30,0,0),
      vector3(10,-20,20),
      vector3(-10,20,-20),
      vector3(-30,0,0)
		};

    NumberBvr[] knots1 =  {
      toBvr(0), toBvr(0), toBvr(0), toBvr(1), toBvr(2), 
      toBvr(2), toBvr(3), toBvr(3), toBvr(3)
		};

    _spline1 = bSpline(3,knots1,points1,null,localTime);

    NumberBvr current = mod(mul(localTime,toBvr(0.2)),toBvr(3));

    _spline1_current = (Vector3Bvr)_spline1.substituteTime(current);

    DXMEvent ev = evGirlClicked.snapshotEvent(current);

    _position_kite = (Vector3Bvr)untilNotify(_spline1_current,ev,
      new CityScapeNotifierCurr(this));
        
    Vector3Bvr direction = derivative(_position_kite);

    ImageBvr girl = (ImageBvr)kiteGirl(direction, xfGirl, szGirl, 
      evGirlClicked).nth(0);
        
    Point2Bvr pHand = (Point2Bvr)kiteGirl(direction, xfGirl, szGirl, 
      evGirlClicked).nth(1);
        
    GeometryBvr movingKite3D = movingKiteAndKnot(0, _position_kite, direction);
    GeometryBvr movingKiteKnot = movingKiteAndKnot(1,_position_kite, direction);
        
    GeometryBvr x1 = union(movingKite3D.lightColor(
      colorRgb(1,1,1)),ambientLight);
    GeometryBvr x2 = union(x1,directionalLight);

    ImageBvr kiteImage = x2.render(perspectiveCamera(toBvr(1),toBvr(0))
      .transform(cameraXf()));

    GeometryBvr y1 = union(movingKiteKnot,directionalLight);

    ImageBvr kiteKnotImage = y1.render(perspectiveCamera(toBvr(1),toBvr(0))
      .transform(cameraXf()));

    Bbox2Bvr movkiteBB = kiteKnotImage.boundingBox();

    Vector2Bvr halfExtent = sub(movkiteBB.getMax(),
      movkiteBB.getMin()).mul(0.5);
        
    Point2Bvr pKiteKnot = add(movkiteBB.getMin(),halfExtent);

    Path2Bvr pts = line(pKiteKnot, pHand);

    ImageBvr kiteString = pts.draw(defaultLineStyle);

    DXMEvent kiteStopped = predicate(lte(_position_kite.getX(),toBvr(-50)));

    SoundBvr kiteSound = importSound(buildURL(soundBase,"water.mp2"),null);

    SoundBvr snd = SoundBvr.newUninitBvr();
    snd.init(until(silence, kiteStopped,
      until(kiteSound,notEvent(kiteStopped),snd)));

    return pairBvr(overlay(pickImg.getImageBvr()
      ,overlay(kiteImage,overlay(kiteString,girl))),snd);
	}  

  private GeometryBvr movingKiteAndKnot(int n, Vector3Bvr position, 
    Vector3Bvr direction)  {
    GeometryBvr kiteorknot = (GeometryBvr)centeredKite().nth(n);
                
    Transform3Bvr xfStop = compose(translate(-50,0,20),
      translate(position));

    Vector3Bvr tweak = vector3(0.1,0.1,0.1);

    Transform3Bvr xfFlying = compose(translate(-50,0,20),
      lookAtFrom(add(add(add(origin3,direction),position),tweak),
        add(origin3,position),vector3(0,20,20)));

    Transform3Bvr xf = (Transform3Bvr)cond(gt(direction.getX(),toBvr(0)),
      xfStop,xfFlying);         

    return kiteorknot.transform(xf);
	}

  private TupleBvr kiteGirl(Vector3Bvr direction,Transform2Bvr xfGirl, 
    Vector2Bvr szGirl,DXMEvent evGirlClicked)  {
    ImageBvr kiteL1  = importImage(buildURL(imageBase,"kitel1b.gif"))
      .transform(xfGirl);

    Point2Bvr posHandL1 = origin2.transform(xfGirl)
      .transform(xfPt(point2(mul(toBvr(28),pixelBvr),
        mul(toBvr(4),pixelBvr)),szGirl));
      
    ImageBvr kiteR1  = importImage(buildURL(imageBase,"kiter1b.gif"))
      .transform(xfGirl);

    Point2Bvr posHandR1 = origin2.transform(xfGirl)
      .transform(xfPt(point2(mul(toBvr(38),pixelBvr),
        mul(toBvr(9),pixelBvr)),szGirl));

    ImageBvr kiteTalk = importImage(buildURL(imageBase,"kitetalk.gif"))
      .transform(xfGirl);

    Point2Bvr posHandTalk = origin2.transform(xfGirl)
      .transform(xfPt(point2(mul(toBvr(31),pixelBvr),
        mul(toBvr(22),pixelBvr)),szGirl));

    NumberBvr xDir = direction.getX();

    ImageBvr img = (ImageBvr)cond(gt(xDir,toBvr(0)),kiteR1,kiteL1);
    Point2Bvr pos= (Point2Bvr)cond(gt(xDir,toBvr(0)),posHandR1,posHandL1);

    TupleBvr img_pos = pairBvr(img,pos);

    TupleBvr kiteTalk_posHandTalk = pairBvr(kiteTalk,posHandTalk);

    TupleBvr girl_hand = TupleBvr.newUninitBvr(pairBvr(emptyImage,origin2));
    girl_hand.init(until(img_pos,evGirlClicked,until(kiteTalk_posHandTalk,
      timer(toBvr(10)),girl_hand)));

    return girl_hand;
	}
        
  private TupleBvr city()  {
    TupleBvr mid1 = makePicker(point2(mul(toBvr(208),pixelBvr),
      mul(toBvr(293),pixelBvr)),vector2(mul(toBvr(50),pixelBvr),
        mul(toBvr(60),pixelBvr)),_posAbove,toBvr("Hotels"),
          toBvr("http://www.all-hotels.com/usa/seattle.htm"));
                
    TupleBvr mid3 = makePicker(point2(mul(toBvr(362),pixelBvr),
			mul(toBvr(319),pixelBvr)),vector2(mul(toBvr(65),pixelBvr),
        mul(toBvr(48),pixelBvr)),_posAbove,toBvr("Movies"),
          toBvr("http://seattle.sidewalk.com/movies"));

    TupleBvr mid4 = makePicker(point2(mul(toBvr(432),pixelBvr),
      mul(toBvr(319),pixelBvr)),vector2(mul(toBvr(65),pixelBvr),
        mul(toBvr(48),pixelBvr)),_posAbove,toBvr("Restaurants"),
          toBvr("http://seattle.sidewalk.com/findrestaurant"));

    TupleBvr mid6 = makePicker(point2(mul(toBvr(432),pixelBvr),
      mul(toBvr(256),pixelBvr)),vector2(mul(toBvr(28),pixelBvr),
        mul(toBvr(57),pixelBvr)),_posAbove,toBvr("Amtrak"),
          toBvr("http://www.wsdot.wa.gov/pubtran/_rail_sched"));

    TupleBvr mid8 = makePicker(point2(mul(toBvr(548),pixelBvr),
      mul(toBvr(301),pixelBvr)),vector2(mul(toBvr(90),pixelBvr),
        mul(toBvr(97),pixelBvr)),_posAbove,toBvr("Sports"),
          toBvr("http://seattle.sidewalk.com/findsports"));

    TupleBvr front1 = makePicker(point2(mul(toBvr(204),pixelBvr),
      mul(toBvr(371),pixelBvr)),vector2(mul(toBvr(56),pixelBvr),
        mul(toBvr(46),pixelBvr)),_posAbove,toBvr("Ferry Guide"),
          toBvr("http://www.wsdot.wa.gov/ferries/current"));

    TupleBvr front3 = makePicker(point2(mul(toBvr(345),pixelBvr),
      mul(toBvr(373),pixelBvr)),vector2(mul(toBvr(58),pixelBvr),
        mul(toBvr(52),pixelBvr)),_posAbove,toBvr("Jazz & Blues"),
				  toBvr("http://seattle.sidewalk.com/clubs?FID=54&SID=1118&SID=1159&SID=1169"));

    TupleBvr front4 = makePicker(point2(mul(toBvr(418),pixelBvr),
      mul(toBvr(377),pixelBvr)),vector2(mul(toBvr(55),pixelBvr),
        mul(toBvr(58),pixelBvr)),_posAbove,toBvr("Night Clubs"),
          toBvr("http://seattle.sidewalk.com/findclub"));

    TupleBvr front5 = makePicker(point2(mul(toBvr(561),pixelBvr),
      mul(toBvr(407),pixelBvr)),vector2(mul(toBvr(74),pixelBvr),
        mul(toBvr(63),pixelBvr)),_posAbove,toBvr("Art Guide"),
          toBvr("http://www.artguidenw.com"));

    ImageBvr totalPicker = overlay((ImageBvr)front1.nth(0),
      overlay((ImageBvr)front3.nth(0),overlay((ImageBvr)front4.nth(0),
        overlay((ImageBvr)front5.nth(0),overlay((ImageBvr)mid1.nth(0),
          overlay((ImageBvr)mid3.nth(0),overlay((ImageBvr)mid4.nth(0),
            overlay((ImageBvr)mid6.nth(0),(ImageBvr)mid8.nth(0)))))))));

    SoundBvr totalPickerSnd = mix((SoundBvr)front1.nth(1),
      mix((SoundBvr)front3.nth(1),mix((SoundBvr)front4.nth(1),
        mix((SoundBvr)front5.nth(1),mix((SoundBvr)mid1.nth(1),
          mix((SoundBvr)mid3.nth(1),mix((SoundBvr)mid4.nth(1),
            mix((SoundBvr)mid6.nth(1),(SoundBvr)mid8.nth(1)))))))));

    return pairBvr(totalPicker,totalPickerSnd);
	}

  private TupleBvr makePicker(Point2Bvr posImg, Vector2Bvr szImg, 
    NumberBvr posTip,StringBvr str, StringBvr url)  {
    NumberBvr s1 = div(neg(szImg.getX()),toBvr(2));
    NumberBvr s2 = div(neg(szImg.getY()),toBvr(2));
    NumberBvr s3 = div(szImg.getX(),toBvr(2));
    NumberBvr s4 = div(szImg.getY(),toBvr(2));

    ImageBvr imgDetect = detectableEmptyImage.crop(point2(s1, s2),
      point2(s3, s4));

    Transform2Bvr xlate = xfPixel(posImg, szImg);

    PickableImage imgPick = new PickableImage(imgDetect.transform(xlate),true);

    DXMEvent evPick = imgPick.getPickEvent();

    DXMEvent ev = andEvent(leftButtonDown, evPick);

		FontStyleBvr font = defaultFont.bold().color(magenta);

		ImageBvr txt = stringImage(str, font);

    Bbox2Bvr txtBB = txt.boundingBox();

    NumberBvr a1 = div(add(sub(txtBB.getMax().getY(),txtBB.getMin().getY()),
      mul(szImg.getY(),pixelBvr)),toBvr(2));
    NumberBvr b1 = div(add(sub(txtBB.getMax().getX(),txtBB.getMin().getX()),
      mul(szImg.getX(),pixelBvr)),toBvr(2));

    Transform2Bvr x1 = translate(toBvr(0),a1);
    Transform2Bvr x2 = translate(b1,toBvr(0));

    Transform2Bvr xlate2 = (Transform2Bvr)cond(eq(posTip,_posAbove),x1,x2);

    ImageBvr temp = solidColorImage(colorRgb(1,1,135/255))
      .crop(txtBB.getMin(),txtBB.getMax());

    ImageBvr tipImg = overlay(txt,temp);

    TupleBvr temp1 = pairBvr(imgPick.getImageBvr(),silence);
    Transform2Bvr t1 = compose(xlate,xlate2);
    ImageBvr i1 = tipImg.transform(t1);
    ImageBvr i2 = overlay(i1,imgPick.getImageBvr());
    TupleBvr temp2 = pairBvr(i2,_radioSnd);

    TupleBvr picked_1 = TupleBvr.newUninitBvr(pairBvr(emptyImage,silence));
    picked_1.init(until(temp1,evPick,
      until(temp2,notEvent(evPick),picked_1)));
                
    picked = (TupleBvr)untilNotify(picked_1,
      ev,new CityScapeNotifierLink(url,this));

    return picked;
	}

  private TupleBvr trainOn()  {
    Vector2Bvr[] points =  {             
      vector2(mul(toBvr((-200-320)),pixelBvr),mul(toBvr((240-370)),pixelBvr)), 
      vector2(mul(toBvr((140-320)),pixelBvr),mul(toBvr((240-277)),pixelBvr)),
      vector2(mul(toBvr((433-320)),pixelBvr),mul(toBvr((240-274)),pixelBvr)), 
      vector2(mul(toBvr((800-320)),pixelBvr),mul(toBvr((240-320)),pixelBvr)), 
		};

    NumberBvr[] knots =  {             
      toBvr(0),toBvr(0),toBvr(0.5),toBvr(1),toBvr(1)
		};

    Vector2Bvr spline = bSpline(2,knots,points,null,localTime);
    Vector2Bvr position = (Vector2Bvr)spline
      .substituteTime(mod(mul(localTime,toBvr(0.02)),toBvr(1)));

    Vector2Bvr direction = derivative(position);
                                                                                                                                                                                                
    Transform2Bvr comp = compose(translate(position),
      rotate(atan2(direction.getY(),direction.getX())));

    ImageBvr movingTrain = importImage(buildURL(imageBase,"train16s.gif"))
      .transform(comp);

    NumberBvr xPos = div(position.getX(), pixelBvr);

    NumberBvr gn_1 = add(div(mul(add(xPos,toBvr(500)),toBvr(0.45)),toBvr(160)),toBvr(0.55));
    NumberBvr gn_2 = add(div(mul(sub(toBvr(-30),xPos),toBvr(0.25)),toBvr(130)),toBvr(0.75));
    NumberBvr gn_3 = add(mul(toBvr(Math.random()),toBvr(0.03)),toBvr(0.75));
    NumberBvr gn_4 = add(div(mul(sub(toBvr(350),xPos),toBvr(0.2)),toBvr(130)),toBvr(0.55));

    NumberBvr gn = (NumberBvr)until(gn_1,predicate(gt(xPos,toBvr(-160))),
      until(gn_2,predicate(gt(xPos,toBvr(-30))),
        until(gn_3,predicate(gt(xPos,toBvr(220))),gn_4)));

    NumberBvr pn = div(xPos,toBvr(3200));

    SoundBvr trainSnd = importSound(buildURL(soundBase,"train.mp2"),null)
      .loop().rate(toBvr(0.5)).gain(gn).pan(pn);

    SoundBvr trainWhistle = importSound(buildURL(soundBase,"trainwhi.mp2"),null);
    SoundBvr whistleTwice = (SoundBvr)until(trainWhistle.pan(-1),
      timer(toBvr(0.9)),trainWhistle.pan(-1));

    TupleBvr trainOn = pairBvr(movingTrain, trainSnd);
    TupleBvr trainOff = pairBvr(emptyImage,silence);
    TupleBvr trainComing = pairBvr(emptyImage,whistleTwice);
		
    DXMEvent evNext = predicate(gt(localTime,
      add(toBvr(60),mul(toBvr(Math.random()),toBvr(8)))));
		
    TupleBvr train = TupleBvr.newUninitBvr(pairBvr(emptyImage,silence));

    train.init(until(trainOff,orEvent(evNext,callTrainEv),
      until(trainComing, timer(toBvr(4)),until(trainComing, timer(toBvr(4)),
        until(trainComing, timer(toBvr(4)),
				  until(trainOn, predicate(gt(xPos,toBvr(415))),train))))));

    return train;
	}

  public Vector3Bvr spline2(NumberBvr snapshot)  {
    Vector3Bvr pt1 = (Vector3Bvr)_spline1
      .substituteTime(mod(add(snapshot,toBvr(0.1)),toBvr(3)));
    Vector3Bvr pt2 = (Vector3Bvr)_spline1
      .substituteTime(mod(add(snapshot,toBvr(0.5)),toBvr(3)));
    Vector3Bvr pt3 = (Vector3Bvr)_spline1
      .substituteTime(mod(add(snapshot,toBvr(0.8)),toBvr(3)));
                
    Vector3Bvr[] points2 =  {
      pt1,pt2,pt3, 
      vector3(-40,-30,0),
      vector3(-50,-50,0)
		};
                
    NumberBvr[] knots2=  {
      toBvr(0), toBvr(0), toBvr(0), toBvr(1), 
      toBvr(2), toBvr(2), toBvr(2)
		};

    return bSpline(3,knots2,points2,null,localTime);
	}

  public static NumberBvr cap(NumberBvr max, NumberBvr num)  {
    return (NumberBvr)cond(gt(num,max),max,num);
  }

  public void cleanup() {
    super.cleanup();
    _position_kite=null;
	picked=null;
  }

  Vector3Bvr _position_kite;
  public Vector3Bvr _spline1;
  public Vector3Bvr _spline1_current;

  public static AppletContext getAppContext;

  public  URL mediaBase;
  public  URL imageBase;
  public  URL geometryBase;
  public  URL soundBase;

  NumberBvr _posAbove = toBvr(1);
  NumberBvr _posSide = toBvr(2);
  SoundBvr _radioSnd;
  SoundBvr _copterSnd;
  DXMEvent callTrainEv = keyDown(116);
  DXMEvent callCopterEv = keyDown(104);
  TupleBvr picked;
}

//The CityScapeNotifierLink Class is used to link to the various web sites.
//currentRunningBvr is returned to keep the notifier happy.
class CityScapeNotifierLink extends Statics implements UntilNotifier  {
  public CityScapeNotifierLink(StringBvr url,CityScapeModel model)  {

		try  {
      _clickDest = new URL((String) url.extract());
			_model = model;
		}
		catch(MalformedURLException mal)  {
      System.out.println("Malformed URL: Check Applet Tag.");
		}
	}
	
  public Behavior notify(Object eventData,Behavior currentRunningBvr, 
		BvrsToRun btr)  {       
    _model.getAppContext.showDocument(_clickDest);
    return currentRunningBvr;
	}

  URL _clickDest;
  CityScapeModel _model;
}

class CityScapeNotifierCurr extends Statics implements UntilNotifier  {
  public CityScapeNotifierCurr(CityScapeModel model)  {
		_model = model; 
	}
	
	public Behavior notify(Object eventData,Behavior currentRunningBvr,
		BvrsToRun btr)  {       
    NumberBvr temp1 = _model.cap(toBvr(2),mul(localTime,toBvr(1)));

		return (Vector3Bvr)until(_model.spline2((NumberBvr)eventData)
     .substituteTime(temp1),timer(toBvr(10)),_model._position_kite);
	}
  CityScapeModel _model;
}

public class CityScape extends DXMApplet  {
  public CityScape()  {
	}

  public void init()  {
    super.init();
    CityScapeModel newCity = new CityScapeModel();
    setModel(newCity);
    newCity.appLink(getAppletContext());
  }
}
