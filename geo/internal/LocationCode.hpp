#ifndef GEO_LOCATIONCODE_HPP_
#define GEO_LOCATIONCODE_HPP_

#include <bitset>
#include <utility>

#include "Coordinates.hpp"

namespace geo {

/**
 * Class that creates location codes from given coordinates.
 *
 * @param size Number of bits which represent a local code. It is also the maximum number of
 * levels in a QuadTree.
 */
template <size_t size>
class LocationCode
{
public:
    /**
     * Constructor.
     * locationCode = coordinate * 2^(ROOT_LEVEL) = coordinate * 2^(size - 1)
     *
     * @param coord Coordinate from which a location code is created.
     */
    explicit LocationCode(const Coordinates& coord) :
        x_(coord.x() * (2 << (size - 2))),
        y_(coord.y() * (2 << (size - 2)))
    { }

    bool operator==(const LocationCode& rhs) const
    {
        return (x_ == rhs.x_ && y_ == rhs.y_);
    }

    LocationCode& operator++() const
    {

    }

    std::bitset<size> x() const
    {
        return std::bitset<size>(x_);
    }

    std::bitset<size> y() const
    {
        return std::bitset<size>(x_);
    }

private:
    unsigned x_;
    unsigned y_;

private:
    std::pair<bool, bool> incrementOrder;
};

template <typename ObjectType, size_t locCodeMaxSize>
struct ObjectWithLocationCode {
    LocationCode<locCodeMaxSize> location;
    ObjectType object;

    ObjectWithLocationCode(const LocationCode<locCodeMaxSize>& location, const ObjectType& object)
        : location(location), object(object) { }

    ObjectWithLocationCode(LocationCode<locCodeMaxSize>&& location, ObjectType&& object)
        : location(std::move(location)), object(object) { }
};

} // namespace geo

#endif
