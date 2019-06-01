#ifndef KICK_TABLE_H_
#define KICK_TABLE_H_

#include "Tetrimino.h"

enum OrientationTurn : int
{
    OT_NorthToEast,
    OT_EastToNorth,
    OT_EastToSouth,
    OT_SouthToEast,
    OT_SouthToWest,
    OT_WestToSouth,
    OT_WestToNorth,
    OT_NorthToWest,
    OT_End
};

struct Kick
{
    int dx, dy;
};

struct KickTestSet
{
    enum { TestSetSize = 5 };
    Kick tests[TestSetSize];
};

class KickTable
{
public:
    static int GetKickTestSetIndex(TetriminoType type, Orientation orientation, bool clockwise);

    static KickTestSet GetKickTestSet(TetriminoType tt, int index);
};

#endif
