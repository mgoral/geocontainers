#include "gtest/gtest.h"

#include "internal/QuadNode.hpp"

using namespace testing;
using namespace geo;

class QuadNodeTests : public Test
{
protected:
    void createTree()
    {
        QuadNode<int, 10>* child;

        child = root.child(0,0);
        child->child(0,0);
        child->child(0,1);
        child->child(1,0);
        child->child(1,1);

        child = root.child(0,1);
        child->child(1,0);

        child = root.child(1,1);
        child->child(1,0);
    }

    QuadNode<int, 10> root;
};

TEST_F(QuadNodeTests, LocationCodeIsProperForRoot)
{
    ASSERT_EQ("0000000000", root.locationCode().x.to_string());
    ASSERT_EQ("0000000000", root.locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForFirstChild)
{
    QuadNode<int, 10>* child = root.child(0, 0);

    ASSERT_EQ("0000000000", child->locationCode().x.to_string());
    ASSERT_EQ("0000000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForSecondChild)
{
    QuadNode<int, 10>* child = root.child(0, 1);

    ASSERT_EQ("0000000000", child->locationCode().x.to_string());
    ASSERT_EQ("0100000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForThirdChild)
{
    QuadNode<int, 10>* child = root.child(1, 0);

    ASSERT_EQ("0100000000", child->locationCode().x.to_string());
    ASSERT_EQ("0000000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForFourthChild)
{
    QuadNode<int, 10>* child = root.child(1, 1);

    ASSERT_EQ("0100000000", child->locationCode().x.to_string());
    ASSERT_EQ("0100000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForFirstGrandchild)
{
    QuadNode<int, 10>* child = root.child(0, 0);
    child = child->child(0, 0);

    ASSERT_EQ("0000000000", child->locationCode().x.to_string());
    ASSERT_EQ("0000000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForSecondGrandchild)
{
    QuadNode<int, 10>* child = root.child(1, 0);
    child = child->child(0, 1);

    ASSERT_EQ("0100000000", child->locationCode().x.to_string());
    ASSERT_EQ("0010000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForThirdGrandchild)
{
    QuadNode<int, 10>* child = root.child(0, 1);
    child = child->child(1, 0);

    ASSERT_EQ("0010000000", child->locationCode().x.to_string());
    ASSERT_EQ("0100000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForFourthGrandchild)
{
    QuadNode<int, 10>* child = root.child(1, 1);
    child = child->child(1, 1);

    ASSERT_EQ("0110000000", child->locationCode().x.to_string());
    ASSERT_EQ("0110000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForGrandgrandchild)
{
    QuadNode<int, 10>* child = root.child(1, 1);
    child = child->child(0, 1);
    child = child->child(1, 1);

    ASSERT_EQ("0101000000", child->locationCode().x.to_string());
    ASSERT_EQ("0111000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, NextNodeReturnsCorrectChild)
{
    createTree();
    QuadNode<int, 10>* child = root.child(0,0);
    ASSERT_EQ(root.child(0,0)->child(0,0), nextNode(child));
}

TEST_F(QuadNodeTests, NextNodeReturnsCorrectSibling)
{
    createTree();
    QuadNode<int, 10>* child = root.child(0,0)->child(0,0);
    ASSERT_EQ(root.child(0,0)->child(0,1), nextNode(child));
}

TEST_F(QuadNodeTests, NextNodeReturnsCorrectLastSibling)
{
    createTree();
    QuadNode<int, 10>* child = root.child(0,0)->child(1,0);
    ASSERT_EQ(root.child(0,0)->child(1,1), nextNode(child));
}

TEST_F(QuadNodeTests, NextNodeReturnsCorrectWhenGivenNodeIsALastChild)
{
    createTree();
    QuadNode<int, 10>* child = root.child(0,0)->child(1,1);
    ASSERT_EQ(root.child(0,1), nextNode(child));
}

TEST_F(QuadNodeTests, NextNodeCorrectlySkipsNulls)
{
    createTree();
    QuadNode<int, 10>* child = root.child(0,1)->child(1,0);
    ASSERT_EQ(root.child(1,1), nextNode(child));
}

TEST_F(QuadNodeTests, NextNodeOfTheLastNodeReturnsNull)
{
    createTree();
    QuadNode<int, 10>* child = root.child(1,1)->child(1,0);
    ASSERT_EQ(nullptr, nextNode(child));
}

TEST_F(QuadNodeTests, NextNodeOfNullPtrReturnsNull)
{
    QuadNode<int, 10>* child = nullptr;
    ASSERT_EQ(nullptr, nextNode(child));
}

TEST_F(QuadNodeTests, PreviousNodeReturnsParentWhenNodeIsTheOnlyChild)
{
    createTree();
    QuadNode<int, 10>* child = root.child(1,1)->child(1,0);
    ASSERT_EQ(root.child(1,1), previousNode(child));
}

TEST_F(QuadNodeTests, PreviousNodeReturnsParentWhenNodeIsTheFirstChild)
{
    createTree();
    QuadNode<int, 10>* child = root.child(0,0)->child(0,0);
    ASSERT_EQ(root.child(0,0), previousNode(child));
}

TEST_F(QuadNodeTests, PreviousNodeReturnsCorrectSibling)
{
    createTree();
    QuadNode<int, 10>* child = root.child(0,0)->child(1,1);
    ASSERT_EQ(root.child(0,0)->child(1,0), previousNode(child));
}

TEST_F(QuadNodeTests, PreviousNodeReturnsCorrectFirstSibling)
{
    createTree();
    QuadNode<int, 10>* child = root.child(0,0)->child(0,1);
    ASSERT_EQ(root.child(0,0)->child(0,0), previousNode(child));
}

TEST_F(QuadNodeTests, PreviousNodeCorrectlySkipsNulls)
{
    createTree();
    QuadNode<int, 10>* child = root.child(1,1);
    ASSERT_EQ(root.child(0,1)->child(1,0), previousNode(child));
}

TEST_F(QuadNodeTests, PreviousNodeOfTheRootNodeReturnsNull)
{
    createTree();
    QuadNode<int, 10>* child = &root;
    ASSERT_EQ(nullptr, previousNode(child));
}

TEST_F(QuadNodeTests, PreviousNodeOfNullPtrReturnsNull)
{
    QuadNode<int, 10>* child = nullptr;
    ASSERT_EQ(nullptr, previousNode(child));
}