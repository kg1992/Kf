#ifndef STATE_ONE_PLAYER_H_
#define STATE_ONE_PLAYER_H_

#include "FiniteStateMachine.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"

class StateOnePlayer : public State
{
public:
    StateOnePlayer(TetrisRenderer& tetrisRenderer);

protected:
    TetrisGame m_tetrisGame;
    TetrisRenderer& m_tetrisRenderer;

    TetrisRenderDesc MakeRenderDesc();
};

#endif
