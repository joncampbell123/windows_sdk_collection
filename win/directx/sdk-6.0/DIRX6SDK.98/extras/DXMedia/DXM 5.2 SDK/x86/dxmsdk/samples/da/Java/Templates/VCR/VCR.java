// <Tutorial Section=1.0 Title="Section 1: Video Playback">
/**
This applet uses AWT UI controls to control the rate of a video playback.
<BR>
**/
//</Tutorial>

import com.ms.dxmedia.*;
import java.awt.*;
import java.applet.*;
import java.net.URL;

//<Tutorial Section=1.1>
// There are two components in this applet.  One is a DXMCanvas used to display
// the movie image, the other is a panel of AWT controls used to control the
// playback.
public class VCR extends Applet {
  public void init() {
    setLayout(new BorderLayout());
    DxmMovie cv = new DxmMovie();
    Panel ctrls = new Controls(cv._model);
    add("Center", cv);
    add("South", ctrls);
  }
}
// </Tutorial>

// <Tutorial Section=2.0 Title="Section 2: Display the Movie Model in DXMCanvas">
// This class extends the DXMCanvas class.  The model you set in this class,
// by calling the setModel() method, is the model that will be displayed.
class DxmMovie extends DXMCanvas {
  DxmMovie() {
    _model = new VCRModel();
    setModel(_model);
  }

  VCRModel _model;
}

// This class extends the Model class.  The createModel method in this class
// is where you construct your model.
class VCRModel extends Model {

  // Import a movie in the model and have a switcher behavior to control
  // the playback rate.
  public void createModel(BvrsToRun bvrs) {

    // Create a movie behavior by importing an avi file.
    URL movieBase = buildURL(getImportBase(), "Movie/");
    ImageBvr[] imgArr = {null};
    SoundBvr[] sndArr = {null};
    NumberBvr lenNum = importMovie(buildURL(movieBase, "movie.avi"),
                                   imgArr, sndArr);

    // The playback rate switcher starts with 1, the normal rate.
    _rateSw = new ModifiableBehavior(toBvr(1));

    // The playback position of the movie is the sum of the rate.  Use
    // modulus to loop the movie since positions less than 0 or greater than
    // the length of the movie yields emptyImage.  The specialMod is a
    // private function that always returns a positive number less than
    // lenNum.
    NumberBvr posNum = specialMod(integral((NumberBvr)_rateSw.getBvr()), lenNum);

    // Use AppTriggeredEvent for the restart event.  When the restart button
    // is pressed, the event will be triggered.  Construct a recursive
    // position behavior - the resetablePosNum - that resets to itself when
    // the event occurs.  Note that integral has implicit time.  When a
    // restart posNum at the time _resetEv occurs, integration starts from
    // 0.
    _resetEv = new AppTriggeredEvent();
    NumberBvr resetablePosNum = NumberBvr.newUninitBvr();
    resetablePosNum.init(until(posNum, _resetEv, resetablePosNum));

    // Play the movie on top of a black background.  Use substituteTime
    // to control the position of a movie.  The localTime for movies
    // is implicit.
    setImage(overlay((ImageBvr)imgArr[0].substituteTime(resetablePosNum),
                     solidColorImage(black)));

    // Loop the sound here. 
    setSound((SoundBvr)sndArr[0].loop().substituteTime(resetablePosNum));
  }

  // This function does a "special" mod for negative numbers.  It returns
  // the linear distance between the passed-in numerator and the biggest
  // multiple of the denominator smaller than the numerator.
  // For example, specialMode(-1, 5) = 4, specialMod(-4, 5) = 1.
  // This is defined so that a decreasing number behavior passed in as the
  // numerator yields a consistent result whether its value is positive or
  // negative.
  NumberBvr specialMod(NumberBvr numNum, NumberBvr denomNum) {
    return (NumberBvr)cond(gt(numNum, toBvr(0)),
                           mod(numNum, denomNum),
                           sub(denomNum, mod(abs(numNum), denomNum)));
  }

  // This method changes the value of the rate switcher to the given rate.
  // This is called by the event handlers of the playback controls.
  void setRate(int rate) {
    _rateSw.switchTo(toBvr(rate*0.1));
  }

  // This method is called when the restart button is pressed.  It resets
  // the rate to the normal playback rate, and triggers the event to
  // restart the movie.
  void restart() {
    setRate(10);
    _resetEv.trigger();
  }

  AppTriggeredEvent _resetEv;
  ModifiableBehavior _rateSw;
}
// </Tutorial>

// <Tutorial Section=3.0 Title="Section 3: Use AWT Controls">

// This class extends Panel. It has four buttons - pause, restart, fast forward,
// rewind - and a scrollbar.  The scrollbar is used to control the playback rate
// at a finer level.
class Controls extends Panel {

  // Place the scrollbar at the center, then two buttons on either side.
  Controls(VCRModel movie) {
    _movie = movie;
    setLayout(new BorderLayout());

    _rateBar = new Scrollbar(Scrollbar.HORIZONTAL, 10, 1, -40, 40);
    add("Center", _rateBar);

    // Pause and restart buttons on the west side.
    Panel pWest = new Panel();
    pWest.setLayout(new GridLayout(1, 2));
    pWest.add(new Button("| |"));
    pWest.add(new Button(">"));
    add("West", pWest);

    // Fast forward and rewind buttons on the east side.
    Panel pEast = new Panel();
    pEast.setLayout(new GridLayout(1, 2));
    pEast.add(new Button(">>"));
    pEast.add(new Button("<<"));
    add("East", pEast);
  }

  // The event handler when any buttons are pressed.
  public boolean handleEvent(Event ev) {
    switch (ev.id) {
      case Event.SCROLL_LINE_UP:
      case Event.SCROLL_LINE_DOWN:
      case Event.SCROLL_PAGE_UP:
      case Event.SCROLL_PAGE_DOWN:
      case Event.SCROLL_ABSOLUTE:

        // Scroll position changed: Clear the pause state, set the rate to
        // the current scroll position.
        _bPaused = false;
        _movie.setRate(_rateBar.getValue());
        return true;

      case Event.ACTION_EVENT:
        if (ev.arg == "| |") {

          // Pause: Toggle the pause state.  This means that pressing the pause
          // button the second time resumes the normal playback rate.
          // The normal playback rate is 10, the pause rate is 0.
          // Need to reflect the current rate in the scrollbar.
          int rate;
          if (_bPaused) {
            _bPaused = false;
            rate = 10;
          } else {
            _bPaused = true;
            rate = 0;
          }
          _movie.setRate(rate);
          _rateBar.setValue(rate);
          return true;
        } else if (ev.arg == ">") {

          // Restart: Clear the pause state, restart the movie at normal rate.
          _bPaused = false;
          _movie.restart();
          _rateBar.setValue(10);
          return true;
        } else if (ev.arg == ">>") {// Fast forward

          // FastForward: Clear the pause state, set rate to 30 and update the
          // scrollbar position.
          _bPaused = false;
          _movie.setRate(30);
          _rateBar.setValue(30);
          return true;
        } else if (ev.arg == "<<") {

          // Rewind: Clear the pause state, set rate to -30 and update the
          // scrollbar position.
          _bPaused = false;
          _movie.setRate(-30);
          _rateBar.setValue(-30);
          return true;
        }
    }
    return false;
  }

  boolean _bPaused = false;
  Scrollbar _rateBar;
  VCRModel _movie;
}

// </Tutorial>
