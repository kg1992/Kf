#ifndef SCORE_KEEPER_H_
#define SCORE_KEEPER_H_

#include "LineClearInfoTable.h"

class ScoreKeeper
{
public:
    enum { ComboReward = 50 };
    ScoreKeeper();

    void ResetScore();

    void OnLineClear(LineClearType lct, bool backToBackChance, int combo, int level);

    void OnSoftDrop();

    void OnHardDrop();

    int GetScore();

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

private:
    int m_score;
};

#endif
