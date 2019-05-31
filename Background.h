#ifndef BACKGROUND_H_
#define BACKGROUND_H_

class Background
{
public:
    Background(SDL_Renderer* pRenderer, const char* filename)
        : m_pTexture(nullptr)
    {
        SDL_Surface* pTextureSource = IMG_Load(filename);
        if (!pTextureSource)
        {
            printf("Unable to load image. Error : %s", IMG_GetError());
            return;
        }

        m_pTexture = SDL_CreateTextureFromSurface(pRenderer, pTextureSource);
        if (!m_pTexture)
        {
            printf("Unable to create texture. Error : %s", SDL_GetError());
            return;
        }

        SDL_FreeSurface(pTextureSource);
    }

    ~Background()
    {
        Free();
    }

    void Free()
    {   
        if (m_pTexture)
        {
            SDL_DestroyTexture(m_pTexture);
            m_pTexture = nullptr;
        }
    }

    void Render(SDL_Renderer* pRenderer)
    {
        if (m_pTexture)
        {
            // Render texture to screen
            SDL_RenderCopy(pRenderer, m_pTexture, nullptr, nullptr);
        }
    }

private:
    SDL_Texture* m_pTexture;
};

#endif
