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

using namespace std;

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		ls::list();
	}
	else 
	{
		

	}
	return 0;
}
