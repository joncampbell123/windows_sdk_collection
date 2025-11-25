import java.util.*;
import Board;
import Parser;

// class: Piece
// This class is used to provide the chess logic needed to parse the PGN file. This
// class is only utilized suring the parsing of the file, and is not used while
// running the animation.

// this class is abstract, the Pawn,Rook,Knight,Bishop,King, and Queen classes
// override the GetLegalMoves function.

public abstract class Piece
{	
	//define constants
	static final int PAWN = 0;
	static final int KING = 1;
	static final int QUEEN = 2;
	static final int BISHOP = 3;
	static final int KNIGHT = 4;
	static final int ROOK = 5;

	static final int WHITE = 0;
	static final int BLACK = 1;
	
		
	// define member variables
	int                   m_xPos = 0;
	int                   m_yPos = 0;
	int		                m_type;
	int	                  m_color;
	boolean               m_moved = false;
	
	// method definitions
	Piece( int mycolor, int x, int y){
		this.m_xPos = x;
		this.m_yPos = y;
		this.m_color = mycolor;
	}
	
	
	int GetX() { return m_xPos; }
	int GetY() { return m_yPos; }

	abstract Vector GetLegalMoves();
	
  void Move(int x, int y){  
		m_moved = true;
		m_xPos = x;
		m_yPos = y;
	}
	
	
	boolean IsOccupiable(int x,int y){
		
		if (!Parser.board().IsOnBoard(x,y)) {
			return false; 
		}
		if (Parser.board().GetPiece(x,y) == null) return true;
			
		if (Parser.board().GetPiece(x,y).GetColor() != m_color) return true;
		
		return false;
			
	}
	
	void GetSlideMoves(int xStart, int yStart, int yDelta, int xDelta, Vector result) {

	  int x = xStart;
	  int y = yStart;
	  
	  while (true) {
		  x += xDelta;
		  y += yDelta;
		  
		  // if we're off the board, just stop.
		  if (!Parser.board().IsOnBoard(x,y)) {
			  break; }
		  
		  if (Parser.board().GetPiece(x,y) != null) {
			  //if there is a piece there add it as a move (if capturable) and stop the loop
			  if (Parser.board().GetPiece(x,y).GetColor() != m_color)
				  result.insertElementAt(Board.CoordToString(x,y), result.size());
			  break;
		  } else {
			  //otherwise add the piece and keep going
			  result.insertElementAt(Board.CoordToString(x,y), result.size());
		  }
	  }
	}
	

	boolean CanMoveTo(int x, int y){
		Vector moves = null;
		int i = 0;
		boolean result = false;
		
		String theMove = Board.CoordToString(x,y);
		
		moves = GetLegalMoves();
		
		for (i = 0; i < moves.size(); i++) {
			if (theMove.equals(moves.elementAt(i))){
				result = true;
				break;
			}
		}
		
		return result;
	};
	
	
	int GetColor()              { return m_color; };
	int GetType()           { return m_type; };
}

