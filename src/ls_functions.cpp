#include <sys/types.h>
#include <string>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <iomanip>
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
			{
				std::cout << file << std::endl;
				return;
			}
		}
		//goes to dir and prints out content
		std::vector<std::string> output;
		if (NULL == (dir = opendir(file.c_str())))
			perror("no such directory.");
		errno = 0;
		while(NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name.find(".") != 0)
				output.push_back(name);
		}
		if (errno != 0)
			perror("directory read error");
		if (-1 == closedir(dir))
			perror("close directory error");
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
			{
				std::cout << file << std::endl;
				return;
			}
		
		}
		//goes to dir and prints out content 
		std::vector<std::string> output;
		if (NULL == (dir = opendir(file.c_str())))
			perror("no such directory.");
		errno = 0;
		while(NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			output.push_back(name);
		}
		if (errno != 0)
			perror("directory read error");
		if (-1 == closedir(dir))
			perror("close directory error");
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
			{
				std::cout << file << std::endl;
				return;
			}
		
		}
		//list files/dir in cur dir
		std::string newdir = file;
		std::cout << newdir << ":" << std::endl;
		std::vector<std::string> dirpath;
		list(newdir);
		std::cout << std::endl;
		if (NULL == (dir = opendir(file.c_str())))
			perror("no such directory.");
		errno = 0;
		while (NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name.find(".") != 0)
				dirpath.push_back(name);
		}
		if (errno != 0)
			perror("directory read error");
		if (-1 == closedir(dir))
			perror("close directory error");
		std::sort(dirpath.begin(), dirpath.end());
		for (size_t i = 0; i < dirpath.size(); i++)
		{
			std::string path = file + "/" + dirpath.at(i);
			if (stat(path.c_str(), &s) == -1)
				perror("stat error");
			if ((s.st_mode & S_IFMT) == S_IFDIR)
				listrec(path);
		}
	}

	void listrecall(std::string file)
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
			{
				std::cout << file << std::endl;
				return;
			}
		
		}
		//list all files/dir in cur dir
		std::string newdir = file;
		std::cout << newdir << ":" << std::endl;
		std::vector<std::string> dirpath;
		listall(newdir);
		std::cout << std::endl;
		if (NULL == (dir = opendir(file.c_str())))
			perror("no such directory.");
		errno = 0;
		while (NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name != "." && name != "..") 
				dirpath.push_back(name);
		}
		if (errno != 0)
			perror("directory read error");
		if (-1 == closedir(dir))
			perror("close directory error");
		std::sort(dirpath.begin(), dirpath.end());
		for (size_t i = 0; i < dirpath.size(); i++)
		{
			std::string path = file + "/" + dirpath.at(i);
			if (stat(path.c_str(), &s) == -1)
				perror("stat error");
			if ((s.st_mode & S_IFMT) == S_IFDIR)
				listrecall(path);
		}
	}
	//all the info needed to output for ls -l
	struct fileinfo
	{
		std::string permissions;
		std::string user;
		std::string group;
		std::string name;
		std::string time_m;
		std::string time_d;
		std::string time_time;
		int hardlinks;
		int size;
	};
	//comparison to sort struct by file name	
	bool sortalpha(const fileinfo &name1, const fileinfo &name2)
	{
		return name1.name < name2.name;
	}

	unsigned totalspace;

	unsigned longestsize;

	unsigned longestlink;

	void listextra(std::string file)
	{	
		if (stat(file.c_str(), &s) == -1)
			perror("stat error");
		if ((s.st_mode & S_IFMT) == S_IFREG)
		{
			fileinfo f;
			f.name = file;
			f.permissions += "-";
			//whare are the user group and other permissions 
			(s.st_mode & S_IRUSR) ? (f.permissions += "r") : (f.permissions += "-");
			(s.st_mode & S_IWUSR) ? (f.permissions += "w") : (f.permissions += "-");
			(s.st_mode & S_IXUSR) ? (f.permissions += "x") : (f.permissions += "-");
			(s.st_mode & S_IRGRP) ? (f.permissions += "r") : (f.permissions += "-");
			(s.st_mode & S_IWGRP) ? (f.permissions += "w") : (f.permissions += "-");
			(s.st_mode & S_IXGRP) ? (f.permissions += "x") : (f.permissions += "-");
			(s.st_mode & S_IROTH) ? (f.permissions += "r") : (f.permissions += "-");
			(s.st_mode & S_IWOTH) ? (f.permissions += "w") : (f.permissions += "-");
			(s.st_mode & S_IXOTH) ? (f.permissions += "x") : (f.permissions += "-");
			
			//hard links
			f.hardlinks = s.st_nlink;

			//size
			f.size = s.st_size;

			//get group name and username
			passwd *userid;
			group *groupid;
			if (NULL == (userid = getpwuid(s.st_uid)))
				perror("userid fail");
			if (NULL == (groupid = getgrgid(s.st_gid)))
				perror("group id fail");
			f.user = userid->pw_name;
			f.group = groupid->gr_name;

			//time last mod
			char buffer [80];
			struct tm* timeinfo = localtime(&s.st_mtime); 
			strftime(buffer, 80, "%b", timeinfo);
			f.time_m = buffer;
			strftime(buffer, 80, "%d", timeinfo);
			f.time_d = buffer;
			strftime(buffer, 80, "%R", timeinfo);
			f.time_time = buffer;

			std::cout << f.permissions << " "
				<< f.hardlinks << " "
				<< f.user << " "
				<< f.group << " "
				<< f.size << " "
				<< f.time_m << " "
				<< f.time_d << " "
				<< f.time_time << " "
				<< f.name
				<< std::endl;
		}
		else
		{
			std::vector<fileinfo> fileinfolist;
			if (NULL == (dir = opendir(file.c_str())))
				perror("no such dirctory.");
			errno = 0;
			while (NULL != (dirfiles = readdir(dir)))
			{
				fileinfo f;
				//what is the file name
				std::string temp = dirfiles->d_name;
				if (temp.find(".") != 0)
				{	
					f.name = temp;
					temp = file + "/" + temp;
					if (stat(temp.c_str(), &s) == -1)
						perror("stat error");
					//what type of file is it? stores in permissions
					if ((s.st_mode & S_IFMT) == S_IFDIR)
						f.permissions += "d";
					else if ((s.st_mode & S_IFMT) == S_IFREG)
						f.permissions += "-";
					else if ((s.st_mode & S_IFMT) == S_IFLNK)
						f.permissions += "l";
					else if ((s.st_mode & S_IFMT) == S_IFCHR)
						f.permissions += "c";
					else if ((s.st_mode & S_IFMT) == S_IFBLK)
						f.permissions += "b";
					
					//whare are the user group and other permissions 
					(s.st_mode & S_IRUSR) ? (f.permissions += "r") : (f.permissions += "-");
					(s.st_mode & S_IWUSR) ? (f.permissions += "w") : (f.permissions += "-");
					(s.st_mode & S_IXUSR) ? (f.permissions += "x") : (f.permissions += "-");
					(s.st_mode & S_IRGRP) ? (f.permissions += "r") : (f.permissions += "-");
					(s.st_mode & S_IWGRP) ? (f.permissions += "w") : (f.permissions += "-");
					(s.st_mode & S_IXGRP) ? (f.permissions += "x") : (f.permissions += "-");
					(s.st_mode & S_IROTH) ? (f.permissions += "r") : (f.permissions += "-");
					(s.st_mode & S_IWOTH) ? (f.permissions += "w") : (f.permissions += "-");
					(s.st_mode & S_IXOTH) ? (f.permissions += "x") : (f.permissions += "-");
					
					//hard links
					f.hardlinks = s.st_nlink;
					unsigned temp_link = 0;
					for (size_t i = s.st_nlink; i > 0; i/=10)
						temp_link++;
					if (temp_link > longestlink)
						longestlink = temp_link;


					//size
					f.size = s.st_size;
					totalspace = totalspace + s.st_size;
					unsigned temp = 0;
					for (size_t i = s.st_size; i > 0; i/=10)
						temp++;
					if (temp > longestsize)
						longestsize = temp;

					//get group name and username
					passwd *userid;
					group *groupid;
					if (NULL == (userid = getpwuid(s.st_uid)))
						perror("user id fail");
					if (NULL == (groupid = getgrgid(s.st_gid)))
						perror("group id fail");
					f.user = userid->pw_name;
					f.group = groupid->gr_name;

					//time last mod
					char buffer [80];
					struct tm* timeinfo = localtime(&s.st_mtime); 
					strftime(buffer, 80, "%b", timeinfo);
					f.time_m = buffer;
					strftime(buffer, 80, "%-d", timeinfo);
					f.time_d = buffer;
					strftime(buffer, 80, "%R", timeinfo);
					f.time_time = buffer;

					fileinfolist.push_back(f);
				}
			}
			if (errno != 0)
				perror("directory read error");
			if (-1 == closedir(dir))
				perror("close directory error");
			std::cout << "total " << totalspace << std::endl;
			std::sort(fileinfolist.begin(), fileinfolist.end(), sortalpha);
			for (size_t i = 0; i < fileinfolist.size(); i++)
			{
				std::cout << fileinfolist.at(i).permissions << " "
					<< std::setw(longestlink) << std::right
					<< fileinfolist.at(i).hardlinks << " "
					<< fileinfolist.at(i).user << " "
					<< fileinfolist.at(i).group << " "
					<< std::setw(longestsize) << std::right
					<< fileinfolist.at(i).size << " "
					<< fileinfolist.at(i).time_m << " "
					<< std::setw(2) << std::right
					<< fileinfolist.at(i).time_d << " "
					<< fileinfolist.at(i).time_time << " "
					<< fileinfolist.at(i).name
					<< std::endl;
			}
		}
	}
	
	void listextraall(std::string file)
	{
		if (stat(file.c_str(), &s) == -1)
			perror("stat error");
		if ((s.st_mode & S_IFMT) == S_IFREG)
		{
			fileinfo f;
			f.permissions += "-";
			//whare are the user group and other permissions 
			(s.st_mode & S_IRUSR) ? (f.permissions += "r") : (f.permissions += "-");
			(s.st_mode & S_IWUSR) ? (f.permissions += "w") : (f.permissions += "-");
			(s.st_mode & S_IXUSR) ? (f.permissions += "x") : (f.permissions += "-");
			(s.st_mode & S_IRGRP) ? (f.permissions += "r") : (f.permissions += "-");
			(s.st_mode & S_IWGRP) ? (f.permissions += "w") : (f.permissions += "-");
			(s.st_mode & S_IXGRP) ? (f.permissions += "x") : (f.permissions += "-");
			(s.st_mode & S_IROTH) ? (f.permissions += "r") : (f.permissions += "-");
			(s.st_mode & S_IWOTH) ? (f.permissions += "w") : (f.permissions += "-");
			(s.st_mode & S_IXOTH) ? (f.permissions += "x") : (f.permissions += "-");
			
			//hard links
			f.hardlinks = s.st_nlink;

			//size
			f.size = s.st_size;

			//get group name and username
			passwd *userid;
			group *groupid;
			if (NULL == (userid = getpwuid(s.st_uid)))
				perror("userid fail");
			if (NULL == (groupid = getgrgid(s.st_gid)))
				perror("group id fail");
			f.user = userid->pw_name;
			f.group = groupid->gr_name;

			//time last mod
			char buffer [80];
			struct tm* timeinfo = localtime(&s.st_mtime); 
			strftime(buffer, 80, "%-b", timeinfo);
			f.time_m = buffer;
			strftime(buffer, 80, "%d", timeinfo);
			f.time_d = buffer;
			strftime(buffer, 80, "%R", timeinfo);
			f.time_time = buffer;

			std::cout << f.permissions << " "
				<< f.hardlinks << " "
				<< f.user << " "
				<< f.group << " "
				<< f.size << " "
				<< f.time_m << " "
				<< f.time_d << " "
				<< f.time_time << " "
				<< f.name
				<< std::endl;
		}
		else
		{
			std::vector<fileinfo> fileinfolist;
			if (NULL == (dir = opendir(file.c_str())))
				perror("no such dirctory.");
			errno = 0;
			while (NULL != (dirfiles = readdir(dir)))
			{
				fileinfo f;
				f.name = file;
				//what is the file name
				std::string temp = dirfiles->d_name;
				f.name = temp;
				temp = file + "/" + temp;
				if (stat(temp.c_str(), &s) == -1)
					perror("stat error");
			
				//what type of file is it? stores in permissions
				if ((s.st_mode & S_IFMT) == S_IFDIR)
					f.permissions += "d";
				else if ((s.st_mode & S_IFMT) == S_IFREG)
					f.permissions += "-";
				else if ((s.st_mode & S_IFMT) == S_IFLNK)
					f.permissions += "l";
				else if ((s.st_mode & S_IFMT) == S_IFCHR)
					f.permissions += "c";
				else if ((s.st_mode & S_IFMT) == S_IFBLK)
					f.permissions += "b";
				
				//whare are the user group and other permissions 
				(s.st_mode & S_IRUSR) ? (f.permissions += "r") : (f.permissions += "-");
				(s.st_mode & S_IWUSR) ? (f.permissions += "w") : (f.permissions += "-");
				(s.st_mode & S_IXUSR) ? (f.permissions += "x") : (f.permissions += "-");
				(s.st_mode & S_IRGRP) ? (f.permissions += "r") : (f.permissions += "-");
				(s.st_mode & S_IWGRP) ? (f.permissions += "w") : (f.permissions += "-");
				(s.st_mode & S_IXGRP) ? (f.permissions += "x") : (f.permissions += "-");
				(s.st_mode & S_IROTH) ? (f.permissions += "r") : (f.permissions += "-");
				(s.st_mode & S_IWOTH) ? (f.permissions += "w") : (f.permissions += "-");
				(s.st_mode & S_IXOTH) ? (f.permissions += "x") : (f.permissions += "-");
				
				//hard links
				f.hardlinks = s.st_nlink;
				unsigned temp_link = 0;
				for (size_t i = s.st_nlink; i > 0; i/=10)
					temp_link++;
				if (temp_link > longestlink)
					longestlink = temp_link;
				//size
				f.size = s.st_size;
				totalspace = totalspace + s.st_size;
				unsigned temp_length = 0;
				for (size_t i = s.st_size; i > 0; i/= 10)
					temp_length++;
				if (temp_length > longestsize)
					longestsize = temp_length;

				//get group name and username
				passwd *userid = getpwuid(s.st_uid);
				group *groupid = getgrgid(s.st_gid);
				f.user = userid->pw_name;
				f.group = groupid->gr_name;

				//time last mod
				char buffer [80];
				struct tm* timeinfo = localtime(&s.st_mtime); 
				strftime(buffer, 80, "%b", timeinfo);
				f.time_m = buffer;
				strftime(buffer, 80, "%-d", timeinfo);
				f.time_d = buffer;
				strftime(buffer, 80, "%R", timeinfo);
				f.time_time = buffer;

				fileinfolist.push_back(f);
			}
			if (errno != 0)
				perror("directory read error");
			if (-1 == closedir(dir))
				perror("close directory error");
			std::sort(fileinfolist.begin(), fileinfolist.end(), sortalpha);
			std::cout << "total " << totalspace << std::endl;
			for (size_t i = 0; i < fileinfolist.size(); i++)
			{
				std::cout << fileinfolist.at(i).permissions << " "
					<< std::setw(longestlink) << std::right
					<< fileinfolist.at(i).hardlinks << " "
					<< fileinfolist.at(i).user << " "
					<< fileinfolist.at(i).group << " "
					<< std::setw(longestsize) << std::right
					<< fileinfolist.at(i).size << " "
					<< fileinfolist.at(i).time_m << " "
					<< std::setw(2) << std::right
					<< fileinfolist.at(i).time_d << " "
					<< fileinfolist.at(i).time_time << " "
					<< fileinfolist.at(i).name
					<< std::endl;
			}
		}
	}

	void listextrarec(std::string file)
	{
		//list all files/dir in cur dir
		std::string newdir = file;
		std::cout << newdir << ":" << std::endl;
		std::vector<std::string> dirpath;
		listextra(newdir);
		std::cout << std::endl;
		//just print file if not dir
		if (-1 == stat(file.c_str(), &s))
		{
			perror("stat error");
			exit(1);
		}
		else
			if ((s.st_mode & S_IFMT) == S_IFREG)
				return;
		if (NULL == (dir = opendir(file.c_str())))
			perror("no such directory.");
		errno = 0;
		while (NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name.find(".") != 0) 
				dirpath.push_back(name);
		}
		if (errno != 0)
			perror("directory read error");
		if (-1 == closedir(dir))
			perror("close directory error");
		std::sort(dirpath.begin(), dirpath.end());
		for (size_t i = 0; i < dirpath.size(); i++)
		{
			totalspace = 0;
			longestsize = 0;
			longestlink = 0;
			std::string path = file + "/" + dirpath.at(i);
			if (stat(path.c_str(), &s) == -1)
				perror("stat error");
			if ((s.st_mode & S_IFMT) == S_IFDIR)
			{
				listextrarec(path);
			}
		}

	}

	void listextrarecall(std::string file)
	{
		//list all files/dir in cur dir
		std::string newdir = file;
		std::cout << newdir << ":" << std::endl;
		std::vector<std::string> dirpath;
		listextraall(newdir);
		//exit if file
		if (-1 == stat(file.c_str(), &s))
		{
			perror("stat error");
			exit(1);
		}
		else
			if ((s.st_mode & S_IFMT) == S_IFREG)
				return;
		std::cout << std::endl;
		if (NULL == (dir = opendir(file.c_str())))
			perror("no such directory.");
		errno = 0;
		while (NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name != "." && name != "..") 
				dirpath.push_back(name);
		}
		if (errno != 0)
			perror("directory read error");
		if (-1 == closedir(dir))
			perror("close directory error");
		std::sort(dirpath.begin(), dirpath.end());
		for (size_t i = 0; i < dirpath.size(); i++)
		{
			totalspace = 0;
			longestsize = 0;
			longestlink = 0;
			std::string path = file + "/" + dirpath.at(i);
			if (stat(path.c_str(), &s) == -1)
				perror("stat error");
			if ((s.st_mode & S_IFMT) == S_IFDIR)
				listextrarecall(path);
		}

	}
}


