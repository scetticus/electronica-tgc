SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(addprefix out/,$(addsuffix .o,$(notdir $(basename $(SOURCES)))))
LDFLAGS= -lmingw32 -lws2_32 -lsdlmain -lSDL

G++FLAGS= -O3 -Isrc/ -Wall -pedantic -Wextra

all: server.exe

server.exe: $(OBJECTS)
	@echo Linkeando
	@g++ $(OBJECTS) -o $@ $(LDFLAGS)

out/%.o: src/%.cpp
	@echo Compilando $<
	@g++ -c $< -o $@ $(G++FLAGS)

clean:
	@del out\*.o
	@del server.exe
