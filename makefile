CFLAGS = -c -g -Wall
#GL_INCLUDE    = -I/usr/include/GL
GL_LIBDIR    = -L. -L/usr/X11R6/lib
GL_LIBRARIES = -lglut -lGL -lm -lGLU -lXmu -lXext -lXi -lX11
CC=gcc
OS = "unknown"

# Add objectiveC library for MacOSX.
ifneq ($(strip $(shell $(CC) -v 2>&1 | grep -i "Apple")),)
 GL_INCLUDE    = -I/usr/X11R6/include
 GL_LIBRARIES = -lobjc -framework OpenGL -framework GLUT
 BROWSER = dillo
 OS = "osx"
endif
# Nothing to be done for Linux :o)
ifneq ($(strip $(shell $(CC) -v 2>&1 | grep -i "Linux")),)
 OS = "linux"
 BROWSER = mozilla
endif
#



job: one two dox
	one -c4 && two -n100 && $(BROWSER) DOX/html/index.html &
one: one.o
	$(CC) $(GL_LIBDIR) one.o $(GL_LIBRARIES) -o $@
one.o : one.c
	$(CC) $(GL_INCLUDE) $(CFLAGS) $<
two: two.o
	$(CC) $(GL_LIBDIR) two.o $(GL_LIBRARIES) -o $@
two.o : two.c
	$(CC) $(GL_INCLUDE) $(CFLAGS) $<
inclusion: inclusion.o
	$(CC) $(GL_LIBDIR) inclusion.o $(GL_LIBRARIES) -o $@
inclusion.o : inclusion.c
	$(CC) $(GL_INCLUDE) $(CFLAGS) $<


clean:
	@echo "operating system = $(OS)"
	rm -rf *.o one two DOX

###############################################################################
# Generate doxygen documentation of file two.c using command file two.dox.
dox: two.c two.h
	rm -rf DOX
	doxygen two.dox


