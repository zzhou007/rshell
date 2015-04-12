#include <string.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main(int argc, char **argv)
{
	string in;
	int waits;
	for(;;)
	{
		cout << "$";
		getline(cin,in);
		pid_t pid = fork();
		if (pid == -1)
			perror("failed to fork");
		else if (pid == 0)
		{
			char *arg[in.length()];
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
				perror("cannot execute command");
		}
		else 
		{
			
			waits = wait(NULL);
			if (waits == -1)
				perror("no child");
		}
	}
	return 0;
}
