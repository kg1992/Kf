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
    : m_minoRenderer(minoRenderer)
{

}

void TetrisRenderer::DrawTetrimino(Tetrimino& tm, int height, bool ghost, bool fillBackground)
{
    int px, py, sx, sy;
    MinoPatternIndexToXY(tm.x, tm.y, 0, px, py);
    PlayFieldXyToScreenCoord(px, py, height, sx, sy);
    DrawTetrimino(sx, sy, tm.type, tm.orientation, desc.pxBlockSize, ghost, fillBackground);
}

void TetrisRenderer::DrawTetrimino(int sx, int sy, TetriminoType type, Orientation orientation, int pxBlockSize, bool ghost, bool fillBackground)
{
    const int* pattern = TetriminoPatterns[type][orientation];
    for (int i = 0; i < PatternSize; ++i)
    {
        int px = i % 4;
        int py = i / 4;
        SDL_Rect rect =
        {
            sx + px * pxBlockSize,
            sy + py * pxBlockSize,
            pxBlockSize,
            pxBlockSize
        };
        if (pattern[i])
        {
            if (!ghost)
                m_minoRenderer.DrawMino(TetriminoTypeToBlock(type), &rect);
            else
                m_minoRenderer.DrawGhostMino(TetriminoTypeToBlock(type), &rect);
        }
        else if (fillBackground)
        {
            m_minoRenderer.DrawMino(Block::B_Empty, &rect);
        }
    }
}

void TetrisRenderer::DrawTetris(TetrisGame& game)
{
    const int Height = std::min(game.GetPlayField().GetHeight(), desc.visibleLines);

    // Draw playfield
    DrawPlayField(game.GetPlayField());

    // Draw active tetrimino
    if (game.GetPlayState() == PS_Control)
    {
        DrawTetrimino(game.GetActiveTetrimino(), Height);
    }

    // Draw ghost piece
    if (game.GetPlayState() == PS_Control)
    {
        const Tetrimino& tmActive = game.GetActiveTetrimino();
        Tetrimino tmGhost = tmActive;
        while (!game.GetPlayField().CheckDownContact(tmGhost))
            --tmGhost.y;
        if (tmGhost.y > tmActive.y)
            tmGhost.y = tmActive.y;
        DrawTetrimino(tmGhost, Height, true);
    }
    
    // Draw next tetriminos
    for (int i = 0; i < desc.nextCount; ++i)
    {
        TetriminoType tt = game.GetRandomizer().Peek(i);
        if (tt != TT_End)
        {
            DrawTetrimino(desc.pxNextX[i], desc.pxNextY[i], tt, O_North, desc.pxBlockSize, false, true);
        }
    }

    // Draw hold tetrimino
    TetriminoType hold = game.GetHold();
    if (hold != TT_End)
    {
        DrawTetrimino(desc.pxHoldX, desc.pxHoldY, hold, O_North, desc.pxBlockSize, false, true);
    }
}

void TetrisRenderer::SetTetrisRenderDesc(const TetrisRenderDesc& desc)
{
    this->desc = desc;
}

const TetrisRenderDesc& TetrisRenderer::GetTetrisRenderDesc()
{
    return desc;
}

void TetrisRenderer::DrawPlayField(PlayField& playField)
{
    const int Width = playField.GetWidth();
    const int Height = std::min(desc.visibleLines, playField.GetHeight());
    const int PxPlayFieldWidth = Width * desc.pxBlockSize + 2;
    const int PxPlayFieldHeight = Height * desc.pxBlockSize + 2;
    SDL_Renderer* pRenderer = m_minoRenderer.GetSDLRenderer();

    // Draw playfield boundary
    SDL_Rect rect = {
        desc.pxPlayFieldX,
        desc.pxPlayFieldY,
        PxPlayFieldWidth,
        PxPlayFieldHeight,
    };
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(pRenderer, &rect);

    for (int x = 0; x < Width; ++x)
    {
        for (int y = 0; y < Height; ++y)
        {
            Block block = playField.Get(x, y);
            int sx, sy;
            PlayFieldXyToScreenCoord( x, y , Height, sx, sy);
            SDL_Rect rect = { sx, sy, desc.pxBlockSize, desc.pxBlockSize };
            m_minoRenderer.DrawMino(block, &rect);
        }
    }
}

