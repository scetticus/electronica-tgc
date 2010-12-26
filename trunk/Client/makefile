SOURCES=$(wildcard src/*.cpp src/*.rc)
OBJECTS=$(addprefix out/,$(addsuffix .o,$(notdir $(basename $(SOURCES)))))
LDFLAGS= -lmingw32 -lopengl32 -lsdlmain -lsdl -lsdl_image -lsdl_ttf -lglu32 -Wl,--subsystem,windows -lws2_32

G++FLAGS= -O3 -Isrc/ -Wall -pedantic -Wextra

all: client.exe

client.exe: $(OBJECTS)
	@echo Linkeando
	@g++ $(OBJECTS) -o $@ $(LDFLAGS)

out/%.o: src/%.cpp
	@echo Compilando $<
	@g++ -c $< -o $@ $(G++FLAGS)

out/%.o: src/%.rc
	@echo Armando $<
	@windres $< -O coff -o $@
	
clean:
	@del out\*.o
	@del client.exe
