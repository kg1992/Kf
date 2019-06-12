#include "Pch.h"
#include "TetrisEventHandler.h"
#include "TetrisGame.h"

void TetrisEventHandler::OnLock() {}

void TetrisEventHandler::OnLineClear(DropResult dropResult) {}

// dx should always be either one of 1 or -1

void TetrisEventHandler::OnShift(int dx) {}

// softdrop is true if the movmenet is result of softdrop request

void TetrisEventHandler::OnMoveDown(bool softdrop) {}

void TetrisEventHandler::OnHardDrop() {}

void TetrisEventHandler::OnRotation(RotateResult result, bool clockwise) {}

void TetrisEventHandler::OnGameStart() {}

void TetrisEventHandler::OnWin() {}

TetrisEventHandler::TetrisEventHandler() {}

void TetrisEventHandlerComposite::OnLock()
{
    for (std::size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_handlers[i]->OnLock();
    }
}

void TetrisEventHandlerComposite::OnLineClear(DropResult dropResult)
{
    for (std::size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_handlers[i]->OnLineClear(dropResult);
    }
}

// dx should always be either one of 1 or -1

void TetrisEventHandlerComposite::OnShift(int dx)
{
    for (std::size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_handlers[i]->OnShift(dx);
    }
}

// softdrop is true if the movmenet is result of softdrop request

void TetrisEventHandlerComposite::OnMoveDown(bool softdrop)
{
    for (std::size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_handlers[i]->OnMoveDown(softdrop);
    }
}

void TetrisEventHandlerComposite::OnHardDrop()
{
    for (std::size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_handlers[i]->OnHardDrop();
    }
}

void TetrisEventHandlerComposite::OnRotation(RotateResult result, bool clockwise)
{
    for (std::size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_handlers[i]->OnRotation(result, clockwise);
    }
}

void TetrisEventHandlerComposite::AddHandler(std::shared_ptr<TetrisEventHandler> pHandler)
{
    if (std::find(m_handlers.begin(), m_handlers.end(), pHandler) == m_handlers.end())
    {
        m_handlers.push_back(pHandler);
    }
}

void TetrisEventHandlerComposite::RemoveHandler(std::shared_ptr<TetrisEventHandler> pHandler)
{
    std::remove(m_handlers.begin(), m_handlers.end(), pHandler);
}

void TetrisEventHandlerComposite::OnGameStart()
{
    for (std::size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_handlers[i]->OnGameStart();
    }
}

void TetrisEventHandlerComposite::OnWin()
{
    for (std::size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_handlers[i]->OnWin();
    }
}
