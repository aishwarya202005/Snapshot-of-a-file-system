#2018201046
#Divyanshi Kushwaha
CC = g++
CFLAGS = -Wall -std=c++1z
LDFLAGS = -lcrypto 
DEPS = headerfiles.h Checksum.cpp Chunks.cpp rsync.cpp snapshot.cpp
OBJ = main.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


