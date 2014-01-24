#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "internal/TreeNodeIterator.hpp"
#include "NodeMock.hpp"

using namespace testing;
using namespace geo;
using namespace geo::mocks;

class TreeNodeIteratorTests : public Test
{
protected:
    void configureNodeParent(NodeMock& node, NodeMock& parent)
    {
        // REMEMBER: Google Mock doesn't require the following statements to be called at all!
        // If there are 'n' 'WillOnce()s' followed by 'WillRepeatedly' Google Mock will assume
        // 'AtLeast(n)' cardinality. But we have 0 'WillOnce()s', so 'AtLeast(0)' cardinality will
        // be used.
        // It is useful because we can use a single configurator function while some tests require
        // e.g. operator!=() and the others operator==().

        EXPECT_CALL(node, parent())
            .WillRepeatedly(ReturnRef(parent));
        EXPECT_CALL(node, NotEquals(Ref(parent)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(node, Equals(Ref(parent)))
            .WillRepeatedly(Return(false));
    }

    void configureHeaderNode(NodeMock& header)
    {
        // @see comment at configureNodeParent

        EXPECT_CALL(header, parent())
            .WillRepeatedly(ReturnRef(header));
        EXPECT_CALL(header, NotEquals(Ref(header)))
            .WillRepeatedly(Return(false));
        EXPECT_CALL(header, Equals(Ref(header)))
            .WillRepeatedly(Return(true));
    }
};

TEST_F(TreeNodeIteratorTests, IteratorIsDefaultConstruable)
{
    ASSERT_NO_THROW((TreeNodeIterator<NodeMock>()));
}

TEST_F(TreeNodeIteratorTests, DefaultIteratorConvertsToFalse)
{
    ASSERT_FALSE((TreeNodeIterator<NodeMock>()));
}

TEST_F(TreeNodeIteratorTests, NonDefaultIteratorConvertsToTrue)
{
    NodeMock node;
    TreeNodeIterator<NodeMock> it(&node, it);
    ASSERT_TRUE(it);
}

TEST_F(TreeNodeIteratorTests, DefaultIteratorsAreEqual)
{
    TreeNodeIterator<NodeMock> a;
    TreeNodeIterator<NodeMock> b;

    ASSERT_EQ(a, b);
}

TEST_F(TreeNodeIteratorTests, NonDefaultIteratorsAreEqualWhenTheyPointToTheSameElement)
{
    NodeMock node;
    TreeNodeIterator<NodeMock> a(&node, 2);
    TreeNodeIterator<NodeMock> b(&node, 2);
    ASSERT_EQ(a, b);
}

TEST_F(TreeNodeIteratorTests, NonDefaultIteratorsAreNotEqualWhenTheyPointToDifferentNodes)
{
    NodeMock node1;
    NodeMock node2;
    TreeNodeIterator<NodeMock> a(&node1, 2);
    TreeNodeIterator<NodeMock> b(&node2, 2);
    ASSERT_NE(a, b);
}

TEST_F(TreeNodeIteratorTests, NonDefaultIteratorsAreNotEqualWhenTheyPointToDifferentPositions)
{
    NodeMock node;
    TreeNodeIterator<NodeMock> a(&node, 1);
    TreeNodeIterator<NodeMock> b(&node, 2);
    ASSERT_NE(a, b);
}

TEST_F(TreeNodeIteratorTests, IteratorDereferencesCorrectNodeElement)
{
    NodeMock node;
    NodeMock::ElementType toRet = 2;
    size_t iteratorPosition = 0;

    EXPECT_CALL(node, At(iteratorPosition))
        .WillOnce(ReturnRef(toRet));

    TreeNodeIterator<NodeMock> testIt(&node, iteratorPosition);

    NodeMock::ElementType ret = *testIt;
    ASSERT_EQ(toRet, ret);
}

TEST_F(TreeNodeIteratorTests, IncrementationDoesntChangeIteratorOnHeaderNode)
{
    // Header: tree Super Root, usually returned by tree::end(). It is special, because it is its
    // own parent (i.e. Node::parent() returns *this)
    // Note that iterator position on a storage shouldn't be taken into account (and shouldn't
    // change).

    NodeMock node;

    configureHeaderNode(node);

    TreeNodeIterator<NodeMock> old(&node, 1);
    TreeNodeIterator<NodeMock> testIt(&node, 1);

    testIt++;

    ASSERT_EQ(old, testIt);
}

TEST_F(TreeNodeIteratorTests, IncrementationReturnsCorrectValue)
{
    NodeMock parent;
    NodeMock node;

    configureNodeParent(node, parent);

    EXPECT_CALL(node, count())
        .WillRepeatedly(Return(3));

    TreeNodeIterator<NodeMock> testIt(&node, 1);
    testIt++;

    // We will check to what iterator points by dereferencing it.
    // It should point to 2nd element.
    NodeMock::ElementType toRet = 42;
    EXPECT_CALL(node, At(2))
        .WillOnce(ReturnRef(toRet));

    NodeMock::ElementType ret = *testIt;
    ASSERT_EQ(toRet, ret);
}

TEST_F(TreeNodeIteratorTests, PostIncrementationReturnsOldIterator)
{
    NodeMock parent;
    NodeMock node;

    configureNodeParent(node, parent);

    // Report that mock stores more elements than iterator increments to. This way iterator won't
    // jump to the next node.
    EXPECT_CALL(node, count())
        .WillRepeatedly(Return(3));

    TreeNodeIterator<NodeMock> old(&node, 1);
    TreeNodeIterator<NodeMock> testIt(&node, 1);

    TreeNodeIterator<NodeMock> ret = testIt++;

    EXPECT_NE(old, testIt);
    EXPECT_EQ(old, ret);
}

TEST_F(TreeNodeIteratorTests, PreIncrementationReturnsNewIterator)
{
    NodeMock parent;
    NodeMock node;

    configureNodeParent(node, parent);

    // Report that mock is able to store more elements than iterator increments to. That way
    // iterator won't jump to the next node.
    EXPECT_CALL(node, count())
        .WillRepeatedly(Return(3));

    TreeNodeIterator<NodeMock> old(&node, 0);
    TreeNodeIterator<NodeMock> testIt(&node, 0);

    TreeNodeIterator<NodeMock> ret = ++testIt;

    EXPECT_NE(old, ret);
    EXPECT_EQ(testIt, ret);
}

TEST_F(TreeNodeIteratorTests, IteratorIncrementationCanSwitchToNextNode)
{
    NodeMock parent;
    NodeMock next;
    NodeMock node;

    configureNodeParent(node, parent);
    configureNodeParent(next, parent);

    // This time we initialize iterator on last position of maximum node storage size. It should
    // cause iterator to jump to the next node on incrementation.
    EXPECT_CALL(node, count())
        .WillRepeatedly(Return(3));
    EXPECT_CALL(node, NextNode())
        .WillOnce(ReturnRef(next));

    // Iterator will check if next node has any elements that it can point to.
    EXPECT_CALL(next, count())
        .WillRepeatedly(Return(3));

    // We check if iterator points to the new node by expecting that it will ask for its storage on
    // dereferencing.
    NodeMock::ElementType toRet = 42;
    EXPECT_CALL(next, At(0))
        .WillOnce(ReturnRef(toRet));

    TreeNodeIterator<NodeMock> testIt(&node, 2);
    testIt++; // Whole test trigger ;)
    NodeMock::ElementType ret = *testIt;

    ASSERT_EQ(toRet, ret);
}

/**
  * This test checks if TreeNodeIterator can increment nodes in a loop. We start at 'node' but
  * incrementing iterator should jump to 'next'. 'next' doesn't have any elements that iterator
  * can point to so it continues its search and finds 'nextNext' which fortunately has some
  * elements stored.
  */
TEST_F(TreeNodeIteratorTests, IteratorIncrementationCanJumpBySeveralNodes)
{

    NodeMock parent;
    NodeMock next;
    NodeMock nextNext;
    NodeMock node;

    configureNodeParent(node, parent);
    configureNodeParent(next, parent);
    configureNodeParent(nextNext, parent);

    // This time we initialize iterator on last position of maximum node storage size. It should
    // cause iterator to jump to the next node on incrementation.
    EXPECT_CALL(node, count())
        .WillRepeatedly(Return(3));
    EXPECT_CALL(node, NextNode())
        .WillOnce(ReturnRef(next));

    // Iterator will check if next node has any elements that it can point to. Say no, so we can
    // jump to nextNext.
    EXPECT_CALL(next, count())
        .WillRepeatedly(Return(0));
    EXPECT_CALL(next, NextNode())
        .WillOnce(ReturnRef(nextNext));

    // nextNext is the final node in this test.
    EXPECT_CALL(nextNext, count())
        .WillRepeatedly(Return(3));

    // We check if iterator points to the new node by expecting that it will ask for its storage on
    // dereferencing.
    NodeMock::ElementType toRet = 31;
    EXPECT_CALL(nextNext, At(0))
        .WillOnce(ReturnRef(toRet));

    TreeNodeIterator<NodeMock> testIt(&node, 2);
    testIt++; // Whole test trigger ;)
    NodeMock::ElementType ret = *testIt;

    ASSERT_EQ(toRet, ret);
}

TEST_F(TreeNodeIteratorTests, IncrementingStopsAtHeaderNode)
{
    NodeMock header;
    NodeMock parent;
    NodeMock node;

    configureNodeParent(node, parent);
    configureHeaderNode(header);

    EXPECT_CALL(node, count())
        .WillRepeatedly(Return(3));
    EXPECT_CALL(node, NextNode())
        .WillOnce(ReturnRef(header));

    TreeNodeIterator<NodeMock> testIt(&node, 2);
    testIt++;

    TreeNodeIterator<NodeMock> headerIt(&header, 0);
    ASSERT_EQ(headerIt, testIt);

}

TEST_F(TreeNodeIteratorTests, DecrementationOnHeaderNodeReturnsCorrectPreviousNode)
{
    // Header: tree Super Root, usually returned by tree::end(). It is special, because it is its
    // own parent (i.e. Node::parent() returns *this)

    NodeMock previousParent;
    NodeMock previous;
    NodeMock node;

    configureNodeParent(previous, previousParent);
    configureHeaderNode(node);

    EXPECT_CALL(node, PreviousNode())
        .WillOnce(ReturnRef(previous));

    EXPECT_CALL(previous, count())
        .WillRepeatedly(Return(3));

    // We will check to what iterator points by dereferencing it.
    // It should point to the last element of 'previous'.
    NodeMock::ElementType toRet = 11;
    EXPECT_CALL(previous, At(2))
        .WillOnce(ReturnRef(toRet));

    TreeNodeIterator<NodeMock> testIt(&node, 0);
    testIt--;

    NodeMock::ElementType ret = *testIt;

    ASSERT_EQ(toRet, ret);
}

TEST_F(TreeNodeIteratorTests, DecrementationReturnsCorrectValue)
{
    NodeMock parent;
    NodeMock node;

    configureNodeParent(node, parent);

    TreeNodeIterator<NodeMock> testIt(&node, 1);
    testIt--;

    // We will check to what iterator points by dereferencing it.
    // It should point to 0th element.
    NodeMock::ElementType toRet = 42;
    EXPECT_CALL(node, At(0))
        .WillOnce(ReturnRef(toRet));

    NodeMock::ElementType ret = *testIt;
    ASSERT_EQ(toRet, ret);
}

TEST_F(TreeNodeIteratorTests, PostDecrementationReturnsOldIterator)
{
    NodeMock parent;
    NodeMock node;

    configureNodeParent(node, parent);

    TreeNodeIterator<NodeMock> old(&node, 1);
    TreeNodeIterator<NodeMock> testIt(&node, 1);

    TreeNodeIterator<NodeMock> ret = testIt--;

    EXPECT_NE(old, testIt);
    EXPECT_EQ(old, ret);
}

TEST_F(TreeNodeIteratorTests, PreDecrementationReturnsNewIterator)
{
    NodeMock parent;
    NodeMock node;

    configureNodeParent(node, parent);

    TreeNodeIterator<NodeMock> old(&node, 2);
    TreeNodeIterator<NodeMock> testIt(&node, 2);

    TreeNodeIterator<NodeMock> ret = --testIt;

    EXPECT_NE(old, ret);
    EXPECT_EQ(ret, testIt);
}

TEST_F(TreeNodeIteratorTests, IteratorDecrementationCanSwitchToPreviousNode)
{
    NodeMock parent;
    NodeMock previous;
    NodeMock node;

    configureNodeParent(node, parent);
    configureNodeParent(previous, parent);

    EXPECT_CALL(node, PreviousNode())
        .WillOnce(ReturnRef(previous));

    EXPECT_CALL(previous, count())
        .WillRepeatedly(Return(3));

    // We check if iterator points to the new node by expecting that it will ask for its storage on
    // dereferencing.
    NodeMock::ElementType toRet = 42;
    EXPECT_CALL(previous, At(2))
        .WillOnce(ReturnRef(toRet));

    TreeNodeIterator<NodeMock> testIt(&node, 0);
    testIt--; // Whole test trigger ;)
    NodeMock::ElementType ret = *testIt;

    ASSERT_EQ(toRet, ret);
}

/**
  * This test checks if TreeNodeIterator can decremen nodes in a loop. We start at 'node' but
  * decrementing iterator should jump to 'previous'. 'previous' doesn't have any elements that
  * iterator can point to so it continues its search and finds 'previousPrevious' which
  * fortunately has some elements stored.
  */
TEST_F(TreeNodeIteratorTests, IteratorDecrementationCanJumpBySeveralNodes)
{
    NodeMock parent;
    NodeMock previous;
    NodeMock previousPrevious;
    NodeMock node;

    configureNodeParent(node, parent);
    configureNodeParent(previous, parent);
    configureNodeParent(previousPrevious, parent);

    // This time we initialize iterator on the first position of node storage size. It should
    // cause iterator to jump to the previous node on decrementation.
    EXPECT_CALL(node, PreviousNode())
        .WillOnce(ReturnRef(previous));

    // Iterator will check if previous node has any elements that it can point to. Say no, so we can
    // jump to the previousPrevious.
    EXPECT_CALL(previous, count())
        .WillRepeatedly(Return(0));
    EXPECT_CALL(previous, PreviousNode())
        .WillOnce(ReturnRef(previousPrevious));

    // previousPrevious is the final node in this test.
    EXPECT_CALL(previousPrevious, count())
        .WillRepeatedly(Return(5));

    // We check if iterator points to the new node by expecting that it will ask for its storage on
    // dereferencing.
    NodeMock::ElementType toRet = 44;
    EXPECT_CALL(previousPrevious, At(4))
        .WillOnce(ReturnRef(toRet));

    TreeNodeIterator<NodeMock> testIt(&node, 0);
    testIt--; // Whole test trigger ;)
    NodeMock::ElementType ret = *testIt;

    ASSERT_EQ(toRet, ret);
}

TEST_F(TreeNodeIteratorTests, DecrementingIteratorDoesntResultInHeaderOrRoot)
{
    NodeMock header;
    NodeMock node;

    configureNodeParent(node, header);
    configureHeaderNode(header);

    EXPECT_CALL(node, PreviousNode())
        .WillRepeatedly(ReturnRef(header));
    EXPECT_CALL(header, count())
        .WillRepeatedly(Return(0));

    TreeNodeIterator<NodeMock> testIt(&node, 0);

    testIt--;

    TreeNodeIterator<NodeMock> headerIt(&header, 0);
    TreeNodeIterator<NodeMock> rootIt(&node, 0);

    EXPECT_NE(headerIt, testIt);
    EXPECT_NE(rootIt, testIt);
}
