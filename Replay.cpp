#include "Pch.h"
#include "Replay.h"
#include "TetrisGame.h"

void ReplayRecorder::PushRecord(const RecordType recordType)
{
    m_records.push_front({ std::chrono::steady_clock::now().time_since_epoch().count(), recordType });
}

void ReplayRecorder::Save(std::ostream& s)
{
    std::size_t size = m_records.size();
    s.write(reinterpret_cast<char*>(&size), sizeof(size));
    for (auto iter = m_records.rbegin(); iter != m_records.rend(); ++iter)
    {
        s.write(reinterpret_cast<char*>(&*iter), sizeof(*iter));
    }
}

void ReplayRecorder::Load(std::istream& s)
{
    std::size_t size;
    s.read(reinterpret_cast<char*>(&size), sizeof(size));
    for (std::size_t i = 0; i < size; ++i )
    {
        ReplayRecord record;
        s.read(reinterpret_cast<char*>(&record), sizeof(record));
        m_records.push_front(record);
    }
}

void ReplayRecorder::Clear()
{
    m_records.clear();
}

RecorderTetrisEventHandler::RecorderTetrisEventHandler(ReplayRecorder* pRecorder)
    : m_pRecorder(pRecorder)
{
}

void RecorderTetrisEventHandler::OnLock()
{
    m_pRecorder->PushRecord(RT_Lock);
}

void RecorderTetrisEventHandler::OnLineClear(DropResult dropResult)
{
}

void RecorderTetrisEventHandler::OnShift(int dx)
{
    if (dx > 0)
        m_pRecorder->PushRecord(RT_ShiftRight);
    else if (dx < 0)
        m_pRecorder->PushRecord(RT_ShiftLeft);
}

void RecorderTetrisEventHandler::OnMoveDown(bool softdrop)
{
    m_pRecorder->PushRecord(softdrop ? RT_Softdrop : RT_Gravity);
}

void RecorderTetrisEventHandler::OnHardDrop()
{
    m_pRecorder->PushRecord(RT_Harddrop);
}

void RecorderTetrisEventHandler::OnRotation(RotateResult result, bool clockwise)
{
    m_pRecorder->PushRecord(clockwise ? RT_RotateClockwise : RT_RotateAnticlockwise);
}

void RecorderTetrisEventHandler::OnGameStart()
{
    m_pRecorder->PushRecord(RT_GameStart);
}
