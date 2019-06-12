#include "Pch.h"
#include "StateInfiniteMode.h"
#include "Application.h"

StateInfiniteMode::StateInfiniteMode(TetrisRenderer& tetrisRenderer)
    : StateOnePlayer(tetrisRenderer)
{
}

void StateInfiniteMode::OnStart()
{
    StateOnePlayer::OnStart();

    PlayField& playField = m_tetrisGame.GetPlayField();

    m_uiInfinite.Clear();
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_LineClear), ColorDarkBrown)));
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetTotalClearedLines, &m_tetrisGame), MinTextBoxHeight)));
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Level), ColorDarkBrown)));
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetLevel, &m_tetrisGame), MinTextBoxHeight)));
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Bonus), ColorDarkBrown)));
    m_uiBonus.reset(new UITextBox("_", ColorDarkBrown));
    m_uiBonus->SetMinHeight(MinTextBoxHeight);
    m_uiInfinite.AddUI(m_uiBonus);
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Combo), ColorDarkBrown)));
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetCombo, &m_tetrisGame), MinTextBoxHeight)));
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Score), ColorDarkBrown)));
    m_uiInfinite.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetScore, &m_tetrisGame), MinTextBoxHeight)));

    TetrisRenderDesc desc = m_tetrisRenderer.GetTetrisRenderDesc();
    m_uiInfinite.SetXy(desc.pxPlayFieldX - m_uiInfinite.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);
}

void StateInfiniteMode::OnUpdate()
{
    switch (m_tetrisGame.GetPlayState())
    {
    case PS_Dropped:
    {
        auto result = DoDrop(m_tetrisGame);
        if (result.lineClearCount != 0)
        {
            LineClearInfo lci = LineClearInfoTable[result.lct];
            std::string msg;
            if (result.backToBack)
                msg = "B2B ";
            msg += lci.msg;
            m_uiBonus->SetContent(msg);
            
            m_uiInfinite.SetX(m_tetrisRenderer.GetTetrisRenderDesc().pxPlayFieldX - m_uiInfinite.GetWidth());
            m_uiInfinite.SetY(m_tetrisRenderer.GetTetrisRenderDesc().pxHoldY + m_tetrisRenderer.GetTetrisRenderDesc().pxBlockSize * 5);
        }
    }
    break;
    case PS_Control:
    {
        m_tetrisGame.OnControl();

        DoShift(m_tetrisGame);
    }
    break;
    }
}

void StateInfiniteMode::OnRender()
{
    StateOnePlayer::OnRender();

    m_uiInfinite.Render();
}

void StateInfiniteMode::OnSdlEvent(const SDL_Event& e)
{
    StateOnePlayer::OnSdlEvent(e);

    // User presses a key
    if (e.type == SDL_KEYDOWN)
    {
        //Select surfaces based on key press
        switch (e.key.keysym.sym)
        {
        case SDLK_F5:
        {
            if (m_tetrisGame.GetPlayState() == PS_Control)
            {
                std::ofstream ofs(Application::GetPrefPath() / "SavedState.dat");
                m_tetrisGame.SaveState(ofs);
            }
            break;
        }
        case SDLK_F9:
        {
            if (m_tetrisGame.GetPlayState() == PS_Control)
            {
                std::ifstream ifs(Application::GetPrefPath() / "SavedState.dat");
                m_tetrisGame.LoadState(ifs);
            }
            break;
        }

        default:
            break;
        }
    }
}