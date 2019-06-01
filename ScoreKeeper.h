#ifndef SCORE_KEEPER_H_
#define SCORE_KEEPER_H_

#include "LineClearInfoTable.h"

class ScoreKeeper
{
public:
    enum { ComboReward = 50 };
    ScoreKeeper()
        : m_score(0)
    {
    }

    void ResetScore()
    {
        m_score = 0;
    }

    void OnLineClear(LineClearType lct, bool backToBackChance, int combo, int level)
    {
        const LineClearInfo& lci = LineClearInfoTable[lct];

        int addScore = lci.score;

        if (lci.difficult && backToBackChance)
            addScore += addScore / 2;

        addScore += ComboReward * combo;
        addScore *= level;
        m_score += addScore;
    }

    void OnSoftDrop()
    {
        m_score += 1;
    }

    void OnHardDrop()
    {
        m_score += 2;
    }

    int GetScore()
    {
        return m_score;
    }

    void Serialize(std::ostream& os)
    {
        os.write(reinterpret_cast<char*>(&m_score), sizeof(m_score));
    }

    void Deserialize(std::istream& is)
    {
        is.read(reinterpret_cast<char*>(&m_score), sizeof(m_score));
    }

private:
    int m_score;
};

#endif
