#pragma once
#include "SelectionTreeNode.h"
#include <algorithm>
#include <fstream>
class SelectionTree
{
private:
    SelectionTreeNode *root;
    ofstream *fout;
    SelectionTreeNode *run[8];

public:
    SelectionTree(ofstream *fout)
    {
        // init pointer
        this->root = NULL;
        this->fout = fout;
        for (int i = 0; i < 8; i++)
        {
            run[i] = NULL;
        }
    }

    ~SelectionTree()
    {
        freeNode(root);
        root = NULL;
        for (int i = 0; i < 8; ++i)
            run[i] = NULL;
    }

    void setRoot(SelectionTreeNode *pN) { this->root = pN; }
    SelectionTreeNode *getRoot() { return root; }

    void setTree();

    bool Insert(EmployeeData *newData);
    bool Delete();
    bool printEmployeeData(int dept_no);

    void freeNode(SelectionTreeNode *node);
};