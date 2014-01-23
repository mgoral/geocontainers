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
        EXPECT_CALL(node, parent())
            .WillRepeatedly(ReturnRef(parent));
        EXPECT_CALL(node, NotEquals(Ref(parent)))
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

    NodeMock node;

    EXPECT_CALL(node, parent())
        .WillRepeatedly(ReturnRef(node));
    EXPECT_CALL(node, NotEquals(Ref(node)))
        .WillRepeatedly(Return(false));

    TreeNodeIterator<NodeMock> old(&node, 0);
    TreeNodeIterator<NodeMock> testIt(&node, 0);

    TreeNodeIterator<NodeMock> ret = testIt++;

    ASSERT_EQ(old, testIt);
}

TEST_F(TreeNodeIteratorTests, PostIncrementationReturnsOldIterator)
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
    testIt++; // Actually, that's the whole test body ;)
    NodeMock::ElementType ret = *testIt;

    ASSERT_EQ(toRet, ret);
}
