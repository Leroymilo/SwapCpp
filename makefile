lin: grid.o entities.o logic.o level.o swap.o
	g++ grid.o entities.o logic.o level.o swap.o -o swap -Lsrc/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -lopengl32 -lwinmm -lgdi32 -ljsoncpp -static

win: grid.o entities.o logic.o level.o swap.o
	g++ grid.o entities.o logic.o level.o swap.o -o swap -Lsrc/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -lopengl32 -lwinmm -lgdi32 -ljsoncpp -static

swap.o: swap.cpp
	g++ -Isrc/include -c swap.cpp -DSFML_STATIC -static

level.o: level.cpp
	g++ -Isrc/include -c level.cpp -DSFML_STATIC -static

grid.o: grid.cpp
	g++ -Isrc/include -c grid.cpp -DSFML_STATIC -static

entities.o: entities.cpp
	g++ -Isrc/include -c entities.cpp -DSFML_STATIC -static

logic.o: logic.cpp
	g++ -Isrc/include -c logic.cpp -DSFML_STATIC -static

clear:
	-rm *.o $(objects) swap.exe