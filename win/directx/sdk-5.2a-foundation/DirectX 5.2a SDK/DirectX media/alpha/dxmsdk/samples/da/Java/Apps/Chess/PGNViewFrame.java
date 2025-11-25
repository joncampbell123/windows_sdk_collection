//******************************************************************************
// PGNViewFrame.java:	
//
//******************************************************************************
import java.awt.*;
import com.ms.dxmedia.*;
import GameFrame;
import java.io.*;
import ChessModel;
import java.util.Vector;
import Game;


public class PGNViewFrame extends Frame 
{
  // AWT components for the GUI
  protected Button m_nextButton = new Button("Next Move");
  protected Button m_prevButton = new Button("Prev Move");
  protected	MenuItem m_fileOpen = new MenuItem("Open...");
  protected Label m_moveLabel = new Label("Move 0 of 0");
  protected Checkbox m_auto = new Checkbox("AutoPlay");

  public CheckboxMenuItem m_fileGame = new CheckboxMenuItem("Show Games");
  protected GameFrame m_gameFrame = new GameFrame(this);


  AutoPlayer m_autoThread = new AutoPlayer(this);
  // DXM stuff
  protected ChessCanvas m_canvas = null;
  protected ChessModel m_model = null;

  // the current game
  protected Game m_currentGame = null;

  // the index of which move we are on in tha game
  protected int m_curMoveIdx = 0;


  
  public static void main(String args[])
  {
    // create an instance of the frame
    PGNViewFrame frame = new PGNViewFrame();
    frame.show();
    frame.hide();
    frame.resize(frame.insets().left + frame.insets().right  + 300,
		 frame.insets().top  + frame.insets().bottom + 340);
    frame.show();
    
    // create and start the DXM animation
    frame.startme();

    if (args.length > 0)
      frame.OpenFile(args[0]);
  }


  public PGNViewFrame()
  {
    super ("Chess PGN Viewer");
    System.err.println("ARG");
    DisableButtons();
    System.err.println("ARG");
  }

  public void startme() {

    System.err.println("ARK");
    setBackground(Color.lightGray);
    setForeground(Color.black);

    //setup the frame's menubar

    MenuBar bar = new MenuBar();

    Menu fileMenu = new Menu("File");		
	
    System.err.println("ARK");
    fileMenu.add(m_fileOpen);
    fileMenu.add(new MenuItem("-"));

    System.err.println("ARK");
    fileMenu.add(m_fileGame);
    //m_fileGame.setState(false);
    System.err.println("ARK");

    bar.add(fileMenu);
    setMenuBar(bar);
		
		
    //setup the move buttons
    GridBagLayout layout = new GridBagLayout();
    setLayout(layout);
    GridBagConstraints gbc = new GridBagConstraints();
    System.err.println("ARK");
    gbc.weightx = 0;
    gbc.fill = GridBagConstraints.HORIZONTAL;
    gbc.ipadx = 10;
    layout.setConstraints(m_prevButton, gbc);
    add(m_prevButton);
    System.err.println("ARK");

    layout.setConstraints(m_nextButton, gbc);
    add(m_nextButton);
    System.err.println("ARK");
    // add an autoPlay checkbox
    gbc.fill = GridBagConstraints.NONE;
    gbc.ipadx = 0;
    gbc.insets = new Insets(0,10,0,0);	
    gbc.anchor = GridBagConstraints.EAST;
    layout.setConstraints(m_auto,gbc);
    add(m_auto);

    //add the label giving move information
    gbc.fill = GridBagConstraints.HORIZONTAL;
    gbc.gridwidth = GridBagConstraints.REMAINDER;
    gbc.weightx = 1;
    gbc.insets = new Insets(0,0,0,10);
		
    m_moveLabel.setAlignment(Label.RIGHT);
    layout.setConstraints(m_moveLabel, gbc);
    add(m_moveLabel);

    // add the canvas for displaying the DXM Animation
    gbc.insets = new Insets(0,0,0,0);	
    gbc.fill = GridBagConstraints.BOTH;
    gbc.anchor = GridBagConstraints.NORTHWEST;
    gbc.weighty = 1;
    gbc.weightx = 1;
    System.err.println("ART");
    m_model = new ChessModel();
    m_canvas = new ChessCanvas(m_model);
    System.err.println("HM!");
    m_canvas.resize(350,300);
    System.err.println("MPH!");
    layout.setConstraints(m_canvas, gbc);
    add(m_canvas);
    pack();

    m_autoThread.start();
	
  }

  public boolean handleEvent(Event evt)
  {
    switch (evt.id)
      {
	// Application shutdown (e.g. user chooses Close from the system menu).
      case Event.WINDOW_DESTROY:
	dispose();
	System.exit(0);
	return true;

      default:
	return super.handleEvent(evt);
      }			 
  }

  public boolean action(Event  evt, Object  what) {
    boolean result = false;

    // sychronize with the canvas to make sure all the code we execute here
    // is completed before the next frame of the animation is generated.
    synchronized(m_canvas){
      // callbacks for the menu items and move buttons.
      if (evt.target == m_nextButton) {
	Next();
	result = true;
      }	
      else if (evt.target == m_prevButton) {
	Prev();
	result = true;
      }
      else if (evt.target == m_fileGame) {
	if (m_fileGame.getState()){
	  m_gameFrame.show();
	} else {
	  m_gameFrame.hide();
	}
	result = true;
      }
      else  if (evt.target == m_fileOpen) {
	OpenFileGUI();
	result = true;
      }
      else  if (evt.target == m_auto) {
	//          System.err.println("WE GO");
	AutoPlay();
	result = true;
      }
    } 
    
    return result;
  }

  protected void Next() {
    // synchronize again just in case we're called from outside of action() by
    // the AutoPlayer
    synchronized (m_canvas) {
    
      // if we're at the end of the game, do nothing
      if (m_curMoveIdx >= m_currentGame.m_moves.size())
        return;
      if (!m_nextButton.isEnabled())
        return;
      
      //			System.err.println("moving NEXT");
      // otherwise animate the next move
      MoveForward((Move)m_currentGame.m_moves.elementAt(m_curMoveIdx));
      m_curMoveIdx++;
      ResetButtons();
    }
  }

  protected void Prev() {
    // if we're at the beginning of the game do nothing
    if (m_curMoveIdx <= 0)
      return;

    // otherwise animate the current move backwards
    m_curMoveIdx--;
    MoveBackward((Move)m_currentGame.m_moves.elementAt(m_curMoveIdx));
    ResetButtons();
  }

  public void ResetGame() {
    // reset the game to initial state. For simplicity, we'll just create a new
    // model and canvas
    remove(m_canvas);

    m_model = new ChessModel();
    m_canvas = new ChessCanvas(m_model);
    m_canvas.resize(300,300);
    GridBagConstraints gbc = new GridBagConstraints();

    gbc.gridwidth = GridBagConstraints.REMAINDER;

    gbc.fill = GridBagConstraints.BOTH;
    gbc.anchor = GridBagConstraints.NORTHWEST;
    gbc.weighty = 1;
    gbc.weightx = 1;

    ((GridBagLayout)getLayout()).setConstraints(m_canvas, gbc);
    add(m_canvas);
    pack();

    m_curMoveIdx = 0;
  } 

  void DisableButtons() {
    // disable the buttons so the user can't push buttons that are not appropriate 
    m_prevButton.disable();
    m_nextButton.disable();
    m_fileGame.disable();
    m_gameFrame.hide();
    m_auto.disable();
    if (m_auto.getState()){
      m_autoThread.suspend();
      //			 System.err.println("Suspended autoplayer via open");
      m_auto.setState(false);
    }
  }

  void EnableButtons() {
    m_auto.enable();
    m_fileGame.enable();

  }

  void ResetButtons() {
    // enable/disable the next & prev move buttons based on if we can move forward or back
    // also update the move label in the corner
    m_moveLabel.setText("Move " + (m_curMoveIdx + 1)/2 + " of " + (m_currentGame.m_moves.size()+1)/2);

    if (m_curMoveIdx > 0)
      m_prevButton.enable();
    else
      m_prevButton.disable();

    if (m_curMoveIdx < m_currentGame.m_moves.size())
      m_nextButton.enable();
    else
      m_nextButton.disable();
  }

  // load a new file via a FileOpen dialog
  void OpenFileGUI() {
    FileDialog dlg = new FileDialog(this, "Open PGN file...", FileDialog.LOAD);

    dlg.show();

    if (dlg.getFile() != null){
      OpenFile(dlg.getFile());
    }
		
  }

  // open a filename
  void OpenFile(String filename) {
		System.out.println(filename);
    DisableButtons();
    try {
      // parse the game into a format easy to animate. (Game classes)
      Parser parser = new Parser();
      parser.Reset();
      m_gameFrame.SetGames(parser.ParseFile(filename));

    } catch (Exception e)		{
      System.out.println("ERROR PARSING GAME!!!");
      ResetGame();
      m_gameFrame.SetGames(null);
      return;
    }

    ResetGame();
    EnableButtons();
    ResetButtons();
  }

  void SetGame(Game game) {
    m_currentGame = game;
  }

  // animate a Move structure
  void MoveForward(Move move) {
    if (move.result != null) {
      m_model.m_resultLabel.switchText(move.result);
      return;
    }
    // if we are promoting a piece, tell the model to promote it
    if (move.promote != -1)
      m_model.PromotePiece(move.xFrom,move.yFrom, move.promote);
    
    // if we cature a piece, tell the model to capture it
    if (move.xCapture != -1) 
      m_model.CapturePiece(move.xCapture, move.yCapture, move.player);
    
    // tell the model to animate the move
    m_model.MovePiece(move.xFrom, move.yFrom, move.xTo, move.yTo);

    // if we're castling, tell the model to move the second piece
    if (move.xFrom2 != -1)
      m_model.MovePiece(move.xFrom2, move.yFrom2, move.xTo2, move.yTo2);


  }

  // Animate a move structure backward. undo a MoveForward.
  void MoveBackward(Move move) {
    
    if (move.result != null) {
      m_model.m_resultLabel.switchText(null);
      return;
    }
    // unpromote if nessecary
    if (move.promote != -1)
      m_model.PromotePiece(move.xTo,move.yTo, Piece.PAWN);
    
    // move the piece back
    m_model.MovePiece(move.xTo, move.yTo, move.xFrom, move.yFrom);

    // uncastle
    if (move.xFrom2 != -1)
      m_model.MovePiece(move.xTo2, move.yTo2, move.xFrom2, move.yFrom2);

    // revive any captured piece
    if (move.xCapture != -1) 
      m_model.RevivePiece(move.xCapture, move.yCapture, move.player);
    
  }

  // stuff for Autoplay button  
  protected void AutoPlay() {
    if (m_auto.getState()) {
      m_autoThread.resume();
    } else {
      m_autoThread.suspend();
    }
  }
}


class AutoPlayer extends Thread {

  protected PGNViewFrame m_frame = null;

  AutoPlayer (PGNViewFrame frame) {
    m_frame = frame;
  }

  public void run() {
    suspend();

    while (true) {
      try {
        m_frame.Next();
        Thread.sleep(4000);
      } catch (InterruptedException e){
        // do nothing
      }
    }
  }
}
