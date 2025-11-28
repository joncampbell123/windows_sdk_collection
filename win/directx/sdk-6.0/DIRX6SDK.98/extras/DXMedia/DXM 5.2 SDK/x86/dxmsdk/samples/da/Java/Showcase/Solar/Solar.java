// Solar.java - The solar system in 3D
// DirectX Media Java Demo 
//
// Copyright (c) 1997, Microsoft Corporation
import com.ms.dxmedia.*;
import java.net.*;

class SolarModel extends Model implements UntilNotifier
{
    // create the camera
    NumberBvr               time   = (NumberBvr)localTime.runOnce();
    Point3Bvr               planetPosition[]; 
    Transform3Bvr   planetTransforms[];
    DXMEvent                changePlanetEvent;


    SoundBvr mySoundImporter(String file, boolean loop) {        
        SoundBvr snd = importSound(buildURL(sndBase, file),
                                   null);
        
        if (loop) {
            snd = snd.loop();
        }
        return snd;
    }
    
    ImageBvr myImageImporter(String file) {        
        return importImage(buildURL(imgBase, file));
    }
    
  public void createModel(BvrsToRun blst) {
    CameraBvr camera = perspectiveCamera(toBvr(1),toBvr(0)); 
    mediaBase = getImportBase();
    sndBase = buildURL(mediaBase,"sound/");
    imgBase = buildURL(mediaBase,"image/");
    geoBase = buildURL(mediaBase,"geometry/");



        int index;      // counter
        planetPosition   = new Point3Bvr[6];
        planetTransforms = new Transform3Bvr[6];

        // import the images
        ImageBvr inup           = myImageImporter("inup.gif");
        ImageBvr outup          = myImageImporter("outup.gif");
        ImageBvr zoomup         = myImageImporter("zoomup.gif");
        ImageBvr zoomdown       = myImageImporter("zoomdown.gif");
        ImageBvr rainup         = myImageImporter("rainup.gif");
        ImageBvr raindown       = myImageImporter("raindown.gif");
        ImageBvr sunjpg         = myImageImporter("sun.jpg");
        ImageBvr mercuryjpg     = myImageImporter("mercury.jpg");
        ImageBvr venusjpg       = myImageImporter("venus.jpg");
        ImageBvr moonjpg        = myImageImporter("moon.jpg");
        ImageBvr marsjpg        = myImageImporter("mars.jpg");
        ImageBvr rainyPic       = myImageImporter("rain.jpg");
        ImageBvr sunnyPic       = myImageImporter("earth.jpg");
        ImageBvr stars          = myImageImporter("stars.jpg");
                                

        // import the sounds
        SoundBvr click          = mySoundImporter("butin.wav",false);
        SoundBvr sunwav         = mySoundImporter("sun.mp2",true);
        SoundBvr mercurywav     = mySoundImporter("mercury.mp2",true);
        SoundBvr venuswav       = mySoundImporter("venus.mp2",true);
        SoundBvr moonwav        = mySoundImporter("moon.mp2",true);
        SoundBvr marswav        = mySoundImporter("mars.mp2",true);
        SoundBvr torain         = mySoundImporter("torain.mp2",false);
        SoundBvr toearth        = mySoundImporter("toearth.mp2",false);
        SoundBvr zoomin         = mySoundImporter("zoomin.mp2",false);
        SoundBvr zoomout        = mySoundImporter("zoomout.mp2",false);
        SoundBvr rainySound     = mySoundImporter("rain.mp2",true);
        SoundBvr sunnySound     = mySoundImporter("earth.mp2",true);


        // change the sound & texture based on the rainEvent         
        PickableImage raindownPick = new PickableImage(raindown);
        PickableImage rainupPick   = new PickableImage(rainup);
        DXMEvent earthEvent = orEvent(andEvent(raindownPick.getPickEvent(),leftButtonDown),
                                      andEvent(rainupPick.getPickEvent(),leftButtonDown));

                SoundBvr earthSoundBvrs[]     = {sunnySound,rainySound};
        Cycler earthSoundcyl          = new Cycler(earthSoundBvrs, earthEvent);
        SoundBvr earthSound           = (SoundBvr)earthSoundcyl.getBvr();                                      

        ImageBvr earthBvrs[]  = {raindownPick.getImageBvr(),rainupPick.getImageBvr()};
        Cycler earthStatuscyl             = new Cycler(earthBvrs,earthEvent);
        PickableImage earthStatus  = new PickableImage((ImageBvr)earthStatuscyl.getBvr());

        ImageBvr earthImageBvrs[]     = {sunnyPic,rainyPic};
        Cycler earthImagecyl          = new Cycler(earthImageBvrs, earthEvent);
        ImageBvr earthTexture         = (ImageBvr)earthImagecyl.getBvr();

        // Describe the planets and fill in the data associated with it.
        StringBvr planetNames[] = {toBvr("Sun"),
                                   toBvr("Mercury"),
                                   toBvr("Venus"),
                                   toBvr("Earth"),
                                   toBvr("Moon"),
                                   toBvr("Mars")};
        
        ArrayBvr displayText = new ArrayBvr(planetNames);                       
        ImageBvr pstatus[]        = {emptyImage,emptyImage,emptyImage,earthStatus.getImageBvr(),emptyImage,emptyImage};
        ArrayBvr displayImg  = new ArrayBvr(pstatus);   
                
        planetData planets[];
        planets     = new planetData[6];
        planets[0] = new planetData(toBvr(109.50), toBvr(0.0),   toBvr(2.00),   toBvr(1.00), toBvr(0.0),  sunwav,         sunjpg);
        planets[1] = new planetData(toBvr(0.38),   toBvr(57.9),  toBvr(5.70),   toBvr(0.24), toBvr(7.0),  mercurywav, mercuryjpg);
        planets[2] = new planetData(toBvr(0.95),   toBvr(108.2), toBvr(-20.00), toBvr(0.62), toBvr(3.4),  venuswav,   venusjpg);
        planets[3] = new planetData(toBvr(1.00),   toBvr(149.5), toBvr(1.00),   toBvr(1.00), toBvr(23.3), earthSound, earthTexture);
        planets[4] = new planetData(toBvr(0.27),   toBvr(30.0),  toBvr(27.29),  toBvr(0.25), toBvr(0.0),  moonwav,        moonjpg);
        planets[5] = new planetData(toBvr(0.53),   toBvr(227.9), toBvr(1.00),   toBvr(1.88), toBvr(1.9),  marswav,        marsjpg);

        // scale the planet attributes so they look nice
        for(index=0;index<6;index++)
            planets[index] = scaleData(planets[index]);
                

        // place the zoom button & make sound associated with each
        PickableImage  zoomUpButton     = new PickableImage(zoomup);
        PickableImage  zoomDownButton   = new PickableImage(zoomdown);
        ImageBvr  PickButtons[]    = {zoomUpButton.getImageBvr(),zoomDownButton.getImageBvr()};
        DXMEvent buttonEvent                       = orEvent(andEvent(zoomUpButton.getPickEvent(),leftButtonDown),
                                                             andEvent(zoomDownButton.getPickEvent(),leftButtonDown));
        Cycler PickButtonscyl                      = new Cycler(PickButtons, buttonEvent);
        PickableImage  zoomButton       = new PickableImage((ImageBvr)PickButtonscyl.getBvr());
        PickableImage  placedZoomButton = new PickableImage(zoomButton.getImageBvr().transform(translate(toBvr(.05),toBvr(-.025))));
        SoundBvr buttonSounds[]                    = {zoomin,zoomout};
        Cycler buttonSoundscyl                     = new Cycler(buttonSounds, buttonEvent);
        SoundBvr buttonSound                       = (SoundBvr)buttonSoundscyl.getBvr();

        // place the arrows on the screen
        PickableImage inUpButton                = new PickableImage(inup);
        PickableImage outUpButton       = new PickableImage(outup);
        PickableImage placedInUpButton  = new PickableImage(inUpButton.getImageBvr().transform(translate(toBvr(.03),toBvr(-.025))));
        PickableImage placedOutUpButton = new PickableImage(outUpButton.getImageBvr().transform(translate(toBvr(.037),toBvr(-.025))));


        // center the correct planet, via events attached to the arrow images
        DXMEvent evLeftButton  = andEvent(placedInUpButton.getPickEvent(),leftButtonDown).attachData(new Integer(-1));
        DXMEvent evRightButton = andEvent(placedOutUpButton.getPickEvent(),leftButtonDown).attachData(new Integer(1));
        changePlanetEvent      = orEvent(evLeftButton,evRightButton);
        centerPlanet = (NumberBvr)untilNotify(toBvr(0),changePlanetEvent, this).runOnce();

        // place the text on the screen
        ImageBvr text =
            stringImage(((StringBvr)displayText.nth(centerPlanet)),
                      defaultFont.color(white).bold());

        text = text.transform(translate(-.048,-.025));

        // fill in the planetPositions
        planetPos(planets); 
        ArrayBvr  planetPosArray   = new ArrayBvr(planetPosition);

			  Point3Bvr currPlanet = (Point3Bvr)planetPosArray.nth(centerPlanet);

        // set the camera to look at the correct place....
        Transform3Bvr c = Transform3Bvr.newUninitBvr();
        c.init(until(lookAtFrom(currPlanet,point3(toBvr(0),toBvr(35),toBvr(40)),yVector3), orEvent(andEvent(zoomUpButton.getPickEvent(),leftButtonDown),
                                                                                                                                       andEvent(zoomDownButton.getPickEvent(),leftButtonDown)),
                     until(lookAtFrom(currPlanet,point3(toBvr(0),sub(toBvr(35),mul(localTime,toBvr(35))),sub(toBvr(40),mul(localTime,toBvr(20)))),yVector3),
                           predicate(lte(sub(toBvr(35),mul(localTime,toBvr(35))),toBvr(0))),
                           until(lookAtFrom(currPlanet,point3(toBvr(0),toBvr(0), toBvr(15)),yVector3), orEvent(andEvent(zoomUpButton.getPickEvent(),leftButtonDown),
                                                                                                                                                   andEvent(zoomDownButton.getPickEvent(),leftButtonDown)),
                                 until(lookAtFrom(currPlanet,point3(toBvr(0),add(toBvr(0),mul(localTime,toBvr(35))),add(toBvr(15),mul(localTime,toBvr(25)))),yVector3),
                                       predicate(lte(sub(toBvr(35),mul(localTime,toBvr(35))),toBvr(0))),
                        
                                       c)))));

            
        camera = camera.transform(c);

        // Create the microphone to be transformed to the same
        // place the camera is.
        MicrophoneBvr mic = defaultMicrophone.transform(c);

        // create the planets in the scene, based on the camera
        GeometryBvr worldGeo = placePlanets(planets);                        
        ImageBvr worldImage  = worldGeo.render(camera);

        // Create the sound, and increase its gain to make it more
        // audible.  May need to play with this number.
        SoundBvr worldSound  = worldGeo.render(mic);

				worldSound = worldSound.gain(0.5);

        // If planet in center is Earth then show 
        ImageBvr EarthWeather = ((ImageBvr)displayImg.nth(centerPlanet)).transform(translate(toBvr(-.035),toBvr(-.025)));
                
                
        // create the model
        setImage(overlay(text,
                         overlay(EarthWeather,
                                 overlay(placedInUpButton.getImageBvr(),
                                         overlay(placedOutUpButton.getImageBvr(),
                                                 overlay(placedZoomButton.getImageBvr(),
                                                         overlay(worldImage,stars)))))));

        setSound(mix(buttonSound, worldSound));

    }
	public void cleanup() {
      super.cleanup();
      centerPlanet=null;
	}

    // set up where the planets should be and the transform3bvr to use on each planet
    void planetPos(planetData planets[])
    {
        int index;
                
        for(index=0;index<6;index++)
          {
              Transform3Bvr xfpt;
              if(index == 4) // moon
                {
                    xfpt = compose(translate(sub(planetPosition[3],origin3)),
                                   compose(rotate(yVector3,div(mul(toBvr(2),mul(toBvr(Math.PI),time)), planets[index].orbit)),
                                           translate(planets[index].distance,toBvr(0),toBvr(0))));
                }
              else
                {
                    xfpt = compose(identityTransform3,
                                   compose(rotate(yVector3,div(mul(toBvr(2),mul(toBvr(Math.PI),time)), planets[index].orbit)),
                                           translate(planets[index].distance,toBvr(0),toBvr(0))));
                }
              planetTransforms[index] = xfpt;
              planetPosition[index]   = origin3.transform(xfpt);
          }

    }

		NumberBvr interpolate1(NumberBvr from, NumberBvr to, NumberBvr num)  {
//      NumberBvr dt = sin(localTime);//mul(cycle,add(toBvr(.5),div(sin(localTime),toBvr(2))));
//		  NumberBvr dx = div(mul(num,sub(X(P2),X(P1))),dt);
//		  NumberBvr dy = div(mul(num,sub(Y(P2),Y(P1))),dt);

			NumberBvr temp1 = sub(to,from);
			NumberBvr temp2 = mul(temp1,num);
			NumberBvr temp3 = add(from, temp2);
			return temp3;
		}

		Point3Bvr interpolatePoint3(Point3Bvr from, Point3Bvr to, NumberBvr num)  {
			return point3(
				interpolate1(from.getX(),to.getX(),num),
				interpolate1(from.getY(),to.getY(),num),
				interpolate1(from.getZ(),to.getZ(),num)
			);
		}


    // Place the planets in the corrent spot in space.
    GeometryBvr placePlanets(planetData planets[])
    {
        int index;
        GeometryBvr sphere =
            importGeometry(buildURL(geoBase,"sphere.x"));
        GeometryBvr ss                                  = emptyGeometry;
        
        for(index=0;index<6;index++)
          {
              Transform3Bvr xfGeo = compose(rotate(zVector3,div(mul(planets[index].inclination,toBvr(Math.PI)),toBvr(180))),
                                            compose(rotate(yVector3, div(mul(toBvr(2),mul(toBvr(Math.PI),time)),planets[index].period)),
                                                    scale3(planets[index].radius)));

              GeometryBvr snd = soundSource(planets[index].sound);
              GeometryBvr sndplanet = union(snd, sphere);

              ss = union(ss,
                         ((sndplanet.texture((planets[index].image).mapToUnitSquare())).transform(xfGeo).transform(planetTransforms[index])));

          }                                       

        return(union(ss, directionalLight));
    }

    // scale the planet attributes so they look nice
    planetData scaleData(planetData planet)
    {
        // scale factors
        NumberBvr periodScale   = toBvr(0.1);
        NumberBvr timeScale             = toBvr(10);
        NumberBvr distScale             = toBvr(100);
        NumberBvr radiusScale   = toBvr(6);
        
        // speed up everything by a timescale
        planet.period   = div(planet.period,periodScale);
        planet.orbit    = div(mul(planet.orbit,toBvr(365)),timeScale);

        // adjust the sizes larger
        planet.distance = div(planet.distance,distScale);
        planet.radius   = div(add(toBvr(1),log10(planet.radius)),radiusScale);

        return(planet);
    }

    // Handle the change from one planet to another, via the arrow buttons
  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

      NumberBvr currentNum = (NumberBvr)currentRunningBvr;
      int increment            = ((Integer)eventData).intValue();     
      NumberBvr newValue   = add(currentNum,toBvr(increment));
      newValue                         = (NumberBvr) cond(lt(newValue,toBvr(0)) ,toBvr(5),newValue);
      newValue                         = (NumberBvr) cond(gt(newValue,toBvr(5)) ,toBvr(0),newValue);          
      return untilNotify(newValue,changePlanetEvent,this);
  }
  URL mediaBase;
  URL sndBase;
  URL imgBase;
  URL geoBase;
  NumberBvr centerPlanet;
}

// Class used from storing the data associated with each planet
class planetData {
        NumberBvr       radius,distance,period,orbit,inclination;
        SoundBvr        sound;
        ImageBvr        image;

        planetData(NumberBvr radiusIN,NumberBvr distanceIN,
                           NumberBvr periodIN,NumberBvr orbitIN, NumberBvr inclinationIN,SoundBvr soundIN,ImageBvr imageIN)
        {
                radius          = radiusIN;
                distance        = distanceIN;
                period          = periodIN;
                orbit           = orbitIN;
                inclination     = inclinationIN;
                sound           = soundIN;
                image           = imageIN;
        }
}
                                        
public class Solar extends DXMApplet {
        public Solar() { setModel(new SolarModel()); }
}
                                                         
