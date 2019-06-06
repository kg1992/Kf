#include "Application.h"
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
#include "FiniteStateMachine.h"
#include "StateMainMenu.h"
#include "StateSprintMode.h"
#include "StateInfiniteMode.h"

FiniteStateMachine Application::gsm;
std::shared_ptr<State> Application::pStateMainMenu;
std::shared_ptr<State> Application::pStateSprintMode;
std::shared_ptr<State> Application::pStateInfiniteMode;
const Uint8* Application::state;
bool Application::quit = false;

const int FontSize = 18;
TTF_Font* g_pFont = nullptr;

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

Mix_Music* g_pMusic;
Mix_Chunk* g_pWavLevelup;
Mix_Chunk* g_pWavExplosion;
Mix_Chunk* g_pWavDrop;

int Application::Start()
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

    // Initialize SDL_ttf
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

    // Load music
    g_pMusic = Mix_LoadMUS("Tetris Official Theme song.mp3");
    if (g_pMusic == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    // Load sound effects
    g_pWavLevelup = Mix_LoadWAV("Levelup.wav");
    g_pWavExplosion = Mix_LoadWAV("Explosion.wav");
    g_pWavDrop = Mix_LoadWAV("Drop.wav");

    // Load font
    g_pFont = TTF_OpenFont("NanumBarunGothicBold.ttf", FontSize);
    if (g_pFont == nullptr)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    DisplayDpiAndFontsize(FontSize);

    Mix_PlayMusic(g_pMusic, -1);
    Mix_PauseMusic();

    MinoRenderer minoRenderer(pRenderer);
    minoRenderer.Load();

    TetrisRenderer tetrisRenderer(minoRenderer);

    pStateMainMenu.reset(new StateMainMenu(pRenderer));
    pStateSprintMode.reset(new StateSprintMode(pRenderer, tetrisRenderer));
    pStateInfiniteMode.reset(new StateInfiniteMode(pRenderer, tetrisRenderer));

    gsm.SetState(&*pStateMainMenu);

    state = SDL_GetKeyboardState(nullptr);

    std::chrono::high_resolution_clock frameClock;
    std::chrono::high_resolution_clock::time_point frameTick = frameClock.now();
    // While application is running
    SDL_Event sdlEvent;
    while (!quit)
    {
        // Handle events on queue
        while (SDL_PollEvent(&sdlEvent) != 0)
        {
            gsm.OnSdlEvent(sdlEvent);
            // User requests quit
            if (sdlEvent.type == SDL_QUIT)
                quit = true;
        }

        SDL_SetRenderDrawColor(pRenderer, 0xF5, 0xF5, 0xDC, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(pRenderer);

        gsm.OnUpdate();

        gsm.OnRender();

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
    }

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

void Application::Quit()
{
    quit = true;
}