#include "gtest/gtest.h"

#include "QuadTree.hpp"
#include <string>

using namespace testing;
using namespace geo;

// Note that we have to place QuadTree constructors in additional parentheses because GTest macros
// are sensitive to semicolons in template parameters definitions.

// FakeClass modifies checker depending on what's happening to it.
struct FakeClass {
    int checker;
    const int checkerOriginal;

    FakeClass() : checker(1), checkerOriginal(1) {}
    FakeClass(const FakeClass& that) : checkerOriginal(that.checkerOriginal)
    {
        checker = 2; // isn't really a copy... :(
    };
    FakeClass(FakeClass&& that) : checkerOriginal(that.checkerOriginal)
    {
        checker = 3;
        that.checker = 0;
    }
    FakeClass& operator=(const FakeClass& that)
    {
        if (this != &that)
        {
            checker = 4;
        }
        return *this;
    }
};

class QuadTreeTests : public Test
{
};

TEST_F(QuadTreeTests, InitOkCase)
{
    ASSERT_NO_THROW((QuadTree<int>(1)));
}

TEST_F(QuadTreeTests, InitThrowsExceptionWhenSizeIsTooSmall)
{
    ASSERT_THROW((QuadTree<int>(0)), std::invalid_argument);
}

TEST_F(QuadTreeTests, InitThrowsExceptionWhenSizeIsNotAPowerOf2)
{
    ASSERT_THROW((QuadTree<int>(3)), std::invalid_argument);
}

TEST_F(QuadTreeTests, InsertValueFromWithinRangeWithOnlyWidthSpecified)
{
    QuadTree<std::string> tree(1);
    ASSERT_TRUE(tree.insert(0, 0, "fake"));
    ASSERT_TRUE(tree.insert(1, 1, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromBeyondRangeWithOnlyWidthSpecified)
{
    QuadTree<std::string> tree(1);
    ASSERT_FALSE(tree.insert(1.001, 1.001, "fake"));
    ASSERT_FALSE(tree.insert(-0.0001, -0.0001, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromWithinRangeWithStartingPointsSpecified)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_TRUE(tree.insert(2, 2, "fake"));
    ASSERT_TRUE(tree.insert(6, 6, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromBeyondRangeWithStartingPointsSpecified)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_FALSE(tree.insert(6.001, 6.001, "fake"));
    ASSERT_FALSE(tree.insert(1.9999, 1.9999, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromWithinRangeWithCapacitySpecified)
{
    QuadTree<std::string> tree(8, 2);
    ASSERT_TRUE(tree.insert(8, 8, "fake"));
    ASSERT_TRUE(tree.insert(0, 0, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromBeyondRangeWithCapacitySpecified)
{
    QuadTree<std::string> tree(8, 2);
    ASSERT_FALSE(tree.insert(8.001, 8.001, "fake"));
    ASSERT_FALSE(tree.insert(-0.001, -0.001, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromWithinRangeWithEverythingSpecified)
{
    QuadTree<std::string> tree(2, 1, 1, 3);
    ASSERT_TRUE(tree.insert(1, 1, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromBeyondRangeWithEverythingSpecified)
{
    QuadTree<std::string> tree(2, 1, 1, 3);
    ASSERT_FALSE(tree.insert(3.001, 3.001, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_EqualsToLowerBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_TRUE(tree.insert(2, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_EqualsToHigherBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_TRUE(tree.insert(6, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_EqualsToLowerBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_TRUE(tree.insert(4, 2, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_EqualsToHigherBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_TRUE(tree.insert(4, 6, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_IsLowerThanLowerBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_FALSE(tree.insert(1.999, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_IsHigherThanHigherBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_FALSE(tree.insert(6.001, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_IsLowerThanLowerBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_FALSE(tree.insert(4, 1.999, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_IsHigherThanHigherBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_FALSE(tree.insert(4, 6.001, "fake"));
}

TEST_F(QuadTreeTests, InsertASingleObjectTwiceProperly)
{
    QuadTree<FakeClass> tree(4, 2, 2);
    FakeClass foo;

    EXPECT_TRUE(tree.insert(3, 3, foo));
    EXPECT_TRUE(tree.insert(3, 3, foo));

    // TODO: telling the truth, it's test of size() method. Better use some kind of getters when
    // they're implemented.
    EXPECT_EQ(tree.size(), (size_t)2);
    ASSERT_EQ(foo.checker, 1); // check if original is still usable
}
