#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "FiniteStateMachine.h"
#include <memory>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

enum GameState
{
    GS_MainMenu,
    GS_Sprint,
    GS_Infinite,
};

struct Options
{
    int resolutionIndex = 0;
};

class Application
{
public:
    static int Start();
    static void Quit();
    static void ResizeWindow(int width, int height);
    static int GetClientAreaWidth();
    static int GetClientAreaHeight();

    static FiniteStateMachine gsm;
    static std::shared_ptr<State> pStateMainMenu;
    static std::shared_ptr<State> pStateSprintMode;
    static std::shared_ptr<State> pStateInfiniteMode;
    static std::shared_ptr<State> pStateOptions;
    static const Uint8* state;
    static SDL_Window* window;
    static Options options;

private:
    static bool quit;
};

// minimum ui textbox height
const int MinTextBoxHeight = 28;
// white color
const SDL_Color ColorWhite = { 0xff, 0xff, 0xff };
// black color
const SDL_Color ColorBlack = { 0x00, 0x00, 0x00 };
// Block size in pixel
const int PxBlockSize = 16;

extern Mix_Chunk* g_pWavLevelup;
extern Mix_Chunk* g_pWavExplosion;
extern Mix_Chunk* g_pWavDrop;
extern TTF_Font* g_pFont;

#endif
