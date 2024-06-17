FLAGS = -Wall -Wextra -fsanitize=address,undefined -g
MATHLIB = -lm
SDLLIB = `sdl2-config --cflags --libs`
SRC = $(wildcard ./sources/*.c)
OBJ = $(SRC:sources/%.c=build/%.o)

%.o : %.c
	mkdir -p build
	gcc $(FLAGS) $(MATHLIB) $(SDLLIB) -c -o $@ $<

main.exe : $(OBJ)
	mkdir -p build
	gcc $(FLAGS) $(MATHLIB) $(SDLLIB) -o ./build/main.exe $^

clean : 
	rm -rfv ./build

run :
	./build/main.exe
