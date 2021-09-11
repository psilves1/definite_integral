integrate: main.o
	g++  -Wall -Wextra -pedantic main.o -o integrate -pthread

main.o: main.cpp
	g++  -Wall -Wextra -pedantic -c main.cpp

.PHONY: clean

clean:
	rm *.o integrate