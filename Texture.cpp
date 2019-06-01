#include "Texture.h"
#include <cstdio>
#include <SDL_image.h>
#include <SDL_ttf.h>

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
    //Deallocate
    Free();
}

bool Texture::LoadFromFile(std::string path)
{
    //Get rid of preexisting texture
    Free();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(m_pRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else
        {
            //Get image dimensions
            m_width = loadedSurface->w;
            m_height = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    m_pTexture = newTexture;
    return m_pTexture != NULL;
}

bool Texture::LoadFromRenderedText(std::string textureText, TTF_Font* pFont, SDL_Color textColor)
{
    //Get rid of preexisting texture
    Free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(pFont, textureText.c_str(), textColor);
    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        //Create texture from surface pixels
        m_pTexture = SDL_CreateTextureFromSurface(m_pRenderer, textSurface);
        if (m_pTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get image dimensions
            m_width = textSurface->w;
            m_height = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    }

    //Return success
    return m_pTexture != NULL;
}

void Texture::Free()
{
    //Free texture if it exists
    if (m_pTexture != NULL)
    {
        SDL_DestroyTexture(m_pTexture);
        m_pTexture = NULL;
        m_width = 0;
        m_height = 0;
    }
}

void Texture::SetAlpha(Uint8 alpha)
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(m_pTexture, alpha);
}

void Texture::Render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, m_width, m_height };

    //Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
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
