.SILENT: main
main:
	g++ main.cpp -std=c++11 -I /opt/homebrew/include -L /opt/homebrew/lib -l SDL2-2.0.0
	./a.out
	-rm a.out
