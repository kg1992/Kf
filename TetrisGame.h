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
#include "TetrisEventHandler.h"

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

// Result of a rotation
struct RotateResult
{
    bool success;
    bool tspin;
    bool minitspin;
};

// Result of drop
struct DropResult
{
    // how many lines were cleared. 0 <= lineClearCount <= 4
    int lineClearCount;
    // line clear type. was it a single? double? triple? 
    LineClearType lct;
    // Is it a back-to-back clearing since the last one?
    bool backToBack;
    // did level increase as result of this?
    bool levelUp;
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

    TetrisEventHandlerComposite& GetEventHandler()
    {
        return m_handler;
    }

private:
    enum
    {
        InitialGravityDelay = 500,
        LockDelay = 500,
        LevelUpStep = 10,
        RepeatDelay = 220,
        MoveDelay = 30,
    };

    PlayField m_playField;
    Tetrimino m_activeTetrimino;
    ScoreKeeper m_scoreKeeper;
    Randomizer m_randomizer;
    PlayState m_playState;
    RotateResult m_lastRotateResult;
    int m_totalClearedLines;
    int m_level;
    TetriminoType m_hold;
    int m_holdChance;
    int m_combo;
    bool m_backToBackChance;
    unsigned int m_repeatDelayTick;
    unsigned int m_softDropDelay;
    unsigned int m_softDropDelayTick;
    unsigned int m_gravityDelay;
    unsigned int m_moveDelayTick;
    unsigned int m_consecutiveMoveCount;
    unsigned int m_gravityTick;
    unsigned int m_lockDelayTick;
    int m_lastShift;
    bool m_gravity;
    TetrisEventHandlerComposite m_handler;

    PlayState LockAndSelectState(Tetrimino& tm);

    Tetrimino SpawnNext(Randomizer& randomizer);

    RotateResult RotateTetrimino(Tetrimino& tm, bool clockwise);

};

#endif
