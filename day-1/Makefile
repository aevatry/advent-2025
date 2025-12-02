# from https://spin.atomicobject.com/makefile-c-projects/

TARGET_EXEC ?= main.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
# line below finds all the dependencies
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC = clang
CFLAGS += -Wall -Wextra

# flags to generate .d files next to .o files
CINCLUDEFLAGS ?= $(INC_FLAGS) -MMD -MP

# for general information, the $@ evaluates to the target, so here: $(BUILD_DIR)/$(TARGET_EXEC)
# build the main executable
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $@ $(LDFLAGS)

# for general information, the $< evaluates to the first preriquisite, so here: %.c
# build the c source files
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CINCLUDEFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean run debug test

clean:
	$(RM) -r $(BUILD_DIR)

run: $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC)

debug: clean debug_compile
	@echo "Now in debug mode"
debug_compile: CFLAGS += -g
debug_compile: $(BUILD_DIR)/$(TARGET_EXEC)

# include compile dependencies while supressing errors with the -
-include $(DEPS)

# assign only if not already set
MKDIR_P ?= mkdir -p
