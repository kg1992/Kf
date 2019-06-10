#ifndef STATE_INFINITE_MODE_H_
#define STATE_INFINITE_MODE_H_

#include "StateOnePlayer.h"
#include "UI.h"
#include "SprintTimer.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"

class StateInfiniteMode : public StateOnePlayer
{
public:
    StateInfiniteMode(TetrisRenderer& tetrisRenderer);

    void OnStart() override;

    void OnUpdate() override;

    void OnRender() override;

    void OnSdlEvent(const SDL_Event& e);

private:
    UITextBox m_uiReady;
    UITextBox m_uiGameOver;
    UIStack m_uiInfinite;
    std::shared_ptr<UITextBox> m_uiBonus;
};


#endif
