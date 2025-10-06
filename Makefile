

CC=g++
CFLAGS= -g -Wall -std=c++20
LDFLAGS= -lws2_32 -lssl -lcrypto -lz -ljsoncpp
EXECUTABLE=main
FILES= $(wildcard *.cpp)

all: $(EXECUTABLE)


run: $(EXECUTABLE)
	./$(EXECUTABLE).exe https://example.com

$(EXECUTABLE): $(FILES)
	$(CC) $^ $(CFLAGS) $(DFLAGS) -o $@ $(LDFLAGS)

clean:
	if [ -e $(EXECUTABLE).exe ]; then rm.exe $(EXECUTABLE).exe; fi


