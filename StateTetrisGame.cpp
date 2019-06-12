#include "Pch.h"
#include "StateTetrisGame.h"
#include <cmath>
#include "Application.h"

TetrisRenderDesc StateTetrisGame::MakeRenderDesc(TetrisGame& tetrisGame, int x, int y, int pxBlockSize)
{
    TetrisRenderDesc desc = { 0 };
    const int PxPlayFIeldWidth = tetrisGame.GetPlayField().GetWidth() * pxBlockSize;
    const int NextCount = 5;

    desc.pxBlockSize = pxBlockSize;
    desc.pxPlayFieldX = x;
    desc.pxPlayFieldY = y;
    desc.pxHoldX = desc.pxPlayFieldX - pxBlockSize * 5;
    desc.pxHoldY = desc.pxPlayFieldY;
    desc.nextCount = NextCount;
    for (int i = 0; i < NextCount; ++i)
    {
        desc.pxNextX[i] = desc.pxPlayFieldX + desc.pxBlockSize * 11;
        desc.pxNextY[i] = desc.pxPlayFieldY + i * desc.pxBlockSize * 4;
    }

    desc.visibleLines = 20;
    return desc;
}

void StateTetrisGame::DoShift(TetrisGame& tetrisGame)
{
    int dx = 0;
    if (Application::state[SDL_SCANCODE_DOWN]) tetrisGame.SoftDrop();
    if (Application::state[SDL_SCANCODE_RIGHT]) dx += 1;
    if (Application::state[SDL_SCANCODE_LEFT]) dx += -1;
    tetrisGame.Shift(dx);
}

DropResult StateTetrisGame::DoDrop(TetrisGame& tetrisGame)
{
    auto result = tetrisGame.OnDrop();
    if (result.lineClearCount != 0)
    {
        Mix_PlayChannel(-1, g_pWavExplosion, 0);
    }
    return result;
}
