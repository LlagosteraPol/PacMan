Pacman: Block.cpp MapAlgorithm.cpp Ghost.cpp PacmanGuy.cpp Main.cpp
	g++  -o Pacman .objs/Block.o .objs/Ghost.o .objs/Main.o .objs/MapAlgorithm.o .objs/PacmanGuy.o /usr/local/lib/libjpeg.a  -lglut -lGLU -lGL -pthread
clean:
	rm pacman


