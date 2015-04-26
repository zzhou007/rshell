#include <sys/types.h>
#include <algorithm>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "ls.h"
namespace ls
{
	DIR *dir;
	struct stat s;
	struct dirent *dirfiles;
	
	void list(std::string file)
	{
		//just print file if not dir
		if (-1 == stat(file.c_str(), &s))
		{
			perror("stat error");
			exit(1);
		}
		else
		{
			if ((s.st_mode & S_IFMT) == S_IFREG)
				std::cout << file << std::endl;
			return;
		}
		//goes to dir and prints out content
		std::vector<std::string> output;
		if (NULL == (dir = opendir(file.c_str())))
		{
			perror("no such directory.");
		}

		errno = 0;
		while(NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name.find(".") != 0)
				output.push_back(name);
		}
		if (errno != 0)
		{
			perror("directory read error");
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
		}
		std::sort(output.begin(), output.end());
		for (size_t i = 0; i < output.size(); i++)
			std::cout << output.at(i) << "  ";
		std::cout << std::endl;
	}

	void listall(std::string file)
	{
		//just print file if not dir
		if (-1 == stat(file.c_str(), &s))
		{
			perror("stat error");
			exit(1);
		}
		else
		{
			if ((s.st_mode & S_IFMT) == S_IFREG)
				std::cout << file << std::endl;
			return;
		}
		//goes to dir and prints out content 
		std::vector<std::string> output;
		if (NULL == (dir = opendir(file.c_str())))
		{
			perror("no such directory.");
		}
		errno = 0;
		while(NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			output.push_back(name);
		}
		if (errno != 0)
		{
		perror("directory read error");
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
		}
		std::sort(output.begin(), output.end());
		for (size_t i = 0; i < output.size(); i++)
			std::cout << output.at(i) << "  ";
		std::cout << std::endl;
	}

	void listrec(std::string file)
	{
		//just print file if not dir
		if (-1 == stat(file.c_str(), &s))
		{
			perror("stat error");
			exit(1);
		}
		else
		{
			if ((s.st_mode & S_IFMT) == S_IFREG)
				std::cout << file << std::endl;
			return;
		}
		//prints all files rec
		std::vector<std::string> output;
		if (NULL == (dir = opendir(file.c_str())))
		{
			perror("no such directory.");
		}

		errno = 0;
		while(NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name.find(".") != 0)
				output.push_back(name);
		}
		if (errno != 0)
		{
			perror("directory read error");
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
		}
		std::sort(output.begin(), output.end());
		for (size_t i = 0; i < output.size(); i++)
			std::cout << output.at(i) << "  ";
		std::cout << std::endl;

	}

}


