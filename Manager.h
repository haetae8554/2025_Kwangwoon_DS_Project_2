#pragma once
#include "SelectionTree.h"
#include "BpTree.h"
#include <fstream>

class Manager
{
private:
	char *cmd;
	BpTree *bptree;
	SelectionTree *stree;
	bool loaded; // load flag

public:
	Manager(int bpOrder)
	{ // constructor
		cmd = NULL;
		bptree = NULL;
		stree = NULL;
		loaded = false;
	}

	~Manager()
	{ // destructor
		if (bptree)
		{
			delete bptree;
		}
		if (stree)
		{
			delete stree;
		}
		if (flog.is_open())
		{
			flog.close();
		}
	}

	ifstream fin;
	ofstream flog;

	void run(const char *command);
	void LOAD();
	void ADD_BP();
	void SEARCH_BP_NAME(string name);
	void SEARCH_BP_RANGE(string start, string end);
	void PRINT_BP();
	void ADD_ST_DEPTNO(int dept_no);
	void ADD_ST_NAME(string name);
	void PRINT_ST();
	void DELETE();

	void printErrorCode(int n);
	void printSuccessCode(string success);
};
