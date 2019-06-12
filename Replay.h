#ifndef REPLAY_H_
#define REPLAY_H_

#include "TetrisEventHandler.h"

enum RecordType : uint8_t
{
    RT_GameStart,
    RT_ShiftRight,
    RT_ShiftLeft,
    RT_RotateClockwise,
    RT_RotateAnticlockwise,
    RT_Lock,
    RT_Hold,
    RT_Gravity,
    RT_Softdrop,
    RT_Harddrop,
    RT_GameOver,
    RT_TakeGarbage,
    RT_Win
};

struct ReplayRecord
{
    long long nsTimestamp;
    RecordType move;;
};

class ReplayRecorder
{
public:
    void PushRecord(const RecordType recordType);

    void Save(std::ostream& s);

    void Load(std::istream& s);

    void Clear();

private:
    std::list<ReplayRecord> m_records;
};

class RecorderTetrisEventHandler : public TetrisEventHandler
{
public:
    RecorderTetrisEventHandler(ReplayRecorder* pRecorder);

    void OnLock() override;

    void OnLineClear(DropResult dropResult) override;

    void OnShift(int dx) override;

    void OnMoveDown(bool softdrop) override;

    void OnHardDrop() override;

    void OnRotation(RotateResult result, bool clockwise) override;

    void OnGameStart() override;

private:
    ReplayRecorder* m_pRecorder;
};

#endif
