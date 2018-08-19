.SUFFIXES:
.SUFFIXES: .c .o

CFLAGS = -g --pedantic

OBJS= main.o img.o
LIBS= -lm -lpng

imgproj: $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o imgproj $(OBJS) $(LIBS)

$(OBJS):

main.o: imgproj.h
img.o: imgproj.h

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) *~ *.bak imgproj

tags: main.c img.c imgproj.h
	ctags *.c *.h
