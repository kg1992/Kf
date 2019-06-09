#ifndef STATE_OPTIONS_H_
#define STATE_OPTIONS_H_

#include "FiniteStateMachine.h"
#include "UI.h"

class StateOptions : public State
{
public:
    StateOptions();

    void OnRender();

    void OnSdlEvent(const SDL_Event& e);

private:
    std::shared_ptr<UI> m_ui;
    std::shared_ptr<UITextBox> m_changeResolutioniBox;
    std::shared_ptr<UITextBox> m_toggleFullscreenBox;
    std::shared_ptr<UITextBox> m_changeLanguageBox;
    std::shared_ptr<UITextBox> m_quitBox;

    void RefreshTexts();
};

#endif
