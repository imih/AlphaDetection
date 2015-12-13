PROJECT = nenr
SOURCE = main.cc common.cc window.cc neural_network.cc neuron.cc

CC = g++
CFLAGS =-Wall -g -lGL -lglut -lGLU -c -std=c++11
LDFLAGS = 
OBJECTS = ${SOURCE:.cpp=.o}

all: $(SOURCE) $(PROJECT)

$(PROJECT): $(OBJECTS)
		$(CC) $(OBJECTS) -std=c++11 -g -lGL -lglut -lGLU -o $(PROJECT)

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

