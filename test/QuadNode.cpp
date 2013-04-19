#include "gtest/gtest.h"

#include "internal/QuadNode.hpp"

using namespace testing;
using namespace geo;

class QuadNodeTests : public Test
{ };

TEST_F(QuadNodeTests, LocationCodeIsProperForRoot)
{
    QuadNode<int, 10> root(9);
    ASSERT_EQ("0000000000", root.locationCode().x.to_string());
    ASSERT_EQ("0000000000", root.locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForFirstChild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(0, 0);

    ASSERT_EQ("0000000000", child->locationCode().x.to_string());
    ASSERT_EQ("0000000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForSecondChild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(0, 1);

    ASSERT_EQ("0000000000", child->locationCode().x.to_string());
    ASSERT_EQ("0100000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForThirdChild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(1, 0);

    ASSERT_EQ("0100000000", child->locationCode().x.to_string());
    ASSERT_EQ("0000000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForFourthChild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(1, 1);

    ASSERT_EQ("0100000000", child->locationCode().x.to_string());
    ASSERT_EQ("0100000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForFirstGrandchild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(0, 0);
    child = child->child(0, 0);

    ASSERT_EQ("0000000000", child->locationCode().x.to_string());
    ASSERT_EQ("0000000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForSecondGrandchild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(1, 0);
    child = child->child(0, 1);

    ASSERT_EQ("0100000000", child->locationCode().x.to_string());
    ASSERT_EQ("0010000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForThirdGrandchild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(0, 1);
    child = child->child(1, 0);

    ASSERT_EQ("0010000000", child->locationCode().x.to_string());
    ASSERT_EQ("0100000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForFourthGrandchild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(1, 1);
    child = child->child(1, 1);

    ASSERT_EQ("0110000000", child->locationCode().x.to_string());
    ASSERT_EQ("0110000000", child->locationCode().y.to_string());
}

TEST_F(QuadNodeTests, LocationCodeIsProperForGrandgrandchild)
{
    QuadNode<int, 10> root(9);
    QuadNode<int, 10>* child = root.child(1, 1);
    child = child->child(0, 1);
    child = child->child(1, 1);

    ASSERT_EQ("0101000000", child->locationCode().x.to_string());
    ASSERT_EQ("0111000000", child->locationCode().y.to_string());
}
