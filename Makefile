

CC=g++
CFLAGS= -Wall -std=c++20
LDFLAGS= -lws2_32 -lssl -lcrypto -lz -ljsoncpp
EXECUTABLE=main
FILES= $(wildcard *.cpp)
EXCLUDED_ITEM = main.cpp driver.cpp file_reader.cpp
SELECTED_ITEMS = $(filter-out $(EXCLUDED_ITEM),$(FILES))
OBJECTS := $(patsubst %.cpp,build/%.o,$(SELECTED_ITEMS))
all: $(EXECUTABLE)

lib: $(OBJECTS)
	ar rcs build/libhttp.a $(OBJECTS)
 
build/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE).exe https://example.com

$(EXECUTABLE): $(FILES)
	$(CC) $^ $(CFLAGS) $(DFLAGS) -o $@ $(LDFLAGS)

clean:
	if [ -e $(EXECUTABLE).exe ]; then rm.exe $(EXECUTABLE).exe; fi


