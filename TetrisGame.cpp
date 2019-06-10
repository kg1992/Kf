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
    : playField(playFieldWidth, playFieldHeight, spawnX, spawnY)
{
    Reset();
}

void TetrisGame::Reset()
{
    level = 1;
    hold = TT_End;
    randomizer.Reset();    
    randomizer.Seed(static_cast<unsigned int>(std::time(nullptr)));
    scoreKeeper.ResetScore();
    totalClearedLines = 0;
    playField.ClearPlayfield();
    playState = PS_Ready;

    lastRotateResult = { false, false, false };

    holdChance = 1;
    combo = 0;
    backToBackChance = false;

    Tetrimino tmActive = { 0,0,TT_End,O_End };

    repeatDelayTick = 0;
    softDropDelay = 50;
    softDropDelayTick = 0;
    gravityDelay = 500;
    moveDelayTick = 0;
    consecutiveMoveCount = 0;
    gravityTick = 0;
    lockDelayTick = 0;

    gravity = true;
}

void TetrisGame::SaveState(std::ostream& ofs)
{
    tmActive.Serialize(ofs);
    playField.Serialize(ofs);
    randomizer.Serialize(ofs);
    scoreKeeper.Serialize(ofs);
    ofs.write(reinterpret_cast<char*>(&level), sizeof(level));
    ofs.write(reinterpret_cast<char*>(&hold), sizeof(hold));
}

void TetrisGame::LoadState(std::istream& ifs)
{
    tmActive.Deserialize(ifs);
    playField.Deserialize(ifs);
    randomizer.Deserialize(ifs);
    scoreKeeper.Deserialize(ifs);
    ifs.read(reinterpret_cast<char*>(&level), sizeof(level));
    ifs.read(reinterpret_cast<char*>(&hold), sizeof(hold));
}

void TetrisGame::ToggleGravity()
{
    gravity = !gravity;
}

void TetrisGame::HardDrop()
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

void TetrisGame::StartGame()
{
    if (playState == PS_Ready)
    {
        playState = PS_Dropped;
    }
}

void TetrisGame::RotateAntiClockwise()
{
    if (!playField.CheckOverlap(tmActive))
        lastRotateResult = RotateTetrimino(tmActive, false);
    if (lastRotateResult.success)
        lockDelayTick = SDL_GetTicks();
}

void TetrisGame::RotateClockwise()
{
    if (!playField.CheckOverlap(tmActive))
    {
        lastRotateResult = RotateTetrimino(tmActive, true);
        if (lastRotateResult.success)
            lockDelayTick = SDL_GetTicks();
    }
}

void TetrisGame::Hold()
{
    if (playState == PS_Control && 0 < holdChance)
    {
        --holdChance;

        Tetrimino next;
        if (hold != TetriminoType::TT_End)
        {
            next = MakeTetrimino(hold, playField.GetSpawnX(), playField.GetSpawnY());
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

void TetrisGame::Restart()
{
    if (playState == PS_GameOver)
    {
        Reset();
    }
}

PlayState TetrisGame::GetPlayState()
{
    return playState;
}

TetrisGame::DropResult TetrisGame::OnDrop()
{
    DropResult result = { 0,LCT_End, false, false };

    const int MaximumLineClearCount = 4;
    int clearedLines[MaximumLineClearCount] = { playField.GetHeight() };
    int lineClearCount = 0;
    const int PlayFieldHeight = playField.GetHeight();
    const int PlayFieldWidth = playField.GetWidth();

    // Count and remember cleared lines
    for (int y = 0; y < PlayFieldHeight; ++y)
    {
        bool lineComplete = true;
        for (int x = 0; x < PlayFieldWidth; ++x)
        {
            Block block = playField.Get(x, y);
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

    result.lct = DetectLineClearType(lineClearCount, lastRotateResult.tspin, lastRotateResult.minitspin);
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
                for (int x = 0; x < PlayFieldWidth; ++x)
                {
                    playField.Set(x, y - dy, playField.Get(x, y));
                }

                if (PlayFieldHeight - y <= dy)
                {
                    playField.ClearRow(y);
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

int TetrisGame::GetCombo()
{
    return combo;
}

int TetrisGame::GetLevel()
{
    return level;
}

void TetrisGame::OnControl()
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

void TetrisGame::SoftDrop()
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

void TetrisGame::Shift(int dx)
{
    if (dx != 0)
    {
        if (lastShift * dx < 0)
        {
            consecutiveMoveCount = 0;
            repeatDelayTick = SDL_GetTicks();
        }
        lastShift = dx;
        if (consecutiveMoveCount == 0 || (SDL_GetTicks() - repeatDelayTick > RepeatDelay))
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

PlayField& TetrisGame::GetPlayField()
{
    return playField;
}

Randomizer& TetrisGame::GetRandomizer()
{
    return randomizer;
}

Tetrimino& TetrisGame::GetActiveTetrimino()
{
    return tmActive;
}

TetriminoType TetrisGame::GetHold()
{
    return hold;
}

int TetrisGame::GetTotalClearedLines()
{
    return totalClearedLines;
}

int TetrisGame::GetScore()
{
    return scoreKeeper.GetScore();
}

void TetrisGame::Win()
{
    playState = PS_Win;
}

void TetrisGame::AddGarbageLines(int count)
{
    for (int y = playField.GetHeight() - 1; y >= count; --y)
    {
        int sy = y - count;
        if (sy >= 0)
        {
            for (int x = 0; x < playField.GetWidth(); ++x)
            {
                playField.Set(x, y, playField.Get(x, sy));
            }
        }
    }
    for (int y = count - 1; y >= 0; --y)
    {
        int bx = rand() % playField.GetWidth();
        for (int x = 0; x < playField.GetWidth(); ++x)
        {
            playField.Set(x, y, x != bx ? Block::B_Gray : Block::B_Empty);
        }
    }

    if (playField.CheckOverlap(tmActive))
        tmActive.y++;
}

PlayState TetrisGame::LockAndSelectState(Tetrimino& tm)
{
    bool overwrite = playField.LockTetrimino(tm);
    return !overwrite ? PS_Dropped : PS_GameOver;
}

Tetrimino TetrisGame::SpawnNext(Randomizer& randomizer)
{
    TetriminoType tmType = randomizer.PopNext();
    return MakeTetrimino(tmType, playField.GetSpawnX(), playField.GetSpawnY());
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
