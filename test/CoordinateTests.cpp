#include "gtest/gtest.h"

#include "Coordinates.hpp"

#include <iostream>

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
// AffineCoordinatetrerTests
//

TEST_F(AffineCoordTrTests, CorrectTransformWithoutShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<0, 0, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.forward(orig);

    ASSERT_DOUBLE_EQ(1, newCoord.x());
    ASSERT_DOUBLE_EQ(1, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectTransformWithShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<1, 1, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.forward(orig);

    ASSERT_DOUBLE_EQ(2, newCoord.x());
    ASSERT_DOUBLE_EQ(2, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformWithoutShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<0, 0, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformWithShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<1, 1, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectTransformOfNonEdgePointWithoutShift)
{
    Coordinates orig(8, 10);
    AffineCoordTr<0, 0, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.forward(orig);
    ASSERT_DOUBLE_EQ(0.8, newCoord.x());
    ASSERT_DOUBLE_EQ(1, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectTransformOfNonEdgePointWithShift)
{
    Coordinates orig(8, 10);
    AffineCoordTr<1, 1, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.forward(orig);

    ASSERT_DOUBLE_EQ(1.8, newCoord.x());
    ASSERT_DOUBLE_EQ(2, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformOfNonEdgePointWithoutShift)
{
    Coordinates orig(7, 9);
    AffineCoordTr<0, 0, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformOfNonEdgePointWithShift)
{
    Coordinates orig(7, 9);
    AffineCoordTr<1, 1, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectTransformOfPointsNotStartingOn_0)
{
    Coordinates orig(10, 10);
    AffineCoordTr<1, 1, 1, 1> tr(5, 5, 10, 10);

    Coordinates newCoord = tr.forward(orig);

    ASSERT_DOUBLE_EQ(2, newCoord.x());
    ASSERT_DOUBLE_EQ(2, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformOfPointsNotStartingOn_0)
{
    Coordinates orig(10, 10);
    AffineCoordTr<1, 1, 1, 1> tr(5, 5, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectTransformOfPointsNotStartingOn_0_ToNegativeSystem)
{
    Coordinates orig(8, 8);
    AffineCoordTr<-2, -2, 1, 1> tr(3, 3, 8, 8);

    Coordinates newCoord = tr.forward(orig);

    ASSERT_DOUBLE_EQ(-1, newCoord.x());
    ASSERT_DOUBLE_EQ(-1, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReverseTransformOfPointsNotStartingOn_0_ToNegativeSystem)
{
    Coordinates orig(8, 8);
    AffineCoordTr<-2, -2, 1, 1> tr(3, 3, 8, 8);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}
