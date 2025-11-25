import java.util.Vector;
import Board;
import Piece;
import Parser;


// class: Pawn
// This class is used to provide the chess logic needed to parse the PGN file. This
// class is only utilized suring the parsing of the file, and is not used while
// running the animation.

public class Pawn extends Piece {

	public static Parser m_parser = null;
	
	int m_EnPassantLocs[][] = new int[2][2];
	
	Pawn(int myColor,int xPos, int yPos){
		super(myColor, xPos, yPos);
		m_type = PAWN;
		ClearEnPassante();  
	}
	
	
	Vector GetLegalMoves() {
		
		Vector result = new Vector();
		
		int yDelta;
		
		if (m_color == WHITE )
			yDelta = 1;
		else
			yDelta = -1;
		
		int x = m_xPos;
		int y = m_yPos + yDelta;
		
		//see if we can move forward
		if (Parser.board().IsOnBoard(x,y) && (Parser.board().GetPiece(x, y) == null)) {
			result.insertElementAt(Board.CoordToString(x,y),0);
			y = y + yDelta;
			if (!m_moved && Parser.board().IsOnBoard(x,y)) {
				if (Parser.board().GetPiece(x,y) == null)
					result.insertElementAt(Board.CoordToString(x,y),0);
			}   
		}
		
		//check to see if we can capture a piece
		y = m_yPos + yDelta;
		x = m_xPos + 1;
		
		if (Parser.board().IsOnBoard(x,y) && (Parser.board().GetPiece(x, y) != null)
			&& (Parser.board().GetPiece(x, y).GetColor() != m_color)) {
			result.insertElementAt(Board.CoordToString(x,y),0);
		}
		
		y = m_yPos + yDelta;
		x = m_xPos - 1;
		
		if (Parser.board().IsOnBoard(x,y) && (Parser.board().GetPiece(x, y) != null)
			&& (Parser.board().GetPiece(x, y).GetColor() != m_color)) {
			result.insertElementAt(Board.CoordToString(x,y),0);
		}
		
		// and en passant moves if we have them
		if (m_EnPassantLocs[0][0] != -1){
			result.insertElementAt(Board.CoordToString(m_EnPassantLocs[0][0], m_EnPassantLocs[0][1]),0);
			if (m_EnPassantLocs[1][0] != -1)
				result.insertElementAt(Board.CoordToString(m_EnPassantLocs[1][0], m_EnPassantLocs[1][1]),0);
		}
		
		
		return result;
	}
	
	
	
	void Move(int x, int y) {
		m_moved = true;
		int xp;
		
		int yDir = (m_color == WHITE) ? 1 : -1;
		
		if (Math.abs(y - m_yPos) == 2) {
			// check for possibility of being captured en passant
			xp = x + 1;
			if (Parser.board().IsOnBoard(xp,y) &&
				(Parser.board().GetPiece(xp,y) != null) &&
				(Parser.board().GetPiece(xp,y).GetColor() != m_color) &&
				(Parser.board().GetPiece(xp,y).GetType() == PAWN)){
				((Pawn)Parser.board().GetPiece(xp,y)).AddEnPassante(x, y - yDir);
			}
			
			xp = x - 1;
			if (Parser.board().IsOnBoard(xp,y) &&
				(Parser.board().GetPiece(xp,y) != null) &&
				(Parser.board().GetPiece(xp,y).GetColor() != m_color) &&
				(Parser.board().GetPiece(xp,y).GetType() == PAWN)){
				((Pawn)Parser.board().GetPiece(xp,y)).AddEnPassante(x, y - yDir );
			}
		}
		m_xPos = x;
		m_yPos = y;
		
		// check to see if we just captured someone en-passant
		
		if (x == m_EnPassantLocs[0][0]){
			Parser.board().KillPiece(m_EnPassantLocs[0][0],m_EnPassantLocs[0][1] - yDir);
			m_parser.NotifyOfCapture(m_EnPassantLocs[0][0],m_EnPassantLocs[0][1] - yDir);
		}
		if (y == m_EnPassantLocs[1][0]){
			Parser.board().KillPiece(m_EnPassantLocs[1][0],m_EnPassantLocs[1][1] - yDir);
			m_parser.NotifyOfCapture(m_EnPassantLocs[1][0],m_EnPassantLocs[1][1] - yDir);
		}
		
	}
	
	
	////////////////////////////////////////////////////////////////
	//
	// Function Name: AddEnPassante
	// Parameters: 
	// Returns: 
	// Effects: m_EnPassantLocs stores the location that this piece can move to
	// and capture a piece En Passante
	//
	/////////////////////////////////-------------------------------
	
	
	void AddEnPassante(int x, int y){
		if (m_EnPassantLocs[0][0] == -1){
			m_EnPassantLocs[0][0] = x;
			m_EnPassantLocs[0][1] = y;
		} else {
			m_EnPassantLocs[1][0] = x;
			m_EnPassantLocs[1][1] = y;
		}
	}
	
	void ClearEnPassante(){
		
		m_EnPassantLocs[0][0] = -1;
		m_EnPassantLocs[1][0] = -1;
	}
	
	
}









