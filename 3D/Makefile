# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC
LDFLAGS = -lGL -lGLU -lglut -lm

# Инструменты
AR = ar
ARFLAGS = rcs

# Имена файлов
SRC = EasyGLUT3D.c
HDR = EasyGLUT3D.h
OBJ = EasyGLUT3D.o
STATIC_LIB = libEasyGLUT3D.a
SHARED_LIB = libEasyGLUT3D.so

# Фиктивные цели (не являются файлами)
.PHONY: all static shared clean

# По умолчанию собираем обе библиотеки
all: static shared

# --- Только статическая библиотека ---
static: $(STATIC_LIB)

# --- Только динамическая библиотека ---
shared: $(SHARED_LIB)

# --- Сборка статической библиотеки ---
$(STATIC_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

# --- Сборка динамической библиотеки ---
$(SHARED_LIB): $(OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

# --- Компиляция объектного файла ---
$(OBJ): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Очистка от временных файлов ---
clean:
	rm -f $(OBJ) $(STATIC_LIB) $(SHARED_LIB)
