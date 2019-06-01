#ifndef TETRIMINO_H_
#define TETRIMINO_H_

#include <iostream>

enum TetriminoType : int
{
    TT_I, TT_O, TT_T, TT_S, TT_Z, TT_J, TT_L, TT_End
};

enum Orientation : int
{
    O_North, O_East, O_South, O_West, O_End
};

const int PatternSize = 16;

const int TetriminoPatterns[][O_End][PatternSize] =
{
{
    {
        0,0,0,0,
        1,1,1,1,
        0,0,0,0,
        0,0,0,0,
    },
    {
        0,0,1,0,
        0,0,1,0,
        0,0,1,0,
        0,0,1,0,
    },
    {
        0,0,0,0,
        0,0,0,0,
        1,1,1,1,
        0,0,0,0,
    },
    {
        0,1,0,0,
        0,1,0,0,
        0,1,0,0,
        0,1,0,0,
    }
}

,

{
    {
        0,0,0,0,
        0,1,1,0,
        0,1,1,0,
        0,0,0,0,
    },
    {
        0,0,0,0,
        0,1,1,0,
        0,1,1,0,
        0,0,0,0,
    },
    {
        0,0,0,0,
        0,1,1,0,
        0,1,1,0,
        0,0,0,0,
    },
    {
        0,0,0,0,
        0,1,1,0,
        0,1,1,0,
        0,0,0,0,
    },
}
,
{
    {
        0,1,0,0,
        1,1,1,0,
        0,0,0,0,
        0,0,0,0,
    },
    {
        0,1,0,0,
        0,1,1,0,
        0,1,0,0,
        0,0,0,0,
    },
    {
        0,0,0,0,
        1,1,1,0,
        0,1,0,0,
        0,0,0,0,
    },
    {
        0,1,0,0,
        1,1,0,0,
        0,1,0,0,
        0,0,0,0,
    },
}
,
{
    {
        0,1,1,0,
        1,1,0,0,
        0,0,0,0,
        0,0,0,0,
    },
    {
        0,1,0,0,
        0,1,1,0,
        0,0,1,0,
        0,0,0,0,
    },
    {
        0,0,0,0,
        0,1,1,0,
        1,1,0,0,
        0,0,0,0,
    },
    {
        1,0,0,0,
        1,1,0,0,
        0,1,0,0,
        0,0,0,0,
    },
}
,
{
    {
        1,1,0,0,
        0,1,1,0,
        0,0,0,0,
        0,0,0,0,
    },
    {
        0,0,1,0,
        0,1,1,0,
        0,1,0,0,
        0,0,0,0,
    },
    {
        0,0,0,0,
        1,1,0,0,
        0,1,1,0,
        0,0,0,0,
    },
    {
        0,1,0,0,
        1,1,0,0,
        1,0,0,0,
        0,0,0,0,
    },
}
,
{
    {
        1,0,0,0,
        1,1,1,0,
        0,0,0,0,
        0,0,0,0,
    },
    {
        0,1,1,0,
        0,1,0,0,
        0,1,0,0,
        0,0,0,0,
    },
    {
        0,0,0,0,
        1,1,1,0,
        0,0,1,0,
        0,0,0,0,
    },
    {
        0,1,0,0,
        0,1,0,0,
        1,1,0,0,
        0,0,0,0,
    },
}
,
{
    {
        0,0,1,0,
        1,1,1,0,
        0,0,0,0,
        0,0,0,0,
    },
    {
        0,1,0,0,
        0,1,0,0,
        0,1,1,0,
        0,0,0,0,
    },
    {
        0,0,0,0,
        1,1,1,0,
        1,0,0,0,
        0,0,0,0,
    },
    {
        1,1,0,0,
        0,1,0,0,
        0,1,0,0,
        0,0,0,0,
    },
}

};

void MinoPatternIndexToXY(int ox, int oy, int i, int& mx, int& my);

// Tetrimino structure for tetrimino that is controlled by the player
struct Tetrimino
{
public:
    int x, y;
    TetriminoType type;
    Orientation orientation;

    void IncreaseOrientation();

    void DecreaseOrientation();

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);
};

#endif
