// ref) What is SDL? : http://lazyfoo.net/tutorials/SDL/
// ref) What is Super Rotation System : https://tetris.fandom.com/wiki/SRS
// ref) How to score? : https://tetris.fandom.com/wiki/Scoring
// ref) What is back-toback bonus? : https://harddrop.com/wiki/Back-to-Back
// ref) How to calculate drop speed : https://tetris.fandom.com/wiki/Drop
// ref) colinfahey tetris : https://www.colinfahey.com/tetris/tetris.html
// ref) Lock Delay : https://tetris.fandom.com/wiki/Lock_delay
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <functional>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "PresetBoards.h"
#include "Texture.h"   
#include "Background.h"
#include "TetrisGame.h"

namespace filesystem = std::experimental::filesystem;

// window client area width in pixel
const int ScreenWidth = 800;
// window client area height in pixel
const int ScreenHeight = 600;

Mix_Music* g_pMusic = nullptr;
Mix_Chunk* g_pWavLevelup;
Mix_Chunk* g_pWavExplosion;
Mix_Chunk* g_pWavDrop;

TTF_Font* g_pFont = nullptr;

filesystem::path GetPrefPath()
{
    return SDL_GetPrefPath("Kf", "Tetris");
}

bool IsWriteEnabled()
{
    return filesystem::is_directory(GetPrefPath());
}

const SDL_Color ColorWhite = { 0xff, 0xff, 0xff };
const SDL_Color ColorBlack = { 0x00, 0x00, 0x00 };

enum PivotPoint {
    P_Center,
    P_Top,
    P_TopRight,
    P_Right,
    P_BottomRight,
    P_Bottom,
    P_LeftBottom,
    P_Left,
    P_LeftTop,
};

class MinoRenderer
{
public:
    enum { Step = PlayFieldMetric::PxBlockSize };

    MinoRenderer(SDL_Renderer* pRenderer)
        : m_minoTextureSlots{
            {0,Step * 2,Step,Step}, // Empty,
            {Step,0,Step,Step},// B_Red,
            {Step,Step,Step,Step},// B_Green,
            {0,Step,Step,Step},// B_Blue,
            {Step * 2,0,Step,Step},// B_Cyan,
            {Step * 2,Step,Step,Step},// B_Magenta,
            {Step * 3,0,Step,Step},// B_Yellow,
            {Step * 3,Step,Step,Step}// B_Orange,
    }
        , m_pRenderer(pRenderer)
        , m_minoTexture(pRenderer)
    {
    }

    void Load()
    {
        m_minoTexture.LoadFromFile("MInos.png");
    }

    void DrawMino(Block block, SDL_Rect* pRect)
    {
        m_minoTexture.renderPart(&m_minoTextureSlots[block], pRect);
    }

    void DrawGhostMino(Block block, SDL_Rect* pRect)
    {
        const Uint8 GhostMinoAlpha = 0x88;
        m_minoTexture.SetAlpha(GhostMinoAlpha);
        m_minoTexture.renderPart(&m_minoTextureSlots[block], pRect);
        m_minoTexture.SetAlpha(SDL_ALPHA_OPAQUE);
    }

    void DrawMinoOpaque(Block block, SDL_Rect* pRect)
    {
        switch (block)
        {
        case B_Red:       SDL_SetRenderDrawColor(m_pRenderer, 0xff, 0x00, 0x00, SDL_ALPHA_OPAQUE); break;
        case B_Green:     SDL_SetRenderDrawColor(m_pRenderer, 0x00, 0xff, 0x00, SDL_ALPHA_OPAQUE); break;
        case B_Blue:      SDL_SetRenderDrawColor(m_pRenderer, 0x00, 0x00, 0xff, SDL_ALPHA_OPAQUE); break;
        case B_Cyan:      SDL_SetRenderDrawColor(m_pRenderer, 0x00, 0xff, 0xff, SDL_ALPHA_OPAQUE); break;
        case B_Magenta:   SDL_SetRenderDrawColor(m_pRenderer, 0xff, 0x00, 0xff, SDL_ALPHA_OPAQUE); break;
        case B_Yellow:    SDL_SetRenderDrawColor(m_pRenderer, 0xff, 0xff, 0x00, SDL_ALPHA_OPAQUE); break;
        case B_Orange:    SDL_SetRenderDrawColor(m_pRenderer, 0xff, 0xa5, 0x00, SDL_ALPHA_OPAQUE); break;
        }
        SDL_RenderFillRect(m_pRenderer, pRect);
    }

    void Free()
    {
        m_minoTexture.Free();
    }

    SDL_Renderer* GetSDLRenderer()
    {
        return m_pRenderer;
    }


private:
    SDL_Rect m_minoTextureSlots[Block::B_End];
    SDL_Renderer* const m_pRenderer;
    Texture m_minoTexture;
};

class TetrisRenderer
{
public:
    TetrisRenderer(MinoRenderer& minoRenderer, int pivotX, int pivotY)
        : minoRenderer(minoRenderer)
        , pivotX(pivotX)
        , pivotY(pivotY)
    {
        
    }

    void DrawTetrimino(Tetrimino& tm, bool ghost = false, bool fillBackground = false)
    {
        const int* pattern = TetriminoPatterns[tm.type][tm.orientation];
        for (int i = 0; i < PatternSize; ++i)
        {
            int mx, my;
            MinoPatternIndexToXY(tm, i, mx, my);
            SDL_Rect rect = MakeRect(mx, my);
            if (pattern[i])
            {
                if (!ghost)
                    minoRenderer.DrawMino(tm.block, &rect);
                else
                    minoRenderer.DrawGhostMino(tm.block, &rect);
            }
            else if (fillBackground)
            {
                minoRenderer.DrawMino(Block::Empty, &rect);
            }
        }
    }

    void DrawPlayField(TetrisGame& game)
    {
        SDL_Renderer* pRenderer = minoRenderer.GetSDLRenderer();
        // Draw playfield boundary
        SDL_Rect rect = {
            pivotX,
            pivotY - Height * PxBlockSize,
            PlayFieldMetric::Width * PlayFieldMetric::PxBlockSize,
            PlayFieldMetric::Height * PlayFieldMetric::PxBlockSize,
        };
        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(pRenderer, &rect);

        SDL_SetRenderDrawColor(pRenderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
        int xbeg = pivotX;
        int xend = pivotX + PlayFieldMetric::Width * PlayFieldMetric::PxBlockSize - 1;
        int ybeg = pivotY - 1;
        int yend = ybeg - PlayFieldMetric::Height * PlayFieldMetric::PxBlockSize;
        for (int i = 1; i < PlayFieldMetric::Width; ++i)
        {
            int x = pivotX + PlayFieldMetric::PxBlockSize * i;
            SDL_RenderDrawLine(pRenderer, x, ybeg, x, yend);
        }
        for (int i = 1; i < PlayFieldMetric::Height; ++i)
        {
            int y = pivotY - PlayFieldMetric::PxBlockSize * i;
            SDL_RenderDrawLine(pRenderer, xbeg, y, xend, y);
        }

        for (int x = 0; x < PlayFieldMetric::Width; ++x)
        {
            for (int y = 0; y < PlayFieldMetric::Height; ++y)
            {
                Block block = game.GetPlayField().Get(x, y);
                SDL_Rect rect = MakeRect(x, y);
                SDL_Rect minoRect = { 0,0,16,16 };
                minoRenderer.DrawMino(block, &rect);
            }
        }

        if (game.GetPlayState() == PS_Control)
        {
            DrawTetrimino(game.GetActiveTetrimino());
        }

        // Draw ghost
        if (game.GetPlayState() == PS_Control)
        {
            const Tetrimino& tmActive = game.GetActiveTetrimino();
            Tetrimino tmGhost = tmActive;
            while (!game.GetPlayField().CheckDownContact(tmGhost))
            {
                --tmGhost.y;
            }
            if (tmGhost.y > tmActive.y)
            {
                tmGhost.y = tmActive.y;
            }
            DrawTetrimino(tmGhost, true);
        }

        // Draw next tetriminos
        const int MaximumVisibleQueueSize = 5;
        for (int i = 0; i < MaximumVisibleQueueSize; ++i)
        {
            TetriminoType tt = game.GetRandomizer().Peek(i);
            if (tt != TT_End)
            {
                Tetrimino tm = MakeTetrimino(tt);
                tm.x = 12;
                tm.y = 22 - i * 5;
                DrawTetrimino(tm, false, true);
            }
        }

        // Draw hold tetrimino
        TetriminoType hold = game.GetHold();
        if (hold != TT_End)
        {
            Tetrimino tm = MakeTetrimino(hold);
            tm.x = -5;
            tm.y = 22;
            DrawTetrimino(tm, false, true);
        }
    }

    void SetPivotX(int x) {
        pivotX = x;
    }

    void SetPivotY(int y) {
        pivotY = y;
    }

private:
    MinoRenderer& minoRenderer;
    int pivotX, pivotY;

    SDL_Rect MakeRect(int x, int y)
    {
        SDL_Rect rect;
        rect.x = pivotX + x * PxBlockSize;
        rect.y = pivotY - (y + 1) * PxBlockSize;
        rect.h = rect.w = PxBlockSize;
        return rect;
    }
};

enum GameState
{
    GS_MainMenu,
    GS_Sprint,
    GS_Infinite,
};

class UI
{
public:
    int GetX() { return x; }
    int GetY() { return y; }
    int GetMinWidth() { return minWidth; }
    int GetMinHeight() { return minHeight; }
    void SetX(int x) { this->x = x; }
    void SetY(int y) { this->y = y; }
    void SetMinWidth(int minWidth) { this->minWidth = minWidth; }
    void SetMinHeight(int minHeight) { this->minHeight = minHeight; }
    virtual int GetWidth() { return 0; }
    virtual int GetHeight() { return 0; }
    void SetXy(int x, int y) { this->x = x; this->y = y; }
    PivotPoint GetPivotPoint() { return pivot; }
    void SetPivotPoint(PivotPoint pivot) { this->pivot = pivot; }
    virtual void Render() {};

private:
    PivotPoint pivot;
    int x, y;
    int minWidth, minHeight;
};

template <typename T>
class UINumberBox : public UI
{
public:
    UINumberBox(SDL_Renderer* pRenderer, T initialValue, std::function<T(void)> fpTargetGetter, int minHeight)
        : mLast(initialValue)
        , mTexture(pRenderer)
        , mTargetGetter(fpTargetGetter)
        
    {
        mTexture.LoadFromRenderedText(std::to_string(initialValue), g_pFont, ColorWhite);
        SetMinHeight(minHeight);
    }

    void Render() override
    {
        T val = mTargetGetter();
        if (mLast != val)
        {
            mTexture.LoadFromRenderedText(std::to_string(val), g_pFont, ColorWhite);
            if (OnChange) OnChange(mLast, val);
            mLast = val;
        }
        mTexture.render(GetX(), GetY());
    }

    int GetWidth() override
    {
        return std::max(GetMinWidth(), mTexture.GetWidth());
    }

    int GetHeight() override
    {
        return std::max(GetMinHeight(), mTexture.GetHeight());
    }

    std::function<void(T oldVal, T newVal)> OnChange;

private:
    T mLast;
    Texture mTexture;
    std::function<T()> mTargetGetter;
};

class UITextBox : public UI
{
public:
    UITextBox(SDL_Renderer* pRenderer, const std::string& content, SDL_Color color)
        : mTexture(pRenderer)
    {
        mTexture.LoadFromRenderedText(content, g_pFont, color);
    }

    void SetContent(const std::string& content, SDL_Color color)
    {
        mTexture.LoadFromRenderedText(content, g_pFont, color);
    }

    void FreeContent()
    {
        mTexture.Free();
    }

    ~UITextBox()
    {
        mTexture.Free();
    }

    void Render() override
    {
        mTexture.render(GetX(), GetY());
    }

    int GetWidth() override
    {
        return std::max(GetMinWidth(), mTexture.GetWidth());
    }

    int GetHeight() override
    {
        return std::max(GetMinHeight(), mTexture.GetHeight());
    }

private:
    Texture mTexture;
};

class UIStack : public UI
{
public:
    void AddUI(std::shared_ptr<UI> ui)
    {
        uis.push_back(ui);
    }

    void Render()
    {
        int cursorX = GetX();
        int cursorY = GetY();
        for (std::size_t i = 0; i < uis.size(); ++i)
        {
            uis[i]->SetX(cursorX);
            uis[i]->SetY(cursorY);
            uis[i]->Render();
            cursorY += uis[i]->GetHeight();
        }
    }

    int GetWidth() override
    {
        int width = 0;
        for (std::size_t i = 0; i < uis.size(); ++i)
        {
            int childWidth = uis[i]->GetWidth();
            if (width < childWidth)
                width = childWidth;
        }
        return width;
    }

    int GetHeight() override
    {
        int height = 0;
        for (std::size_t i = 0; i < uis.size(); ++i)
        {
            int childHeight = uis[i]->GetHeight();
            if (height < childHeight)
                height = childHeight;
        }
        return height;
    }

private:
    std::vector<std::shared_ptr<UI>> uis;
};

struct TimerTime
{
    int hour;
    int min;
    int sec;
    int msec;
};

class UITimer : public UI
{
public:
    UITimer(SDL_Renderer* pRenderer, std::function<TimerTime()> fpTargetGetter, SDL_Color color, int minHeight)
        : mColor(color)
        , mTexSep(pRenderer)
        , mTexHour(pRenderer)
        , mTexMin(pRenderer)
        , mTexSec(pRenderer)
        , mTexMsec(pRenderer)
        , mLast({ -1,-1,-1,-1 })
        , mTargetGetter(fpTargetGetter)
    {
        SetMinHeight(minHeight);
        mTexSep.LoadFromRenderedText(" : ", g_pFont, color);
    }

    void Render() override
    {
        TimerTime tt = mTargetGetter();

        if (mLast.hour != tt.hour)
        {
            char buf[5];
            sprintf_s(buf, "%02d", tt.hour);
            mTexHour.LoadFromRenderedText(buf, g_pFont, mColor);
        }
        if (mLast.min != tt.min)
        {
            char buf[5];
            sprintf_s(buf, "%02d", tt.min);
            mTexMin.LoadFromRenderedText(buf, g_pFont, mColor);
        }
        if (mLast.sec != tt.sec)
        {
            char buf[5];
            sprintf_s(buf, "%02d", tt.sec);
            mTexSec.LoadFromRenderedText(buf, g_pFont, mColor);
        }
        if (mLast.msec != tt.msec)
        {
            char buf[5];
            sprintf_s(buf, "%03d", tt.msec);
            mTexMsec.LoadFromRenderedText(buf, g_pFont, mColor);
        }

        mLast = tt;

        int cursorX = GetX();
        int cursorY = GetY();
        mTexHour.render(cursorX, cursorY);
        cursorX += mTexHour.GetWidth();
        mTexSep.render(cursorX, cursorY);
        cursorX += mTexSep.GetWidth();
        mTexMin.render(cursorX, cursorY);
        cursorX += mTexMin.GetWidth();
        mTexSep.render(cursorX, cursorY);
        cursorX += mTexSep.GetWidth();
        mTexSec.render(cursorX, cursorY);
        cursorX += mTexSec.GetWidth();
        mTexSep.render(cursorX, cursorY);
        cursorX += mTexSep.GetWidth();
        mTexMsec.render(cursorX, cursorY);
    }

private:
    SDL_Color mColor;
    Texture mTexSep;
    Texture mTexHour;
    Texture mTexMin;
    Texture mTexSec;
    Texture mTexMsec;
    TimerTime mLast;
    std::function<TimerTime()> mTargetGetter;
};

class SprintTimer
{
public:
    SprintTimer()
        : stop(true)
        , sprintClock()
        , sprintBegin()
        , sprintNow()
        , time({ 0,0,0,0 })
    {
    }

    void Start()
    {
        if (stop)
        {
            sprintBegin = sprintClock.now();
            stop = false;
        }
    }

    void Stop()
    {
        if (!stop)
        {
            sprintNow = sprintClock.now();
            stop = true;
        }
    }

    TimerTime GetTimerTime()
    {
        if (!stop)
            sprintNow = sprintClock.now();
        
        auto duration = sprintNow - sprintBegin;
        TimerTime tt = { 0,0,0 };
        long long total = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        tt.msec = total % 1000;
        tt.sec = (total / 1000) % 60;
        tt.min = (total / 60000) % 60;
        tt.hour = static_cast<int>(total / 3600000);

        return tt;
    }

private:
    bool stop;
    std::chrono::high_resolution_clock sprintClock;
    std::chrono::high_resolution_clock::time_point sprintBegin;
    std::chrono::high_resolution_clock::time_point sprintNow;
    TimerTime time;
};

int main(int argc, char** argv)
{
    // Iniitlaize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not be initialized!");
        return -1;
    }

    // The window we will be rendering to
    SDL_Window* pWindow = SDL_CreateWindow("KF", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, 0);
    if (!pWindow)
    {
        printf("SDL could not create window");
        return -1;
    }

    // The surface contained by the window
    SDL_Surface* pScreenSurface = SDL_GetWindowSurface(pWindow);

    // Create renderer for window
    SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pRenderer)
    {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    // Update the surface
    SDL_UpdateWindowSurface(pWindow);   

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf(" SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return -1;
    }

    // Initialize SDL-ttf
    if (TTF_Init() == -1)
    {
        printf(" SDL_ttf could not initialize! SDL_ttf Error : %s\n", TTF_GetError());
        return -1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }

    // Initialize background
    Background background(pRenderer, "background.png");

    //Load music
    g_pMusic = Mix_LoadMUS("Tetris Official Theme song.mp3");
    if (g_pMusic == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }
    
    g_pWavLevelup = Mix_LoadWAV("Levelup.wav");
    g_pWavExplosion = Mix_LoadWAV("Explosion.wav");
    g_pWavDrop = Mix_LoadWAV("Drop.wav");

    g_pFont = TTF_OpenFont("arial.ttf", 22);
    if (g_pFont == nullptr)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return -1;
    }

    TetrisGame tetrisGame;

    MinoRenderer minoRenderer(pRenderer);
    minoRenderer.Load();

    // Pivot point of the playfield; left bottom coner of the field. in screen space coordinate.
    const int PlayFieldPivotX = (ScreenWidth - PlayFieldMetric::Width * PlayFieldMetric::PxBlockSize) / 2;
    // Pivot point of the playfield; Left bottom corner of the field. in screen space coordinate.
    const int PlayFieldPivotY = 500;
    TetrisRenderer tetrisRenderer(minoRenderer, PlayFieldPivotX , PlayFieldPivotY);

    Mix_PlayMusic(g_pMusic, -1);
    Mix_PauseMusic();

    UIStack mainMenu;
    mainMenu.SetXy(200, 200);
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "Press number key to select", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "1. Infinity Mode", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "2. Sprint 40", ColorBlack)));
    mainMenu.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "3. Quit", ColorBlack)));

    UIStack infiniteUI;
    infiniteUI.SetXy(120, 200);
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "LINES CLEARED", ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox<int>(pRenderer, 0, std::bind(&TetrisGame::GetTotalClearedLines, &tetrisGame), 40)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "LEVEL", ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox<int>(pRenderer, 0, std::bind(&TetrisGame::GetLevel, &tetrisGame), 40)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "BONUS", ColorBlack)));
    std::shared_ptr<UITextBox> bonusShow(new UITextBox(pRenderer, "", ColorWhite));
    bonusShow->SetMinHeight(40);
    infiniteUI.AddUI(bonusShow);
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "COMBO", ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox<int>(pRenderer, 0, std::bind(&TetrisGame::GetCombo, &tetrisGame), 40)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "SCORE", ColorBlack)));
    infiniteUI.AddUI(std::shared_ptr<UI>(new UINumberBox<int>(pRenderer, 0, std::bind(&TetrisGame::GetScore, &tetrisGame), 40)));

    SprintTimer sprintTimer;

    UIStack sprintUI;
    sprintUI.SetXy(120, 200);
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "LINES CLEARED", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UINumberBox<int>(pRenderer, 0, std::bind(&TetrisGame::GetTotalClearedLines, &tetrisGame), 40)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "SCORE", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UINumberBox<int>(pRenderer, 0, std::bind(&TetrisGame::GetScore, &tetrisGame), 40)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITextBox(pRenderer, "TIME", ColorBlack)));
    sprintUI.AddUI(std::shared_ptr<UI>(new UITimer(pRenderer, std::bind(&SprintTimer::GetTimerTime, &sprintTimer), ColorWhite, 40)));

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
                bonusShow->SetContent(msg, ColorWhite);
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

        // Clear screen
        background.Render(pRenderer);

        if (gameState == GS_MainMenu)
        {
            mainMenu.Render();
        }
        else if (gameState == GS_Infinite)
        {
            tetrisRenderer.DrawPlayField(tetrisGame);

            infiniteUI.Render();

            if (tetrisGame.GetPlayState() == PS_GameOver)
                gameoverShow.render((ScreenWidth - gameoverShow.GetWidth()) / 2, (ScreenHeight - gameoverShow.GetHeight()) / 2);
            else if (tetrisGame.GetPlayState() == PS_Ready)
                readyShow.render((ScreenWidth - readyShow.GetWidth()) / 2, (ScreenHeight - readyShow.GetHeight()) / 2);
        }
        else if (gameState == GS_Sprint)
        {
            tetrisRenderer.DrawPlayField(tetrisGame);

            sprintUI.Render();

            if (tetrisGame.GetPlayState() == PS_GameOver) {
                sprintTimer.Stop();
                gameoverShow.render((ScreenWidth - gameoverShow.GetWidth()) / 2, (ScreenHeight - gameoverShow.GetHeight()) / 2);
            }
            else if (tetrisGame.GetPlayState() == PS_Ready)
                readyShow.render((ScreenWidth - readyShow.GetWidth()) / 2, (ScreenHeight - readyShow.GetHeight()) / 2);

            if( tetrisGame.GetTotalClearedLines() >= 40 )
            {
                sprintTimer.Stop();
                sprintCompleteShow.render((ScreenWidth - sprintCompleteShow.GetWidth()) / 2, (ScreenHeight - sprintCompleteShow.GetHeight()) / 2);
                tetrisGame.Win();
            }
        }

        // Update screen
        SDL_RenderPresent(pRenderer);

        // 0.016 seconds = farme length
        const long long NsFrameLength = 16000000;
        std::chrono::nanoseconds frameTime;
        do
        {
            frameTime = frameClock.now() - frameTick;
        } while (frameTime.count() < NsFrameLength);

        frameTick = frameClock.now();
    } // while(!quit)

    background.Free();

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
