CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
OBJS     = main.o game.o map.o entity.o save.o util.o

game: $(OBJS)
	$(CXX) $(CXXFLAGS) -o game $(OBJS)

main.o: main.cpp game.h save.h util.h
	$(CXX) $(CXXFLAGS) -c main.cpp

game.o: game.cpp game.h save.h util.h entity.h map.h
	$(CXX) $(CXXFLAGS) -c game.cpp

map.o: map.cpp map.h entity.h util.h
	$(CXX) $(CXXFLAGS) -c map.cpp

entity.o: entity.cpp entity.h util.h
	$(CXX) $(CXXFLAGS) -c entity.cpp

save.o: save.cpp save.h game.h util.h entity.h map.h
	$(CXX) $(CXXFLAGS) -c save.cpp

util.o: util.cpp util.h
	$(CXX) $(CXXFLAGS) -c util.cpp

run: game
	./game

clean:
	rm -f *.o game game.exe

.PHONY: run clean
