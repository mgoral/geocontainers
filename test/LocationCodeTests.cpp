#include "gtest/gtest.h"

#include "QuadTree.hpp"

using namespace testing;
using namespace geo;

class LocationCodeTests : public Test
{
};

TEST_F(LocationCodeTests, ProperRepresentationOf_0)
{
    LocationCode<6> loc(Coordinates(0, 0));
    EXPECT_EQ("000000", loc.x.to_string());
    EXPECT_EQ("000000", loc.y.to_string());
}

TEST_F(LocationCodeTests, ProperRepresentationOf_1)
{
    LocationCode<6> loc(Coordinates(1, 1));
    EXPECT_EQ("111111", loc.x.to_string());
    EXPECT_EQ("111111", loc.y.to_string());
}
