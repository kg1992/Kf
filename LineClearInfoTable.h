#ifndef LINE_CLEAR_INFO_TABLE_H_
#define LINE_CLEAR_INFO_TABLE_H_

enum LineClearType : int
{
    LCT_Single,
    LCT_Mini_Tspin_Single,
    LCT_Double,
    LCT_Triple,
    LCT_Tspin_Single,
    LCT_Tetris,
    LCT_Tspin_Double,
    LCT_Tspin_Triple,
    LCT_End,
};

struct LineClearInfo
{
    LineClearType type;
    int score;
    bool difficult;
    const char* msg;
    int sendingLine;
};

const LineClearInfo LineClearInfoTable[LCT_End] =
{
    {LCT_Single,100, false, "Single", 0},
    {LCT_Mini_Tspin_Single, 200, true, "mini Tspin Single", 0},
    {LCT_Double, 300, false, "Double", 1},
    {LCT_Triple, 500, false, "Triple", 2},
    {LCT_Tspin_Single, 800, true, "Tspin Single", 2},
    {LCT_Tetris, 800, true, "Tetris", 4},
    {LCT_Tspin_Double, 1200, true, "Tspin Double", 4},
    {LCT_Tspin_Triple, 1600, true, "Tspin Triple", 6},
};

LineClearType DetectLineClearType(int lineClearCount, bool tspin, bool minitspin);

#endif
