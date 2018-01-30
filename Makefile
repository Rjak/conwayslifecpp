all: gameoflife
	
gameoflife: main.o Life.o
	g++ main.o Life.o -o gameoflife

main.o: main.cpp
	g++ -c -Wall main.cpp

Life.o: Life.cpp
	g++ -c -Wall Life.cpp


clean:
	rm *.o gameoflife
