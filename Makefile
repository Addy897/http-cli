

CC=g++
CFLAGS= -g -Wall
LDFLAGS= -lws2_32 -lssl -lcrypto
EXECUTABLE=main
FILES= $(wildcard *.cpp)

all: $(EXECUTABLE)


run: $(EXECUTABLE)
	./$(EXECUTABLE).exe

$(EXECUTABLE): $(FILES)
	$(CC) $^ $(CFLAGS) $(DFLAGS) -o $@ $(LDFLAGS)

clean:
	if [ -e $(EXECUTABLE).exe ]; then rm.exe $(EXECUTABLE).exe; fi


