#build rshell
all:
	mkdir bin 
	cd src; \
	g++ -Wall -Werror -ansi -pedantic -o rshell rshell.cpp; \
	g++ -Wall -Werror -ansi -pedantic -o ls ls.cpp \
	mv ls ../bin; \
	mv rshell ../bin; \
	cd -; \

rshell: src/rshell
	g++ -Wall -Werror -ansi -pedantic -o rshell src/rshell.cpp
	mv src/rshell bin
	
ls: src/ls
	g++ -Wall -Werror -ansi -pedantic -o ls src/ls.cpp
	mv src/ls bin

clean:
	rm -rf bin
