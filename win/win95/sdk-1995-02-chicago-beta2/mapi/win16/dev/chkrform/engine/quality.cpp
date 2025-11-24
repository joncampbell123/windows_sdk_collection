/* --------------------------------------------------------------------------
returns the value of the board from 'computer_colors' perspective
$REVIEW: this routine is so pitifully slow it hurts
-------------------------------------------------------------------------- */
long QualityOfBoard(BOARD b,int t)
{
    /* ----- constants (to be tweaked) ----- */
    long wKing            = 50;
    long wPiece           = 350;
    long wBlank           = 1;
    long wMoveTheory      = 20;
    long wCenterPos       = 2;
    long wDoubleCorner    = 1;
    long wHoldingKingRow  = 1;
    long wEdges           = -1;
    long wFormation       = 10;
    long wFormationCutoff = 6;

    #ifndef SHIPPING_VERSION_REVIEW
        pdebug(stddbg,"!!! WARNING this needs to be removed !!! %s(%d)\n",__FILE__,__LINE__); //REVIEW
        wKing            = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lKingWeight;          
        wPiece           = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lPieceWeight;         
        wBlank           = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lBlankWeight;         
        wMoveTheory      = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lMoveTheoryWeight;
        wCenterPos       = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lCenterPosWeight;
        wDoubleCorner    = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lDoubleCornerWeight;
        wHoldingKingRow  = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lHoldingKingRowWeight;
        wEdges           = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lEdgesWeight;
        wFormation       = rConfig.rPlayer[rGameState.iPlayerTurn].rWeights.lFormationWeight;
        wFormationCutoff = rConfig.rPlayer[rGameState.iPlayerTurn].lFormationCutoff;
    #endif

    #ifdef DEBUG
        static long temp=0;
        temp++;
        if (0 == temp % 500)
        pdebug(stddbgmin,"%ld boards evaluated %s(%d)\n",temp,__FILE__,__LINE__);
    #endif

    /* ----- variable factors ----- */
    long q=50000; /* quality */ //REVIEW
    int co_kings                   = 0;
    int op_kings                   = 0;
    int co_pieces                  = 0;
    int op_pieces                  = 0;
    int blanks                     = 0;
    long movetheory_piecesinsystem  = 0;
    int  i;

    /* ----- king those who deserve it ----- */
    //REVIEW: do we really want to? should we assert here?
    //if (RED & b[1]) b[1] |= KING;
    //if (RED & b[2]) b[2] |= KING;
    //if (RED & b[3]) b[3] |= KING;
    //if (RED & b[4]) b[4] |= KING;
    //if (BLACK & b[32]) b[32] |= KING;
    //if (BLACK & b[31]) b[31] |= KING;
    //if (BLACK & b[30]) b[30] |= KING;
    //if (BLACK & b[29]) b[29] |= KING;

    /* ----- number of pieces ----- */
    blanks = SQRS_MAX-2;
    for (i=1;i<SQRS_MAX; i++)
        {
        if (b[i] & computer_color)       /* computers piece (good) */
            {
            blanks--;
            co_pieces += 1;
            if (b[i] & KING)
                co_kings += 1;
            }
        else if (b[i] & next(computer_color)) /* opponents piece (bad) */
            {
            blanks--;
            op_pieces += 1;
            if (b[i] & KING)
                op_kings += 1;
            }
        }
    #ifndef SHIPPING_VERSION_REVIEW
    if (0==rConfig.iGameType)
    #endif
        {
        if (0==op_pieces) {AssertSz(co_pieces,"frogs"); return MAX_QUALITY; }
        if (0==co_pieces) {AssertSz(op_pieces,"sucker"); return MIN_QUALITY;}
        }

    /* ----- position of pieces ----- */
    if (0 != b[1]) {if (b[1] & computer_color)   q+=wDoubleCorner; else q-=wDoubleCorner;}
    if (0 != b[5]) {if (b[5] & computer_color)   q+=wDoubleCorner; else q-=wDoubleCorner;}
    if (0 != b[32]) {if (b[32] & computer_color) q+=wDoubleCorner; else q-=wDoubleCorner;}
    if (0 != b[28]) {if (b[28] & computer_color) q+=wDoubleCorner; else q-=wDoubleCorner;}

    if (0 != b[14]) {if (b[14] & computer_color) q+=wCenterPos; else q-=wCenterPos;}
    if (0 != b[19]) {if (b[19] & computer_color) q+=wCenterPos; else q-=wCenterPos;}
    if (0 != b[18]) {if (b[18] & computer_color) q+=wCenterPos; else q-=wCenterPos;}
    if (0 != b[15]) {if (b[15] & computer_color) q+=wCenterPos; else q-=wCenterPos;}

    if ((0 == (KING & b[1])) && 0 != b[1])   {if (b[1] & computer_color)  q+=wHoldingKingRow; else q-=wHoldingKingRow;}
    if ((0 == (KING & b[3])) && 0 != b[3])   {if (b[3] & computer_color)  q+=wHoldingKingRow; else q-=wHoldingKingRow;}
    if ((0 == (KING & b[30])) && 0 != b[30]) {if (b[30] & computer_color) q+=wHoldingKingRow; else q-=wHoldingKingRow;}
    if ((0 == (KING & b[32])) && 0 != b[32]) {if (b[32] & computer_color) q+=wHoldingKingRow; else q-=wHoldingKingRow;}

    if (0 != b[12]) {if (b[12] & computer_color) q+=wEdges; else q-=wEdges;}
    if (0 != b[13]) {if (b[13] & computer_color) q+=wEdges; else q-=wEdges;}
    if (0 != b[20]) {if (b[20] & computer_color) q+=wEdges; else q-=wEdges;}
    if (0 != b[21]) {if (b[21] & computer_color) q+=wEdges; else q-=wEdges;}

    /* ----- move theory ----- */
    #ifndef SHIPPING_VERSION_REVIEW
    pdebug(stddbg,"!!! WARNING this needs to be removed !!! %s(%d)\n",__FILE__,__LINE__); //REVIEW
    if (0!=rConfig.rPlayer[rGameState.iPlayerTurn].iUseMoveTheory)
    #endif
    
    if (co_pieces == op_pieces && 0==co_kings && 0==op_kings) //REVIEW consider move theory with kings
        {
        int base_of_system=0;
        if (BLACK == next(t)) base_of_system=4;
        for (;base_of_system < 32; base_of_system += 8)
            {
            if (b[1+base_of_system]) movetheory_piecesinsystem++;
            if (b[2+base_of_system]) movetheory_piecesinsystem++;
            if (b[3+base_of_system]) movetheory_piecesinsystem++;
            if (b[4+base_of_system]) movetheory_piecesinsystem++;
            }
        movetheory_piecesinsystem &= 1;
        if (next(t) == computer_color)
            {
            if (movetheory_piecesinsystem) /* computer has the move */
                {
                pdebug(stddbg,"computer has \"the move\" %s(%d)\n",__FILE__,__LINE__);
                q += wMoveTheory;
                }
            else                           /* opponent has the move */
                {
                pdebug(stddbg,"opponent has \"the move\" %s(%d)\n",__FILE__,__LINE__);
                q -= wMoveTheory;
                }
            }
        else
            {
            if (movetheory_piecesinsystem) /* opponent has the move */
                {
                pdebug(stddbg,"opponent has \"the move\" %s(%d)\n",__FILE__,__LINE__);
                q -= wMoveTheory;
                }
            else                           /* computer has the move */
                {
                pdebug(stddbg,"computer has \"the move\" %s(%d)\n",__FILE__,__LINE__);
                q += wMoveTheory;
                }
            }
        }

    #ifdef NEVER
    /* ----- book moves and memorized moves ----- */
    if (depth_maximum <= depth_bookmoves && depth_bookmoves)
        q += BoardInBook(b,t,co_pieces,op_pieces,co_kings,op_kings);
    #endif

    /* ----- is the game over? ----- */
    //REVIEW: should I consider this or is it too expensive?
    if (co_pieces < wFormationCutoff && op_pieces < co_pieces)
        {
        if (0 == co_pieces) return MIN_QUALITY;
        if (0 == op_pieces) return MAX_QUALITY;
        }

    /* ----- mobility (if everything is even so far) ----- */
    //REVIEW: should I consider this or is it too expensive?

    /* ----- formation of pieces ----- */
    //REVIEW: should I consider this or is it too expensive?
    if (co_pieces < wFormationCutoff && op_pieces < co_pieces)
        {
        int i=0,j=0,X1,Y1,X2,Y2;
        int distance=0;
        static int a[SQRS_MAX];

        for (i=0;i<SQRS_MAX;++i) a[i]=0;
        for (i=0;i<SQRS_MAX;++i)
            if (b[i]) a[j++]=i;

        for (i=0;i<co_pieces+op_pieces;++i)
            {
            Y1 = (a[i] - 1) / 4;
            X1 = ((a[i] - 1) % 4) * 2 + ((Y1 + 1) % 2);
            for (j=i+1;j<co_pieces+op_pieces;++j)
                {
                int t1,t2,t3,t4,t5;
                Y2 = (a[j] - 1) / 4;
                X2 = ((a[j] - 1) % 4) * 2 + ((Y2 + 1) % 2);
                t1 = X1-X2;
                t2 = Y1-Y2;
                t3 = t1 * t1;
                t4 = t2 * t2;
                t5 = t4 / 2; /* no sqrt op here for distance to save time */
                AssertSz(t5 >= 0,"saki");
                if (t5 > distance) distance = t5;
                //NEVER distance += t5;
                }
            }
        //NEVER distance /= co_pieces+op_pieces; /* average distance between pieces */
        AssertSz(distance >= 0, "sucky");
        AssertSz(distance <= 49, "barney is a white dog");

        /* what does this mean for quality? */
        long qf = 0;
        qf = ((-2*distance)/5)-10; /* qf is now 10 to -10 usually -7 */
        q += qf * wFormation;

        #ifdef NEVER
        static char buf[100];
        wsprintf(buf,"dis = %d  qf = %ld",distance,qf);
        MessageBox(NULL,buf,"quality.cpp",MB_OK);
        #endif
        }

    /* ----- multiply factors by weights (when necc.) ----- */
    q += (co_kings  * wKing);
    q -= (op_kings  * wKing);
    q += (co_pieces * wPiece);
    q -= (op_pieces * wPiece);
    q += (blanks    * wBlank);

    pdebug(stddbg,"QualityOfBoard=%ld %s(%d)\n",q,__FILE__,__LINE__);
    #ifndef SHIPPING_VERSION_REVIEW
    if (0==rConfig.iGameType) return q;
    if (1==rConfig.iGameType) return 100000-q; //REVIEW
    AssertSz(0,"what kind of game are you playing anyway?");
    #endif
    return q;
}
