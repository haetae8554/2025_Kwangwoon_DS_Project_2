#include "BpTree.h"
// find next child
BpTreeNode *BpTree::nextChild(BpTreeNode *idx, const string &key)
{
    BpTreeNode *child = idx->getMostLeftChild();
    map<string, BpTreeNode *> *im = idx->getIndexMap();

    if (im != NULL)
    {
        for (auto it = im->begin(); it != im->end(); ++it)
        {
            if (key >= it->first)
            {
                child = it->second;
            }
            else
            {
                break;
            }
        }
    }
    return child;
}

// first key
string BpTree::firstKeyData(BpTreeDataNode *dn)
{
    string k = "";
    map<string, EmployeeData *> *dm = dn->getDataMap();

    if (dm != NULL)
    {
        if (!dm->empty())
        {
            k = dm->begin()->first;
        }
    }
    return k;
}

bool BpTree::Insert(EmployeeData *newData)
{
    // tree empty
    if (root == NULL)
    {
        BpTreeDataNode *dn = new BpTreeDataNode; // data node
        dn->insertDataMap(newData->getName(), newData);
        root = dn;
        return true;
    }

    BpTreeNode *leaf = searchDataNode(newData->getName());
    // leaf not found
    if (leaf == NULL)
    {
        return false;
    }

    // map
    map<string, EmployeeData *> *dm = leaf->getDataMap();
    if (dm != NULL)
    {
        map<string, EmployeeData *>::iterator it = dm->find(newData->getName());
        if (it != dm->end())
        {
            // key exists
            if (it->second != NULL)
            {
                it->second->setDeptNo(newData->getDeptNo());
                it->second->setID(newData->getID());
                it->second->setIncome(newData->getIncome());
                delete newData;
            }
            return true;
        }
        else
        {
            dm->insert(make_pair(newData->getName(), newData));
            if (excessDataNode(leaf))
            {
                splitDataNode(leaf);
            }
            return true;
        }
    }
    return false;
}

bool BpTree::excessDataNode(BpTreeNode *pDataNode)
{
    map<string, EmployeeData *> *dm = pDataNode->getDataMap();

    // null map -> no exceed
    if (dm == NULL)
    {
        return false;
    }

    // max keys = order-1
    int count = (int)dm->size();
    int maxKeys = order - 1;

    // exceed?
    if (count > maxKeys)
    {
        return true;
    }

    return false;
}

bool BpTree::excessIndexNode(BpTreeNode *pIndexNode)
{
    // index map
    map<string, BpTreeNode *> *im = pIndexNode->getIndexMap();

    // null map -> no exceed
    if (im == NULL)
    {
        return false;
    }

    // children = keys+1
    int keyCount = (int)im->size();
    int childCount = keyCount + 1;

    // exceed?
    if (childCount > order)
    {
        return true;
    }

    return false;
}

void BpTree::splitDataNode(BpTreeNode *pDataNode)
{
    BpTreeDataNode *left = (BpTreeDataNode *)pDataNode;
    BpTreeDataNode *right = new BpTreeDataNode;

    // move half to right
    map<string, EmployeeData *> *dm = left->getDataMap();
    int sz = (int)dm->size();
    int mid = sz / 2;

    int idx = 0;
    for (map<string, EmployeeData *>::iterator it = dm->begin(); it != dm->end();)
    {
        if (idx >= mid)
        {
            right->insertDataMap(it->first, it->second);
            map<string, EmployeeData *>::iterator cur = it++;
            dm->erase(cur);
        }
        else
        {
            ++it;
        }
        idx++;
    }

    // link neighbors
    right->setNext(left->getNext());
    if (left->getNext() != NULL)
    {
        left->getNext()->setPrev(right);
    }
    left->setNext(right);
    right->setPrev(left);

    // parent
    BpTreeNode *parent = left->getParent();
    string sep = firstKeyData(right);

    // no parent
    if (parent == NULL)
    {
        BpTreeIndexNode *nr = new BpTreeIndexNode;
        nr->setMostLeftChild(left);
        nr->insertIndexMap(sep, right);
        left->setParent(nr);
        right->setParent(nr);
        root = nr;
        return;
    }

    // has parent
    BpTreeIndexNode *pin = (BpTreeIndexNode *)parent;
    pin->insertIndexMap(sep, right);
    right->setParent(pin);

    if (excessIndexNode(pin))
    {
        splitIndexNode(pin);
    }
}

void BpTree::splitIndexNode(BpTreeNode *pIndexNode)
{
    // left node
    BpTreeIndexNode *left = (BpTreeIndexNode *)pIndexNode;

    // right node
    BpTreeIndexNode *right = new BpTreeIndexNode;

    map<string, BpTreeNode *> *im = left->getIndexMap();
    int sz = (int)im->size();
    int mid = sz / 2;

    // to mid
    map<string, BpTreeNode *>::iterator it = im->begin();
    int i = 0;
    while (i < mid && it != im->end())
    {
        ++it;
        i++;
    }

    if (it == im->end())
    {
        return;
    }

    // promote key
    string promoteKey = it->first;
    BpTreeNode *promoteRightChild = it->second;

    right->setMostLeftChild(promoteRightChild);
    if (promoteRightChild != NULL)
    {
        promoteRightChild->setParent(right);
    }

    map<string, BpTreeNode *>::iterator it_right = it;
    ++it_right;
    while (it_right != im->end())
    {
        right->insertIndexMap(it_right->first, it_right->second);
        if (it_right->second != NULL)
        {
            it_right->second->setParent(right);
        }
        ++it_right;
    }

    // erase from left
    map<string, BpTreeNode *>::iterator it_del = it;
    ++it_del;
    while (it_del != im->end())
    {
        map<string, BpTreeNode *>::iterator cur = it_del++;
        im->erase(cur);
    }
    im->erase(promoteKey);

    BpTreeNode *parent = left->getParent();

    if (parent == NULL)
    {
        BpTreeIndexNode *nr = new BpTreeIndexNode;
        nr->setMostLeftChild(left);
        nr->insertIndexMap(promoteKey, right);
        left->setParent(nr);
        right->setParent(nr);
        root = nr;
        return;
    }

    // insert to parent
    BpTreeIndexNode *pin = (BpTreeIndexNode *)parent;
    pin->insertIndexMap(promoteKey, right);
    right->setParent(pin);

    // check parent
    if (excessIndexNode(pin))
    {
        splitIndexNode(pin);
    }
}

BpTreeNode *BpTree::searchDataNode(string name)
{
    // tree empty
    if (root == NULL)
    {
        return NULL;
    }

    // root data node
    if (dynamic_cast<BpTreeIndexNode *>(root) == NULL)
    {
        return root;
    }

    // down
    BpTreeNode *cur = root;

    while (cur != NULL)
    {
        if (dynamic_cast<BpTreeIndexNode *>(cur) != NULL)
        {
            cur = nextChild(cur, name);
        }
        else
        {
            break;
        }
    }

    return cur;
}

void BpTree::searchRange(string start, string end)
{
    if (root == NULL)
    {
        *fout << "========ERROR========\n";
        *fout << "300\n";
        *fout << "=====================\n\n";
        return;
    }

    BpTreeNode *cur = searchDataNode(start);
    if (!cur)
    {
        *fout << "========ERROR========\n";
        *fout << "300\n";
        *fout << "=====================\n\n";
        return;
    }

    bool any = false;
    stringstream ss;

    while (cur)
    {
        map<string, EmployeeData *> *dm = cur->getDataMap();
        if (dm)
        {
            for (auto it = dm->begin(); it != dm->end(); ++it)
            {
                if (it->second)
                {
                    const string &key = it->first;

                    // key ∈ [start, end] or key starts_with(end)
                    bool ge_start = !(key < start);
                    bool le_end = !(key > end);
                    bool starts_with_end = (key.compare(0, end.size(), end) == 0);

                    if (ge_start && (le_end || starts_with_end))
                    {
                        ss << " " << it->second->getName() << "/"
                           << it->second->getDeptNo() << "/"
                           << it->second->getID() << "/"
                           << it->second->getIncome() << "\n";
                        any = true;
                    }
                }
            }
        }

        // next node
        BpTreeNode *next = cur->getNext();
        if (next)
        {
            auto dm2 = next->getDataMap();
            if (dm2 && !dm2->empty())
            {
                const string &first = dm2->begin()->first;
                bool starts_with_end = (first.compare(0, end.size(), end) == 0);
                if (!starts_with_end && first > end)
                    break;
            }
        }
        cur = next;
    }

    if (!any)
    {
        *fout << "========ERROR========\n";
        *fout << "300\n";
        *fout << "=====================\n\n";
        return;
    }

    *fout << "========SEARCH_BP========\n";
    *fout << ss.str();
    *fout << " =======================\n\n";
}

void BpTree::freeNode(BpTreeNode *n)
{
    if (n == NULL)
        return;

    // index node: free most-left
    // all children in index map
    if (dynamic_cast<BpTreeIndexNode *>(n) != NULL)
    {
        BpTreeIndexNode *in = (BpTreeIndexNode *)n;

        BpTreeNode *ml = in->getMostLeftChild();
        if (ml)
            freeNode(ml);

        map<string, BpTreeNode *> *im = in->getIndexMap();
        if (im != NULL)
        {
            for (auto &kv : *im)
            {
                if (kv.second)
                    freeNode(kv.second);
            }
        }
    }

    else if (dynamic_cast<BpTreeDataNode *>(n) != NULL)
    {
        BpTreeDataNode *dn = (BpTreeDataNode *)n;
        map<string, EmployeeData *> *dm = dn->getDataMap();
        if (dm != NULL)
        {
            for (auto &kv : *dm)
            {
                if (kv.second)
                    delete kv.second; // EmployeeData free
            }
            dm->clear();
        }
    }

    delete n;
}
