#include "BpTree.h"

// next child  key (for index node)
BpTreeNode *BpTree::nextChild(BpTreeNode *idx, const string &key)
{
    BpTreeNode *child = idx->getMostLeftChild();
    map<string, BpTreeNode *> *im = idx->getIndexMap();

    if (im != NULL)
    {
        for (map<string, BpTreeNode *>::iterator it = im->begin(); it != im->end(); ++it)
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

// get first key from data node
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
    // B+-Tree empty
    if (root == NULL)
    {
        BpTreeDataNode *dn = new BpTreeDataNode; // make Data Node
        dn->insertDataMap(newData->getName(), newData);
        root = dn;
        return true;
    }
    //
    BpTreeNode *leaf = searchDataNode(newData->getName());
    // if leaf not found
    if (leaf == NULL)
    {
        return false;
    }
    // mapping
    map<string, EmployeeData *> *dm = leaf->getDataMap();
    if (dm != NULL)
    {
        map<string, EmployeeData *>::iterator it = dm->find(newData->getName());
        if (it != dm->end())
        {
            // key exist?
            if (it->second != NULL)
            {
                it->second->setDeptNo(newData->getDeptNo());
                it->second->setID(newData->getID());
                it->second->setIncome(newData->getIncome());
                delete newData;
            }
            return true;
        }
    }
    return false;
}

bool BpTree::excessDataNode(BpTreeNode *pDataNode)
{
    map<string, EmployeeData *> *dm = pDataNode->getDataMap();

    // if map pointer null -> treat as not exceed
    if (dm == NULL)
    {
        return false;
    }

    // leaf can have at most (order - 1) keys
    int count = (int)dm->size();
    int maxKeys = order - 1;

    // if current keys > max allowed -> exceed
    if (count > maxKeys)
    {
        return true;
    }

    return false;
}

bool BpTree::excessIndexNode(BpTreeNode *pIndexNode)
{
    // inex map
    map<string, BpTreeNode *> *im = pIndexNode->getIndexMap();

    // if map pointer null -> not exceed
    if (im == NULL)
    {
        return false;
    }

    // index node
    //(number of children) = (number of keys) + 1
    int keyCount = (int)im->size();
    int childCount = keyCount + 1;

    // if child count > order -> node exceed
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

    // half move right
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
    // link connect
    right->setNext(left->getNext());
    if (left->getNext() != NULL)
    {
        left->getNext()->setPrev(right);
    }
    left->setNext(right);
    right->setPrev(left);
    // check parent
    BpTreeNode *parent = left->getParent();
    string sep = firstKeyData(right);

    // if no parent
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
    // exist parent
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
    // old index node
    BpTreeIndexNode *left = (BpTreeIndexNode *)pIndexNode;

    // new index node
    BpTreeIndexNode *right = new BpTreeIndexNode;

    map<string, BpTreeNode *> *im = left->getIndexMap();
    int sz = (int)im->size();
    int mid = sz / 2;

    // find key iterator
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

    // promote key and right child
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

    // erase keys from left
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

    // parent exists -> insert promoteKey into parent
    BpTreeIndexNode *pin = (BpTreeIndexNode *)parent;
    pin->insertIndexMap(promoteKey, right);
    right->setParent(pin);

    // check parent exceed
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

    // root data node (leaf)
    if (dynamic_cast<BpTreeIndexNode *>(root) == NULL)
    {
        return root;
    }

    // down  index nodes
    BpTreeNode *cur = root;

    while (cur != NULL)
    {
        if (dynamic_cast<BpTreeIndexNode *>(cur) != NULL)
        {
            // choose next child by key
            cur = nextChild(cur, name);
        }
        else
        {
            // reach a data node (leaf)
            break;
        }
    }

    return cur; // leaf or NULL
}

BpTreeNode *BpTree::searchRange(string start, string end)
{
    // if tree is empty
    if (root == NULL)
    {
        return NULL;
    }

    // find the leaf node that contains the 'start' key
    BpTreeNode *leaf = searchDataNode(start);
    if (leaf == NULL)
    {
        return NULL;
    }

    // if the leaf is a data node, return its address
    if (dynamic_cast<BpTreeDataNode *>(leaf) != NULL)
    {
        return leaf;
    }

    // if not a data node (rare case), go down until reach data node
    BpTreeNode *cur = leaf;
    while (cur != NULL)
    {
        if (dynamic_cast<BpTreeIndexNode *>(cur) != NULL)
        {
            cur = nextChild(cur, start);
        }
        else
        {
            break;
        }
    }

    return cur; // return the data node (start point of range)
}
