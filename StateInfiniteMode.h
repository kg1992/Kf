#ifndef STATE_INFINITE_MODE_H_
#define STATE_INFINITE_MODE_H_

#include "FiniteStateMachine.h"
#include "UI.h"
#include "SprintTimer.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"

class StateInfiniteMode : public State
{
public:
    StateInfiniteMode(TetrisRenderer& tetrisRenderer);

    void OnStart() override;

    void OnUpdate() override;

    void OnRender() override;

    void OnSdlEvent(const SDL_Event& e);

private:
    TetrisGame m_tetrisGame;
    TetrisRenderer& m_tetrisRenderer;
    UITextBox m_uiReady;
    UITextBox m_uiGameOver;
    UIStack m_uiInfinite;
    std::shared_ptr<UITextBox> m_uiBonus;
};


#endif
