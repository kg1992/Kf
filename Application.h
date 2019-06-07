#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "FiniteStateMachine.h"
#include <filesystem>
#include <memory>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <cstring>
#include "Options.h"
#include "StringTable.h"

namespace filesystem = std::experimental::filesystem;

enum GameState
{
    GS_MainMenu,
    GS_Sprint,
    GS_Infinite,
};

class Application
{
public:
    static int Start();
    static void Quit();
    static void ResizeWindow(int width, int height);
    static int GetClientAreaWidth();
    static int GetClientAreaHeight();
    static filesystem::path GetPrefPath();
    static bool IsWriteEnabled();
    static const std::wstring& GetString(StringTable::StringID id);
    template <typename... Ts>
    static std::wstring GetFormattedString(StringTable::StringID id, Ts... rest)
    {
        std::wstring format = GetString(id);
        std::vector<wchar_t> buffer(0x200, '\0');
        swprintf_s(buffer.data(), buffer.size(), format.c_str(), rest...);
        return std::wstring(buffer.data());
    }
    static SDL_Renderer* GetWindowRenderer();

    static FiniteStateMachine gsm;
    static std::shared_ptr<State> pStateMainMenu;
    static std::shared_ptr<State> pStateSprintMode;
    static std::shared_ptr<State> pStateInfiniteMode;
    static std::shared_ptr<State> pStateOptions;
    static const Uint8* state;
    static SDL_Window* window;
    static Options options;
    static StringTable stringTable;

private:
    static bool quit;
};

std::string Utf16wstrToUtf8str(const std::wstring& content);

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
