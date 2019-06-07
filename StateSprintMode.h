#ifndef STATE_SPRINT_MODE_H_
#define STATE_SPRINT_MODE_H_

#include "FiniteStateMachine.h"
#include "UI.h"
#include "SprintTimer.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"

class StateSprintMode : public State
{
public:
    StateSprintMode(TetrisRenderer& tetrisRenderer);

    void OnStart() override;

    void OnUpdate() override;

    void OnRender() override;

    void OnSdlEvent(const SDL_Event& e);

private:
    TetrisGame tetrisGame;
    TetrisRenderer& tetrisRenderer;
    SprintTimer sprintTimer;
    UIStack sprintUI;
    Texture readyShow;
    Texture gameoverShow;
    Texture sprintCompleteShow;
};

#endif
