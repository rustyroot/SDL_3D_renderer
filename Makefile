FLAGS = -Wall -Wextra -fsanitize=address,undefined -g
MATHLIB = -lm
SDLLIB = -lSDL2
TTFLIB = -lSDL2_ttf

SOURCE_DIR = sources
OBJ_DIR = build/obj
SRC = $(wildcard $(SOURCE_DIR)/*.c)
OBJ = $(SRC:$(SOURCE_DIR)/%.c=$(OBJ_DIR)/%.o)

main.exe : $(OBJ)
	mkdir -p build
	gcc $(FLAGS) $(MATHLIB) $(SDLLIB) $(TTFLIB) -o ./build/main.exe $^

$(OBJ_DIR)/%.o : $(SOURCE_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc $(FLAGS) $(MATHLIB) $(SDLLIB) -c -o $@ $<

clean : 
	rm -rfv ./build

run :
	./build/main.exe
