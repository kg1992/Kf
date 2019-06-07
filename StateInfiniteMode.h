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
    StateInfiniteMode(SDL_Renderer* pRenderer, TetrisRenderer& tetrisRenderer);

    void OnStart() override;

    void OnUpdate() override;

    void OnRender() override;

    void OnSdlEvent(const SDL_Event& e);

private:
    TetrisGame tetrisGame;
    TetrisRenderer& tetrisRenderer;
    Texture readyShow;
    Texture gameoverShow;
    UIStack infiniteUI;
};


#endif
