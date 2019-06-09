#ifndef LTEXTURE_H_
#define LTEXTURE_H_

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class Texture
{
public:
    Texture(SDL_Renderer* pRenderer);

    ~Texture();

    bool Load(std::string path);

    bool LoadFromRenderedText(std::string textureText, TTF_Font* pFont, SDL_Color textColor);

    void Free();

    void SetAlpha(Uint8 alpha);

    void Render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void RenderPart(SDL_Rect* src, SDL_Rect* dst);

    int GetWidth();

    int GetHeight();

private:
    SDL_Texture* m_pTexture;
    SDL_Renderer* m_pRenderer;
    int m_width;
    int m_height;
};

#endif
