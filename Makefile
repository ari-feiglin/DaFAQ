PROJECT_NAME = dafaq
CC = gcc
INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
CFLAGS = -I$(INCLUDE_DIR)

DEPS = $(wildcard $(INCLUDE_DIR)/*.h)
__OBJ = $(wildcard $(SRC_DIR)/*/*.c)
_OBJ = $(notdir $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(__OBJ)))
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

#OBJ = $(patsubst %,$(OBJ_DIR)/%,$(notdir $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*/*.c))))

all: $(OBJ_DIR) $(PROJECT_NAME)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o$@

$(PROJECT_NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o$@ -lm

clean:
	rm -r $(OBJ_DIR)/*.o 

.PHONY: all clean