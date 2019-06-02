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

struct TetrisRenderDesc
{
    int pxPlayFieldX;
    int pxPlayFieldY;
    int pxBlockSize = 16;
    int visibleLines = 20;
    enum { MAXIMUM_NEXT_COUNT = 0x20 };
    int nextCount = MAXIMUM_NEXT_COUNT;
    int pxNextX[MAXIMUM_NEXT_COUNT];
    int pxNextY[MAXIMUM_NEXT_COUNT];
    int pxHoldX;
    int pxHoldY;
};

class TetrisRenderer
{
public:
    TetrisRenderer(MinoRenderer& minoRenderer);

    void DrawTetrimino(Tetrimino& tm, int height, bool ghost = false, bool fillBackground = false);

    void DrawTetrimino(int sx, int sy, TetriminoType type, Orientation orientation, int pxBlockSize, bool ghost = false, bool fillBackground = false);

    void DrawTetris(TetrisGame& game);

    void SetTetrisRenderDesc(const TetrisRenderDesc& desc);
    
    const TetrisRenderDesc& GetTetrisRenderDesc();

private:
    MinoRenderer& m_minoRenderer;
    TetrisRenderDesc desc;

    void DrawPlayField(PlayField& playField);

    void PlayFieldXyToScreenCoord(int px, int py, int height, int& sx, int& sy);
};

#endif