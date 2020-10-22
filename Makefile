PROJECT_NAME = dafaq
CC = gcc
INCLUDE_DIR = ./include
SRC_DIR = ./src
MENU_DIR = ./interfaces
OBJ_DIR = ./obj
CFLAGS = -I$(INCLUDE_DIR)
LIBS = -l:libcolor.so

DEPS = $(wildcard $(INCLUDE_DIR)/*.h)
__OBJ = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(MENU_DIR)/*.c)
_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(__OBJ)) $(patsubst $(MENU_DIR)/%.c,$(OBJ_DIR)/%.o,$(__OBJ))
OBJ = $(filter %.o, $(_OBJ))

all: $(OBJ_DIR) $(PROJECT_NAME)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(MENU_DIR)/%.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o$@
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o$@

$(PROJECT_NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o$@ -l:libccolor.so -lm

clean:
	rm -r $(OBJ_DIR)/*.o 

.PHONY: all clean