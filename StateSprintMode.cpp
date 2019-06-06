#include "StateSprintMode.h"
#include <SDL_mixer.h>
#include "TetrisGame.h"
#include "TetrisRenderer.h"
#include "Application.h"

StateSprintMode::StateSprintMode(SDL_Renderer* pRenderer, TetrisRenderer& tetrisRenderer)
    : tetrisGame(10, 22, 3, 19)
    , tetrisRenderer(tetrisRenderer)
    , sprintTimer()
    , readyShow(pRenderer)
    , gameoverShow(pRenderer)
    , sprintCompleteShow(pRenderer)
{
    PlayField& playField = tetrisGame.GetPlayField();

    TetrisRenderDesc desc = { 0 };
    // Pivot point of the playfield; left bottom coner of the field. in screen space coordinate.
    desc.pxPlayFieldX = (ScreenWidth - playField.GetWidth() * PxBlockSize) / 2;
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

    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "LINES CLEARED", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetTotalClearedLines, &tetrisGame), MinTextBoxHeight)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "SCORE", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetScore, &tetrisGame), MinTextBoxHeight)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "TIME", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITimer(pRenderer, std::bind(&SprintTimer::GetTimerTime, &sprintTimer), ColorBlack, MinTextBoxHeight)));
    sprintUI.SetXy(desc.pxPlayFieldX - sprintUI.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);

    readyShow.LoadFromRenderedText("Press Space To Start", g_pFont, ColorBlack);
    gameoverShow.LoadFromRenderedText("Game Over! Press R to restart. Press Q to return to main menu.", g_pFont, ColorBlack);
    sprintCompleteShow.LoadFromRenderedText("Done! Check the time on left. Press Q to return to main menu", g_pFont, ColorBlack);
}

void StateSprintMode::OnStart()
{
}

void StateSprintMode::OnUpdate()
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

void StateSprintMode::OnRender()
{
    tetrisRenderer.DrawTetris(tetrisGame);

    sprintUI.Render();

    if (tetrisGame.GetPlayState() == PS_GameOver) {
        sprintTimer.Stop();
        gameoverShow.Render((ScreenWidth - gameoverShow.GetWidth()) / 2, (ScreenHeight - gameoverShow.GetHeight()) / 2);
    }
    else if (tetrisGame.GetPlayState() == PS_Ready)
        readyShow.Render((ScreenWidth - readyShow.GetWidth()) / 2, (ScreenHeight - readyShow.GetHeight()) / 2);

    if (tetrisGame.GetTotalClearedLines() >= 40)
    {
        sprintTimer.Stop();
        sprintCompleteShow.Render((ScreenWidth - sprintCompleteShow.GetWidth()) / 2, (ScreenHeight - sprintCompleteShow.GetHeight()) / 2);
        tetrisGame.Win();
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
                sprintTimer.Start();
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
            sprintTimer.Stop();
            Application::gsm.SetState(&*Application::pStateMainMenu);
            break;

        default:
            break;
        }
    }
}
