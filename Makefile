.SUFFIXES:
.SUFFIXES: .c .o

OBJS= sat2tile.o

sat2tile: $(OBJS)

$(OBJS):

.c.o:
	cc -c $<
