import Piece;
import Pawn;
import Game;
import Board;
import TokenInputStream;
import java.util.*;
import java.io.*;
import Move;


// class: Parser
//
// This class parses a PGN chess file into a Games made up of a list of Move classes.
// To parse the game this class makes uses the Board, Piece, Pawn, King, Queen, Bishop,
// Knight, and Rook classes. These classes are only used to parse the file when the 
// file is opened. The parser and these classes are not used while the animation is
// running. 
//


public class Parser {
	
	public int MAX_LINE_LENGTH = 256;
	public int MIN_GAME_LENGTH = 16;
	
	protected Vector m_games = new Vector();
	
	protected int m_currentGame;
	
	protected static Board m_board;
		
	protected int m_xCapture = -1;
  protected int m_yCapture = -1;
	
	public Parser() {
		m_board = null;
		Pawn.m_parser = this;    
	}
	
  public static Board board() { return m_board;};
	
	void Reset() {
		
		m_board = new Board();
		
    m_xCapture = -1;
    m_yCapture = -1;
  }	
	
	void NotifyOfCapture(int x, int y) {
		m_xCapture = x;
    m_yCapture = y;
    
	}
	
	
	
	////////////////////////////////////////////////////////////////
	//
	// Function Name: Parsefile
	// Parameters: filename - name of PGN database
	// Returns: number of games parsed. 0 indicates an error occured.
	// Effects: 
	//
	/////////////////////////////////-------------------------------
	
	
	Vector ParseFile(String filename) throws FileNotFoundException{
		
		m_currentGame = 0;
		
		TokenInputStream is = new TokenInputStream(new FileInputStream(filename));
		
		try {
			while(is.peek() > 0) {
				ParseGame(is);   
			}
		} catch (Exception e) {
			System.out.println("ERROR Parsing Game!!");
			return null;
		}
		if ( m_currentGame > 0)
      return m_games;
    else
      return null;
	}
	
	
	////////////////////////////////////////////////////////////////
	//
	// Function Name: ParseGame
	// Parameters: is - stream to get the game from
	// Effects: parses the game. throws if there is an error. reads the
	// file until the error or the game has been read.
	//
	/////////////////////////////////-------------------------------
	
	
	void ParseGame(TokenInputStream is) throws IOException, Exception {
		char next;

		StringBuffer srcCopy = new StringBuffer();
		
		Reset();
		
		m_games.insertElementAt(new Game(),m_currentGame);
		
		
		char buf[] = new char[MAX_LINE_LENGTH];
		
		//move up to tag pair section
		next = is.peek();
		is.setWhite("\r\n");
		// read in all the tags
		while (next == '['){
			int read = 0;
			read = is.read(buf,0,buf.length);
			
			String tag = new String(buf,0,read);
			
			Game current_game = (Game)m_games.elementAt(m_currentGame);
			current_game.m_tags.insertElementAt(tag, current_game.m_tags.size());
			
			is.eatWhite();
			next = is.peek(); 
		}
		
		is.setWhite("\r\n\t ");
		
		// now we're at the beginning of the movetext
		if (next != -1){ 
			while (is.peek() != 0) {
				if (ParseMove(is, srcCopy))
					break;
			}
		}
		if (((Game)m_games.elementAt(m_currentGame)).m_moves.size() == 0) {
			m_games.removeElementAt(m_currentGame);
			
		} else {
			
			((Game)m_games.elementAt(m_currentGame)).m_src = srcCopy.toString();
			m_currentGame++;
		}
	}
	
	
	////////////////////////////////////////////////////////////////
	//
	// Function Name: CharToType
	// Parameters: character identifying piece type
	// Returns: corresponding enumeration type
	//
	/////////////////////////////////-------------------------------
	
	int CharToType(char c) throws Exception{
		switch(c) {
		case 'P':
			return Piece.PAWN;
		case 'N':
			return Piece.KNIGHT;
		case 'B':
			return Piece.BISHOP;
		case 'R':
			return Piece.ROOK;
		case 'Q':
			return Piece.QUEEN;
		case 'K':
			return Piece.KING;
		default:
			throw new Exception("Invalid piece code");
		}
	}

	public static char TypeToChar(int c) {
	switch(c) {
		case Piece.PAWN:
			return 'P';
		case Piece.KNIGHT:
			return 'N';
		case Piece.BISHOP:
			return 'B';
		case Piece.ROOK:
			return 'R';
		case Piece.QUEEN:
			return 'Q';
		case Piece.KING:
			return 'K';
		default:
			return '!';
		}
	
	}
	
	int FindChar(char buf[], char c, int len) {
		for (int i = 0; i < len; i++) {
			if (buf[i] == c)
				return i;
		}
		return -1;
	}
	
	////////////////////////////////////////////////////////////////
	//
	// Function Name: ParseMove
	// Parameters: input stream with cursor positioned at beginning of move to parse
	// Returns: True if the move signifies the end of the game
	// Effects: Adds move to move list (if appropriate) moves stream cursor to next element of movetext.
	//
	/////////////////////////////////-------------------------------
	
	
	boolean ParseMove(TokenInputStream is, StringBuffer srcCopy)throws IOException, Exception {
		
		char buf[] = new char[16];
		int iEnd, i;
		int xTo, yTo;  
		int xFrom = -1, yFrom = -1;
		char promote = 0;
		
    Move Result = new Move();
		
		int srcType = Piece.PAWN;
		
		int len = is.read(buf, 0, buf.length);

		// copy this move for src
    srcCopy.append(new String(buf,0,len)).append(" ");

		
		// check to see if it's a move number
	if ((i = FindChar(buf,'.', len)) != -1) {
		if (i == (len - 1))
			return false;
		else { // move number and real move together, so toss the move number info
			char temp[] = new char[16];
			i++;
			int j;
			for (j=0; i < len; i++,j++){
				temp[j] = buf[i];
			}
			temp[i] = 0;
			buf=temp;
			len = j;
		}
	}
		
		// check for kingside or queenside castling
		if ( (new String(buf, 0, 5)).equals("O-O-O")  ||
			(new String(buf, 0, 5)).equals("o-o-o")){
			// Queenside castling
			if (m_board.m_currentPlayer == Piece.WHITE) {
				
				m_board.MovePiece(4,0,2,0);
				m_board.MovePiece(0,0,3,0);
				Result.xFrom = 4;
        Result.yFrom = 0;
        Result.xTo = 2;
        Result.yTo = 0;
        Result.xFrom2 = 0;
        Result.yFrom2 = 0;
        Result.xTo2 = 3;
        Result.yTo2 = 0;
        Result.player = m_board.m_currentPlayer;
        
			} else {
				
				m_board.MovePiece(4,7,2,7);
				m_board.MovePiece(0,7,3,7);
        Result.xFrom = 4;
        Result.yFrom = 7;
        Result.xTo = 2;
        Result.yTo = 7;
        Result.xFrom2 = 0;
        Result.yFrom2 = 7;
        Result.xTo2 = 3;
        Result.yTo2 = 7;
        Result.player = m_board.m_currentPlayer;
			}
			m_board.m_currentPlayer = Board.OtherPlayer(m_board.m_currentPlayer);
			((Game)m_games.elementAt(m_currentGame)).m_moves.addElement(Result);
			return false;
		} else if ((new String(buf, 0, 3)).equals("O-O")  ||
			(new String(buf, 0, 3)).equals("o-o")){
			// kingside castling
			if (m_board.m_currentPlayer == Piece.WHITE) {
				m_board.MovePiece(4,0,6,0);
				m_board.MovePiece(7,0,5,0);
        Result.xFrom = 4;
        Result.yFrom = 0;
        Result.xTo = 6;
        Result.yTo = 0;
        Result.xFrom2 = 7;
        Result.yFrom2 = 0;
        Result.xTo2 = 5;
        Result.yTo2 = 0;
        Result.player = m_board.m_currentPlayer;
			} else {
				m_board.MovePiece(4,7,6,7);
				m_board.MovePiece(7,7,5,7);
        Result.xFrom = 4;
        Result.yFrom = 7;
        Result.xTo = 6;
        Result.yTo = 7;
        Result.xFrom2 = 7;
        Result.yFrom2 = 7;
        Result.xTo2 = 5;
        Result.yTo2 = 7;
        Result.player = m_board.m_currentPlayer;
			}
			m_board.m_currentPlayer = Board.OtherPlayer(m_board.m_currentPlayer);
			((Game)m_games.elementAt(m_currentGame)).m_moves.addElement(Result);
			return false;
		}
		
		// check for end of game
		if (FindChar(buf,'-', len) != -1 || 
      FindChar(buf,'*', len) != -1 ||
      (new String(buf,0,4)).equals("draw")) {
			

      if ((new String(buf,0,1)).equals("*")) {
        Result.result = "Game ended";
      } else if ((new String(buf,0,3)).equals("0-1")) {
        Result.result = "Black Wins";
      } else if ((new String(buf,0,3)).equals("1-0")) {
        Result.result = "White Wins";
      } else if ((new String(buf,0,7)).equals("1/2-1/2") || 
        (new String(buf,0,4)).equals("draw")) {
			  Result.result = "Draw Game";
      }
      m_board.m_currentPlayer = Board.OtherPlayer(m_board.m_currentPlayer);
			((Game)m_games.elementAt(m_currentGame)).m_moves.addElement(Result);
      System.err.print("parsed result: ");
      System.err.println(Result.result);
			return true;
		
    }
		
		// check to see if it's an annotation
		if (FindChar(buf,'$',len) != -1) return false;
		
		
		//!!! check for comments and RAV 
		
		iEnd = len - 1;
		
		// move past extra annotations
		switch(buf[iEnd]) {    
		case '+':
		case '#':
		case '?':
		case '!':
			iEnd--;
		}
		
		iEnd--;
		
		// check for promotion
		if ((buf[iEnd] == '=')) {
			promote = buf[iEnd + 1];
			iEnd = iEnd - 2;
		}
		
		
		// now we're at the start of the dest location
		xTo = Board.StringToX(new String(buf,iEnd,2));
		yTo = Board.StringToY(new String(buf,iEnd,2));	
		iEnd--;
		
		if (iEnd >= 0){
			//move through capture indicator if it's there
			if (buf[iEnd] == 'x')
				iEnd--;
			
			// iEnd should be at the end of the src piece identification
			i = 0;
			while (i <= iEnd){
				
				if ((buf[i] >= 'B') && (buf[i] <= 'R')){
					
					srcType = CharToType(buf[i]);
					
				} else if ((buf[i] >= 'a') && (buf[i] <= 'h')) {
					
					xFrom = buf[i] - 'a';
				}
				else if ((buf[i] >= '1') && (buf[i] <= '8')) {
					
					yFrom = buf[i] - '1';
				}        
				i++;
				
			}
		}
		// we've got all the information out of the string, now do something with it.
		Result = FindMove(srcType, xTo, yTo, xFrom, yFrom);
      
		if (promote != 0) {
			// and a "=Q" for piece promotion
			Result.promote = CharToType(promote);
			// and change the promoted pawn to whatever type
			m_board.PromotePiece(xTo,yTo,CharToType(promote));
			
		}
		
		// add an ,xA0 if we need to explicitly notify of a capture
		
		if (m_xCapture != -1) {
      Result.xCapture = m_xCapture;
      Result.yCapture = m_yCapture;

      m_xCapture = -1;
      m_yCapture = -1;
  	
		}
		
		((Game)m_games.elementAt(m_currentGame)).m_moves.
			addElement(Result);
		
    return false;
}



////////////////////////////////////////////////////////////////
//
// Function Name: FindMove
// Parameters: pieceType, square the piece is moving to, and
//  whatever knowledge about where it's coming from you have (xFrom & yFrom as -1 signify no knowledge)
// Returns: string descibing move which you must delete
// Effects: 
//
/////////////////////////////////-------------------------------


Move FindMove( int type,
              int xTo, int yTo, int xFrom, int yFrom) throws Exception{
  
  Vector pieces;
	int i;
  int x,y;
  
  pieces = m_board.GetPiecesOfType(type);
	
  // filter out the other player's pieces
  i = 0;
  while(i < pieces.size()){
    
    if (((Piece)pieces.elementAt(i)).GetColor() != m_board.m_currentPlayer){
      pieces.removeElementAt(i);
			continue;
    }
    i++;
  }
	
	// remove pieces that don't match the rank or
	// file specification (if either is given)
	if (xFrom != -1){
		i = 0;
		while(i < pieces.size()){
			x = ((Piece)pieces.elementAt(i)).GetX();
			y = ((Piece)pieces.elementAt(i)).GetY();
			if (x != xFrom){
				pieces.removeElementAt(i);
				continue;
			}
			i++;
		}
	}
	
	if (yFrom != -1){
		i = 0;
		while(i < pieces.size()){
			x = ((Piece)pieces.elementAt(i)).GetX();
			y = ((Piece)pieces.elementAt(i)).GetY();
			
			if (y != yFrom){
				pieces.removeElementAt(i);
				continue;
			}	
			i++;
		}
	}
	
	// remove pieces that cannot move to target square
	
	i = 0;
	while(i < pieces.size()){
		
		if (!((Piece)pieces.elementAt(i)).CanMoveTo(xTo,yTo)){
			pieces.removeElementAt(i);
			continue;
		}
		i++;
	}
	
	//  check for putting King in check disambiguation
	if (pieces.size() > 1) {
		i = 0;
		while(i < pieces.size()){
			x = ((Piece)pieces.elementAt(i)).GetX();
			y = ((Piece)pieces.elementAt(i)).GetY();
			
			if (!m_board.IsKingStillSafe(x, y, xTo, yTo, m_board.m_currentPlayer)){
				pieces.removeElementAt(i);
				continue;
			}
			i++;
		}
	}
	
	// we should have removed all ambiguity;
	//  assert(pieces->Count() == 1);  
	if (pieces.size() != 1) {
		System.out.println("ERROR!");
//		m_board.PrintBoard();
		throw new Exception("Unresolved ambiguity");
	}
	
	xFrom = ((Piece)pieces.elementAt(0)).GetX();
	yFrom = ((Piece)pieces.elementAt(0)).GetY();
	
  
  Move Result = new Move();
  if (m_board.GetPiece(xTo,yTo) != null) {
    Result.xCapture = xTo;
    Result.yCapture = yTo;
  }
  Result.xFrom = xFrom;
  Result.yFrom = yFrom;
  Result.xTo = xTo;
  Result.yTo = yTo;
  Result.player = m_board.m_currentPlayer;

  m_board.MovePiece(xFrom, yFrom, xTo, yTo);
	m_board.m_currentPlayer = Board.OtherPlayer(m_board.m_currentPlayer);

  
	return Result;
	
} 

} 


