#ifndef TETRIS_RENDERER_H_
#define TETRIS_RENDERER_H_

#include "PlayField.h"
#include "TetrisGame.h"
#include "Texture.h"

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
    enum { Step = 16 };

    MinoRenderer(SDL_Renderer* pRenderer);

    void Load();

    void DrawMino(Block block, SDL_Rect* pRect);

    void DrawGhostMino(Block block, SDL_Rect* pRect);

    void DrawMinoOpaque(Block block, SDL_Rect* pRect);

    void Free();

    SDL_Renderer* GetSDLRenderer();

private:
    SDL_Rect m_minoTextureSlots[Block::B_End];
    SDL_Renderer* const m_pRenderer;
    Texture m_minoTexture;
};

struct TetrisDesc
{
    int pxPlayFieldX;
    int pxPlayFieldY;
    int pxBlockSize = 16;
};

class TetrisRenderer
{
public:
    TetrisRenderer(MinoRenderer& minoRenderer);

    void DrawTetrimino(Tetrimino& tm, const TetrisDesc& desc, bool ghost = false, bool fillBackground = false);

    void DrawTetris(TetrisGame& game, const TetrisDesc& desc);

private:
    MinoRenderer& minoRenderer;

    SDL_Rect MakeRect(int x, int y, const TetrisDesc& desc);
};

#endif
