#ifndef STATE_TETRIS_GAME_H_
#define STATE_TETRIS_GAME_H_

#include "FiniteStateMachine.h"
#include "TetrisRenderer.h"

class StateTetrisGame : public State
{
protected:
    static TetrisRenderDesc MakeRenderDesc(TetrisGame& tetrisGame, int x, int y, int pxBlockSize);

    void DoShift(TetrisGame& tetrisGame);

    DropResult DoDrop(TetrisGame& tetrisGame);
};

#endif
