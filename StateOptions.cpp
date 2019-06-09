#include "StateOptions.h"
#include <SDL.h>
#include <vector>
#include "UI.h"
#include "Application.h"

StateOptions::StateOptions()
{
    const SDL_Color ColorBlack = {0,0,0};
    std::shared_ptr<UIStack> stack(new UIStack);
    
    m_changeResolutioniBox.reset(new UITextBox("_", ColorBlack));
    stack->AddUI(m_changeResolutioniBox);

    m_toggleFullscreenBox.reset(new UITextBox("_", ColorBlack));
    stack->AddUI(m_toggleFullscreenBox);

    m_changeLanguageBox.reset(new UITextBox("_", ColorBlack));
    stack->AddUI(m_changeLanguageBox);

    m_quitBox.reset(new UITextBox("_", ColorBlack));
    stack->AddUI(m_quitBox);

    stack->SetXy(10, 10);

    RefreshTexts();

    m_ui = std::dynamic_pointer_cast<UI>(stack);
}

void StateOptions::OnRender()
{
    m_ui->Render();
}

void StateOptions::OnSdlEvent(const SDL_Event& e)
{
    // User presses a key
    if (e.type == SDL_KEYDOWN)
    {
        //Select surfaces based on key press
        switch (e.key.keysym.sym)
        {
        case SDLK_1:
        {
            int numDisplays = SDL_GetNumVideoDisplays();
            int numModes = SDL_GetNumDisplayModes(0);
            std::vector<std::pair<int, int>> resolutions;
            for (int i = 0; i < numModes; ++i)
            {
                SDL_DisplayMode mode;
                SDL_GetDisplayMode(0, i, &mode);
                std::pair<int, int> resolution(mode.w, mode.h);
                if (std::find(resolutions.begin(), resolutions.end(), resolution) == resolutions.end())
                    resolutions.insert(resolutions.begin(), resolution);
            }
            auto iter = std::find(resolutions.begin(), resolutions.end(), Application::options.resolution);
            int ri = (iter != resolutions.end() ? std::distance(resolutions.begin(), iter) : 0);
            ri = (ri + 1) % resolutions.size();
            Application::options.resolution = resolutions[ri];
            Application::ResizeWindow(resolutions[ri].first, resolutions[ri].second);
            RefreshTexts();
            break;
        }
        case SDLK_2:
        {
            if (SDL_GetWindowFlags(Application::window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
            {
                SDL_SetWindowFullscreen(Application::window, 0);
            }
            else
            {
                SDL_SetWindowFullscreen(Application::window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            RefreshTexts();
            break;
        }
        case SDLK_3:
        {
            Application::options.IncreaseLanguage();
            RefreshTexts();
        }
            break;
        case SDLK_4:
            Application::gsm.SetState(&*Application::pStateMainMenu); 
            break;
        default:
            break;
        }
    }
}

void StateOptions::RefreshTexts()
{
    m_changeResolutioniBox->SetContent(Application::GetFormattedString(StringTable::SI_Options_Resolution, Application::GetClientAreaWidth(), Application::GetClientAreaHeight()));

    m_toggleFullscreenBox->SetContent(Application::GetString(StringTable::SI_Options_Toggle_Fullscreen));

    m_changeLanguageBox->SetContent(Application::GetFormattedString(StringTable::SI_Options_Language, Application::options.language.c_str()));

    m_quitBox->SetContent(Application::GetString(StringTable::SI_Options_Return));
}
