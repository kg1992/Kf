#include "StateMainMenu.h"
#include <SDL_mixer.h>
#include "Application.h"

StateMainMenu::StateMainMenu(SDL_Renderer* pRenderer)
{
    mainMenu.SetXy(10, 10);
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "Press number key to select", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "1. Infinity Mode", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "2. Sprint 40", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "3. Options", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "4. Quit", ColorBlack)));
}

void StateMainMenu::OnRender()
{
    mainMenu.Render();
}

void StateMainMenu::OnSdlEvent(const SDL_Event& e)
{
    // User presses a key
    if (e.type == SDL_KEYDOWN)
    {
        //Select surfaces based on key press
        switch (e.key.keysym.sym)
        {
        case SDLK_1:
            Application::gsm.SetState(&*Application::pStateInfiniteMode);
            Mix_ResumeMusic();
            break;
        case SDLK_2:
            Application::gsm.SetState(&*Application::pStateSprintMode);
            Mix_ResumeMusic();
            break;
        case SDLK_3:
            Application::gsm.SetState(&*Application::pStateOptions);
            break;
        case SDLK_4:
            Application::Quit();
            break;
        default:
            break;
        }
    }
}
