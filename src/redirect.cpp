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

	struct segment
	{
		std::string command;
		int fd[2];
		int oldfd[2];
	};
	std::vector<segment> v;

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
	//removes starting and trailing spaces
	void removespace(std::string &s)
	{
		std::string cmd;
		size_t start = s.find_first_not_of(" \t\f\v\r");
		if (start != std::string::npos)
		{
			//removes leading white space
			cmd = s.substr(start);
		}
		size_t end = cmd.find_last_not_of(" \t\f\v\r");
		if (end != std::string::npos)
		{
			//removes ending white spaces 
			cmd = s.substr(0, end + 1);
		}
		s = cmd;
		
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
		
		removespace(rhs);
		run = run.substr(0,p);
		removespace(run);
		return rhs;

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
			if (-1 == (fdold = dup(0)))
				perror("save fdold");
			if (-1 == close(0))
				perror("close 1");
			fdnew = open(rhs.c_str(), O_RDONLY);
			if (fdnew == -1)
				perror("open 1");
		}
		if (out)
		{
			if (-1 == (fdold = dup(1)))
				perror("save fdold (1)");
			if (-1 == close(1))
				perror("close 2");
			fdnew = open(rhs.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if (fdnew == -1)
				perror("open 2");
		}
		if (appen)
		{
			if (-1 == (fdold = dup(1)))
				perror("save fd appen");
			if (-1 == close(1))
				perror("close 3");
			fdnew = open(rhs.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			if (fdnew == -1)
				perror("open 3");
		}
	}
	//rshell will call this and it well change everything back
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
	void pipestart(segment &s, bool first, segment &p)
	{
		
		if (-1 == (s.oldfd[0] = dup(0)))
			perror("save fd 0");	
		if (-1  == (s.oldfd[1] = dup(1)))
			perror("save fd 1");
		if (-1 == close(1))
			perror("close 5");
		if (-1 == dup2(s.fd[1], 1))
			perror("dup2 3");
		if (!first)
		{
			if (-1 == close(0))
				perror("close 7");
			if (-1 == dup2(p.fd[0], 0))
				perror("dup2 5");
	
		}

	}
	
	void redir(std::string &command, int currentp, bool &first, bool &last, bool &redirio)
	{
		first = false;
		last = false;
		redirio = false;
		segment temp;
		in = false;
		out = false;
		appen = false;
		std::string run;
		//this loop seperates command into pieces 
		//pieces are based off |
		do
		{
			run = seperatep(command);
			temp.command = run;
			//make pipe for vector
			if (-1 == pipe(temp.fd))
			{
				perror("pipe 1");
				exit(1);
			}
			//push onto vector
			v.push_back(temp);

		}
		while (findpipe(command));
		temp.command = command;
		if (-1 == pipe(temp.fd))
		{
			perror("pipe 2");
			exit(1);
		}
		v.push_back(temp);
		
		//debug
		for (size_t i = 0; i < v.size(); i++)
			std::cout << "v at " << i << " " 
			<< "command " << v.at(i).command 
			<< " pipe f[0] " << v.at(i).fd[0]
			<< " pipe f[1] " << v.at(i).fd[1]
			<< std::endl;
		//end debug
		
		//runs iostart that redirects input and output
		//if finds io
		//only for starting and ending pipes
		if (findio(v.at(0).command))
		{
			redirio = true;
			iostart(v.at(0).command);
		}
		if (findio(v.at(v.size() - 1).command))
		{
			redirio = true;
			iostart(v.at(v.size() -1).command);
		}
		//sets first and last pipe;
		std::cout << (unsigned)currentp << std::endl;
		std::cout << v.size() - 1 << std::endl;
		if (currentp == 0)
			first = true;
		if (v.size() == 1 || (unsigned)currentp == v.size() - 1)
			last = true;

		//changes fd for current pipe
		if (first)
			pipestart(v.at(currentp), first, v.at(currentp));
		else
			pipestart(v.at(currentp), first, v.at(currentp-1));

		
		//sets command for execvp to run 
		command = v.at(currentp).command;


	}
	void restoreallfd()
	{
		for(size_t i; i < v.size(); i++)
		{
			close(v.at(i).fd[0]);
			dup2(v.at(i).oldfd[0], 0);
			close(v.at(i).fd[1]);
			dup2(v.at(i).oldfd[1], 1);
		}
	}


}
