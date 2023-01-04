OBJ_DIR := ./objects
LIB_DIR := src/lib
INCLUDE := src/include
LIBS_LIN := -L$(LIB_DIR) -lsfml-graphics -lsfml-window -lsfml-system -ljsoncpp
LIBS_WIN := -L$(LIB_DIR) -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 -ljsoncpp -static

SRC := $(wildcard src/*.cpp)
OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

lin: $(OBJECTS)
	g++ -O3 $(OBJECTS) -o rtx.out $(LIBS_LIN)

win: $(OBJECTS)
	g++ -O3 $(OBJECTS) -o rtx.exe $(LIBS_WIN)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	g++ -I$(INCLUDE) -c $< -o $@ -DSFML_STATIC -static

clr_obj:
	-rm *.o objects

clr_link:
	[ ! -e swap.exe ] || rm swap.exe
	[ ! -e swap.out ] || rm swap.out

clear: clr_obj clr_link