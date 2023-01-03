win: grid.o entities.o logic.o level.o swap.o button.o menu.o
	g++ grid.o entities.o logic.o button.o level.o menu.o swap.o -o swap -Lsrc/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -lopengl32 -lwinmm -lgdi32 -ljsoncpp-s -static

lin: grid.o entities.o logic.o level.o swap.o button.o menu.o
	g++ grid.o entities.o logic.o button.o level.o menu.o swap.o -o swap.out -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -ljsoncpp

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

button.o: button.cpp
	g++ -Isrc/include -c button.cpp -DSFML_STATIC -static

menu.o: menu.cpp
	g++ -Isrc/include -c menu.cpp -DSFML_STATIC -static

clr_obj:
	-rm *.o $(objects)

clr_link:
	[ ! -e swap.exe ] || rm swap.exe
	[ ! -e swap.out ] || rm swap.out

clear: clr_obj clr_link

doc:
	@echo "https://www.partow.net/programming/makefile/index.html"