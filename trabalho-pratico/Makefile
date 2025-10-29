# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS = 

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
BIN_DIR = .

# Nome do executável
TARGET = $(BIN_DIR)/programa

# Ficheiros fonte
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Ficheiros objeto (substituir src/ por obj/ e .c por .o)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Regra padrão
all: $(TARGET)

# Criar o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilar ficheiros .c em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Criar diretório obj se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpar ficheiros compilados
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Limpar e recompilar
rebuild: clean all

# Executar o programa
run: $(TARGET)
	./$(TARGET)

# Regras que não são ficheiros
.PHONY: all clean rebuild run