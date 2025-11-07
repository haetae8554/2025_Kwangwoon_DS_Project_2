#pragma once
#ifndef _BpTree_H_
#define _BpTree_H_

#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "EmployeeData.h"
#include "SelectionTree.h"
// for output in log
#include <sstream>
class BpTree
{
private:
	BpTreeNode *root;
	int order;
	ofstream *fout;

	BpTreeNode *nextChild(BpTreeNode *idx, const string &key);

	// get first key from data node
	string firstKeyData(BpTreeDataNode *dn);
	void freeNode(BpTreeNode *n);

public:
	BpTree(ofstream *fout, int order)
	{
		this->order = order;
		this->fout = fout;
		this->root = NULL;
	}

	~BpTree()
	{
		freeNode(root);
		root = NULL;
	}

	/* essential */
	bool Insert(EmployeeData *newData);
	bool excessDataNode(BpTreeNode *pDataNode);
	bool excessIndexNode(BpTreeNode *pIndexNode);
	void splitDataNode(BpTreeNode *pDataNode);
	void splitIndexNode(BpTreeNode *pIndexNode);
	BpTreeNode *getRoot() { return root; }
	BpTreeNode *searchDataNode(string name);
	void searchRange(string start, string end);
};

#endif
