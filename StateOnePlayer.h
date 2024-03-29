#ifndef STATE_ONE_PLAYER_H_
#define STATE_ONE_PLAYER_H_

#include "FiniteStateMachine.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"
#include "UI.h"

class StateOnePlayer : public State
{
protected:
    StateOnePlayer(TetrisRenderer& tetrisRenderer);

    virtual void OnStart();

    virtual void OnRender();

    virtual void OnSdlEvent(const SDL_Event& e);

    TetrisGame m_tetrisGame;
    TetrisRenderer& m_tetrisRenderer;
    UITextBox m_uiReady;
    UITextBox m_uiGameOver;

    TetrisRenderDesc MakeRenderDesc();

    void DoShift();

    TetrisGame::DropResult DoDrop();
};

#endif
