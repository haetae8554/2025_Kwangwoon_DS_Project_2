#include "SelectionTree.h"

// helper: map dept_no to run index (0~7)
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

// helper: choose winner by income
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

// helper: update path to root
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
    // make 15 nodes (index 1..15), leaves are 8..15
    SelectionTreeNode *node[16];
    for (int i = 1; i <= 15; i++)
    {
        node[i] = new SelectionTreeNode();
    }
    // link parent-child
    for (int i = 1; i <= 7; i++)
    {
        node[i]->setLeftChild(node[i * 2]);
        node[i]->setRightChild(node[i * 2 + 1]);
        node[i * 2]->setParent(node[i]);
        node[i * 2 + 1]->setParent(node[i]);
    }
    // set root
    this->root = node[1];

    // map 8 leaves to run (0~7)
    for (int k = 0; k < 8; k++)
    {
        run[k] = node[8 + k];
        // heap is lazy (make when insert)
        // set leaf data from heap later
    }
}

// insert one EmployeeData into its dept heap and update winners
bool SelectionTree::Insert(EmployeeData *newData)
{
    if (newData == NULL)
        return false;
    if (root == NULL)
        setTree();

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
    // insert to heap
    leaf->getHeap()->Insert(newData);

    // leaf shows heap top
    leaf->setEmployeeData(leaf->getHeap()->Top());

    // update to root
    update_up(leaf);
    return true;
}

// delete root winner (remove from corresponding heap) and update
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

    // remove from heap
    leaf->getHeap()->Delete();

    // refresh leaf data (top or null)
    if (leaf->getHeap()->IsEmpty())
    {
        leaf->setEmployeeData(NULL);
    }
    else
    {
        leaf->setEmployeeData(leaf->getHeap()->Top());
    }

    // update to root
    update_up(leaf);
    return true;
}

// print all data of dept_no by name ascending (do not change heap)
bool SelectionTree::printEmployeeData(int dept_no)
{
    // header for this command
    (*fout) << "========PRINT_ST " << dept_no << "========\n";

    if (root == NULL)
    {
        (*fout) << "Error: Tree not built\n";
        (*fout) << "=====================\n\n";
        return false;
    }

    int idx = dept_to_idx(dept_no);
    if (idx < 0)
    {
        (*fout) << "Error: Invalid department\n";
        (*fout) << "=====================\n\n";
        return false;
    }

    SelectionTreeNode *leaf = run[idx];
    if (leaf == NULL)
    {
        (*fout) << "Error: No leaf for dept\n";
        (*fout) << "=====================\n\n";
        return false;
    }

    EmployeeHeap *hp = leaf->getHeap();
    if (hp == NULL)
    {
        (*fout) << "Error: No heap\n";
        (*fout) << "=====================\n\n";
        return false;
    }
    if (hp->IsEmpty())
    {
        (*fout) << "Error: No employee data\n";
        (*fout) << "=====================\n\n";
        return false;
    }

    // get all
    std::vector<EmployeeData *> v;
    hp->GetAll(v);

    // sort by name asc
    std::sort(v.begin(), v.end(),
              [](EmployeeData *a, EmployeeData *b) -> bool
              {
                  if (a == NULL && b == NULL)
                      return false;
                  if (a != NULL && b == NULL)
                      return true;
                  if (a == NULL && b != NULL)
                      return false;
                  return a->getName() < b->getName();
              });

    // print "name/dept/id/income"
    for (size_t i = 0; i < v.size(); i++)
    {
        if (v[i] == NULL)
            continue;
        (*fout)
            << v[i]->getName() << "/"
            << v[i]->getDeptNo() << "/"
            << v[i]->getID() << "/"
            << v[i]->getIncome() << "\n";
    }

    // footer
    (*fout) << "=====================\n\n";
    return true;
}
