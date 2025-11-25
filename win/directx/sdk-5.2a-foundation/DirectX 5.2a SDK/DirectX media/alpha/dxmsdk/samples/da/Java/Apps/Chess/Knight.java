import java.util.Vector;
import Board;
import Piece;


// class: Knight
// This class is used to provide the chess logic needed to parse the PGN file. This
// class is only utilized suring the parsing of the file, and is not used while
// running the animation.

public class Knight extends Piece {
	
	Knight(int myColor, int xPos, int yPos) {
		super(myColor, xPos, yPos);
		m_type = KNIGHT;
		
	}
	
	Vector GetLegalMoves() {
		
		Vector result = new Vector();
		
		int dx, dy;
		
		// just check to see if we can move to all the 8 possible locations
		dx = 1; dy = 2;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy),0);
		
		dx = 1; dy = -2;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy),0);
		
		dx = -1; dy = 2;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy),0);
		
		dx = -1; dy = -2;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy),0);
		
		dx = 2; dy = 1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy),0);
		
		dx = 2; dy = -1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy),0);
		
		dx = -2; dy = 1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy),0);
		
		dx = -2; dy = -1;
		if (IsOccupiable(m_xPos + dx, m_yPos + dy))
			result.insertElementAt(Board.CoordToString(m_xPos + dx, m_yPos + dy),0);
		
		return result;
	}
	
}