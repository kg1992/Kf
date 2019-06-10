#include "Pch.h"
#include "StateSprintMode.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"
#include "Application.h"

StateSprintMode::StateSprintMode(TetrisRenderer& tetrisRenderer)
    : StateOnePlayer(tetrisRenderer)
    , m_sprintTimer()
    , m_uiReady(Application::GetString(StringTable::SI_Ready), ColorDarkBrown, ColorDarkYellow)
    , m_uiGameOver(Application::GetString(StringTable::SI_GameOver), ColorDarkBrown, ColorDarkYellow)
    , m_uiSprintComplete(Application::GetString(StringTable::SI_Sprint_Complete), ColorDarkBrown, ColorDarkYellow)
{
}

void StateSprintMode::OnStart()
{
    PlayField& playField = m_tetrisGame.GetPlayField();

    m_uiSprint.Clear();
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_LineClear), ColorDarkBrown)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetTotalClearedLines, &m_tetrisGame), MinTextBoxHeight + 16)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Score), ColorDarkBrown)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetScore, &m_tetrisGame), MinTextBoxHeight + 16)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Time), ColorDarkBrown)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UITimer(std::bind(&SprintTimer::GetTimerTime, &m_sprintTimer), ColorDarkBlue, MinTextBoxHeight + 16)));

    m_uiReady.SetContent(Application::GetString(StringTable::SI_Ready));
    m_uiGameOver.SetContent(Application::GetString(StringTable::SI_GameOver));
    m_uiSprintComplete.SetContent(Application::GetString(StringTable::SI_Sprint_Complete));

    TetrisRenderDesc desc = MakeRenderDesc();
    m_tetrisRenderer.SetTetrisRenderDesc(desc);
    m_uiSprint.SetXy(desc.pxPlayFieldX - m_uiSprint.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);
}

void StateSprintMode::OnUpdate()
{
    switch (m_tetrisGame.GetPlayState())
    {
    case PS_Dropped:
    {
        auto result = m_tetrisGame.OnDrop();
        if (result.lineClearCount != 0)
        {
            Mix_PlayChannel(-1, g_pWavExplosion, 0);
        }
    }
    break;
    case PS_Control:
    {
        m_tetrisGame.OnControl();

        int dx = 0;
        if (Application::state[SDL_SCANCODE_DOWN]) m_tetrisGame.SoftDrop();
        if (Application::state[SDL_SCANCODE_RIGHT]) dx += 1;
        if (Application::state[SDL_SCANCODE_LEFT]) dx += -1;
        m_tetrisGame.Shift(dx);
    }
    break;
    }
}

void StateSprintMode::OnRender()
{
    int screenWidth = Application::GetClientAreaWidth();
    int screenHeight = Application::GetClientAreaHeight();

    m_tetrisRenderer.DrawTetris(m_tetrisGame);

    m_uiSprint.Render();

    if (m_tetrisGame.GetPlayState() == PS_GameOver) {
        m_sprintTimer.Stop();
        m_uiGameOver.SetXy((screenWidth - m_uiGameOver.GetWidth()) / 2, (screenHeight - m_uiGameOver.GetHeight()) / 2);
        m_uiGameOver.Render();
    }
    else if (m_tetrisGame.GetPlayState() == PS_Ready)
    {
        m_uiReady.SetXy((screenWidth - m_uiReady.GetWidth()) / 2, (screenHeight - m_uiReady.GetHeight()) / 2);
        m_uiReady.Render();
    }

    if (m_tetrisGame.GetTotalClearedLines() >= 40)
    {
        m_sprintTimer.Stop();
        m_uiSprintComplete.SetXy((screenWidth - m_uiSprintComplete.GetWidth()) / 2, (screenHeight - m_uiSprintComplete.GetHeight()) / 2);
        m_uiSprintComplete.Render();
        m_tetrisGame.Win();
    }
}

void StateSprintMode::OnSdlEvent(const SDL_Event& e)
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
                m_sprintTimer.Start();
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
            m_sprintTimer.Stop();
            Application::gsm.SetState(&*Application::pStateMainMenu);
            break;

        default:
            break;
        }
    }
}
