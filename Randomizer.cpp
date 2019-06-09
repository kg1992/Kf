#include <sstream>
#include "Randomizer.h"


TetriminoType Randomizer::Peek(int step)
{
    return step < static_cast<int>(m_queue.size())
        ? *(m_queue.rbegin() + step)
        : TT_End;
}

TetriminoType Randomizer::PopNext()
{
    while (m_queue.size() < 7)
    {
        FillQueue(m_queue);
    }
    TetriminoType type = m_queue.back();
    m_queue.pop_back();
    return type;
}

void Randomizer::FillQueue(std::vector<TetriminoType>& tmQueue)
{
    std::vector<TetriminoType> bag =
    {
        TT_I, TT_O, TT_T, TT_S, TT_Z, TT_J, TT_L
    };

    while (!bag.empty())
    {
        int i = m_rng() % bag.size();
        tmQueue.insert(tmQueue.begin(), bag[i]);
        bag.erase(bag.begin() + i);
    }
}

void Randomizer::Serialize(std::ostream& os)
{
    std::size_t queueSize = m_queue.size();
    os.write(reinterpret_cast<char*>(&queueSize), sizeof(queueSize));
    os.write(reinterpret_cast<char*>(m_queue.data()), static_cast<std::streamsize>(sizeof(m_queue[0])) * queueSize);

    std::stringstream ss;
    ss << m_rng;
    std::string srng = ss.str();
    int srngSize = srng.size();
    os.write(reinterpret_cast<char*>(&srngSize), sizeof(srngSize));
    os.write(srng.c_str(), srngSize);
}

void Randomizer::Deserialize(std::istream& is)
{
    std::size_t queueSize = 0;
    is.read(reinterpret_cast<char*>(&queueSize), sizeof(queueSize));
    m_queue.resize(queueSize);
    is.read(reinterpret_cast<char*>(m_queue.data()), static_cast<std::streamsize>(sizeof(m_queue[0])) * queueSize);

    int srngSize = 0;
    is.read(reinterpret_cast<char*>(&srngSize), sizeof(srngSize));
    std::vector<char> vrng;
    vrng.resize(srngSize);
    is.read(vrng.data(), srngSize);
    std::string srng(vrng.begin(), vrng.end());
    std::stringstream ss(srng);
    ss >> m_rng;
}

void Randomizer::Reset()
{
    m_queue.clear();
}

void Randomizer::Seed(unsigned int seed)
{
    m_rng.seed(seed);
}