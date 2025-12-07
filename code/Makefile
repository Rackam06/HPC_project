CC      = gcc
CFLAGS  = -Wall -O2 -Iinclude
LIBS    = -lm

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

SRCS    = $(SRC_DIR)/sequential.c $(SRC_DIR)/bat_utils.c
OBJS    = $(OBJ_DIR)/sequential.o $(OBJ_DIR)/bat_utils.o

TARGET  = sequential

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

# Compile each .c into obj/*.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/bat.h $(INC_DIR)/bat_utils.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

.PHONY: all clean
