main.exe: main.o level.o grid.o entities.o logic.o
	g++ main.o level.o grid.o entities.o logic.o src\json_reader.o src\json_value.o src\json_writer.o -o main -Lsrc\lib -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	g++ -Isrc\include -c main.cpp

level.o: level.cpp
	g++ -Isrc\include -c level.cpp

grid.o: grid.cpp
	g++ -Isrc\include -c grid.cpp

entities.o: entities.cpp
	g++ -Isrc\include -c entities.cpp

logic.o: logic.cpp
	g++ -Isrc\include -c logic.cpp

clean:
	-rm *.o $(objects) main.exe