// This applet imports and playback a movie repeatedly.
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.URL;

public class Movie extends DXMApplet {  
  public void init() {
    super.init() ;
    setModel (new MovieModel());
  }
}

// This class extends the Model class.  The createModel method in this class
// is where you construct your model.
class MovieModel extends Model {

  // Import and plays back a movie.
  public void createModel(BvrsToRun bvrs) {

    // Create a movie behavior by importing an avi file.
    URL movieBase = buildURL(getImportBase(),"movie/");
    ImageBvr[] imgArr = {null};
    SoundBvr[] sndArr = {null};
    NumberBvr lenNum = importMovie(buildURL(movieBase, "movie.avi"),
                                   imgArr, sndArr);

    // construct an image and a sound behavior that loop
    ImageBvr movieImg = ImageBvr.newUninitBvr();
    movieImg.init(until(imgArr[0], timer(lenNum), movieImg));

    setImage(overlay(movieImg, solidColorImage(black)));
    setSound(sndArr[0].loop());
  }
}
