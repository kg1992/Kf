#ifndef STATE_BATTLE_MODE_H_
#define STATE_BATTLE_MODE_H_

#include "TetrisGame.h"
#include "TetrisRenderer.h"
#include "FiniteStateMachine.h"
#include "StateTetrisGame.h"

class StateBattleMode : public StateTetrisGame
{
public:
    StateBattleMode(TetrisRenderer& renderer);

    void OnStart() override;

    void OnUpdate() override;

    void OnRender() override;

    void OnSdlEvent(const SDL_Event& e) override;
   
    void OnEnd() override;

private:
    TetrisGame m_left;
    TetrisGame m_right;
    TetrisRenderer& m_tetrisRenderer;

    TetrisRenderDesc MakeDescLeft();
    TetrisRenderDesc MakeDescRight();

    void UpdateTetrisGame(TetrisGame& tetrisGame);
};

#endif
