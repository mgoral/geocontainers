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
 * Affine transformation is used. Current implemententation omits systems rotations. The
 * transformation is given by the following equations:
 *
 * X' = sx * X * cos(rotation)  - sy * Y * sin(rotation) + x0
 * Y' = sx * X * sin(rotation)  + sy * Y * cos(rotation) + y0
 * and rotation = 0.
 *
 * @param newStartX New systems' x-axis starting point.
 * @param newStartY New systems' y-axis starting point.
 * @param newWidthX New systems' x-axis width (newStartX + newWidthX defines maximum x-axis point).
 * @param newWidthY New systems' y-axis width (newStartY + newWidthY defines maximum y-axis point).
 */
template <int newStartX, int newStartY, size_t newWidthX, size_t newWidthY>
class AffineCoordTr
{
public:
    AffineCoordTr(int startX, int startY, size_t widthX, size_t widthY)
    {
        scaleX = static_cast<double>(newWidthX) / static_cast<double>(widthX);
        scaleY = static_cast<double>(newWidthY) / static_cast<double>(widthY);
        shiftX = newStartX - startX;
        shiftY = newStartY - startY;
    }

    Coordinates forward(const Coordinates& oldCoord) const
    {
        return Coordinates(
            scaleX * oldCoord.x() + shiftX,
            scaleY * oldCoord.y() + shiftY
        );
    }

    Coordinates reverse(const Coordinates& newCoord) const
    {
        return Coordinates(
            (newCoord.x() - shiftX) / scaleX,
            (newCoord.y() - shiftY) / scaleY
        );
    }

private:
    double scaleX;
    double scaleY;
    int shiftX;
    int shiftY;
};

} // namespace geo

#endif

