#include "FiniteStateMachine.h"

FiniteStateMachine::FiniteStateMachine()
    : m_pCurr(nullptr)
{
}

void FiniteStateMachine::SetState(State* pState)
{
    if (m_pCurr)
        m_pCurr->OnEnd();
    m_pCurr = pState;
    if (m_pCurr)
        m_pCurr->OnStart();
}

State* FiniteStateMachine::GetCurrent()
{
    return m_pCurr;
}

void FiniteStateMachine::OnUpdate()
{
    if (m_pCurr)
        m_pCurr->OnUpdate();
}

void FiniteStateMachine::OnRender()
{
    if (m_pCurr)
        m_pCurr->OnRender();
}

void FiniteStateMachine::OnSdlEvent(const SDL_Event& e)
{
    if (m_pCurr)
        m_pCurr->OnSdlEvent(e);
}
