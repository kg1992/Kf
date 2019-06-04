// ref) What is SDL? : http://lazyfoo.net/tutorials/SDL/
// ref) What is Super Rotation System : https://tetris.fandom.com/wiki/SRS
// ref) How to score? : https://tetris.fandom.com/wiki/Scoring
// ref) What is back-toback bonus? : https://harddrop.com/wiki/Back-to-Back
// ref) How to calculate drop speed : https://tetris.fandom.com/wiki/Drop
// ref) colinfahey tetris : https://www.colinfahey.com/tetris/tetris.html
// ref) Lock Delay : https://tetris.fandom.com/wiki/Lock_delay
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <functional>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "PresetBoards.h"
#include "Texture.h"   
#include "TetrisGame.h"
#include "TetrisRenderer.h"
#include "UI.h"
#include "SprintTimer.h"

namespace filesystem = std::experimental::filesystem;

// window client area width in pixel
const int ScreenWidth = 800;
// window client area height in pixel
const int ScreenHeight = 600;

Mix_Music* g_pMusic = nullptr;
Mix_Chunk* g_pWavLevelup;
Mix_Chunk* g_pWavExplosion;
Mix_Chunk* g_pWavDrop;  

const int FontSize = 18;
TTF_Font* g_pFont = nullptr;

const SDL_Color ColorWhite = { 0xff, 0xff, 0xff };
const SDL_Color ColorBlack = { 0x00, 0x00, 0x00 };

filesystem::path GetPrefPath()
{
    return SDL_GetPrefPath("Kf", "Tetris");
}

bool IsWriteEnabled()
{
    return filesystem::is_directory(GetPrefPath());
}

enum GameState
{
    GS_MainMenu,
    GS_Sprint,
    GS_Infinite,
};

void DisplayDpiAndFontsize(float ptFontSize)
{
    const float PointsPerInch = 72.272f;
    const int DisplayCount = SDL_GetNumVideoDisplays();
    for (int i = 0; i < DisplayCount; ++i)
    {
        float ddpi, hdpi, vdpi;
        SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi);
        printf("%f, %f, %f\n", ddpi, hdpi, vdpi);
        float point = ptFontSize;
        float inch = point / PointsPerInch;
        float pixel = inch * vdpi;
        printf("point: %f\ninch : %f\npixel : %.0f\n", point, inch, pixel);
    }
}

int main(int argc, char** argv)
{
    // Iniitlaize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not be initialized!");
        return 1;
    }

    // The window we will be rendering to
    SDL_Window* pWindow = SDL_CreateWindow("KFTetris v0.0.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, 0);
    if (!pWindow)
    {
        printf("SDL could not create window");
        return 1;
    }

    // Create renderer for window
    SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pRenderer)
    {
        printf("Unable to create Renderer : %s\n", SDL_GetError());
        return 1;
    }

    // Update the surface
    SDL_UpdateWindowSurface(pWindow);   

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf(" SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    // Initialize SDL-ttf
    if (TTF_Init() == -1)
    {
        printf(" SDL_ttf could not initialize! SDL_ttf Error : %s\n", TTF_GetError());
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    //Load music
    g_pMusic = Mix_LoadMUS("Tetris Official Theme song.mp3");
    if (g_pMusic == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }
    
    g_pWavLevelup = Mix_LoadWAV("Levelup.wav");
    g_pWavExplosion = Mix_LoadWAV("Explosion.wav");
    g_pWavDrop = Mix_LoadWAV("Drop.wav");

    g_pFont = TTF_OpenFont("arial.ttf", FontSize);
    if (g_pFont == nullptr)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    DisplayDpiAndFontsize(FontSize);

    TetrisGame tetrisGame(10, 22, 3, 19);
    PlayField& playField = tetrisGame.GetPlayField();

    MinoRenderer minoRenderer(pRenderer);
    minoRenderer.Load();

    const int BlockSize = 16;
    TetrisRenderDesc desc = { 0 };
    // Pivot point of the playfield; left bottom coner of the field. in screen space coordinate.
    desc.pxPlayFieldX = (ScreenWidth - playField.GetWidth() * BlockSize) / 2;
    // Pivot point of the playfield; Left bottom corner of the field. in screen space coordinate.
    desc.pxPlayFieldY = 100;
    desc.pxHoldX = desc.pxPlayFieldX - 64 - 16;
    desc.pxHoldY = desc.pxPlayFieldY;
    desc.nextCount = 5;
    for (int i = 0; i < 5; ++i)
    {
        desc.pxNextX[i] = desc.pxPlayFieldX + BlockSize * 11;
        desc.pxNextY[i] = desc.pxPlayFieldY + i * 64;
    }
    
    desc.visibleLines = 20;
    TetrisRenderer tetrisRenderer(minoRenderer);
    tetrisRenderer.SetTetrisRenderDesc(desc);

    Mix_PlayMusic(g_pMusic, -1);
    Mix_PauseMusic();

    UIStack mainMenu;
    mainMenu.SetXy(200, 200);
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "Press number key to select", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "1. Infinity Mode", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "2. Sprint 40", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "3. Quit", ColorBlack)));

    const int MinWidth = 28;

    UIStack infiniteUI;
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "LINES CLEARED", ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetTotalClearedLines, &tetrisGame), MinWidth)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "LEVEL", ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetLevel, &tetrisGame), MinWidth)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "BONUS", ColorBlack)));
    infiniteUI.SetXy(desc.pxPlayFieldX - infiniteUI.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);

    std::shared_ptr<UITextBox> bonusShow(new UITextBox(pRenderer, "", ColorBlack));
    bonusShow->SetMinHeight(MinWidth);
    infiniteUI.AddUI(bonusShow);
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "COMBO", ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetCombo, &tetrisGame), MinWidth)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "SCORE", ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetScore, &tetrisGame), MinWidth)));

    SprintTimer sprintTimer;
    UIStack sprintUI;
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "LINES CLEARED", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetTotalClearedLines, &tetrisGame), MinWidth)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "SCORE", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UINumberBox(pRenderer, 0, std::bind(&TetrisGame::GetScore, &tetrisGame), MinWidth)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "TIME", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITimer(pRenderer, std::bind(&SprintTimer::GetTimerTime, &sprintTimer), ColorBlack, MinWidth)));
    sprintUI.SetXy(desc.pxPlayFieldX - sprintUI.GetWidth(), desc.pxHoldY + desc.pxBlockSize * 5);

    Texture readyShow(pRenderer);
    readyShow.LoadFromRenderedText("Press Space To Start", g_pFont, ColorBlack);
    Texture gameoverShow(pRenderer);
    gameoverShow.LoadFromRenderedText("Game Over! Press R to restart. Press Q to return to main menu.", g_pFont, ColorBlack);
    Texture sprintCompleteShow(pRenderer);
    sprintCompleteShow.LoadFromRenderedText("Done! Check the time on left. Press Q to return to main menu", g_pFont, ColorBlack);
    
    GameState gameState = GS_MainMenu;

    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    
    std::chrono::high_resolution_clock frameClock;
    std::chrono::high_resolution_clock::time_point frameTick = frameClock.now();
    // While application is running
    bool quit = false;
    SDL_Event sdlEvent;
    while (!quit)
    {
        // Handle events on queue
        while (SDL_PollEvent(&sdlEvent) != 0)
        {
            // User requests quit
            if (sdlEvent.type == SDL_QUIT)
                quit = true;

            // User presses a key
            if (sdlEvent.type == SDL_KEYDOWN)
            {
                //Select surfaces based on key press
                switch (sdlEvent.key.keysym.sym)
                {
                case SDLK_1:
                {
                    if (gameState == GS_MainMenu)
                    {
                        gameState = GS_Infinite;
                        Mix_ResumeMusic();
                    }
                    break;
                }
                case SDLK_2:
                {
                    if (gameState == GS_MainMenu)
                    {
                        gameState = GS_Sprint;
                        Mix_ResumeMusic();
                    }
                    break;
                }
                case SDLK_3:
                {
                    if (gameState == GS_MainMenu)
                    {
                        quit = true;
                    }
                    break;
                }
                    break;
                case SDLK_F5:
                {
                    if (gameState == GS_Infinite)
                    {
                        if (tetrisGame.GetPlayState() == PS_Control)
                        {
                            std::ofstream ofs(GetPrefPath() / "SavedState.dat");
                            tetrisGame.SaveState(ofs);
                        }
                    }
                    break;
                }
                case SDLK_F9:
                {
                    if (gameState == GS_Infinite)
                    {
                        if (tetrisGame.GetPlayState() == PS_Control)
                        {
                            std::ifstream ifs(GetPrefPath() / "SavedState.dat");
                            tetrisGame.LoadState(ifs);
                        }
                    }
                    break;
                }

                case SDLK_g:
                    tetrisGame.ToggleGravity();
                    break;

                case SDLK_SPACE:
                    if (gameState == GS_Infinite || gameState == GS_Sprint)
                    {
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
                    if (gameState == GS_Infinite || gameState == GS_Sprint)
                    {
                        tetrisGame.Restart();
                    }
                    break;

                case SDLK_q:
                    if (gameState == GS_Infinite)
                    {
                        tetrisGame.Reset();
                        Mix_PauseMusic();
                        Mix_RewindMusic();
                        gameState = GS_MainMenu;
                    }
                    if (gameState == GS_Sprint)
                    {
                        tetrisGame.Reset();
                        Mix_PauseMusic();
                        Mix_RewindMusic();
                        sprintTimer.Stop();
                        gameState = GS_MainMenu;
                    }
                    break;

                default:
                    break;
                }
            }
        }

        switch (tetrisGame.GetPlayState())
        {
        case PS_Dropped:
        {
            auto result = tetrisGame.OnDrop();
            if (result.lineClearCount != 0)
            {
                LineClearInfo lci = LineClearInfoTable[result.lct];
                std::string msg;
                if (result.backToBack)
                    msg = "B2B ";
                msg += lci.msg;
                bonusShow->SetContent(msg, ColorBlack);
                Mix_PlayChannel(-1, g_pWavExplosion, 0);
            }
            else
            {
                bonusShow->FreeContent();
            }
        }
        break;
        case PS_Control:
        {
            tetrisGame.OnControl();

            int dx = 0;
            if (state[SDL_SCANCODE_DOWN]) tetrisGame.SoftDrop();
            if (state[SDL_SCANCODE_RIGHT]) dx += 1;
            if (state[SDL_SCANCODE_LEFT]) dx += -1;
            tetrisGame.Shift(dx);
        }
        break;
        }

        SDL_SetRenderDrawColor(pRenderer, 0xF5, 0xF5, 0xDC, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(pRenderer);

        if (gameState == GS_MainMenu)
        {
            mainMenu.Render();
        }
        else if (gameState == GS_Infinite)
        {
            tetrisRenderer.DrawTetris(tetrisGame);

            infiniteUI.Render();

            if (tetrisGame.GetPlayState() == PS_GameOver)
                gameoverShow.Render((ScreenWidth - gameoverShow.GetWidth()) / 2, (ScreenHeight - gameoverShow.GetHeight()) / 2);
            else if (tetrisGame.GetPlayState() == PS_Ready)
                readyShow.Render((ScreenWidth - readyShow.GetWidth()) / 2, (ScreenHeight - readyShow.GetHeight()) / 2);
        }
        else if (gameState == GS_Sprint)
        {
            tetrisRenderer.DrawTetris(tetrisGame);

            sprintUI.Render();

            if (tetrisGame.GetPlayState() == PS_GameOver) {
                sprintTimer.Stop();
                gameoverShow.Render((ScreenWidth - gameoverShow.GetWidth()) / 2, (ScreenHeight - gameoverShow.GetHeight()) / 2);
            }
            else if (tetrisGame.GetPlayState() == PS_Ready)
                readyShow.Render((ScreenWidth - readyShow.GetWidth()) / 2, (ScreenHeight - readyShow.GetHeight()) / 2);

            if( tetrisGame.GetTotalClearedLines() >= 40 )
            {
                sprintTimer.Stop();
                sprintCompleteShow.Render((ScreenWidth - sprintCompleteShow.GetWidth()) / 2, (ScreenHeight - sprintCompleteShow.GetHeight()) / 2);
                tetrisGame.Win();
            }
        }

        // Update screen
        SDL_RenderPresent(pRenderer);;

        // 0.016 seconds = farme length
        const long long NsFrameLength = 16000000;
        std::chrono::nanoseconds frameTime;
        do
        {
            frameTime = frameClock.now() - frameTick;
        } while (frameTime.count() < NsFrameLength);

        frameTick = frameClock.now();
    } // while(!quit)

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    TTF_CloseFont(g_pFont);

    Mix_FreeChunk(g_pWavDrop);
    Mix_FreeChunk(g_pWavExplosion);
    Mix_FreeChunk(g_pWavLevelup);
    Mix_FreeMusic(g_pMusic);
    minoRenderer.Free();
    
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    SDL_Quit();

    return 0;
}
