#include "Pch.h"
#include "StateDigDownMode.h"
#include "Application.h"

StateDigDownMode::StateDigDownMode(TetrisRenderer& renderer)
    : StateOnePlayer(renderer)
{
}

void StateDigDownMode::OnStart()
{
    StateOnePlayer::OnStart();

    lastGrow = clock.now();
    
    m_uiDigDown.Clear();
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_LineClear), ColorDarkBrown)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetTotalClearedLines, &m_tetrisGame), MinTextBoxHeight)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Level), ColorDarkBrown)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetLevel, &m_tetrisGame), MinTextBoxHeight)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Combo), ColorDarkBrown)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetCombo, &m_tetrisGame), MinTextBoxHeight)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Score), ColorDarkBrown)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetScore, &m_tetrisGame), MinTextBoxHeight)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_GrowDuration), ColorDarkBrown)));
    m_uiDigDown.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&StateDigDownMode::GetGrowTime, this), MinTextBoxHeight)));

    TetrisRenderDesc desc = m_tetrisRenderer.GetTetrisRenderDesc();
    m_uiDigDown.SetXy(desc.pxPlayFieldX - m_uiDigDown.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);
}

void StateDigDownMode::OnUpdate()
{
    switch (m_tetrisGame.GetPlayState())
    {
    case PS_Dropped:
    {
        DoDrop();
        break;
    }
    case PS_Control:
    {
        m_tetrisGame.OnControl();

        DoShift();
        break;
    }
    }
    
    auto now = clock.now();
    std::chrono::steady_clock::duration duration = now - lastGrow;
    std::chrono::steady_clock::rep den = std::chrono::steady_clock::period::den;
   
    int growTime = GetGrowTime();

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastGrow).count() >= growTime)
    {
        m_tetrisGame.AddGarbageLines(1);
        lastGrow = now;
    }
}

void StateDigDownMode::OnRender() 
{
    StateOnePlayer::OnRender();

    m_uiDigDown.Render();
}

int StateDigDownMode::GetGrowTime()
{
    int growTime = 4000 - m_tetrisGame.GetLevel() * 50;
    if (growTime < 0)
        growTime = 1;
    return static_cast<int>(growTime);
}
