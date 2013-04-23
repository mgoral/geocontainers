#ifndef GEO_TREENODE_ITERATOR_HPP_
#define GEO_TREENODE_ITERATOR_HPP_

#include <iterator>
#include <type_traits>

namespace geo {

template <typename ObjectType, size_t totalLevels> class QuadNode;

template <typename TreeNode>
class TreeNodeIterator
: public std::iterator<std::bidirectional_iterator_tag, TreeNode >
{
private:
    typedef std::iterator<std::bidirectional_iterator_tag, TreeNode> IteratorType;
    typedef typename TreeNode::iterator NodeIterator;

public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename IteratorType::value_type value_type;
    typedef typename IteratorType::difference_type difference_type;
    typedef typename IteratorType::reference reference;
    typedef typename IteratorType::pointer pointer;

public:
    TreeNodeIterator() : node(nullptr), nodeIt(0) {}
    explicit TreeNodeIterator(TreeNode* node, NodeIterator it) : node(node), nodeIt(it) {}
    TreeNodeIterator(const TreeNodeIterator& that) : node(that.node), nodeIt(that.nodeIt) {}
    ~TreeNodeIterator() {}

    TreeNodeIterator& operator=(TreeNodeIterator rhs)
    {
        swap(*this, rhs);
        return *this;
    }

    operator bool() const
    {
        return (node != nullptr);
    }

    bool operator==(const TreeNodeIterator& rhs)
    {
        return (nodeIt == rhs.nodeIt);
    }

    bool operator!=(const TreeNodeIterator& rhs)
    {
        return (nodeIt != rhs.nodeIt);
    }

    typename NodeIterator::reference operator*() const
    {
        return *nodeIt;
    }

    // TODO this somehow doesn't return reference to vector iterator
    typename NodeIterator::pointer operator->() const
    {
        return &(operator*());
    }

    NodeIterator operator++()
    {
        ++nodeIt;
        while (node != nullptr && nodeIt == node->end())
        {
            node = nextNode(node);
            nodeIt = node->begin();
        }
        return nodeIt;
    }

    friend void swap(TreeNodeIterator& first, TreeNodeIterator& second)
    {
        std::swap(first.node, second.node);
        std::swap(first.nodeIt, second.nodeIt);
    }

private:
    TreeNode* node;
    NodeIterator nodeIt;

};

} // namespace geo

#endif

