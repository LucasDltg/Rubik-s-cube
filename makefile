CC ?= g++
CFLAGS ?=

LDLIBS?= -lSDL2 -lGL

INCLUDE_PATH = ./include

TARGET   = main

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCESC  := $(wildcard $(SRCDIR)/*.c)
SOURCESCPP  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(INCLUDE_PATH)/*.h) $(wildcard ./glm/*.hpp)
OBJECTSC  := $(SOURCESC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJECTSCPP  := $(SOURCESCPP:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTSC) $(OBJECTSCPP)
	mkdir -p $(BINDIR)
	g++ -o $@ $^ $(CFLAGS) $(LDLIBS)
	@echo "Linking complete!"

$(OBJECTSC): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	gcc -o $@ -c $< $(CFLAGS) -I$(INCLUDE_PATH)

$(OBJECTSCPP): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	g++ -o $@ -c $< $(CFLAGS) -I$(INCLUDE_PATH)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.gcda
	rm -f $(OBJDIR)/*.gcno
	rm -f $(BINDIR)/$(TARGET)


