#pragma once
#include "EmployeeHeap.h"

void EmployeeHeap::Insert(EmployeeData *data)
{
    //empty
    if (data == NULL)
        return;

    //full
    if (datanum + 1 > maxCapacity)
    {
        ResizeArray();
    }
    datanum = datanum + 1;
    heapArr[datanum] = data;

    UpHeap(datanum);
}

// return top 
EmployeeData *EmployeeHeap::Top()
{
    //empty
    if (IsEmpty())
        return NULL;
        // not use 0 
    return heapArr[1];
}

void EmployeeHeap::Delete()
{
    if (IsEmpty())
        return;

    heapArr[1] = heapArr[datanum];
    heapArr[datanum] = NULL;
    datanum = datanum - 1;

    if (!IsEmpty())
    {
        DownHeap(1);
    }
}

bool EmployeeHeap::IsEmpty()
{
    if (datanum == 0)
        return true;
    return false;
}

void EmployeeHeap::UpHeap(int index)
{
    // bubble up
    while (index > 1)
    {
        // p node
        int parent = index / 2;
        // no p
        if (heapArr[parent] == NULL){
            break;
        }
            
        //
        if (heapArr[index] == NULL){
            break;
        }

        int a = heapArr[parent]->getIncome();
        int b = heapArr[index]->getIncome();

        if (b > a)
        {
            EmployeeData *tmp = heapArr[parent];
            heapArr[parent] = heapArr[index];
            heapArr[index] = tmp;
            index = parent;
        }
        else
        {
            break;
        }
    }
}

void EmployeeHeap::DownHeap(int index)
{
    //  down
    while (true)
    {
        int left = index * 2;
        int right = index * 2 + 1;
        int big = index;

        if (left <= datanum)
        {
            if (heapArr[left] != NULL && heapArr[big] != NULL)
            {
                // left big
                if (heapArr[left]->getIncome() > heapArr[big]->getIncome())
                {
                    big = left;
                }
            }
            else if (heapArr[left] != NULL && heapArr[big] == NULL)
            {
                big = left;
            }
        }
        if (right <= datanum)
        {
            if (heapArr[right] != NULL && heapArr[big] != NULL)
            {
                //right big
                if (heapArr[right]->getIncome() > heapArr[big]->getIncome())
                {
                    big = right;
                }
            }
            else if (heapArr[right] != NULL && heapArr[big] == NULL)
            {
                big = right;
            }
        }
        // swap 
        if (big != index)
        {
            EmployeeData *tmp = heapArr[index];
            heapArr[index] = heapArr[big];
            heapArr[big] = tmp;
            index = big;
        }
        else
        {
            break;
        }
    }
}

// copy and *2 size array 
void EmployeeHeap::ResizeArray()
{
    int newCap = maxCapacity * 2;
    EmployeeData **arr = new EmployeeData *[newCap + 1];

    int i = 0;
    while (i <= newCap)
    {
        arr[i] = NULL;
        i = i + 1;
    }
    for (int j = 1; j <= datanum; j++)
    {
        arr[j] = heapArr[j];
    }
    delete[] heapArr;
    heapArr = arr;
    maxCapacity = newCap;
}
// for print
void EmployeeHeap::GetAll(vector<EmployeeData *> &out)
{
    for (int i = 1; i <= datanum; i++)
    {
        if (heapArr[i] != NULL)
            out.push_back(heapArr[i]);
    }
}