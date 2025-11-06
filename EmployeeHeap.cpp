#pragma once
#include "EmployeeHeap.h"

void EmployeeHeap::Insert(EmployeeData *data)
{
    if (data == NULL)
        return;

    if (datanum + 1 > maxCapacity)
    {
        ResizeArray();
    }
    datanum = datanum + 1;
    heapArr[datanum] = data;

    UpHeap(datanum);
}

EmployeeData *EmployeeHeap::Top()
{
    if (IsEmpty())
        return NULL;
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
        int parent = index / 2;
        if (heapArr[parent] == NULL)
            break;
        if (heapArr[index] == NULL)
            break;

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
    // push down by income
    while (true)
    {
        int left = index * 2;
        int right = index * 2 + 1;
        int largest = index;

        if (left <= datanum)
        {
            if (heapArr[left] != NULL && heapArr[largest] != NULL)
            {
                if (heapArr[left]->getIncome() > heapArr[largest]->getIncome())
                {
                    largest = left;
                }
            }
            else if (heapArr[left] != NULL && heapArr[largest] == NULL)
            {
                largest = left;
            }
        }
        if (right <= datanum)
        {
            if (heapArr[right] != NULL && heapArr[largest] != NULL)
            {
                if (heapArr[right]->getIncome() > heapArr[largest]->getIncome())
                {
                    largest = right;
                }
            }
            else if (heapArr[right] != NULL && heapArr[largest] == NULL)
            {
                largest = right;
            }
        }
        if (largest != index)
        {
            EmployeeData *tmp = heapArr[index];
            heapArr[index] = heapArr[largest];
            heapArr[largest] = tmp;
            index = largest;
        }
        else
        {
            break;
        }
    }
}

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

void EmployeeHeap::GetAll(vector<EmployeeData *> &out)
{
    for (int i = 1; i <= datanum; i++)
    {
        if (heapArr[i] != NULL)
            out.push_back(heapArr[i]);
    }
}