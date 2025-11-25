import Piece;
import java.util.*;
import Pawn;
import King;
import Queen;
import Bishop;
import Knight;
import Rook;


// class: Board
// This class is used to provide the chess logic needed to parse the PGN file. This
// class is only utilized suring the parsing of the file, and is not used while
// running the animation.

public class Board {
	
	// member variables
	Piece[][]             m_pieceArray = new Piece[8][8];
	Vector                m_pieces = new Vector();
	
	Piece[]               m_Kings = new Piece[2];
	
	int                 m_currentPlayer;
	
	// method definitions
	
	Board()
	{
		
		for (int i = 0; i <= 7; i++){
			for (int j = 0; j <= 7;  j++) {
				m_pieceArray[i][j] = null;
			}
		}
		
		m_currentPlayer = Piece.WHITE;
		
		SetupBoard();  
	}
	
	
	
	Piece GetPiece(int x, int y){
		return m_pieceArray[x][y];
	}	
	
	void MovePiece(int xFrom, int yFrom, int xTo, int yTo ){
		
		Piece from = GetPiece(xFrom, yFrom);
		
		KillPiece(xTo,yTo);
		m_pieceArray[xTo][yTo] = from;
		from.Move(xTo,yTo);
		m_pieceArray[xFrom][yFrom] = null;
		
		ClearEnPassantes(m_currentPlayer);
	}

	void PrintBoard() {
		System.out.println("Board:");
		for (int i = 7; i >= 0; i--) {
			for (int j = 0; j < 8; j++) {
				if (m_pieceArray[j][i] == null)
					System.out.print(". ");
				else{
					System.out.print(Parser.TypeToChar(m_pieceArray[j][i].GetType()));				
					System.out.print(' ');
				}
			}
			System.out.println("");
		}

	}
	
	void KillPiece(int x, int y) {
		Piece dead = GetPiece(x,y);
		
		if (dead != null) {
			int i = 0;
			while( i < m_pieces.size()){
				if (dead == m_pieces.elementAt(i)){
					m_pieces.removeElementAt(i);
					continue;
				}
				i++;
			}
			m_pieceArray[x][y] = null;
		}
	}
	
	void PromotePiece(int x, int y, int type){
		
		Piece piece = null;
		switch(type){
		case Piece.PAWN:
			piece = new Pawn(GetPiece(x,y).GetColor(),x,y);
			break;
		case Piece.ROOK:
			piece = new Rook(GetPiece(x,y).GetColor(),x,y);
			break;
		case Piece.KNIGHT:
			piece = new Knight(GetPiece(x,y).GetColor(),x,y);
			break;
		case Piece.BISHOP:
			piece = new Bishop(GetPiece(x,y).GetColor(),x,y);
			break;
		case Piece.KING:
			piece = new King(GetPiece(x,y).GetColor(),x,y);
			break;
		case Piece.QUEEN:
			piece = new Queen(GetPiece(x,y).GetColor(),x,y);
			break;
		}
		
		KillPiece(x,y);
		
		m_pieces.insertElementAt(piece,0);
		m_pieceArray[x][y] = piece;  
	}
	
	
	boolean IsOnBoard(int x, int y) {
		return ((x >= 0) && (x <= 7) && (y >= 0) && (y <= 7));
	}
	
    
	////////////////////////////////////////////////////////////////
	//
	// Function Name: IsKingStillSafe
	// Parameters: from and to location of the piece we are moving,
	//             as well as the color of the king who's safety concerns us
	// Returns: False if the move in question would put the king in check
	// Effects: leaves the board in original configuration
	//
	/////////////////////////////////-------------------------------
	
	
	boolean IsKingStillSafe(int xFrom, int yFrom, int xTo, int yTo, int kingsColor){
		Piece to, from; 
		int i, xKing, yKing;
		boolean result = true;
		
		to = GetPiece(xTo, yTo);
		from = GetPiece(xFrom, yFrom);
		
		xKing = m_Kings[kingsColor].GetX();
		yKing = m_Kings[kingsColor].GetY();
		
		m_pieceArray[xTo][yTo] = from;
		m_pieceArray[xFrom][yFrom] = null;
		
		// check to see if any of the other players pieces can attack the king
		i = 0;
		while( i < m_pieces.size()){
			if ((to != m_pieces.elementAt(i)) &&             // we captured this one (maybe, or it's null)
				(((Piece)m_pieces.elementAt(i)).GetColor() != kingsColor) && // quick reject
				(((Piece)m_pieces.elementAt(i)).CanMoveTo(xKing, yKing))) {
				result = false;
				break;
			}
			i++;
		}
		
		//move the board back to original state
		m_pieceArray[xTo][yTo] = to;
		m_pieceArray[xFrom][yFrom] = from;
		
		return result;
	}
	
	
	String GetMovesForPiece(int x, int y) {
		Vector moves = GetPiece(x,y).GetLegalMoves();
		String result = "";
		
		Enumeration enum = moves.elements();
		
		while(enum.hasMoreElements()) {
			result = result + enum.nextElement();
		}
		return result;
	}
	
	
	void ClearEnPassantes(int colorToClear){
		int i = 0;
		while (i < m_pieces.size()){
			if ((((Piece)m_pieces.elementAt(i)).GetType() == Piece.PAWN) && (((Piece)m_pieces.elementAt(i)).GetColor() == colorToClear))
				i = i;//((Pawn)m_pieces.elementAt(i)).ClearEnPassante();
			i++;
		}
	}
	
	
	
	
	////////////////////////////////////////////////////////////////
	//
	// Function Name: GetPiecesOfType
	// Parameters: type of pieces you want
	// Returns: a list of all pieces of that type (ie: gimme all the Rooks)
	// Effects: 
	//
	/////////////////////////////////-------------------------------
	
	
	Vector GetPiecesOfType(int type){
			
		Vector piecesOfType = new Vector();
		Piece currentPiece;
		
		Enumeration pieces = m_pieces.elements();
		while (pieces.hasMoreElements()){
			currentPiece = (Piece)(pieces.nextElement());
			if (currentPiece.GetType() == type)
				piecesOfType.insertElementAt(currentPiece,0);
		}
		return piecesOfType;
	}	
	
	
	
	////////////////////////////////////////////////////////////////
	//
	// Function Name: SetupBoard
	// Parameters: 
	// Returns: 
	// Effects: Sets the initial state of the board.
	//
	/////////////////////////////////-------------------------------
	
	
	void SetupBoard(){
		
		int x,y;
		
		// set up the white ranks
		x = 0; y = 1;
		m_pieceArray[x][y] = new Pawn(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 1; 
		m_pieceArray[x][y] = new Pawn(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 2; 
		m_pieceArray[x][y] = new Pawn(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 3; 
		m_pieceArray[x][y] = new Pawn(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 4; 
		m_pieceArray[x][y] = new Pawn(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 5; 
		m_pieceArray[x][y] = new Pawn(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 6; 
		m_pieceArray[x][y] = new Pawn(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 7; 
		m_pieceArray[x][y] = new Pawn(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		
		x = 0; y = 0;
		m_pieceArray[x][y] = new Rook(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 1; 
		m_pieceArray[x][y] = new Knight(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 2; 
		m_pieceArray[x][y] = new Bishop(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 3; 
		m_pieceArray[x][y] = new Queen(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 4; 
		m_pieceArray[x][y] = new King(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 5; 
		m_pieceArray[x][y] = new Bishop(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 6; 
		m_pieceArray[x][y] = new Knight(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 7; 
		m_pieceArray[x][y] = new Rook(Piece.WHITE, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		
		
		//set up the Black ranks
		x = 0; y = 6;
		m_pieceArray[x][y] = new Pawn(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 1; 
		m_pieceArray[x][y] = new Pawn(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 2; 
		m_pieceArray[x][y] = new Pawn(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 3; 
		m_pieceArray[x][y] = new Pawn(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 4; 
		m_pieceArray[x][y] = new Pawn(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 5; 
		m_pieceArray[x][y] = new Pawn(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 6; 
		m_pieceArray[x][y] = new Pawn(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 7; 
		m_pieceArray[x][y] = new Pawn(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		
		x = 0; y = 7;
		m_pieceArray[x][y] = new Rook(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 1; 
		m_pieceArray[x][y] = new Knight(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 2; 
		m_pieceArray[x][y] = new Bishop(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 3; 
		m_pieceArray[x][y] = new Queen(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 4; 
		m_pieceArray[x][y] = new King(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 5; 
		m_pieceArray[x][y] = new Bishop(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 6; 
		m_pieceArray[x][y] = new Knight(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		x = 7; 
		m_pieceArray[x][y] = new Rook(Piece.BLACK, x, y);
		m_pieces.insertElementAt(m_pieceArray[x][y],0);
		
		// to keep track of the kings
		m_Kings[Piece.WHITE] = m_pieceArray[4][0];
		m_Kings[Piece.BLACK] = m_pieceArray[4][7];
}


////////////////////////////////////////////////////////////////
//
// Function Name: CoordToString
// Parameters: 0 based indexes of coord
// Returns: two character, null terminated string signifying location
// 
// You are responsible for deleting the string.
// 
/////////////////////////////////-------------------------------


static String CoordToString(int x, int y){
	char[] result  = new char[2];
	
	result[0] = (char)('a' + (char)x);
	result[1] = (char)('1' + (char)y);
	
	return new String(result);
	
}

static int StringToX(String location) throws Exception{
	int x,y;
	
	x = location.charAt(0) - 'a';
	y = location.charAt(1) - '1';
	
	//  assert((x >= 0) && (x < 8) && (y >= 0) && (y < 8));
	if (!((x >= 0) && (x < 8) && (y >= 0) && (y < 8)))
		throw new Exception("Invalid board location specified");
	return x;
}

static int StringToY(String location) throws Exception{
	
	int x,y;
	
	x = location.charAt(0) - 'a';
	y = location.charAt(1) - '1';
	
	if (!((x >= 0) && (x < 8) && (y >= 0) && (y < 8)))
		throw new Exception("Invalid board location specified");
	return y;
}

static int OtherPlayer(int color) {
	if (color == Piece.WHITE)
		return Piece.BLACK;
	else
		return Piece.WHITE;
	
}
}



