#build rshell
all:
	mkdir bin
	cd src; \
	g++ -Wall -Werror -ansi -pedantic -o rshell *.cpp; \
	mv rshell ../bin; \
	cd -; \

rshell: src/rshell
	g++ -Wall -Werror -ansi -pedantic -o rshell src/*.cpp
	mv src/rshell bin
	
clean:
	rm -rf bin
