objects = main.o makemap.o
rogue:$(objects)
	g++ $(objects) -o rogue -lncurses

makemap.o:makemap.cpp
	g++ -c makemap.cpp

main.o:main.cpp
	g++ -c main.cpp 

clean:
	rm -rf *.o rogue
