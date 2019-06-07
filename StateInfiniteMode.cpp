#include "StateInfiniteMode.h"
#include <iostream>
#include <fstream>
#include <SDL_mixer.h>
#include "Application.h"

StateInfiniteMode::StateInfiniteMode(SDL_Renderer* pRenderer, TetrisRenderer& tetrisRenderer)
    : tetrisGame(10, 22, 3, 19)
    , tetrisRenderer(tetrisRenderer)
    , readyShow(pRenderer)
    , gameoverShow(pRenderer)
{
}

void StateInfiniteMode::OnStart()
{
    PlayField& playField = tetrisGame.GetPlayField();

    SDL_Renderer* pRenderer = Application::GetWindowRenderer();

    infiniteUI.Clear();
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, Application::GetString(StringTable::SI_LineClear), ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetTotalClearedLines, &tetrisGame), MinTextBoxHeight)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, Application::GetString(StringTable::SI_Level), ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetLevel, &tetrisGame), MinTextBoxHeight)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, Application::GetString(StringTable::SI_Bonus), ColorBlack)));
    std::shared_ptr<UITextBox> bonusShow(new UITextBox(pRenderer, "", ColorBlack));
    bonusShow->SetMinHeight(MinTextBoxHeight);
    infiniteUI.AddUI(bonusShow);
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, Application::GetString(StringTable::SI_Combo), ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetCombo, &tetrisGame), MinTextBoxHeight)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, Application::GetString(StringTable::SI_Score), ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetScore, &tetrisGame), MinTextBoxHeight)));

    readyShow.LoadFromRenderedText("Press Space To Start", g_pFont, ColorBlack);
    gameoverShow.LoadFromRenderedText("Game Over! Press R to restart. Press Q to return to main menu.", g_pFont, ColorBlack);

    TetrisRenderDesc desc = { 0 };
    // Pivot point of the playfield; left bottom coner of the field. in screen space coordinate.
    desc.pxPlayFieldX = (Application::GetClientAreaWidth() - tetrisGame.GetPlayField().GetWidth() * PxBlockSize) / 2;
    // Pivot point of the playfield; Left bottom corner of the field. in screen space coordinate.
    desc.pxPlayFieldY = 100;
    desc.pxHoldX = desc.pxPlayFieldX - 64 - 16;
    desc.pxHoldY = desc.pxPlayFieldY;
    desc.nextCount = 5;
    for (int i = 0; i < 5; ++i)
    {
        desc.pxNextX[i] = desc.pxPlayFieldX + PxBlockSize * 11;
        desc.pxNextY[i] = desc.pxPlayFieldY + i * 64;
    }

    desc.visibleLines = 20;
    tetrisRenderer.SetTetrisRenderDesc(desc);
    infiniteUI.SetXy(desc.pxPlayFieldX - infiniteUI.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);
}

void StateInfiniteMode::OnUpdate()
{
    switch (tetrisGame.GetPlayState())
    {
    case PS_Dropped:
    {
        auto result = tetrisGame.OnDrop();
        if (result.lineClearCount != 0)
        {
            Mix_PlayChannel(-1, g_pWavExplosion, 0);
        }
    }
    break;
    case PS_Control:
    {
        tetrisGame.OnControl();

        int dx = 0;
        if (Application::state[SDL_SCANCODE_DOWN]) tetrisGame.SoftDrop();
        if (Application::state[SDL_SCANCODE_RIGHT]) dx += 1;
        if (Application::state[SDL_SCANCODE_LEFT]) dx += -1;
        tetrisGame.Shift(dx);
    }
    break;
    }
}

void StateInfiniteMode::OnRender()
{
    int screenWidth = Application::GetClientAreaWidth();
    int screenHeight = Application::GetClientAreaHeight();

    tetrisRenderer.DrawTetris(tetrisGame);

    infiniteUI.Render();

    if (tetrisGame.GetPlayState() == PS_GameOver)
        gameoverShow.Render((screenWidth - gameoverShow.GetWidth()) / 2, (screenHeight - gameoverShow.GetHeight()) / 2);
    else if (tetrisGame.GetPlayState() == PS_Ready)
        readyShow.Render((screenWidth - readyShow.GetWidth()) / 2, (screenHeight - readyShow.GetHeight()) / 2);
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
            if (tetrisGame.GetPlayState() == PS_Control)
            {
                std::ofstream ofs(Application::GetPrefPath() / "SavedState.dat");
                tetrisGame.SaveState(ofs);
            }
            break;
        }
        case SDLK_F9:
        {
            if (tetrisGame.GetPlayState() == PS_Control)
            {
                std::ifstream ifs(Application::GetPrefPath() / "SavedState.dat");
                tetrisGame.LoadState(ifs);
            }
            break;
        }

        case SDLK_g:
            tetrisGame.ToggleGravity();
            break;

        case SDLK_SPACE:
            if (tetrisGame.GetPlayState() == PS_Control)
            {
                tetrisGame.HardDrop();
                Mix_PlayChannel(-1, g_pWavDrop, 0);
            }
            else if (tetrisGame.GetPlayState() == PS_Ready)
            {
                tetrisGame.StartGame();
            }
            break;

        case SDLK_z:
            tetrisGame.RotateAntiClockwise();
            break;

        case SDLK_UP:
        case SDLK_x:
            tetrisGame.RotateClockwise();
            break;

        case SDLK_c:
        case SDLK_LSHIFT:
            tetrisGame.Hold();
            break;

        case SDLK_r:
            tetrisGame.Restart();
            break;

        case SDLK_q:
            tetrisGame.Reset();
            Mix_PauseMusic();
            Mix_RewindMusic();
            Application::gsm.SetState(&*Application::pStateMainMenu);
            break;

        default:
            break;
        }
    }
}