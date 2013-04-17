#ifndef GEO_TREENODE_ITERATOR_HPP_
#define GEO_TREENODE_ITERATOR_HPP_

#include <iterator>
#include <type_traits>

namespace geo {

template <typename ObjectType, size_t totalLevels> class QuadNode;

template <typename Container>
class TreeNodeIterator
: public std::iterator<std::bidirectional_iterator_tag, Container >
{
private:
    typename iterator<bidirectional_iterator_tag, Container> IteratorType;

public:
    typedef bidirectional_iterator_tag iterator_category;
    typedef typename IteratorType::value_type value_type;
    typedef typename IteratorType::difference_type difference_type;
    typedef typename IteratorType::reference reference;
    typedef typename IteratorType::pointer pointer;

public:
    TreeNodeIterator() {}
    TreeNodeIterator
    explicit TreeNodeIterator(Container& node) : node(node) {}
    ~TreeNodeIterator() {}

private:
    Container& node;

};

} // namespace geo

#endif

