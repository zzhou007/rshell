#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "redirect.h"
namespace redirect
{
	//splits command to before first pipe and after first pipe
	//returns before pipe
	//sets command to after pipe
	std::string seperatep(std::string &command)
	{
		int p = command.find("|");
		if (p == -1)
			return command;
		std::string tmp = command.substr(0,p);
		command = command.substr(p+1);
		return tmp;
	}
	//splits run to before seperator and after seperator
	//removes extra spaces
	//sets bool statement for in out and appen
	//sets run to lefthand side
	//return what is on the rhs 
	std::string seperaterio(bool &in, bool &out, bool &appen, std::string &run)
	{
		int p = run.find(">>");
		if (p == -1)
		{
			p = run.find(">");
			if (p == -1)
			{
				p = run.find("<");
				in = true;
			}
			else
			{
				out = true;
			}
		}
		else
		{
			appen = true;	
		}
		std::string rhs;
		if (appen)
			rhs = run.substr(p + 2);
		else
			rhs = run.substr(p + 1);
		
		std::string cmd;
		size_t start = rhs.find_first_not_of(" \t\f\v\r");
		if (start != std::string::npos)
		{
			//removes leading white space
			cmd = rhs.substr(start);
		}
		size_t end = cmd.find_last_not_of(" \t\f\v\r");
		if (end != std::string::npos)
		{
			//removes ending white spaces 
			cmd = cmd.substr(0, end + 1);
		}
		run = run.substr(0,p);
		return cmd;

	}
	int fdold = 0;
	bool in = false, out = false, appen = false;
	//run changes to the lhs
	void iostart(std::string &run)
	{
		int fdnew = 0;
		std::string rhs = seperaterio(in, out, appen, run);
		//std::cout << "rhs " << rhs << "run " << run << std::endl;
		if (in)
		{
			fdold = dup(0);
			if (-1 == close(0))
				perror("close 1");
			fdnew = open(rhs.c_str(), O_RDONLY);
			if (fdnew == -1)
				perror("open 1");
		}
		if (out)
		{
			fdold = dup(1);
			if (-1 == close(1))
				perror("close 2");
			fdnew = open(rhs.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if (fdnew == -1)
				perror("open 2");
		}
		if (appen)
		{
			fdold = dup(1);
			if (-1 == close(1))
				perror("close 3");
			fdnew = open(rhs.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			if (fdnew == -1)
				perror("open 3");
		}
	}
	void ioend()
	{
		if (in)
		{
			if (-1 == close(0))
				perror("close 5");
			if (-1 == dup2(fdold, 0))
				perror("dup2 1");
		}
		if (out || appen)
		{
			if (-1 == close(1))
				perror("close 4");
			if (-1 == dup2(fdold, 1))
				perror("dup2 2");
		}
	}
	bool findio(std::string run)
	{	
		if (run.find(">") != std::string::npos)
		{
			return true;
		}
		if (run.find("<") != std::string::npos)
			return true;
		return false;
	}
	bool findpipe(std::string run)
	{
		if (run.find("|") != std::string::npos)
			return true;
		return false;
	}
	void redir(std::string &command)
	{
		std::vector<std::string> v;
		in = false;
		out = false;
		appen = false;
		std::string run;
		do
		{
			run = seperatep(command);
			v.push_back(run);
			//debug
			std::cout << run;
		}
		while (findpipe(run));
		iostart(v.at(0));
		std::cout << v.at(0);
		command = v.at(0);

	}


}
