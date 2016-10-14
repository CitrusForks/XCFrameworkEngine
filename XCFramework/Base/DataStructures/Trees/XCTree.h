/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

//#define DEBUG_TREES 1

#include "XCTreeNode.h"

template<class T, class TreeType>
class XCTree
{
public:
    explicit XCTree(const T& data)
    {
        m_rootNode = XCNEW(TreeType)(data);
    }

    ~XCTree()
    {
        XCDELETE(static_cast<TreeType*>(m_rootNode));
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

template<class T, class TreeType>
XCTreeNode<T>& XCTree<T, TreeType>::AddNode(const T& data)
{
    return AddNode(*m_rootNode, data);
}

template<class T, class TreeType>
XCTreeNode<T>& XCTree<T, TreeType>::AddNode(XCTreeNode<T>& parentNode, const T& data)
{
    return static_cast<TreeType&>(parentNode).AddNode(data);
}

template<class T, class TreeType>
void XCTree<T, TreeType>::RemoveNode(XCTreeNode<T>& node)
{
    //Find the parent node containing this node
    XCTreeNode<T>* parentNode = GetParentNode(node);
    XCASSERT(parentNode != nullptr);

    static_cast<TreeType*>(parentNode)->RemoveNode(node);
}

template<class T, class TreeType>
void XCTree<T, TreeType>::RemoveNode(const T& dataPattern)
{
    XCASSERT(false);
}

template<class T, class TreeType>
XCTreeNode<T>* XCTree<T, TreeType>::GetParentNode(const XCTreeNode<T>& childNode)
{
    XCTreeNode<T>* outNode = m_rootNode;

    //Traverse from root node
    if (*outNode != childNode)
    {
        outNode = static_cast<TreeType*>(m_rootNode)->GetParentNode(childNode);
    }

    return outNode;
}

template<class T, class TreeType>
void XCTree<T, TreeType>::PrintAll()
{
    m_rootNode->Print();
}

template<class T>
using XCNTreeBFS = XCTree<T, NTree<T, BreathFirstSearch>>;

template<class T>
using XCNTreeDFS = XCTree<T, NTree<T, DepthFirstSearch>>;