#include "StateMainMenu.h"
#include <SDL_mixer.h>
#include "Application.h"

StateMainMenu::StateMainMenu(SDL_Renderer* pRenderer)
    : m_pRenderer(pRenderer)
{
}

void StateMainMenu::OnStart()
{
    mainMenu.Clear();
    mainMenu.SetXy(10, 10);
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(m_pRenderer, Application::GetString(StringTable::SI_MainMenu_Top), ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(m_pRenderer, Application::GetString(StringTable::SI_MainMenu_Infinite), ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(m_pRenderer, Application::GetString(StringTable::SI_MainMenu_Sprint), ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(m_pRenderer, Application::GetString(StringTable::SI_MainMenu_Options), ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(m_pRenderer, Application::GetString(StringTable::SI_MainMenu_Quit), ColorBlack)));
    mainMenu.Render();
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
