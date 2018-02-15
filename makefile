EXEC_NAME = owl

# Build type : 'release' or 'debug'
BUILD_TYPE = release

PLATFORM = linux

SRC_ROOT = $(abspath .)
TGT_ROOT ?= $(SRC_ROOT)/build

ifeq ($(PLATFORM), win64)
	EXEC ?= $(TGT_ROOT)/$(EXEC_NAME).exe
else
	EXEC ?= $(TGT_ROOT)/$(EXEC_NAME)
endif
TGT_DIRS += $(dir $(EXEC))

DATA_DIR = $(SRC_ROOT)/data

# Libraries
# ---------

# GLFW
#INC_DIRS += glfw
#ifeq ($(PLATFORM), win64)
#	LINK_LIBS += glfw3dll
#else
#	LINK_LIBS += glfw
#endif

# C++ Flags
# ---------

INC_DIRS += $(SRC_ROOT)/include
CPP_FLAGS += $(addprefix -I, $(INC_DIRS))

CPP_FLAGS += -std=c++17 -Wall -Wextra
ifeq ($(BUILD_TYPE), release)
	CPP_FLAGS += -O3
else ifeq ($(BUILD_TYPE), debug)
	CPP_FLAGS += -g -fsanitize=address
	CPP_FLAGS += -DDEBUG_BUILD
endif

# Link Flags
# ----------

LINK_FLAGS += -pedantic
LINK_FLAGS += $(addprefix -l, $(LINK_LIBS))
LINK_FLAGS += -lpthread -lm

# Tools
# -----

ifeq ($(PLATFORM), win64)
	TOOL_PREFIX ?= x86_64-w64-mingw32-
else
	TOOL_PREFIX ?=
endif

AS  = $(TOOL_PREFIX)gcc
CC  = $(TOOL_PREFIX)gcc
CPP = $(TOOL_PREFIX)g++
LD  = $(TOOL_PREFIX)g++
AR  = $(TOOL_PREFIX)ar

TOOL_DIR ?= $(abspath $(dir $(shell which $(CC))))

# Source files
# ------------

SRC_RFILES += $(shell ls $(SRC_ROOT)/src/*.{s,S,c,cpp} 2> /dev/null)
SRC_RFILES += $(shell ls $(SRC_ROOT)/src/util/*.{s,S,c,cpp} 2> /dev/null)
SRC_RFILES += $(shell ls $(SRC_ROOT)/src/core/*.{s,S,c,cpp} 2> /dev/null)
SRC_RFILES += $(shell ls $(SRC_ROOT)/src/engine/*.{s,S,c,cpp} 2> /dev/null)
SRC_RFILES += $(shell ls $(SRC_ROOT)/src/gfx/*.{s,S,c,cpp} 2> /dev/null)
SRC_RFILES += $(shell ls $(SRC_ROOT)/src/ui/*.{s,S,c,cpp} 2> /dev/null)

SRC_FILES = $(abspath $(SRC_RFILES))

OBJ_FILES += $(subst $(SRC_ROOT), $(TGT_ROOT), $(addsuffix .o, $(SRC_FILES)))
DEP_FILES += $(subst $(SRC_ROOT), $(TGT_ROOT), $(addsuffix .d, $(SRC_FILES)))

TGT_DIRS += $(dir $(OBJ_FILES) $(DEP_FILES))

# Rules
# -----

.PHONY : all clean

all : tree $(EXEC)

rebuild : clean all

run : all
	@echo "[`date "+%H:%M:%S"`] Running '$(EXEC)'..."
	@$(EXEC)

wine : all
	@echo "[`date "+%H:%M:%S"`] Running '$(EXEC)' with WINE..."
	@WINEPREFIX=~/.wine64 wine $(EXEC)

tree :
	@mkdir -p $(TGT_DIRS)

clean :
	@rm -r -f $(EXEC) $(OBJ_FILES) $(DEP_FILES) $(TGT_DIRS)

$(EXEC) : $(OBJ_FILES)
	@#echo "[`date "+%H:%M:%S"`] Linking '$@'..."
	@$(TOOL_DIR)/$(LD) $(CPP_FLAGS) -o $@ $(OBJ_FILES) $(LINK_FLAGS)
	@echo "[`date "+%H:%M:%S"`] Linked '$@'."

-include $(DEP_FILES)

# Compile .s files
$(TGT_ROOT)/%.s.o : $(SRC_ROOT)/%.s
	@#echo "[`date "+%H:%M:%S"`] Compiling '$<'..."
	@$(TOOL_DIR)/$(AS) -c -o $@ $< $(CPP_FLAGS)
	@echo "[`date "+%H:%M:%S"`] Compiled '$@'."

# Compile .S files
$(TGT_ROOT)/%.S.o : $(SRC_ROOT)/%.S
	@#echo "[`date "+%H:%M:%S"`] Compiling '$<'..."
	@$(TOOL_DIR)/$(AS) -c -o $@ $< $(CPP_FLAGS)
	@echo "[`date "+%H:%M:%S"`] Compiled '$@'."

# Compile .c files
$(TGT_ROOT)/%.c.o : $(SRC_ROOT)/%.c
	@#echo "[`date "+%H:%M:%S"`] Compiling '$<'..."
	@$(TOOL_DIR)/$(CC) -MMD -c -o $@ $< $(CPP_FLAGS)
	@echo "[`date "+%H:%M:%S"`] Compiled '$@'."

# Compile .cpp files
$(TGT_ROOT)/%.cpp.o : $(SRC_ROOT)/%.cpp
	@#echo "[`date "+%H:%M:%S"`] Compiling '$<'..."
	@$(TOOL_DIR)/$(CPP) -MMD -c -o $@ $< $(CPP_FLAGS)
	@echo "[`date "+%H:%M:%S"`] Compiled '$@'."
