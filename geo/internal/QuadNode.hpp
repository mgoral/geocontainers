#ifndef GEO_QUADNODE_HPP_
#define GEO_QUADNODE_HPP_

#include <array>
#include <vector>
#include <utility>

#include "LocationCode.hpp"

namespace geo {

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

    QuadNode(QuadNode&& that)
    {
        childNodes.fill(nullptr);
        nodeParent = nullptr;
        swap(*this, that);
    }

    QuadNode(const QuadNode& that)
        : nodeLevel(that.nodeLevel), storage(that.storage), nodeParent(that.nodeParent)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (that.childNodes[i] != nullptr)
                childNodes[i] = new QuadNode(*that.childNodes[i]);
            else
                childNodes[i] = nullptr;
        }
    }

    QuadNode& operator=(QuadNode rhs)
    {
        swap(*this, rhs);
        return *this;
    }

    friend void swap(QuadNode& first, QuadNode& second)
    {
        std::swap(first.nodeLevel, second.nodeLevel);
        std::swap(first.nodeParent, second.nodeParent);
        first.storage.swap(second.storage);
        first.childNodes.swap(second.childNodes);
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
        return child(locationToInt(locX, locY));
    }

    bool childExists(bool locX, bool locY) const
    {
        return (childNodes[locationToInt(locX, locY)] != nullptr);
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

    /**
     * Removes all elements that match a given LocationCode.
     */
    void erase(const LocationCode<totalLevels>& loc)
    {
        typename Objects::iterator it;
        for (it = storage.begin(); it != storage.end();)
        {
            if (loc == (it->location))
                it = storage.erase(it);
            else
                ++it;
        }
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

    static uint32_t locationToInt(bool locX, bool locY)
    {
        return ((locX << 1) + locY);
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


} // namespace geo

#endif
