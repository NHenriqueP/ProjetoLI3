# Compilador e flags
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Iinclude -g

PKG_CFLAGS := $(shell { pkg-config --cflags glib-2.0 2>/dev/null || echo "-I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include"; })
PKG_LIBS := $(shell { pkg-config --libs glib-2.0 2>/dev/null || echo "-lglib-2.0"; })
LDFLAGS := $(PKG_LIBS)

# Append pkg-config cflags to CFLAGS
CFLAGS += $(PKG_CFLAGS)

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
BIN_DIR = .
RESULTS_DIR = resultados

# Nome do executável
TARGET = $(BIN_DIR)/programa-principal

# Ficheiros fonte
SRCS = $(wildcard $(SRC_DIR)/*.c)
CORE_SRCS = $(filter-out $(SRC_DIR)/main.c $(SRC_DIR)/programa_testes.c, $(SRCS))

CORE_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(CORE_SRCS))
MAIN_OBJS = $(CORE_OBJS) $(OBJ_DIR)/main.o
TEST_OBJS = $(CORE_OBJS) $(OBJ_DIR)/programa_testes.o

TEST_TARGET = $(BIN_DIR)/programa-testes

# Regra padrão
all: $(TARGET) $(TEST_TARGET)

# Criar o executável
$(TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) $(MAIN_OBJS) -o $(TARGET) $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

# Compilar ficheiros .c em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Criar diretório obj se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpar ficheiros compilados
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

# Limpar apenas os ficheiros gerados na pasta de resultados
clean_outputs:
	rm -rf $(RESULTS_DIR)/*.txt $(RESULTS_DIR)/*.csv

# Limpar e recompilar
rebuild: clean all

# Executar o programa
run: clean_outputs $(TARGET)
	./$(TARGET)

# Regras que não são ficheiros
.PHONY: all clean clean_outputs rebuild run