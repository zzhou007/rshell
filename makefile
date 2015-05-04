#build rshell
CFLAGS = -Wall -Werror -ansi -pedantic -o
all:
	mkdir bin 
	cd src; \
	g++ $(CFLAGS) rshell rshell.cpp; \
	g++ $(CFLAGS) ls ls_functions.cpp ls.cpp; \
	g++ $(CFLAGS) cp cp.cpp ; \
	g++ $(CFLAGS) rm rm.cpp; \
	g++ $(CFLAGS) mv mv.cpp; \
	mv mv ../bin; \
	mv cp ../bin; \
	mv rm ../bin; \
	mv ls ../bin; \
	mv rshell ../bin; \
	cd -; \

rshell: src/rshell.cpp
	g++ $(CFLAGS) rshell src/rshell.cpp; \
	mv src/rshell bin; \
	
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

rm: src/rm.cpp
	cd src; \
	g++ $(CFLAGS) rm rm.cpp; \
	mv rm ../bin; \
	cd -; \

mv: src/mv.cpp
	cd src; \
	g++ $(CFLAGS) mv mv.cpp; \
	mv mv ../bin; \
	cd -; \

clean:
	rm -rf bin
