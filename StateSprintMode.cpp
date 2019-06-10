#include "Pch.h"
#include "StateSprintMode.h"
#include "TetrisGame.h"
#include "TetrisRenderer.h"
#include "Application.h"

namespace
{
    const int SprintCompleteLines = 40;
}

StateSprintMode::StateSprintMode(TetrisRenderer& tetrisRenderer)
    : StateOnePlayer(tetrisRenderer)
    , m_sprintTimer()
    , m_uiSprintComplete("", ColorDarkBrown, ColorDarkYellow)
{
}

void StateSprintMode::OnStart()
{
    StateOnePlayer::OnStart();

    PlayField& playField = m_tetrisGame.GetPlayField();

    m_uiSprint.Clear();
    const int PxOffset = 16;
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_LineClear), ColorDarkBrown)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetTotalClearedLines, &m_tetrisGame), MinTextBoxHeight + PxOffset)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Score), ColorDarkBrown)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetScore, &m_tetrisGame), MinTextBoxHeight + PxOffset)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Time), ColorDarkBrown)));
    m_uiSprint.AddUI(std::shared_ptr<UI>(new UITimer(std::bind(&SprintTimer::GetTimerTime, &m_sprintTimer), ColorDarkBlue, MinTextBoxHeight + PxOffset)));

    m_uiSprintComplete.SetContent(Application::GetString(StringTable::SI_Sprint_Complete));

    TetrisRenderDesc desc = m_tetrisRenderer.GetTetrisRenderDesc();
    m_uiSprint.SetXy(desc.pxPlayFieldX - m_uiSprint.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);
}

void StateSprintMode::OnUpdate()
{
    switch (m_tetrisGame.GetPlayState())
    {
    case PS_Dropped:
    {
        DoDrop();
    }
    break;
    case PS_Control:
    {
        m_tetrisGame.OnControl();

        DoShift();
    }
    break;
    }

    if (m_tetrisGame.GetPlayState() == PS_Win || m_tetrisGame.GetPlayState() == PS_GameOver )
    {
        m_sprintTimer.Stop();
    }
    else
    {
        m_sprintTimer.Start();
    }
}

void StateSprintMode::OnRender()
{
    int screenWidth = Application::GetClientAreaWidth();
    int screenHeight = Application::GetClientAreaHeight();

    StateOnePlayer::OnRender();

    m_uiSprint.Render();

    if (m_tetrisGame.GetTotalClearedLines() >= SprintCompleteLines)
    {
        m_sprintTimer.Stop();
        m_uiSprintComplete.SetXy((screenWidth - m_uiSprintComplete.GetWidth()) / 2, (screenHeight - m_uiSprintComplete.GetHeight()) / 2);
        m_uiSprintComplete.Render();
        m_tetrisGame.Win();
    }
}

void StateSprintMode::OnSdlEvent(const SDL_Event& e)
{
    StateOnePlayer::OnSdlEvent(e);

    // User presses a key
    if (e.type == SDL_KEYDOWN)
    {
        //Select surfaces based on key press
        switch (e.key.keysym.sym)
        {

        case SDLK_q:
            m_sprintTimer.Stop();
            break;

        default:
            break;

        }
    }
}
