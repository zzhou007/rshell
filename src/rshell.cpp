#include <string.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "redirect.h"
#include <limits.h>
using namespace std;

//global variable for signal handler
pid_t pid = 1;
//global variable for cd -
bool noprint;
bool oldpwd = false;
bool nostop = false;
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
//prints the starting prompt
void printprompt()
{
	char hostname[20];
	if (-1 == (gethostname(hostname, 20)))
		perror("get hostname fail");
	char* log;
	if (NULL == (log = getlogin()))
		perror("get login fail");
	cerr << log << "@" << hostname;
	cerr << ":";
	char* workingdir;
	if (NULL == (workingdir = getenv("PWD")))
	{
		perror("getenv");
		exit(1);
	}
	char* home;
	if (NULL == (home = getenv("HOME")))
	{
		perror("getenv home");
		exit(1);
	}
	string workingdirs = workingdir;
	string homes = home;
	if (homes == workingdirs.substr(0, homes.length()))
	{
		workingdirs = workingdirs.substr(homes.length());
		cerr << "~" +  workingdirs + "$ ";
	}
	else
		cerr << workingdirs + "$ ";
}
//handles the ctrl + c signal
//does noting for parent
//kills child
void killhandle(int signum)
{
	cout << endl;
	printprompt();
	noprint = true;

}
void stophandle(int signum)
{
	if (pid != 0)
	{
		kill(pid, SIGSTOP);
		nostop = true;
	}
}
//checks if command is cd dir 
string checkdir(string in, bool &cd, bool &cdp, bool &cdd)
{
	//remove beginning and ending spaces
	redirect::removespace(in);
	//if cd command must have cd at beginning 
	int i = in.find("cd");
	if (i == 0)
	{
		//delete cd from beginning
		in.erase(0,2);
		//must have space after if not just return 
		string s = in.substr(0,1);
		if (!(s == " ") && !(s.empty()))
			return "";
		//removes all spaces after
		redirect::removespace(in);
		//if empty command is jsut cd
		if (in.length() == 0)
		{
			cd = true;
			return "";	
		}
		//if has dash command is just dash
		if (in.compare("-") == 0)
		{
			cdd = true;
			return "-";
		}
		//only thing left is path
		cdp = true;
		return in;
	}
	return "";


}
//if command is cd dir runs cd command
//else exits 
//return 1 if success
//-1 if fail
//0 if not cd command
int changedir(string in)
{
	bool cd = false, cdp = false, cdd = false;
	string path = checkdir(in, cd, cdp, cdd);
	//checkdir returns path and sets on bool flag
	if (cd || cdp || cdd)
	{
		//if just cd by itself go to home
		if (cd)
		{
			//make tmp char*
			char* tmp;
			//set tmp to current pwd
			if (NULL == (tmp = getenv("PWD")))
			{
				perror("getenv");
				exit(1);
			}
			//set oldpwd to tmp
			if (-1 == (setenv("OLDPWD", tmp, 1)))
			{
				perror("set old fail");
				exit(1);
			}
			//set tmp to home
			if (NULL == (tmp = getenv("HOME")))
			{
				perror("getenv home");
				exit(1);
			}
			//set pwd to home
			if (-1 == (setenv("PWD",tmp,1)))
			{
				perror("set env cd");
				exit(1);
			}
			//change dir to home
			if (-1 == chdir(tmp))
			{
				perror("cd");
				exit(1);
			}
			oldpwd = true;
		}
		else if (cdp)//for cd path
		{
			//make tmp char*
			char* tmp;
			//set tmp to current pwd
			if (NULL == (tmp = getenv("PWD")))
			{
				perror("getenv");
				exit(1);
			}
			string pathl = tmp;
			if (path.substr(0,1) != "/")
			{
				if (path.substr(0,2) == "..")
				{
					if (pathl == "/")
					{
						path = pathl;
					}
					else
					{
						unsigned split = pathl.find_last_of("/");
						path = pathl.substr(0,split);
					}
				}
				else if (path.substr(0,1) == ".")
				{
					path = pathl;
				}
				else
				{
					path = pathl + "/" + path;
				}
			}
			//set oldpwd to tmp
			if (-1 == setenv("OLDPWD", tmp, 1))
			{
				perror("set old fail");
				exit(1);
			}
			//set pwd to curr path
			if (-1 == (setenv("PWD",path.c_str(),1)))
			{
				perror("set env cd");
				exit(1);
			}
			//change dir to path
			if (-1 == chdir(path.c_str()))
			{
				perror("cd");
				exit(1);
			}
			oldpwd = true;
		}
		else //for cd -
		{
			if (!oldpwd)
			{
				cout << "no old pwd" << endl;
				return -1;
			}
			//make tmp char*
			char* tmp;
			//set tmp to current pwd
			if (NULL == (tmp = getenv("PWD")))
			{
				perror("getenv");
				exit(1);
			}
			char *olddir;
			if (NULL == (olddir = getenv("OLDPWD")))
			{
				perror("get env");
				exit(1);
			}
			//set pwd to old dir
			if (-1 == (setenv("PWD",olddir,1)))
			{
				perror("set env cd");
				exit(1);
			}
			//change dir to old pwd
			if (-1 == chdir(olddir))
			{
				perror("cd");
				exit(1);
			}
			//set old pwd to current pwd
			if (-1 == (setenv("OLDPWD", tmp, 1)))
			{
				perror("cd");
				exit(1);
			}
			
		}

		return 1;
	}
	return 0;
}
//will fork and run cmd
//will perror if system call faults
//returns -1 if unable to run command else 0
int execb (string in)
{
	if (in == "bg")
	{
		if (pid != 0 && pid != 1)
		{
			kill(pid, SIGCONT);
			pid = 0;
			nostop = false;
		}
		else
		{
			cout << "no process" << endl;
		}
		return 0;
	}
	else if (in == "fg")
	{
		if (pid != 0 && pid != 1)
		{
			kill(pid, SIGCONT);
			nostop = false;
		}
		else
		{
			cout << "no process" << endl;
		}
		return 0;
	}
	//function to handle cd
	//returns true if in is cd
	//exits with 0 if it is cd
	int changestatus = changedir(in);
	if (changestatus == 1)
		return 0;
	else if (changestatus == -1)
		return -1;

	//at beginning 
	int currentp = 0;
	int status = 0;
	bool first = true, last = true;
	bool redirio = false;
	do
	{
		if (!first || !last || redirio ||
			in.find("|") != string::npos || 
			in.find("<") != string::npos ||
			in.find(">") != string::npos)
		{
			redirect::redir(in, currentp, first, last, redirio);
		}
		else
		{
			redirect::setispipe();
		}
		char **arg = new char*[in.length()];
		char *temp = strdup(in.c_str());
		int i;

		pid = fork();
		if (pid == -1)
		{
			perror("failed to fork");
			exit(1);
		}
		else if (pid == 0) //child
		{
			arg[0] = strtok(temp, " ");
			char *token = arg[0];
			for (i = 1; token != NULL; i++)
			{
				arg[i] = strtok(NULL, " ") + NULL;
				token = arg[i];
			}
			for (; (unsigned)i < in.length(); i ++)
			{
				arg[i] = NULL;
			}
			int execs = execvp(arg[0], arg);
			if (execs == -1)
			{
				perror("cannot execute command");
				exit(1);
			}
		}//parent 
		currentp++;
		if (redirio)
			redirect::ioend();

		delete arg;
		delete temp;
	}
	while(!last);
	if (!nostop)
	{
		cout << "hi";
		int waits;
		while ((waits = wait(&status)) > 0)
			if (waits < -1)
			{
				perror("no child");
				exit(1);
			}
	}
	redirect::restoreallfd();
	redirect::clearvector();
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
	noprint = false;
	//sigk handles the kill signal sends to killhandle
	struct sigaction sigk;
	sigk.sa_handler = killhandle;
	sigemptyset(&sigk.sa_mask);
	sigk.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sigk, NULL) == -1)
		perror("signal kill fail");
	
	//sigs handles the stop signal
	struct sigaction sigs;
	sigs.sa_handler = stophandle;
	sigemptyset(&sigs.sa_mask);
	sigk.sa_flags = SA_RESTART;
	if (sigaction(SIGTSTP, &sigs, NULL) == -1)
		perror("signal stop  fail");
	
	string cmd;
	for(;;)
	{
		finish:if (!noprint)
			printprompt();
		string in;		
		getline(cin,in);
		noprint = false;
		//flush getline
		cin.clear();
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
