#ifndef FINITE_STATE_MACHINE_H_
#define FINITE_STATE_MACHINE_H_

#include <string>
#include <SDL.h>

class State
{
public:
    virtual void OnStart() {};
    virtual void OnUpdate() {};
    virtual void OnEnd() {};
    virtual void OnRender() {};
    virtual void OnSdlEvent(const SDL_Event& e) {};
    virtual ~State() {};
};

class FiniteStateMachine
{
public:
    FiniteStateMachine();

    void SetState(State* pState);

    State* GetCurrent();

    void OnUpdate();

    void OnRender();

    void OnSdlEvent(const SDL_Event& e);

private:
    State* m_pCurr;
};

#endif
