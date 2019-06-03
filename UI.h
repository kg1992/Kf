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
    UINumberBox(SDL_Renderer* pRenderer, int initialValue, std::function<int(void)> fpTargetGetter, int minHeight);

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
    UITextBox(SDL_Renderer* pRenderer, const std::string& content, SDL_Color color);

    void SetContent(const std::string& content, SDL_Color color);

    void FreeContent();

    ~UITextBox();

    void Render() override;

    int GetWidth() override;

    int GetHeight() override;

private:
    Texture mTexture;
};

class UIStack : public UI
{
public:
    void AddUI(std::shared_ptr<UI> ui);

    void Render();

    int GetWidth() override;

    int GetHeight() override;

private:
    std::vector<std::shared_ptr<UI>> m_children;
};

class UITimer : public UI
{
public:
    UITimer(SDL_Renderer* pRenderer, std::function<TimerTime()> fpTargetGetter, SDL_Color color, int minHeight);

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
};

#endif
