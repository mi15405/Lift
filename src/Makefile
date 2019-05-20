PROGRAM = lift
SOURCES_01 = main.cpp ball.cpp elevator.cpp bridge.cpp floor.cpp camera.cpp color.cpp image.cpp textureManager.cpp
SOURCES_02 = point3D.cpp movable.cpp drawable.cpp material.cpp ballRow.cpp gameController.cpp
CC = g++
INCLUDE = -Iheaders
CFLAGS = -Wall -std=c++17 
CLIBS = -lGL -lGLU -lglut
PROFILER = -pg -O0
DEBUG = -g

test_00: $(SOURCES_01:.cpp=.o) $(SOURCES_02:.cpp=.o)
	$(CC) $(CFLAGS) -o $(PROGRAM) $^ $(CLIBS) $(INCLUDE)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ -MMD $< $(INCLUDE)

-include $(SOURCES_01:.cpp=.d)
-include $(SOURCES_02:.cpp=.d)

profiler: $(SOURCES_01:.cpp=.o) $(SOURCES_02:.cpp=.o)
	$(CC) $(CFLAGS) -o $(PROGRAM) $^ $(CLIBS) $(INCLUDE) $(PROFILER)

debug: $(SOURCES_01:.cpp=.o) $(SOURCES_02:.cpp=.o)
	$(CC) $(CFLAGS) -o $(PROGRAM) $^ $(CLIBS) $(INCLUDE) $(DEBUG)

.PHONY: clean

clean:
	rm *.o *.d 
	echo clean done


