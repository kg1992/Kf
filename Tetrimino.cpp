#include "Tetrimino.h"

void MinoPatternIndexToXY(int ox, int oy, int i, int& mx, int& my)
{
    const int BoundingBoxSideLength = 4;
    const int dy = i / BoundingBoxSideLength;
    const int dx = i % BoundingBoxSideLength;
    mx = ox + dx;
    my = oy - dy;
}

void Tetrimino::IncreaseOrientation()
{
    orientation = static_cast<Orientation>((orientation + 1) % Orientation::O_End);
}

void Tetrimino::DecreaseOrientation()
{
    orientation = static_cast<Orientation>((orientation + Orientation::O_End - 1) % Orientation::O_End);
}

void Tetrimino::Serialize(std::ostream& os)
{
    os.write(reinterpret_cast<char*>(&x), sizeof(x));
    os.write(reinterpret_cast<char*>(&y), sizeof(y));
    os.write(reinterpret_cast<char*>(&type), sizeof(type));
    os.write(reinterpret_cast<char*>(&orientation), sizeof(orientation));
}

void Tetrimino::Deserialize(std::istream& is)
{
    is.read(reinterpret_cast<char*>(&x), sizeof(x));
    is.read(reinterpret_cast<char*>(&y), sizeof(y));
    is.read(reinterpret_cast<char*>(&type), sizeof(type));
    is.read(reinterpret_cast<char*>(&orientation), sizeof(orientation));
}
