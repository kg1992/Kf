#include "Pch.h"
#include "StateBattleMode.h"
#include <cmath>
#include "Application.h"

StateBattleMode::StateBattleMode(TetrisRenderer& renderer)
    : m_left(10, 22, 3, 19)
    , m_right(10, 22, 3, 19)
    , m_tetrisRenderer(renderer)
{
}

void StateBattleMode::OnStart()
{
    m_left.StartGame();
    m_right.StartGame();
}

void StateBattleMode::OnUpdate()
{
    UpdateTetrisGame(m_left);
    UpdateTetrisGame(m_right);
}

void StateBattleMode::OnRender()
{
    m_tetrisRenderer.SetTetrisRenderDesc(MakeDescLeft());
    m_tetrisRenderer.DrawTetris(m_left);

    m_tetrisRenderer.SetTetrisRenderDesc(MakeDescRight());
    m_tetrisRenderer.DrawTetris(m_right);
}

void StateBattleMode::OnSdlEvent(const SDL_Event& e)
{
    // User presses a key
    if (e.type == SDL_KEYDOWN)
    {
        //Select surfaces based on key press
        switch (e.key.keysym.sym)
        {
        case SDLK_g:
            m_left.ToggleGravity();
            break;

        case SDLK_SPACE:
            if (m_left.GetPlayState() == PS_Control)
            {
                m_left.HardDrop();
                Mix_PlayChannel(-1, g_pWavDrop, 0);
            }
            else if (m_left.GetPlayState() == PS_Ready)
            {
                m_left.StartGame();
            }
            break;

        case SDLK_z:
            m_left.RotateAntiClockwise();
            break;

        case SDLK_UP:
        case SDLK_x:
            m_left.RotateClockwise();
            break;

        case SDLK_c:
        case SDLK_LSHIFT:
            m_left.Hold();
            break;

        case SDLK_r:
            m_left.Restart();
            break;

        case SDLK_q:
            m_left.Reset();
            Mix_PauseMusic();
            Mix_RewindMusic();
            Application::gsm.SetState(&*Application::pStateMainMenu);
            break;

        default:
            break;
        }
    }
}

void StateBattleMode::OnEnd()
{
}

TetrisRenderDesc StateBattleMode::MakeDescLeft()
{
    TetrisRenderDesc desc = { 0 };
    const int ClientHeight = Application::GetClientAreaHeight();
    const int ClientWidth = Application::GetClientAreaWidth();
    const int PxBlockSize = static_cast<int>(std::round(Application::GetClientAreaHeight() / 600.f * 24.f));
    const int PxPlayFIeldWidth = m_left.GetPlayField().GetWidth() * PxBlockSize;
    const int NextCount = 5;

    desc.pxBlockSize = PxBlockSize;
    desc.pxPlayFieldX = 100;
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

TetrisRenderDesc StateBattleMode::MakeDescRight()
{
    TetrisRenderDesc desc = { 0 };
    const int ClientHeight = Application::GetClientAreaHeight();
    const int ClientWidth = Application::GetClientAreaWidth();
    const int PxBlockSize = static_cast<int>(std::round(Application::GetClientAreaHeight() / 600.f * 24.f));
    const int PxPlayFIeldWidth = m_left.GetPlayField().GetWidth() * PxBlockSize;
    const int NextCount = 5;

    desc.pxBlockSize = PxBlockSize;
    desc.pxPlayFieldX = ClientWidth - PxPlayFIeldWidth - 50;;
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

void StateBattleMode::UpdateTetrisGame(TetrisGame& tetrisGame)
{
    switch (tetrisGame.GetPlayState())
    {
    case PS_Dropped:
    {
        DoDrop(tetrisGame);
    }
    break;
    case PS_Control:
    {
        tetrisGame.OnControl();

        DoShift(tetrisGame);
    }
    break;
    }
}
