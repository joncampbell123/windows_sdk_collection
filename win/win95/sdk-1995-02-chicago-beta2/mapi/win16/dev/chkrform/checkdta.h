// Some more defines

#define PLAYER1         BLACK
#define PLAYER2         RED

// Game type definitions

#define GAME_CHECKERS   0       
#define GAME_GIVEAWAY   1
#define GAME_FOXGOOSE   2

// Cursor states

#define UNDEFINED_CURSOR    -1
#define NORMAL_CURSOR       1
#define RESIZE_CURSOR       2

#define RESIZE_RANGE        10      // How far from bottom right should resize
                                    // cursor be shown.

// Data structures for Checkers.

struct rGameStateRec
{
    int iPlayerTurn;                // Who's turn
    BOOL fInitDone;                 // Game initialization over (when to use timer).
    BOOL fContinuable;              // Is the turn continuable?
    BOOL fSetupMode;                // Game in setup mode? Need for button processing
    BOOL fThinking;                 // Is the computer thinking?
    BOOL fPaused;                   // Is the game paused?
    BOOL fGameInProgress;           // Did the game start? How to react to "File:New"
    BOOL fLMouseButtonDown;         // Is the left mouse button currently down?
    BOOL fRMouseButtonDown;         // Is the right mouse button currently down?
    BOOL fMoveInProgress;           // Is the user in the middle of a move?
    BOOL fComputerBusy;             // Is the computer thinking? $$REVIEW (dupe fThinking)
    BOOL fGameOver;                 // Is the game over?
    BOOL fMouseResizeInProgress;    // Is the user resizing the board with the mouse?
    int iCursorState;               // What state is the cursor in (resize, normal?)
};

struct rWeightRec
{
    long lKingWeight;               // All of these are used for weighting
    long lPieceWeight;              // the quality of the board for move
    long lBlankWeight;              // move determination. See quality.cpp
    long lMoveTheoryWeight;
    long lCenterPosWeight;
    long lDoubleCornerWeight;
    long lHoldingKingRowWeight;
    long lEdgesWeight;
    long lFormationWeight;
};

struct rPlayerRec
{
    int iPlayerType;                // (ENUM) Human/Computer/Network
    int iUseOpeningBook;            // (BOOL) Use the opening-book moves
    int iMemPositions;              // (BOOL) Use memorized positions?
    int iUseMoveTheory;             // (BOOL) Use move theory?
    int iUseMoveShuffling;          // (BOOL) Use move shuffling
    int iUseEqualMoveSkipping;      // (BOOL) Use Equal move skipping
    long lUseGoodMoveSkipping;      // (LONG) Use Good move skipping

    int iBookMoveDepth;                 // (INT) Depth of book moves
    int iMaxRecursionDepth;             // (LONG) Max recursion depth
    int iUseAlphaBetaPruning;           // (BOOL) Use alpha-beta pruning
    int iShortJumpEval;                 // (BOOL) Increment recursion level on jump continuation
    long lAlphaPruningOriginalDepth;    // (LONG) Alpha pruning original depth
    long lAlphaPruningWidth;            // (LONG) Alpha pruning width
    long lTimeConstraint;               // (LONG) Computer time restraints
    long lFormationCutoff;              // (LONG) Formation cutoff something
    struct rWeightRec rWeights;         // Weighting structure. See above
};

struct rCheckConfigRec
{
    int iGameType;                      // (INT) enum game type (Checkers/FoxGoose)

    // BOOL's

    int iMustJump;                      // (BOOL) Player must jump when available?
    int iShowTimer;                     // (BOOL) Show the UI Timer?
    int iShowDebug;                     // (BOOL) Show the UI Debug Window
    int iShowMoves;                     // (BOOL) Show the UI Debug Window
    int iShowOpenInfo;                  // (BOOL) Show the opening book info
    int iShowCommandWindow;             // (BOOL) Show the UI Command Window
    int iRandType;                      // (BOOL) Use rand(0) or rand(time)?
    int iBoardFlipped;                  // (BOOL) Board flipped?
    int iSquareSize;                    // (INT) Size of board square
    int iSetupPurgeBoards;              // (BOOL) Purge boards after setup?
    int iToolbar;                       // (BOOL) Toolbar showing?
    int iStatusbar;                     // (BOOL) Statusbar showing?
    int iMaxMoves;                      // (BOOL) Statusbar showing?

    // We use an extra player struct here because we're so screwed up with
    // RED and BLACK definitions. We'll just use RED and BLACK to refer
    // to the indices now.

    struct rPlayerRec   rPlayer[3];       // Config info for players
};
