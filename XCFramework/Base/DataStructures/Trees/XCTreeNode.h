/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#define DEBUG_TREES 1

//Expects data to be pointer.
//On destruction it will delete the contained data
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

    inline bool operator ==(const XCTreeNode<T>& otherObj)
    {
        return Get() == otherObj.Get();
    }

    inline bool operator !=(const XCTreeNode<T>& otherObj)
    {
        return Get() != otherObj.Get();
    }

    //Adds the node to its child list.
    XCTreeNode<T>&                   AddNode(const T& data);

    //Removes a child node. Does not traverse. Use GetParentNode to find the parent.
    void                             RemoveNode(const XCTreeNode<T>& node);

    const T&                         Get() const         { return m_data; }
    T&                               GetMutable()        { return m_data; }
    std::vector<XCTreeNode<T>* >&    GetNodesMutable()   { return m_childNodes; }
    XCTreeNode<T>*                   GetParentNode(const XCTreeNode<T>& childNode);

    bool                             HasChildNodes() { m_childNodes.size() > 0; }
    void                             Print();

private:
    std::vector<XCTreeNode<T>* >  m_childNodes;
    T                             m_data;
};

template<class T>
XCTreeNode<T>& XCTreeNode<T>::AddNode(const T& data)
{
    XCTreeNode<T>* node = XCNEW(XCTreeNode<T>)(data);
    m_childNodes.push_back(node);

    return *m_childNodes.back();
}

template<class T>
void XCTreeNode<T>::RemoveNode(const XCTreeNode<T>& node)
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

template<class T>
XCTreeNode<T>*  XCTreeNode<T>::GetParentNode(const XCTreeNode<T>& childNode)
{
    XCTreeNode<T>* outNode = nullptr;

    //Follow BFS
    //First check within this list
    for (auto& node : m_childNodes)
    {
        if (*node == childNode)
        {
            outNode = this;
            break;
        }
    }

    if (outNode == nullptr)
    {
        //Ask the child nodes
        for (auto& node : m_childNodes)
        {
            outNode = node->GetParentNode(childNode);
            if (outNode != nullptr)
            {
                break;
            }
        }
    }

    return outNode;
}

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