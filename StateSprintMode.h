#ifndef STATE_SPRINT_MODE_H_
#define STATE_SPRINT_MODE_H_

#include "StateOnePlayer.h"
#include "UI.h"
#include "SprintTimer.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"

class StateSprintMode : public StateOnePlayer
{
public:
    StateSprintMode(TetrisRenderer& tetrisRenderer);

    void OnStart() override;

    void OnUpdate() override;

    void OnRender() override;

    void OnSdlEvent(const SDL_Event& e);

private:
    SprintTimer m_sprintTimer;
    UIStack m_uiSprint;
    UITextBox m_uiReady;
    UITextBox m_uiGameOver;
    UITextBox m_uiSprintComplete;
};

#endif
