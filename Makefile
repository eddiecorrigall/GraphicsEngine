PATHS = -Iheader -I/usr/local/include/
LIBRARIES = -L/usr/local/lib

COMPILER = g++
CPPFLAGS = -O3 $(PATHS)

LINKERS = \
	-lGL \
	-lGLU \
	-lSDL2 \
	-lSDL2main \
	-lSDL2_image \
	-lSDL2_mixer

OBJECTS = Main.o \
	source/Audio.o \
	source/Colour.o \
	source/Input.o \
	source/MD2.o \
	source/Process.o \
	source/Video.o

all: $(OBJECTS)
	$(COMPILER) $^ -o engine $(LIBRARIES) $(LINKERS)

clean:
	rm -f *.o source/*.o engine
