#include "Pch.h"
#include "StateOnePlayer.h"
#include "Application.h"

StateOnePlayer::StateOnePlayer(TetrisRenderer& tetrisRenderer)
    : m_tetrisGame(10, 22, 3, 19)
    , m_tetrisRenderer(tetrisRenderer)
    , m_uiReady("", ColorDarkBrown, ColorDarkYellow)
    , m_uiGameOver("", ColorDarkBrown, ColorDarkYellow)
{
}

void StateOnePlayer::OnStart()
{
    m_uiReady.SetContent(Application::GetString(StringTable::SI_Ready));
    m_uiGameOver.SetContent(Application::GetString(StringTable::SI_GameOver));


    const int ClientHeight = Application::GetClientAreaHeight();
    const int ClientWidth = Application::GetClientAreaWidth();
    const int PxBlockSize = static_cast<int>(std::round(Application::GetClientAreaHeight() / 600.f * 24.f));
    const int PxX = (ClientWidth - m_tetrisGame.GetPlayField().GetWidth() * PxBlockSize) / 2;
    const int PxY = 100;
    
    m_tetrisRenderer.SetTetrisRenderDesc(MakeRenderDesc(m_tetrisGame, PxX, PxY, PxBlockSize));
}

void StateOnePlayer::OnRender()
{
    const int ScreenWidth = Application::GetClientAreaWidth();
    const int ScreenHeight = Application::GetClientAreaHeight();

    m_tetrisRenderer.DrawTetris(m_tetrisGame);

    if (m_tetrisGame.GetPlayState() == PS_GameOver) {
        m_uiGameOver.SetXy((ScreenWidth - m_uiGameOver.GetWidth()) / 2, (ScreenHeight - m_uiGameOver.GetHeight()) / 2);
        m_uiGameOver.Render();
    }
    else if (m_tetrisGame.GetPlayState() == PS_Ready)
    {
        m_uiReady.SetXy((ScreenWidth - m_uiReady.GetWidth()) / 2, (ScreenHeight - m_uiReady.GetHeight()) / 2);
        m_uiReady.Render();
    }
}

void StateOnePlayer::OnSdlEvent(const SDL_Event& e)
{
    // User presses a key
    if (e.type == SDL_KEYDOWN)
    {
        //Select surfaces based on key press
        switch (e.key.keysym.sym)
        {
        case SDLK_g:
            m_tetrisGame.ToggleGravity();
            break;

        case SDLK_SPACE:
            if (m_tetrisGame.GetPlayState() == PS_Control)
            {
                m_tetrisGame.HardDrop();
                Mix_PlayChannel(-1, g_pWavDrop, 0);
            }
            else if (m_tetrisGame.GetPlayState() == PS_Ready)
            {
                m_tetrisGame.StartGame();
            }
            break;

        case SDLK_z:
            m_tetrisGame.RotateAntiClockwise();
            break;

        case SDLK_UP:
        case SDLK_x:
            m_tetrisGame.RotateClockwise();
            break;

        case SDLK_c:
        case SDLK_LSHIFT:
            m_tetrisGame.Hold();
            break;

        case SDLK_r:
            m_tetrisGame.Restart();
            break;

        case SDLK_q:
            m_tetrisGame.Reset();
            Mix_PauseMusic();
            Mix_RewindMusic();
            Application::gsm.SetState(&*Application::pStateMainMenu);
            break;

        default:
            break;
        }
    }
}

