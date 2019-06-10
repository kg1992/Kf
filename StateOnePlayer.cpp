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

    m_tetrisRenderer.SetTetrisRenderDesc(MakeRenderDesc());
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

TetrisRenderDesc StateOnePlayer::MakeRenderDesc()
{
    TetrisRenderDesc desc = { 0 };
    const int ClientHeight = Application::GetClientAreaHeight();
    const int ClientWidth = Application::GetClientAreaWidth();
    const int PxBlockSize = static_cast<int>(std::round(Application::GetClientAreaHeight() / 600.f * 24.f));
    const int PxPlayFIeldWidth = m_tetrisGame.GetPlayField().GetWidth() * PxBlockSize;
    const int NextCount = 5;

    desc.pxBlockSize = PxBlockSize;
    desc.pxPlayFieldX = (ClientWidth - PxPlayFIeldWidth) / 2;
    desc.pxPlayFieldY = 100;
    desc.pxHoldX = desc.pxPlayFieldX - PxBlockSize * 5;
    desc.pxHoldY = desc.pxPlayFieldY;
    desc.nextCount = NextCount;
    for (int i = 0; i < NextCount; ++i)
    {
        desc.pxNextX[i] = desc.pxPlayFieldX + desc.pxBlockSize * 11;
        desc.pxNextY[i] = desc.pxPlayFieldY + i * desc.pxBlockSize * 4;
    }

    desc.visibleLines = 20;
    return desc;
}

void StateOnePlayer::DoShift()
{
    int dx = 0;
    if (Application::state[SDL_SCANCODE_DOWN]) m_tetrisGame.SoftDrop();
    if (Application::state[SDL_SCANCODE_RIGHT]) dx += 1;
    if (Application::state[SDL_SCANCODE_LEFT]) dx += -1;
    m_tetrisGame.Shift(dx);
}

TetrisGame::DropResult StateOnePlayer::DoDrop()
{
    auto result = m_tetrisGame.OnDrop();
    if (result.lineClearCount != 0)
    {
        Mix_PlayChannel(-1, g_pWavExplosion, 0);
    }
    return result;
}
