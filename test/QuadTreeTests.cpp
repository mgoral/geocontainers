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

//
// QuadTree tests
//

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
    ASSERT_TRUE(tree.insert(0.99, 0.99, "fake"));
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
    ASSERT_TRUE(tree.insert(5.99, 5.99, "fake"));
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
    ASSERT_TRUE(tree.insert(7.99, 7.99, "fake"));
    ASSERT_TRUE(tree.insert(0, 0, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromBeyondRangeWithCapacitySpecified)
{
    QuadTree<std::string> tree(8, 2);
    ASSERT_FALSE(tree.insert(8.000, 8.000, "fake"));
    ASSERT_FALSE(tree.insert(-0.001, -0.001, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromWithinRangeWithEverythingSpecified)
{
    QuadTree<std::string> tree(2, 1, 1, 3);
    ASSERT_TRUE(tree.insert(1, 1, "fake"));
    ASSERT_TRUE(tree.insert(2.99, 2.99, "fake"));
}

TEST_F(QuadTreeTests, InsertValueFromBeyondRangeWithEverythingSpecified)
{
    QuadTree<std::string> tree(2, 1, 1, 3);
    ASSERT_FALSE(tree.insert(3.000, 3.000, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_EqualsToLowerBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_TRUE(tree.insert(2, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_X_EqualsToHigherBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_FALSE(tree.insert(6, 4, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_EqualsToLowerBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_TRUE(tree.insert(4, 2, "fake"));
}

TEST_F(QuadTreeTests, InsertValueWhere_Y_EqualsToHigherBoundary)
{
    QuadTree<std::string> tree(4, 2, 2);
    ASSERT_FALSE(tree.insert(4, 6, "fake"));
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

TEST_F(QuadTreeTests, ClearLeavesNoElementsInATree)
{
    QuadTree<std::string> tree(4);
    EXPECT_TRUE(tree.insert(1, 1, "fake"));

    tree.clear();
    ASSERT_EQ((size_t)0, tree.size());
}

TEST_F(QuadTreeTests, EraseASingleElement)
{
    QuadTree<std::string> tree(4);
    EXPECT_TRUE(tree.insert(1, 1, "fake"));

    tree.erase(1, 1);
    ASSERT_EQ((size_t)0, tree.size());
}

TEST_F(QuadTreeTests, EraseASingleElementWithFloatCoordinates)
{
    QuadTree<std::string> tree(4);
    EXPECT_TRUE(tree.insert(1.00001, 1.00002, "fake"));

    tree.erase(1.00001, 1.00002);
    ASSERT_EQ((size_t)0, tree.size());
}

TEST_F(QuadTreeTests, EraseDoesntRemoveNotMatchingElementWithFloatCoordinates)
{
    QuadTree<std::string> tree(4);
    EXPECT_TRUE(tree.insert(1.00001, 1.00001, "fake"));

    tree.erase(1.00001, 1.00002);
    ASSERT_EQ((size_t)0, tree.size());
}

TEST_F(QuadTreeTests, EraseASingleElementWhenThereIsMoreThan1)
{
    QuadTree<std::string> tree(4);
    EXPECT_TRUE(tree.insert(0, 0, "fake"));
    EXPECT_TRUE(tree.insert(1, 2, "fake"));

    // TODO: telling the truth, it's test of size() method. Better use some kind of getters when
    // they're implemented.
    tree.erase(1, 2);
    ASSERT_EQ((size_t)1, tree.size());
}

TEST_F(QuadTreeTests, EraseDoesntThrowOnIncorrectRequest)
{
    QuadTree<std::string> tree(4);

    tree.erase(1, 1);
    ASSERT_NO_THROW(tree.size());
}

TEST_F(QuadTreeTests, EraseRemovesNothingOnIncorrectRequest)
{
    QuadTree<std::string> tree(4);
    EXPECT_TRUE(tree.insert(1, 2, "fake"));

    // TODO: telling the truth, it's test of size() method. Better use some kind of getters when
    // they're implemented.
    tree.erase(1, 1);
    ASSERT_EQ((size_t)1, tree.size());
}

TEST_F(QuadTreeTests, EraseDoesntThrowWhenCoordinatesAreOutOfBoundaries)
{
    QuadTree<std::string> tree(4);
    //
    // TODO: telling the truth, it's test of size() method. Better use some kind of getters when
    // they're implemented.
    ASSERT_NO_THROW(tree.erase(11, 9));
}

TEST_F(QuadTreeTests, EraseRemovesNothingWhenCoordinatesAreOutOfBoundaries)
{
    QuadTree<std::string> tree(4);
    EXPECT_TRUE(tree.insert(3, 3, "fake"));

    // TODO: telling the truth, it's test of size() method. Better use some kind of getters when
    // they're implemented.
    tree.erase(10, 8);
    ASSERT_EQ((size_t)1, tree.size());
}

TEST_F(QuadTreeTests, EraseOnlyRemovesMatchingElements)
{
    QuadTree<std::string> tree(4);
    EXPECT_TRUE(tree.insert(1, 1, "fake"));
    EXPECT_TRUE(tree.insert(2, 1, "fake"));
    EXPECT_TRUE(tree.insert(1, 1, "fake"));
    EXPECT_TRUE(tree.insert(1, 3, "fake"));
    EXPECT_TRUE(tree.insert(1, 1, "fake"));

    // TODO: telling the truth, it's test of size() method. Better use some kind of getters when
    // they're implemented.
    tree.erase(1, 1);
    ASSERT_EQ((size_t)2, tree.size());
}

TEST_F(QuadTreeTests, InsertReturnsProperIterator)
{
    QuadTree<std::string> tree(4);
    tree.insert(0, 0, "");
    QuadTree<std::string>::iterator it = tree.insert(0, 0, "fake");

    ASSERT_EQ((size_t)4, it->size());
    ASSERT_EQ((size_t)4, (*it).size());
}

TEST_F(QuadTreeTests, BeginIsEqualToEndWhenTreeIsEmpty)
{
    QuadTree<std::string> tree(4, 4);
    ASSERT_EQ(tree.end(), tree.begin());
}

TEST_F(QuadTreeTests, BeginIsNotEqualToEndWhenTreeIsNotEmpty)
{
    QuadTree<std::string> tree(4, 4);
    tree.insert(0, 0, "fake");
    ASSERT_NE(tree.end(), tree.begin());
}

TEST_F(QuadTreeTests, IteratingThroughAllElements)
{
    // FIXME: test is implementation dependent. Order of iteration is not specified and we have to
    // check elements in a correct order to be able to perform the test.
    QuadTree<int> tree(4, 2);
    tree.insert(0, 0, 10);
    tree.insert(0, 0, 11);
    tree.insert(0, 1, 12);
    tree.insert(0, 3, 13);
    tree.insert(2, 4, 14);

    int n = 10;
    QuadTree<int>::iterator it;
    for (it = tree.begin(); it != tree.end(); ++it)
    {
        EXPECT_EQ(n, *it);
        ++n;
    }
    ASSERT_EQ(14, n);
}
