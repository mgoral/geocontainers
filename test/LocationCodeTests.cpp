#include "gtest/gtest.h"

#include "internal/LocationCode.hpp"

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
    LocationCode<6> loc(Coordinates(0.99, 0.99));
    EXPECT_EQ("011111", loc.x.to_string());
    EXPECT_EQ("011111", loc.y.to_string());
}
