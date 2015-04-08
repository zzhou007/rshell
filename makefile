#build rshell
bin:
	mkdir bin/
	cd src; \
	g++ -Wall -Werror -ansi -pedantic -o rshell *.cpp; \
	mv rshell ../bin; \
	cd -; \
	done

