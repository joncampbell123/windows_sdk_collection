// class : Move
// this class stores all the information nessecary for a single move.
// a list of this is used to specify a game.


public class Move 
{
  Move() {}
  // where we're moving from
  public int xFrom = -1;
  public int yFrom = -1;
  // where we're moving to
  public int xTo = -1;
  public int yTo = -1;
  // any piece we've captured in this move
  public int xCapture = -1;
  public int yCapture = -1;
  // do we promote a piece?
  public int promote = -1;
  // info about castling
  public int xFrom2 = -1;
  public int yFrom2 = -1;
  public int xTo2 = -1;
  public int yTo2 = -1;
  // which player is making this move
  public int player = 0;

  public String result = null;
}

