#ifndef GEO_QUADTREE_HPP_
#define GEO_QUADTREE_HPP_

#include <stdexcept>
#include <utility>
#include <iterator>
#include <type_traits>
#include <cstring>

#include "internal/QuadNode.hpp"
#include "internal/Coordinates.hpp"
#include "internal/LocationCode.hpp"
#include "internal/TreeNodeIterator.hpp"

namespace geo {

/**
* Main class to construct a Quad Tree.
*
* Graphically it represents a two-dimension field with elements from the range of [x, y). That field
* might be divided into 4 regions (tree nodes) which might contain a particular number of elements
* described by [x, y] coordinates. If a new element is added to a region which already contains a
* maximum number of elements (so its capacity is exceeded), that region is further divided into
* another 4 subregions. Each element stored in a parent region is then added to appropriate
* subregion.
*
* This way the whole field is divided into smaller regions, each of them containing only adjacent
* elements. Thanks to this e.g. efficent collision detection algorithms might be performed.
*
* @param ElementType Type of elements that will be stored inside QuadTree.
* @param maxLevels   Maximum number of tree levels (used for practical reasons). Must be higher than
*                    0 and smaller than 32. Default is 10.
*/
template <typename ElementType, size_t maxLevels = 10>
class QuadTree
{
private:
    typedef ObjectWithLocationCode<ElementType, maxLevels> StoredObject;
    typedef QuadNode<ElementType, maxLevels> TreeNode;

public:
    typedef TreeNodeIterator<TreeNode> iterator;

public:
    /**
     * QuadTree Constructor.
     * Parameters startX, startY and nodeCapacity are set to 0.
     *
     *  @see QuadTree(size_t width, int startX, int startY, capacity)
     */
    explicit QuadTree(size_t width)
        : width(width), startX(0), startY(0), nodeCapacity(0),
        tr(startX, startY, width, width)
    {
        checkRequirements();
    }

    /**
     * QuadTree Constructor
     * Parameters startX and startY are set to 0.
     *
     *  @see QuadTree(size_t width, int startX, int startY, capacity)
     */
    QuadTree(size_t width, size_t capacity)
        : width(width), startX(0), startY(0), nodeCapacity(capacity),
        tr(startX, startY, width, width)
    {
        checkRequirements();
    }

    /**
     * QuadTree Constructor
     * Parameter capacity is set to 0.
     *
     *  @see QuadTree(size_t width, int startX, int startY, capacity)
     */
    QuadTree(size_t width, int startX, int startY)
        : width(width), startX(startX), startY(startY), nodeCapacity(0),
        tr(startX, startY, width, width)
    {
        checkRequirements();
    }

    /**
     * QuadTree Constructor.
     *
     * @param width    Width of a field that might be represented in a QuadTree. Also specifies a
     *                 height because field must be rectangular. Width must be a power of 2.
     * @param startX   Starting point of represented field in x-axis.
     * @param startY   Starting point of represented field in y-axis.
     * @param capacity Maximum capacity of a single tree node. Specifies how much elements (at most)
     *                 can be stored in one node. An exception are nodes at the maximum level of the
     *                 tree which shall store all remaining elements inserted into QuadTree (because
     *                 QuadTree doesn't have a limit to maximum number of stored elements).
     */
    QuadTree(size_t width, int startX, int startY, size_t capacity)
        : width(width), startX(startX), startY(startY), nodeCapacity(capacity),
        tr(startX, startY, width, width)
    {
        checkRequirements();
    }

    ~QuadTree() {}

    iterator begin()
    {
        if (root.child(0, 0).hasChildren() || root.child(0, 0).count() > 0)
            return iterator(&(root.leftMostNode()), 0);
        return end();
    }

    iterator end()
    {
        return iterator(&root, 0);
    }

    /**
     * Clear the tree, removing and destroying all elements stored inside the QuadTree container.
     */
    void clear()
    {
        root = TreeNode();
    }

    /**
     * Removes from the QuadTree container all elements that match given coordinates. They are then
     * destroyed.
     *
     * @param x X-axis coordinate of the element to be removed.
     * @param y Y-axis coordinate of the element to be removed.
     */
    void erase(double x, double y)
    {
        // TODO: relocate elements to node->parent() if node and its siblings count is lower or equal than capacity.
        if (coordinatesAreOk(x, y))
        {
            LocationCode<maxLevels> code(tr.forward(Coordinates(x, y)));
            TreeNode* node = getNode(code);
            node->erase(code);
        }
    }

    /**
     * Insert a single element into Quadtree at given coordinates. Range check of coordinates is
     * performed. They should be in range: [startX, startX + width) x [startY, startY + width)
     * to be inserted. Otherwise an element is not inserted.
     *
     * There are two overloaded versions of insert provided: one where given element is copied and
     * one where it's moved.
     *
     * @param  x   X-axis coordinate of val.
     * @param  y   Y-axis coordinate of val.
     * @param  val Element to be added.
     * @raturn     Bidirectional iterator pointing to the new element location.
     */
    iterator insert(double x, double y, const ElementType& val)
    {
        if (coordinatesAreOk(x, y))
        {
            return insert(StoredObject(
                LocationCode<maxLevels>(tr.forward(Coordinates(x, y))), val));
        }
        return iterator();
    }

    /**
     * @see insert(double x, double y, const ElementType& newObject)
     */
    iterator insert(double x, double y, ElementType&& val)
    {
        if (coordinatesAreOk(x, y))
        {
            return insert(StoredObject(
                LocationCode<maxLevels>(tr.forward(Coordinates(x, y))), std::move(val)));
        }
        return iterator();
    }

    // TODO: Implement const version of near(). This requires const_iterator, const getExistingNode
    // implementation and const QuadNode::existingChild implementation. Also, getExistingNode must
    // not call child(), so in near() const check if there is root.child(0,0) and if not, return
    // empty iterators.

    /**
     * Return the bounds of a range that includes all the elements that are near specified (x, y).
     *
     * @param  x X-axis coordinate of val.
     * @param  y Y-axis coordinate of val.
     * @return   Pair of iterators that point to the first and the last of the elements that are
     *           near the (x, y) point. If any of given x or y is outside of a QuadTree range (i.e.
     *           x >= startX + width or y >= startY + width), pair of end() is returned.
     */
    std::pair<iterator, iterator> near(double x, double y)
    {
        if (coordinatesAreOk(x, y))
        {
            TreeNode* node = getExistingNode(
                LocationCode<maxLevels>(tr.forward(Coordinates(x, y))));
            return std::pair<iterator, iterator>(
                iterator(node, 0), ++iterator(node, node->count()));
        }
        return std::pair<iterator, iterator>(end(), end());
    }

    /**
     * @return Total number of elements in QuadTree.
     */
    size_t size() const
    {
        return root.totalCount();
    }

private:
    void checkRequirements()
    {
        if (maxLevels < 1)
            throw std::invalid_argument("maximum levels number is less than 1");
        if (maxLevels > 32)
            throw std::invalid_argument("maximum levels number is too big");
        if (width < 1)
            throw std::invalid_argument("size is less than 1");
        if (((width - 1) & width) != 0)
            throw std::invalid_argument("size is not power of 2");
    }

    bool coordinatesAreOk(double x, double y) const
    {
        if (x >= startX + width || y >= startY + width || x < startX || y < startY)
            return false;
        return true;
    }

    TreeNode* getExistingNode(const LocationCode<maxLevels>& code)
    {
        int level = maxLevels;

        // FIXME: it's really important to start from root.child (as root is a header) and ALL TESTS
        // PASS WHEN IT'S CHANGED TO: `node = &root;`
        TreeNode* node = &(root.child(0, 0));

        do
        {
            if (!node->hasChildren() || node == &(node->existingChild(code)))
                break;
            node = &(node->existingChild(code));
        } while (--level);
        return node;
    }

    TreeNode* getNode(const LocationCode<maxLevels>& code)
    {
        int level = maxLevels;

        // FIXME: it's really important to start from root.child (as root is a header) and ALL TESTS
        // PASS WHEN IT'S CHANGED TO: `node = &root;`
        TreeNode* node = &(root.child(0, 0));

        do
        {
            if (!node->hasChildren() || node == &(node->child(code)))
                break;
            node = &(node->child(code));
        } while (--level);
        return node;
    }

    iterator insert(StoredObject&& toStore)
    {
        TreeNode* node = getNode(toStore.location);
        if (node->level() > 0)
        {
            // We store one element at time so there will be a moment before node overflow when its
            // count will be equal capacity. Then we'll relocate all its elements to the new child
            // nodes. At worst scenario, all elements will be relocated to the same node, so its
            // count() will be again equal to capacity. The loop ends when at least one element is
            // relocated to the another child node.
            while (node->count() == nodeCapacity && node->level() > 0)
            {
                typename TreeNode::iterator it;
                typename TreeNode::iterator itEnd = node->end();
                for (it = node->begin(); it != itEnd; ++it)
                {
                    node->child(it->location).insert(std::move(*it));
                }
                node->clear();
                node = &(node->child(toStore.location));
            }
        }
        return iterator(node, node->insert(std::move(toStore)));
    }

private:
    size_t width;
    size_t startX;
    size_t startY;
    size_t nodeCapacity;

    CoordTr<0, 0, 1, 1> tr;
    TreeNode header;
    TreeNode root;
};

} // namespace geo

#endif

