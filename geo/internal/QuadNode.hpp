#ifndef GEO_QUADNODE_HPP_
#define GEO_QUADNODE_HPP_

#include <array>
#include <vector>
#include <utility>
#include <stdexcept>

#include "LocationCode.hpp"

namespace geo {

template <typename ObjectType, size_t totalLevels>
class QuadNode {
public:
    typedef LocationCode<totalLevels> NodeCode;

private:
    typedef ObjectWithLocationCode<ObjectType, totalLevels> StoredObject;
    typedef std::vector<StoredObject> Objects;
    typedef std::array<QuadNode<ObjectType, totalLevels>*, 4> Nodes;
    typedef QuadNode<ObjectType, totalLevels> QuadNodeT;

private:
    /**
     * Constructor with LocationCode set. User is not allowed to explicitly set node code. It's
     * calculated by node's parent instead.
     */
    QuadNode(size_t level, NodeCode&& nodeCode, QuadNode* nodeParent)
        : nodeLevel(level), nodeParent(nodeParent), nodeCode(nodeCode)
    {
        childNodes.fill(nullptr);
    }

public:
    /**
     * Default constructor. Always creates a root node.
     */
    QuadNode()
        : nodeLevel(totalLevels - 1), nodeParent(nullptr)
    {
        if (totalLevels < 1)
            throw std::invalid_argument("total levels number is less than 1");
        childNodes.fill(nullptr);
    }

    QuadNode(QuadNode&& that)
    {
        childNodes.fill(nullptr);
        nodeParent = nullptr;
        swap(*this, that);
    }

    QuadNode(const QuadNode& that)
        : nodeLevel(that.nodeLevel), storage(that.storage), nodeParent(that.nodeParent),
        nodeCode(that.nodeCode)
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
        std::swap(first.nodeCode, second.nodeCode);
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

    QuadNode* child(const NodeCode& loc)
    {
        // TODO: check if a given loc is valid from a current QuadNode POV, i.e. first
        // "currentlevelNo - 1" bits of (loc ^ nodeCode) are equal to 0.
        bool childLocX = loc.x[nodeLevel - 1];
        bool childLocY = loc.y[nodeLevel - 1];
        return child(childLocX, childLocY);
    }

    QuadNode* child(bool locX, bool locY)
    {
        if (0 == nodeLevel)
            return this;

        uint32_t childNo = locToInt(locX, locY);
        if (childNodes[childNo] == nullptr && nodeLevel > 0)
        {
            NodeCode newNodeCode(nodeCode);
            // Bitset position is counted from right (LSB), but LocationCode is checked from
            // left (MSB)
            newNodeCode.x[nodeLevel - 1] = locX;
            newNodeCode.y[nodeLevel - 1] = locY;
            childNodes[childNo] = new QuadNode(nodeLevel - 1, std::move(newNodeCode), this);
        }
        return childNodes[childNo];
    }

    bool childExists(bool locX, bool locY) const
    {
        return (childNodes[locToInt(locX, locY)] != nullptr);
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
    void erase(const NodeCode& loc)
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

    bool isChildOf(const QuadNode& node) const
    {
        if (level() < node.level())
        {
            for (size_t i = totalLevels - 1; i >= node.level(); --i)
            {
                if (nodeCode.x[i] != node.locationCode().x[i] ||
                    nodeCode.y[i] != node.locationCode().y[i])
                {
                    return false;
                }
            }
            return true;
        }
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

    const NodeCode& locationCode() const
    {
        return nodeCode;
    }

    static uint32_t locToInt(bool locX, bool locY)
    {
        return ((locX << 1) + locY);
    }

    QuadNode* parent()
    {
        return nodeParent;
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
    size_t nodeLevel;
    Objects storage;

    QuadNode* nodeParent;
    Nodes childNodes;
    NodeCode nodeCode;
};

template <typename T, size_t lev>
QuadNode<T, lev>* nextNode(QuadNode<T, lev>* node)
{
    if (node == nullptr)
        return nullptr;

    if (node->hasChildren())
    {
        if (node->childExists(0, 0)) return node->child(0, 0);
        else if (node->childExists(0, 1)) return node->child(0, 1);
        else if (node->childExists(1, 0)) return node->child(1, 0);
        else return node->child(1, 1);
    }
    else
    {
        // initial prepare for the first check of parent node
        bool x = node->locationCode().x[node->level()];
        bool y = node->locationCode().y[node->level()];
        node = node->parent();
        while (node != nullptr)
        {
            // evaluate node number in node->parent() child list and check only children with
            // higher index.
            for (uint32_t i = QuadNode<T, lev>::locToInt(x, y) + 1; i < 4; ++i)
            {
                bool cx = (i & 2) >> 1;
                bool cy = i & 1;
                if (node->childExists(cx, cy))
                {
                    return node->child(cx, cy);
                }
            }

            // prepare the next (parent) node to check
            x = node->locationCode().x[node->level()];
            y = node->locationCode().y[node->level()];
            node = node->parent();
        }

        return nullptr;
    }
}

template <typename T, size_t lev>
QuadNode<T, lev>* previousNode(QuadNode<T, lev>* node)
{
    if (node == nullptr || node->parent() == nullptr)
        return nullptr;

    bool x = node->locationCode().x[node->level()];
    bool y = node->locationCode().y[node->level()];
    node = node->parent();
    for (int i = QuadNode<T, lev>::locToInt(x, y) - 1; i >= 0; --i)
    {
        bool cx = (i & 2) >> 1;
        bool cy = i & 1;
        if (node->childExists(cx, cy))
        {
            node = node->child(cx, cy);
            while (node->hasChildren())
            {
                if (node->childExists(1, 1)) node = node->child(1, 1);
                else if (node->childExists(1, 0)) node = node->child(1, 0);
                else if (node->childExists(0, 1)) node = node->child(0, 1);
                else node = node->child(0, 0);
            }
            return node;
        }
    }
    return node;
}

} // namespace geo

#endif
