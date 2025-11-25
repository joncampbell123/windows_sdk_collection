// <Tutorial Section=1.0 Title="Section 1: Latency Mask with Asynchronous Download">
/**
This demo makes use of Asynchronous importation APIs to mask
load time.  Here, an opening animation plays until the
media is loaded.
<BR>
**/
// </Tutorial>
//

import com.ms.dxmedia.*;
import java.net.*;

// <Tutorial Section=1.1>
// This class constructs the image and sound behaviors for the model.
class AsyncModel extends Model {

  public void createModel(BvrsToRun blist) {
    try {

      // Prepare the out parameters for importMovie.
      ImageBvr[] movieImg = {null};
      SoundBvr[] movieSnd = {null};
      NumberBvr[] progress = {null};
      ModifiableBehavior progressSw = new ModifiableBehavior(toBvr(0));
      URL movieURL;

      // The location of the movie.
        movieURL = buildURL(getImportBase(),"movie/movie.avi");
					
      // Create a stand-in image.
      loadAnimation waitLogo = new loadAnimation();
      ImageBvr standinImg = waitLogo.getImage((NumberBvr)progressSw.getBvr());

      // Create a movie image behavior and a movie sound behavior by
      // importing an avi file.
      importMovie(movieURL, movieImg, movieSnd,
                  standinImg, silence,
                  null, progress, null);

      progressSw.switchTo(progress[0]);

      // Set the image and sound behaviors for this model.  The stand-in image
      // will be displayed while the movie is being downloaded.  In this
      // example, the stand-in sound is silence.
      setImage(overlay(movieImg[0], solidColorImage(black)));
      setSound(movieSnd[0]);
    } catch(MalformedURLException ex) {}
  }
}

// Set the model in our applet class.
public class AsyncApplet extends DXMApplet {
  public void init() {
    super.init();
    setModel(new AsyncModel());
  }
}
// </Tutorial>
