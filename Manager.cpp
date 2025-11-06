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

	// open command file
	fin.open(command);
	if (!fin.is_open())
	{
		printErrorCode(100);
		return;
	}

	// create trees
	bptree = new BpTree(&flog);
	stree = new SelectionTree(&flog);

	string line;
	while (std::getline(fin, line))
	{
		// trim
		auto ltrim = [](string &s)
		{
			size_t p = s.find_first_not_of(" \t\r\n");
			if (p == string::npos)
			{
				s.clear();
				return;
			}
			s.erase(0, p);
		};
		auto rtrim = [](string &s)
		{
			size_t p = s.find_last_not_of(" \t\r\n");
			if (p == string::npos)
			{
				s.clear();
				return;
			}
			s.erase(p + 1);
		};
		ltrim(line);
		rtrim(line);
		if (line.empty())
			continue;

		// tokenize
		std::istringstream iss(line);
		std::vector<std::string> tok;
		for (std::string t; iss >> t;)
			tok.push_back(t);
		if (tok.empty())
			continue;

		const std::string &cmd = tok[0];

		// EXIT (no args)
		if (cmd == "EXIT")
		{
			if (tok.size() != 1)
			{
				printErrorCode(800);
				continue;
			}
			printSuccessCode("EXIT");
			break; // loop out, then free trees
		}
		// LOAD (no args)
		else if (cmd == "LOAD")
		{
			if (tok.size() != 1)
			{
				printErrorCode(100);
				continue;
			}
			LOAD();
		}

		// ADD BP
		else if (cmd == "ADD_BP")
		{

			if (tok.size() == 5)
			{
				const std::string &name = tok[1];
				int dept = 0, id = 0, income = 0;
				std::istringstream a(tok[2]), b(tok[3]), c(tok[4]);

				if (!(a >> dept) || !(b >> id) || !(c >> income))
				{
					printErrorCode(200);
					continue;
				}

				ADD_BP(name, dept, id, income);
			}
			else
			{

				printErrorCode(200);
			}
		}

		// SEARCH_BP name  SEARCH_BP start end
		else if (cmd == "SEARCH_BP")
		{
			if (tok.size() == 2)
			{
				SEARCH_BP_NAME(tok[1]);
			}
			else if (tok.size() == 3)
			{
				SEARCH_BP_RANGE(tok[1], tok[2]);
			}
			else
			{
				printErrorCode(300);
			}
		}
		// PRINT_BP (no args)
		else if (cmd == "PRINT_BP")
		{
			if (tok.size() != 1)
			{
				printErrorCode(400);
				continue;
			}
			PRINT_BP();
		}
		// ADD_ST dept_no d   |   ADD_ST name n
		else if (cmd == "ADD_ST")
		{
			if (tok.size() != 3)
			{
				printErrorCode(500);
				continue;
			}
			if (tok[1] == "dept_no")
			{
				int d = 0;
				std::istringstream v(tok[2]);
				if (!(v >> d))
				{
					printErrorCode(500);
					continue;
				}
				ADD_ST_DEPTNO(d);
			}
			else if (tok[1] == "name")
			{
				ADD_ST_NAME(tok[2]);
			}
			else
			{
				printErrorCode(500);
			}
		}
		// PRINT_ST dept_no
		else if (cmd == "PRINT_ST")
		{
			if (tok.size() == 2)
			{
				int d = 0;
				std::istringstream v(tok[1]);
				if (!(v >> d))
				{
					printErrorCode(600);
					continue;
				}
				if (!stree->printEmployeeData(d))
					printErrorCode(600);
			}
			else if (tok.size() == 1)
			{
				PRINT_ST();
			}
			else
			{
				printErrorCode(600);
			}
		}
		// DELETE (no args)
		else if (cmd == "DELETE")
		{
			if (tok.size() != 1)
			{
				printErrorCode(700);
				continue;
			}
			if (stree->Delete())
				printSuccessCode("DELETE");
			else
				printErrorCode(700);
		}
		// unknown
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

void Manager::ADD_BP(const string &name, int dept, int id, int income)
{
	EmployeeData *e = new EmployeeData;
	e->setData(name, dept, id, income);

	bool ok = bptree->Insert(e);
	if (ok)
	{
		flog << "========ADD_BP========\n";

		// find data
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
	bptree->searchRange(start, end);
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

void Manager::PRINT_ST()
{
	// read dept_no
	int dept_no;

	if (fin >> dept_no)
	{
		// print by dept_no
		bool ok = stree->printEmployeeData(dept_no);
		if (!ok)
		{
			printErrorCode(600);
		}
	}
	else
	{
		printErrorCode(600);
		fin.clear();
	}
}

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