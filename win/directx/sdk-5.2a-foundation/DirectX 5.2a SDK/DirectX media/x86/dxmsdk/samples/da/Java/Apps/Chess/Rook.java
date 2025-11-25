import Piece;
import Board;
import java.util.Vector;


// class: Rook 
// This class is used to provide the chess logic needed to parse the PGN file. This
// class is only utilized suring the parsing of the file, and is not used while
// running the animation.

public class Rook extends Piece {
	
	Rook(int myColor, int xPos, int yPos) {
		super(myColor, xPos, yPos);
		m_type = ROOK;
	}
	
	Vector GetLegalMoves() {
		
		Vector result = new Vector();

		int dx, dy;
		
		dx = 1; dy = 0;
		GetSlideMoves(m_xPos, m_yPos, dx,dy, result);
		
		dx = -1; dy = 0;
		GetSlideMoves(m_xPos, m_yPos, dx,dy, result);
		
		dx = 0; dy = 1;
		GetSlideMoves(m_xPos, m_yPos, dx,dy, result);
		
		dx = 0; dy = -1;
		GetSlideMoves(m_xPos, m_yPos, dx,dy, result);
		
		return result;
	}
}
