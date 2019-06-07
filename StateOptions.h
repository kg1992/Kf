#ifndef STATE_OPTIONS_H_
#define STATE_OPTIONS_H_

#include "FiniteStateMachine.h"
#include "UI.h"

class StateOptions : public State
{
public:
    StateOptions(SDL_Renderer* pRenderer);

    void OnRender();

    void OnSdlEvent(const SDL_Event& e);

private:
    std::shared_ptr<UI> m_ui;
};

#endif
