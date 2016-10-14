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
        m_rootNode = XCNEW(XCTreeNode<T>)(data);
    }

    ~XCTree()
    {
        XCDELETE(m_rootNode);
    }

    //Node Addition
    //Adds the node to the root node.
    XCTreeNode<T>&    AddNode(const T& data);

    //Adds the node to the parentNode's child list
    XCTreeNode<T>&    AddNode(XCTreeNode<T>& parentNode, const T& data);

    //Node Removal
    //Remove node based on node inside tree
    void              RemoveNode(XCTreeNode<T>& parentNode);

    //Remove node based on data patterns
    void              RemoveNode(const T& dataPattern);

    XCTreeNode<T>&    GetRootNode() { return *m_rootNode; }
    void              PrintAll();

protected:
    XCTreeNode<T>*    GetParentNode(const XCTreeNode<T>& childNode);

private:
    XCTreeNode<T>*    m_rootNode;
};

template<class T>
XCTreeNode<T>& XCTree<T>::AddNode(const T& data)
{
    return AddNode(*m_rootNode, data);
}

template<class T>
XCTreeNode<T>& XCTree<T>::AddNode(XCTreeNode<T>& parentNode, const T& data)
{
    return parentNode.AddNode(data);
}

template<class T>
void XCTree<T>::RemoveNode(XCTreeNode<T>& node)
{
    //Find the parent node containing this node
    XCTreeNode<T>* parentNode = GetParentNode(node);
    XCASSERT(parentNode != nullptr);

    parentNode->RemoveNode(node);
}

template<class T>
void XCTree<T>::RemoveNode(const T& dataPattern)
{
    XCASSERT(false);
}

template<class T>
XCTreeNode<T>* XCTree<T>::GetParentNode(const XCTreeNode<T>& childNode)
{
    XCTreeNode<T>* outNode = m_rootNode;

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