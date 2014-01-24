#ifndef GEO_QUADNODE_HPP_
#define GEO_QUADNODE_HPP_

#include <array>
#include <vector>
#include <utility>
#include <stdexcept>

#include "LocationCode.hpp"

#include <iostream>

namespace geo {

template <typename ObjectType, size_t totalLevels>
class QuadNode {
private:
    typedef ObjectWithLocationCode<ObjectType, totalLevels> StoredObject;
    typedef std::vector<StoredObject> Objects;
    typedef std::array<QuadNode<ObjectType, totalLevels>*, 4> Nodes;
    typedef QuadNode<ObjectType, totalLevels> QuadNodeT;

public:
    typedef LocationCode<totalLevels> NodeCode;
    typedef ObjectType ElementType;
    typedef typename Objects::iterator iterator;
    typedef typename Objects::const_iterator const_iterator;

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
        : nodeLevel(totalLevels), nodeParent(nullptr)
    {
        if (totalLevels < 1)
            throw std::invalid_argument("total levels number is less than 1");
        childNodes.fill(nullptr);

        // Only the super-root (header) is created via default constructor. It's created for
        // bidirectional iteration purposes. Header is a node that is pointed by a tree end()
        // function. User must be able to perform `--end()` operation which should return a proper,
        // rightmost node.
        NodeCode newNodeCode(nodeCode);
        childNodes[0] = new QuadNode(totalLevels - 1, std::move(newNodeCode), this);
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
        using std::swap;
        swap(first.nodeLevel, second.nodeLevel);
        swap(first.nodeParent, second.nodeParent);
        first.storage.swap(second.storage);
        first.childNodes.swap(second.childNodes);
        swap(first.nodeCode, second.nodeCode);
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

    iterator begin()
    {
        return storage.begin();
    }

    const_iterator begin() const
    {
        return storage.begin();
    }

    // FIXME: can the following 2 functions be written without a redundant code?
    QuadNode& child(const NodeCode& loc)
    {
        // TODO: check if a given loc is valid from a current QuadNode POV, i.e. first
        // "currentlevelNo - 1" bits of (loc ^ nodeCode) are equal to 0.
        bool childLocX = loc.x[nodeLevel - 1];
        bool childLocY = loc.y[nodeLevel - 1];
        return child(childLocX, childLocY);
    }

    QuadNode& existingChild(const NodeCode& loc)
    {
        // TODO: check if a given loc is valid from a current QuadNode POV, i.e. first
        // "currentlevelNo - 1" bits of (loc ^ nodeCode) are equal to 0.
        bool childLocX = loc.x[nodeLevel - 1];
        bool childLocY = loc.y[nodeLevel - 1];
        return existingChild(childLocX, childLocY);
    }

    /**
     * Return a child with a given location. A new child is created if it doesn't exist.
     */
    QuadNode& child(bool locX, bool locY)
    {
        if (0 == nodeLevel)
            return *this;

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
        return *childNodes[childNo];
    }

    /**
     * Return a child with a given location. If a child doesn't exist, current node is returned
     * instead.
     */
    QuadNode& existingChild(bool locX, bool locY)
    {
        if (childExists(locX, locY) && nodeLevel > 0)
            return *childNodes[locToInt(locX, locY)];
        return *this;
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

    iterator end()
    {
        return storage.end();
    }

    const_iterator end() const
    {
        return storage.end();
    }

    void erase(const iterator& it)
    {
        storage.erase(it);
    }

    void erase(const iterator& itStart, const iterator& itEnd)
    {
        storage.erase(itStart, itEnd);
    }

    /**
     * Removes all elements that match a given LocationCode.
     */
    void erase(const NodeCode& loc)
    {
        for (iterator it = storage.begin(); it != storage.end();)
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

    // TODO: method should be const somehow
    QuadNode& leftMostNode()
    {
        QuadNodeT* retNode = this;
        while (retNode->hasChildren())
        {
            if (retNode->childNodes[0] != nullptr) retNode = retNode->childNodes[0];
            else if (retNode->childNodes[1] != nullptr) retNode = retNode->childNodes[1];
            else if (retNode->childNodes[2] != nullptr) retNode = retNode->childNodes[2];
            else retNode = retNode->childNodes[3];
        }
        return *retNode;
    }

    // TODO: method should be const somehow
    QuadNode& rightMostNode()
    {
        QuadNodeT* retNode = this;
        while (retNode->hasChildren())
        {
            if (retNode->childNodes[3] != nullptr) retNode = retNode->childNodes[3];
            else if (retNode->childNodes[2] != nullptr) retNode = retNode->childNodes[2];
            else if (retNode->childNodes[1] != nullptr) retNode = retNode->childNodes[1];
            else retNode = retNode->childNodes[0];
        }
        return *retNode;
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

    size_t insert(StoredObject&& object)
    {
        storage.push_back(std::move(object));
        return (count() - 1);
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

    QuadNode& parent()
    {
        if (nodeParent == nullptr)
            return *this;
        return *nodeParent;
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

    ElementType& operator[](size_t element)
    {
        return storage[element].object;
    }

    bool operator==(const QuadNodeT& rhs) const
    {
        return (nodeCode == rhs.nodeCode && nodeLevel == rhs.nodeLevel);
    }

    bool operator!=(const QuadNodeT& rhs) const
    {
        return !(operator==(rhs));
    }

private:
    size_t nodeLevel;
    Objects storage;

    QuadNode* nodeParent;
    Nodes childNodes;
    NodeCode nodeCode;
};

template <typename T, size_t lev>
QuadNode<T, lev>& nextNode(QuadNode<T, lev>& node)
{
    if (node.hasChildren())
    {
        if (node.childExists(0, 0)) return node.child(0, 0);
        else if (node.childExists(0, 1)) return node.child(0, 1);
        else if (node.childExists(1, 0)) return node.child(1, 0);
        else return node.child(1, 1);
    }
    else
    {
        QuadNode<T, lev>* refNode = &node;

        // initial prepare for the first check of parent node
        bool x = refNode->locationCode().x[refNode->level()];
        bool y = refNode->locationCode().y[refNode->level()];
        while (*refNode != refNode->parent())
        {
            refNode = &(refNode->parent());
            // evaluate node number in node->parent() child list and check only children with
            // higher index.
            for (uint32_t i = QuadNode<T, lev>::locToInt(x, y) + 1; i < 4; ++i)
            {
                bool cx = (i & 2) >> 1;
                bool cy = i & 1;
                if (refNode->childExists(cx, cy))
                {
                    return refNode->child(cx, cy);
                }
            }

            // prepare the next (parent) node to check
            x = refNode->locationCode().x[refNode->level()];
            y = refNode->locationCode().y[refNode->level()];
        }

        // At this point refNode == refNode.parent(), so it's a header. We'll return it as 
        // nextNode() of the last (rightmost) node in a tree.
        return *refNode;
    }
}

template <typename T, size_t lev>
QuadNode<T, lev>& previousNode(QuadNode<T, lev>& node)
{
    // If header node is given, then its previousNode is the rightmost one.
    // requirement: --end()
    if (node.parent() == node)
        return node.rightMostNode();

    QuadNode<T, lev>* refNode = &node;

    bool x = refNode->locationCode().x[refNode->level()];
    bool y = refNode->locationCode().y[refNode->level()];
    refNode = &(refNode->parent());
    for (int i = QuadNode<T, lev>::locToInt(x, y) - 1; i >= 0; --i)
    {
        bool cx = (i & 2) >> 1;
        bool cy = i & 1;
        if (refNode->childExists(cx, cy))
        {
            refNode = &(refNode->child(cx, cy));
            while (refNode->hasChildren())
            {
                if (refNode->childExists(1, 1)) refNode = &(refNode->child(1, 1));
                else if (refNode->childExists(1, 0)) refNode = &(refNode->child(1, 0));
                else if (refNode->childExists(0, 1)) refNode = &(refNode->child(0, 1));
                else refNode = &(refNode->child(0, 0));
            }
            return *refNode;
        }
    }
    std::cout << std::boolalpha << "header: " << (*refNode == refNode->parent()) << std::endl;
    return *refNode;
}

} // namespace geo

#endif
