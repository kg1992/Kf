#include "StateOptions.h"
#include <SDL.h>
#include <vector>
#include "UI.h"
#include "Application.h"

StateOptions::StateOptions(SDL_Renderer* pRenderer)
{
    const SDL_Color ColorBlack = {0,0,0};
    std::shared_ptr<UIStack> stack(new UIStack);
    stack->AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "1. Change Resolution", ColorBlack)));
    stack->AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "2. Toggle Fullscreen", ColorBlack)));
    stack->AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "3. Return", ColorBlack)));
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
            int& ri = Application::options.resolutionIndex;
            ri = (ri + 1) % resolutions.size();
            Application::ResizeWindow(resolutions[ri].first, resolutions[ri].second);
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
            break;
        }
        case SDLK_3:
            Application::gsm.SetState(&*Application::pStateMainMenu); 
            break;
        default:
            break;
        }
    }
}
