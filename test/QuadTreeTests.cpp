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
    ASSERT_NO_THROW((QuadTree<int,16>()));
}

TEST_F(QuadTreeTests, InitThrowsExceptionWhenSizeIsTooSmall)
{
    ASSERT_THROW((QuadTree<int, 0>()), std::invalid_argument);
}

TEST_F(QuadTreeTests, InitThrowsExceptionWhenSizeIsNotAPowerOf2)
{
    ASSERT_THROW((QuadTree<int, 7>()), std::invalid_argument);
}

TEST_F(QuadTreeTests, InsertValueFromWithinRangeWithDefaultCtor)
{
    QuadTree<std::string, 4> tree;
    ASSERT_TRUE(tree.insert(1, 1, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromRangeBeyondWithDefaultCtor)
{
    QuadTree<std::string, 4> tree;
    ASSERT_FALSE(tree.insert(4, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromWithinRangeWithNonDefaultCtor)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_TRUE(tree.insert(2, 2, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromRangeBeyondWithNonDefaultCtor)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_FALSE(tree.insert(6, 6, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_EqualsToLowerBoundary)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_TRUE(tree.insert(2, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_EqualsToHigherBoundary)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_TRUE(tree.insert(5, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_EqualsToLowerBoundary)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_TRUE(tree.insert(4, 2, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_EqualsToHigherBoundary)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_TRUE(tree.insert(4, 5, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_IsLowerThanLowerBoundary)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_FALSE(tree.insert(1, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_IsHigherThanHigherBoundary)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_FALSE(tree.insert(6, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_IsLowerThanLowerBoundary)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_FALSE(tree.insert(4, 1, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_IsHigherThanHigherBoundary)
{
    QuadTree<std::string, 4> tree(2, 2);
    ASSERT_FALSE(tree.insert(4, 6, "fake"));
}

TEST_F(QuadTreeTests, InsertASingleObjectTwiceProperly)
{
    QuadTree<FakeClass, 4> tree(2, 2);
    FakeClass foo;

    EXPECT_TRUE(tree.insert(3, 3, foo));
    EXPECT_TRUE(tree.insert(3, 3, foo));

    // TODO: telling the truth, it's test of size() method. Better use some kind of getters when
    // they're implemented.
    EXPECT_EQ(tree.size(), (size_t)2);
    ASSERT_EQ(foo.checker, 1); // check if original is still usable
}
