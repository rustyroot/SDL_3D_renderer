cmp :
	gcc -o main main.c -lm `sdl2-config --cflags --libs`

safe :
	gcc -Wall -Wextra -fsanitize=address,undefined -o main main.c -lm `sdl2-config --cflags --libs` -g%