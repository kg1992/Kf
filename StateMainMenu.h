#ifndef STATE_MAIN_MENU_H_
#define STATE_MAIN_MENU_H_

#include "FiniteStateMachine.h"
#include "UI.h"

class StateMainMenu : public State
{
public:
    StateMainMenu();

    void OnStart() override;

    void OnRender() override;

    void OnSdlEvent(const SDL_Event& e) override;

private:
    UIStack mainMenu;
};

#endif
