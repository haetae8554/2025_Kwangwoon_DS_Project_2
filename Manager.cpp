#include "Manager.h"
#include "EmployeeData.h"
#include "BpTreeIndexNode.h"
#include "BpTreeDataNode.h"
#include <sstream>

using namespace std;

void Manager::run(const char *command)
{
	flog.open("log.txt", ios::out | ios::trunc);
	if (!flog.is_open())
		return;

	ifstream cmdin(command);
	if (!cmdin.is_open())
	{
		printErrorCode(100);
		return;
	}

	bptree = new BpTree(&flog);
	stree = new SelectionTree(&flog);

	string cmd;
	while (cmdin >> cmd)
	{
		if (cmd == "EXIT")
		{
			printSuccessCode("EXIT");
			if (bptree)
			{
				delete bptree;
				bptree = nullptr;
			}
			if (stree)
			{
				delete stree;
				stree = nullptr;
			}
			break;
		}
		else if (cmd == "LOAD")
		{
			LOAD();
		}
		else if (cmd == "ADD_BP")
		{
			string name;
			int dept, id, income;
			if (cmdin >> name >> dept >> id >> income)
			{
				EmployeeData *e = new EmployeeData;
				e->setData(name, dept, id, income);

				bool ok = bptree->Insert(e);
				if (ok)
				{
					flog << "========ADD_BP========\n";

					EmployeeData *p = nullptr;
					if (BpTreeNode *leaf = bptree->searchDataNode(name))
					{
						if (auto dm = leaf->getDataMap())
						{
							auto it = dm->find(name);
							if (it != dm->end() && it->second)
								p = it->second;
						}
					}
					if (!p)
						p = e;

					flog << " " << p->getName() << "/"
						 << p->getDeptNo() << "/"
						 << p->getID() << "/"
						 << p->getIncome() << "\n";
					flog << " =======================\n\n";
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
		else if (cmd == "SEARCH_BP")
		{
			string a, b;
			if (cmdin >> a)
			{
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
		else if (cmd == "PRINT_BP")
		{
			PRINT_BP();
		}
		else if (cmd == "ADD_ST")
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
		else if (cmd == "PRINT_ST")
		{
			int dept_no;
			if (cmdin >> dept_no)
			{
				bool ok = stree->printEmployeeData(dept_no);
				if (!ok)
					printErrorCode(600);
			}
			else
			{
				printErrorCode(600);
			}
		}
		else if (cmd == "DELETE")
		{
			DELETE();
		}
		else
		{
			printErrorCode(800);
		}
	}

	cmdin.close();

	if (bptree)
	{
		delete bptree;
		bptree = nullptr;
	}
	if (stree)
	{
		delete stree;
		stree = nullptr;
	}
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
		printSuccessCode("LOAD");
		loaded = true;
	}
	else
	{
		printErrorCode(100);
	}
}

void Manager::ADD_BP() {}

void Manager::SEARCH_BP_NAME(string name)
{
	BpTreeNode *leaf = bptree->searchDataNode(name);
	if (!leaf)
	{
		printErrorCode(300);
		return;
	}

	auto dm = leaf->getDataMap();
	if (!dm)
	{
		printErrorCode(300);
		return;
	}

	auto it = dm->find(name);
	if (it == dm->end() || !it->second)
	{
		printErrorCode(300);
		return;
	}

	flog << "========SEARCH_BP========\n";
	flog << " " << it->second->getName() << "/"
		 << it->second->getDeptNo() << "/"
		 << it->second->getID() << "/"
		 << it->second->getIncome() << "\n";
	flog << " =======================\n\n";
}

void Manager::SEARCH_BP_RANGE(string start, string end)
{
	BpTreeNode *cur = bptree->searchRange(start, end);
	if (!cur)
	{
		printErrorCode(300);
		return;
	}

	bool any = false;
	stringstream ss;

	while (cur)
	{
		if (auto dm = cur->getDataMap())
		{
			for (auto it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second && !(it->first < start) && !(it->first > end))
				{
					ss << " " << it->second->getName() << "/"
					   << it->second->getDeptNo() << "/"
					   << it->second->getID() << "/"
					   << it->second->getIncome() << "\n";
					any = true;
				}
			}
		}
		cur = cur->getNext();
		if (cur)
		{
			auto dm2 = cur->getDataMap();
			if (dm2 && !dm2->empty())
			{
				if (dm2->begin()->first > end)
					cur = NULL;
			}
		}
	}

	if (!any)
	{
		printErrorCode(300);
		return;
	}

	flog << "========SEARCH_BP========\n";
	flog << ss.str();
	flog << " =======================\n\n";
}

void Manager::ADD_ST_DEPTNO(int dept_no)
{
	BpTreeNode *cur = bptree->getRoot();
	if (cur)
	{
		while (cur)
		{
			if (dynamic_cast<BpTreeIndexNode *>(cur) != NULL)
				cur = cur->getMostLeftChild();
			else
				break;
		}
	}

	bool any = false;

	while (cur)
	{
		if (auto dm = cur->getDataMap())
		{
			for (auto it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second && it->second->getDeptNo() == dept_no)
				{
					if (stree->Insert(it->second))
						any = true;
				}
			}
		}
		cur = cur->getNext();
	}

	if (any)
		printSuccessCode("ADD_ST");
	else
		printErrorCode(500);
}

void Manager::ADD_ST_NAME(string name)
{
	BpTreeNode *leaf = bptree->searchDataNode(name);
	if (!leaf)
	{
		printErrorCode(500);
		return;
	}

	auto dm = leaf->getDataMap();
	if (!dm)
	{
		printErrorCode(500);
		return;
	}

	auto it = dm->find(name);
	if (it == dm->end() || !it->second)
	{
		printErrorCode(500);
		return;
	}

	if (stree->Insert(it->second))
		printSuccessCode("ADD_ST");
	else
		printErrorCode(500);
}

void Manager::PRINT_BP()
{
	BpTreeNode *cur = bptree->getRoot();
	if (cur)
	{
		while (dynamic_cast<BpTreeIndexNode *>(cur) != NULL)
			cur = cur->getMostLeftChild();
	}

	if (!cur)
	{
		printErrorCode(400);
		return;
	}

	bool any = false;
	stringstream ss;

	while (cur)
	{
		if (auto dm = cur->getDataMap())
		{
			for (auto it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second)
				{
					ss << it->second->getName() << "/"
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

	flog << "========PRINT_BP========\n";
	flog << ss.str();
	flog << "========================\n\n";
}

void Manager::PRINT_ST() {}

void Manager::DELETE()
{
	if (stree->Delete())
		printSuccessCode("DELETE");
	else
		printErrorCode(700);
}

void Manager::printErrorCode(int n)
{
	flog << " ========ERROR========\n";
	flog << " " << n << "\n";
	flog << " =======================\n\n";
}

void Manager::printSuccessCode(string success)
{
	flog << "========" << success << "========\n";
	flog << " Success\n";
	flog << " =======================\n\n";
}
