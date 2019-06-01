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
#include "Tetrimino.h"
#include "PlayField.h"
#include "ScoreKeeper.h"
#include "Randomizer.h"
#include "KickTable.h"

enum PlayState : int
{
    PS_Ready,
    PS_Control,
    PS_Dropped,
    PS_GameOver,
    PS_Win,
    PS_End,
};

// Tetrimino spawn location X in play field space
const int SpawnX = 3;
// Tetrimino spawn location Y in play field space
const int SpawnY = 21;

Tetrimino MakeTetrimino(TetriminoType type);

struct RotateResult
{
    bool success;
    bool tspin;
    bool minitspin;
};

class TetrisGame
{
public:
    TetrisGame(int playFieldWidth, int playFieldHeight);

    void Reset();

    void SaveState(std::ostream& ofs);

    void LoadState(std::istream& ifs);

    void ToggleGravity();

    void HardDrop();

    void StartGame();

    void RotateAntiClockwise();

    void RotateClockwise();

    void Hold();

    void Restart();

    PlayState GetPlayState();

    struct DropResult
    {
        int lineClearCount;
        LineClearType lct;
        bool backToBack;
        bool levelUp;
    };

    DropResult OnDrop();

    int GetCombo();

    int GetLevel();

    void OnControl();

    void SoftDrop();

    void Shift(int dx);

    PlayField& GetPlayField();

    Randomizer& GetRandomizer();

    Tetrimino& GetActiveTetrimino();

    TetriminoType GetHold();

    int GetTotalClearedLines();

    int GetScore();

    void Win();

private:
    int totalClearedLines = 0;
    int level = 1;
    ScoreKeeper scoreKeeper;
    Randomizer randomizer;
    RotateResult lastRotateResult = { false, false, false };

    TetriminoType hold = TetriminoType::TT_End;
    int holdChance = 1;
    int combo = 0;
    bool backToBackChance;

    Tetrimino tmActive = { 0,0,TT_End,O_End};
    const unsigned int initialGravityDelay = 500;
    const unsigned int LockDelay = 500;
    const unsigned int LevelUpStep = 10;
    const unsigned int RepeatDelay = 220;
    const unsigned int MoveDelay = 30;
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

    PlayState LockAndSelectState(Tetrimino& tm);

    Tetrimino SpawnNext(Randomizer& randomizer);

    RotateResult RotateTetrimino(Tetrimino& tm, bool clockwise);
};

#endif
