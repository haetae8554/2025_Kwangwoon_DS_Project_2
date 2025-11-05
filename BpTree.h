#pragma once
#ifndef _BpTree_H_
#define _BpTree_H_

#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "EmployeeData.h"
#include "SelectionTree.h"

class BpTree
{
private:
	BpTreeNode *root;
	int order;
	ofstream *fout;

	BpTreeNode *nextChild(BpTreeNode *idx, const string &key);

	// get first key from data node
	string firstKeyData(BpTreeDataNode *dn);

	// debug
	void printTreeRecursive(BpTreeNode *node, int depth);

public:
	BpTree(ofstream *fout, int order = 3)
	{
		this->order = order;
		this->fout = fout;
		this->root = NULL;
	}

	~BpTree()
	{
	}

	/* essential */
	bool Insert(EmployeeData *newData);
	bool excessDataNode(BpTreeNode *pDataNode);
	bool excessIndexNode(BpTreeNode *pIndexNode);
	void splitDataNode(BpTreeNode *pDataNode);
	void splitIndexNode(BpTreeNode *pIndexNode);
	BpTreeNode *getRoot() { return root; }
	BpTreeNode *searchDataNode(string name);
	BpTreeNode *searchRange(string start, string end);

	// debug
	void debugPrintTree();
	// In BpTree.h (private section)
};

#endif
