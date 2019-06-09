#ifndef UI_H_
#define UI_H_

#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <SDL.h>
#include "TimerTime.h"
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

class UI
{
public:
    int GetX() { return m_x; }
    int GetY() { return m_y; }
    int GetMinWidth() { return m_minWidth; }
    int GetMinHeight() { return m_minHeight; }
    void SetX(int x) { this->m_x = x; }
    void SetY(int y) { this->m_y = y; }
    void SetMinWidth(int minWidth) { this->m_minWidth = minWidth; }
    void SetMinHeight(int minHeight) { this->m_minHeight = minHeight; }
    virtual int GetWidth() { return 0; }
    virtual int GetHeight() { return 0; }
    void SetXy(int x, int y) { this->m_x = x; this->m_y = y; }
    PivotPoint GetPivotPoint() { return m_pivot; }
    void SetPivotPoint(PivotPoint pivot) { this->m_pivot = pivot; }
    virtual void Render() {};

private:
    PivotPoint m_pivot;
    int m_x, m_y;
    int m_minWidth, m_minHeight;
};

class UINumberBox : public UI
{
public:
    UINumberBox(int initialValue, std::function<int(void)> fpTargetGetter, int minHeight);

    void Render() override;

    int GetWidth() override;

    int GetHeight() override;

    std::function<void(int oldVal, int newVal)> OnChange;

private:
    int mLast;
    Texture mTexture;
    std::function<int()> mTargetGetter;
};

class UITextBox : public UI
{
public:
    static const SDL_Color ColorTransparent;

    UITextBox();

    UITextBox(const std::string& content, SDL_Color color, SDL_Color backgroundColor = ColorTransparent);

    UITextBox(const std::wstring& content, SDL_Color color, SDL_Color backgroundColor = ColorTransparent);

    void SetContent(const std::string& content);

    void SetContent(const std::wstring& content);

    void FreeContent();

    ~UITextBox();

    void Render() override;

    int GetWidth() override;

    int GetHeight() override;

    SDL_Color& GetTextColor();

    void SetTextColor(const SDL_Color& color);

    SDL_Color& GetBackgroundColor();

    void SetBackgroundColor(const SDL_Color& color);

private:
    Texture m_texture;
    std::string m_u8Content;
    SDL_Color m_textColor;
    SDL_Color m_backgroundColor;

    void Refresh();
};

class UIStack : public UI
{
public:
    void AddUI(std::shared_ptr<UI> ui);

    void Render();

    int GetWidth() override;

    int GetHeight() override;

    void Clear();

private:
    std::vector<std::shared_ptr<UI>> m_children;
};

class UITimer : public UI
{
public:
    UITimer(std::function<TimerTime()> fpTargetGetter, SDL_Color color, int minHeight);

    int GetWidth() override;

    void Render() override;

private:
    SDL_Color mColor;
    Texture mTexSep;
    Texture mTexHour;
    Texture mTexMin;
    Texture mTexSec;
    Texture mTexMsec;
    TimerTime mLast;
    std::function<TimerTime()> mTargetGetter;

    void Refresh();
};

#endif
