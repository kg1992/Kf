// ref : https://harddrop.com/wiki/Spawn_Location
// ref : https://tetris.fandom.com/wiki/Playfield
// ref : https://tetris.fandom.com/wiki/Tetromino
// ref : https://tetris.fandom.com/wiki/SRS
#ifndef TETRIS_GAME_H_
#define TETRIS_GAME_H_

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

Tetrimino MakeTetrimino(TetriminoType type, int x = 0, int y = 0);

struct RotateResult
{
    bool success;
    bool tspin;
    bool minitspin;
};

class TetrisGame
{
public:
    TetrisGame(int playFieldWidth, int playFieldHeight, int spawnX, int spawnY);

    void Reset();

    // Saves current state to the stream in binary
    void SaveState(std::ostream& ofs);

    // Loads a state from the stream in binary
    void LoadState(std::istream& ifs);

    // Toggles gravity. If gravity is turend off, active tetrimino will not fall over time
    void ToggleGravity();

    // Trigger hard drop
    void HardDrop();

    // Start the game
    void StartGame();

    // Rotate active tetrimino anticlockwise. Check lastRotateResult
    void RotateAntiClockwise();

    // Rotate active tetrimino clockwise. Check lastRotateResult
    void RotateClockwise();

    // Trigger hold
    void Hold();

    // Restart the game if the game is in 'PS_GameOver' state
    void Restart();

    // Gets current state of the game
    PlayState GetPlayState();

    struct DropResult
    {
        int lineClearCount;
        LineClearType lct;
        bool backToBack;
        bool levelUp;
    };

    DropResult OnDrop();

    // Gets current comboe count
    int GetCombo();

    // Gets current level of the game. gravity power is proportional to the level
    int GetLevel();

    // invoke this function every frame to activate for 'Lock' and 'Gravity'
    void OnControl();

    // Trigger soft drop
    void SoftDrop();

    // Triggers shifting
    void Shift(int dx);

    // Gets PlayField
    PlayField& GetPlayField();

    // Gets Randomizer
    Randomizer& GetRandomizer();

    // Gets active tetrimino
    Tetrimino& GetActiveTetrimino();

    // Gets current tetrimino that is held
    TetriminoType GetHold();

    // Gets total line clear count
    int GetTotalClearedLines();

    // Gets score
    int GetScore();

    // Change state to win
    void Win();

    // Lines
    void AddGarbageLines(int count);

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
    int lastShift = 0;

    PlayState playState = PS_Ready;
    PlayField playField;

    bool gravity = true;

    PlayState LockAndSelectState(Tetrimino& tm);

    Tetrimino SpawnNext(Randomizer& randomizer);

    RotateResult RotateTetrimino(Tetrimino& tm, bool clockwise);
};

#endif
