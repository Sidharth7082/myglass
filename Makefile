# MyGlass Plugin

CXX ?= g++
CXXFLAGS = -fPIC -g -O2 -std=c++23 -MMD -MP
LDFLAGS = -shared
INCLUDES = $(shell pkg-config --cflags hyprland pixman-1 libdrm)
LIBS = $(shell pkg-config --libs hyprland)

# --no-gnu-unique is required for plugins loaded into Hyprland with GCC.
# Match the compiler basename so /usr/bin/g++ also works and clang++ is not
# accidentally treated as g++.
ifneq (,$(filter g++,$(notdir $(CXX))))
	CXXFLAGS += --no-gnu-unique
endif

TARGET = myglass.so

SOURCES = \
	src/main.cpp \
	src/GlassDecoration.cpp \
	src/GlassPassElement.cpp \
	src/MyGlassRenderer.cpp \
	src/GlassLayerSurface.cpp \
	src/GlassLayerPassElement.cpp \
	src/GlassLayerCompositeElement.cpp \
	src/PluginConfig.cpp \
	src/ShaderManager.cpp

OBJ = $(SOURCES:.cpp=.o)
DEP = $(OBJ:.o=.d)

all: $(TARGET)

%.o : %.cpp
	@echo "[$(CXX)] $<"
	@$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

$(TARGET): $(OBJ)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(LDFLAGS) $(OBJ) -o $@ $(LIBS)
	@echo "Done!"

clean:
	rm -f $(OBJ) $(DEP) $(TARGET)

-include $(DEP)

.PHONY: all clean
