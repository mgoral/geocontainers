#ifndef GEO_QUADTREE_HPP_
#define GEO_QUADTREE_HPP_

#include <stdexcept>
#include <vector>
#include <array>
#include <bitset>
#include <utility>
#include <iterator>
#include <type_traits>

#include "Coordinates.hpp"

namespace geo {

template <size_t size>
struct LocationCode
{
    explicit LocationCode(const Coordinates& coord) :
        x(coord.x() * (2 << (size - 1))),
        y(coord.y() * (2 << (size - 1)))
    { std::cout << (unsigned long)(coord.x() * (2 << (size - 1))) << " x " << (unsigned long)(coord.y() * (2 << (size - 1))) << "\n"; }

    std::bitset<size> x;
    std::bitset<size> y;
};

template <typename ObjectType, size_t locCodeMaxSize>
struct ObjectWithLocationCode {
    LocationCode<locCodeMaxSize> location;
    ObjectType object;

    ObjectWithLocationCode(const LocationCode<locCodeMaxSize>& location, const ObjectType& object)
        : location(location), object(object) { }

    ObjectWithLocationCode(LocationCode<locCodeMaxSize>&& coord, ObjectType&& object)
        : location(std::move(location)), object(object) { }
};

/*
template <typename ObjectType>
class QuadNodeIterator
: public std::iterator<std::bidirectional_iterator_tag, QuadNode<ObjectType> >
{
public:
    explicit QuadNodeIterator(ObjectType& object) :

private:
    Objects::

};
*/

template <typename ObjectType, size_t totalLevels>
class QuadNode {
private:
typedef ObjectWithLocationCode<ObjectType, totalLevels> StoredObject;
typedef std::vector<StoredObject> Objects;
typedef std::array<QuadNode<ObjectType, totalLevels>*, 4> Nodes;

public:
QuadNode(size_t level, QuadNode* nodeParent = nullptr)
    : nodeLevel(level), nodeParent(nodeParent)
{
    childNodes.fill(nullptr);
}

~QuadNode()
{
    delete childNodes[0];
    childNodes[0] = nullptr;
    delete childNodes[1];
    childNodes[1] = nullptr;
    delete childNodes[2];
    childNodes[2] = nullptr;
    delete childNodes[3];
    childNodes[3] = nullptr;
}

QuadNode* child(const LocationCode<totalLevels>& loc)
{
    bool childLocX = loc.x[totalLevels - nodeLevel - 1];
    bool childLocY = loc.y[totalLevels - nodeLevel - 1];
    return child(childLocX, childLocY);
}

QuadNode* child(bool locX, bool locY)
{
    // children are located in an 'U-shape':
    // | 0 3 |
    // | 1 2 |
    if (!locX)
    {
        if (!locY)
            return child(0); // x = 0, y = 0
        return child(1); // x = 0, y = 1
    }
    else
    {
        if (!locY)
            return child(4); // x = 1, y = 0
        return child(3); // x = 1, y = 1
    }
}

void clear()
{
    storage.clear();
}

/**
    * Gives a number of objects stored in a current node.
    */
size_t count() const
{
    return storage.size();
}

bool hasChildren() const
{
    if (childNodes[0] != nullptr || childNodes[1] != nullptr ||
        childNodes[2] != nullptr || childNodes[3] != nullptr)
        return true;
    return false;
}

void insert(StoredObject&& object)
{
    storage.push_back(std::move(object));
}

size_t level() const
{
    return nodeLevel;
}

const QuadNode* parent() const
{
    return parent;
}

/**
    * Returns a number of objects stored in a current node and all subnodes.
    */
size_t totalCount() const
{
    size_t tc = storage.size();
    if (hasChildren())
    {
        for (int i = 0; i < 4; ++i)
        {
            if (childNodes[i] != nullptr)
                tc += childNodes[i]->totalCount();
        }
    }
    return tc;
}

/*
ObjectType& operator[](size_t element)
{
    return storage[element].object;
}
*/

StoredObject& operator[](size_t element)
{
    return storage[element];
}

private:
QuadNode* child(uint32_t childNo)
{
    if (childNodes[childNo] == nullptr)
        childNodes[childNo] = new QuadNode(nodeLevel - 1, this);
    return childNodes[childNo];
}

private:
size_t nodeLevel;
Objects storage;

const QuadNode* nodeParent;
Nodes childNodes;
};


/**
* QuadTree<ElementType, size_t maxLevels=10> main class to construct a Quad Tree. Graphically it
* represents a two-dimension field with elements from the range of [x, y]. That field might be
* divided into 4 regions (tree nodes) which might contain a particular number of elements described
* by [x, y] coordinates. If a new element is added to a region which already contains a maximum
* number of elements (so its capacity is exceeded), that region is further divided into another 4
* subregions. Each element stored in a parent region is then added to appropriate subregion.
*
* This way the whole field is divided into smaller regions, each of them containing only adjacent
* elements. Thanks to this e.g. efficent collision detection algorithms might be performed.
*
* @param ElementType Type of elements that will be stored inside QuadTree.
* @param maxLevels   Maximum number of tree levels (used for practical reasons). Must be higher than
*                    0 and smaller than 32.
*/
template <typename ElementType, size_t maxLevels = 10>
class QuadTree
{
private:
    typedef ObjectWithLocationCode<ElementType, maxLevels> StoredObject;
    typedef QuadNode<ElementType, maxLevels> TreeNode;

public:
    /**
     * QuadTree Constructor.
     * Parameters startX, startY and nodeCapacity are set to 0.
     *
     *  @see QuadTree(size_t width, int startX, int startY)
     */
    explicit QuadTree(size_t width)
        : width(width), startX(0), startY(0), nodeCapacity(0),
        tr(startX, startY, width, width), root(maxLevels - 1)
    {
        checkRequirements();
    }

    /**
     * QuadTree Constructor
     * Parameters startX and startY are set to 0.
     *
     *  @see QuadTree(size_t width, int startX, int startY)
     */
    QuadTree(size_t width, size_t capacity)
        : width(width), startX(0), startY(0), nodeCapacity(capacity),
        tr(startX, startY, width, width), root(maxLevels - 1)
    {
        checkRequirements();
    }

    /**
     * QuadTree Constructor
     * Parameter capacity is set to 0.
     *
     *  @see QuadTree(size_t width, int startX, int startY)
     */
    QuadTree(size_t width, int startX, int startY)
        : width(width), startX(startX), startY(startY), nodeCapacity(0),
        tr(startX, startY, width, width), root(maxLevels - 1)
    {
        checkRequirements();
    }

    /**
     * QuadTree Constructor.
     * Parameters startX, startY and nodeCapacity are set to 0.
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
        tr(startX, startY, width, width), root(maxLevels - 1)
    {
        checkRequirements();
    }

    ~QuadTree() {}

    // QuadTree is non-copyable
    QuadTree & operator=(const QuadTree&) = delete;
    QuadTree(const QuadTree&) = delete;

    /**
     * Insert a single element into Quadtree at given coordinates. Range check of coordinates is
     * performed. They should be in range: [startX, startX + width] x [startY, startY + width]
     * to be inserted. Otherwise an element is not inserted.
     *
     * There are two overloaded versions of insert provided: one where given element is copied and
     * one where it's moved.
     *
     * @param  x   X-axis coordinate of val.
     * @param  y   Y-axis coordinate of val.
     * @param  val Element to be added.
     * @raturn     True if a new element has been added properly and false if not.
     */
    bool insert(double x, double y, const ElementType& val)
    {
        if (x > startX + width || y > startY + width || x < startX || y < startY)
            return false;
        return insert(StoredObject(LocationCode<maxLevels>(tr.forward(Coordinates(x, y))), val));
    }

    /**
     * @see insert(double x, double y, const ElementType& newObject)
     */
    bool insert(double x, double y, ElementType&& val)
    {
        if (x > startX + width || y > startY + width || x < startX || y < startY)
            return false;
        return insert(StoredObject(LocationCode<maxLevels>(tr.forward(Coordinates(x, y))), std::move(val)));
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

    TreeNode* getNode(const LocationCode<maxLevels>& code)
    {
        int level = maxLevels;
        TreeNode* node = &root;

        do
        {
            if (!node->hasChildren())
                break;
            node = node->child(code);
        } while (--level);
        return node;
    }

    bool insert(StoredObject&& toStore)
    {
        TreeNode* node = getNode(toStore.location);
        if (0 < node->level())
        {
            if (node->count() == nodeCapacity)
            {
                for (size_t i = 0; i < node->count(); ++i)
                {
                    StoredObject moved = (*node)[i];
                    node->child(moved.location)->insert(std::move(moved));
                }
                node->child(toStore.location)->insert(std::move(toStore));
                return true;
            }
        }
        node->insert(std::move(toStore));
        return true;
    }

private:
    size_t width;
    size_t startX;
    size_t startY;
    size_t nodeCapacity;

    AffineCoordTr<0, 0, 1, 1> tr;
    TreeNode root;
};

} // namespace geo

#endif

