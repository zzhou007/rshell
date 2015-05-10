#build rshell
CFLAGS = -Wall -Werror -ansi -pedantic -o
all:
	mkdir bin 
	cd src; \
	g++ $(CFLAGS) ls ls_functions.cpp ls.cpp; \
	g++ $(CFLAGS) cp cp.cpp; \
	g++ $(CFLAGS) rshell redirect.cpp rshell.cpp; \
	mv cp ../bin; \
	mv ls ../bin; \
	mv rshell ../bin; \
	cd -; \

rshell: src/rshell.cpp
	cd src; \
	g++ $(CFLAGS) rshell rshell.cpp redirect.cpp; \
	mv rshell ../bin; \
	cd -; \
	
ls: src/ls.cpp src/ls_functions.cpp src/ls.h
	cd src; \
	g++ $(CFLAGS) ls ls_functions.cpp ls.cpp; \
	mv ls ../bin; \
	cd -; \

cp: src/cp.cpp src/Timer.h
	cd src; \
	g++ $(CFLAGS) cp cp.cpp ; \
	mv cp ../bin; \
	cd -; \

redirect: src/rshell.cpp src/redirect.cpp src/redirect.h
	cd src; \
	g++ $(CFLAGS) rshell src/rshell.cpp src/redirect.cpp; \
	mv rshell ../bin; \
	cd -; \

clean:
	rm -rf bin
