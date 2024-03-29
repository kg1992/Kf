#ifndef PLAY_FIELD_H_
#define PLAY_FIELD_H_

#include "Tetrimino.h"

enum Block
{
    B_Empty,
    B_Red,
    B_Green,
    B_Blue,
    B_Cyan,
    B_Magenta,
    B_Yellow,
    B_Orange,
    B_Gray,

    B_End
};

Block TetriminoTypeToBlock(TetriminoType type);

struct PlayFieldDrawMetrics
{
    int pxOriginX;
    int pxOriginY;
    int pxBlockSize = 16;
};

class PlayField
{
public:
    PlayField(int width, int height, int spawnX, int spawnY);

    bool CheckXyOverlap(int x, int y);

    bool CheckOverlap(Tetrimino& tm);

    bool CheckDownContact(Tetrimino& tm);

    bool TestThreeCorner(Tetrimino tm);

    bool TestThreeHighWallSide(int x, int y);

    void ClearPlayfield();

    void CopyPlayfield(int* board);

    Block Get(int x, int y);

    void Set(int x, int y, Block block);

    bool LockTetrimino(Tetrimino& tm);

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

    int XyToIndex(int x, int y);

    int GetBlockCount();

    int GetWidth();

    int GetHeight();

    void ClearRow(int y);

    int GetSpawnX();

    int GetSpawnY();

    void CopyLine(int srcy, int dsty);

private:
    int m_width, m_height;
    int m_spawnX, m_spawnY;
    std::vector<Block> m_playField;
};

#endif
