#include "StateSprintMode.h"
#include <SDL_mixer.h>
#include "TetrisGame.h"
#include "TetrisRenderer.h"
#include "Application.h"

StateSprintMode::StateSprintMode(TetrisRenderer& tetrisRenderer)
    : tetrisGame(10, 22, 3, 19)
    , tetrisRenderer(tetrisRenderer)
    , sprintTimer()
    , readyShow(Application::GetString(StringTable::SI_Ready), ColorDarkBrown, ColorDarkYellow)
    , gameoverShow(Application::GetString(StringTable::SI_GameOver), ColorDarkBrown, ColorDarkYellow)
    , sprintCompleteShow(Application::GetString(StringTable::SI_Sprint_Complete), ColorDarkBrown, ColorDarkYellow)
{
}

void StateSprintMode::OnStart()
{
    PlayField& playField = tetrisGame.GetPlayField();

    sprintUI.Clear();
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_LineClear), ColorDarkBrown)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetTotalClearedLines, &tetrisGame), MinTextBoxHeight)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Score), ColorDarkBrown)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UINumberBox(0, std::bind(&TetrisGame::GetScore, &tetrisGame), MinTextBoxHeight)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(Application::GetString(StringTable::SI_Time), ColorDarkBrown)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITimer(std::bind(&SprintTimer::GetTimerTime, &sprintTimer), ColorDarkBlue, MinTextBoxHeight)));

    readyShow.SetContent(Application::GetString(StringTable::SI_Ready));
    gameoverShow.SetContent(Application::GetString(StringTable::SI_GameOver));
    sprintCompleteShow.SetContent(Application::GetString(StringTable::SI_Sprint_Complete));

    TetrisRenderDesc desc = { 0 };
    desc.pxBlockSize = static_cast<int>(std::round(Application::GetClientAreaHeight() / 600.f * 16.f));
    desc.pxPlayFieldX = (Application::GetClientAreaWidth() - tetrisGame.GetPlayField().GetWidth() * desc.pxBlockSize) / 2;
    desc.pxPlayFieldY = (Application::GetClientAreaHeight() - tetrisGame.GetPlayField().GetHeight() * desc.pxBlockSize) / 2;
    desc.pxHoldX = desc.pxPlayFieldX - desc.pxBlockSize * 5;
    desc.pxHoldY = desc.pxPlayFieldY;
    desc.nextCount = 5;
    for (int i = 0; i < 5; ++i)
    {
        desc.pxNextX[i] = desc.pxPlayFieldX + desc.pxBlockSize * 11;
        desc.pxNextY[i] = desc.pxPlayFieldY + i * desc.pxBlockSize * 4;
    }
    desc.visibleLines = 20;

    tetrisRenderer.SetTetrisRenderDesc(desc);

    sprintUI.SetXy(desc.pxPlayFieldX - sprintUI.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);
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
    int screenWidth = Application::GetClientAreaWidth();
    int screenHeight = Application::GetClientAreaHeight();

    tetrisRenderer.DrawTetris(tetrisGame);

    sprintUI.Render();

    if (tetrisGame.GetPlayState() == PS_GameOver) {
        sprintTimer.Stop();
        gameoverShow.SetXy((screenWidth - gameoverShow.GetWidth()) / 2, (screenHeight - gameoverShow.GetHeight()) / 2);
        gameoverShow.Render();
    }
    else if (tetrisGame.GetPlayState() == PS_Ready)
    {
        readyShow.SetXy((screenWidth - readyShow.GetWidth()) / 2, (screenHeight - readyShow.GetHeight()) / 2);
        readyShow.Render();
    }

    if (tetrisGame.GetTotalClearedLines() >= 5)
    {
        sprintTimer.Stop();
        sprintCompleteShow.SetXy((screenWidth - sprintCompleteShow.GetWidth()) / 2, (screenHeight - sprintCompleteShow.GetHeight()) / 2);
        sprintCompleteShow.Render();
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
