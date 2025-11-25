//
// This is an example of using simple sampled sounds, dynamically transformed 
// and composited using standard ActiveAnimation controls to create a much more 
// rich and dynamic parametrically controlled sound.  
//
// Uses: utility.class and algosnd.class
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;
import encarta_module.*;
import java_utility.*;

public class waterfall extends DXMApplet {
    waterfall() { 
		_model = new waterfallmodel();
		setModel(_model); 
	}

	public void destroy() {
      super.destroy();
      _model = null;
	}
  private waterfallmodel _model;

}

class waterfallmodel extends Model 
{
	public waterfallmodel(){
		_encartaBase = null;
		_encarta= false;
	}

	public waterfallmodel(URL mediaBase)  {
		_encartaBase = mediaBase;
		_encarta = true;
	}

	public void createModel(BvrsToRun blist)	
	{
		URL mediaBase;
		if(_encarta)
          mediaBase = _encartaBase;
		else
		  mediaBase = getImportBase();

		URL imgBase = buildURL(mediaBase, "image/");
		URL sndBase = buildURL(mediaBase, "sound/");
		// Setup of audio environment
		SoundBvr audioEnv = mix(waterfallSynth.getSound(sndBase), parrotSynth.getSound(sndBase));
		setSound(audioEnv.gain(1.5));

		// Setup of the animation
		cellImage1 fallsImg  = new cellImage1(buildURL(imgBase, "waterfall.jpg"),2);
		cellImage1 parrotImg = new cellImage1(buildURL(imgBase, "parrot.gif"),2);
		ImageBvr  coupleImg  = importImage(buildURL(imgBase, "couple.gif"));
		ImageBvr  branch1Img = importImage(buildURL(imgBase, "branch1.gif"));
		ImageBvr  branch2Img = importImage(buildURL(imgBase, "branch2.gif"));

		NumberBvr B1_X,B1_Y,B2_X,B2_Y;
		B1_X = branch1Img.boundingBox().getMax().getX();
		B1_Y = branch1Img.boundingBox().getMax().getY();
		B2_X = branch2Img.boundingBox().getMax().getX();
		B2_Y = branch2Img.boundingBox().getMax().getY();

		ImageBvr movCoupleImg = 
			coupleImg.transform(
				translate(
					mul( mul(toBvr(8),pixelBvr), 
						 cos( integral(toBvr(Math.random())) ) ),
					add( toBvr(-.017),
						mul( mul(toBvr(2),pixelBvr), 
							 sin(localTime) ))				
				));

		// set up swaying branches
		NumberBvr sway1 = add(add(mul(toBvr(.06),sin(mul(toBvr(2.4),globalTime))),
								  mul(toBvr(.095),sin(globalTime))),
							  toBvr(.2));

		NumberBvr xOff,yOff,jitter;
		xOff = fallsImg._BaseImg.boundingBox().getMax().getX();
		// NOTE: The yOff value returned appears to be incorrect (1/20/97)
		yOff = fallsImg._BaseImg.boundingBox().getMax().getY(); 						 

		ImageBvr movBranch1Img =
			branch1Img.transform(
				compose(
					translate(
						sub(mul(toBvr(1.005),xOff), B1_X), 
						add(mul(toBvr(.5), yOff), mul(toBvr(.0005),sin(globalTime))) ),
					compose(
						translate(B1_X, mul(toBvr(.8),B1_Y) ),
						compose(
							xShear(sway1),
							translate(neg(B1_X), mul(toBvr(-.8),B1_Y))
						)
					)
				) 
			);

		NumberBvr sway2 = add(add(mul(toBvr(-.1),sin(mul(toBvr(3.1),globalTime))),
								  mul(toBvr(.074),sin(globalTime))),
							  toBvr(.2));

		ImageBvr movBranch2Img =
			branch1Img.transform(
				compose(
					translate(
						sub(mul(toBvr(1.005),xOff), B2_X), 
						mul(toBvr(.0005),sin(globalTime))),
					compose(
						translate(B2_X, mul(toBvr(.8),B2_Y)),
						compose(
							xShear(sway2),
							translate(neg(B2_X), mul(toBvr(-.8),B2_Y)) 
						)
					)
				)
			);
		
		ImageBvr sceneImg =
			overlay(movBranch1Img, 
			overlay(movBranch2Img,
			overlay(movCoupleImg, 
			overlay(parrotImg.loopStrip(toBvr(1)), 
					fallsImg.loopStrip(toBvr(2)) ))));
		
		setImage(sceneImg);	
	}
  public void cleanup() {
	_encartaBase =null;
  }

	URL _encartaBase;
	boolean _encarta;
	
}

