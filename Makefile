all:
	g++ -I my_lib/game -I my_lib/graph -I include/SDL2 -L lib -o Main src/game/game.cpp main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image