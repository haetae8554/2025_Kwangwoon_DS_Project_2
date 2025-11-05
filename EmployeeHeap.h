#pragma once

#include "EmployeeData.h"

class EmployeeHeap
{
private:
    int datanum;
    EmployeeData **heapArr;
    int maxCapacity = 10;

public:
    EmployeeHeap()
    {
        datanum = 0;
        heapArr = new EmployeeData *[maxCapacity + 1];
        for (int i = 0; i <= maxCapacity; i++)
        {
            heapArr[i] = NULL;
        }
    }
    ~EmployeeHeap()
    {
        if (heapArr)
            delete[] heapArr;
    }

    void Insert(EmployeeData *data);
    EmployeeData *Top();
    void Delete();

    bool IsEmpty();

    void UpHeap(int index);
    void DownHeap(int index);
    void ResizeArray();

    void GetAll(vector<EmployeeData *> &out);
};