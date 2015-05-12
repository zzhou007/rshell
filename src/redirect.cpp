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
#include <sys/wait.h>

#include "redirect.h"
namespace redirect
{
	struct segment
	{
		std::string command;
		int fd[2];
	};
	std::vector<segment> v;
	int pipefd[2];
	bool ispipe = false;
	int herestrp[2];
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
			cmd = cmd.substr(0, end + 1);
		}
		s = cmd;
		
	}
	void removequotes(std::string &s)
	{
		std::string cmd;
		size_t start = s.find_first_not_of("\"\'");
		if (start != std::string::npos)
		{
			//removes leading white space
			cmd = s.substr(start);
		}
		size_t end = cmd.find_last_not_of("\"\'");
		if (end != std::string::npos)
		{
			//removes ending white spaces 
			cmd = cmd.substr(0, end + 1);
		}
		s = cmd;
		
	}

	//splits run to before seperator and after seperator
	//removes extra spaces
	//sets bool statement for in out and appen
	//sets run to lefthand side
	//return what is on the rhs 
	std::string seperaterio(bool &in, bool &out, bool &appen, bool &sin, std::string &run)
	{

		int p = run.find(">>");
		if (p == -1)
		{
			p = run.find(">");
			if (p == -1)
			{
				p = run.find("<<<");
				if (p == -1)
				{
					p = run.find("<");
					in = true;
				}
				else
				{
					sin = true;
				}
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
		else if (in || out)
			rhs = run.substr(p + 1);
		else 
			rhs = run.substr(p + 3);
		
		//remove space debug
		//std::cout << "rhs " << rhs << std::endl;
		removespace(rhs);
		run = run.substr(0,p);
		removespace(run);
		return rhs;

	}

	int iofdold = 0;
	bool in = false, out = false, appen = false,  sin = false;
	//run changes to the lhs
	void filtersin(std::string &in)
	{
		removespace(in);
		removequotes(in);
		in = in + "\n";

	}
	void iostart(std::string &run)
	{
		int iofdnew = 0;
		std::string rhs = seperaterio(in, out, appen, sin, run);
		//seperateio debug
		//std::cout << "rhs " << rhs << std::endl
		//	<< "in out appen " << in << out << appen << std::endl
		//	<< "run " << run << std::endl;
		//std::cout << "rhs " << rhs << "run " << run << std::endl;
		if (in)
		{
			if (-1 == (iofdold = dup(0)))
				perror("save fdold");
			if (-1 == close(0))
				perror("close 1");
			iofdnew = open(rhs.c_str(), O_RDONLY);
			if (iofdnew == -1)
				perror("open 1");
		}
		if (out)
		{
			if (-1 == (iofdold = dup(1)))
				perror("save fdold (1)");
			//std::cout << "vector error here" << std::endl;
			if (-1 == close(1))
				perror("close 2");
			//std::cout << "rhs " << rhs << std::endl;
			iofdnew = open(rhs.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			//std::cout << "vector error here2" << std::endl;
			if (iofdnew == -1)
				perror("open 2");
		}
		if (appen)
		{
			if (-1 == (iofdold = dup(1)))
				perror("save fd appen");
			if (-1 == close(1))
				perror("close 3");
			iofdnew = open(rhs.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			if (iofdnew == -1)
				perror("open 3");
		}
		if (sin)
		{
			//save old read
			if (-1 == (iofdold = dup(0)))
				perror("save sin failr");
			//to get program to read from string instad of file
			//make pipe
			if (-1 == pipe(herestrp))
				perror("here string");
			//filter string rhs first 
			filtersin(rhs);
			//write rhs to write end of pipe
			if (-1 == write(herestrp[1], rhs.c_str(), rhs.length()))
				perror("here str write");
			//close read end of stdio
			if (-1 == close(0))
				perror("close sin");
			//set read end to read end of pipe
			if (-1 == dup2(herestrp[0], 0))
				perror("dup read");
			//close extra pipe read end
			if (-1 == close(herestrp[0]))
				perror("close pipe");
			//close write end of pipe we are done
			if (-1 == close(herestrp[1]))
				perror("close write end of pipe");


		}
	}
	//rshell will call this and it well change everything back
	void ioend()
	{
		if (in)
		{
			if (-1 == close(0))
				perror("close 5");
			if (-1 == dup2(iofdold, 0))
				perror("dup2 1");
			if (-1 == close(iofdold))
				perror("close in");
		}
		if (out || appen)
		{
			if (-1 == close(1))
				perror("close 4");
			if (-1 == dup2(iofdold, 1))
				perror("dup2 2");
			if (-1 == close(iofdold))
				perror("close out/appen");
		}
		if (sin)
		{
			//close read end of std
			if (-1 == close(0))
				perror("close sin");
			//dup old fd to read end
			if (-1 == dup2(iofdold, 0))
				perror("dup old");
			//close pipe
			if (-1 == close(iofdold))
				perror("close extra");
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
	void pipestart(segment &s, bool first, bool last, segment &p)
	{
		//save std in and std out
		if (first)
		{
			if (-1 == (pipefd[0] = dup(0)))
				perror("save fd 0");	
			if (-1  == (pipefd[1] = dup(1)))
				perror("save fd 1");
		}
		if (!last)
		{
			//close write end of fd
			//set write end to write end of pipe
			if (-1 == dup2(s.fd[1], 1))
				perror("dup2 3");
			if (-1 == close(s.fd[1]))
				perror("close 11");
		}
		if (!first)
		{
			//close read end of fd
			//set read end to read end of previous pipe
			if (-1 == dup2(p.fd[0], 0))
				perror("dup2 5");
			if (-1 == close(p.fd[0]))
				perror("close 12");
		}
		if (last)
		{	
			if (-1 == dup2(pipefd[1], 1))
				perror("restore write end of pipe");
			if (-1 == close(s.fd[1]))
				perror("close 13");
		}

	}
	
	void redir(std::string &command, int currentp, bool &first, bool &last, bool &redirio)
	{
		//sets all bool values
		if (currentp == 0)
			first = true;
		else
			first = false;
		
		if (v.size() == 1 || (unsigned)currentp == v.size() - 1)
			last = true;
		else 
			last = false;
		//incase redirio was true on first run set to false
		redirio = false;
		segment temp;
		in = false;
		out = false;
		appen = false;
		sin = false;
		ispipe = true;
		std::string run;
		//only redirect io if first
		//no construct pipe vector if first
		if (first)
		{
			//no pipe just io
			if (!findpipe(command))
			{
				temp.command = command;
				temp.fd[0] = 0;
				temp.fd[1] = 0;
				v.push_back(temp);
				last = true;
				first = true;
				redirio = true;
				ispipe = false;
			}
			//this loop seperates command into pieces 
			//pieces are based off |
			else
			{
				ispipe = true;
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
				if (findio(v.at(0).command))
					redirio = true;
			}
		}
		//if it is first and is a pipe cannot send prev
		//send current pipe instead
		if (first && ispipe)
			pipestart(v.at(currentp), first, last, v.at(currentp));
		else if (ispipe)
			pipestart(v.at(currentp), first, last, v.at(currentp-1));

		//runs iostart that redirects input and output
		//if finds io
		//only for starting and ending pipes
		if (redirio && first)
		{
			iostart(v.at(0).command);
		}
		//there is a pipe and the last statement wants to
		//redirect io
		if (last && findio(v.at(currentp).command))
		{
			redirio = true;
			iostart(v.at(currentp).command);
		}
	
		//sets command for execvp to run 
		command = v.at(currentp).command;
		removespace(command);

	}

	void restoreallfd()
	{
		if (ispipe)
		{
			if (-1 == (dup2(pipefd[0], 0)))
				perror("restore pipe read");
			if (-1 == (dup2(pipefd[1], 1)))
				perror("restore pipe write");
			if (-1 == close(pipefd[0]))
				perror("restore fail");
			if (-1 == close(pipefd[1]))
				perror("restore fail");
		}
	}
	//clears vector at the end
	void clearvector()
	{
		v.clear();
	}
	void setispipe()
	{
		ispipe = false;
	}


}
