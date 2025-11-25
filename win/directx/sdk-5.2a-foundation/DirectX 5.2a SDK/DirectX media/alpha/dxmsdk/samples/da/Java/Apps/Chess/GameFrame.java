import java.awt.*;
import java.util.*;
import PGNViewFrame;

public class GameFrame extends Frame
{
	protected Button m_nextButton = new Button("Next Game");
	protected Button m_prevButton = new Button("Prev Game");
	protected Label m_gameLabel = new Label("Game 0 of 0");
	protected TextArea m_tagsText = new TextArea();
	protected TextArea m_srcText = new TextArea();
	protected PGNViewFrame m_mainFrame = null;
  protected Vector m_games = null;
  protected int m_curGameIdx;

	public GameFrame(PGNViewFrame parent)
	{
		super("Games in this PGN file");

		m_mainFrame = parent;

    setBackground(Color.lightGray);
		setForeground(Color.black);

    m_tagsText.setEditable(false);
    m_srcText.setEditable(false);

		//setup the other components
		GridBagLayout layout = new GridBagLayout();
		setLayout(layout);
		GridBagConstraints gbc = new GridBagConstraints();

    // add the enxt and previous game buttons
		gbc.weightx = 0;
		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.ipadx = 20;
		layout.setConstraints(m_prevButton, gbc);
		add(m_prevButton);


		layout.setConstraints(m_nextButton, gbc);
		add(m_nextButton);
    
    // add the label that displays the current game
    gbc.ipadx = 0;
		gbc.gridwidth = GridBagConstraints.REMAINDER;
		gbc.weightx = 1;
		gbc.insets = new Insets(0,0,0,20);
		gbc.anchor = GridBagConstraints.EAST;

		m_gameLabel.setAlignment(Label.RIGHT);
		layout.setConstraints(m_gameLabel, gbc);
		add(m_gameLabel);
     
    // setup the tags area
		gbc.insets = new Insets(0,0,0,0);	
		gbc.fill = GridBagConstraints.BOTH;
		gbc.anchor = GridBagConstraints.NORTHWEST;
		gbc.weighty = 0;
		gbc.weightx = 0;

		Label label = new Label("Game Information");
		layout.setConstraints(label, gbc);
		add(label);

		gbc.weighty = 1;
		gbc.weightx = 1;
		layout.setConstraints(m_tagsText, gbc);
		add(m_tagsText);

    // setup the game source text area
		gbc.weighty = 0;
		gbc.weightx = 0;
		label = new Label("Game MoveText");
		layout.setConstraints(label, gbc);
		add(label);

		gbc.weighty = 1;
		gbc.weightx = 1;
		layout.setConstraints(m_srcText, gbc);		
		add(m_srcText);

		pack();
	
	}

	public boolean handleEvent(Event evt)
	{
		switch (evt.id)
		{
			case Event.WINDOW_DESTROY:
				hide();
        // uncheck the viewGame in the main frame
				m_mainFrame.m_fileGame.setState(false);
				return true;

			default:
				return super.handleEvent(evt);
		}			 
	}

	public boolean action(Event  evt, Object  what) {
		if (evt.target == m_nextButton) {
			Next();
			return true;
		}	
		else if (evt.target == m_prevButton) {
			Prev();
			return true;
		}
		
		return false;
	}

  void Next() {
    // if we're at the last game do nothing
    if (m_curGameIdx >= m_games.size())
      return;

    // display the next game
    m_curGameIdx++;
    ResetButtons();
    m_mainFrame.SetGame((Game)m_games.elementAt(m_curGameIdx));
    m_mainFrame.ResetGame();
  }

  void Prev() {
    // if there is no previous game do nothing
    if (m_curGameIdx <= 0)
      return;

    // display the previous game
    m_curGameIdx--;
    ResetButtons();
    m_mainFrame.SetGame((Game)m_games.elementAt(m_curGameIdx));
    m_mainFrame.ResetGame();
  }

  
  void ResetButtons() {
    // set the game label text
    m_gameLabel.setText("Game " + (m_curGameIdx + 1) + " of " + m_games.size());
    SetText((Game)m_games.elementAt(m_curGameIdx));

    // enable/disable the next and prev buttons
    if (m_curGameIdx > 0)
      m_prevButton.enable();
    else
      m_prevButton.disable();

    if (m_curGameIdx < m_games.size() - 1)
      m_nextButton.enable();
    else
      m_nextButton.disable();
  }

  // set the text inside the tags and source text areas
  protected void SetText(Game game) {

    // set the tags text
    m_tagsText.setText("");
    Enumeration tags = game.m_tags.elements();
    while(tags.hasMoreElements()){
      m_tagsText.appendText((String)tags.nextElement());
      m_tagsText.appendText("\n");
    }

    // set the source text
    m_srcText.setText(WrapString(game.m_src,40));
  }

  // utility function to wrap a string to fit into a certain number of columns
  public static String WrapString(String str, int cols) {
    StringBuffer buffer = new StringBuffer();

    char[] chars = str.toCharArray();

    int left = chars.length;
    int start = 0;
    int end = 0;

    while (left > 0 ) {
      if (cols > left) {
        buffer.append(chars,start,left);
        return buffer.toString();
      }
      end = start + cols;
      while (chars[end] != ' ') {
        end--;
        if (end <= start) {
          end = start + cols;
          break;
        }
      }
      buffer.append(chars,start,end-start);
      buffer.append("\r\n");
      left -= (end - start);
      start += (end - start);
    }
    return buffer.toString();
  }

  public void SetGames(Vector games) {
    m_games = games;
    m_curGameIdx = 0;
    if (games == null ) {
      // !!! do something smart 
    }
    m_mainFrame.SetGame((Game)m_games.elementAt(m_curGameIdx));
    ResetButtons();
  }

}

