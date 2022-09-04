COMPILER=g++
COMMONFLAGS=-Isrc -lsfml-system -lsfml-window -lsfml-graphics -Wall -Wextra -Wpedantic 
RELEASEFLAGS=-O3 
DEBUGFLAGS=-O0 -ggdb -g3
PROFILERFLAGS=-pg

RELEASEOBJDIR=obj/release
DEBUGOBJDIR=obj/debug
PROFILEROBJDIR=obj/profiler
SRCDIR=src

EXEC=life

DEPS=$(wildcard $(SRCDIR)/*.hpp)
SRC=$(wildcard $(SRCDIR)/*.cpp)
RELEASEOBJ=$(patsubst $(SRCDIR)/%.cpp,$(RELEASEOBJDIR)/%.o,$(SRC))
DEBUGOBJ=$(patsubst $(SRCDIR)/%.cpp,$(DEBUGOBJDIR)/%.o,$(SRC))
PROFILEROBJ=$(patsubst $(SRCDIR)/%.cpp,$(PROFILEROBJDIR)/%.o,$(SRC))

$(RELEASEOBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@mkdir -p $(RELEASEOBJDIR)
	$(COMPILER) -c -o $@ $< $(COMMONFLAGS) $(RELEASEFLAGS)

$(DEBUGOBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@mkdir -p $(DEBUGOBJDIR)
	$(COMPILER) -c -o $@ $< $(COMMONFLAGS) $(DEBUGFLAGS)

$(PROFILEROBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@mkdir -p $(PROFILEROBJDIR)
	$(COMPILER) -c -o $@ $< $(COMMONFLAGS) $(RELEASEFLAGS) $(PROFILERFLAGS)

release: $(RELEASEOBJ)
	$(COMPILER) $^ -o $(EXEC) $(COMMONFLAGS) $(RELEASEFLAGS)

debug: $(DEBUGOBJ)
	$(COMPILER) $^ -o $(EXEC) $(COMMONFLAGS) $(DEBUGFLAGS)

profiler: $(PROFILEROBJ)
	$(COMPILER) $^ -o $(EXEC) $(COMMONFLAGS) $(RELEASEFLAGS) $(PROFILERFLAGS)
