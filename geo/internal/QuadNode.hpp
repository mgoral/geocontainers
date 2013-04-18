#ifndef GEO_QUADNODE_HPP_
#define GEO_QUADNODE_HPP_

#include <array>
#include <vector>
#include <utility>

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

private:
    /**
     * Constructor with LocationCode set. User is not allowed to explicitly set node code. It's
     * calculated by node's parent instead.
     */
    QuadNode(size_t level, NodeCode&& nodeCode, QuadNode* nodeParent)
        : nodeLevel(level), nodeParent(nodeParent), nodeCode(nodeCode)
    {
        std::cout << "QuadNode with location code. level: " << nodeLevel << ", x: " << nodeCode.x.to_string() <<
            ", y: " << nodeCode.y.to_string() << "\n";
        childNodes.fill(nullptr);
    }

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
        bool childLocX = loc.x[totalLevels - nodeLevel - 1];
        bool childLocY = loc.y[totalLevels - nodeLevel - 1];
        return child(childLocX, childLocY);
    }

    QuadNode* child(bool locX, bool locY)
    {
        uint32_t childNo = locToInt(locX, locY);
        if (childNodes[childNo] == nullptr)
        {
            NodeCode newNodeCode(nodeCode);
            newNodeCode.x[totalLevels - nodeLevel - 1] = locX;
            newNodeCode.y[totalLevels - nodeLevel - 1] = locY;
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

    void insert(StoredObject&& object)
    {
        storage.push_back(std::move(object));
    }

    size_t level() const
    {
        return nodeLevel;
    }

    static uint32_t locToInt(bool locX, bool locY)
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

    }

private:
    size_t nodeLevel;
    Objects storage;

    const QuadNode* nodeParent;
    Nodes childNodes;
    NodeCode nodeCode;
};

/*
QuadNode* nextNode(QuadNode* node)
{
    if (node->hasChildren())
    {
        if (node->childExists(0, 0)) return child(QuadNode::locationToInt(0, 0));
        else if (node->childExists(0, 1)) return child(QuadNode::locationToInt(0, 1));
        else if (node->childExists(1, 0)) return child(QuadNode::locationToInt(1, 0));
        else return child(QuadNode::locationToInt(1, 1));
    }
    else
    {
        // No children and node is a root node.
        if (nullptr == node->parent())
            return nullptr;

        QuadNode* nodeParent;
        while (1)
        {
            nodeParent = node->parent();
            bool currNodeFound = false;
            for (uint32_t i = 0; i < 4; ++i)
            {
                bool x = (i & 0x10) >> 1;
                bool y = i & 0x01;
                if (nodeParent->childExists(x, y))
                {
                    if (nodeParent->child(x, y) == node)
                        currNodeFound = true;
                    else if (currNodeFound)
                        return nodeParent->child(x, y)
                }
            }
        }

        QuadNode* retNode = node->parent()
    }
}
*/

} // namespace geo

#endif
