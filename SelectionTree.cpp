#include "SelectionTree.h"

// helper: map dept_no to run index 0~7
static int dept_to_idx(int dept_no)
{
    if (dept_no == 100)
        return 0;
    if (dept_no == 200)
        return 1;
    if (dept_no == 300)
        return 2;
    if (dept_no == 400)
        return 3;
    if (dept_no == 500)
        return 4;
    if (dept_no == 600)
        return 5;
    if (dept_no == 700)
        return 6;
    if (dept_no == 800)
        return 7;
    return -1;
}

// helper: choose max by income
static EmployeeData *choose_max(EmployeeData *a, EmployeeData *b)
{
    if (a == NULL && b == NULL)
        return NULL;
    if (a != NULL && b == NULL)
        return a;
    if (a == NULL && b != NULL)
        return b;
    if (a->getIncome() >= b->getIncome())
        return a;
    return b;
}

// helper: update path up to root
static void update_up(SelectionTreeNode *leaf)
{
    SelectionTreeNode *cur = leaf;
    while (cur != NULL)
    {
        SelectionTreeNode *p = cur->getParent();
        if (p == NULL)
            break;

        EmployeeData *L = NULL;
        EmployeeData *R = NULL;

        if (p->getLeftChild() != NULL)
        {
            L = p->getLeftChild()->getEmployeeData();
        }
        if (p->getRightChild() != NULL)
        {
            R = p->getRightChild()->getEmployeeData();
        }
        p->setEmployeeData(choose_max(L, R));
        cur = p;
    }
}

// build perfect binary tree with 8 leaves
void SelectionTree::setTree()
{
    // make 15 nodes (1..15), leaves are 8..15
    SelectionTreeNode *node[16];
    for (int i = 1; i <= 15; i++)
    {
        node[i] = new SelectionTreeNode();
    }

    // link parent and child
    for (int i = 1; i <= 7; i++)
    {
        node[i]->setLeftChild(node[i * 2]);
        node[i]->setRightChild(node[i * 2 + 1]);
        node[i * 2]->setParent(node[i]);
        node[i * 2 + 1]->setParent(node[i]);
    }

    // set root
    root = node[1];

    // map leaves to run
    for (int k = 0; k < 8; k++)
    {
        run[k] = node[8 + k];
    }
}

// insert one data into its dept heap and update to root
bool SelectionTree::Insert(EmployeeData *newData)
{
    if (newData == NULL)
        return false;

    if (root == NULL)
    {
        setTree();
    }

    int idx = dept_to_idx(newData->getDeptNo());
    if (idx < 0)
        return false;

    SelectionTreeNode *leaf = run[idx];
    if (leaf == NULL)
        return false;

    if (leaf->getHeap() == NULL)
    {
        leaf->HeapInit(); // make heap
    }

    // push data into heap
    leaf->getHeap()->Insert(newData);

    // set leaf data by heap top
    if (!leaf->getHeap()->IsEmpty())
    {
        leaf->setEmployeeData(leaf->getHeap()->Top());
    }
    else
    {
        leaf->setEmployeeData(NULL);
    }

    // update to root
    update_up(leaf);
    return true;
}

// delete root winner from its heap and update to root
bool SelectionTree::Delete()
{
    if (root == NULL)
        return false;

    EmployeeData *win = root->getEmployeeData();
    if (win == NULL)
        return false;

    int idx = dept_to_idx(win->getDeptNo());
    if (idx < 0)
        return false;

    SelectionTreeNode *leaf = run[idx];
    if (leaf == NULL)
        return false;
    if (leaf->getHeap() == NULL)
        return false;
    if (leaf->getHeap()->IsEmpty())
        return false;

    // remove one from heap
    leaf->getHeap()->Delete();

    // set leaf data again
    if (!leaf->getHeap()->IsEmpty())
    {
        leaf->setEmployeeData(leaf->getHeap()->Top());
    }
    else
    {
        leaf->setEmployeeData(NULL);
    }

    // update to root
    update_up(leaf);
    return true;
}

// print all data of dept by name asc (do not change heap)
bool SelectionTree::printEmployeeData(int dept_no)
{
    // check basic
    if (root == NULL)
        return false;

    int idx = dept_to_idx(dept_no);
    if (idx < 0)
        return false;

    SelectionTreeNode *leaf = run[idx];
    if (leaf == NULL)
        return false;

    EmployeeHeap *hp = leaf->getHeap();
    if (hp == NULL)
        return false;
    if (hp->IsEmpty())
        return false;

    // get all
    std::vector<EmployeeData *> v;
    hp->GetAll(v);

    // selection sort by name asc
    for (size_t i = 0; i < v.size(); i++)
    {
        if (v[i] == NULL)
            continue;
        size_t min_idx = i;
        for (size_t j = i + 1; j < v.size(); j++)
        {
            if (v[j] == NULL)
                continue;
            if (v[j]->getName() < v[min_idx]->getName())
            {
                min_idx = j;
            }
        }
        if (min_idx != i)
        {
            EmployeeData *tmp = v[i];
            v[i] = v[min_idx];
            v[min_idx] = tmp;
        }
    }

    (*fout) << "========PRINT_ST========\n";

    for (size_t i = 0; i < v.size(); i++)
    {
        if (v[i] == NULL)
            continue;
        (*fout) << v[i]->getName() << "/"
                << v[i]->getDeptNo() << "/"
                << v[i]->getID() << "/"
                << v[i]->getIncome() << "\n";
    }

    (*fout) << "=======================\n\n";
    return true;
}
