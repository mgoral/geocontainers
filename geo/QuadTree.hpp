#ifndef GEO_QUADTREE_HPP_
#define GEO_QUADTREE_HPP_

#include <stdexcept>
#include <vector>
#include <array>
#include <utility>

namespace geo {

template <typename ObjectType>
struct ObjectWithCoordinates {
    int x;
    int y;
    ObjectType object;

    ObjectWithCoordinates(int x, int y, const ObjectType& object) : x(x), y(y), object(object) {}
    ObjectWithCoordinates(int x, int y, ObjectType&& object) : x(x), y(y), object(std::move(object)) {
        object = std::move(object);
    }
};

template <typename ObjectType>
class QuadNode {
private:
    typedef ObjectWithCoordinates<ObjectType> StoredObject;
    typedef std::vector<StoredObject> Objects;
    typedef std::array<QuadNode<ObjectType>*, 4> Nodes;

public:
    QuadNode(int startX, int startY, int width, size_t capacity, QuadNode* nodeParent = nullptr)
        : startX(startX), startY(startY), width(width), capacity(capacity)
    {
        if (width < 1)
            throw std::invalid_argument("size is less than 1");
        if (((width - 1) & width) != 0)
            throw std::invalid_argument("size is not power of 2");

        childNodes.fill(nullptr);
        parent = nodeParent;
    }

    ~QuadNode()
    {
        delete childNodes[0];
        delete childNodes[1];
        delete childNodes[2];
        delete childNodes[3];
    }

    bool insert(StoredObject&& object)
    {
        // True should be always returned from inside this if.
        if (object.x >= startX && object.x < startX + width &&
            object.y >= startY && object.y < startY + width)
        {
            // Final node, no further divisions are possible.
            if (1 == width)
            {
                objects.push_back(std::move(object));
                return true;
            }
            else if (hasChildren())
            {
                return insertToProperNode(std::move(object));
            }


            // Inserting a new object would increase stored objects count over its max capacity, so
            // we have to reallocate them into new subnodes.
            if (objects.size() == capacity)
            {
                int newWidth = width/2;
                childNodes[0] = new QuadNode(
                    startX, startY, newWidth, capacity, this);
                childNodes[1] = new QuadNode(
                    startX, startY + newWidth, newWidth, capacity, this);
                childNodes[2] = new QuadNode(
                    startX + newWidth, startY + newWidth, newWidth, capacity, this);
                childNodes[3] = new QuadNode(
                    startX + newWidth, startY, newWidth, capacity, this);

                typename Objects::iterator it;
                for (it = objects.begin(); it != objects.end(); ++it)
                {
                    insertToProperNode(std::move(*it));
                }
                objects.clear();

                return insertToProperNode(std::move(object));
            }

            objects.push_back(std::move(object));
            return true;
        }

        return false;
    }

    /**
     * Gives a number of objects stored in a current node.
     */
    size_t count() const
    {
        return objects.size();
    }

    /**
     * Returns a number of objects stored in a current node and all subnodes.
     */
    size_t totalCount() const
    {
        size_t tc = objects.size();
        if (hasChildren())
        {
            for (int i = 0; i < 4; ++i)
                tc += childNodes[i]->totalCount();
        }
        return tc;
    }

private:
    bool hasChildren() const
    {
        return childNodes[0] != nullptr;
    }

    bool insertToProperNode(StoredObject&& object)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (childNodes[i]->insert(std::move(object)))
                return true;
        }
        return false;
    }

private:
    int startX;
    int startY;
    const int width;
    size_t capacity;
    Objects objects;

    QuadNode* parent;
    Nodes childNodes;
};


/**
 * QuadTree<ObjectType, width> - main class to construct a Quad Tree. Graphically it represents
 * a two-dimension field with objects from the range of [x, y). That field might be divided into 4
 * regions (tree nodes) which might contain a particular number of objects described by (x, y)
 * coordinates. If a new object is added to a region which already contains a maximum number of
 * objects (so its capacity is exceeded), that region is further divided into another 4 subregions.
 * Each object stored in a parent region is then added to appropriate subregion.
 *
 * This way the whole field is divided into smaller regions, each of them containing only adjacent
 * objects. Thanks to this e.g. efficent collision detection algorithms might be performed.
 *
 * @param ObjectType Type of objects that will be stored inside QuadTree.
 * @param width Absolute total size of a field. Must be higher than 0 and a power of 2.
 */
template <typename ObjectType, int width>
class QuadTree {
private:
    typedef ObjectWithCoordinates<ObjectType> StoredObject;

public:
    QuadTree(int startX = 0, int startY = 0, size_t capacity = 0)
    : root(startX, startY, width, capacity) {}
    ~QuadTree() {}

    bool insert(int x, int y, const ObjectType& newObject)
    {
        return root.insert(StoredObject(x, y, newObject));
    }

    bool insert(int x, int y, ObjectType&& newObject)
    {
        return root.insert(StoredObject(x, y, std::move(newObject)));
    }

    size_t size() const
    {
        return root.totalCount();
    }

private:
    QuadNode<ObjectType> root;
};

} // namespace geo

#endif

