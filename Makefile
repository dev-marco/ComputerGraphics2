# Parametros alteraveis

CXX = c++
CXXFLAGS = -std=c++11 -g -Wall -O3 -Wno-missing-braces
CXXLIBS = -lglfw3 -lpng
SRC := main.cc\
 engine/object.cc engine/mesh.cc engine/background.cc engine/event.cc engine/color.cc engine/window.cc engine/shader.cc engine/audio.cc\
 boids/bird.cc
OBJ := $(SRC:%.cc=build/%.o)
DEP := $(SRC:%.cc=deps/%.d)
NAME = boids

ifeq ($(OS), Windows_NT)
CXXLIBS += -lopengl32 -lglew32 -lglu32 -lgdi32 -lSDL2 -lSDL2_Mixer -static-libstdc++ -static-libgcc
else
CXXLIBS += -lGL -lGLEW -lGLU -lglut -lXrandr -lXext -lX11 -ldl -lXxf86vm -lXinerama -lXcursor -lpthread $(shell sdl2-config --cflags --libs) -lSDL2_mixer
endif

# Fim dos parametros

ALL := bin/$(NAME)

all default: $(ALL)

$(ALL): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(ALL) $(OBJ) $(CXXLIBS)

build: $(OBJ)
	@:

build/%.o: src/%.cc deps/%.d
	$(CXX) $(CXXFLAGS) -c -o $@ $<

autodeps deps: $(DEP)
	@:

deps/%.d: src/%.cc
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:deps/%.d=build/%.o) -o $@ $<

check test: all
	bin/$(NAME)

.PHONY: clean

clean:
	$(RM) $(OBJ) $(DEP) $(ALL)

.DEFAULT: all

ifeq ($(MAKECMDGOALS),)
TYPES := all
else
TYPES := $(MAKECMDGOALS)
endif

ifneq ($(shell (echo $(TYPES) | grep -oP "(all|default|build|check|test)")),)
-include $(DEP)
endif
