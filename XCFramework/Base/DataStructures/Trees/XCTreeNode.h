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
        : m_data(nullptr)
    {}

    explicit XCTreeNode(const T& data)
        : m_data(data)
    {}

    inline bool operator ==(const XCTreeNode<T>& otherObj)
    {
        return Get() == otherObj.Get();
    }

    inline bool operator !=(const XCTreeNode<T>& otherObj)
    {
        return Get() != otherObj.Get();
    }

    const T&                         Get() const         { return m_data; }
    T&                               GetMutable()        { return m_data; }
    std::vector<XCTreeNode<T>* >&    GetNodesMutable()   { return m_childNodes; }

    bool                             HasChildNodes() { m_childNodes.size() > 0; }
    void                             Print();

protected:

    //Protect this dtor and call delete on derived ones.
    ~XCTreeNode()
    {
        //Delete contained data.
        XCDELETE(m_data);

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
    std::vector<XCTreeNode<T>* >     m_childNodes;
    T                                m_data;
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
    static XCTreeNode<T>* GetParentNode(XCTreeNode<T>& parentNode, const XCTreeNode<T>& childNode);
};

template<class T>
XCTreeNode<T>* DepthFirstSearch::GetParentNode(XCTreeNode<T>& parentNode, const XCTreeNode<T>& childNode)
{
    XCTreeNode<T>* outNode = nullptr;
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
    static XCTreeNode<T>* GetParentNode(XCTreeNode<T>& parentNode, const XCTreeNode<T>& childNode);
};

template<class T>
XCTreeNode<T>* BreathFirstSearch::GetParentNode(XCTreeNode<T>& parentNode, const XCTreeNode<T>& childNode)
{
    XCTreeNode<T>* outNode = nullptr;
    auto& childNodesList = parentNode.GetNodesMutable();

    //First check within this list
    for (auto& node : childNodesList)
    {
        if (*node == childNode)
        {
            outNode = &parentNode;
            break;
        }
    }

    if (outNode == nullptr)
    {
        //Ask the child nodes
        for (auto& node : childNodesList)
        {
            outNode = GetParentNode(*node, childNode);
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

    explicit XCTreeNodeModifier(const T& data)
        : XCTreeNode(data)
    {}

    //Adds the node to its child list.
    inline XCTreeNode<T>& AddNode(const T& data)
    {
        (static_cast<TreeStrategy*>(this))->AddNode(data);
    }

    //Removes a child node. Does not traverse. Use GetParentNode to find the parent.
    inline void RemoveNode(const XCTreeNode<T>& node)
    {
        (static_cast<TreeStrategy*>(this))->RemoveNode(data);
    }

    inline XCTreeNode<T>* GetParentNode(const XCTreeNode<T>& childNode)
    {
        return SearchStartegy::GetParentNode<T>(*this, childNode);
    }

protected:
    ~XCTreeNodeModifier()
    {}
};

#pragma endregion

#pragma region NTree

template<class T, class SearchStrategy = BreathFirstSearch>
class NTree : public XCTreeNodeModifier<T, NTree<T>, SearchStrategy>
{
public:
    NTree()
    {}

    explicit NTree(const T& data)
        : XCTreeNodeModifier(data)
    {}

    //Adds the node to its child list.
    XCTreeNode<T>&                   AddNode(const T& data);

    //Removes a child node. Does not traverse. Use GetParentNode to find the parent.
    void                             RemoveNode(const XCTreeNode<T>& node);
};

template<class T, class SearchStrategy>
XCTreeNode<T>& NTree<T, SearchStrategy>::AddNode(const T& data)
{
    XCTreeNode<T>* node = XCNEW(NTree<T>)(data);
    m_childNodes.push_back(node);

    return *m_childNodes.back();
}

template<class T, class SearchStrategy>
void NTree<T, SearchStrategy>::RemoveNode(const XCTreeNode<T>& node)
{
    auto findResult = std::find_if(m_childNodes.begin(), m_childNodes.end(),
        [&node](const XCTreeNode<T>* val) -> bool
    {
        return val->Get() == node.Get();
    });

    if (findResult != m_childNodes.end())
    {
        m_childNodes.erase(findResult);
    }
    else
    {
        //Could not delete. Did you request wrong node to be deleted.
        XCASSERT(false);
    }
}

#pragma endregion

#pragma region BinaryTree

template<class T, class SearchStrategy = BreathFirstSearch>
class BinaryTree : public XCTreeNodeModifier<T, BinaryTree<T>, SearchStrategy>
{
public:
    static const u32 LeftChild  = 0;
    static const u32 RightChild = 1;

    BinaryTree()
    {}

    explicit BinaryTree(const T& data)
        : XCTreeNodeModifier(data)
    {}

    //Adds the node to its child list.
    XCTreeNode<T>&                   AddNode(const T& data);

    //Removes a child node. Does not traverse. Use GetParentNode to find the parent.
    void                             RemoveNode(const XCTreeNode<T>& node);
};

template<class T, class SearchStrategy>
XCTreeNode<T>& BinaryTree<T, SearchStrategy>::AddNode(const T& data)
{
    //TODO : This needs to be handled based on weights
    XCASSERT(m_childNodes.size() < 2);

    XCTreeNode<T>* node = XCNEW(BinaryTree<T>)(data);
    m_childNodes.push_back(node);

    return *m_childNodes.back();
}

template<class T, class SearchStrategy>
void BinaryTree<T, SearchStrategy>::RemoveNode(const XCTreeNode<T>& node)
{
    auto findResult = std::find_if(m_childNodes.begin(), m_childNodes.end(),
        [node](const XCTreeNode<T>* val) -> bool
    {
        return val->Get() == node.Get();
    });

    if (findResult != m_childNodes.end())
    {
        m_childNodes.erase(findResult);
    }
    else
    {
        //Could not delete. Did you request wrong node to be deleted.
        XCASSERT(false);
    }
}

#pragma endregion