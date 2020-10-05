PROJECT_NAME = dafaq
CC = gcc
INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
CFLAGS = -I$(INCLUDE_DIR)
LIBS = -l:libcolor.so

DEPS = $(wildcard $(INCLUDE_DIR)/*.h)
_OBJ = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(_OBJ))

all: $(OBJ_DIR) $(PROJECT_NAME)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o$@

$(PROJECT_NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o$@ -l:libccolor.so -lm

clean:
	rm -r $(OBJ_DIR)/*.o 

.PHONY: all clean