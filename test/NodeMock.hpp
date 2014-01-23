#ifndef NODEMOCK_HPP_
#define NODEMOCK_HPP_

#include "gmock/gmock.h"

namespace geo {
namespace mocks {

class NodeMock
{
public:
    typedef unsigned ElementType;

public:
    MOCK_CONST_METHOD0(count, size_t());
    MOCK_METHOD0(parent, NodeMock&());

    // Fake methods to which operators and friends forward calls
    MOCK_CONST_METHOD1(Equals, bool(const NodeMock&));
    MOCK_CONST_METHOD1(NotEquals, bool(const NodeMock&));
    MOCK_METHOD1(At, ElementType&(size_t));
    MOCK_METHOD0(NextNode, NodeMock&());
    MOCK_METHOD0(PreviousNode, NodeMock&());

    // now the things that can't be mocked
    bool operator==(const NodeMock& rhs) const
    {
        return Equals(rhs);
    }

    bool operator!=(const NodeMock& rhs) const
    {
        return NotEquals(rhs);
    }

    ElementType& operator[](size_t element)
    {
        return At(element);
    }

    friend NodeMock& nextNode(NodeMock& node)
    {
        return node.NextNode();
    }

    friend NodeMock& previousNode(NodeMock& node)
    {
        return node.PreviousNode();
    }
};

} // namespace mocks 
} // namespace geo

#endif
