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
    typedef TreeNodeIterator<TreeNode> TreeNodeIteratorT;

public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename IteratorType::value_type value_type;
    typedef typename IteratorType::difference_type difference_type;
    typedef typename IteratorType::reference reference;
    typedef typename IteratorType::pointer pointer;

public:
    TreeNodeIterator() : node(nullptr), pos(0) {}
    explicit TreeNodeIterator(TreeNode* node, size_t pos) : node(node), pos(pos) {}
    TreeNodeIterator(const TreeNodeIterator& that) : node(that.node), pos(that.pos) {}
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
        return (node == rhs && pos == rhs.pos);
    }

    bool operator!=(const TreeNodeIterator& rhs)
    {
        return (node != rhs || pos != rhs.pos);
    }

    typename TreeNode::ElementType& operator*() const
    {
        return (*node)[pos];
    }

    typename TreeNode::ElementType* operator->() const
    {
        return &(operator*());
    }

    TreeNodeIteratorT& operator++()
    {
        ++pos;
        while (node != nullptr && pos >= node->count())
        {
            node = nextNode(node);
            pos = 0;
        }
        return *this;
    }

    TreeNodeIteratorT operator++(int)
    {
        TreeNodeIteratorT ret(*this);
        operator++();
        return ret;
    }

    TreeNodeIteratorT& operator--()
    {
        if (pos == 0)
        {
            while (node != nullptr && node->count() > 0)
            {
                node = previousNode(node);
                pos = node->count() - 1;
            }
        }
        else
        {
            --pos;
        }
        return *this;
    }

    TreeNodeIteratorT operator--(int)
    {
        TreeNodeIteratorT ret(*this);
        operator--();
        return ret;
    }

    friend void swap(TreeNodeIterator& first, TreeNodeIterator& second)
    {
        std::swap(first.node, second.node);
        std::swap(first.pos, second.pos);
    }

private:
    TreeNode* node;
    size_t pos;

};

} // namespace geo

#endif

