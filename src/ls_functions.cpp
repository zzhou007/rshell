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
	//green
	#define EXE "\033[32m"
	//blue
	#define DIRC "\033[34m"
	#define REG "\033[0m"
	//gray back
	#define HIDDEN "\033[47m"

	DIR *dir;
	struct stat s;
	struct dirent *dirfiles;
	
	//this checks if it is a file or a dir if it is a file it returns true
	//returns false if it is a dir
	bool printfd(std::string file)
	{
		//just print file if not dir
		if (-1 == stat(file.c_str(), &s))
		{
			perror("stat error");
			exit(1);
		}
		if ((s.st_mode & S_IFMT) == S_IFREG)
		{
			if (file.find(".") == 0)
			{
				if (s.st_mode & S_IXUSR)
					std::cout << HIDDEN << EXE << file << REG << "  " << std::endl;
				else
					std::cout << HIDDEN << file << REG << "  " << std::endl;
				return true;
			}
			if (s.st_mode & S_IXUSR)
				std::cout << EXE << file << REG << "  " << std::endl;
			else
				std::cout << file << "  " << std::endl;
			return true;

		}
		return false;

	}
	
	//function prints dir or file name with color
	//path is path to dir or file
	//width sets colum width 
	//assumes terminal is 80 char across
	void printcolor(std::string name, std::string path, int w)
	{
		if (-1 == stat(path.c_str(), &s))
		{
			perror("output with color fail");
			exit(1);
		}
		if (name.find(".") == 0)
		{
			if ((s.st_mode & S_IFMT) & S_IFDIR)
				std::cout << HIDDEN << DIRC 
					<< std::setw(w) << std::left << name  << REG << "  ";
			else if (s.st_mode & S_IXUSR)
				std::cout << HIDDEN << EXE 
					<< std::setw(w) << std::left << name  << REG << "  ";
			else 
				std::cout << HIDDEN 
					<< std::setw(w) << std::left << name << REG << "  ";
			return;
		}
		if ((s.st_mode & S_IFMT) & S_IFDIR)
			std::cout << DIRC 
				<< std::setw(w) << std::left << name << REG << "  ";
		else if (s.st_mode & S_IXUSR)
			std::cout << EXE 
				<< std::setw(w) << std::left << name << REG << "  ";
		else 
			std::cout << std::setw(w) << std::left << name << "  ";
	}
	
	int longest(std::vector<std::string> v)
	{
		unsigned l = 0;
		for (size_t i = 0; i < v.size(); i++)
		{
			if (v.at(i).length() > l)
				l = v.at(i).length();
		}
		return l;
	}
	void list(std::string file)
	{
		if (printfd(file))
			return;
		//goes to dir and prints out content
		std::vector<std::string> output;
		if (NULL == (dir = opendir(file.c_str())))
		{
			perror("no such directory.");
			exit(1);
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
			exit(1);
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
			exit(1);
		}
		std::sort(output.begin(), output.end());
		int line = 0;
		int colw = longest(output);
		for (size_t i = 0; i < output.size(); i++)
		{
			printcolor(output.at(i), file + "/" + output.at(i), colw);
			line += colw;
			if (colw > 80)
				std::cout << std::endl;
			else if (line + colw > 80)
			{
				line = 0;
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
	}

	void listall(std::string file)
	{
		if(printfd(file))
			return;
		//goes to dir and prints out content 
		std::vector<std::string> output;
		if (NULL == (dir = opendir(file.c_str())))
		{
			perror("no such directory.");
			exit(1);
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
			exit(1);
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
			exit(1);
		}
		std::sort(output.begin(), output.end());
		int line = 0;
		int colw = longest(output);
		for (size_t i = 0; i < output.size(); i++)
		{
			printcolor(output.at(i), file + "/" + output.at(i), colw);
			line += colw;
			if (line > 80)
				std::cout << std::endl;
			else if (line + colw > 80)
			{
				line = 0;
				std::cout << std::endl;
			}
		}
		//std::cout << output.at(i) << "  ";
		std::cout << std::endl;
	}

	void listrec(std::string file)
	{
		if (printfd(file))
			return;
		//list files/dir in cur dir
		std::string newdir = file;
		std::cout << newdir << ":" << std::endl;
		std::vector<std::string> dirpath;
		list(newdir);
		std::cout << std::endl;
		if (NULL == (dir = opendir(file.c_str())))
		{
			perror("no such directory.");
			exit(1);
		}
		errno = 0;
		while (NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name.find(".") != 0)
				dirpath.push_back(name);
		}
		if (errno != 0)
		{
			perror("directory read error");
			exit(1);
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
			exit(1);
		}
		std::sort(dirpath.begin(), dirpath.end());
		for (size_t i = 0; i < dirpath.size(); i++)
		{
			std::string path = file + "/" + dirpath.at(i);
			if (stat(path.c_str(), &s) == -1)
			{
				perror("stat error");
				exit(1);
			}
			if ((s.st_mode & S_IFMT) == S_IFDIR)
				listrec(path);
		}
	}

	void listrecall(std::string file)
	{
		if (printfd(file))
			return;
		//list all files/dir in cur dir
		std::string newdir = file;
		std::cout << newdir << ":" << std::endl;
		std::vector<std::string> dirpath;
		listall(newdir);
		std::cout << std::endl;
		if (NULL == (dir = opendir(file.c_str())))
		{
			perror("no such directory.");
			exit(1);
		}
		errno = 0;
		while (NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name != "." && name != "..") 
				dirpath.push_back(name);
		}
		if (errno != 0)
		{
			perror("directory read error");
			exit(1);
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
			exit(1);
		}
		std::sort(dirpath.begin(), dirpath.end());
		for (size_t i = 0; i < dirpath.size(); i++)
		{
			std::string path = file + "/" + dirpath.at(i);
			if (stat(path.c_str(), &s) == -1)
			{
				perror("stat error");
				exit(1);
			}
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
		//depending on the int the color changes
		//0 no color
		//1 hidden
		//12 hidden + dir
		//13 hidden + exec
		//2 dir
		//3 exec
		int color;
	};

	void printextracolor(int color, std::string name)
	{
		if (color == 1)
			std::cout << HIDDEN << name << REG;
		else if (color == 12)
			std::cout << HIDDEN << DIRC << name << REG;
		else if (color == 13)
			std::cout << HIDDEN << EXE << name << REG;
		else if (color == 2)
			std::cout << DIRC << name << REG;
		else if (color == 3)
			std::cout << EXE << name << REG;
		else 
			std::cout << name;
	}
	//comparison to sort struct by file name	
	bool sortalpha(const fileinfo &name1, const fileinfo &name2)
	{
		return name1.name < name2.name;
	}

	unsigned totalspace = 0;

	unsigned longestsize = 0;

	unsigned longestlink = 0;

	void listextra(std::string file)
	{	
		int color = 0;
		if (stat(file.c_str(), &s) == -1)
		{
			perror("stat error");
			exit(1);
		}
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
			{
				perror("userid fail");
				exit(1);
			}
			if (NULL == (groupid = getgrgid(s.st_gid)))
			{
				perror("group id fail");
				exit(1);
			}
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

			//calculate color
			if (s.st_mode & S_IXUSR)
				color += 3;
			if (f.name.find(".") == 0)
				color += 10;

			//output 
			std::cout << f.permissions << " "
				<< f.hardlinks << " "
				<< f.user << " "
				<< f.group << " "
				<< f.size << " "
				<< f.time_m << " "
				<< f.time_d << " "
				<< f.time_time << " ";
			printextracolor(color, f.name);
			std::cout << std::endl;
		}
		else
		{
			std::vector<fileinfo> fileinfolist;
			if (NULL == (dir = opendir(file.c_str())))
			{
				perror("no such dirctory.");
				exit(1);
			}
			errno = 0;
			while (NULL != (dirfiles = readdir(dir)))
			{
				color = 0;
				fileinfo f;
				//what is the file name
				std::string temp = dirfiles->d_name;
				if (temp.find(".") != 0)
				{	
					f.name = temp;
					temp = file + "/" + temp;
					if (stat(temp.c_str(), &s) == -1)
					{
						perror("stat error");
						exit(1);
					}
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
					totalspace = totalspace + s.st_blocks;
					unsigned temp = 0;
					for (size_t i = s.st_size; i > 0; i/=10)
						temp++;
					if (temp > longestsize)
						longestsize = temp;

					//get group name and username
					passwd *userid;
					group *groupid;
					if (NULL == (userid = getpwuid(s.st_uid)))
					{
						perror("user id fail");
						exit(1);
					}
					if (NULL == (groupid = getgrgid(s.st_gid)))
					{
						perror("group id fail");
						exit(1);
					}
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
					
					//calculate color
					if ((s.st_mode & S_IFMT) == S_IFDIR)
						color += 2;
					else if (s.st_mode & S_IXUSR)
						color += 3;
					if (f.name.find(".") == 0)
						color += 10;
					f.color = color;

					fileinfolist.push_back(f);
				}
			}
			if (errno != 0)
			{
				perror("directory read error");
				exit(1);
			}
			if (-1 == closedir(dir))
			{
				perror("close directory error");
				exit(1);
			}
			std::cout << "total " << totalspace/2 << std::endl;
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
					<< fileinfolist.at(i).time_time << " ";
					//<< fileinfolist.at(i).name
					//<< std::endl;
				printextracolor(fileinfolist.at(i).color, fileinfolist.at(i).name);
				std::cout << std::endl;
			}
		}
	}
	
	void listextraall(std::string file)
	{
		int color = 0;
		if (stat(file.c_str(), &s) == -1)
		{
			perror("stat error");
			exit(1);
		}
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
			{
				perror("userid fail");
				exit(1);
			}
			if (NULL == (groupid = getgrgid(s.st_gid)))
			{
				perror("group id fail");
				exit(1);
			}
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

			//calculate color
			if (s.st_mode & S_IXUSR)
				color += 3;
			if (f.name.find(".") == 0)
				color += 10;
			
			std::cout << f.permissions << " "
				<< f.hardlinks << " "
				<< f.user << " "
				<< f.group << " "
				<< f.size << " "
				<< f.time_m << " "
				<< f.time_d << " "
				<< f.time_time << " ";
				//<< f.name
				//<< std::endl;
			printextracolor(color, f.name);
			std::cout << std::endl;
		}
		else
		{
			std::vector<fileinfo> fileinfolist;
			if (NULL == (dir = opendir(file.c_str())))
			{
				perror("no such dirctory.");
				exit(1);
			}
			errno = 0;
			while (NULL != (dirfiles = readdir(dir)))
			{
				color = 0;
				fileinfo f;
				f.name = file;
				//what is the file name
				std::string temp = dirfiles->d_name;
				f.name = temp;
				temp = file + "/" + temp;
				if (stat(temp.c_str(), &s) == -1)
				{
					perror("stat error");
					exit(1);
				}
			
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
				totalspace = totalspace + s.st_blocks;
				unsigned temp_length = 0;
				for (size_t i = s.st_size; i > 0; i/= 10)
					temp_length++;
				if (temp_length > longestsize)
					longestsize = temp_length;

				//get group name and username
				passwd *userid;
				group *groupid;
				if (NULL == (userid = getpwuid(s.st_uid)))
				{
					perror("get user id fail");
					exit(1);
				}
				if (NULL == (groupid = getgrgid(s.st_gid)))
				{
					perror("get group id fail");
					exit(1);
				}
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

				//calculate color
				if ((s.st_mode & S_IFMT) == S_IFDIR)
					color += 2;
				else if (s.st_mode & S_IXUSR)
					color += 3;
				if (f.name.find(".") == 0)
					color += 10;
				f.color = color;
				
				fileinfolist.push_back(f);
			}
			if (errno != 0)
			{
				perror("directory read error");
				exit(1);
			}
			if (-1 == closedir(dir))
			{
				perror("close directory error");
				exit(1);
			}
			std::sort(fileinfolist.begin(), fileinfolist.end(), sortalpha);
			std::cout << "total " << totalspace/2 << std::endl;
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
					<< fileinfolist.at(i).time_time;
					//<< fileinfolist.at(i).name
					//<< std::endl;
				printextracolor(fileinfolist.at(i).color, fileinfolist.at(i).name);
				std::cout << std::endl;
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
		{
			perror("no such directory.");
			exit(1);
		}
		errno = 0;
		while (NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name.find(".") != 0) 
				dirpath.push_back(name);
		}
		if (errno != 0)
		{
			perror("directory read error");
			exit(1);
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
			exit(1);
		}
		std::sort(dirpath.begin(), dirpath.end());
		for (size_t i = 0; i < dirpath.size(); i++)
		{
			totalspace = 0;
			longestsize = 0;
			longestlink = 0;
			std::string path = file + "/" + dirpath.at(i);
			if (stat(path.c_str(), &s) == -1)
			{
				perror("stat error");
				exit(1);
			}
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
		{
			perror("no such directory.");
			exit(1);
		}
		errno = 0;
		while (NULL != (dirfiles = readdir(dir)))
		{
			std::string name = dirfiles->d_name;
			if (name != "." && name != "..") 
				dirpath.push_back(name);
		}
		if (errno != 0)
		{
			perror("directory read error");
			exit(1);
		}
		if (-1 == closedir(dir))
		{
			perror("close directory error");
			exit(1);
		}
		std::sort(dirpath.begin(), dirpath.end());
		for (size_t i = 0; i < dirpath.size(); i++)
		{
			totalspace = 0;
			longestsize = 0;
			longestlink = 0;
			std::string path = file + "/" + dirpath.at(i);
			if (stat(path.c_str(), &s) == -1)
			{
				perror("stat error");
				exit(1);
			}
			if ((s.st_mode & S_IFMT) == S_IFDIR)
				listextrarecall(path);
		}

	}
}

