// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*; //added to support URL's

class CoffeeModel extends Model {

	final NumberBvr steamDurationConst = toBvr(7.25);

	URL base;
	URL imgbase;
	URL sndbase;

  	public void createModel(BvrsToRun blst) 
  	{
		base  = getImportBase();
		imgbase = buildURL(base,"image/");
		sndbase = buildURL(base,"sound/");

		MontageBvr finalMtg = union(union(steamMontage(),montage()),machineMontage());

		ImageBvr image = overlay(finalMtg.render(),overlay(beans(),
		  importImage(buildURL(imgbase,"clouds_coffee.gif"))));

		setSound(mix(sound().pan(-1),sound().pan(1)));
		setImage(image);
  	}

	SoundBvr sound()
	{
		SoundBvr steamSound = importSound(buildURL(sndbase,"steam.mp2"),null);
		SoundBvr s0 = SoundBvr.newUninitBvr();
		SoundBvr s1 = SoundBvr.newUninitBvr();
		s0.init(until(silence,leftButtonDown,s1));
		s1.init(until(steamSound.gain(0.85), 
			predicate(gt(localTime,steamDurationConst)), s0));
		return(s0);
	}

	MontageBvr montage()
	{
		int total = 5;
		MontageBvr cupImage = emptyMontage;

		for(int i=0; i<total;i++)
		{
			cupImage = union(cupImage, 
				orbitCup(add(mul(toBvr(i),mul(toBvr(2),
				div(toBvr(Math.PI),toBvr(total)))),localTime)));
		}
		return(cupImage);
	}


	ImageBvr beans()
	{
		NumberBvr delay = toBvr(0.5);
		NumberBvr size  = toBvr(0.5);
		ImageBvr  image0= ImageBvr.newUninitBvr();
		ImageBvr  image1= ImageBvr.newUninitBvr();
		image0.init(until(importImage(buildURL(imgbase,"bean1.gif")),predicate(gt(localTime,delay)),image1));
		image1.init(until(importImage(buildURL(imgbase,"bean2.gif")),predicate(gt(localTime,delay)),image0));
		ImageBvr bean1  = image0.transform(scale2(size));
		ImageBvr bean2  = image1.transform(scale2(size));

		NumberBvr tileXSize = toBvr(0.04*0.2);
		NumberBvr tileYSize = toBvr(0.04*0.2);

		ImageBvr beans  = overlay(bean1.transform(translate(-0.01,-0.01)),
								  bean2.transform(translate(0.01,0)));			  		 
		ImageBvr rain   = beans.tile();
		NumberBvr motion = mul(localTime,mul(toBvr(2),div(toBvr(.03),toBvr(4))));
		return(rain.transform(translate(neg(motion),neg(motion))));
	}
							

	MontageBvr machineMontage()
	{
		ImageBvr  image   = ImageBvr.newUninitBvr();
		image.init(until(importImage(buildURL(imgbase,"espreso1.gif")),leftButtonDown,
					  until(importImage(buildURL(imgbase,"espreso2.gif")), predicate(gt(localTime,steamDurationConst)),image)));
		return(imageMontage(image,toBvr(0)));
	}

	MontageBvr steamMontage()
	{
		ImageBvr steamImages[] = {importImage(buildURL(imgbase,"steam_1.gif")),
								  importImage(buildURL(imgbase,"steam_2.gif")),
								  importImage(buildURL(imgbase,"steam_3.gif")),
								  importImage(buildURL(imgbase,"steam_4.gif")),
								  importImage(buildURL(imgbase,"steam_5.gif"))};

		NumberBvr len   = toBvr(4); // number of images.
		NumberBvr index = (NumberBvr) cond(gt(add(div(mul(localTime,len),steamDurationConst),toBvr(1)),len),len,
												add(div(mul(localTime,len),steamDurationConst),toBvr(1)));
		ArrayBvr a      = new ArrayBvr(steamImages);
		ImageBvr  s0    = ImageBvr.newUninitBvr();
		s0.init(until(emptyImage,leftButtonDown,
					  until((ImageBvr) a.nth(index), predicate(gt(localTime,steamDurationConst)),s0)));
		ImageBvr image  = s0.transform(translate(-0.0085,0.0020));
		return(imageMontage(image,toBvr(-0.0001)));
	}

	MontageBvr orbitCup(NumberBvr angle)
	{

		Point3Bvr pos 	   = point3(0,.05,0);
		pos = pos.transform(
			compose(rotate(xVector3,mul(toBvr(7),div(toBvr(Math.PI),toBvr(16)))),
																				      rotate(zVector3, angle)));
		NumberBvr cupAngle = localTime;
		ImageBvr image     = cupImage(cupAngle).transform(compose(translate(pos.getX(),pos.getY()),
																  scale2(sub(toBvr(1),mul(abs(cos(div(angle,toBvr(2)))),toBvr(0.5))))));
		MontageBvr montage = imageMontage(image,neg(pos.getZ()));
		return(montage);
	}


	ImageBvr cupImage( NumberBvr cupAngle)
	{
		ImageBvr cupImages[] = {importImage(buildURL(imgbase,"cup1.gif")),importImage(buildURL(imgbase,"cup2.gif")),
								importImage(buildURL(imgbase,"cup3.gif")),importImage(buildURL(imgbase,"cup4.gif")),
								importImage(buildURL(imgbase,"cup5.gif")),importImage(buildURL(imgbase,"cup6.gif")),
								importImage(buildURL(imgbase,"cup7.gif")),importImage(buildURL(imgbase,"cup8.gif"))};
	
		NumberBvr number = toBvr(7); // number of images
		NumberBvr index  = add(mod(mul(number,div(cupAngle,mul(toBvr(2),toBvr(Math.PI)))),number),toBvr(1));
		ArrayBvr a       = new ArrayBvr(cupImages);
		return((ImageBvr) a.nth(index));
	}
}
						
public class Coffee extends DXMApplet {
	public Coffee() { setModel(new CoffeeModel()); }
}
							 

