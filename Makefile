
SRC = defender.cpp Level.cpp Pool.cpp Vector2f.cpp defender_glfw.cpp math.cpp

CFLAGS = -pthread 
LFLAGS = -L/usr/local/lib -lXrandr -lrt -lX11 -lGL -lm -lGLU  -lGL -pthread -lm -lglfw

defender: ${SRC}
	g++ ${CFLAGS} ${SRC} ${LFLAGS} -o defender


clean:
	rm -f *.o defender
	