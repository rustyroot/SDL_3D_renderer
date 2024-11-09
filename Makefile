FLAGS = -Wall -Wextra -fsanitize=address,undefined -g
#FLAGS = -g
LIBS = -lm -lSDL2 -lSDL2_ttf

SOURCE_DIR = sources
OBJ_DIR = build/obj
SRC = $(wildcard $(SOURCE_DIR)/*.c)
OBJ = $(SRC:$(SOURCE_DIR)/%.c=$(OBJ_DIR)/%.o)

main.exe : $(OBJ)
	mkdir -p build
	gcc $(FLAGS) -o ./build/main.exe $^ $(LIBS)

$(OBJ_DIR)/%.o : $(SOURCE_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc -c -o $@ $< $(LIBS)

clean :
	rm -rfv ./build

run :
	./build/main.exe

fast : $(OBJ)
	mkdir -p build
	gcc -o ./build/main.exe $^ $(LIBS)
