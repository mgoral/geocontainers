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
    LocationCode(const Coordinates& coord) :
        x((coord.x() + 0.5) * (2 << size)),
        y((coord.y() + 0.5) * (2 << size))
    { }

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
* QuadTree<ObjectType, size_t maxLevels=10> main class to construct a Quad Tree. Graphically it
* represents a two-dimension field with objects from the range of [x, y]. That field might be
* divided into 4 regions (tree nodes) which might contain a particular number of objects described
* by [x, y] * coordinates. If a new object is added to a region which already contains a maximum
* number of * objects (so its capacity is exceeded), that region is further divided into another 4
* subregions. Each object stored in a parent region is then added to appropriate subregion.
*
* This way the whole field is divided into smaller regions, each of them containing only adjacent
* objects. Thanks to this e.g. efficent collision detection algorithms might be performed.
*
* @param ObjectType Type of objects that will be stored inside QuadTree.
* @param maxLevels  Maximum number of tree levels (used for practical reasons). Must be higher than
*                   0 and smaller than 32.
*/
template <typename ObjectType, size_t maxLevels = 10>
class QuadTree
{
private:
    typedef ObjectWithLocationCode<ObjectType, maxLevels> StoredObject;
    typedef QuadNode<ObjectType, maxLevels> TreeNode;

public:
    explicit QuadTree(size_t width)
        : width(width), startX(0), startY(0), nodeCapacity(0),
        tr(startX, startY, width, width), root(maxLevels - 1)
    {
        checkRequirements();
    }

    QuadTree(size_t width, size_t capacity)
        : width(width), startX(0), startY(0), nodeCapacity(capacity),
        tr(startX, startY, width, width), root(maxLevels - 1)
    {
        checkRequirements();
    }

    QuadTree(size_t width, int startX, int startY)
        : width(width), startX(startX), startY(startY), nodeCapacity(0),
        tr(startX, startY, width, width), root(maxLevels - 1)
    {
        checkRequirements();
    }

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

    bool insert(double x, double y, const ObjectType& newObject)
    {
        if (x > startX + width || y > startY + width || x < startX || y < startY)
            return false;
        return insert(StoredObject(tr.forward(Coordinates(x, y)), newObject));
    }

    bool insert(double x, double y, ObjectType&& newObject)
    {
        if (x > startX + width || y > startY + width || x < startX || y < startY)
            return false;
        return insert(StoredObject(tr.forward(Coordinates(x, y)), std::move(newObject)));
    }

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

