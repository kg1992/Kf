#ifndef TETRIS_EVENT_HANDLER_H_
#define TETRIS_EVENT_HANDLER_H_

struct DropResult;
struct RotateResult;

// Tetris Event Handler. Override this class and register to tetris game to listen on many items.
class TetrisEventHandler
{
public:
    virtual void OnLock();

    virtual void OnLineClear(DropResult dropResult);

    // dx should always be either one of 1 or -1
    virtual void OnShift(int dx);

    // softdrop is true if the movmenet is result of softdrop request
    virtual void OnMoveDown(bool softdrop);

    virtual void OnHardDrop();

    virtual void OnRotation(RotateResult result, bool clockwise);

    virtual void OnGameStart();

    virtual void OnWin();

protected:
    TetrisEventHandler();
};

class TetrisEventHandlerComposite : TetrisEventHandler
{
public:
    void OnLock() override;

    void OnLineClear(DropResult dropResult) override;

    void OnShift(int dx) override;

    void OnMoveDown(bool softdrop) override;

    void OnHardDrop() override;

    void OnRotation(RotateResult result, bool clockwise) override;

    void AddHandler(std::shared_ptr<TetrisEventHandler> pHandler);

    void RemoveHandler(std::shared_ptr<TetrisEventHandler> pHandler);

    void OnGameStart() override;

    void OnWin() override;

private:
    std::vector<std::shared_ptr<TetrisEventHandler>> m_handlers;
};

#endif
