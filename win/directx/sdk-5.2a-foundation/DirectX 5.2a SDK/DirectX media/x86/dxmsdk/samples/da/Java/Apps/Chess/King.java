import java.util.Vector;
import Board;
import Piece;

// class: King
// This class is used to provide the chess logic needed to parse the PGN file. This
// class is only utilized suring the parsing of the file, and is not used while
// running the animation.

public class King extends Piece {
	King(int myColor, int xPos, int yPos) {
		super(myColor,xPos,yPos);
		m_type = KING;		
	}
	
	Vector GetLegalMoves() {
		
		Vector result = new Vector();
			
		int dx, dy;
		
		// just check to see if we can move to all the 8 possible locations
		dx = 1; dy = 0;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy), 0);
		
		dx = 1; dy = -1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy), 0);
		
		dx = 0; dy = -1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy), 0);
		
		dx = -1; dy = -1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy), 0);
		
		dx = -1; dy = 0;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy), 0);
		
		dx = -1; dy = 1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy), 0);
		
		dx = 0; dy = 1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy), 0);
		
		dx = 1; dy = 1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy), 0);
		
		return result;
	}
	
}