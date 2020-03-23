build:
	g++ -O3 -o main main.cpp types.h types.cpp renderer.cpp -lSDL2
debug:
	g++ -g -fsanitize=address -o main_debug main.cpp types.cpp renderer.cpp -lSDL2