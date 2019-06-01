#include "LineClearInfoTable.h"

LineClearType DetectLineClearType(int lineClearCount, bool tspin, bool minitspin)
{
    if (lineClearCount == 1)
    {
        // * T-spin single
        if (tspin) return LCT_Tspin_Single;

        // * mini T-spin single
        if (minitspin) return LCT_Mini_Tspin_Single;

        // * Single
        return LCT_Single;
    }
    else if (lineClearCount == 2)
    {
        // * T-spin double
        if (tspin) return LCT_Tspin_Double;

        // * Double
        return LCT_Double;
    }
    else if (lineClearCount == 3)
    {
        // * T-spin triple
        if (tspin) return LCT_Tspin_Triple;

        // * Triple
        return LCT_Triple;
    }
    else if (lineClearCount == 4)
    {
        // * Tetris
        return LCT_Tetris;
    }

    return LCT_End;
};
