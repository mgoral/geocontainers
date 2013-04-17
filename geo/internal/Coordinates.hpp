#ifndef GEO_COORDINATES_HPP_
#define GEO_COORDINATES_HPP_

#include <limits>
#include <cmath>

namespace geo {

class Coordinates
{
public:
    Coordinates(double x, double y) : x_(x), y_(y) {}
    bool operator==(const Coordinates& rhs) const
    {
        return floatEq(x_, rhs.x()) && floatEq(y_, rhs.y());
    }
    bool operator!=(const Coordinates& rhs) const { return x_ != rhs.x() || y_ != rhs.y(); }
    bool operator<(const Coordinates& rhs) const { return x_ < rhs.x() && y_ < rhs.y(); }
    bool operator>(const Coordinates& rhs) const { return x_ > rhs.x() && y_ > rhs.y(); }

    double x() const { return x_; }
    double y() const { return y_; }

public:
    static bool floatEq(double a, double b)
    {
        // http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
        float diff = fabs(a - b);
        a = fabs(a);
        b = fabs(b);
        float largest = (b > a) ? b : a;

        if (diff <= largest * (4 * std::numeric_limits<double>::epsilon()))
            return true;
        return false;
    }

private:
    double x_;
    double y_;
};

/**
 * Transforms coordinates from one carthesian system to another.
 *
 * @param newStartX New systems' x-axis starting point.
 * @param newStartY New systems' y-axis starting point.
 * @param newWidthX New systems' x-axis width (newStartX + newWidthX defines maximum x-axis point).
 * @param newWidthY New systems' y-axis width (newStartY + newWidthY defines maximum y-axis point).
 */
template <int newStartX, int newStartY, size_t newWidthX, size_t newWidthY>
class CoordTr
{
public:
    CoordTr(int startX, int startY, size_t widthX, size_t widthY)
        : startX(startX), startY(startY), widthX(widthX), widthY(widthY)
    {
        shiftX = newStartX - startX;
        shiftY = newStartY - startY;
    }

    Coordinates forward(const Coordinates& oldCoord) const
    {
        double scaleX = (oldCoord.x() - startX) / static_cast<double>(widthX);
        double scaleY = (oldCoord.y() - startY) / static_cast<double>(widthY);

        return Coordinates(
            newStartX + (scaleX * newWidthX),
            newStartY + (scaleY * newWidthY)
        );
    }

    Coordinates reverse(const Coordinates& newCoord) const
    {
        double scaleX = (newCoord.x() - newStartX) / static_cast<double>(newWidthX);
        double scaleY = (newCoord.y() - newStartY) / static_cast<double>(newWidthY);

        return Coordinates(
            startX + (scaleX * widthX),
            startY + (scaleY * widthY)
        );
    }

private:
    int shiftX;
    int shiftY;
    int startX;
    int startY;
    int widthX;
    int widthY;
};

} // namespace geo

#endif

