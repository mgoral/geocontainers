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

TEST_F(AffineCoordTrTests, CorrecttrationWithoutShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<0, 0, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.forward(orig);

    ASSERT_DOUBLE_EQ(1, newCoord.x());
    ASSERT_DOUBLE_EQ(1, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrecttrationWithShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<1, 1, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.forward(orig);

    ASSERT_DOUBLE_EQ(2, newCoord.x());
    ASSERT_DOUBLE_EQ(2, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReversetrationWithoutShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<0, 0, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReversetrationWithShift)
{
    Coordinates orig(10, 10);
    AffineCoordTr<1, 1, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

//////////////////
//
//

TEST_F(AffineCoordTrTests, CorrecttrationOfNonEdgePointWithoutShift)
{
    Coordinates orig(8, 10);
    AffineCoordTr<0, 0, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.forward(orig);
    ASSERT_DOUBLE_EQ(0.8, newCoord.x());
    ASSERT_DOUBLE_EQ(1, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrecttrationOfNonEdgePointWithShift)
{
    Coordinates orig(8, 10);
    AffineCoordTr<1, 1, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.forward(orig);

    ASSERT_DOUBLE_EQ(1.8, newCoord.x());
    ASSERT_DOUBLE_EQ(2, newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReversetrationOfNonEdgePointWithoutShift)
{
    Coordinates orig(7, 9);
    AffineCoordTr<0, 0, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}

TEST_F(AffineCoordTrTests, CorrectReversetrationOfNonEdgePointWithShift)
{
    Coordinates orig(7, 9);
    AffineCoordTr<1, 1, 1, 1> tr(0, 0, 10, 10);

    Coordinates newCoord = tr.reverse(tr.forward(orig));

    ASSERT_DOUBLE_EQ(orig.x(), newCoord.x());
    ASSERT_DOUBLE_EQ(orig.y(), newCoord.y());
}
