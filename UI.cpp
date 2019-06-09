#include "UI.h"
#include <algorithm>
#include <codecvt>
#include "Application.h"

extern TTF_Font* g_pFont;

UINumberBox::UINumberBox(int initialValue, std::function<int(void)> fpTargetGetter, int minHeight)
    : mLast(initialValue)
    , mTexture(Application::GetWindowRenderer())
    , mTargetGetter(fpTargetGetter)
{
    mTexture.LoadFromRenderedText(std::to_string(initialValue), g_pFont, ColorBlack);
    SetMinHeight(minHeight);
}

void UINumberBox::Render()
{
    int val = mTargetGetter();
    if (mLast != val)
    {
        mTexture.LoadFromRenderedText(std::to_string(val), g_pFont, ColorBlack);
        if (OnChange) OnChange(mLast, val);
        mLast = val;
    }
    mTexture.Render(GetX(), GetY());
}

int UINumberBox::GetWidth()
{
    return std::max(GetMinWidth(), mTexture.GetWidth());
}

int UINumberBox::GetHeight()
{
    return std::max(GetMinHeight(), mTexture.GetHeight());
}

const SDL_Color UITextBox::ColorTransparent = { 0,0,0,SDL_ALPHA_TRANSPARENT };

UITextBox::UITextBox()
    : m_texture(Application::GetWindowRenderer())
    , m_u8Content()
    , m_textColor(ColorBlack)
    , m_backgroundColor(ColorTransparent)
{

}

UITextBox::UITextBox(const std::string& content, SDL_Color color, SDL_Color backgroundColor)
    : m_texture(Application::GetWindowRenderer())
    , m_u8Content(content)
    , m_textColor(color)
    , m_backgroundColor(backgroundColor)
{
    m_texture.LoadFromRenderedText(content, g_pFont, color);
}

UITextBox::UITextBox(const std::wstring& content, SDL_Color color, SDL_Color backgroundColor)
    : m_texture(Application::GetWindowRenderer())
    , m_u8Content(Utf16wstrToUtf8str(content))
    , m_textColor(color)
    , m_backgroundColor(backgroundColor)
{
    m_texture.LoadFromRenderedText(m_u8Content, g_pFont, color);
}

void UITextBox::SetContent(const std::string& content)
{
    m_u8Content = content;
    Refresh();
}

void UITextBox::SetContent(const std::wstring& content)
{
    m_u8Content = Utf16wstrToUtf8str(content);
    Refresh();
}

void UITextBox::FreeContent()
{
    m_texture.Free();
}

UITextBox::~UITextBox()
{
    m_texture.Free();
}

void UITextBox::Render()
{
    SDL_Renderer* pRenderer = Application::GetWindowRenderer();
    if (std::memcmp(&m_backgroundColor, &ColorTransparent, sizeof(m_backgroundColor)))
    {
        SDL_Rect rect = { GetX(), GetY(), GetWidth(), GetHeight() };
        SDL_SetRenderDrawColor(pRenderer, m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
        SDL_RenderFillRect(pRenderer, &rect);
    }
    m_texture.Render(GetX(), GetY());
}

int UITextBox::GetWidth()
{
    return std::max(GetMinWidth(), m_texture.GetWidth());
}

int UITextBox::GetHeight()
{
    return std::max(GetMinHeight(), m_texture.GetHeight());
}

SDL_Color& UITextBox::GetTextColor()
{
    return m_textColor;
}

void UITextBox::SetTextColor(const SDL_Color& color)
{
    m_textColor = color;
    Refresh();
}

SDL_Color& UITextBox::GetBackgroundColor()
{
    return m_backgroundColor;
}

void UITextBox::SetBackgroundColor(const SDL_Color& color)
{
    m_backgroundColor = color;
}

void UITextBox::Refresh()
{
    m_texture.LoadFromRenderedText(m_u8Content, g_pFont, m_textColor);
}

void UIStack::AddUI(std::shared_ptr<UI> ui)
{
    m_children.push_back(ui);
}

void UIStack::Render()
{
    int cursorX = GetX();
    int cursorY = GetY();
    for (std::size_t i = 0; i < m_children.size(); ++i)
    {
        m_children[i]->SetX(cursorX);
        m_children[i]->SetY(cursorY);
        m_children[i]->Render();
        cursorY += m_children[i]->GetHeight();
    }
}

int UIStack::GetWidth()
{
    int width = 0;
    for (std::size_t i = 0; i < m_children.size(); ++i)
    {
        int childWidth = m_children[i]->GetWidth();
        if (width < childWidth)
            width = childWidth;
    }
    return width;
}

int UIStack::GetHeight()
{
    int height = 0;
    for (std::size_t i = 0; i < m_children.size(); ++i)
    {
        int childHeight = m_children[i]->GetHeight();
        height += childHeight;
    }
    return height;
}

void UIStack::Clear()
{
    m_children.clear();
}

UITimer::UITimer(std::function<TimerTime()> fpTargetGetter, SDL_Color color, int minHeight)
    : mColor(color)
    , mTexSep(Application::GetWindowRenderer())
    , mTexHour(Application::GetWindowRenderer())
    , mTexMin(Application::GetWindowRenderer())
    , mTexSec(Application::GetWindowRenderer())
    , mTexMsec(Application::GetWindowRenderer())
    , mLast({ -1,-1,-1,-1 })
    , mTargetGetter(fpTargetGetter)
{
    SetMinHeight(minHeight);
    mTexSep.LoadFromRenderedText(" : ", g_pFont, color);
    Refresh();
}

int UITimer::GetWidth()
{
    return
        mTexSep.GetWidth() * 3 +
        mTexHour.GetWidth() +
        mTexMin.GetWidth() +
        mTexSec.GetWidth() +
        mTexMsec.GetWidth();
}

void UITimer::Render()
{
    Refresh();

    int cursorX = GetX();
    int cursorY = GetY();
    mTexHour.Render(cursorX, cursorY);
    cursorX += mTexHour.GetWidth();
    mTexSep.Render(cursorX, cursorY);
    cursorX += mTexSep.GetWidth();
    mTexMin.Render(cursorX, cursorY);
    cursorX += mTexMin.GetWidth();
    mTexSep.Render(cursorX, cursorY);
    cursorX += mTexSep.GetWidth();
    mTexSec.Render(cursorX, cursorY);
    cursorX += mTexSec.GetWidth();
    mTexSep.Render(cursorX, cursorY);
    cursorX += mTexSep.GetWidth();
    mTexMsec.Render(cursorX, cursorY);
}


void UITimer::Refresh()
{
    TimerTime tt = mTargetGetter();

    if (mLast.hour != tt.hour)
    {
        char buf[5];
        sprintf_s(buf, "%02d", tt.hour);
        mTexHour.LoadFromRenderedText(buf, g_pFont, mColor);
    }
    if (mLast.min != tt.min)
    {
        char buf[5];
        sprintf_s(buf, "%02d", tt.min);
        mTexMin.LoadFromRenderedText(buf, g_pFont, mColor);
    }
    if (mLast.sec != tt.sec)
    {
        char buf[5];
        sprintf_s(buf, "%02d", tt.sec);
        mTexSec.LoadFromRenderedText(buf, g_pFont, mColor);
    }
    if (mLast.msec != tt.msec)
    {
        char buf[5];
        sprintf_s(buf, "%03d", tt.msec);
        mTexMsec.LoadFromRenderedText(buf, g_pFont, mColor);
    }

    mLast = tt;
}