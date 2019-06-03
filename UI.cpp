#include "UI.h"
#include <algorithm>

extern TTF_Font* g_pFont;

const SDL_Color ColorWhite = { 0xff, 0xff, 0xff };
const SDL_Color ColorBlack = { 0x00, 0x00, 0x00 };


UINumberBox::UINumberBox(SDL_Renderer* pRenderer, int initialValue, std::function<int(void)> fpTargetGetter, int minHeight)
    : mLast(initialValue)
    , mTexture(pRenderer)
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

UITextBox::UITextBox(SDL_Renderer* pRenderer, const std::string& content, SDL_Color color)
    : mTexture(pRenderer)
{
    mTexture.LoadFromRenderedText(content, g_pFont, color);
}

void UITextBox::SetContent(const std::string& content, SDL_Color color)
{
    mTexture.LoadFromRenderedText(content, g_pFont, color);
}

void UITextBox::FreeContent()
{
    mTexture.Free();
}

UITextBox::~UITextBox()
{
    mTexture.Free();
}

void UITextBox::Render()
{
    mTexture.Render(GetX(), GetY());
}

int UITextBox::GetWidth()
{
    return std::max(GetMinWidth(), mTexture.GetWidth());
}

int UITextBox::GetHeight()
{
    return std::max(GetMinHeight(), mTexture.GetHeight());
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

UITimer::UITimer(SDL_Renderer* pRenderer, std::function<TimerTime()> fpTargetGetter, SDL_Color color, int minHeight)
    : mColor(color)
    , mTexSep(pRenderer)
    , mTexHour(pRenderer)
    , mTexMin(pRenderer)
    , mTexSec(pRenderer)
    , mTexMsec(pRenderer)
    , mLast({ -1,-1,-1,-1 })
    , mTargetGetter(fpTargetGetter)
{
    SetMinHeight(minHeight);
    mTexSep.LoadFromRenderedText(" : ", g_pFont, color);
}

void UITimer::Render()
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
