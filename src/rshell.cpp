#include <string.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "redirect.h"
using namespace std;

//takes in 3 bools and 1 string 
//outputs the number for the closest connector
//sets one or none of the 3 connector bools
int connector(bool &next, bool &orr, bool &andd, string const cmd)
{
	int nexti = cmd.find(";");
	int orri = cmd.find("||");
	int anddi = cmd.find("&&");
	
	if (nexti == -1 && orri == -1 && anddi == -1)
	{
		return -1;
	}
	else if (nexti == -1 && orri >= 0 && anddi >= 0)
	{
		if (orri < anddi)
		{
			orr = true;
			return orri;
		}
		else
		{
			andd = true;
			return anddi;
		}
	}
	else if (nexti >= 0 && orri == -1 && anddi >= 0)
	{	
		if (nexti < anddi)
		{
			next = true;
			return nexti;
		}
		else
		{
			andd = true;
			return anddi;
		}
	}
	else if (nexti >= 0 && orri >= 0 && anddi == -1)
	{
		if (nexti < orri)
		{
			next = true;
			return nexti;
		}
		else 
		{
			orr = true;
			return orri;
		}
	}
	else if (nexti >= 0 && orri == -1 && anddi == -1)
	{
		next = true;
		return nexti;
	}
	else if (nexti == -1 && orri >= 0 && anddi == -1)
	{
		orr = true;
		return orri;
	}
	else if (nexti == -1 && orri == -1 && anddi >= 0)
	{
		andd = true;
		return anddi;
	}
	else
	{
		if (nexti < orri)
		{
			if (anddi < nexti)
			{
				andd = true;
				return anddi;
			}
			else
			{
				next = true;
				return nexti;
			}
		}
		else
		{
			if (anddi < orri)
			{
				andd = true;
				return anddi;
			}
			else 
			{
				orr = true;
				return orri;
			}
		}
	}
}
//will fork and run cmd
//will perror if system call faults
//returns -1 if unable to run command else 0
int execb (string in)
{
	bool redirio = false;
	if (in.find("|") != string::npos || 
		in.find("<") != string::npos ||
		in.find(">") != string::npos)
	{
		redirect::redir(in);
		redirio = true;
	}
	int status = 0;
	pid_t pid = fork();
		if (pid == -1)
			perror("failed to fork");
		else if (pid == 0)
		{
			char *arg[99];
			char *temp = strdup(in.c_str());
			arg[0] = strtok(temp, " ");
			char *token = arg[0];
			for (int i = 1; token != NULL; i++)
			{
				arg[i] = strtok(NULL, " ");
				token = arg[i];
			}
			int execs = execvp(arg[0], arg);
			if (execs == -1)
			{
				perror("cannot execute command");
				exit(1);
			}
			delete temp;
		}
		else 
		{
			int waits = waitpid(-1, &status, 0);
			if (waits == -1)
				perror("no child");
			if (status > 0)
			{
				return -1;
			}
		}
	if (redirio)
		redirect::ioend();
	return 0;
}
//will output true if program needs to be exited false otherwise
bool quit(string in)
{
	size_t start = in.find_first_not_of(" \t\f\v\r");
	if (start == string::npos) 
		return false;
	//removes leading white space
	string cmd = in.substr(start);
	
	size_t end = cmd.find_last_not_of(" \t\f\v\r");
	if (end == string::npos)
		return false;
	//removes ending white spaces 
	cmd = cmd.substr(0, end + 1);
	
	//this will exit even if exit is followed by other characters ie exitasdf or exit -a
	//string cmd = in.substr(start, 4);
	
	if (cmd  == "exit")
	{
		return true;
	}
	return false;
}
//"in" is portion of cmd that does not contain connectors
//"cmd" is everything the use inputs
int main(int argc, char **argv)
{
	string cmd;
	for(;;)
	{
		finish:char hostname[20];
		if (-1 == (gethostname(hostname, 20)))
			perror("get hostname fail");
		char* log;
		if (NULL == (log = getlogin()))
			perror("get login fail");
		cout << log << "@" << hostname;
		string in;
		cout << "$";
		getline(cin,in);
		size_t comment = in.find("#");
		if (comment != string::npos)
			in = in.substr(0, comment);
		bool next = false, orr = false, andd = false;
		int cut = connector(next, orr, andd, in);
		while (cut != -1)
		{	
			//if not ; then it is || or &&
			// ; = remove 1 form cmd
			// || or && = remove 2 from cmd
			if (next)
			{
				cmd = in.substr(0,cut);
				in = in.substr(cut+1, string::npos);
			}
			else
			{	
				cmd = in.substr(0,cut);
				in = in.substr(cut+2, string::npos);
			
			}
			if (orr)
			{
				if (quit(cmd))
					exit(0);
				if (execb(cmd) != -1)
					goto finish;
			}
			if (andd)
			{
				if (quit(cmd))
					exit(0);
				if (execb(cmd) == -1)
					goto finish;
			}
			if (next)
			{
				if (quit(cmd))
					exit(0);
				execb(cmd); //if (execb(cmd) == -1); no if run when success or fail
			}
			next = false;
			andd = false;
			orr = false;
			cut = connector(next, orr, andd, in);
		
		}
		if (quit(in))
		{
			return 0;
		}
		execb(in); //if (execb(in) == -1); if statement is empty does not matter if succecd or fail 
	}
	return 0;
}
