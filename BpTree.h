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
	int order; // m children
			   // class inner functions

	// go down from index node by key
	BpTreeNode *nextChild(BpTreeNode *idx, const string &key);

	// get first key from data node
	string firstKeyData(BpTreeDataNode *dn);

public:
	BpTree(ofstream *fout, int order = 3)
	{
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
};

#endif
