#include "Pch.h"
#include "StateOnePlayer.h"
#include "Application.h"

StateOnePlayer::StateOnePlayer(TetrisRenderer& tetrisRenderer)
    : m_tetrisGame(10, 22, 3, 19)
    , m_tetrisRenderer(tetrisRenderer)
{

}

TetrisRenderDesc StateOnePlayer::MakeRenderDesc()
{
    TetrisRenderDesc desc = { 0 };
    const int ClientHeight = Application::GetClientAreaHeight();
    const int ClientWidth = Application::GetClientAreaWidth();
    const int PxBlockSize = static_cast<int>(std::round(Application::GetClientAreaHeight() / 600.f * 24.f));
    const int PxPlayFIeldWidth = m_tetrisGame.GetPlayField().GetWidth() * PxBlockSize;
    const int NextCount = 5;

    desc.pxBlockSize = PxBlockSize;
    desc.pxPlayFieldX = (ClientWidth - PxPlayFIeldWidth) / 2;
    desc.pxPlayFieldY = 100;
    desc.pxHoldX = desc.pxPlayFieldX - PxBlockSize * 5;
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
