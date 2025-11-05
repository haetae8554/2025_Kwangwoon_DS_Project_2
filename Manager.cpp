#include "Manager.h"
#include "EmployeeData.h"
#include "BpTreeIndexNode.h"
#include "BpTreeDataNode.h"

void Manager::run(const char *command)
{
	// open log
	flog.open("log.txt", ios::out | ios::trunc);
	if (!flog.is_open())
		return;

	ifstream cmdin(command);
	if (!cmdin.is_open())
	{
		printErrorCode(100);
		return;
	}

	// make structures
	bptree = new BpTree(&flog); // order not used here
	stree = new SelectionTree(&flog);

	string token;
	while (cmdin >> token)
	{
		if (token == "EXIT")
		{
			printSuccessCode("EXIT");
			break;
		}
		else if (token == "LOAD")
		{
			LOAD();
		}
		else if (token == "ADD_BP")
		{
			// parse inline like 1st project
			string name;
			int dept, id, income;
			if (cmdin >> name >> dept >> id >> income)
			{
				flog << "========ADD_BP========\n";
				EmployeeData *e = new EmployeeData;
				e->setData(name, dept, id, income);
				bool ok = bptree->Insert(e);
				if (ok)
				{
					flog << "Success\n";
					flog << "=====================\n\n";
				}
				else
				{
					delete e;
					printErrorCode(200);
				}
			}
			else
			{
				printErrorCode(200);
				cmdin.clear();
			}
		}
		else if (token == "SEARCH_BP")
		{
			string a, b;
			if (cmdin >> a)
			{
				// try second arg
				streampos p = cmdin.tellg();
				if (cmdin >> b)
				{
					SEARCH_BP_RANGE(a, b);
				}
				else
				{
					cmdin.clear();
					cmdin.seekg(p);
					SEARCH_BP_NAME(a);
				}
			}
			else
			{
				printErrorCode(300);
			}
		}
		else if (token == "PRINT_BP")
		{
			PRINT_BP();
		}
		else if (token == "ADD_ST")
		{
			string kind;
			if (cmdin >> kind)
			{
				if (kind == "dept_no")
				{
					int d;
					if (cmdin >> d)
						ADD_ST_DEPTNO(d);
					else
						printErrorCode(500);
				}
				else if (kind == "name")
				{
					string n;
					if (cmdin >> n)
						ADD_ST_NAME(n);
					else
						printErrorCode(500);
				}
				else
				{
					printErrorCode(500);
				}
			}
			else
			{
				printErrorCode(500);
			}
		}
		else if (token == "PRINT_ST")
		{
			int dept_no;
			if (cmdin >> dept_no)
			{
				// SelectionTree prints inside
				bool ok = stree->printEmployeeData(dept_no);
				if (!ok)
					printErrorCode(600);
			}
			else
			{
				printErrorCode(600);
			}
		}
		else if (token == "DELETE")
		{
			DELETE();
		}
		else
		{
			printErrorCode(800);
		}
	}

	cmdin.close();
}

void Manager::LOAD()
{
	if (loaded)
	{
		printErrorCode(100);
		return;
	}

	ifstream ein("employee.txt");
	if (!ein.is_open())
	{
		printErrorCode(100);
		return;
	}

	flog << "========LOAD========\n";

	string name;
	int dept, id, income;
	bool any = false;

	while (ein >> name >> dept >> id >> income)
	{
		EmployeeData *e = new EmployeeData;
		e->setData(name, dept, id, income);
		bool ok = bptree->Insert(e);
		if (ok)
			any = true;
		else
			delete e;
	}
	ein.close();

	if (any)
	{
		flog << "Success\n";
		flog << "=====================\n\n";
		loaded = true;
	}
	else
	{
		printErrorCode(100);
	}
}

void Manager::ADD_BP()
{
	// not used (parse in run)
}

void Manager::SEARCH_BP_NAME(string name)
{
	flog << "========SEARCH_BP========\n";

	BpTreeNode *leaf = bptree->searchDataNode(name);
	if (leaf == NULL)
	{
		printErrorCode(300);
		return;
	}

	map<string, EmployeeData *> *dm = leaf->getDataMap();
	if (dm == NULL)
	{
		printErrorCode(300);
		return;
	}

	map<string, EmployeeData *>::iterator it = dm->find(name);
	if (it == dm->end())
	{
		printErrorCode(300);
		return;
	}

	if (it->second)
	{
		// print one employee (no static)
		flog << it->second->getName() << "/"
			 << it->second->getDeptNo() << "/"
			 << it->second->getID() << "/"
			 << it->second->getIncome() << "\n";
		flog << "=====================\n\n";
		return;
	}

	printErrorCode(300);
}

void Manager::SEARCH_BP_RANGE(string start, string end)
{
	flog << "========SEARCH_BP========\n";

	BpTreeNode *cur = bptree->searchRange(start, end);
	if (cur == NULL)
	{
		printErrorCode(300);
		return;
	}

	bool any = false;

	while (cur)
	{
		map<string, EmployeeData *> *dm = cur->getDataMap();
		if (dm)
		{
			for (map<string, EmployeeData *>::iterator it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second)
				{
					if (!(it->first < start))
					{
						if (!(it->first > end))
						{
							flog << it->second->getName() << "/"
								 << it->second->getDeptNo() << "/"
								 << it->second->getID() << "/"
								 << it->second->getIncome() << "\n";
							any = true;
						}
					}
				}
			}
		}
		cur = cur->getNext();
		if (cur)
		{
			map<string, EmployeeData *> *dm2 = cur->getDataMap();
			if (dm2 && !dm2->empty())
			{
				string k = dm2->begin()->first;
				if (k > end)
					cur = NULL;
			}
		}
	}

	if (!any)
	{
		printErrorCode(300);
		return;
	}

	flog << "=====================\n\n";
}

void Manager::ADD_ST_DEPTNO(int dept_no)
{
	flog << "========ADD_ST========\n";

	// go leftmost data (inline, no static)
	BpTreeNode *cur = bptree->getRoot();
	if (cur)
	{
		while (cur)
		{
			if (dynamic_cast<BpTreeIndexNode *>(cur) != NULL)
			{
				cur = cur->getMostLeftChild();
			}
			else
			{
				break;
			}
		}
	}

	bool any = false;

	while (cur)
	{
		map<string, EmployeeData *> *dm = cur->getDataMap();
		if (dm)
		{
			for (map<string, EmployeeData *>::iterator it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second)
				{
					if (it->second->getDeptNo() == dept_no)
					{
						bool ok = stree->Insert(it->second);
						if (ok)
							any = true;
					}
				}
			}
		}
		cur = cur->getNext();
	}

	if (any)
	{
		flog << "Success\n";
		flog << "=====================\n\n";
	}
	else
	{
		printErrorCode(500);
	}
}

void Manager::ADD_ST_NAME(string name)
{
	flog << "========ADD_ST========\n";

	BpTreeNode *leaf = bptree->searchDataNode(name);
	if (leaf == NULL)
	{
		printErrorCode(500);
		return;
	}
	map<string, EmployeeData *> *dm = leaf->getDataMap();
	if (dm == NULL)
	{
		printErrorCode(500);
		return;
	}
	map<string, EmployeeData *>::iterator it = dm->find(name);
	if (it == dm->end() || it->second == NULL)
	{
		printErrorCode(500);
		return;
	}

	bool ok = stree->Insert(it->second);
	if (ok)
	{
		flog << "Success\n";
		flog << "=====================\n\n";
	}
	else
	{
		printErrorCode(500);
	}
}

void Manager::PRINT_BP()
{
	flog << "========PRINT_BP========\n";

	// go leftmost data (inline)
	BpTreeNode *cur = bptree->getRoot();
	if (cur)
	{
		while (cur)
		{
			if (dynamic_cast<BpTreeIndexNode *>(cur) != NULL)
			{
				cur = cur->getMostLeftChild();
			}
			else
			{
				break;
			}
		}
	}

	if (cur == NULL)
	{
		printErrorCode(400);
		return;
	}

	bool any = false;

	while (cur)
	{
		map<string, EmployeeData *> *dm = cur->getDataMap();
		if (dm)
		{
			for (map<string, EmployeeData *>::iterator it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second)
				{
					flog << it->second->getName() << "/"
						 << it->second->getDeptNo() << "/"
						 << it->second->getID() << "/"
						 << it->second->getIncome() << "\n";
					any = true;
				}
			}
		}
		cur = cur->getNext();
	}

	if (!any)
	{
		printErrorCode(400);
		return;
	}

	flog << "=====================\n\n";
}

void Manager::PRINT_ST()
{
	// not used (PRINT_ST is handled in run with parameter)
}

void Manager::DELETE()
{
	flog << "========DELETE========\n";

	bool ok = stree->Delete();
	if (ok)
	{
		flog << "Success\n";
		flog << "=====================\n\n";
	}
	else
	{
		printErrorCode(700);
	}
}

void Manager::printErrorCode(int n)
{
	flog << "========ERROR========\n";
	flog << n << "\n";
	flog << "=====================\n\n";
}

void Manager::printSuccessCode(string success)
{
	flog << "========" << success << "========\n";
	flog << "Success" << "\n";
	flog << "====================\n\n";
}
