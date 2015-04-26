#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include "ls.h"
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char** argv)
{
	//just ls nothing else
	if (argc <= 1)
	{
		ls::list(".");
	}
	//has something else
	else
	{
		//seperates flags and files/dir and pushes them to seperate vectors
		vector<string> filesdir;
		vector<string> flags;
		for (int i = 1; i < argc; i++)
		{
			string s = argv[i];
			if (s.find("-") == 0)
		{
				if (s.size() == 1)
					filesdir.push_back(s);
				flags.push_back(s.substr(1,s.size()));
			}
			else
			{
				filesdir.push_back(s);
			}
		}
		sort(filesdir.begin(), filesdir.end());
		//loops through flags vector and sets bool (a, R, l) 
		//exits if flags not (a, R, l)
		bool a = false;
		bool R = false;
		bool l = false;
		for (size_t i = 0; i < flags.size(); i++)
		{
			string s;
			while (!flags.at(i).empty())
			{
				s = flags.at(i).substr(0,1);
				if (s == "a")
					a = true;
				else if (s == "R")
					R = true;
				else if (s == "l")
					l = true;
				else
				{
					cout << "ls: invalid option -- '" << s << "'" << endl;
					exit(1);
				}
				flags.at(i).erase(0,1);

			}
		}
		if (!a && !R && !l)
		{
			if (filesdir.size() > 1)
				for (size_t i = 0; i < filesdir.size(); i++)
				{
					cout << filesdir.at(i) << ":" << endl;
					ls::list(filesdir.at(i));
					if (i != filesdir.size() -1)
						cout << endl;
				}
			else
				ls::list(filesdir.at(0));
		}
		if (a && !R && !l)
		{	
			if (filesdir.size() > 1)
				for (size_t i = 0; i < filesdir.size(); i++)
				{
					cout << filesdir.at(i) << ":" << endl;
					ls::listall(filesdir.at(i));
					if (i != filesdir.size() -1)
						cout << endl;
				}
			else if (filesdir.size() == 0)
				ls::listall(".");
			else
				ls::listall(filesdir.at(0));
		}
	}
	return 0;
}
