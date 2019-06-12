#include "Pch.h"
#include "TetrisGame.h"

Tetrimino MakeTetrimino(TetriminoType type, int x, int y)
{
    Tetrimino tm;
    tm.x = x;
    tm.y = y;
    tm.type = type;
    tm.orientation = O_North;
    return tm;
}

TetrisGame::TetrisGame(int playFieldWidth, int playFieldHeight, int spawnX, int spawnY)
    : m_playField(playFieldWidth, playFieldHeight, spawnX, spawnY)
    , m_activeTetrimino{ 0,0,TT_End,O_End }
    , m_scoreKeeper()
    , m_randomizer()
    , m_playState(PS_Ready)
    , m_lastRotateResult{false, false, false}
    , m_totalClearedLines(0)
    , m_level(1)
    , m_hold(TetriminoType::TT_End)
    , m_holdChance(1)
    , m_combo(0)
    , m_backToBackChance(false)
    , m_repeatDelayTick(0)
    , m_softDropDelay(50)
    , m_softDropDelayTick(0)
    , m_gravityDelay(500)
    , m_moveDelayTick(0)
    , m_consecutiveMoveCount(0)
    , m_gravityTick(0)
    , m_lockDelayTick(0)
    , m_lastShift(0)
    , m_gravity(true)

{
    Reset();
}

void TetrisGame::Reset()
{
    m_playField.ClearPlayfield();
    m_activeTetrimino = { 0,0,TT_End,O_End };
    m_scoreKeeper.ResetScore();
    m_randomizer.Reset();
    m_randomizer.Seed(static_cast<unsigned int>(std::time(nullptr)));
    m_playState = PS_Ready;
    m_totalClearedLines = 0;
    m_lastRotateResult = { false, false, false };
    m_level = 1;
    m_hold = TT_End;
    m_holdChance = 1;
    m_combo = 0;
    m_backToBackChance = false;
    m_repeatDelayTick = 0;
    m_softDropDelay = 50;
    m_softDropDelayTick = 0;
    m_gravityDelay = 500;
    m_moveDelayTick = 0;
    m_consecutiveMoveCount = 0;
    m_gravityTick = 0;
    m_lockDelayTick = 0;
    m_lastShift = 0;
    m_gravity = true;
}

void TetrisGame::SaveState(std::ostream& ofs)
{
    m_activeTetrimino.Serialize(ofs);
    m_playField.Serialize(ofs);
    m_randomizer.Serialize(ofs);
    m_scoreKeeper.Serialize(ofs);
    ofs.write(reinterpret_cast<char*>(&m_level), sizeof(m_level));
    ofs.write(reinterpret_cast<char*>(&m_hold), sizeof(m_hold));
}

void TetrisGame::LoadState(std::istream& ifs)
{
    m_activeTetrimino.Deserialize(ifs);
    m_playField.Deserialize(ifs);
    m_randomizer.Deserialize(ifs);
    m_scoreKeeper.Deserialize(ifs);
    ifs.read(reinterpret_cast<char*>(&m_level), sizeof(m_level));
    ifs.read(reinterpret_cast<char*>(&m_hold), sizeof(m_hold));
}

void TetrisGame::ToggleGravity()
{
    m_gravity = !m_gravity;
}

void TetrisGame::HardDrop()
{
    if (m_playState == PS_Control)
    {
        while (!m_playField.CheckDownContact(m_activeTetrimino))
        {
            --m_activeTetrimino.y;
            m_scoreKeeper.OnHardDrop();
            m_lastRotateResult = { false, false, false };
        }
        m_playState = LockAndSelectState(m_activeTetrimino);

        m_handler.OnHardDrop();
    }
}

void TetrisGame::StartGame()
{
    if (m_playState == PS_Ready)
    {
        m_playState = PS_Dropped;
    }
}

void TetrisGame::RotateAntiClockwise()
{
    if (!m_playField.CheckOverlap(m_activeTetrimino))
        m_lastRotateResult = RotateTetrimino(m_activeTetrimino, false);
    if (m_lastRotateResult.success)
        m_lockDelayTick = SDL_GetTicks();
}

void TetrisGame::RotateClockwise()
{
    if (!m_playField.CheckOverlap(m_activeTetrimino))
    {
        m_lastRotateResult = RotateTetrimino(m_activeTetrimino, true);
        if (m_lastRotateResult.success)
            m_lockDelayTick = SDL_GetTicks();
    }
}

void TetrisGame::Hold()
{
    if (m_playState == PS_Control && 0 < m_holdChance)
    {
        --m_holdChance;

        Tetrimino next;
        if (m_hold != TetriminoType::TT_End)
        {
            next = MakeTetrimino(m_hold, m_playField.GetSpawnX(), m_playField.GetSpawnY());
        }
        else
        {
            next = SpawnNext(m_randomizer);
        }

        m_hold = m_activeTetrimino.type;

        m_activeTetrimino = next;

        m_gravityTick = SDL_GetTicks();
    }
}

void TetrisGame::Restart()
{
    if (m_playState == PS_GameOver)
    {
        Reset();
    }
}

PlayState TetrisGame::GetPlayState()
{
    return m_playState;
}

DropResult TetrisGame::OnDrop()
{
    DropResult result = { 0,LCT_End, false, false };

    m_handler.OnLock();

    const int MaximumLineClearCount = 4;
    int clearedLines[MaximumLineClearCount] = { m_playField.GetHeight() };
    int lineClearCount = 0;
    const int PlayFieldHeight = m_playField.GetHeight();
    const int PlayFieldWidth = m_playField.GetWidth();

    // Count and remember cleared lines
    for (int y = 0; y < PlayFieldHeight; ++y)
    {
        bool lineComplete = true;
        for (int x = 0; x < PlayFieldWidth; ++x)
        {
            Block block = m_playField.Get(x, y);
            if (block == B_Empty)
            {
                lineComplete = false;
                break;
            }
        }

        // Complete line found
        if (lineComplete)
            clearedLines[lineClearCount++] = y;
    }

    result.lct = DetectLineClearType(lineClearCount, m_lastRotateResult.tspin, m_lastRotateResult.minitspin);
    LineClearInfo lci = LineClearInfoTable[result.lct];

    if (lineClearCount)
    {
        int dy = 0;
        // Rearrange cleared lines
        for (int y = 0; y < PlayFieldHeight; ++y)
        {
            bool isClearedLine = false;
            if (clearedLines[dy] == y)
            {
                ++dy;
                isClearedLine = true;
            }
            else if( dy > 0 )
            {
                m_playField.CopyLine(y, y - dy);

                if (PlayFieldHeight - y <= dy)
                {
                    m_playField.ClearRow(y);
                }
            }
        }

        m_scoreKeeper.OnLineClear(result.lct, m_backToBackChance, m_combo, m_level);

        result.backToBack = m_backToBackChance && lci.difficult;

        m_backToBackChance = lci.difficult;

        ++m_combo;

        m_handler.OnLineClear(result);
    }
    else // lineClearCount == 0
    {
        m_combo = 0;
    }

    m_holdChance = 1;

    m_totalClearedLines += lineClearCount;

    int newLevel = (m_totalClearedLines / LevelUpStep) + 1;
    if (m_level != newLevel)
    {
        m_level = newLevel;
        result.levelUp = true;
    }
    unsigned deltaGravityDelay = (m_level - 1) * 50;
    if (deltaGravityDelay < InitialGravityDelay)
    {
        m_gravityDelay = InitialGravityDelay - deltaGravityDelay;
    }
    else
    {
        m_gravityDelay = 50;
    }

    m_activeTetrimino = SpawnNext(m_randomizer);

    m_gravityTick = SDL_GetTicks();
    m_lockDelayTick = SDL_GetTicks();

    m_lastRotateResult = { false,false,false };

    m_playState = PS_Control;

    result.lineClearCount = lineClearCount;

    return result;
}

int TetrisGame::GetCombo()
{
    return m_combo;
}

int TetrisGame::GetLevel()
{
    return m_level;
}

void TetrisGame::OnControl()
{
    if (m_gravity)
    {
        // If tetrimino touched landed
        if (m_playField.CheckDownContact(m_activeTetrimino))
        {
            if (SDL_GetTicks() - m_lockDelayTick > LockDelay)
            {
                m_playState = LockAndSelectState(m_activeTetrimino);
            }
        }
        else
        {
            if (static_cast<int>(SDL_GetTicks() - m_gravityTick) > m_gravityDelay)
            {
                --m_activeTetrimino.y;
                m_gravityTick = SDL_GetTicks();
                m_lockDelayTick = SDL_GetTicks();
                m_lastRotateResult = { false, false, false };
                m_handler.OnMoveDown(false);
            }
        }
    }
}

void TetrisGame::SoftDrop()
{
    if (SDL_GetTicks() - m_softDropDelayTick > m_softDropDelay)
    {
        if (!m_playField.CheckDownContact(m_activeTetrimino))
        {
            --m_activeTetrimino.y;
            m_gravityTick = SDL_GetTicks();
            m_softDropDelayTick = SDL_GetTicks();
            m_lockDelayTick = SDL_GetTicks();
            m_scoreKeeper.OnSoftDrop();
            m_handler.OnMoveDown(true);
        }
    }
}

void TetrisGame::Shift(int dx)
{
    if (dx != 0)
    {
        if (m_lastShift * dx < 0)
        {
            m_consecutiveMoveCount = 0;
            m_repeatDelayTick = SDL_GetTicks();
        }
        m_lastShift = dx;
        if (m_consecutiveMoveCount == 0 || (SDL_GetTicks() - m_repeatDelayTick > RepeatDelay))
        {
            if (SDL_GetTicks() - m_moveDelayTick > MoveDelay)
            {
                if (m_playField.CheckDownContact(m_activeTetrimino))
                {
                    m_gravityTick = SDL_GetTicks();
                }
                m_activeTetrimino.x += dx;
                if (m_playField.CheckOverlap(m_activeTetrimino))
                {
                    m_activeTetrimino.x -= dx;
                }
                else
                {
                    m_lastRotateResult = { false,false,false };
                    m_lockDelayTick = SDL_GetTicks();
                }
                m_moveDelayTick = SDL_GetTicks();
                ++m_consecutiveMoveCount;
            }
        }
    }
    else
    {
        m_consecutiveMoveCount = 0;
        m_repeatDelayTick = SDL_GetTicks();
    }
}

PlayField& TetrisGame::GetPlayField()
{
    return m_playField;
}

Randomizer& TetrisGame::GetRandomizer()
{
    return m_randomizer;
}

Tetrimino& TetrisGame::GetActiveTetrimino()
{
    return m_activeTetrimino;
}

TetriminoType TetrisGame::GetHold()
{
    return m_hold;
}

int TetrisGame::GetTotalClearedLines()
{
    return m_totalClearedLines;
}

int TetrisGame::GetScore()
{
    return m_scoreKeeper.GetScore();
}

void TetrisGame::Win()
{
    if (m_playState != PS_Win)
    {
        m_playState = PS_Win;
        m_handler.OnWin();
    }
}

void TetrisGame::AddGarbageLines(int count)
{
    for (int y = m_playField.GetHeight() - 1; y >= count; --y)
    {
        if (y >= count)
        {
            m_playField.CopyLine(y - count, y);
        }
    }

    for (int y = count - 1; y >= 0; --y)
    {
        int bx = rand() % m_playField.GetWidth();

        for (int x = 0; x < m_playField.GetWidth(); ++x)
        {
            m_playField.Set(x, y, x != bx ? Block::B_Gray : Block::B_Empty);
        }
    }

    if (m_playField.CheckOverlap(m_activeTetrimino))
        m_activeTetrimino.y++;
}

PlayState TetrisGame::LockAndSelectState(Tetrimino& tm)
{
    bool overwrite = m_playField.LockTetrimino(tm);
    return !overwrite ? PS_Dropped : PS_GameOver;
}

Tetrimino TetrisGame::SpawnNext(Randomizer& randomizer)
{
    TetriminoType tmType = randomizer.PopNext();
    return MakeTetrimino(tmType, m_playField.GetSpawnX(), m_playField.GetSpawnY());
}

RotateResult TetrisGame::RotateTetrimino(Tetrimino& tm, bool clockwise)
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
        if (!m_playField.CheckOverlap(next))
        {
            if (tm.type == TT_T)
            {
                // 3-corner tspin chance
                if (i == 0) tspin = m_playField.TestThreeCorner(next);
                switch (kickTableIndex)
                {
                case OT_NorthToEast:
                    if (i == 1) minitspin = m_playField.TestThreeHighWallSide(next.x, next.y);
                    if (i >= 3) tspin = true;
                    break;

                case OT_EastToNorth:
                    if (i == 2) minitspin = m_playField.CheckXyOverlap(next.x + 2, next.y - 2);
                    break;

                case OT_EastToSouth:
                    if (i == 2) tspin = true;
                    break;

                case OT_WestToSouth:
                    if (i == 2) tspin = true;
                    break;

                case OT_WestToNorth:
                    if (i == 2) minitspin = m_playField.CheckXyOverlap(next.x, next.y - 2);
                    break;

                case OT_NorthToWest:
                    if (i == 1) minitspin = m_playField.TestThreeHighWallSide(next.x + 2, next.y);
                    if (i >= 3) tspin = true;
                    break;
                }
            }

            tm = next;
            
            RotateResult result = {
                true, tspin, minitspin
            };

            m_handler.OnRotation(result, clockwise);

            return result;
        }
    }

    return {
        false, false, false
    };
}
