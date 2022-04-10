all:
	gcc -o mysnake  mysnake.c -lncurses "debug/debug.c" "snake/snake.c" "trophy/trophy.c"