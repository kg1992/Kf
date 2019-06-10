#include "Pch.h"
#include "StateInfiniteMode.h"
#include "Application.h"

StateInfiniteMode::StateInfiniteMode(TetrisRenderer& tetrisRenderer)
    : StateOnePlayer(tetrisRenderer)
    , m_uiReady("", ColorDarkBrown, ColorDarkYellow)
    , m_uiGameOver("", ColorDarkBrown, ColorDarkYellow)
{
}

void StateInfiniteMode::OnStart()
{
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

    m_uiReady.SetContent(Application::GetString(StringTable::SI_Ready));
    m_uiGameOver.SetContent(Application::GetString(StringTable::SI_GameOver));

    TetrisRenderDesc desc = MakeRenderDesc();
    m_tetrisRenderer.SetTetrisRenderDesc(desc);
    m_uiInfinite.SetXy(desc.pxPlayFieldX - m_uiInfinite.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);
}

void StateInfiniteMode::OnUpdate()
{
    switch (m_tetrisGame.GetPlayState())
    {
    case PS_Dropped:
    {
        auto result = m_tetrisGame.OnDrop();
        if (result.lineClearCount != 0)
        {
            Mix_PlayChannel(-1, g_pWavExplosion, 0);

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

        int dx = 0;
        if (Application::state[SDL_SCANCODE_DOWN]) m_tetrisGame.SoftDrop();
        if (Application::state[SDL_SCANCODE_RIGHT]) dx += 1;
        if (Application::state[SDL_SCANCODE_LEFT]) dx += -1;
        m_tetrisGame.Shift(dx);
    }
    break;
    }
}

void StateInfiniteMode::OnRender()
{
    int screenWidth = Application::GetClientAreaWidth();
    int screenHeight = Application::GetClientAreaHeight();

    m_tetrisRenderer.DrawTetris(m_tetrisGame);

    m_uiInfinite.Render();

    if (m_tetrisGame.GetPlayState() == PS_GameOver)
    {
        m_uiGameOver.SetXy((screenWidth - m_uiGameOver.GetWidth()) / 2, (screenHeight - m_uiGameOver.GetHeight()) / 2);
        m_uiGameOver.Render();
    }
    else if (m_tetrisGame.GetPlayState() == PS_Ready)
    {
        m_uiReady.SetXy((screenWidth - m_uiReady.GetWidth()) / 2, (screenHeight - m_uiReady.GetHeight()) / 2);
        m_uiReady.Render();
    }
}

void StateInfiniteMode::OnSdlEvent(const SDL_Event& e)
{
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