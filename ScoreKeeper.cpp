#include "Pch.h"
#include "ScoreKeeper.h"

ScoreKeeper::ScoreKeeper()
    : m_score(0)
{
}

void ScoreKeeper::ResetScore()
{
    m_score = 0;
}

void ScoreKeeper::OnLineClear(LineClearType lct, bool backToBackChance, int combo, int level)
{
    const LineClearInfo& lci = LineClearInfoTable[lct];

    int addScore = lci.score;

    if (lci.difficult && backToBackChance)
        addScore += addScore / 2;

    addScore += ComboReward * combo;
    addScore *= level;
    m_score += addScore;
}

void ScoreKeeper::OnSoftDrop()
{
    m_score += 1;
}

void ScoreKeeper::OnHardDrop()
{
    m_score += 2;
}

int ScoreKeeper::GetScore()
{
    return m_score;
}

void ScoreKeeper::Serialize(std::ostream& os)
{
    os.write(reinterpret_cast<char*>(&m_score), sizeof(m_score));
}

void ScoreKeeper::Deserialize(std::istream& is)
{
    is.read(reinterpret_cast<char*>(&m_score), sizeof(m_score));
}
