#include "Manager.h"
#include "EmployeeData.h"
#include "BpTreeIndexNode.h"
#include "BpTreeDataNode.h"
#include <sstream>

using namespace std;

void Manager::run(const char *command)
{
	// open log
	flog.open("log.txt", ios::out | ios::trunc);
	if (!flog.is_open())
		return;

	// open command file with member 'fin'
	fin.open(command);
	if (!fin.is_open())
	{
		printErrorCode(100);
		return;
	}

	// make trees
	bptree = new BpTree(&flog);
	stree = new SelectionTree(&flog);

	string cmd;
	while (fin >> cmd)
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
			// use function that reads from fin
			ADD_BP();
		}
		else if (cmd == "SEARCH_BP")
		{
			string a, b;
			if (fin >> a)
			{
				// try range (two tokens). if not two, fallback to single name
				streampos p = fin.tellg();
				if (fin >> b)
				{
					SEARCH_BP_RANGE(a, b);
				}
				else
				{
					fin.clear();
					fin.seekg(p);
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
			if (fin >> kind)
			{
				if (kind == "dept_no")
				{
					int d;
					if (fin >> d)
						ADD_ST_DEPTNO(d);
					else
						printErrorCode(500);
				}
				else if (kind == "name")
				{
					string n;
					if (fin >> n)
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
			if (fin >> dept_no)
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
		else if (cmd == "DEBUG_PRINT_TREE") // debug
		{
			if (bptree)
			{
				bptree->debugPrintTree();
			}
		}

		else
		{
			printErrorCode(800);
		}
	}

	fin.close();

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

void Manager::ADD_BP()
{
	// read from fin
	string name;
	int dept, id, income;

	if (fin >> name >> dept >> id >> income)
	{
		EmployeeData *e = new EmployeeData;
		e->setData(name, dept, id, income);

		bool ok = bptree->Insert(e);
		if (ok)
		{
			flog << "========ADD_BP========\n";

			// find inserted or updated data
			EmployeeData *p = nullptr;
			BpTreeNode *leaf = bptree->searchDataNode(name);
			if (leaf != NULL)
			{
				map<string, EmployeeData *> *dm = leaf->getDataMap();
				if (dm != NULL)
				{
					map<string, EmployeeData *>::iterator it = dm->find(name);
					if (it != dm->end() && it->second != NULL)
					{
						p = it->second;
					}
				}
			}
			if (p == NULL)
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
		fin.clear(); // clear fail state
	}
}

void Manager::SEARCH_BP_NAME(string name)
{

	BpTreeNode *leaf = bptree->searchDataNode(name);
	if (!leaf)
	{
		printErrorCode(300);
		return;
	}

	map<string, EmployeeData *> *dm = leaf->getDataMap();
	if (!dm)
	{
		printErrorCode(300);
		return;
	}

	map<string, EmployeeData *>::iterator it = dm->find(name);
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
		map<string, EmployeeData *> *dm = cur->getDataMap();
		if (dm != NULL)
		{
			for (map<string, EmployeeData *>::iterator it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second != NULL)
				{
					if (!(it->first < start) && !(it->first > end))
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
		cur = cur->getNext();
		if (cur != NULL)
		{
			map<string, EmployeeData *> *dm2 = cur->getDataMap();
			if (dm2 != NULL && !dm2->empty())
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
		map<string, EmployeeData *> *dm = cur->getDataMap();
		if (dm != NULL)
		{
			for (map<string, EmployeeData *>::iterator it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second != NULL)
				{
					if (it->second->getDeptNo() == dept_no)
					{
						if (stree->Insert(it->second))
							any = true;
					}
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

	map<string, EmployeeData *> *dm = leaf->getDataMap();
	if (!dm)
	{
		printErrorCode(500);
		return;
	}

	map<string, EmployeeData *>::iterator it = dm->find(name);
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
		map<string, EmployeeData *> *dm = cur->getDataMap();
		if (dm != NULL)
		{
			for (map<string, EmployeeData *>::iterator it = dm->begin(); it != dm->end(); ++it)
			{
				if (it->second != NULL)
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

void Manager::PRINT_ST() {} // not used (PRINT_ST is handled in run())

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
