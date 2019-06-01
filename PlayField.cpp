#include "PlayField.h"
#include "Tetrimino.h"

Block TetriminoTypeToBlock(TetriminoType type)
{
    switch (type)
    {
    case TetriminoType::TT_I: return Block::B_Cyan;
    case TetriminoType::TT_O: return Block::B_Yellow;
    case TetriminoType::TT_T: return Block::B_Magenta;
    case TetriminoType::TT_S: return Block::B_Green;
    case TetriminoType::TT_Z: return Block::B_Red;
    case TetriminoType::TT_J: return Block::B_Blue;
    case TetriminoType::TT_L: return Block::B_Orange;
    }
    return Block::B_Empty;
}

PlayField::PlayField(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_playField(width* height, B_Empty)
{
}

bool PlayField::CheckXyOverlap(int x, int y)
{
    return x < 0 || m_width <= x
        || y < 0 || m_height <= y
        || m_playField[XyToIndex(x, y)] != B_Empty;
}

bool PlayField::CheckOverlap(Tetrimino& tm)
{
    const int* pattern = TetriminoPatterns[tm.type][tm.orientation];
    for (int i = 0; i < PatternSize; ++i)
    {
        if (pattern[i])
        {
            int mx, my;
            MinoPatternIndexToXY(tm.x, tm.y, i, mx, my);
            if (CheckXyOverlap(mx, my))
                return true;
        }
    }
    return false;
}

bool PlayField::CheckDownContact(Tetrimino& tm)
{
    const int* pattern = TetriminoPatterns[tm.type][tm.orientation];
    for (int i = 0; i < PatternSize; ++i)
    {
        if (pattern[i])
        {
            int mx, my;
            MinoPatternIndexToXY(tm.x, tm.y, i, mx, my);
            if (CheckXyOverlap(mx, my - 1))
                return true;
        }
    }
    return false;
}

bool PlayField::TestThreeCorner(Tetrimino tm)
{
    int cornerCount = 0;
    if (m_playField[XyToIndex(tm.x, tm.y)] != Block::B_Empty) ++cornerCount;
    if (m_playField[XyToIndex(tm.x + 2, tm.y)] != Block::B_Empty) ++cornerCount;
    if (m_playField[XyToIndex(tm.x + 2, tm.y - 2)] != Block::B_Empty) ++cornerCount;
    if (m_playField[XyToIndex(tm.x, tm.y - 2)] != Block::B_Empty) ++cornerCount;
    return cornerCount >= 3;
}

bool PlayField::TestThreeHighWallSide(int x, int y)
{
    return
        CheckXyOverlap(x, y) &&
        CheckXyOverlap(x, y - 1) &&
        CheckXyOverlap(x, y - 2);
}

void PlayField::ClearPlayfield()
{
    for (int i = 0; i < GetBlockCount(); ++i)
        m_playField[i] = Block::B_Empty;
}

void PlayField::CopyPlayfield(int* board)
{
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            int b = board[XyToIndex(x, m_height - y - 1)];
            m_playField[XyToIndex(x, y)] = b ? Block::B_Red : Block::B_Empty;
        }
    }
}

Block PlayField::Get(int x, int y)
{
    return m_playField[XyToIndex(x, y)];
}

void PlayField::Set(int x, int y, Block block)
{
    m_playField[XyToIndex(x, y)] = block;
}

bool PlayField::LockTetrimino(Tetrimino& tm)
{
    bool overwrite = false;
    const int* pattern = TetriminoPatterns[tm.type][tm.orientation];
    for (int i = 0; i < PatternSize; ++i)
    {
        if (pattern[i])
        {
            int mx, my;
            MinoPatternIndexToXY(tm.x, tm.y, i, mx, my);
            int index = XyToIndex(mx, my);
            if (m_playField[index] != B_Empty)
                overwrite = true;
            m_playField[index] = TetriminoTypeToBlock(tm.type);
        }
    }
    return overwrite;
}

void PlayField::Serialize(std::ostream& s)
{
    std::size_t count = m_playField.size();
    s.write(reinterpret_cast<char*>(&count), sizeof(count));
    s.write(reinterpret_cast<char*>(m_playField.data()), count * sizeof(m_playField[0]));
}

void PlayField::Deserialize(std::istream& s)
{
    std::size_t count = 0;
    s.read(reinterpret_cast<char*>(&count), sizeof(count));
    s.read(reinterpret_cast<char*>(m_playField.data()), count * sizeof(m_playField[0]));
}

int PlayField::XyToIndex(int x, int y)
{
    return x + y * m_width;
}

int PlayField::GetBlockCount()
{ 
    return m_playField.size(); 
}

int PlayField::GetWidth()
{
    return m_width;
}

int PlayField::GetHeight()
{
    return m_height;
}
