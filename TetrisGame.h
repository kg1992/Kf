// ref : https://harddrop.com/wiki/Spawn_Location
// ref : https://tetris.fandom.com/wiki/Playfield
// ref : https://tetris.fandom.com/wiki/Tetromino
// ref : https://tetris.fandom.com/wiki/SRS
#ifndef TETRIS_GAME_H_
#define TETRIS_GAME_H_

#include <cinttypes>
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <SDL.h>

enum TetriminoType : int
{
    TT_I, TT_O, TT_T, TT_S, TT_Z, TT_J, TT_L, TT_End
};

enum Orientation : int
{
    O_North, O_East, O_South, O_West, O_End
};

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

enum Block : std::uint8_t
{
    Empty,
    B_Red,
    B_Green,
    B_Blue,
    B_Cyan,
    B_Magenta,
    B_Yellow,
    B_Orange,

    B_End
};

enum PlayFieldMetric : int
{
    Width = 10,
    Height = 24,
    BlockCount = Width * Height,

    PxBlockSize = 16,
    PxNextPeekWindowSize = 64,
    PxNextLocationX = 176,
    PxNextLocationY = 320,
};

enum PlayState : int
{
    PS_Ready,
    PS_Control,
    PS_Dropped,
    PS_GameOver,
    PS_Win,
    PS_End,
};

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
    static int GetKickTestSetIndex(TetriminoType type, Orientation orientation, bool clockwise)
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

    static KickTestSet GetKickTestSet(TetriminoType tt, int index)
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
};

// Tetrimino spawn location X in play field space
const int SpawnX = 3;
// Tetrimino spawn location Y in play field space
const int SpawnY = 21;


// Tetrimino structure for tetrimino that is controlled by the player
struct Tetrimino
{
public:
    int x, y;
    TetriminoType type;
    Orientation orientation;
    Block block;

    void IncreaseOrientation()
    {
        orientation = static_cast<Orientation>((orientation + 1) % Orientation::O_End);
    }

    void DecreaseOrientation()
    {
        orientation = static_cast<Orientation>((orientation + Orientation::O_End - 1) % Orientation::O_End);
    }

    void Serialize(std::ostream& os)
    {
        os.write(reinterpret_cast<char*>(&x), sizeof(x));
        os.write(reinterpret_cast<char*>(&y), sizeof(y));
        os.write(reinterpret_cast<char*>(&type), sizeof(type));
        os.write(reinterpret_cast<char*>(&orientation), sizeof(orientation));
        os.write(reinterpret_cast<char*>(&block), sizeof(block));
    }

    void Deserialize(std::istream& is)
    {
        is.read(reinterpret_cast<char*>(&x), sizeof(x));
        is.read(reinterpret_cast<char*>(&y), sizeof(y));
        is.read(reinterpret_cast<char*>(&type), sizeof(type));
        is.read(reinterpret_cast<char*>(&orientation), sizeof(orientation));
        is.read(reinterpret_cast<char*>(&block), sizeof(block));
    }
};

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
    return Block::Empty;
}

Tetrimino MakeTetrimino(TetriminoType type)
{
    Tetrimino tm;
    tm.x = SpawnX;
    tm.y = SpawnY;
    tm.type = type;
    tm.orientation = O_North;
    tm.block = TetriminoTypeToBlock(type);
    return tm;
}

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

int XyToIndex(int x, int y)
{
    return x + y * PlayFieldMetric::Width;
}

void MinoPatternIndexToXY(Tetrimino& tm, int i, int& mx, int& my)
{
    const int BoundingBoxSideLength = 4;
    int dy = i / BoundingBoxSideLength;
    int dx = i % BoundingBoxSideLength;
    mx = tm.x + dx;
    my = tm.y - dy;
}

class PlayField
{
public:
    PlayField()
        : m_playField{ Block::Empty }
    {
    }

    bool CheckXyOverlap(int x, int y)
    {
        return x < 0 || PlayFieldMetric::Width <= x
            || y < 0 || PlayFieldMetric::Height <= y
            || m_playField[XyToIndex(x, y)] != Empty;
    }

    bool CheckOverlap(Tetrimino& tm)
    {
        const int* pattern = TetriminoPatterns[tm.type][tm.orientation];
        for (int i = 0; i < PatternSize; ++i)
        {
            if (pattern[i])
            {
                int mx, my;
                MinoPatternIndexToXY(tm, i, mx, my);
                if (CheckXyOverlap(mx, my))
                    return true;
            }
        }
        return false;
    }

    bool CheckDownContact(Tetrimino& tm)
    {
        const int* pattern = TetriminoPatterns[tm.type][tm.orientation];
        for (int i = 0; i < PatternSize; ++i)
        {
            if (pattern[i])
            {
                int mx, my;
                MinoPatternIndexToXY(tm, i, mx, my);
                if (CheckXyOverlap(mx, my - 1))
                    return true;
            }
        }
        return false;
    }

    bool TestThreeCorner(Tetrimino tm)
    {
        int cornerCount = 0;
        if (m_playField[XyToIndex(tm.x, tm.y)] != Block::Empty) ++cornerCount;
        if (m_playField[XyToIndex(tm.x + 2, tm.y)] != Block::Empty) ++cornerCount;
        if (m_playField[XyToIndex(tm.x + 2, tm.y - 2)] != Block::Empty) ++cornerCount;
        if (m_playField[XyToIndex(tm.x, tm.y - 2)] != Block::Empty) ++cornerCount;
        return cornerCount >= 3;
    }

    bool TestThreeHighWallSide(int x, int y)
    {
        return
            CheckXyOverlap(x, y) &&
            CheckXyOverlap(x, y - 1) &&
            CheckXyOverlap(x, y - 2);
    }

    void ClearPlayfield()
    {
        for (int i = 0; i < PlayFieldMetric::BlockCount; ++i)
            m_playField[i] = Block::Empty;
    }

    void CopyPlayfield(int* board)
    {
        for (int y = 0; y < PlayFieldMetric::Height; ++y)
        {
            for (int x = 0; x < PlayFieldMetric::Width; ++x)
            {
                int b = board[XyToIndex(x, PlayFieldMetric::Height - y - 1)];
                m_playField[XyToIndex(x, y)] = b ? Block::B_Red : Block::Empty;
            }
        }
    }

    Block Get(int x, int y)
    {
        return m_playField[XyToIndex(x, y)];
    }

    void Set(int x, int y, Block block)
    {
        m_playField[XyToIndex(x, y)] = block;
    }

    bool LockTetrimino(Tetrimino& tm)
    {
        bool overwrite = false;
        const int* pattern = TetriminoPatterns[tm.type][tm.orientation];
        for (int i = 0; i < PatternSize; ++i)
        {
            if (pattern[i])
            {
                int mx, my;
                MinoPatternIndexToXY(tm, i, mx, my);
                int index = XyToIndex(mx, my);
                if (m_playField[index] != Empty)
                    overwrite = true;
                m_playField[index] = tm.block;
            }
        }
        return overwrite;
    }

    void Serialize(std::ostream& os)
    {
        os.write(reinterpret_cast<const char*>(m_playField), sizeof(m_playField));
    }

    void Deserialize(std::istream& is)
    {
        is.read(reinterpret_cast<char*>(m_playField), sizeof(m_playField));
    }

private:
    Block m_playField[PlayFieldMetric::BlockCount];
};

class Randomizer
{
public:
    TetriminoType Peek(int step)
    {
        return step < static_cast<int>(m_queue.size())
            ? *(m_queue.rbegin() + step)
            : TT_End;
    }

    TetriminoType PopNext()
    {
        while (m_queue.size() < 7)
        {
            FillQueue(m_queue);
        }
        TetriminoType type = m_queue.back();
        m_queue.pop_back();
        return type;
    }

    void FillQueue(std::vector<TetriminoType>& tmQueue)
    {
        std::vector<TetriminoType> bag =
        {
            TT_I, TT_O, TT_T, TT_S, TT_Z, TT_J, TT_L
        };

        while (!bag.empty())
        {
            int i = m_rng() % bag.size();
            tmQueue.insert(tmQueue.begin(), bag[i]);
            bag.erase(bag.begin() + i);
        }
    }

    void Serialize(std::ostream& os)
    {
        std::size_t queueSize = m_queue.size();
        os.write(reinterpret_cast<char*>(&queueSize), sizeof(queueSize));
        os.write(reinterpret_cast<char*>(m_queue.data()), static_cast<std::streamsize>(sizeof(m_queue[0])) * queueSize);

        std::stringstream ss;
        ss << m_rng;
        std::string srng = ss.str();
        int srngSize = srng.size();
        os.write(reinterpret_cast<char*>(&srngSize), sizeof(srngSize));
        os.write(srng.c_str(), srngSize);
    }

    void Deserialize(std::istream& is)
    {
        std::size_t queueSize = 0;
        is.read(reinterpret_cast<char*>(&queueSize), sizeof(queueSize));
        m_queue.resize(queueSize);
        is.read(reinterpret_cast<char*>(m_queue.data()), static_cast<std::streamsize>(sizeof(m_queue[0])) * queueSize);


        int srngSize = 0;
        is.read(reinterpret_cast<char*>(&srngSize), sizeof(srngSize));
        std::vector<char> vrng;
        vrng.resize(srngSize);
        is.read(vrng.data(), srngSize);
        std::string srng(vrng.begin(), vrng.end());
        std::stringstream ss(srng);
        ss >> m_rng;
    }

    void Reset()
    {
        m_queue.clear();
    }

private:
    std::vector<TetriminoType> m_queue;
    std::minstd_rand m_rng;
};

struct RotateResult
{
    bool success;
    bool tspin;
    bool minitspin;
};

class ScoreKeeper
{
public:
    enum { ComboReward = 50 };
    ScoreKeeper()
        : m_score(0)
    {
    }

    void ResetScore()
    {
        m_score = 0;
    }

    void OnLineClear(LineClearType lct, bool backToBackChance, int combo, int level)
    {
        const LineClearInfo& lci = LineClearInfoTable[lct];

        int addScore = lci.score;

        if (lci.difficult && backToBackChance)
            addScore += addScore / 2;

        addScore += ComboReward * combo;
        addScore *= level;
        m_score += addScore;
    }

    void OnSoftDrop()
    {
        m_score += 1;
    }

    void OnHardDrop()
    {
        m_score += 2;
    }

    int GetScore()
    {
        return m_score;
    }

    void Serialize(std::ostream& os)
    {
        os.write(reinterpret_cast<char*>(&m_score), sizeof(m_score));
    }

    void Deserialize(std::istream& is)
    {
        is.read(reinterpret_cast<char*>(&m_score), sizeof(m_score));
    }

private:
    int m_score;
};

class TetrisGame
{
public:
    void Reset()
    {
        level = 1;
        hold = TT_End;
        randomizer.Reset();
        scoreKeeper.ResetScore();
        totalClearedLines = 0;
        playField.ClearPlayfield();
        playState = PS_Ready;
    }

    void SaveState(std::ostream& ofs)
    {
        tmActive.Serialize(ofs);
        playField.Serialize(ofs);
        randomizer.Serialize(ofs);
        scoreKeeper.Serialize(ofs);
        ofs.write(reinterpret_cast<char*>(&level), sizeof(level));
        ofs.write(reinterpret_cast<char*>(&hold), sizeof(hold));
    }

    void LoadState(std::istream& ifs)
    {
        tmActive.Deserialize(ifs);
        playField.Deserialize(ifs);
        randomizer.Deserialize(ifs);
        scoreKeeper.Deserialize(ifs);
        ifs.read(reinterpret_cast<char*>(&level), sizeof(level));
        ifs.read(reinterpret_cast<char*>(&hold), sizeof(hold));
    }

    void ToggleGravity()
    {
        gravity = !gravity;
    }

    void HardDrop()
    {
        if (playState == PS_Control)
        {
            while (!playField.CheckDownContact(tmActive))
            {
                --tmActive.y;
                scoreKeeper.OnHardDrop();
                lastRotateResult = { false, false, false };
            }
            playState = LockAndSelectState(tmActive);
        }
    }

    void StartGame()
    {
        if (playState == PS_Ready)
        {
            playState = PS_Dropped;
        }
    }

    void RotateAntiClockwise()
    {
        if (!playField.CheckOverlap(tmActive))
            lastRotateResult = RotateTetrimino(tmActive, false);
        if (lastRotateResult.success)
            lockDelayTick = SDL_GetTicks();
    }

    void RotateClockwise()
    {
        if (!playField.CheckOverlap(tmActive))
        {
            lastRotateResult = RotateTetrimino(tmActive, true);
            if (lastRotateResult.success)
                lockDelayTick = SDL_GetTicks();
        }
    }

    void Hold()
    {
        if (playState == PS_Control && 0 < holdChance)
        {
            --holdChance;

            Tetrimino next;
            if (hold != TetriminoType::TT_End)
            {
                next = MakeTetrimino(hold);
            }
            else
            {
                next = SpawnNext(randomizer);
            }

            hold = tmActive.type;

            tmActive = next;

            gravityTick = SDL_GetTicks();
        }
    }

    void Restart()
    {
        if (playState == PS_GameOver)
        {
            Reset();
        }
    }

    PlayState GetPlayState()
    {
        return playState;
    }

    struct DropResult
    {
        int lineClearCount;
        LineClearType lct;
        bool backToBack;
        bool levelUp;
    };

    DropResult OnDrop()
    {
        DropResult result = { 0,LCT_End, false, false };

        const int MaximumLineClearCount = 4;
        int clearedLines[MaximumLineClearCount] = { PlayFieldMetric::Height };
        int lineClearCount = 0;

        // Count and remember cleared lines
        for (int y = 0; y < PlayFieldMetric::Height; ++y)
        {
            bool lineComplete = true;
            for (int x = 0; x < PlayFieldMetric::Width; ++x)
            {
                Block block = playField.Get(x, y);
                if (block == Empty)
                {
                    lineComplete = false;
                    break;
                }
            }

            // Complete line found
            if (lineComplete)
                clearedLines[lineClearCount++] = y;
        }

        result.lct = DetectLineClearType(lineClearCount, lastRotateResult.tspin, lastRotateResult.minitspin);
        LineClearInfo lci = LineClearInfoTable[result.lct];

        if (lineClearCount)
        {
            // Rearrange cleared lines
            for (int y = 1; y < PlayFieldMetric::Height; ++y)
            {
                int dy = 0;
                bool isClearedLine = false;
                for (int i = 0; i < lineClearCount; ++i)
                {
                    if (clearedLines[i] == y)
                    {
                        isClearedLine = true;
                        break;
                    }
                    if (clearedLines[i] < y)
                    {
                        ++dy;
                    }
                }
                if (isClearedLine && y == PlayFieldMetric::Height - 1)
                {
                    for (int x = 0; x < PlayFieldMetric::Width; ++x)
                    {
                        playField.Set(x, y, Block::Empty);
                    }
                }
                if (!isClearedLine)
                {
                    for (int x = 0; x < PlayFieldMetric::Width; ++x)
                    {
                        playField.Set(x, y - dy, playField.Get(x, y));
                    }
                }
            }

            scoreKeeper.OnLineClear(result.lct, backToBackChance, combo, level);

            result.backToBack = backToBackChance && lci.difficult;

            backToBackChance = lci.difficult;

            ++combo;
        }
        else // lineClearCount == 0
        {
            combo = 0;
        }

        holdChance = 1;

        totalClearedLines += lineClearCount;

        int newLevel = (totalClearedLines / LevelUpStep) + 1;
        if (level != newLevel)
        {
            level = newLevel;
            result.levelUp = true;
        }
        unsigned deltaGravityDelay = (level - 1) * 50;
        if (deltaGravityDelay < initialGravityDelay)
        {
            gravityDelay = initialGravityDelay - deltaGravityDelay;
        }
        else
        {
            gravityDelay = 50;
        }

        tmActive = SpawnNext(randomizer);

        gravityTick = SDL_GetTicks();
        lockDelayTick = SDL_GetTicks();

        lastRotateResult = { false,false,false };

        playState = PS_Control;

        result.lineClearCount = lineClearCount;

        return result;
    }

    int GetCombo()
    {
        return combo;
    }

    int GetLevel()
    {
        return level;
    }

    void OnControl()
    {
        if (gravity)
        {
            if (playField.CheckDownContact(tmActive))
            {
                if (SDL_GetTicks() - lockDelayTick > LockDelay)
                {
                    playState = LockAndSelectState(tmActive);
                }
            }
            else
            {
                if (static_cast<int>(SDL_GetTicks() - gravityTick) > gravityDelay)
                {
                    --tmActive.y;
                    gravityTick = SDL_GetTicks();
                    lockDelayTick = SDL_GetTicks();
                    lastRotateResult = { false, false, false };
                }
            }
        }
    }

    void SoftDrop()
    {
        if (SDL_GetTicks() - softDropDelayTick > softDropDelay)
        {
            if (!playField.CheckDownContact(tmActive))
            {
                --tmActive.y;
                gravityTick = SDL_GetTicks();
                softDropDelayTick = SDL_GetTicks();
                lockDelayTick = SDL_GetTicks();
                scoreKeeper.OnSoftDrop();
            }
        }
    }

    void Shift(int dx)
    {
        if (dx != 0)
        {
            if (consecutiveMoveCount == 0 || SDL_GetTicks() - repeatDelayTick > RepeatDelay)
            {
                if (SDL_GetTicks() - moveDelayTick > MoveDelay)
                {
                    if (playField.CheckDownContact(tmActive))
                    {
                        gravityTick = SDL_GetTicks();
                    }
                    tmActive.x += dx;
                    if (playField.CheckOverlap(tmActive))
                    {
                        tmActive.x -= dx;
                    }
                    else
                    {
                        lastRotateResult = { false,false,false };
                        lockDelayTick = SDL_GetTicks();
                    }
                    moveDelayTick = SDL_GetTicks();
                    ++consecutiveMoveCount;
                }
            }
        }
        else
        {
            consecutiveMoveCount = 0;
            repeatDelayTick = SDL_GetTicks();
        }
    }

    PlayField& GetPlayField()
    {
        return playField;
    }

    Randomizer& GetRandomizer()
    {
        return randomizer;
    }

    Tetrimino& GetActiveTetrimino()
    {
        return tmActive;
    }

    TetriminoType GetHold()
    {
        return hold;
    }

    int GetTotalClearedLines()
    {
        return totalClearedLines;
    }

    int GetScore()
    {
        return scoreKeeper.GetScore();
    }

    void Win()
    {
        playState = PS_Win;
    }

private:
    int totalClearedLines = 0;
    int level = 1;
    ScoreKeeper scoreKeeper;
    Randomizer randomizer;
    RotateResult lastRotateResult = { false, false, false };

    TetriminoType hold = TetriminoType::TT_End;
    int holdChance = 1;
    int combo = 0;
    bool backToBackChance = false;

    Tetrimino tmActive = { 0,0,TT_End,O_End,B_End };
    Tetrimino tmGhost = { 0,0,TT_End,O_End,B_End };
    const unsigned int initialGravityDelay = 500;
    const unsigned int LockDelay = 500;
    const unsigned int LevelUpStep = 10;
    const unsigned int RepeatDelay = 220;
    const unsigned int MoveDelay = 50;
    unsigned int repeatDelayTick = 0;
    unsigned int softDropDelay = 50;
    unsigned int softDropDelayTick = 0;
    unsigned int gravityDelay = 500;
    unsigned int moveDelayTick = 0;
    unsigned int consecutiveMoveCount = 0;
    unsigned int gravityTick = 0;
    unsigned int lockDelayTick = 0;

    PlayState playState = PS_Ready;
    PlayField playField;

    bool gravity = true;

    PlayState LockAndSelectState(Tetrimino& tm)
    {
        bool overwrite = playField.LockTetrimino(tm);
        return !overwrite ? PS_Dropped : PS_GameOver;
    }

    Tetrimino SpawnNext(Randomizer& randomizer)
    {
        TetriminoType tmType = randomizer.PopNext();
        return MakeTetrimino(tmType);
    }

    RotateResult RotateTetrimino(Tetrimino& tm, bool clockwise)
    {
        int kickTableIndex = KickTable::GetKickTestSetIndex(tm.type, tm.orientation, clockwise);
        if (kickTableIndex == OT_End) // This case should never happen
            return { false, false, false };

        KickTestSet ks = KickTable::GetKickTestSet(tm.type, kickTableIndex);
        Tetrimino next = tm;
        if (clockwise) next.IncreaseOrientation();
        else next.DecreaseOrientation();

        bool tspin = false;
        bool minitspin = false;

        for (int i = 0; i < _countof(ks.tests); ++i)
        {
            Kick kick = ks.tests[i];
            next.x = tm.x + kick.dx;
            next.y = tm.y + kick.dy;
            if (!playField.CheckOverlap(next))
            {
                if (tm.type == TT_T)
                {
                    // 3-corner tspin chance
                    if (i == 0) tspin = playField.TestThreeCorner(next);
                    switch (kickTableIndex)
                    {
                    case OT_NorthToEast:
                        if (i == 1) minitspin = playField.TestThreeHighWallSide(next.x, next.y);
                        if (i >= 3) tspin = true;
                        break;

                    case OT_EastToNorth:
                        if (i == 2) minitspin = playField.CheckXyOverlap(next.x + 2, next.y - 2);
                        break;

                    case OT_EastToSouth:
                        if (i == 2) tspin = true;
                        break;

                    case OT_WestToSouth:
                        if (i == 2) tspin = true;
                        break;

                    case OT_WestToNorth:
                        if (i == 2) minitspin = playField.CheckXyOverlap(next.x, next.y - 2);
                        break;

                    case OT_NorthToWest:
                        if (i == 1) minitspin = playField.TestThreeHighWallSide(next.x + 2, next.y);
                        if (i >= 3) tspin = true;
                        break;
                    }
                }

                tm = next;
                return {
                    true,tspin,minitspin
                };
            }
        }

        return {
            false, false, false
        };
    }
};

#endif
