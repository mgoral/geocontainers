#include "gtest/gtest.h"

#include "internal/TreeNodeIterator.hpp"

using namespace testing;
using namespace geo;

class FakeNode
{
public:
    typedef size_t ElementType;

public:
    explicit FakeNode(size_t count) : next(nullptr), prev(nullptr), elCount(count) {}
    FakeNode(FakeNode& next, FakeNode& prev, size_t count)
        : next(&next), prev(&prev), elCount(count) {}

    size_t count() const
    {
        return elCount;
    }

    ElementType operator[](size_t element)
    {
        return element;
    }

    friend FakeNode* nextNode(FakeNode* node)
    {
        return node->next;
    }

    friend FakeNode* previousNode(FakeNode* node)
    {
        return node->prev;
    }

private:
    FakeNode* next;
    FakeNode* prev;
    size_t elCount;
};

class TreeNodeIteratorTests : public Test
{
};

TEST_F(TreeNodeIteratorTests, IteratorIsDefaultConstruable)
{
    ASSERT_NO_THROW((TreeNodeIterator<FakeNode>()));
}

TEST_F(TreeNodeIteratorTests, DefaultIteratorConvertsToFalse)
{
    ASSERT_FALSE((TreeNodeIterator<FakeNode>()));
}

TEST_F(TreeNodeIteratorTests, NonDefaultIteratorConvertsToTrue)
{
    FakeNode node(0);
    TreeNodeIterator<FakeNode> it(&node, it);
    ASSERT_TRUE(it);
}

TEST_F(TreeNodeIteratorTests, DefaultIteratorsAreEqual)
{
    TreeNodeIterator<FakeNode> a;
    TreeNodeIterator<FakeNode> b;
    ASSERT_TRUE(a == b);
}
