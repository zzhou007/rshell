#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

#include "ls.h"
namespace ls
{
	DIR *dir;
	struct stat s;
	struct dirent *dirfiles;
	
	void list()
	{
		if (NULL == (dir = opendir(".")))
			{
				perror("no such directory.");
				exit(1);
			}
			errno = 0;
			while(NULL != (dirfiles = readdir(dir)))
			{
				std::string name = dirfiles->d_name;
				if (name.find(".") != 0)
					std::cout << name << " ";
			}
			if (errno != 0)
			{
				perror("directory read error");
				exit(1);
			}
			std::cout << std::endl;
			if (-1 == closedir(dir))
			{
				perror("close directory error");
				exit(1);
			}
	}
}


