#include "gtest/gtest.h"

#include "Coordinates.hpp"

using namespace testing;
using namespace geo;

class CoordinatesTests : public Test {};
class AffineCoordTrTests : public Test {};

//
// CoordinatesTests
//

TEST_F(CoordinatesTests, Init_X_IsOk)
{
    Coordinates coord(1, 2);

    ASSERT_EQ(coord.x(), 1.0);
}

TEST_F(CoordinatesTests, Init_Y_IsOk)
{
    Coordinates coord(1, 2);

    ASSERT_EQ(coord.y(), 2.0);
}

TEST_F(CoordinatesTests, SameCoordinatesAreEqual)
{
    Coordinates coord1(-3, 5);
    Coordinates coord2(-3, 5);

    ASSERT_EQ(coord1, coord2);
}

TEST_F(CoordinatesTests, DifferentCoordinatesAreNotEqual)
{
    Coordinates coord1(-3, 4);
    Coordinates coord2(-3, 5);

    ASSERT_NE(coord1, coord2);
}

//
// AffineCoordinateTransformerTests
//

TEST_F(AffineCoordTrTests, CorrectTransformationWithoutShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<0, 0, 1, 1> transform(0, 0, 10, 10);

    Coordinates newCoord = transform.forward(orig);

    ASSERT_DOUBLE_EQ(1, newCoord.x());
    ASSERT_DOUBLE_EQ(1, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectTransformationWithShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<1, 1, 1, 1> transform(0, 0, 10, 10);

    Coordinates newCoord = transform.forward(orig);

    ASSERT_DOUBLE_EQ(2, newCoord.x());
    ASSERT_DOUBLE_EQ(2, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformationWithoutShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<0, 0, 1, 1> transform(0, 0, 10, 10);

    Coordinates newCoord = transform.reverse(transform.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformationWithShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<1, 1, 1, 1> transform(0, 0, 10, 10);

    Coordinates newCoord = transform.reverse(transform.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

//////////////////
//
//

TEST_F(AffineCoordTrTests, CorrectTransformationOfNonEdgePointWithoutShift)
{
    Coordinates orig(8, 10);
    AffineCoordTr<0, 0, 1, 1> transform(0, 0, 10, 10);

    Coordinates newCoord = transform.forward(orig);
    ASSERT_DOUBLE_EQ(0.8, newCoord.x());
    ASSERT_DOUBLE_EQ(1, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectTransformationOfNonEdgePointWithShift)
{
    Coordinates orig(8, 10);
    AffineCoordTr<1, 1, 1, 1> transform(0, 0, 10, 10);

    Coordinates newCoord = transform.forward(orig);

    ASSERT_DOUBLE_EQ(1.8, newCoord.x());
    ASSERT_DOUBLE_EQ(2, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformationOfNonEdgePointWithoutShift)
{
    Coordinates orig(7, 9);
    AffineCoordTr<0, 0, 1, 1> transform(0, 0, 10, 10);

    Coordinates newCoord = transform.reverse(transform.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformationOfNonEdgePointWithShift)
{
    Coordinates orig(7, 9);
    AffineCoordTr<1, 1, 1, 1> transform(0, 0, 10, 10);

    Coordinates newCoord = transform.reverse(transform.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}
