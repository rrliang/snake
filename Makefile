all:
	gcc -o mysnake  mysnake.c -lncurses "debug/snake_debug.c" "snake/snake.c" "trophy/trophy.c"