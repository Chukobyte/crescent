CORE_PROJECT_NAME := roll_back_engine
EDITOR_PROJECT_NAME := roll_back_editor

# OS Specific
ifeq ($(OS),Windows_NT)
    OS_TYPE := windows
    CORE_BUILD_OBJECT := $(CORE_PROJECT_NAME).exe
    CORE_BUILD_OBJECT_TEST := $(CORE_PROJECT_NAME)_test.exe
    EDITOR_BUILD_OBJECT := $(EDITOR_PROJECT_NAME).exe
    EDITOR_BUILD_OBJECT_TEST := $(EDITOR_PROJECT_NAME)_test.exe
    L_FLAGS := -lmingw32 -static-libgcc -lSDL2main -lSDL2 -lpython37 -lfreetype -lws2_32
    DELETE_CMD := del
else
    OS_TYPE := linux
    CORE_BUILD_OBJECT := $(CORE_PROJECT_NAME)
    CORE_BUILD_OBJECT_TEST := $(CORE_PROJECT_NAME)_test
    EDITOR_BUILD_OBJECT := $(EDITOR_PROJECT_NAME)
    EDITOR_BUILD_OBJECT_TEST := $(EDITOR_PROJECT_NAME)_test
    # TODO: Need to validate linux flags
    L_FLAGS := -lSDL2 -lGL -lpython3.7m -lfreetype -lcrypt -lpthread -ldl  -lutil -lm -static-libgcc
    DELETE_CMD := rm
endif

CC := gcc # C Compiler
CXX := g++ # C++ compiler
C_FLAGS := # Overridden by builds

# Engine
CORE_C_FLAGS := -std=c11 -Wfatal-errors -Wall -Wextra -Wno-write-strings -Wno-deprecated-declarations -Wno-unused-variable -Wno-cast-function-type -Wno-unused-parameter -Wno-missing-field-initializers
CORE_SRC = $(wildcard engine/src/main.c engine/src/core/*.c engine/src/core/math/*.c engine/src/core/utils/*.c engine/src/core/data_structures/*.c engine/src/core/memory/*.c engine/src/core/input/*.c engine/src/core/rendering/*.c engine/src/core/audio/*.c engine/src/core/ecs/*.c engine/src/core/ecs/component/*.c engine/src/core/ecs/entity/*.c engine/src/core/ecs/system/*.c engine/src/core/scene/*.c engine/src/core/scripting/*.c engine/src/core/scripting/python/*.c engine/src/core/scripting/native/*.c engine/src/core/scripting/native/internal_classes/*.c engine/src/core/networking/*.c engine/src/core/thread/*.c include/stb_image/stb_image.c include/glad/glad.c)
CORE_OBJ_C = $(CORE_SRC:.c=.o)
CORE_SRC_TEST = $(wildcard engine/src/test/main.c include/unity.c engine/src/core/*.c engine/src/core/math/*.c engine/src/core/utils/*.c engine/src/core/data_structures/*.c engine/src/core/memory/*.c engine/src/core/input/*.c engine/src/core/rendering/*.c engine/src/core/audio/*.c engine/src/core/ecs/component/*.c engine/src/core/ecs/*.c engine/src/core/ecs/entity/*.c engine/src/core/ecs/system/*.c engine/src/core/scene/*.c engine/src/core/scripting/*.c engine/src/core/scripting/python/*.c engine/src/core/scripting/native/*.c engine/src/core/scripting/native/internal_classes/*.c engine/src/core/networking/*.c engine/src/core/thread/*.c include/stb_image/stb_image.c include/glad/glad.c)
CORE_OBJ_C_TEST = $(CORE_SRC_TEST:.c=.o)

# Editor
EDITOR_C_FLAGS := -w -std=c++20 -Wfatal-errors -Wall -Wextra -Wno-write-strings -Wno-deprecated-declarations -Wno-unused-variable -Wno-cast-function-type -Wno-unused-parameter -Wno-missing-field-initializers
EDITOR_SRC = $(wildcard editor/src/main.cpp editor/src/core/*.cpp editor/src/core/ui/*.cpp editor/src/core/ui/imgui/*.cpp editor/src/core/utils/*.cpp include/*.cpp include/imgui/*.cpp)
EDITOR_SRC_C = $(wildcard engine/src/core/game_properties.c engine/src/core/utils/logger.c engine/src/core/utils/rbe_file_system_utils.c engine/src/core/utils/rbe_hash_map.c engine/src/core/memory/rbe_mem.c engine/src/core/scripting/python/rbe_py.c engine/src/core/scripting/python/py_helper.c engine/src/core/scripting/python/rbe_py_api_module.c include/stb_image/stb_image.c include/glad/glad.c)
EDITOR_SRC_TEST = $(wildcard editor/src/test/main.cpp editor/src/test/unit/*.cpp editor/src/core/*.cpp editor/src/core/ui/*.cpp editor/src/core/ui/imgui/*.cpp editor/src/core/utils/*.cpp include/*.cpp include/imgui/*.cpp)
EDITOR_OBJ_CPP = $(EDITOR_SRC:.cpp=.o)
EDITOR_OBJ_CPP_TEST = $(EDITOR_SRC_TEST:.cpp=.o)
EDITOR_OBJ_C = $(EDITOR_SRC_C:.c=.o)


I_FLAGS := -I"./include" -I"${SDL2_INCLUDE_PATH}" -I"${PYTHON_INCLUDE_PATH}" -I"${FREETYPE_INCLUDE_PATH}"
LIBRARIES := -L"${SDL2_LIBS_PATH}" -L"${PYTHON_LIBS_PATH}" -L"${FREETYPE_LIBS_PATH}"
RELEASE_FLAGS = -DHAVE_SNPRINTF=1

### ALL ###
.PHONY: all build clean

all: clean format build

%.o: %.c
	@echo "Compiling " $< " into " $@
	@$(CC) -c $(C_FLAGS) $< -o $@ $(I_FLAGS) $(RELEASE_FLAGS)

%.o: %.cpp
	@echo "Compiling " $< " into " $@
	@$(CXX) -c $(C_FLAGS) $< -o $@ $(I_FLAGS) $(RELEASE_FLAGS)

# Defaults to engine builds
build: build-engine
debug-build: debug-build-engine
format: format-engine
clean: clean-engine
run: run-engine

build-all: build-engine build-editor
format-all: format-engine format-editor
clean-all: clean-engine clean-editor

### ENGINE CORE ###
build-engine: C_FLAGS = $(CORE_C_FLAGS)
build-engine: $(CORE_OBJ_C)
	@echo "Linking " $@
	@$(CC) -o $(CORE_BUILD_OBJECT) $^ $(I_FLAGS) $(L_FLAGS) $(LIBRARIES)

# Adds debug symbols for debugger
debug-build-engine: CORE_C_FLAGS +=  -g
debug-build-engine: build-engine

format-engine:
	astyle -n --style=google --recursive engine/src/*.c engine/src/*.h

clean-engine:
ifneq ("$(wildcard $(CORE_BUILD_OBJECT))","")
	@$(DELETE_CMD) $(CORE_BUILD_OBJECT)
endif
ifeq ($(OS_TYPE),windows)
	@$(foreach object, $(CORE_OBJ_C), $(DELETE_CMD) $(subst /,\,$(object));)
else
	@$(foreach object, $(CORE_OBJ_C), $(DELETE_CMD) $(object);)
endif

run-engine:
	./$(CORE_BUILD_OBJECT)

run-game-fighter-test:
	./$(CORE_BUILD_OBJECT) -d test_games/fighter_test/

# Engine Test
engine-test: build-engine-test run-engine-test
engine-test-clean-run: clean-engine-test engine-test

build-engine-test: $(CORE_OBJ_C_TEST)
	@echo "Linking " $@
	@$(CC) -o $(CORE_BUILD_OBJECT_TEST) $^ $(I_FLAGS) $(L_FLAGS) $(LIBRARIES)

clean-engine-test:
ifneq ("$(wildcard $(CORE_BUILD_OBJECT_TEST))","")
	@$(DELETE_CMD) $(CORE_BUILD_OBJECT_TEST)
endif
ifeq ($(OS_TYPE),windows)
	@$(foreach object, $(CORE_OBJ_C_TEST), $(DELETE_CMD) $(subst /,\,$(object));)
else
	@$(foreach object, $(CORE_OBJ_C_TEST), $(DELETE_CMD) $(object);)
endif

run-engine-test:
	./$(CORE_BUILD_OBJECT_TEST)

### EDITOR ###
build-editor: C_FLAGS = $(EDITOR_C_FLAGS)
build-editor: $(EDITOR_OBJ_CPP) $(EDITOR_OBJ_C)
	@echo "Linking " $@
	@$(CXX) -o $(EDITOR_BUILD_OBJECT) $^ $(I_FLAGS) $(L_FLAGS) $(LIBRARIES)

# Adds debug symbols for debugger
debug-build-editor: CORE_C_FLAGS +=  -g
debug-build-editor: build-editor

format-editor:
	astyle -n --style=google --recursive editor/src/*.cpp editor/src/*.h

clean-editor:
ifneq ("$(wildcard $(EDITOR_BUILD_OBJECT))","")
	@$(DELETE_CMD) $(EDITOR_BUILD_OBJECT)
endif
ifeq ($(OS_TYPE),windows)
	@$(foreach object, $(EDITOR_OBJ_CPP), $(DELETE_CMD) $(subst /,\,$(object));)
else
	@$(foreach object, $(EDITOR_OBJ_CPP), $(DELETE_CMD) $(object);)
endif

run-editor:
	./$(EDITOR_BUILD_OBJECT)

# Editor Test
editor-test: build-editor-test run-editor-test
editor-test-clean-run: clean-editor-test editor-test

build-editor-test: $(EDITOR_OBJ_CPP_TEST) $(EDITOR_OBJ_C)
	@echo "Linking " $@
	@$(CXX) -o $(EDITOR_BUILD_OBJECT_TEST) $^ $(I_FLAGS) $(L_FLAGS) $(LIBRARIES)

clean-editor-test:
ifneq ("$(wildcard $(EDITOR_BUILD_OBJECT_TEST))","")
	@$(DELETE_CMD) $(EDITOR_BUILD_OBJECT_TEST)
endif
ifeq ($(OS_TYPE),windows)
	@$(foreach object, $(EDITOR_OBJ_CPP_TEST), $(DELETE_CMD) $(subst /,\,$(object));)
else
	@$(foreach object, $(EDITOR_OBJ_CPP_TEST), $(DELETE_CMD) $(object);)
endif

run-editor-test:
	./$(EDITOR_BUILD_OBJECT_TEST) --success
