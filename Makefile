CC = gcc
CFLAGS = -lm -I.
DEPS = location.h player.h playerlist.h
OBJ = server.o playerlist.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
