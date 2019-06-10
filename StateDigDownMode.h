#include "StateOnePlayer.h"

class StateDigDownMode : public StateOnePlayer
{
public:
    StateDigDownMode(TetrisRenderer& renderer);

    void OnStart() override;

    void OnUpdate() override;

    void OnRender() override;

    int GetGrowTime();

private:
    std::chrono::steady_clock clock;
    std::chrono::steady_clock::time_point lastGrow;
    UIStack m_uiDigDown;
};
