// Mstcubes.java - Flying Microsoft Cubes
// DirectX Media Java Demo 
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;

class MsCubesModel extends Model {

  private static NumberBvr RotationRate = toBvr(0.2);   // Rate of rotation each cube
  private static NumberBvr SpinRate = toBvr(0.8);   // Rate of spin for all cubes

    GeometryBvr     LogoCube;
    Transform3Bvr   xform1;

  public void createModel(BvrsToRun blst) {    
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "image/");
    URL midiBase = buildURL(mediaBase, "midi/");
    URL geoBase = buildURL(mediaBase, "geometry/");
        
        // import background MIDI sound
        SoundBvr backgroundSound =
            importSound(buildURL(midiBase, "circus.mid"), null).loop();

        // import the images ( Internet Explorer logo files )
        ImageBvr IELogos[] = {importImage(buildURL(imgBase, "clogo1.gif")),
                              importImage(buildURL(imgBase, "clogo2.gif")),
                              importImage(buildURL(imgBase, "clogo3.gif")),
                              importImage(buildURL(imgBase, "clogo4.gif")),
                              importImage(buildURL(imgBase, "clogo5.gif")),
                              importImage(buildURL(imgBase, "clogo6.gif")),
                              importImage(buildURL(imgBase, "clogo7.gif")),
                              importImage(buildURL(imgBase, "clogo8.gif")),
                              importImage(buildURL(imgBase, "clogo9.gif"))};
                                                          
        
        // make the images change
        ImageBvr AnimLogo  = Movie(IELogos);

        // Reactivity
        NumberBvr behaviors[] = {RotationRate,toBvr(0)};
        Cycler cyl = new Cycler(behaviors, leftButtonDown);
        NumberBvr CirclePos = integral((NumberBvr)cyl.getBvr());
                
        // Camera
        CameraBvr  Camera = (perspectiveCamera(toBvr(1),toBvr(0))).transform(compose(translate(toBvr(0),toBvr(0),toBvr(4)),
                                                                                               scale(toBvr(6),toBvr(6),toBvr(.4))));
                                                                                                                                                                                        
        // Lighting
        GeometryBvr Light = directionalLight.transform(translate(toBvr(0),toBvr(0),toBvr(4)));

        // Geometery
				GeometryBvr Cube = (importGeometry(buildURL(geoBase, "cube.x"))).transform(scale3(toBvr(0.5)));
					
					
					//union(Cubebk,union(Cubebt,union(Cubefr,union(Cubelf,union(Cubert,Cubetp)))));
				LogoCube                        = Cube.texture(AnimLogo.mapToUnitSquare());

        // Rotation Data = local axis, local angle, global start position
        CubeRotationData CubeRotations[];
        CubeRotations    = new CubeRotationData[4];
        CubeRotations[0] = new CubeRotationData(vector3(toBvr(0),toBvr(1),toBvr(0)), 
                                                toBvr(0.04), vector3(toBvr(0), toBvr(0),toBvr(2)));
        CubeRotations[1] = new CubeRotationData(vector3(toBvr(0),toBvr(1),toBvr(1)), 
                                                toBvr(0.03), vector3(toBvr(0), toBvr(0),toBvr(-2)));
        CubeRotations[2] = new CubeRotationData(vector3(toBvr(0),toBvr(0),toBvr(1)), 
                                                toBvr(0.02), vector3(toBvr(2), toBvr(0),toBvr(0)));
        CubeRotations[3] = new CubeRotationData(vector3(toBvr(1),toBvr(1),toBvr(1)), 
                                                toBvr(0.03), vector3(toBvr(-2),toBvr(0),toBvr(0)));

        xform1 = rotate(yVector3,CirclePos);

        GeometryBvr CubeSet = CreateCubeSet(CubeRotations);

        // create the model that will be displayed

        ImageBvr im = overlay( (union(CubeSet,Light)).render(Camera),solidColorImage(white));
                                
        setImage(im);
        setSound( backgroundSound );
                
    }

    GeometryBvr CreateCubeSet(CubeRotationData CubeRotations[])
    {
        int NumberOfCubeRotations = CubeRotations.length;
        int index;
        GeometryBvr cubes[];
        GeometryBvr CubeSet = emptyGeometry;
        cubes = new GeometryBvr[CubeRotations.length];

                
        for(index = 0; index < CubeRotations.length; index++)
          {
              cubes[index] = MovingCube(CubeRotations[index].axis,CubeRotations[index].angle,CubeRotations[index].startpos);
              CubeSet = union(CubeSet,cubes[index]);
          }
        return(CubeSet);
    }
        
    ImageBvr Movie(ImageBvr IELogos[]) 
    {
        ArrayBvr a = new ArrayBvr(IELogos);
        return((ImageBvr)a.nth(mod(localTime,toBvr(9))));
    }

    GeometryBvr MovingCube(Vector3Bvr lAxis, NumberBvr lAngle, Vector3Bvr gPos)
    {
        GeometryBvr EachCube = LogoCube.transform(compose(translate(gPos),
                                                          rotate(lAxis, mul(lAngle,mul(SpinRate,mul(localTime,toBvr(9)))))));
        return(EachCube.transform(xform1));                                                                                                               
    }


}

class CubeRotationData{
        Vector3Bvr              axis;
        NumberBvr               angle;
        Vector3Bvr              startpos;

        CubeRotationData(Vector3Bvr localaxis, NumberBvr localangle, Vector3Bvr startposition)
        {
                axis            = localaxis;
                angle           = localangle;
                startpos        = startposition;
        }
}
                                                
public class MsCubes extends DXMApplet {
        public MsCubes() { setModel(new MsCubesModel()); }
}
                                                         

