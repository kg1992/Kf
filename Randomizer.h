#ifndef RANDOMIZER_H_
#define RANDOMIZER_H_

#include "Tetrimino.h"

class Randomizer
{
public:
    TetriminoType Peek(int step);

    TetriminoType PopNext();

    void FillQueue(std::vector<TetriminoType>& tmQueue);

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

    void Reset();

    void Seed(unsigned int seed);

private:
    std::vector<TetriminoType> m_queue;
    std::minstd_rand m_rng;
};

#endif
