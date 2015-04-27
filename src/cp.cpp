#include <iostream>
#include <vector>
#include <string>
#include <sys/type.h>
#include <sys/stat.h>
#include <unistd>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
 
using namespace std;
 
int main(int argc, char** argv)
{
	string arg1 = *argv[1];
    string arg2 = *argv[2];
     
    //check if files exist
    int fd = open(arg1,O_RDONLY);
    if (fd==-1) 
    {
		perror("source file not found");
        exit(1);
    }
    if ( -1 == close(fd))
    {
            perror("close failed");
            exit(1);
    }
     
    fd = open(arg2,O_RDWR|O_CREAT|S_IRUSR|S_IRWXU|S_IWUSR);
    if (fd!=-1) {
		perror("destination file already exists");
        exit(1)
    }
    if ( -1 == close(fd))
    {
            perror("closed failed");
            exit(1);
    }
    
    if(*argv[3] == "r")
    {cout << "SWAG" << endl;}



    //METHOD 1
    fstream in(arg1;
    ofstream outfile(arg2);
    char c;
	
	while(in.get(c))
	{
		outfile.put(c);
	}
	in.close();
	
	return 0;
