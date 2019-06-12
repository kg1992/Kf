#ifndef STATE_SPRINT_MODE_H_
#define STATE_SPRINT_MODE_H_

#include "StateOnePlayer.h"
#include "UI.h"
#include "SprintTimer.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"
#include "Replay.h"

class StateSprintMode : public StateOnePlayer
{
public:
    class SprintTetrisEventHandler : public TetrisEventHandler
    {
    public:
        SprintTetrisEventHandler(StateSprintMode& state);

        void OnGameStart() override;

        void OnWin() override;

    private:
        StateSprintMode& m_state;
        
    };

    StateSprintMode(TetrisRenderer& tetrisRenderer);

    void OnStart() override;

    void OnUpdate() override;

    void OnRender() override;

    void OnSdlEvent(const SDL_Event& e);

private:
    SprintTimer m_sprintTimer;
    UIStack m_uiSprint;
    UITextBox m_uiSprintComplete;
    ReplayRecorder m_recorder;
    std::shared_ptr<SprintTetrisEventHandler> m_handler;
};

#endif
