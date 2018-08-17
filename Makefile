.SUFFIXES:
.SUFFIXES: .c .o

CFLAGS = -g --pedantic

OBJS= main.o img.o

imgproj: $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o imgproj $(OBJS) $(LIBS)

$(OBJS):

main.o: imgproj.h
img.o: imgproj.h

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) *~ *.bak imgproj
