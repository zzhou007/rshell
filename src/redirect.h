#ifndef REDIRECT_H
#define REDIRECT_H

#include <string>
namespace redirect
{
	void redir(std::string &command, int currentp, bool &first, bool &last, bool &redirioi);
	void ioend();
	void restoreallfd();
	void clearvector();
	void closefirst();
	void setispipe();
	void removespace(std::string &s);

}
#endif
