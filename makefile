win: grid.o entities.o logic.o level.o swap.o button.o menu.o
	g++ grid.o entities.o logic.o button.o level.o menu.o swap.o -o swap -Lsrc/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -lopengl32 -lwinmm -lgdi32 -ljsoncpp-s -static

lin: grid.o entities.o logic.o level.o swap.o button.o menu.o
	g++ grid.o entities.o logic.o button.o level.o menu.o swap.o -o swap -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -ljsoncpp

swap.o: swap.cpp
	g++ -Isrc/include -c swap.cpp 

level.o: level.cpp
	g++ -Isrc/include -c level.cpp 

grid.o: grid.cpp
	g++ -Isrc/include -c grid.cpp 

entities.o: entities.cpp
	g++ -Isrc/include -c entities.cpp 

logic.o: logic.cpp
	g++ -Isrc/include -c logic.cpp 

button.o: button.cpp
	g++ -Isrc/include -c button.cpp 

menu.o: menu.cpp
	g++ -Isrc/include -c menu.cpp 

clear:
	-rm *.o $(objects) swap.exe