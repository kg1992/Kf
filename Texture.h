#ifndef LTEXTURE_H_
#define LTEXTURE_H_

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

//Texture wrapper class
class Texture
{
public:
    //Initializes variables
    Texture(SDL_Renderer* pRenderer);

    //Deallocates memory
    ~Texture();

    //Loads image at specified path
    bool LoadFromFile(std::string path);

    //Creates image from font string
    bool LoadFromRenderedText(std::string textureText, TTF_Font* pFont, SDL_Color textColor);

    //Deallocates texture
    void Free();

    //Set color modulation
    void SetColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set blending
    void SetBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void SetAlpha(Uint8 alpha);

    //Renders texture at given point
    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void renderPart(SDL_Rect* src, SDL_Rect* dst);

    //Gets image dimensions
    int GetWidth();
    int GetHeight();

private:
    //The actual hardware texture
    SDL_Texture* m_pTexture;

    SDL_Renderer* m_pRenderer;

    //Image dimensions
    int m_width;
    int m_height;
};

#endif
