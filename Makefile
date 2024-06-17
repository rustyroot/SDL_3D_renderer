FLAGS = -Wall -Wextra -fsanitize=address,undefined -g
MATHLIB = -lm
SDLLIB = -lSDL2
TTFLIB = -lSDL2_ttf
SRC = $(wildcard ./sources/*.c)
OBJ = $(SRC:sources/%.c=build/%.o)

main.exe : $(OBJ)
	mkdir -p build
	gcc $(FLAGS) $(MATHLIB) $(SDLLIB) $(TTFLIB) -o ./build/main.exe $^

%.o : %.c
	mkdir -p build
	gcc $(FLAGS) $(MATHLIB) $(SDLLIB) -c -o $@ $<

clean : 
	rm -rfv ./build

run :
	./build/main.exe
