default:
	g++ -std=c++11 maze.cpp -g -O2 -o maze
	g++ -std=c++11 run.cpp viz.cpp ui.cpp -g -o run
