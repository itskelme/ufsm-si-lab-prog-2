CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

SRCS = tabuleiro.c solver.c main.c
OBJS = $(SRCS:.c=.o)

ifeq ($(OS),Windows_NT)
	TARGET = campo_minado.exe
	# "rm" so existe se houver um shell POSIX (MSYS2, Git Bash, WSL): nesse caso
	# make resolve SHELL para um caminho completo e MSYSTEM vem preenchido.
	# Rodando do cmd.exe puro sobra o literal "sh.exe" e e' preciso usar "del".
	ifeq (,$(findstring /,$(SHELL))$(MSYSTEM))
		LIMPA = del /Q $(subst /,\,$(OBJS) $(TARGET))
	else
		LIMPA = rm -f $(OBJS) $(TARGET)
	endif
else
	TARGET = campo_minado
	LIMPA = rm -f $(OBJS) $(TARGET)
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c tabuleiro.h solver.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(LIMPA)

.PHONY: all clean
