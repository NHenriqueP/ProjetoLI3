# Compilador e flags
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Iinclude -g

# Try to get flags from pkg-config; if not available, fall back to common glib include/lib flags.
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

# Nome do executável
TARGET = $(BIN_DIR)/programa-principal

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

# Limpar apenas os ficheiros gerados na pasta de resultados
clean_outputs:
	@echo "A limpar ficheiros na pasta resultados/ (mantendo .gitkeep)"
	@mkdir -p resultados
	@find resultados -type f ! -name '.gitkeep' -delete

# Limpar e recompilar
rebuild: clean all

# Executar o programa
run: $(TARGET)
	./$(TARGET)

# Regras que não são ficheiros
.PHONY: all clean clean_outputs rebuild run