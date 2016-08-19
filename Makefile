CPP11 = g++ -std=c++11 -fpermissive 

all: main.o Datrie.o NgramSearch.o
	$(CPP11)  -o main main.o Datrie.o NgramSearch.o
main.o: main.cpp
	$(CPP11) -c main.cpp 
Datrie.o: Datrie.cpp Datrie.hpp
	$(CPP11) -c Datrie.cpp -o Datrie.o
NgramSearch.o: NgramSearch.cpp NgramSearch.hpp
	$(CPP11) -c NgramSearch.cpp -o NgramSearch.o
clean:
	rm main main.o


