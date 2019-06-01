#include "KickTable.h"

int KickTable::GetKickTestSetIndex(TetriminoType type, Orientation orientation, bool clockwise)
{
    if (orientation == O_North)
    {
        if (clockwise) return OT_NorthToEast;
        else return OT_NorthToWest;
    }
    if (orientation == O_East)
    {
        if (clockwise) return OT_EastToSouth;
        else return OT_EastToNorth;
    }
    if (orientation == O_South)
    {
        if (clockwise) return OT_SouthToWest;
        else return OT_WestToSouth;
    }
    if (orientation == O_West)
    {
        if (clockwise) return OT_WestToNorth;
        else return OT_WestToSouth;
    }

    return OT_End;
}

KickTestSet KickTable::GetKickTestSet(TetriminoType tt, int index)
{
    static const KickTestSet KickTable1[] =
    {
        /* 0 >> 1 */ {{{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}}},
        /* 1 >> 0 */ {{{0,0},{1,0},{1,-1},{0,2},{1,2},}},
        /* 1 >> 2 */ {{{0,0},{1,0},{1,-1},{0,2},{1,2},}},
        /* 2 >> 1 */ {{{0,0},{-1,0},{-1,1},{0,-2},{-1,-2},}},
        /* 2 >> 3 */ {{{0,0},{1,0},{1,1},{0,-2},{1,-2},}},
        /* 3 >> 2 */ {{{0,0},{-1,0},{-1,-1},{0,2},{-1,2},}},
        /* 3 >> 0 */ {{{0,0},{-1,0},{-1,-1},{0,2},{-1,2},}},
        /* 0 >> 3 */ {{{0,0},{1,0},{1,1},{0,-2},{1,-2},}},
    };

    static const KickTestSet KickTable2[] =
    {
        /* 0 >> 1 */ {{{0,0},{-2,0},{1,0},{-2,-1},{1,2}}},
        /* 1 >> 0 */ {{{0,0},{2,0},{-1,0},{2,1},{-1,-2}}},
        /* 1 >> 2 */ {{{0,0},{-1,0},{2,0},{-1,2},{2,-1}}},
        /* 2 >> 1 */ {{{0,0},{1,0},{-2,0},{1,-2},{-2,1}}},
        /* 2 >> 3 */ {{{0,0},{2,0},{-1,0},{2,1},{-1,-2}}},
        /* 3 >> 2 */ {{{0,0},{-2,0},{1,0},{-2,-1},{1,2}}},
        /* 3 >> 0 */ {{{0,0},{1,0},{-2,0},{1,-2},{-2,1}}},
        /* 0 >> 3 */ {{{0,0},{-1,0},{2,0},{-1,2},{2,-1}}},
    };

    const KickTestSet* table = (tt == TT_I) ? KickTable2 : KickTable1;
    return table[index];
}