#include "TetrisRenderer.h"
#include <SDL.h>

MinoRenderer::MinoRenderer(SDL_Renderer* pRenderer)
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

void MinoRenderer::Load()
{
    m_minoTexture.LoadFromFile("MInos.png");
}

void MinoRenderer::DrawMino(Block block, SDL_Rect* pRect)
{
    m_minoTexture.RenderPart(&m_minoTextureSlots[block], pRect);
}

void MinoRenderer::DrawGhostMino(Block block, SDL_Rect* pRect)
{
    const Uint8 GhostMinoAlpha = 0x88;
    m_minoTexture.SetAlpha(GhostMinoAlpha);
    m_minoTexture.RenderPart(&m_minoTextureSlots[block], pRect);
    m_minoTexture.SetAlpha(SDL_ALPHA_OPAQUE);
}

void MinoRenderer::DrawMinoOpaque(Block block, SDL_Rect* pRect)
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

void MinoRenderer::Free()
{
    m_minoTexture.Free();
}

SDL_Renderer* MinoRenderer::GetSDLRenderer()
{
    return m_pRenderer;
}


TetrisRenderer::TetrisRenderer(MinoRenderer& minoRenderer)
    : minoRenderer(minoRenderer)
{

}

void TetrisRenderer::DrawTetrimino(Tetrimino& tm, const TetrisDesc& desc, bool ghost, bool fillBackground)
{
    const int* pattern = TetriminoPatterns[tm.type][tm.orientation];
    for (int i = 0; i < PatternSize; ++i)
    {
        int mx, my;
        MinoPatternIndexToXY(tm.x, tm.y, i, mx, my);
        SDL_Rect rect = MakeRect(mx, my, desc);
        if (pattern[i])
        {
            if (!ghost)
                minoRenderer.DrawMino(TetriminoTypeToBlock(tm.type), &rect);
            else
                minoRenderer.DrawGhostMino(TetriminoTypeToBlock(tm.type), &rect);
        }
        else if (fillBackground)
        {
            minoRenderer.DrawMino(Block::B_Empty, &rect);
        }
    }
}

void TetrisRenderer::DrawTetris(TetrisGame& game, const TetrisDesc& desc)
{
    PlayField& playField = game.GetPlayField();
    const int Width = playField.GetWidth();
    const int Height = playField.GetHeight();
    SDL_Renderer* pRenderer = minoRenderer.GetSDLRenderer();

    // Draw playfield boundary
    SDL_Rect rect = {
        desc.pxPlayFieldX,
        desc.pxPlayFieldY - Height * desc.pxBlockSize,
        Width * desc.pxBlockSize,
        Height * desc.pxBlockSize,
    };
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(pRenderer, &rect);

    for (int x = 0; x < Width; ++x)
    {
        for (int y = 0; y < Height; ++y)
        {
            Block block = game.GetPlayField().Get(x, y);
            SDL_Rect rect = MakeRect(x, y, desc);
            SDL_Rect minoRect = { 0,0,16,16 };
            minoRenderer.DrawMino(block, &rect);
        }
    }

    if (game.GetPlayState() == PS_Control)
    {
        DrawTetrimino(game.GetActiveTetrimino(), desc);
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
        DrawTetrimino(tmGhost, desc, true);
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
            DrawTetrimino(tm, desc, false, true);
        }
    }

    // Draw hold tetrimino
    TetriminoType hold = game.GetHold();
    if (hold != TT_End)
    {
        Tetrimino tm = MakeTetrimino(hold);
        tm.x = -5;
        tm.y = 22;
        DrawTetrimino(tm, desc, false, true);
    }
}

SDL_Rect TetrisRenderer::MakeRect(int x, int y, const TetrisDesc& desc)
{
    SDL_Rect rect;
    rect.x = desc.pxPlayFieldX + x * desc.pxBlockSize;
    rect.y = desc.pxPlayFieldY - (y + 1) * desc.pxBlockSize;
    rect.h = rect.w = desc.pxBlockSize;
    return rect;
}
