/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//#define DEBUG_TREES 1

//Expects data to be pointer.
//On destruction it will delete the contained data

#pragma region XCTreeNode

template<class T>
class XCTreeNode
{
public:

    XCTreeNode()
    {}

    inline bool operator ==(const T& otherObj)
    {
        return Get() == otherObj->Get();
    }

    inline bool operator !=(const T& otherObj)
    {
        return Get() != otherObj->Get();
    }

    const T                          Get() const         { return (const T)(this); }
    T                                GetMutable()        { return static_cast<T>(this); }
    const std::vector<T>&            GetNodes() const    { return m_childNodes; }
    std::vector<T>&                  GetNodesMutable()   { return m_childNodes; }

    bool                             HasChildNodes()     { m_childNodes.size() > 0; }
    void                             Print();

protected:

    //Protect this dtor and call delete on derived ones.
    ~XCTreeNode()
    {
        //Delete all child nodes
        while (m_childNodes.size() > 0)
        {
            XCDELETE(m_childNodes.back());
            m_childNodes.pop_back();
        }

#if defined(DEBUG_TREES)
        ::Logger("Node destructed");
#endif
    }

protected:
    std::vector<T>                   m_childNodes;
};

template<class T>
void XCTreeNode<T>::Print()
{
#if defined(DEBUG_TREES)
    ::Logger("Object data: %d", *m_data);
#endif

    for (auto& childNode : m_childNodes)
    {
        childNode->Print();
    }
}

#pragma endregion

#pragma region XCTreeNodeSearch

//Search modes & traversals
class DepthFirstSearch
{
public:
    //TODO : Create DFS iterators maintaining info on levels.
    template<class T>
    static T GetParentNode(T& parentNode, const T& childNode);
};

template<class T>
T DepthFirstSearch::GetParentNode(T& parentNode, const T& childNode)
{
    T outNode = nullptr;
    auto& childNodesList = parentNode.GetNodesMutable();

    for (auto& node : childNodesList)
    {
        outNode = GetParentNode(*node, childNode);
        if (outNode != nullptr)
        {
            break;
        }
    }

    for (auto& node : childNodesList)
    {
        if (*node == childNode)
        {
            outNode = &parentNode;
            break;
        }
    }

    return outNode;
}

class BreathFirstSearch
{
public:
    //TODO : Create BFS iterators maintaining info on levels.
    template<class T>
    static T GetParentNode(T& parentNode, const T& childNode);
};

template<class T>
T BreathFirstSearch::GetParentNode(T& parentNode, const T& childNode)
{
    T outNode = nullptr;
    auto& childNodesList = parentNode->GetNodesMutable();

    //First check within this list
    for (auto& node : childNodesList)
    {
        if (*node == childNode)
        {
            outNode = parentNode;
            break;
        }
    }

    if (outNode == nullptr)
    {
        //Ask the child nodes
        for (auto& node : childNodesList)
        {
            outNode = GetParentNode(node, childNode);
            if (outNode != nullptr)
            {
                break;
            }
        }
    }

    return outNode;
}

#pragma endregion

#pragma region XCTreeNodeModifier

template<class T, class TreeStrategy, class SearchStartegy>
class XCTreeNodeModifier : public XCTreeNode<T>
{
public:
    XCTreeNodeModifier()
    {}

    //Adds the node to its child list.
    inline T& AddNode(const T& data)
    {
        (static_cast<TreeStrategy*>(this))->AddNode(data);
    }

    //Removes a child node. Does not traverse. Use GetParentNode to find the parent.
    inline void RemoveNode(const T& node)
    {
        (static_cast<TreeStrategy*>(this))->RemoveNode(data);
    }

    inline T GetParentNode(const T& childNode)
    {
        T currentNode = GetMutable();
        return SearchStartegy::GetParentNode<T>(currentNode, childNode);
    }
};

#pragma endregion

#pragma region NTreeNode

template<class T, class SearchStrategy = BreathFirstSearch>
class NTreeNode : public XCTreeNodeModifier<T, NTreeNode<T>, SearchStrategy>
{
public:
    NTreeNode()
    {}

    //Adds the node to its child list.
    T&                   AddNode(const T& data);

    //Removes a child node. Does not traverse. Use GetParentNode to find the parent.
    void                 RemoveNode(const T& node);
};

template<class T, class SearchStrategy>
T& NTreeNode<T, SearchStrategy>::AddNode(const T& data)
{
    m_childNodes.push_back(data);

    return m_childNodes.back();
}

template<class T, class SearchStrategy>
void NTreeNode<T, SearchStrategy>::RemoveNode(const T& node)
{
    auto findResult = std::find_if(m_childNodes.begin(), m_childNodes.end(),
        [&node](const T val) -> bool
    {
        return val->Get() == node->Get();
    });

    if (findResult != m_childNodes.end())
    {
        XCDELETE(*findResult);
        m_childNodes.erase(findResult);
    }
    else
    {
        //Could not delete. Did you request wrong node to be deleted.
        XCASSERT(false);
    }
}

#pragma endregion

#pragma region BinaryTreeNode

template<class T, class SearchStrategy = BreathFirstSearch>
class BinaryTreeNode : public XCTreeNodeModifier<T, BinaryTreeNode<T>, SearchStrategy>
{
public:
    static const u32 LeftChild  = 0;
    static const u32 RightChild = 1;

    BinaryTreeNode()
    {}

    explicit BinaryTreeNode(const T& data)
        : XCTreeNodeModifier(data)
    {}

    //Adds the node to its child list.
    T&                   AddNode(const T& data);

    //Removes a child node. Does not traverse. Use GetParentNode to find the parent.
    void                 RemoveNode(const T& node);
};

template<class T, class SearchStrategy>
T& BinaryTreeNode<T, SearchStrategy>::AddNode(const T& data)
{
    //TODO : This needs to be handled based on weights
    XCASSERT(m_childNodes.size() < 2);

    m_childNodes.push_back(data);

    return m_childNodes.back();
}

template<class T, class SearchStrategy>
void BinaryTreeNode<T, SearchStrategy>::RemoveNode(const T& node)
{
    auto findResult = std::find_if(m_childNodes.begin(), m_childNodes.end(),
        [node](const T val) -> bool
    {
        return val->Get() == node.Get();
    });

    if (findResult != m_childNodes.end())
    {
        XCDELETE(*findResult);
        m_childNodes.erase(findResult);
    }
    else
    {
        //Could not delete. Did you request wrong node to be deleted.
        XCASSERT(false);
    }
}

#pragma endregion