/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

//#define DEBUG_TREES 1

#include "XCTreeNode.h"

template<class T>
class XCTree
{
public:
    explicit XCTree(const T& data)
    {
        m_rootNode = data;
    }

    ~XCTree()
    {
        XCDELETE(static_cast<T>(m_rootNode));
    }

    //Node Addition
    //Adds the node to the root node.
    T&                AddNode(const T& data);

    //Adds the node to the parentNode's child list
    T&                AddNode(T& parentNode, const T& data);

    //Node Removal
    //Remove node based on node inside tree
    void              RemoveNode(T& parentNode);

    T&                GetRootNode() { return m_rootNode; }
    void              PrintAll();

protected:
    T                 GetParentNode(const T& childNode);

private:
    T                 m_rootNode;
};

template<class T>
T& XCTree<T>::AddNode(const T& data)
{
    return AddNode(m_rootNode, data);
}

template<class T>
T& XCTree<T>::AddNode(T& parentNode, const T& data)
{
    return parentNode->AddNode(data);
}

template<class T>
void XCTree<T>::RemoveNode(T& node)
{
    //Find the parent node containing this node
    T parentNode = GetParentNode(node);
    XCASSERT(parentNode != nullptr);

    parentNode->RemoveNode(node);
}

template<class T>
T XCTree<T>::GetParentNode(const T& childNode)
{
    T outNode = m_rootNode;

    //Traverse from root node
    if (*outNode != childNode)
    {
        outNode = m_rootNode->GetParentNode(childNode);
    }

    return outNode;
}

template<class T>
void XCTree<T>::PrintAll()
{
    m_rootNode->Print();
}