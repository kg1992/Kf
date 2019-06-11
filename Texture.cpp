#include "Pch.h"
#include "Texture.h"

SDL_Renderer* mRenderer;

Texture::Texture(SDL_Renderer* pRenderer)
    : m_pTexture(nullptr)
    , m_pRenderer(pRenderer)
    , m_width(0)
    , m_height(0)
{
}

Texture::~Texture()
{
    Free();
}

bool Texture::Load(std::string path)
{
    Free();

    return m_pTexture = IMG_LoadTexture(m_pRenderer, path.c_str());
}

bool Texture::LoadFromRenderedText(std::string textureText, TTF_Font* pFont, SDL_Color textColor)
{
    Free();

    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(pFont, textureText.c_str(), textColor);
    if (textSurface == NULL)
    {
        printf("TTF_RenderUTF8_Blended failed: %s\n", TTF_GetError());
        return false;
    }
    
    m_pTexture = SDL_CreateTextureFromSurface(m_pRenderer, textSurface);
    if (m_pTexture == nullptr)
    {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    m_width = textSurface->w;
    m_height = textSurface->h;

    SDL_FreeSurface(textSurface);

    return m_pTexture != nullptr;
}

void Texture::Free()
{
    if (m_pTexture != nullptr)
    {
        SDL_DestroyTexture(m_pTexture);
        m_pTexture = nullptr;
        m_width = 0;
        m_height = 0;
    }
}

void Texture::SetAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(m_pTexture, alpha);
}

void Texture::Render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = { x, y, m_width, m_height };

    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopyEx(m_pRenderer, m_pTexture, clip, &renderQuad, angle, center, flip);
}

void Texture::RenderPart(SDL_Rect* src, SDL_Rect* dst)
{
    SDL_RenderCopy(m_pRenderer, m_pTexture, src, dst);
}

int Texture::GetWidth()
{
    return m_width;
}

int Texture::GetHeight()
{
    return m_height;
}
